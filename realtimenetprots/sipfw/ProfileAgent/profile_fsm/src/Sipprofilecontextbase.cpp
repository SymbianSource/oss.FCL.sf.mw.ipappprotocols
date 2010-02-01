// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : sipprofilecontextbase.cpp
// Part of     : sip profile fsm
// implementation
// Version     : %version: 3.1.1 %
//



// INCLUDE FILES
#include <bamdesca.h>
#include "sipprofilecontextbase.h"
#include "sipgendefs.h"
#include "SipProfileLog.h"
#include "sipprflstatebase.h"
#include "sipconcreteprofile.h"
#include "sipprofileagentobserver.h"
#include "siperr.h"
#include "siphttpdigest.h"
#include "sipconnection.h"
#include "sipregistrationbinding.h"
#include "siprefresh.h"
#include "sipclienttransaction.h"
#include "sipresponseelements.h"
#include "sipmessageelements.h"
#include "sipwwwauthenticateheader.h"
#include "sipproxyauthenticateheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipmanagedprofile.h"
#include "sip.h"
#include <uri8.h>

const TInt KMicroSecsInSec = 1000000;
_LIT8(KTmpSipUri, "sip:tmp");
_LIT8(KDynamicProxy, "0.0.0.0");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::CSIPProfileContextBase
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProfileContextBase::CSIPProfileContextBase(
	CSIP& aSIP,
	CSIPConnection& aConnection,
	MSIPProfileAgentObserver& aObserver,
	CSIPPrflStateBase& aInitState,
	CSIPConcreteProfile& aProfile,
	CDeltaTimer& aDeltaTimer)
	: iSIP(aSIP),
	  iConnection(aConnection),
	  iObserver(aObserver),
	  iCurrentState(&aInitState),
	  iInitState(&aInitState),
	  iProfile(&aProfile),
	  iProfileId(aProfile.Id()),
	  iDeltaTimer(aDeltaTimer),
	  iErrorForDeltaTimer(KErrNone),
	  iDynamicProxyCount(0),
	  iDeltaTimerCallBack(RetryDeltaTimerExpired, this), 
	  iRetryPossible(EFalse),
	  iResolvingCompleted(EFalse),
	  iUseBackupProxy(EFalse)
	{
	iDeltaTimerEntry.Set(iDeltaTimerCallBack);
    iWwwAuthenticateHeaderName = 
        SIPStrings::StringF(SipStrConsts::EWWWAuthenticateHeader);
    iProxyAuthenticateHeaderName =
        SIPStrings::StringF(SipStrConsts::EProxyAuthenticateHeader);
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::RetryDeltaTimer()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileContextBase::RetryDeltaTimer( TUint aTime, TInt aError )
	{
	iErrorForDeltaTimer = aError;
    iDeltaTimer.Remove(iDeltaTimerEntry);
    TTimeIntervalMicroSeconds32 interval(aTime*KMicroSecsInSec);
    iDeltaTimer.Queue(interval,iDeltaTimerEntry);
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::RetryDeltaTimerExpired()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSIPProfileContextBase::RetryDeltaTimerExpired( TAny* aPtr )
	{
    CSIPProfileContextBase* self = reinterpret_cast<CSIPProfileContextBase*>(aPtr);
    self->InitializeRetryTimerValue();
    if (self->AgentObserver().ProceedRegistration( *(self->Profile()), self->iErrorForDeltaTimer) )
    	{
		TRAP_IGNORE(self->ContinueRegistrationL())
    	}
    return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::ContinueRegistrationL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPProfileContextBase::ContinueRegistrationL()
	{
    iCurrentState->ChangeRegistrationStateL(*this);
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::SetNegotiatedSecurityMechanismL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileContextBase::SetNegotiatedSecurityMechanismL()
    {
    if (iProfile)
        {
        TPtrC8 proxyHost(ExtractProxyHost());
        if (proxyHost.Length() > 0)
            {
            HBufC8* secMech = iSIP.NegotiatedSecurityMechanismL(proxyHost);
            if (secMech)
                {
                CleanupStack::PushL(secMech);
                iProfile->SetNegotiatedSecurityMechanismL(*secMech);
                CleanupStack::PopAndDestroy(secMech);
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::ResetNegotiatedSecurityMechanismL()
// -----------------------------------------------------------------------------
//		
EXPORT_C void CSIPProfileContextBase::ResetNegotiatedSecurityMechanismL()
    {
    if (iProfile)
        {
        iProfile->SetNegotiatedSecurityMechanismL(KNullDesC8);
        }
    }
	
// -----------------------------------------------------------------------------
// CSIPProfileContextBase::RemoveDeltaTimerEntry
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileContextBase::RemoveDeltaTimerEntry()
	{
	iDeltaTimer.Remove(iDeltaTimerEntry);
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::~CSIPProfileContextBase
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProfileContextBase::~CSIPProfileContextBase()
	{
    delete iClientTx;
	delete iRegistration;
	delete iProxies;
	iDeltaTimer.Remove(iDeltaTimerEntry);
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileContextBase::Connection()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPConnection& CSIPProfileContextBase::Connection() const
	{
	return iConnection;
	}	
	
// -----------------------------------------------------------------------------
// CSIPProfileContextBase::Registration()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRegistrationBinding* CSIPProfileContextBase::Registration()
	{
	return iRegistration;
	}	
	
// -----------------------------------------------------------------------------
// CSIPProfileContextBase::SetNextState()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileContextBase::SetNextState(CSIPPrflStateBase& aState)
	{
	iCurrentState = &aState;
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileContextBase::ForgetProfile
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileContextBase::ForgetProfile()
	{
	PROFILE_DEBUG3("CSIPProfileContextBase::ForgetProfile", ProfileId())

    TRAP_IGNORE(ResetRegisteredAORsL())
	
	if (!iRetryPossible) 
		{
	    delete iClientTx;
	    iClientTx = 0;
	    iProfile = 0;
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::SetTransaction()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileContextBase::SetTransaction(
    CSIPClientTransaction* aTransaction)
	{
	delete iClientTx;
	iClientTx = aTransaction;
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::Profile()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPConcreteProfile* CSIPProfileContextBase::Profile()
	{
	return iProfile;
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::AgentObserver()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C MSIPProfileAgentObserver& CSIPProfileContextBase::AgentObserver() const
	{
	return iObserver;
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::SetProfile()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileContextBase::SetProfile(CSIPConcreteProfile* aProfile)
	{
	iProfile = aProfile;
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::SetRegisteredAORs()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CSIPProfileContextBase::SetRegisteredAORs()
    {
    TRAPD(err, SetRegisteredAORsL());
    return err;
    }
    
// -----------------------------------------------------------------------------
// CSIPProfileContextBase::CurrentState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C MSIPProfileContext::TSIPProfileState 
CSIPProfileContextBase::CurrentState() const
	{
    PROFILE_DEBUG4("CSIPProfileContextBase::CurrentState"\
        ,ProfileId(), iCurrentState->Name())
    return iCurrentState->Name();
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::CurrentMappedState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C CSIPConcreteProfile::TStatus 
CSIPProfileContextBase::CurrentMappedState() const
    {
    PROFILE_DEBUG4("CSIPProfileContextBase::CurrentMappedState"\
        ,ProfileId(), iCurrentState->ConcreteProfileState())
    return iCurrentState->ConcreteProfileState();
    }	

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::RegisterL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileContextBase::RegisterL()
	{
	PROFILE_DEBUG4("CSIPProfileContextBase::RegisterL", ProfileId()\
		,iCurrentState->Name())
	
    iCurrentState->RegisterL(*this);
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::IsRegisterPending()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 		
EXPORT_C TBool CSIPProfileContextBase::IsRegisterPending() const
    {
    return iCurrentState->IsRegisterPending();
    }

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::RetryRegistration
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileContextBase::RetryRegistration()
	{
	PROFILE_DEBUG3("CSIPProfileContextBase::RetryRegistration", ProfileId())
	if (iRetryPossible)
		{
		PROFILE_DEBUG1("RetryRegistration, retry possible")
		SetNextState(*iInitState);
		TInt err = KErrNone;
		if (iProxies && iProxies->MdcaCount() > iDynamicProxyCount)
			{
			PROFILE_DEBUG1("RetryRegistration, register dynamic")
			TRAP(err, RegisterDynamicL(*iProxies));
			}
		else
			{			
			iResolvingCompleted = EFalse;
			TRAP(err, iCurrentState->RegisterL(*this));
			}
	    if (err)
	        {
	        HandleProxyResolvingError(err);
	        }
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::ProxyResolvingRequestCompleteL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileContextBase::ProxyResolvingRequestCompleteL(
	MDesC8Array& aProxies)
	{
	PROFILE_DEBUG3("CSIPProfileContextBase::ProxyResolvingRequestCompleteL"\
		,ProfileId())
    iCurrentState->ProxyResolvingRequestCompleteL(*this,aProxies);
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::ProxyResolvingRequestFailed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileContextBase::ProxyResolvingRequestFailed(TInt aError)
	{
	PROFILE_DEBUG4("CSIPProfileContextBase::ProxyResolvingRequestFailed"\
		,ProfileId(), aError)
	iCurrentState->ProxyResolvingRequestFailed(*this,aError);
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileContextBase::HandleProxyResolvingError()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileContextBase::HandleProxyResolvingError(TInt aError)
    {
	PROFILE_DEBUG4("CSIPProfileContextBase::HandleProxyResolvingError"\
		,ProfileId(), aError)
	//If backup proxy is configured fall back to the backup proxy
	//if its not previously tried(i.e, when iUseBackupProxy is false)
	if(!iUseBackupProxy && !iRetryPossible && iProfile)
		{
		if(iProfile->Server(KSIPOutboundProxy).Length() > 0)
			{
			TUriParser8 parser;
			if (KErrNone == parser.Parse(iProfile->Server(KSIPOutboundProxy)))
				{
				iUseBackupProxy = (parser.Extract(EUriHost).CompareF(KDynamicProxy) != 0);
				}
			}
		if(iUseBackupProxy)
			{
			SetNextState(*iInitState);
			TRAPD(err, iCurrentState->RegisterL(*this));
			if(err == KErrNone)
				{
				return;
				}
			}
		}
	iUseBackupProxy = EFalse;
	iRetryPossible = EFalse;
	delete iProxies;
	iProxies = 0;
	iResolvingCompleted = ETrue;
	iDynamicProxyCount = 0;
	CSIPConcreteProfile* profile = iProfile;
	ForgetProfile();
	DestroyRegistration();
	iObserver.SIPProfileErrorEvent(*profile,aError);
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileContextBase::IgnoreErrorEvent
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPProfileContextBase::IgnoreErrorEvent()
	{
	PROFILE_DEBUG4("CSIPProfileContextBase::IgnoreErrorEvent", ProfileId()\
		, iRetryPossible)
	return iRetryPossible;
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileContextBase::TerminateHandling
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileContextBase::TerminateHandling()
	{
	PROFILE_DEBUG1("CSIPProfileContextBase::TerminateHandling")
	ForgetProfile();
	DestroyRegistration();
	}	

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::ErrorOccured()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileContextBase::ErrorOccured(
	TInt aError,
	CSIPRegistrationBinding& aRegistration,
	TBool& aHandled)
	{
	if (iRegistration && aRegistration==*iRegistration)
		{
		PROFILE_DEBUG3("CSIPProfileContextBase::ErrorOccured", ProfileId())
		if ( !RetryRegister( NULL,  aError) )
			{
			RetryPossible(aError);
			}
		aHandled = ETrue;
		iCurrentState->ErrorOccured(*this, aError);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::ErrorOccured()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileContextBase::ErrorOccured(
	TInt aError,
	CSIPClientTransaction& aTransaction,
	CSIPRegistrationBinding& aRegistration,
	TBool& aHandled)
	{
	if (iRegistration && 
		aRegistration==*iRegistration)
		{
		PROFILE_DEBUG3("CSIPProfileContextBase::ErrorOccured", ProfileId())
		if ( !RetryRegister( &aTransaction,  aError) )
			{
			RetryPossible(aError);
			}
		aHandled = ETrue;
		iCurrentState->ErrorOccured(*this, aError);
		}
	}	

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::ConnectionStateChanged
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileContextBase::ConnectionStateChanged(
	CSIPConnection::TState aState)
	{
	PROFILE_DEBUG4("CSIPProfileContextBase::ConnectionStateChanged"
		,ProfileId(), aState)
	PROFILE_DEBUG3("ConnectionStateChanged, current state"
		,iCurrentState->Name())

    ConnectionStateChangedImpl(aState);

    iCurrentState->ConnectionStateChanged(*this, aState);
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileContextBase::RequireProxyResolving
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPProfileContextBase::RequireProxyResolving()
	{
	PROFILE_DEBUG1("CSIPProfileContextBase::RequireProxyResolving")
	return (iProfile != 0 && IsProxyResolvingEnabled() && !iResolvingCompleted);
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileContextBase::ProxyAddressL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPProfileContextBase::ProxyAddressL()
	{
	if (iProfile)
		{
		return ProxyAddressL(*iProfile);
		}
	else
		{
		return KNullDesC8;
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::ProxyAddressL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPProfileContextBase::ProxyAddressL(
	CSIPConcreteProfile& aProfile)
	{
	if (iProfile && IsProxyResolvingEnabled())
		{
		return iProfile->DynamicProxy();
		}
	else
		{
		return aProfile.Server(KSIPOutboundProxy);
		}
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileContextBase::Received2XXRegisterResponse
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileContextBase::Received2XXRegisterResponse()
	{
	PROFILE_DEBUG1("CSIPProfileContextBase::Received2XXRegisterResponse")
	iRetryPossible = EFalse;
	iResolvingCompleted = ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::RetryPossible
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPProfileContextBase::RetryPossible(TInt aError)
	{
	PROFILE_DEBUG4("CSIPProfileContextBase::RetryPossible", ProfileId(), aError)
	if (iProfile && IsProxyResolvingEnabled())
		{
		iDynamicProxyCount++;
		PROFILE_DEBUG3("RetryPossible, resolving enabled, count",\
			iDynamicProxyCount)
		if (ShouldRetryRegistration(aError))
			{
			if (iResolvingCompleted)
				{
				PROFILE_DEBUG1("RetryPossible = ETrue, re-registration")
				SetRetryPossible(ETrue);
				}
			else if ((iProxies && iDynamicProxyCount < iProxies->MdcaCount()) ||
			          (iProxies == NULL))
				{
				PROFILE_DEBUG1("RetryPossible = ETrue, initial-registration")
				iRetryPossible = ETrue;
				}
			else
				{
				PROFILE_DEBUG1("RetryPossible = EFalse, initial-registration")
				iRetryPossible = EFalse;
				}
			}
		else
			{
			PROFILE_DEBUG1("RetryPossible = EFalse, cleaning")
			iDynamicProxyCount = 0;
			iRetryPossible = EFalse;
			iResolvingCompleted = EFalse;
			}
		}
    else
        {
        iRetryPossible = EFalse;
        }
	return iRetryPossible;
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::SetRetryPossible
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPProfileContextBase::SetRetryPossible(TBool aRetryPossible)
    {
    iRetryPossible = aRetryPossible;
    }

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::ProfileId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
EXPORT_C TUint32 CSIPProfileContextBase::ProfileId() const
	{
	if (iProfile)
		{
		return iProfile->Id();
		}
	else
		{
		return 0;
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::IsProxyResolvingEnabled()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPProfileContextBase::IsProxyResolvingEnabled() const
	{	
	TBool enabled = EFalse; 
	if(!iUseBackupProxy && iProfile)
		{
		iProfile->ExtensionParameter(KSIPResolveProxy,enabled);
		if (!enabled && iProfile->Server(KSIPOutboundProxy).Length() > 0)
			{
			TUriParser8 parser;
			if (KErrNone == parser.Parse(iProfile->Server(KSIPOutboundProxy)))
				{
				enabled = (parser.Extract(EUriHost).CompareF(KDynamicProxy) == 0);
				}
			}
		}
    return enabled;
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileContextBase::AddIntoQueue()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPProfileContextBase::AddIntoQueue( const TDesC8& aRegistrar )
	{
	TBool result = EFalse;
	if ( iCurrentState->IsRegisterPending() )
		{
		result = (Profile()->Server(KSIPRegistrar).Compare( aRegistrar ) == 0);
		}
	return result;
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::AddDeregisterIntoQueue()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPProfileContextBase::AddDeregisterIntoQueue()
	{
	return iCurrentState->IsRegisterPending();
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::IsIdle
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPProfileContextBase::IsIdle()
    {
	PROFILE_DEBUG1("CSIPProfileContextBase::IsIdle")    
    return iCurrentState->IsIdle( *this );
    } 

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::SetCredentials
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPProfileContextBase::SetCredentials(
    const CSIPClientTransaction& aTransaction,
    CSIPHttpDigest& aDigest)
    {
    TBool handled = EFalse;
    const CSIPResponseElements* response = aTransaction.ResponseElements();
    
    if (HasTransaction(aTransaction) && response)
        {
        handled = ETrue;
        SetCredentialsImplementation(aTransaction,*response,aDigest);
        }
    return handled;
    }

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::SetCredentials
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPProfileContextBase::SetCredentials(
    const CSIPRefresh& aRefresh,
    CSIPHttpDigest& aDigest)
    {
    TBool handled = EFalse;
    const CSIPClientTransaction* ta = aRefresh.SIPTransaction();
    
    if (HasRefresh(aRefresh) && ta && ta->ResponseElements())
        {
        handled = ETrue;
        const CSIPResponseElements* response = ta->ResponseElements();
        SetCredentialsImplementation(aRefresh,*response,aDigest);
        }
    return handled;
    }

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::ProxiesAlreadyResolved()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPProfileContextBase::ProxiesAlreadyResolved()
    {
    return (iProxies && iProxies->MdcaCount() > 0);
    }

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::SetProxiesL()
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPProfileContextBase::SetProxiesL(MDesC8Array& aProxies)
    {
    __ASSERT_ALWAYS(aProxies.MdcaCount() > 0, User::Leave(KErrArgument));

    CDesC8ArrayFlat* tmpArray = new (ELeave) CDesC8ArrayFlat(1);
    CleanupStack::PushL(tmpArray);

    for (TInt i = 0; i < aProxies.MdcaCount(); i++)
        {
        tmpArray->AppendL(aProxies.MdcaPoint(i));
        }

    CleanupStack::Pop(tmpArray);
    
    delete iProxies;
	iProxies = tmpArray;
	
	ConstructDynamicProxyL(iProxies->MdcaPoint(0));
	
	iDynamicProxyCount = 0;   
    }

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::HasTransaction
// The default base class implementation
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPProfileContextBase::HasTransaction(
    const CSIPClientTransaction& aTransaction) const
    {
    if (iClientTx && 
        aTransaction == *iClientTx)
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::HasRefresh
// The default base class implementation
// -----------------------------------------------------------------------------
//    
EXPORT_C TBool CSIPProfileContextBase::HasRefresh(
    const CSIPRefresh& aRefresh) const
    {
    if (iRegistration && 
        iRegistration->SIPRefresh() &&
        aRefresh == *(iRegistration->SIPRefresh()))
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::RegistrarUsername
// The default base class implementation
// -----------------------------------------------------------------------------
//   
EXPORT_C const TDesC8& CSIPProfileContextBase::RegistrarUsername() const
    {
    if (iProfile)
        {
        return iProfile->ServerParameter(KSIPRegistrar,KSIPDigestUserName);
        }
    return KNullDesC8;
    }


// -----------------------------------------------------------------------------
// CSIPProfileContextBase::RegisterDynamicL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPProfileContextBase::RegisterDynamicL(MDesC8Array& aProxies)
	{
	__ASSERT_ALWAYS(IsProxyResolvingEnabled(),
		User::Leave(KErrArgument));
	__ASSERT_ALWAYS(aProxies.MdcaCount() > iDynamicProxyCount, 
		User::Leave(KErrArgument));

	ConstructDynamicProxyL(aProxies.MdcaPoint(iDynamicProxyCount));
	iCurrentState->RegisterL(*this);
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::ConstructDynamicProxyL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPProfileContextBase::ConstructDynamicProxyL(const TDesC8& aProxy)
	{
	__ASSERT_ALWAYS(iProfile, User::Leave(KErrNotReady));
	
	TPtrC8 uriDesPtr;
	if (iProfile->Server(KSIPOutboundProxy).Length() > 0)
		{
		uriDesPtr.Set(iProfile->Server(KSIPOutboundProxy));
		}
	else
		{
		uriDesPtr.Set(KTmpSipUri);
		}
    TUriParser8 uriParser;
    User::LeaveIfError(uriParser.Parse(uriDesPtr));
    CUri8* uri = CUri8::NewLC(uriParser);
    uri->SetComponentL(aProxy,EUriHost);
	iProfile->SetDynamicProxyL(uri->Uri().UriDes());
	CleanupStack::PopAndDestroy(uri);	
		
	PROFILE_DEBUG6("CSIPProfileContextBase::ConstructDynamicProxyL", 
	               iProfile->DynamicProxy())
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileContextBase::ClearProxyResolving()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPProfileContextBase::ClearProxyResolving()
	{
	PROFILE_DEBUG1("CSIPProfileContextBase::ClearProxyResolving")
	iDynamicProxyCount = 0;
	iRetryPossible = ETrue;
	iResolvingCompleted = EFalse;
	delete iProxies;
	iProxies = 0;
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileContextBase::ResetRegisteredAORsL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
void CSIPProfileContextBase::ResetRegisteredAORsL()
    {
    if (iProfile)
        {
        CDesC8ArrayFlat* array = new(ELeave)CDesC8ArrayFlat(1);
	    CleanupStack::PushL(array);
	    iProfile->SetRegisteredAORsL(*array);
	    CleanupStack::PopAndDestroy(array);
	    iProfile->SetExtensionParameterL(KSIPRegisteredContact,KNullDesC8);   
        }
    }

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::SetCredentialsImplementation 
// -----------------------------------------------------------------------------
//
template<class T> void CSIPProfileContextBase::SetCredentialsImplementation(
    const T& aChallengeTarget,
    const CSIPResponseElements& aResponse,
    CSIPHttpDigest& aDigest)
    {
    const RPointerArray<CSIPHeaderBase>& headers = 
        aResponse.MessageElements().UserHeaders();
        
    for (TInt i=0; i < headers.Count(); i++)
        {
        const CSIPHeaderBase* header = headers[i];
        if (header->Name() == iWwwAuthenticateHeaderName)
            {
            const CSIPWWWAuthenticateHeader* wwwAuthHeader =
                static_cast<const CSIPWWWAuthenticateHeader*>(header);
            TPtrC8 realm(wwwAuthHeader->DesParamValue(
                SIPStrings::StringF(SipStrConsts::ERealm)));
            TPtrC8 username(KNullDesC8);
            TPtrC8 password(KNullDesC8);
            if (GetRegistrarCredentials(username,password))
                {
                TRAPD(err, aDigest.SetCredentialsL(aChallengeTarget,
                                                   KNullDesC8,
                                                   realm,
                                                   username,
                                                   password));
                if (err)
                    {
                    aDigest.IgnoreChallenge(aChallengeTarget,realm);
                    }
                }
            else
                {
                aDigest.IgnoreChallenge(aChallengeTarget,realm);
                }
            }
        else if (header->Name() == iProxyAuthenticateHeaderName)
            {
            const CSIPProxyAuthenticateHeader* proxyAuthHeader =
                static_cast<const CSIPProxyAuthenticateHeader*>(header);
            TPtrC8 realm = proxyAuthHeader->DesParamValue(
                SIPStrings::StringF(SipStrConsts::ERealm));
            TPtrC8 proxyHost(KNullDesC8);
            TPtrC8 username(KNullDesC8);
            TPtrC8 password(KNullDesC8);
            if (GetProxyCredentials(proxyHost,username,password))
                {
                TRAPD(err, aDigest.SetCredentialsL(aChallengeTarget,
                                                   proxyHost,
                                                   realm,
                                                   username,
                                                   password));
                if (err)
                    {
                    aDigest.IgnoreChallenge(aChallengeTarget,realm);
                    }
                }
            else
                {
                aDigest.IgnoreChallenge(aChallengeTarget,realm);
                }
            }
        else
            {
            // Not WWW-/Proxy-Authenticate-header: Do nothing.
            }
        }
	}

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::GetRegistrarCredentials
// -----------------------------------------------------------------------------
//
TBool CSIPProfileContextBase::GetRegistrarCredentials(
    TPtrC8& aUsername,
    TPtrC8& aPassword) const
    {
    aUsername.Set(KNullDesC8);
    aPassword.Set(KNullDesC8);
    if (iProfile)
        {    
        aUsername.Set(RegistrarUsername()); 
        
        if (aUsername.Length() == 0)
            {
            // To be backwards compatible, 
            // check also proxy settings if registrar settings not found
            aUsername.Set(
                iProfile->ServerParameter(KSIPOutboundProxy,KSIPDigestUserName));
            aPassword.Set(
                iProfile->ServerParameter(KSIPOutboundProxy,KSIPDigestPassword));
            }
        else
            {
            aPassword.Set(
                iProfile->ServerParameter(KSIPRegistrar,KSIPDigestPassword));
            }        
        }
    return (aUsername.Length() && aPassword.Length());
    }
 
 // -----------------------------------------------------------------------------
// CSIPProfileContextBase::GetProxyCredentials
// -----------------------------------------------------------------------------
//
TBool CSIPProfileContextBase::GetProxyCredentials(
    TPtrC8& aProxyHost,
    TPtrC8& aUsername,
    TPtrC8& aPassword) const
    {
    aProxyHost.Set(KNullDesC8);
    aUsername.Set(KNullDesC8);
    aPassword.Set(KNullDesC8);
    if (iProfile)
        {
        aProxyHost.Set(ExtractProxyHost());
                
        aUsername.Set(
            iProfile->ServerParameter(KSIPOutboundProxy,KSIPDigestUserName));
    
        if (aUsername.Length() == 0)
            {
            // To be backwards compatible, 
            // check also registrar settings if proxy settings not found
            aUsername.Set(RegistrarUsername());          
            aPassword.Set(
                iProfile->ServerParameter(KSIPRegistrar,KSIPDigestPassword));
            }
        else
            {
            aPassword.Set(
                iProfile->ServerParameter(KSIPOutboundProxy,KSIPDigestPassword));
            }
        }
    return (aProxyHost.Length() && aUsername.Length() && aPassword.Length());
    }

// -----------------------------------------------------------------------------
// CSIPProfileContextBase::ExtractProxyHost
// -----------------------------------------------------------------------------
//    
TPtrC8 CSIPProfileContextBase::ExtractProxyHost() const
    {
    TPtrC8 proxyHost(KNullDesC8);
    if (iProfile)
        {
        TPtrC8 proxy(iProfile->Server(KSIPOutboundProxy));
        if (proxy.Length() > 0)
            {
            TUriParser8 parser;
            // If parsing fails, proxyHost will be left empty.
            // The empty value can be considered as a failure situation
            // in the calling function and handled there.
            parser.Parse(proxy);
            if (parser.Extract(EUriHost).Compare(KDynamicProxy) == 0)
                {
                parser.Parse(iProfile->DynamicProxy());
                }
            proxyHost.Set(parser.Extract(EUriHost));
            }
        }
    return proxyHost; 
    }
