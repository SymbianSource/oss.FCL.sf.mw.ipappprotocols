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
// Name        : sipprflderegistrationinprogressstate.cpp
// Part of     : sip profile fsm
// implementation
// Version     : %version: 2.1.1 %
//



// INCLUDE FILES
#include "sipprflderegistrationinprogressstate.h"
#include "sipclienttransaction.h"
#include "sipresponseelements.h"
#include "sipconcreteprofile.h"
#include "sipregistrationbinding.h"
#include "sipprofileagentobserver.h"
#include "sipgendefs.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPPrflDeregistrationInProgressState::NewL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPPrflDeregistrationInProgressState* 
CSIPPrflDeregistrationInProgressState::NewL(MSIPProfileFSMUser& aUser)
	{
	return new(ELeave)CSIPPrflDeregistrationInProgressState(aUser);
	}

// -----------------------------------------------------------------------------
// CSIPPrflDeregistrationInProgressState::CSIPPrflDeregistrationInProgressState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPPrflDeregistrationInProgressState::CSIPPrflDeregistrationInProgressState(
    MSIPProfileFSMUser& aUser)
    : CSIPPrflStateBase(aUser,MSIPProfileContext::EDeregistrationInProgress)
	{
	}

// -----------------------------------------------------------------------------
// CSIPPrflDeregistrationInProgressState::CSIPPrflDeregistrationInProgressState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C 
CSIPPrflDeregistrationInProgressState::~CSIPPrflDeregistrationInProgressState()
	{
	}

// -----------------------------------------------------------------------------
// CSIPPrflDeregistrationInProgressState::LinkStates()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPPrflDeregistrationInProgressState::LinkStates(
	CSIPPrflStateBase& aInitState)
	{
	iInitState = &aInitState;
	}

// -----------------------------------------------------------------------------
// CSIPPrflDeregistrationInProgressState::ConcreteProfileState()
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::TStatus 
CSIPPrflDeregistrationInProgressState::ConcreteProfileState() const
    {
    return CSIPConcreteProfile::EUnregistrationInProgress;
    }

// -----------------------------------------------------------------------------
// CSIPPrflDeregistrationInProgressState::ResponseReceived()
// 3xx-6xx response to the de-register or internal transport error
// will remove the registration binding from the SIP stack. So there's
// no need to re-initiate the de-registration in case of failure during
// the suspend state.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflDeregistrationInProgressState::ResponseReceived(
			MSIPProfileContext& aContext,
			CSIPClientTransaction& aTransaction)
	{
	__ASSERT_DEBUG(aTransaction.ResponseElements()!=0, User::Invariant());
	__ASSERT_DEBUG(aContext.Profile()!=0, User::Invariant());
	TUint status = aTransaction.ResponseElements()->StatusCode();
	if (status >= K200Ok && status < K300MultipleChoices)
		{
		TUint32 contextId(0);
		CSIPConcreteProfile& profile = DoCleanup(aContext,contextId);
		TRAPD(err, aContext.ResetNegotiatedSecurityMechanismL());
		if (err)
		    {
		    HandleError(aContext,err,iInitState);
		    }
		else
		    {
		    aContext.SetNextState(*iInitState);
		    aContext.AgentObserver().SIPProfileStatusEvent(profile,contextId);
		    }
		}
	else if (status >= K300MultipleChoices)
		{
		HandleError(aContext,status,iInitState);
		}
	else
		{
		//1xx responses are not propagated
		}
	}

// -----------------------------------------------------------------------------
// CSIPPrflDeregistrationInProgressState::ErrorOccur()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflDeregistrationInProgressState::ErrorOccured(
	MSIPProfileContext& aContext,
	TInt aError)
	{
	HandleError(aContext, aError, iInitState);
	}

// -----------------------------------------------------------------------------
// CSIPPrflDeregistrationInProgressState::ConnectionStateChanged()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflDeregistrationInProgressState::ConnectionStateChanged(
	MSIPProfileContext& aContext,
	CSIPConnection::TState aState)
	{
	if (aState == CSIPConnection::EInactive)
		{
		if (aContext.AgentObserver().ProceedRegistration(*aContext.Profile(), KErrNone))
			{
			aContext.SetTransaction(0); //transactions are deleted in the SIP stack
			TUint32 contextId(0);
			CSIPConcreteProfile& profile = DoCleanup(aContext,contextId);
			aContext.SetNextState(*iInitState);
			aContext.AgentObserver().SIPProfileStatusEvent(profile,contextId);
			}
		else
			{
			HandleError(aContext, KErrGeneral, iInitState);
			}
		}
	/* When State EUnavailable is received, the registration Binding in the SIPStack 
	 * is no longer present. Hence we have to do the cleanup here and propagate the 
	 * error back to clients of profiles.
	 */
	else if(aState == CSIPConnection::EUnavailable)
		{
		HandleError(aContext, KErrCouldNotConnect, iInitState);
		}
	else
		{
		//makes pclint happy
		}
	}
	
// -----------------------------------------------------------------------------
// CSIPPrflDeregistrationInProgressState::IsRegisterPending()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPPrflDeregistrationInProgressState::IsRegisterPending() const
    {
    return ETrue;
    }

