// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : sipietfprofileagent.cpp
// Part of     : sip ietf plugin
// implementation
// Version     : 1.0
//




// INCLUDE FILES
#include	"sipietfprofileagent.h"
#include    "sipconcreteprofile.h"
#include	"sipprofileagentobserver.h"
#include	"sipconnection.h"
#include	"sipprflinitstate.h"
#include	"sipprflresolvingproxiesstate.h"
#include	"sipprflregisterrequestedstate.h"
#include	"sipprflregisterdelayrequestedstate.h"
#include	"sipprflregistrationinprogressstate.h"
#include	"sipprflregisteredstate.h"
#include	"sipprflderegistrationinprogressstate.h"
#include	"sipprflderegisterrequestedstate.h"
#include	"sipprflstatebase.h"
#include	"sipietfprofilecontext.h"
#include	"sipgendefs.h"
#include	"SipProfileLog.h"
#include    "sipstrings.h"
#include    "sipstrconsts.h"
#include    "sipprofilequeuehandling.h"
#include    <sip.h>
#include    <siphttpdigest.h>
#include    <sipservertransaction.h>
#include    <sipprofile.h>

_LIT8(KSIPIETFProfileType, "IETF");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPIetfProfileAgent* CSIPIetfProfileAgent::NewL(TAny* aInitParams)
	{
	CSIPIetfProfileAgent* self = new (ELeave) CSIPIetfProfileAgent(
		static_cast<TSIPProfileAgentInitParams*>(aInitParams));
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::CSIPIetfProfileAgent
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPIetfProfileAgent::CSIPIetfProfileAgent(
    TSIPProfileAgentInitParams* aInitParams)
    : iSIPProfileAgentObserver(aInitParams->iSIPProfileAgentObserver),
      iDeltaTimer(aInitParams->iDeltaTimer)
	{
	iType.iSIPProfileClass = TSIPProfileTypeInfo::EInternet;
	iType.iSIPProfileName = KSIPIetfType();
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileAgent::ConstructL()
    {
    SIPStrings::OpenL();
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
	iProfileQueueHandling = CSIPProfileQueueHandling::NewL(*this);
	
	iSIP = CSIP::NewL(TUid::Null(),*this);
	iHttpDigest = CSIPHttpDigest::NewL(*iSIP,*this);
	iConfigExtension = NULL;
    }

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::~CSIPIetfProfileAgent
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
CSIPIetfProfileAgent::~CSIPIetfProfileAgent()
	{
	delete iInit;
	delete iResolvingProxies;
	delete iRegRequested;
	delete iRegistered;
	delete iRegInProgress;
	delete iDeregRequested;
	delete iDeregInProgress;
	delete iRegDelayRequested;
	delete iProfileQueueHandling;
	iConnectionCtxArray.ResetAndDestroy();
	iConnectionCtxArray.Close();
	SIPStrings::Close();
	delete iHttpDigest;
	delete iSIP;
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::SIPProfileAgentType
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TSIPProfileTypeInfo& CSIPIetfProfileAgent::Type() const
	{
	return iType;
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::CreateL
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile* CSIPIetfProfileAgent::CreateL()
	{
	TSIPProfileTypeInfo type;		
	type.iSIPProfileClass = TSIPProfileTypeInfo::EInternet;
	type.iSIPProfileName = KSIPIETFProfileType;

	CSIPConcreteProfile* profile = CSIPConcreteProfile::NewL();
	profile->SetProfileType(type);
	return profile;
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::RegisterL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileAgent::RegisterL(
	CSIPConcreteProfile& aSIPConcreteProfile)
	{
	__ASSERT_ALWAYS (aSIPConcreteProfile.AOR().Length() > 0, 
		User::Leave(KErrArgument));
	__ASSERT_ALWAYS (aSIPConcreteProfile.Server(KSIPRegistrar).Length(),
		User::Leave(KErrArgument));

	PROFILE_DEBUG3("CSIPIetfProfileAgent::RegisterL", aSIPConcreteProfile.Id())
	
	if (!iProfileQueueHandling->AddRegisterToQueueL( aSIPConcreteProfile, 
	                                                 EFalse ))
		{
        CSIPIetfProfileContext* context = FindProfileContext(aSIPConcreteProfile);
        if (!context)
	        {
	        context = &ProvideProfileContextL(aSIPConcreteProfile);
	        }		
        context->RegisterL();
		}
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::UpdateL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileAgent::UpdateL(
	CSIPConcreteProfile& aNewProfile, 
	CSIPConcreteProfile& aOldProfile)
	{
	PROFILE_DEBUG3("CSIPIetfProfileAgent::UpdateL", aOldProfile.Id())
	iProfileQueueHandling->Cleanup(aOldProfile,&aNewProfile);
	CSIPIetfProfileContext* context = FindProfileContext(aOldProfile);
	if (!context)
	    {
	    User::Leave(KErrNotFound);
	    }
	context->UpdateL(aNewProfile);
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::DeregisterL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileAgent::DeregisterL(
	CSIPConcreteProfile& aSIPConcreteProfile)
	{
	PROFILE_DEBUG3("CSIPIetfProfileAgent::DeregisterL", aSIPConcreteProfile.Id())
	CSIPIetfProfileContext* context = FindProfileContext(aSIPConcreteProfile);
	if ( context )
		{
		if ( !DeregisterToWaitingQueueL( context ))
			{
			context->DeregisterL();
			}
		}
	else
		{
		iProfileQueueHandling->RemoveProfileFromRegQueueL(aSIPConcreteProfile);
		}	 
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::GetProfileState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSIPIetfProfileAgent::GetProfileState(
	CSIPConcreteProfile::TStatus& aState, 
	CSIPConcreteProfile& aProfile) const
	{
	CSIPIetfProfileContext* context = FindProfileContext(aProfile);
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
// CSIPIetfProfileAgent::IsIdle
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIetfProfileAgent::IsIdle()
	{
	CleanIdleConnectionContexts();
	return (iConnectionCtxArray.Count()==0);
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::RegisterPending
// -----------------------------------------------------------------------------
//	
TBool CSIPIetfProfileAgent::RegisterPending(
    CSIPConcreteProfile& aSIPProfile) const
    {
    PROFILE_DEBUG3("CSIPIetfProfileAgent::IsRegisterPending", aSIPProfile.Id()) 
    
	CSIPIetfProfileContext* context = FindProfileContext(aSIPProfile);
	if (context)
		{
		return context->IsRegisterPending();
		}
	return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::TerminateHandling
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSIPIetfProfileAgent::TerminateHandling(CSIPConcreteProfile& aProfile)
	{
	CSIPIetfProfileContext* context = FindProfileContext(aProfile);
	if (context)
		{
		PROFILE_DEBUG3("CSIPIetfProfileAgent::TerminateHandling", aProfile.Id())
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
// CSIPIetfProfileAgent::Extension
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSIPIetfProfileAgent::Extension(TInt aOperationCode, TAny* aParams)
    {
    TInt err(KErrNone);
    if(aParams && aOperationCode == KSipProfileAgentConfigExtension)
    	iConfigExtension = static_cast<CSipProfileAgentConfigExtension*>(aParams);
    else
    	err = KErrNotSupported;
    return err;
    }

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::SIPProfileStatusEvent
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileAgent::SIPProfileStatusEvent(
	CSIPConcreteProfile& aProfile,
	TUint32 aContextId)
	{
	PROFILE_DEBUG4("CSIPIetfProfileAgent::SIPProfileStatusEvent",\
		aProfile.Id(), aContextId)
		
	
	RegisterQueue( aProfile, ETrue );
	
	iSIPProfileAgentObserver.SIPProfileStatusEvent( aProfile, aContextId );
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::SIPProfileErrorEvent
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileAgent::SIPProfileErrorEvent(
    CSIPConcreteProfile& aProfile,
    TInt aError)
	{
	PROFILE_DEBUG4("CSIPIetfProfileAgent::ErrorEvent", aProfile.Id(), aError)
	CSIPIetfProfileContext* context = FindProfileContext(aProfile.Id());

	if (context && context->IgnoreErrorEvent())
		{
		PROFILE_DEBUG4("CSIPIetfProfileAgent::Error ignored",\
                       aProfile.Id(), aError)	
		}
	else
		{
		PROFILE_DEBUG4("CSIPIetfProfileAgent::Error sent", 
		               aProfile.Id(), aError)
			
		RegisterQueue( aProfile, EFalse );
			
		iSIPProfileAgentObserver.SIPProfileErrorEvent(aProfile,aError);
		}
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::ProceedRegistration
// -----------------------------------------------------------------------------
//
TBool CSIPIetfProfileAgent::ProceedRegistration(
	CSIPConcreteProfile& aProfile,
	TInt aError)
	{
	PROFILE_DEBUG3("CSIPIetfProfileAgent::ProceedRegistration", aProfile.Id())
	return iSIPProfileAgentObserver.ProceedRegistration(aProfile, aError);
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::GetFailedProfilesL
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileAgent::GetFailedProfilesL(
    const TSIPProfileTypeInfo& /*aType*/,
    RPointerArray<CSIPConcreteProfile>& /*aFailedProfiles*/) const
	{
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::RegisterProfileL
// From MSIPProfileFSMUser
// -----------------------------------------------------------------------------
//	
void CSIPIetfProfileAgent::RegisterProfileL(
    CSIPConcreteProfile& aSIPProfile)
    {
    RegisterL(aSIPProfile);
    }

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::DeregisterProfileL
// From MSIPProfileFSMUser
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileAgent::DeregisterProfileL(
    CSIPConcreteProfile& aSIPProfile)
    {
    DeregisterL(aSIPProfile);
    }

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::RetryProfileRegistrationL
// From MSIPProfileFSMUser
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileAgent::RetryProfileRegistrationL( 
    CSIPConcreteProfile& aSIPProfile)
    {
	PROFILE_DEBUG3("CSIPIetfProfileAgent::RetryProfileRegistrationL", 
	               aSIPProfile.Id())

	if (!iProfileQueueHandling->AddRegisterToQueueL(aSIPProfile, ETrue))
		{
        CSIPIetfProfileContext* context = FindProfileContext(aSIPProfile);
        if (!context)
	        {
	        User::Leave(KErrNotFound);
	        }		
        context->RetryRegistration();
		}    
    }

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::AddProfileIntoQueue
// From MSIPProfileFSMUser
// -----------------------------------------------------------------------------
//
TBool CSIPIetfProfileAgent::AddProfileIntoQueue(
    CSIPConcreteProfile& aSIPProfile) const
    {
    PROFILE_DEBUG3("CSIPIetfProfileAgent::AddProfileIntoQueue", 
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
// CSIPIetfProfileAgent::RegisterProfileAfterQueueL
// From MSIPProfileFSMUser
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileAgent::RegisterProfileAfterQueueL(
    CSIPConcreteProfile& aSIPProfile)
    {
	PROFILE_DEBUG3("CSIPIetfProfileAgent::RegisterProfileAfterQueueL", 
	               aSIPProfile.Id())    
    
    CSIPIetfProfileContext* context = FindProfileContext(aSIPProfile);
	if (!context)
	    {
	    context = &ProvideProfileContextL(aSIPProfile);
	    } 
	context->RegisterL();   
    }

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::DeregisterProfileAfterQueueL
// From MSIPProfileFSMUser
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileAgent::DeregisterProfileAfterQueueL(
    CSIPConcreteProfile& aSIPProfile)
    {
	PROFILE_DEBUG3("CSIPIetfProfileAgent::DeregisterProfileAfterQueueL", 
	               aSIPProfile.Id())    
    
    CSIPIetfProfileContext* context = FindProfileContext(aSIPProfile);
    __ASSERT_ALWAYS(context != NULL, User::Leave(KErrNotFound));
    context->DeregisterL();
    }

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::RetryProfileRegistrationAfterQueueL
// From MSIPProfileFSMUser
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileAgent::RetryProfileRegistrationAfterQueueL(
    CSIPConcreteProfile& aSIPProfile)
    {
	PROFILE_DEBUG3("CSIPIetfProfileAgent::RetryProfileRegistrationAfterQueueL", 
	               aSIPProfile.Id())    
    
    CSIPIetfProfileContext* context = FindProfileContext(aSIPProfile);
    __ASSERT_ALWAYS(context != NULL, User::Leave(KErrNotFound));
    context->RetryRegistration();   
    }

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::IsInQueue
// From MSIPProfileFSMUser
// -----------------------------------------------------------------------------
//
TBool CSIPIetfProfileAgent::IsInQueue(
    CSIPConcreteProfile& aSIPProfile) const
    {
    PROFILE_DEBUG1("CSIPIetfProfileAgent::IsInQueue")
	PROFILE_DEBUG3("CSIPIetfProfileAgent::IsInQueue", 
	               aSIPProfile.Id())    
    return iProfileQueueHandling->IsInQueue(aSIPProfile);
    }

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::SetInterimProfile
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileAgent::SetInterimProfile( CSIPConcreteProfile* /*aSIPConcreteProfile*/ )
	{

	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::IncomingRequest
// From MSIPObserver
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileAgent::IncomingRequest(
    TUint32 /*aIapId*/,
    CSIPServerTransaction* aTransaction)
    {
    // Should not be called, because CSIP is created with a NULL UID
    // Delete the transaction to prevent  amemory leak
    delete aTransaction;
    }

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::TimedOut
// From MSIPObserver
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileAgent::TimedOut(CSIPServerTransaction& /*aTransaction*/)
    {
    }

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::ChallengeReceived
// From MSIPHttpDigestChallengeObserver2
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileAgent::ChallengeReceived(
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
// CSIPIetfProfileAgent::ChallengeReceived
// From MSIPHttpDigestChallengeObserver2
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileAgent::ChallengeReceived(const CSIPRefresh& aRefresh)
    {
	TBool found = EFalse;
	for (TInt i=0; i < iConnectionCtxArray.Count() && !found; i++)
		{
		found = iConnectionCtxArray[i]->SetCredentials(aRefresh,*iHttpDigest);
		}
    }
		
// ----------------------------------------------------------------------------
// CSIPIetfProfileAgent::RegisterQueue
// ----------------------------------------------------------------------------
//
void CSIPIetfProfileAgent::RegisterQueue( 
    CSIPConcreteProfile& aProfile,
    TBool aReportError )
    {
	PROFILE_DEBUG3("CSIPIetfProfileAgent::RegisterQueue", aProfile.Id())    

    TInt err( KErrNone );
    
    if ( AllowedTakeFromQueue( aProfile ) )
        {
        TRAP( err, iProfileQueueHandling->RegisterFromQueueL( aProfile ) );
        }
        
    if ( err && aReportError )
        {
        iSIPProfileAgentObserver.SIPProfileErrorEvent( aProfile, err );
        }
    }

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::AllowedTakeFromQueue
// -----------------------------------------------------------------------------
//	
TBool CSIPIetfProfileAgent::AllowedTakeFromQueue(
    CSIPConcreteProfile& aSIPConcreteProfile )
    {
	PROFILE_DEBUG3("CSIPIetfProfileAgent::AllowedTakeFromQueueL", 
	               aSIPConcreteProfile.Id())     

	return !AddProfileIntoQueue( aSIPConcreteProfile );
    }
	
// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::DeregisterToWaitingQueueL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
TBool CSIPIetfProfileAgent::DeregisterToWaitingQueueL( 
    CSIPIetfProfileContext* aContext )
	{
	PROFILE_DEBUG3("CSIPIetfProfileAgent::DeregisterToWaitingQueueL", 
	               aContext->Profile()->Id())
	return iProfileQueueHandling->AddDeregisterToQueueL( *aContext->Profile() );
	}	

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::FindProfileContext
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIetfProfileContext* CSIPIetfProfileAgent::FindProfileContext(
	CSIPConcreteProfile& aSIPConcreteProfile) const
	{
	return FindProfileContext(aSIPConcreteProfile.Id());
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::FindProfileContext
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIetfProfileContext* CSIPIetfProfileAgent::FindProfileContext(
	TUint32 aProfileId) const
	{
	for (TInt i=0; i < iConnectionCtxArray.Count(); i++)
		{
		CSIPIetfProfileContext *context = 
			iConnectionCtxArray[i]->FindContext(aProfileId);
		if (context)
			{
			return context;
			}
		}
	return 0;
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::FindConnectionContext
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIetfConnectionContext* CSIPIetfProfileAgent::FindConnectionContext(
	CSIPConcreteProfile& aProfile) const
	{
	CSIPIetfConnectionContext* context =0;
	TBool found = EFalse;
	for (TInt i=0; i < iConnectionCtxArray.Count() && !found; i++)
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
// CSIPIetfProfileAgent::FindConnectionContext
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIetfConnectionContext* CSIPIetfProfileAgent::FindConnectionContext(
	TUint32 aIapId ) const
	{
	CSIPIetfConnectionContext* context =0;
	TBool found = EFalse;
	for (TInt i=0; i < iConnectionCtxArray.Count() && !found; i++)
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
// CSIPIetfProfileAgent::ProvideContextL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIetfProfileContext& CSIPIetfProfileAgent::ProvideProfileContextL(
	CSIPConcreteProfile& aProfile)
	{
	PROFILE_DEBUG3("CSIPIetfProfileAgent::ProvideProfileContextL", 
	               aProfile.Id())
	
	CSIPIetfConnectionContext* connContext = FindConnectionContext(aProfile);
	if (!connContext)
		{
		// Must create connection first
		connContext = CSIPIetfConnectionContext::NewLC();
		
		// If there's several profiles using the same iap, other ones need
		// new csip instance as once csip instance cannot contain several
		// csipconnections having the same iap id.
		//
		TUint32 iapId( aProfile.IapId() );	
		CSIPConnection* connection = NULL;
		if ( FindConnectionContext( iapId ) )
		    {
		    CSIP& sip = connContext->CreateSipL( *this, *this );
		    connection = CSIPConnection::NewL( sip, iapId, *connContext );
		    }
		else
		    {
		    connection = CSIPConnection::NewL( *iSIP, iapId, *connContext );
		    }
		connContext->SetConnection( connection );
		
		// Connection should be set before setting user
		connContext->SetConnectionUserL( aProfile );
		
		User::LeaveIfError( iConnectionCtxArray.Append( connContext ) );
		CleanupStack::Pop();//connContext
		}
	CSIPIetfProfileContext* profilectx = CSIPIetfProfileContext::NewLC(
		*iSIP, *connContext, *this,
		*iInit, aProfile, iDeltaTimer,*iConfigExtension);
	connContext->AddProfileContextL(profilectx);
	CleanupStack::Pop();//profilectx 
	return *profilectx;
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileAgent::ProvideContextL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileAgent::CleanIdleConnectionContexts()
	{
	CSIPIetfConnectionContext* context = 0;

	for (TInt i=iConnectionCtxArray.Count()-1; i>= 0;)
		{
		iConnectionCtxArray[i]->CleanIdleContexts();
		if (iConnectionCtxArray[i]->IsIdle())
			{
			PROFILE_DEBUG1("CSIPIetfProfileAgent::CleanIdleConnectionContexts,\
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
