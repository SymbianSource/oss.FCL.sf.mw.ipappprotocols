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
// Name        : sipprflregistrationinprogressstate.cpp
// Part of     : sip profile fsm
// implementation
// Version     : %version: 2.1.2 %
//



// INCLUDE FILES
#include "sipprflregistrationinprogressstate.h"
#include "sipprofileagentobserver.h"
#include "sipclienttransaction.h"
#include "sipresponseelements.h"
#include "sipconcreteprofile.h"
#include "sipregistrationbinding.h"
#include "siperr.h"
#include "sipgendefs.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPPrflRegistrationInProgressState::CSIPPrflRegistrationInProgressState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPPrflRegistrationInProgressState::CSIPPrflRegistrationInProgressState(
    MSIPProfileFSMUser& aUser)
    : CSIPPrflStateBase(aUser,MSIPProfileContext::ERegistrationInProgress)
	{
	}

// -----------------------------------------------------------------------------
// CSIPPrflRegistrationInProgressState::CSIPImcPaInit
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C 
CSIPPrflRegistrationInProgressState::~CSIPPrflRegistrationInProgressState()
	{
	}

// -----------------------------------------------------------------------------
// CSIPPrflRegistrationInProgressState::NewL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPPrflRegistrationInProgressState* 
CSIPPrflRegistrationInProgressState::NewL(MSIPProfileFSMUser& aUser)
	{
	return new (ELeave) CSIPPrflRegistrationInProgressState(aUser);
	}

// -----------------------------------------------------------------------------
// CSIPPrflRegistrationInProgressState::LinkStates()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPPrflRegistrationInProgressState::LinkStates(
	CSIPPrflStateBase& aRegisterRequestedState,
	CSIPPrflStateBase& aInitState,
	CSIPPrflStateBase& aRegisteredState,
	CSIPPrflStateBase& aRegisterDelayRequestedState)
	{
	iRegisterRequestedState = &aRegisterRequestedState;
	iInitState = &aInitState;
	iRegisteredState = &aRegisteredState;
	iRegisterDelayRequestedState = &aRegisterDelayRequestedState;
	}
	
// -----------------------------------------------------------------------------
// CSIPPrflRegistrationInProgressState::ConcreteProfileState()
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::TStatus 
CSIPPrflRegistrationInProgressState::ConcreteProfileState() const
    {
    return CSIPConcreteProfile::ERegistrationInProgress;
    }	
	
// -----------------------------------------------------------------------------
// CSIPPrflRegistrationInProgressState::ResponseReceivedL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflRegistrationInProgressState::ResponseReceived(
    MSIPProfileContext& aContext,
	CSIPClientTransaction& aTransaction)
	{
	__ASSERT_DEBUG(aTransaction.ResponseElements()!=0, User::Invariant());
	__ASSERT_DEBUG(aContext.Profile()!=0, User::Invariant());
	__ASSERT_DEBUG(aContext.Registration()!=0, User::Invariant());
	TUint status = aTransaction.ResponseElements()->StatusCode();
	if (status >= K200Ok && status < K300MultipleChoices)
		{
		TInt err = KErrNone;
		if (!aContext.AgentObserver().ProceedRegistration(
		        *aContext.Profile(), KErrNone))
			{
			aContext.SetNextState(*iRegisteredState);
			TRAP(err, iRegisteredState->DeregisterL(aContext));
			}
	    else
	        {
		    err = aContext.SetRegisteredAORs();
		    if (err == KErrNone)
		        {
		        TRAP(err, aContext.SetNegotiatedSecurityMechanismL());
		        if (err == KErrNone)
		            {
		            aContext.SetTransaction(0);
		            aContext.SetNextState(*iRegisteredState);
		            aContext.AgentObserver().SIPProfileStatusEvent(
		                *aContext.Profile(),
		                aContext.Registration()->ContextId());
		            }
		        }
	        }
        if (err)
            {
            HandleError(aContext, err, iInitState);
            }
		}
	else if (status >= K300MultipleChoices)
		{
		HandleError(aContext, status, iInitState);
		}
	else
		{
		//1xx responses are not propagated
		}
	}

// -----------------------------------------------------------------------------
// CSIPPrflRegistrationInProgressState::ErrorOccured()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflRegistrationInProgressState::ErrorOccured(
	MSIPProfileContext& aContext,
	TInt aError)
	{
	__ASSERT_DEBUG(aContext.Profile()!=0, User::Invariant());
	__ASSERT_DEBUG(aContext.Registration()!=0, User::Invariant());
	CSIPConcreteProfile& profile = *aContext.Profile();
	if ( aContext.RetryAfterTimer() )
		{
		//registration failed , re-try later
		aContext.SetTransaction(0);
		TUint32	contextId(aContext.Registration()->ContextId());
		if( aContext.RetryTimerInUse() )
			{
			aContext.SetNextState(*iRegisterDelayRequestedState);
			}
		else
			{
			DoCleanup(aContext,contextId);
			aContext.SetNextState(*iInitState);
			}
		aContext.AgentObserver().SIPProfileStatusEvent(profile,contextId);
		}
	else if ((aError == KErrSIPTransportFailure || 
	     aError == KErrSIPResolvingFailure ||
		 aError == KErrSIPICMPFailure ||
		 aError == KErrSIPOutboundProxyNotResponding ) && 
	    aContext.Connection().State()==CSIPConnection::ESuspended)
		{
		//registration failed due to the suspended connection, re-try later
		aContext.SetTransaction(0);
		aContext.SetNextState(*iRegisterRequestedState);
		aContext.AgentObserver().SIPProfileStatusEvent(
		    profile, aContext.Registration()->ContextId());
		}
	else if (aContext.RetryTimerInUse())
		{
		aContext.SetNextState(*iRegisterDelayRequestedState);
		aContext.AgentObserver().SIPProfileErrorEvent(
		    profile,aContext.Registration()->ContextId());
		}
	else
		{
		HandleError(aContext,aError,iInitState);
		}
	}

// -----------------------------------------------------------------------------
// CSIPPrflRegistrationInProgressState::ConnectionStateChanged()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflRegistrationInProgressState::ConnectionStateChanged(
	MSIPProfileContext& aContext,
	CSIPConnection::TState aState)
	{
	//only EInactive or ESuspended are possible
	if (aState==CSIPConnection::EInactive)
		{
		__ASSERT_DEBUG(aContext.Profile()!=0, User::Invariant());
		if (aContext.AgentObserver().ProceedRegistration(*aContext.Profile(), KErrNone))
			{
			aContext.SetTransaction(0); //transactions are deleted in the SIP stack
			aContext.SetNextState(*iRegisterRequestedState);
			__ASSERT_DEBUG(aContext.Registration()!=0, User::Invariant());
			aContext.AgentObserver().SIPProfileStatusEvent(
		    	*aContext.Profile(),
		    	aContext.Registration()->ContextId());
			}
		else
			{
			HandleError(aContext,KErrGeneral,iInitState);
			}
		
		}
	/* When State EUnavailable is received, the registration Binding in the SIPStack 
	 * is no longer present. Hence we have to do the cleanup here and propagate the 
	 * error back to clients of profiles.
	 */
    else if(aState==CSIPConnection::EUnavailable)
		{
		HandleError(aContext, KErrCouldNotConnect, iInitState);
		}
    else
    	{
    	// make pclint happy
    	}
	}
	
// -----------------------------------------------------------------------------
// CSIPPrflRegistrationInProgressState::IsRegisterPending()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPPrflRegistrationInProgressState::IsRegisterPending() const
    {
    return ETrue;
    }
