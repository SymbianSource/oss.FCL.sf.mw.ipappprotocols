// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : sipimsprofileagent.cpp
// Part of     : sip ims plugin
// implementation
// Version     : 1.0
//



// INCLUDE FILES
#include <commsdat.h>
#include <commsdattypesv1_1.h>
#include <metadatabase.h>
#include <cdblen.h>
#include "sipimsprofileagent.h"
#include "sipconcreteprofile.h"
#include "sipprofileagentobserver.h"
#include "sipconnection.h"
#include "sipprflinitstate.h"
#include "sipprflresolvingproxiesstate.h"
#include "sipprflregisterrequestedstate.h"
#include "sipprflregisterdelayrequestedstate.h"
#include "sipprflregistrationinprogressstate.h"
#include "sipprflregisteredstate.h"
#include "sipprflderegistrationinprogressstate.h"
#include "sipprflderegisterrequestedstate.h"
#include "sipprflstatebase.h"
#include "sipimsprofilecontext.h"
#include "sipmanagedprofile.h"
#include "sipgendefs.h"
#include "SipProfileLog.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "CSIPNotifyXmlBodyParser.h"
#include "Sipimsprofileusimprovider.h"
#include "Sipimsprofilesimrecord.h"
#include "sipprofilequeuehandling.h"
#include "sip.h"
#include "siphttpdigest.h"
#include "sipservertransaction.h"

_LIT8(KSIPIMSProfileType, "ims");
const TInt KAuthorizedMicroSecsInSec = 1;
const TUint32 KRegAllowedWithUSIMandISIM = 1;
const TUint32 KRegAllowedWithISIM = 2;

#ifdef CPPUNIT_TEST
#define CPPUNIT_MEMORYLEAVE {	if (err == KErrNoMemory) User::Leave(KErrNoMemory);}
#else
#define CPPUNIT_MEMORYLEAVE
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPIMSProfileAgent* CSIPIMSProfileAgent::NewL(TAny* aInitParams)
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::NewL")
	
	CSIPIMSProfileAgent* self = new (ELeave) CSIPIMSProfileAgent(
		static_cast<TSIPProfileAgentInitParams*>(aInitParams));
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::CSIPIMSProfileAgent
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPIMSProfileAgent::CSIPIMSProfileAgent(
    TSIPProfileAgentInitParams* aInitParams)
: iSIPProfileAgentObserver(aInitParams->iSIPProfileAgentObserver),
  iDeltaTimer(aInitParams->iDeltaTimer),
  iDeltaTimerCallBack(ASyncTimerExpired,this)
	{
	iType.iSIPProfileClass = TSIPProfileTypeInfo::EIms;
	iType.iSIPProfileName = KSIPIMSProfileType();
	iDeltaTimerEntry.Set(iDeltaTimerCallBack);
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::ConstructL()
    {
    PROFILE_DEBUG1("CSIPIMSProfileAgent::ConstructL")
    
    SIPStrings::OpenL();
    
	iSIP = CSIP::NewL(TUid::Null(),*this);
	iHttpDigest = CSIPHttpDigest::NewL(*iSIP,*this);    
    
    TBuf<KCommsDbSvrMaxFieldLength> tsyModuleName;
	GetTsyModuleNameL( tsyModuleName );
    User::LeaveIfError(iTelServer.Connect());    
    User::LeaveIfError(iTelServer.LoadPhoneModule(tsyModuleName));
     
    TInt numPhone(0);
    User::LeaveIfError(iTelServer.EnumeratePhones(numPhone));
    TName tsyName;
    TBool found = EFalse;
    RTelServer::TPhoneInfo phoneInfo;
    TInt i(0);
    while ( !found && i < numPhone )
        {
        User::LeaveIfError( iTelServer.GetPhoneInfo( i, phoneInfo ) );
        User::LeaveIfError( iTelServer.GetTsyName( i, tsyName ) );
        if ( tsyName.CompareF( tsyModuleName ) == 0 )
            {
            found = ETrue;
            }
        i++;
        }
    __ASSERT_ALWAYS(found, User::Leave(KErrNotFound));     
    
    PROFILE_DEBUG1("CSIPIMSProfileAgent::ConstructL, etel ok")     
    
    iXMLParser = CSIPNotifyXmlBodyParser::NewL();
	iInit = CSIPPrflInitState::NewL(*this);
	iResolvingProxies = CSIPPrflResolvingProxiesState::NewL(*this);
	iRegRequested = CSIPPrflRegisterRequestedState::NewL(*this);
	iRegInProgress = CSIPPrflRegistrationInProgressState::NewL(*this);
	iRegistered = CSIPPrflRegisteredState::NewL(*this);
	iDeregRequested = CSIPPrflDeregisterRequestedState::NewL(*this);
	iDeregInProgress = CSIPPrflDeregistrationInProgressState::NewL(*this);
	iRegDelayRequested = CSIPPrflRegisterDelayRequestedState::NewL(*this);
	iInit->LinkStates(*iResolvingProxies, *iRegRequested, *iRegInProgress);
	iResolvingProxies->LinkStates(*iInit, *iRegRequested, *iRegInProgress);
	iRegRequested->LinkStates(*iInit, *iResolvingProxies, *iRegInProgress);
	iRegInProgress->LinkStates(*iRegRequested, *iInit, *iRegistered,
	                           *iRegDelayRequested);
	iRegistered->LinkStates(*iDeregRequested, *iDeregInProgress,
							 *iInit, *iRegRequested,*iRegDelayRequested);
	iDeregRequested->LinkStates(*iInit, *iRegistered, *iDeregInProgress);
	iDeregInProgress->LinkStates(*iInit);
	iRegDelayRequested->LinkStates(*iInit);
	
	PROFILE_DEBUG1("CSIPIMSProfileAgent::ConstructL, states ok")   
	
	iSIMRecord = CSIPProfileSIMRecord::NewL();	
	iProfileQueueHandling = CSIPProfileQueueHandling::NewL(*this);
	TRAPD(err, iUsimProvider = 
	    CSIPProfileUsimProvider::NewL(
	        *iSIMRecord, iTelServer, phoneInfo, *this));
	if(err == KErrNoMemory)
		{
		User::Leave(err);
		}
	
	PROFILE_DEBUG1("CSIPIMSProfileAgent::ConstructL, sim ok") 
	iConfigExtension = NULL;
	PROFILE_DEBUG1("CSIPIMSProfileAgent::ConstructL, exit")
    }

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::~CSIPIMSProfileAgent
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
CSIPIMSProfileAgent::~CSIPIMSProfileAgent()
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::~CSIPIMSProfileAgent")
	
	delete iInit;
	delete iResolvingProxies;
	delete iRegRequested;
	delete iRegistered;
	delete iRegInProgress;
	delete iDeregRequested;
	delete iDeregInProgress;
	delete iRegDelayRequested;
	delete iProfileQueueHandling;
	iDeltaTimer.Remove(iDeltaTimerEntry);
	iConnectionCtxArray.ResetAndDestroy();
	iConnectionCtxArray.Close();
	iWaitForRegisteringArray.Reset();
	iSIMWaitForAuthorizedArray.Reset();
	iUSimWaitForAuthorizedArray.Reset();
	iWaitForAuthorizedArray.Reset();
	iFailedProfilesArray.Reset();
	delete iXMLParser;
	delete iUsimProvider;
	delete iSIMRecord;
	iTelServer.Close();
	delete iHttpDigest;
	delete iSIP;
	SIPStrings::Close();	
	PROFILE_DEBUG1("CSIPIMSProfileAgent::~CSIPIMSProfileAgent, exit")
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::RegAllowedWithUSIMorISIML
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIMSProfileAgent::RegAllowedWithUSIMorISIML(CSIPConcreteProfile& aProfile)
	{
	TBool allowed = ETrue;
	TUint32 confValue = 0;
	TInt result = 0;
	result = aProfile.ExtensionParameter( KSIPAllowIMSRegistration,confValue );
	if ( result != KErrNotFound )
		{
		if (confValue == KRegAllowedWithISIM && !iSIMRecord->IsISIMPresent())
			{
			allowed = EFalse;
			}		
		}
	return allowed;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::RegAllowedWithSIM
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIMSProfileAgent::RegAllowedWithSIM(CSIPConcreteProfile& aProfile)
	{
	TBool allowed = ETrue;
	TUint32 confValue = 0;
	TInt result = 0;
	result = aProfile.ExtensionParameter( KSIPAllowIMSRegistration,confValue );
	if (result != KErrNotFound)
		{
		if (confValue == KRegAllowedWithISIM ||
				 confValue == KRegAllowedWithUSIMandISIM)
			{
			allowed = EFalse;
			}		
		}
	return allowed;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::CheckIPSecRulesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::CheckIPSecRulesL(CSIPConcreteProfile& aProfile)
	{
	TBool found = EFalse;
	TBool isRegInUse = EFalse;
		
	found = IsIPSecAlreadyInUse(isRegInUse);	
	if (found)
		{
		ResetISIMUpdateProfile( &aProfile );
		User::Leave(KErrArgument);
		}
		
	if (isRegInUse)
		{
		iWaitForRegisteringArray.AppendL(&aProfile);
		}
	else 
		{	
		ProfileToWaitingQueueL( &aProfile );
		}
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::ResetISIMUpdateProfile
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::ResetISIMUpdateProfile( CSIPConcreteProfile* aSIPConcreteProfile )
	{
	if ( iISIMUpdateProfile && (iISIMUpdateProfile->Id() == aSIPConcreteProfile->Id()) )
		{
		iISIMUpdateProfile = 0;
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::IsIPSecAlreadyInUse
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIMSProfileAgent::IsIPSecAlreadyInUse( TBool& aIsRegInProgress )
	{
	TBool found = EFalse;
	
	for (TInt i=0; i < iConnectionCtxArray.Count() && !found; i++)
		{
		found = iConnectionCtxArray[i]->IsIPSecAlreadyInUse( aIsRegInProgress );	
		}
	if ( !found && !aIsRegInProgress )
		{
		aIsRegInProgress = iProfileQueueHandling->FoundIPSecProfileCandidate();
		}
	return found;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::Type
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TSIPProfileTypeInfo& CSIPIMSProfileAgent::Type() const
	{
	return iType;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::CreateL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile* CSIPIMSProfileAgent::CreateL()
	{
	TSIPProfileTypeInfo type;		
	type.iSIPProfileClass = TSIPProfileTypeInfo::EIms;
	type.iSIPProfileName = KSIPIMSProfileType;

	CSIPConcreteProfile* profile = CSIPConcreteProfile::NewLC();
	profile->SetProfileType(type);

	profile->EnableSecurityNegotiation(ETrue);
	profile->EnableSigComp(ETrue);
	_LIT8(KSIPDefaultProxy, "sip:0.0.0.0");
	profile->SetServerL(KSIPOutboundProxy, KSIPDefaultProxy);

	CleanupStack::Pop(profile);
	return profile;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::RegisterL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::RegisterL(
	CSIPConcreteProfile& aSIPConcreteProfile)
	{
	PROFILE_DEBUG3("CSIPIMSProfileAgent::RegisterL", aSIPConcreteProfile.Id())
	__ASSERT_ALWAYS (aSIPConcreteProfile.Server(KSIPOutboundProxy).Length(),
		User::Leave(KErrArgument));
	
	TBool isClientConfigured = EFalse;
	
	if (CheckIfConfigured(aSIPConcreteProfile))
		{
		if (!MandatoryValuesConfigured(aSIPConcreteProfile))
			{
			User::Leave(KErrArgument);
			}
		isClientConfigured = ETrue;
		}
	
	if (!iUsimProvider && !isClientConfigured)
		{//No USIM/SIM access available
		User::Leave(KErrArgument);
		}
	
	if (!isClientConfigured)
		{
		TUint32 confValue = 0;
		TInt result = 0;
		result = aSIPConcreteProfile.ExtensionParameter( KSIPAllowIMSRegistration,confValue );
		if (result != KErrNotFound)
			{
			if ((confValue == KRegAllowedWithUSIMandISIM || 
			    confValue == KRegAllowedWithISIM )
			    && !iUsimProvider->IsUsimSupported())
				{
				User::Leave(KErrNotSupported);
				}
			}
		}
	
	// If digest settings are configured, no need to do USIM/SIM authorization	
	if (iUsimProvider && 
	    !CSIPIMSProfileContext::IsHttpDigestSettingsConfigured(aSIPConcreteProfile))
	    {
        TBool requestFailed = EFalse;
	   	if (!iUsimProvider->IsUsimSupported())
    		{
    		iSIMWaitForAuthorizedArray.AppendL(&aSIPConcreteProfile);
    		requestFailed = iUsimProvider->RequestL(EFalse);
    		if(requestFailed)
    		    {
                AuthorizationSIMFailed();
    		    }
    		}
	    else
			{
			iUSimWaitForAuthorizedArray.AppendL(&aSIPConcreteProfile);
			requestFailed = iUsimProvider->RequestL(ETrue);
			if(requestFailed)
			    {
                AuthorizationUSIMFailed();
			    }
			}
	    }
    else
		{
		iWaitForAuthorizedArray.AppendL(&aSIPConcreteProfile);
		AsyncDeltaTimer();
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::UpdateL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::UpdateL(
	CSIPConcreteProfile& aNewProfile, 
	CSIPConcreteProfile& aOldProfile)
	{
	PROFILE_DEBUG3("CSIPIMSProfileAgent::UpdateL", aOldProfile.Id())
	iProfileQueueHandling->Cleanup(aOldProfile,&aNewProfile);
	CSIPIMSProfileContext* context = FindProfileContext(aOldProfile);
	if (!context)
	    {
	    User::Leave(KErrNotFound);
	    }
	context->UpdateL(aNewProfile);
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::DeregisterL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::DeregisterL(
	CSIPConcreteProfile& aSIPConcreteProfile)
	{
	PROFILE_DEBUG3("CSIPIMSProfileAgent::DeregisterL", aSIPConcreteProfile.Id())
	CSIPIMSProfileContext* context = FindProfileContext(aSIPConcreteProfile);
	if ( context )
		{
		if (!DeregisterToWaitingQueueL( context ))
			{
			context->DeregisterL();
			}
		}
	else
		{
		PROFILE_DEBUG1("CSIPIMSProfileAgent::DeregisterL RemoveProfileFromQueue")
		if (! iProfileQueueHandling->RemoveProfileFromRegQueueL(aSIPConcreteProfile) )
			{
			if (!RemoveProfileFromWaitingQueue(iWaitForRegisteringArray,
												aSIPConcreteProfile) )
				{
				if (!RemoveProfileFromWaitingQueue(iUSimWaitForAuthorizedArray,
												    aSIPConcreteProfile))
					{
					if (!RemoveProfileFromWaitingQueue(iSIMWaitForAuthorizedArray,
														aSIPConcreteProfile))
						{
						RemoveProfileFromWaitingQueue(iWaitForAuthorizedArray,
															aSIPConcreteProfile);
						}
					}
				}
			}
		}	   
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::GetProfileState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSIPIMSProfileAgent::GetProfileState(
	CSIPConcreteProfile::TStatus& aState, 
	CSIPConcreteProfile& aProfile) const
	{
	CSIPIMSProfileContext* context = FindProfileContext(aProfile);
	if (context)
		{
		aState = context->CurrentMappedState();
		return KErrNone;
		}
	else
		{
		return KErrNotFound;
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::IsIdle
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIMSProfileAgent::IsIdle()
	{
	CleanIdleConnectionContexts();
	return (iConnectionCtxArray.Count()==0 &&
	        iProfileQueueHandling->IsEmpty() &&
	        iWaitForRegisteringArray.Count()==0 &&
	        iUSimWaitForAuthorizedArray.Count()==0 &&
	        iSIMWaitForAuthorizedArray.Count()==0 &&
	        iWaitForAuthorizedArray.Count()==0 &&
	        !iSIMRecord->IsISIMPresent() );
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::RegisterPending
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
TBool CSIPIMSProfileAgent::RegisterPending(
    CSIPConcreteProfile& aSIPProfile) const
    {
	CSIPIMSProfileContext* context = FindProfileContext(aSIPProfile);
	if (context)
		{
		return context->IsRegisterPending();
		}
	return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::TerminateHandling
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSIPIMSProfileAgent::TerminateHandling(CSIPConcreteProfile& aProfile)
	{
	CSIPIMSProfileContext* context = FindProfileContext(aProfile);
	if (context)
		{
		PROFILE_DEBUG3("CSIPIMSProfileAgent::TerminateHandling", aProfile.Id())
		context->SetNextState(*iInit);
		context->TerminateHandling();
		return KErrNone;
		}
	else
		{
		return KErrNotFound;
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::Extension
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSIPIMSProfileAgent::Extension(TInt aOperationCode, TAny* aParams)
    {
    TInt err(KErrNone);
    if(aParams && aOperationCode == KSipProfileAgentConfigExtension)
       	iConfigExtension = static_cast<CSipProfileAgentConfigExtension*>(aParams);
    else
      	err = KErrNotSupported;
    return err;
    }	

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::RegisterProfileL
// From MSIPProfileFSMUser
// -----------------------------------------------------------------------------
//	
void CSIPIMSProfileAgent::RegisterProfileL(
    CSIPConcreteProfile& aSIPProfile)
    {
	PROFILE_DEBUG3("CSIPIMSProfileAgent::RegisterProfileL", aSIPProfile.Id())    
    
    RegisterFromAuthorizedArrayL(aSIPProfile);
    }

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::DeregisterProfileL
// From MSIPProfileFSMUser
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::DeregisterProfileL(
    CSIPConcreteProfile& aSIPProfile)
    {
    PROFILE_DEBUG3("CSIPIMSProfileAgent::DeregisterProfileL", aSIPProfile.Id())
    
    DeregisterL(aSIPProfile);
    }

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::RetryProfileRegistrationL
// From MSIPProfileFSMUser
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::RetryProfileRegistrationL( 
    CSIPConcreteProfile& aSIPProfile)
    {
	PROFILE_DEBUG3("CSIPIMSProfileAgent::RetryProfileRegistrationL", 
	               aSIPProfile.Id())

	if (!iProfileQueueHandling->AddRegisterToQueueL(aSIPProfile, ETrue))
		{
        CSIPIMSProfileContext* context = FindProfileContext(aSIPProfile);
        if (!context)
	        {
	        User::Leave(KErrNotFound);
	        }		
        context->RetryRegistration();
		}     
    }

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::AddProfileIntoQueue
// From MSIPProfileFSMUser
// -----------------------------------------------------------------------------
//
TBool CSIPIMSProfileAgent::AddProfileIntoQueue(
    CSIPConcreteProfile& aSIPProfile) const
    {
	PROFILE_DEBUG3("CSIPIMSProfileAgent::AddProfileIntoQueue", 
	               aSIPProfile.Id())
	                   
	TBool found = EFalse;
		    
	for (TInt i=0; i < iConnectionCtxArray.Count() && !found; i++)
		{
		found = iConnectionCtxArray[i]->AddIntoQueue( 
		    aSIPProfile.Server(KSIPRegistrar));
		}
	return found;
    }

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::RegisterProfileAfterQueueL
// From MSIPProfileFSMUser
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::RegisterProfileAfterQueueL(
    CSIPConcreteProfile& aSIPProfile)
    {
	PROFILE_DEBUG3("CSIPIMSProfileAgent::RegisterProfileAfterQueueL", 
	               aSIPProfile.Id())    
    
    RegisterFromAuthorizedArrayL(aSIPProfile);   
    }

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::DeregisterProfileAfterQueueL
// From MSIPProfileFSMUser
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::DeregisterProfileAfterQueueL(
    CSIPConcreteProfile& aSIPProfile)
    {
	PROFILE_DEBUG3("CSIPIMSProfileAgent::DeregisterProfileAfterQueueL", 
	               aSIPProfile.Id())    
    
	CSIPIMSProfileContext* context = FindProfileContext(aSIPProfile);
	__ASSERT_ALWAYS(context, User::Leave(KErrNotFound));
    context->DeregisterL();
    
    }

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::RetryProfileRegistrationAfterQueueL
// From MSIPProfileFSMUser
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::RetryProfileRegistrationAfterQueueL(
    CSIPConcreteProfile& aSIPProfile)
    {
	PROFILE_DEBUG3("CSIPIMSProfileAgent::RetryRegisterAfterQueueL", 
	               aSIPProfile.Id())    
    
    CSIPIMSProfileContext* context = FindProfileContext(aSIPProfile);
    __ASSERT_ALWAYS(context, User::Leave(KErrNotFound));
    
    context->RetryRegistration();  
    }	
 
// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::IsInQueue
// From MSIPProfileFSMUser
// -----------------------------------------------------------------------------
//
TBool CSIPIMSProfileAgent::IsInQueue(
    CSIPConcreteProfile& aSIPProfile) const
    {
	PROFILE_DEBUG3("CSIPIMSProfileAgent::IsInQueue", 
	               aSIPProfile.Id())    
    return iProfileQueueHandling->IsInQueue(aSIPProfile);
    } 
 
// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::AllowedTakeFromQueue
// -----------------------------------------------------------------------------
//	
TBool CSIPIMSProfileAgent::AllowedTakeFromQueue( 
    CSIPConcreteProfile& aSIPConcreteProfile )
    {
    PROFILE_DEBUG1("CSIPIMSProfileAgent::AllowedTakeFromQueue")
	TBool found = EFalse;   
	for (TInt i=0; i < iConnectionCtxArray.Count() && !found; i++)
		{
		PROFILE_DEBUG1("CSIPIMSProfileAgent::AllowedTakeFromQueue next AddIntoQueue")
		found = iConnectionCtxArray[i]->AddIntoQueue( 
		    aSIPConcreteProfile.Server(KSIPRegistrar) ) ;
		}
	return !found;
    }
 
// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::CheckIfConfigured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIMSProfileAgent::CheckIfConfigured(CSIPConcreteProfile& aSIPProfile)
	{
	TBool result = EFalse;
	if (aSIPProfile.AOR().Length())
		{
		return ETrue;
		}
	if (aSIPProfile.PrivateIdentity().Length() || 
			aSIPProfile.ServerParameter(KSIPRegistrar, KSIPDigestUserName).Length())
		{
		return ETrue;
		}
	if (aSIPProfile.ServerParameter(KSIPRegistrar, KSIPDigestRealm).Length())
		{
		return ETrue;
		}
	if (aSIPProfile.ServerParameter(KSIPRegistrar, KSIPDigestPassword).Length())
		{
		return ETrue;
		}

	if (aSIPProfile.Server(KSIPRegistrar).Length())
		{
		return ETrue;
		}

	if (aSIPProfile.ServerParameter(KSIPOutboundProxy, KSIPDigestUserName).Length())
		{
		return ETrue;
		}
	if (aSIPProfile.ServerParameter(KSIPOutboundProxy, KSIPDigestRealm).Length())
		{
		return ETrue;
		}
	if (aSIPProfile.ServerParameter(KSIPOutboundProxy, KSIPDigestPassword).Length())
		{
		return ETrue;
		}	
	return result;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::MandatoryValuesConfigured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIMSProfileAgent::MandatoryValuesConfigured(
    CSIPConcreteProfile& aSIPProfile)
	{
	TBool result = ETrue;
	if(!aSIPProfile.IapId())
		{
		return EFalse;
		}
	if (!aSIPProfile.AOR().Length())
		{
		return EFalse;
		}	
	if (!aSIPProfile.PrivateIdentity().Length() && 
		!aSIPProfile.ServerParameter(KSIPRegistrar, KSIPDigestUserName).Length())
		{
		return EFalse;
		}	
	if (!aSIPProfile.ServerParameter(KSIPRegistrar, KSIPDigestPassword).Length())
		{
		return EFalse;
		}
	if (!aSIPProfile.Server(KSIPRegistrar).Length())
		{
		return EFalse;
		}
	return result;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::SIPProfileStatusEvent
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::SIPProfileStatusEvent(
	CSIPConcreteProfile& aProfile,
	TUint32 aContextId)
	{
	TUint32 profileId = aProfile.Id();
	PROFILE_DEBUG4( "CSIPIMSProfileAgent::SIPProfileStatusEvent",\
		profileId, aContextId )
	TBool reportStatusToUser = ETrue;
	CSIPIMSProfileContext* context = FindProfileContext( aProfile.Id() );
	TRAPD( err, reportStatusToUser = 
	    HandleISIMUpdateRegistrationEventL( aProfile, context ) );
	if ( err == KErrNone && reportStatusToUser )
		{
		RegisterQueue( aProfile, ETrue );
		HandleWaitingForRegisteringArray();
		iSIPProfileAgentObserver.SIPProfileStatusEvent( aProfile, aContextId );
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::HandleISIMUpdateRegistrationEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIMSProfileAgent::HandleISIMUpdateRegistrationEventL(
    CSIPConcreteProfile& aProfile, 
	CSIPIMSProfileContext* aContext)
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::HandleISIMUpdateRegistrationEvent")
	TBool notIsimUpdateCase = ETrue;
	if ( iISIMUpdateProfile && iISIMUpdateProfile->Id() == aProfile.Id() )
		{
		PROFILE_DEBUG1("CSIPIMSProfileAgent::HandleISIMUpdateRegistrationEvent ISIMUpdateProfile")
		TUint32 contextId = 0;
		if ( aContext )
			{
			PROFILE_DEBUG1("CSIPIMSProfileAgent::HandleISIMUpdateRegistrationEvent aContext")
			if ( aContext->CurrentState() == MSIPProfileContext::ERegistered )
				{
				PROFILE_DEBUG1("CSIPIMSProfileAgent::HandleISIMUpdateRegistrationEvent ERegistered")
				contextId = aContext->Registration()->ContextId();
				DeregisterL( *iISIMUpdateProfile );
				notIsimUpdateCase = EFalse;
				iSIPProfileAgentObserver.SIPProfileStatusEvent( aProfile, contextId );
				}
			}
		else
			{
			PROFILE_DEBUG1("CSIPIMSProfileAgent::HandleISIMUpdateRegistrationEvent no aContext")
			FindProfileContextAndRegisterL( *iISIMUpdateProfile );
            notIsimUpdateCase = EFalse;
			}
		}
	if ( iInterimProfile && 
	     iInterimProfile->Id() == aProfile.Id() && 
	     !aContext )
		{
		FindProfileContextAndRegisterL( *iInterimProfile );
        notIsimUpdateCase = EFalse;
		}
	PROFILE_DEBUG1("CSIPIMSProfileAgent::HandleISIMUpdateRegistrationEvent end")	
	return notIsimUpdateCase;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::HandleWaitingForRegisteringArray
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::HandleWaitingForRegisteringArray()
	{
	if ( iWaitForRegisteringArray.Count() )
		{
		TBool isRegInProgress = EFalse;

		if ( IsIPSecAlreadyInUse( isRegInProgress ) )
			{
			//delete all in the que, one by one
			DeleteAllProfilesInWaitingQue();
			}
		else
			{
			if ( !isRegInProgress )
				{
				//take the first in the que and register
				TRAP_IGNORE( RegisterFromWaitingQueL() )
				}
			}		
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::DeleteAllProfilesInWaitingQue
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::DeleteAllProfilesInWaitingQue()
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::DeleteAllProfilesInWaitingQue")
	
	CSIPConcreteProfile* profile = NULL;
	for (TInt i = iWaitForRegisteringArray.Count() -1; i >= 0; --i)
		{
		profile = iWaitForRegisteringArray[i];
		if (profile)
			{
			TerminateAndErrorEvent(*profile, KErrArgument);
			iWaitForRegisteringArray.Remove(i);
			}
		}
	iWaitForRegisteringArray.Compress();
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::RegisterFromWaitingQueL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::RegisterFromWaitingQueL()
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::RegisterFromWaitingQue")
	CSIPConcreteProfile* profile = 
        RemoveFirstProfileFromArray( iWaitForRegisteringArray );
	if ( profile )
		{
		ProfileToWaitingQueueL( profile );
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::RemoveFirstProfileFromArray
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
CSIPConcreteProfile* CSIPIMSProfileAgent::RemoveFirstProfileFromArray( 
							RPointerArray<CSIPConcreteProfile>& aProfileArray )
	{
	CSIPConcreteProfile* profile = NULL;
	if (aProfileArray.Count() > 0)
		{
		profile = aProfileArray[0];
		aProfileArray.Remove(0);
		aProfileArray.Compress();
		}
	return profile;
	}	
	
// ----------------------------------------------------------------------------
// CSIPIMSProfileAgent::TerminateAndErrorEvent
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
void CSIPIMSProfileAgent::TerminateAndErrorEvent( CSIPConcreteProfile& aProfile,
												  TInt aError )
	{
	TerminateHandling(aProfile);
	iSIPProfileAgentObserver.SIPProfileErrorEvent(aProfile,aError);	
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::FoundIPSecProfileCandidate
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
TBool CSIPIMSProfileAgent::FoundIPSecProfileCandidate()
	{
	TBool result = EFalse;
	if ( iUsimProvider && iUsimProvider->IsUsimSupported() )
		{
		result = iProfileQueueHandling->FoundIPSecProfileCandidate();
		}
	return result;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::ChooseProfileTypeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
void CSIPIMSProfileAgent::ChooseProfileTypeL( CSIPConcreteProfile* aProfile,
									 CSIPIMSProfileContext* context )
	{
	if (CheckIfConfigured(*aProfile))	
		{
		context->SetConfiguredType(CSIPIMSProfileContext::EClientConfiguredType);
		}
	else
		{
		if (iUsimProvider && !iUsimProvider->IsUsimSupported())	
			{
			context->SetConfiguredType(CSIPIMSProfileContext::EEarlyIMSType);
			}
		if (iUsimProvider && iUsimProvider->IsUsimSupported())
			{
			context->SetConfiguredType(CSIPIMSProfileContext::EIMSReleaseType);
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::RegisterFromAuthorizedArrayL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
void CSIPIMSProfileAgent::RegisterFromAuthorizedArrayL(
    CSIPConcreteProfile& aProfile)
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::RegisterFromAuthorizedArrayL")
	FindProfileContextAndRegisterL( aProfile );
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::ProfileToWaitingQueueL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
void CSIPIMSProfileAgent::ProfileToWaitingQueueL( 
    CSIPConcreteProfile* aProfile )
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::ProfileToWaitingQueueL")
	if ( !iProfileQueueHandling->AddRegisterToQueueL( *aProfile, EFalse ) )
		{
		RegisterFromAuthorizedArrayL( *aProfile );
		}
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::DeregisterToWaitingQueueL
// -----------------------------------------------------------------------------
//	
TBool CSIPIMSProfileAgent::DeregisterToWaitingQueueL( 
    CSIPIMSProfileContext* aContext )
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::DeregisterToWaitingQueueL")
	return iProfileQueueHandling->AddDeregisterToQueueL( *aContext->Profile() );
	}	

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::SetInterimProfile
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::SetInterimProfile( CSIPConcreteProfile* aSIPConcreteProfile )
	{
	iInterimProfile = aSIPConcreteProfile;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::SIPProfileErrorEvent
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::SIPProfileErrorEvent(
    CSIPConcreteProfile& aProfile,
    TInt aError)
	{
	PROFILE_DEBUG4("CSIPIMSProfileAgent::SIPProfileErrorEvent", 
	               aProfile.Id(), aError)
		
	CSIPIMSProfileContext* context = FindProfileContext( aProfile.Id() );
	TBool reportStatusToUser = ETrue;
	TRAPD( err, reportStatusToUser = 
	    HandleISIMUpdateRegistrationEventL( aProfile, context ) );	
	if ( reportStatusToUser )
		{
		if ( context && context->IgnoreErrorEvent() )
			{
			PROFILE_DEBUG4( "CSIPIMSProfileAgent::Error ignored",\
				aProfile.Id(), aError )	
			}
		else
			{
			if ( err == KErrNone )
			    {
			    err = aError;
			    }
			    
			PROFILE_DEBUG4( "CSIPIMSProfileAgent::Error sent",\
				aProfile.Id(), err )
	
			RegisterQueue( aProfile, EFalse );
			HandleWaitingForRegisteringArray();
			iSIPProfileAgentObserver.SIPProfileErrorEvent( aProfile, err );	
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::ProceedRegistration
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIMSProfileAgent::ProceedRegistration(
	CSIPConcreteProfile& aProfile,
	TInt aError)
	{
	PROFILE_DEBUG3("CSIPIMSProfileAgent::ProceedRegistration", aProfile.Id())
	return iSIPProfileAgentObserver.ProceedRegistration(aProfile, aError);
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::GetFailedProfilesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::GetFailedProfilesL(const TSIPProfileTypeInfo& /*aType*/,
					      RPointerArray<CSIPConcreteProfile>& /*aArray*/ ) const
	{
	}	
	

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::AuthorizedL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::AuthorizedL()
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::AuthorizedL")
	//function called after async call to request for authorization info
	CSIPConcreteProfile* profile = NULL;
	if (iSIMRecord->PrivateIdentity().Length())
		{
		while (iUSimWaitForAuthorizedArray.Count() > 0)
			{
			profile = NULL;
			profile = RemoveFirstProfileFromArray( 
								iUSimWaitForAuthorizedArray );
		
			if (RegAllowedWithUSIMorISIML(*profile))
				{
                 PROFILE_DEBUG1("CSIPIMSProfileAgent::AuthorizedL ISIM Registration Allowed")
				 if (profile->IsSecurityNegotiationEnabled() &&
	 				!CSIPIMSProfileContext::IsHttpDigestSettingsConfigured(
	 									*profile))
					{	
					_LIT8(KSIPIpSec3gpp, "ipsec-3gpp");
					if (CSIPIMSProfileContext::IsSupportedSecurityMechanismL(
											*iSIP, KSIPIpSec3gpp))
						{
						TRAPD(err, CheckIPSecRulesL(*profile));
						CPPUNIT_MEMORYLEAVE
						if (err)
							{
							TerminateAndErrorEvent(*profile, KErrArgument);
							}
						}
					else
						{
						TerminateAndErrorEvent(*profile, KErrArgument);
						}
					}
				else
					{
					ProfileToWaitingQueueL( profile );
					}	
				}
			else
				{
				TerminateAndErrorEvent(*profile, KErrNotSupported);
				}
			}
		}
	if ( iSIMRecord->SIMPrivateIdentity().Length() )
		{
		while (iSIMWaitForAuthorizedArray.Count() > 0)
			{
			profile = RemoveFirstProfileFromArray( 
									iSIMWaitForAuthorizedArray );
			ProfileToWaitingQueueL( profile );
			}
		}
		
	while (iWaitForAuthorizedArray.Count() > 0)
		{
		profile = RemoveFirstProfileFromArray( 
									iWaitForAuthorizedArray );
		ProfileToWaitingQueueL( profile );
		}
	}
		
// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::AsyncDeltaTimer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
void CSIPIMSProfileAgent::AsyncDeltaTimer()
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::AsyncDeltaTimer")	
	if (iDeltaTimerEntryListCount == 0)
  		{
  		TTimeIntervalMicroSeconds32 interval(KAuthorizedMicroSecsInSec);
  		iDeltaTimer.Queue(interval,iDeltaTimerEntry);
  		}
  	iDeltaTimerEntryListCount++;
	}
	
// ----------------------------------------------------------------------------
// CSIPIMSProfileAgent::ASyncTimerExpired
// ----------------------------------------------------------------------------
//     
TInt CSIPIMSProfileAgent::ASyncTimerExpired(TAny* aPtr)
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::ASyncTimerExpired")
	CSIPIMSProfileAgent* self = reinterpret_cast<CSIPIMSProfileAgent*>(aPtr);
  	TRAPD(err, self->AuthorizedL());
   	CPPUNIT_MEMORYLEAVE
    err++;
    self->iDeltaTimerEntryListCount = 0;
    self->iDeltaTimer.Remove(self->iDeltaTimerEntry);
    return ETrue;
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::AuthorizationFailed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::AuthorizationFailed()
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::AuthorizationFailed")
	TInt error = 0;
	//function called after async call to request for authorization info
	CSIPConcreteProfile* profile = NULL;
	for(TUint i = iUSimWaitForAuthorizedArray.Count(); i>0; i--)
		{
		profile = RemoveFirstProfileFromArray( 
									iUSimWaitForAuthorizedArray );
		if (RegAllowedWithSIM(*profile))
			{
			TRAP(error, iSIMWaitForAuthorizedArray.AppendL(profile));
			}
		else
			{
			TerminateAndErrorEvent( *profile, KErrNotSupported );
			}
		}
	TBool requestFailed = EFalse;
	if ( iSIMWaitForAuthorizedArray.Count() > 0 && !error )
		{
		TRAP(error, requestFailed = iUsimProvider->RequestL(EFalse));
		}
	
	if(error || requestFailed)
		{
		AuthorizationSIMFailed();
		}
	}	

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::AuthorizationSIMFailed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::AuthorizationSIMFailed()
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::AuthorizationSIMFailed")
	//function called after async call to request for authorization info
	CSIPConcreteProfile* profile = NULL;
	while (iSIMWaitForAuthorizedArray.Count() > 0)
		{
		profile = RemoveFirstProfileFromArray( 
											iSIMWaitForAuthorizedArray );
		TerminateAndErrorEvent( *profile, KErrArgument );
		}
	}	

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::AuthorizationUSIMFailed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::AuthorizationUSIMFailed()
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::AuthorizationUSIMFailed")
	//function called after async call to request for authorization info
	CSIPConcreteProfile* profile = NULL;
	while (iUSimWaitForAuthorizedArray.Count() > 0)
		{
		profile = RemoveFirstProfileFromArray( 
							iUSimWaitForAuthorizedArray );
		TerminateAndErrorEvent( *profile, KErrArgument );
		}
	}	

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::UpdateRegistrationsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::UpdateRegistrationsL()
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::UpdateRegistrationsL")
	//function called after ISIM OTA update notified
	CheckIMSReleaseTypeProfilesL();
	}	

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::CheckIMSReleaseTypeProfilesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::CheckIMSReleaseTypeProfilesL()
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::CheckIMSReleaseTypeProfilesL")
	//check if there is EIMSReleaseType profiles
	TBool found = EFalse;
	found = CheckRegistrationsL();
	CSIPConcreteProfile* profile = NULL;
	if ( !found )
		{
		TBool registering = EFalse;
		profile = iProfileQueueHandling->IPSecProfileCandidate( registering );
		if ( profile )
			{
			iUSimWaitForAuthorizedArray.AppendL( profile );
			found = ETrue;
			}
		else
			{
			profile = PreviouslyFailedIMSProfileL();
			if ( profile )
				{
				iISIMUpdateProfile = profile;
				iUSimWaitForAuthorizedArray.AppendL( profile );
				}
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::PreviouslyFailedIMSProfileL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile* CSIPIMSProfileAgent::PreviouslyFailedIMSProfileL()
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::PreviouslyFailedIMSProfile")
	iPreviouslyFailedIMSProfile = 0;
	iSIPProfileAgentObserver.GetFailedProfilesL(Type(), iFailedProfilesArray);
	for (TInt i = 0; i < iFailedProfilesArray.Count(); i++)
        {
        if (!CheckIfConfigured(*iFailedProfilesArray[i]))
        	{
        	iPreviouslyFailedIMSProfile = iFailedProfilesArray[i];
        	iFailedProfilesArray.Reset();
        	}
        }
	return iPreviouslyFailedIMSProfile;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::RemoveProfileFromWaitingQueue
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
CSIPConcreteProfile* CSIPIMSProfileAgent::RemoveProfileFromWaitingQueue(
							RPointerArray<CSIPConcreteProfile>& aProfileArray,
							CSIPConcreteProfile& aSIPConcreteProfile )
	{
	TBool found = EFalse;
    CSIPConcreteProfile* profile = NULL;
	for (TInt i= aProfileArray.Count() -1; i >= 0 && !found; --i)
		{
		CSIPConcreteProfile* tmp = aProfileArray[ i ];
		found = ( tmp->Id() == aSIPConcreteProfile.Id() );
		profile = NULL;
		if ( found )
			{
			profile = tmp;
			aProfileArray.Remove( i );
			aProfileArray.Compress();
			}					
		}
	return profile;	
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::FindProfileContextAndRegisterL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::FindProfileContextAndRegisterL( 
	CSIPConcreteProfile& aSIPConcreteProfile )
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::FindProfileContextAndRegisterL")
	CSIPIMSProfileContext* context = FindProfileContext(aSIPConcreteProfile);
	if (!context)
		{
		TRAPD(err, context = &ProvideProfileContextL(aSIPConcreteProfile));
		CPPUNIT_MEMORYLEAVE
		if (err)
			{
			ResetISIMUpdateProfile( iISIMUpdateProfile );
			TerminateAndErrorEvent(aSIPConcreteProfile, KErrArgument);
			}
		}
	if (context)
		{
		ChooseProfileTypeL( &aSIPConcreteProfile , context );	
		TRAPD(err, context->RegisterL());
		CPPUNIT_MEMORYLEAVE
		if (err)
			{
			ResetISIMUpdateProfile( iISIMUpdateProfile );
			TerminateAndErrorEvent(aSIPConcreteProfile, KErrArgument);
			}
		else
			{
			if (iISIMUpdateProfile && 
					( iISIMUpdateProfile->Id() == aSIPConcreteProfile.Id() ))
				{
				iISIMUpdateProfile = 0;
				TUint32 contextId = context->Registration()->ContextId();
	
				iSIPProfileAgentObserver.SIPProfileStatusEvent(
				    aSIPConcreteProfile, contextId );
				}
			else
				{
				if ( iInterimProfile && 
				     iInterimProfile->Id() == aSIPConcreteProfile.Id() )
					{
					PROFILE_DEBUG1("CSIPIMSProfileAgent::FindProfileContextAndRegisterL iInterimProfile")
					iInterimProfile = 0;
					TUint32 contextId = context->Registration()->ContextId();
	
					iSIPProfileAgentObserver.SIPProfileStatusEvent(
					    aSIPConcreteProfile, contextId );
					}
				}
			}
		}
	PROFILE_DEBUG1("CSIPIMSProfileAgent::FindProfileContextAndRegisterL end")
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::CheckRegistrationsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIMSProfileAgent::CheckRegistrationsL()
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::CheckRegistrationsL")
	TBool found = EFalse;
	TUint32 profileId = 0;
	CSIPConcreteProfile* profile = NULL;
	for (TInt i=0; i < iConnectionCtxArray.Count() && !profileId; i++)
		{
		profileId = iConnectionCtxArray[i]->IsIPSecInUse();
		if (profileId)
			{
			found = ETrue;
			}
		}
	if ( found )
		{
		TBool registering = EFalse;
		iProfileQueueHandling->IPSecProfileCandidate( registering );
		if ( registering )
			{
			//profile is not found in deregistering que either, so new register is possible
			CSIPIMSProfileContext* profilecontext = FindProfileContext( profileId );
			MSIPProfileContext::TSIPProfileState state = 
			    profilecontext->CurrentState();
			if (state == MSIPProfileContext::EProxyResolvingInProgress ||
				state == MSIPProfileContext::EDelayedRegisterRequested ||
				state == MSIPProfileContext::ERegisterRequested)
				{
				profile = profilecontext->Profile();
				profilecontext->SetNextState( *iInit );
				CleanIdleConnectionContexts();
				iUSimWaitForAuthorizedArray.AppendL( profile );
				}
			if ( state == MSIPProfileContext::ERegistrationInProgress )
				{
				iISIMUpdateProfile = profilecontext->Profile();
				}
			if ( state == MSIPProfileContext::ERegistered )
				{
				iISIMUpdateProfile = profilecontext->Profile();
				DeregisterL( *iISIMUpdateProfile );
				}
			}
		}
	return found;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::IncomingRequest
// From MSIPObserver
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::IncomingRequest(
    TUint32 /*aIapId*/,
    CSIPServerTransaction* aTransaction)
    {
    // Should not be called, because CSIP is created with a NULL UID
    // Delete the transaction to prevent  amemory leak
    delete aTransaction;
    }

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::TimedOut
// From MSIPObserver
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::TimedOut(CSIPServerTransaction& /*aTransaction*/)
    {
    }

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::ChallengeReceived
// From MSIPHttpDigestChallengeObserver2
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::ChallengeReceived(
    const CSIPClientTransaction& aTransaction)
    {
	TBool found = EFalse;
	for (TInt i=0; i < iConnectionCtxArray.Count() && !found; i++)
		{
		found = 
		    iConnectionCtxArray[i]->SetCredentials(aTransaction,*iHttpDigest);
		}
    }

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::ChallengeReceived
// From MSIPHttpDigestChallengeObserver2
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::ChallengeReceived(const CSIPRefresh& aRefresh)
    {
	TBool found = EFalse;
	for (TInt i=0; i < iConnectionCtxArray.Count() && !found; i++)
		{
		found = iConnectionCtxArray[i]->SetCredentials(aRefresh,*iHttpDigest);
		}
    }

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::FindProfileContext
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIMSProfileContext* CSIPIMSProfileAgent::FindProfileContext(
	CSIPConcreteProfile& aSIPConcreteProfile) const
	{
	return FindProfileContext(aSIPConcreteProfile.Id());
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::FindProfileContext
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIMSProfileContext* CSIPIMSProfileAgent::FindProfileContext(
	TUint32 aProfileId) const
	{
	for (TInt i=0; i < iConnectionCtxArray.Count(); i++)
		{
		CSIPIMSProfileContext *context = 
			iConnectionCtxArray[i]->FindContext(aProfileId);
		if (context)
			{
			return context;
			}
		}
	return 0;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::FindConnectionContext
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIMSConnectionContext* CSIPIMSProfileAgent::FindConnectionContext(
	CSIPConcreteProfile& aProfile) const
	{
	CSIPIMSConnectionContext* context = 0;
	TBool found = EFalse;
	for ( TInt i=0; i < iConnectionCtxArray.Count() && !found; i++ )
		{
		if ( iConnectionCtxArray[i]->ConnectionUser( aProfile ) )
			{
			context = iConnectionCtxArray[i];
			found = ETrue;
			}
		}
	return context;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::FindConnectionContext
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIMSConnectionContext* CSIPIMSProfileAgent::FindConnectionContext(
	TUint32 aIapId ) const
	{
	CSIPIMSConnectionContext* context =0;
	TBool found = EFalse;
	for ( TInt i=0; i < iConnectionCtxArray.Count() && !found; i++ )
		{
		if ( iConnectionCtxArray[i]->Connection()->IapId() == aIapId )
			{
			context = iConnectionCtxArray[i];
			found = ETrue;
			}
		}
	return context;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::ProvideProfileContextL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIMSProfileContext& CSIPIMSProfileAgent::ProvideProfileContextL(
	CSIPConcreteProfile& aProfile)
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::ProvideProfileContextL")
	CSIPIMSConnectionContext* connContext = FindConnectionContext(aProfile);
	if (!connContext)
		{
		PROFILE_DEBUG1("CSIPIMSProfileAgent::ProvideProfileContextL create connection first")

		connContext = CSIPIMSConnectionContext::NewLC();
		
		// If there's several profiles using the same iap, other ones need
		// new csip instance as once csip instance cannot contain several
		// csipconnections having the same iap id.
		//
		TUint32 iapId( aProfile.IapId() );	
		CSIPConnection* connection = NULL;
		if ( FindConnectionContext( iapId ) )
		    {
		    CSIP& sip = connContext->CreateSipL( *this, *this );
		    connection = CSIPConnection::NewLC( sip, iapId, *connContext );
		    }
		else
		    {
		    connection = CSIPConnection::NewLC( *iSIP, iapId, *connContext );
		    }
		connContext->SetConnectionL( connection );
		CleanupStack::Pop( connection );
		
		// Connection should be set before setting user
		connContext->SetConnectionUserL( aProfile );
		
        iConnectionCtxArray.AppendL( connContext );
        
		CleanupStack::Pop( connContext );
		}
		
	CSIPIMSProfileContext* profilectx = 
	    CSIPIMSProfileContext::NewLC(*iXMLParser, *iSIP, *connContext, *this,
	        *iInit, aProfile, iDeltaTimer, *iSIMRecord,*iConfigExtension);
	connContext->AddProfileContextL(profilectx);
	CleanupStack::Pop(profilectx);
	return *profilectx;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileAgent::CleanIdleConnectionContexts
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::CleanIdleConnectionContexts()
	{
	PROFILE_DEBUG1("CSIPIMSProfileAgent::CleanIdleConnectionContexts")
	CSIPIMSConnectionContext* context = 0;

	for (TInt i=iConnectionCtxArray.Count()-1; i>= 0;)
		{
		iConnectionCtxArray[i]->CleanIdleContexts();
		if (iConnectionCtxArray[i]->IsIdle())
			{
			PROFILE_DEBUG1("CSIPIMSProfileAgent::CleanIdleConnectionContexts,\
				ConnectionContext removed")
			context = iConnectionCtxArray[i];
			iConnectionCtxArray.Remove(i);
			delete context;
			context = NULL;
			}
		i--;
		}
		
	iConnectionCtxArray.Compress();
	}
	
// ----------------------------------------------------------------------------
// CSIPIMSProfileAgent::GetTsyModuleNameL
// ----------------------------------------------------------------------------
//
void CSIPIMSProfileAgent::GetTsyModuleNameL( TDes& aTsyModuleName ) const
    {
    __ASSERT_ALWAYS( aTsyModuleName.MaxSize() >= KCommsDbSvrMaxFieldLength,
		User::Leave( KErrArgument ) );

    using namespace CommsDat;

    CMDBSession* db = CMDBSession::NewLC(KCDVersion1_1);

    CMDBField<TUint32>* globalSettingField =
    	new (ELeave) CMDBField<TUint32>(KCDTIdModemPhoneServicesSMS);
    CleanupStack::PushL(globalSettingField);

    globalSettingField->SetRecordId(1);
    globalSettingField->LoadL(*db);
    TUint32 modemId = *globalSettingField;

    CMDBField<TDesC>* tsyField = new (ELeave) CMDBField<TDesC>(KCDTIdTsyName);
    CleanupStack::PushL(tsyField);

    tsyField->SetRecordId(modemId);
    tsyField->LoadL(*db);
    aTsyModuleName = *tsyField;

    CleanupStack::PopAndDestroy(3, db); // db, tsyField & globalSettingField 
    }
		
// ----------------------------------------------------------------------------
// CSIPIMSProfileAgent::RegisterQueue
// ----------------------------------------------------------------------------
//
TInt CSIPIMSProfileAgent::RegisterQueue( 
    CSIPConcreteProfile& aProfile,
    TBool aReportError )
    {
    PROFILE_DEBUG1("CSIPIMSProfileAgent::RegisterQueue")
    TInt err( KErrNone );
    
    if ( AllowedTakeFromQueue( aProfile ) )
        {
        PROFILE_DEBUG1("CSIPIMSProfileAgent::RegisterQueue RegisterFromQueueL")
        TRAP( err, iProfileQueueHandling->RegisterFromQueueL( aProfile ) );
        }
        
    if ( err && aReportError )
        {
        PROFILE_DEBUG1("CSIPIMSProfileAgent::RegisterQueue ErrorEvent")
        iSIPProfileAgentObserver.SIPProfileErrorEvent( aProfile, err );
        }
    
    PROFILE_DEBUG3("CSIPIMSProfileAgent::RegisterQueue end, err=", err)   
    return err;
    }
    
