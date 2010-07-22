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
// Name        : sipprflderegisterrequestedstate.cpp
// Part of     : sip profile fsm
// implementation
// Version     : %version: 2.1.2 %
//



// INCLUDE FILES
#include	"sipprflderegisterrequestedstate.h"
#include	"sipregistrationbinding.h"
#include	"sipclienttransaction.h"
#include	"sipprofileagentobserver.h"
#include    "sipprofilefsmuser.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPPrflDeregisterRequestedState::NewL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPPrflDeregisterRequestedState* 
CSIPPrflDeregisterRequestedState::NewL(MSIPProfileFSMUser& aUser)
	{
	return new (ELeave) CSIPPrflDeregisterRequestedState(aUser);
	}

// -----------------------------------------------------------------------------
// CSIPPrflDeregisterRequestedState::CSIPPrflDeregisterRequestedState()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPPrflDeregisterRequestedState::CSIPPrflDeregisterRequestedState(
    MSIPProfileFSMUser& aUser)
    : CSIPPrflStateBase(aUser,MSIPProfileContext::EDeregisterRequested)
	{
	}

// -----------------------------------------------------------------------------
// CSIPPrflDeregisterRequestedState::~CSIPPrflDeregisterRequestedState()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPPrflDeregisterRequestedState::~CSIPPrflDeregisterRequestedState()
	{
	}

// -----------------------------------------------------------------------------
// CSIPPrflDeregisterRequestedState::LinkStates()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPPrflDeregisterRequestedState::LinkStates(
	CSIPPrflStateBase& aInitState,
	CSIPPrflStateBase& aRegisteredState,
	CSIPPrflStateBase& aDeregistrationInProgressState)
	{
	iInitState = &aInitState;
	iRegisteredState = &aRegisteredState;
	iDeregistrationInProgressState = &aDeregistrationInProgressState;
	}

// -----------------------------------------------------------------------------
// CSIPPrflDeregisterRequestedState::ConcreteProfileState()
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::TStatus 
CSIPPrflDeregisterRequestedState::ConcreteProfileState() const
    {
    return CSIPConcreteProfile::EUnregistrationInProgress;
    }

// -----------------------------------------------------------------------------
// CSIPPrflDeregisterRequestedState::Deregister()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflDeregisterRequestedState::RegisterL(
	MSIPProfileContext& aContext)
	{
	__ASSERT_DEBUG(aContext.Profile()!=0, User::Invariant());
	aContext.SetNextState(*iRegisteredState);
	}

// -----------------------------------------------------------------------------
// CSIPPrflDeregisterRequestedState::DeregisterL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflDeregisterRequestedState::DeregisterL(
    MSIPProfileContext& aContext)
    {
    __ASSERT_DEBUG(aContext.Profile()!=0, User::Invariant());
    __ASSERT_DEBUG(aContext.Registration()!=0, User::Invariant());
    if (aContext.Connection().State()==CSIPConnection::EActive)
        {
        if (iUser.AddProfileIntoQueue(*aContext.Profile())) //compares the registrar of the present profile with the other profiles existing in the connection context array.
            {
            iUser.DeregisterProfileL(*aContext.Profile()); 
            
            } 
        else
            {
            CSIPMessageElements* elements = 
                aContext.CreateDeRegisterElementsL();
            CleanupStack::PushL(elements);
            CSIPClientTransaction* tx = 
                aContext.Registration()->DeregisterL(elements);
            CleanupStack::Pop(elements);
            aContext.SetTransaction(tx);
            aContext.SetNextState(*iDeregistrationInProgressState);
            }
        }
    aContext.AgentObserver().SIPProfileStatusEvent(
    *aContext.Profile(),
    aContext.Registration()->ContextId());
    }


// -----------------------------------------------------------------------------
// CSIPPrflDeregisterRequestedState::ErrorOccured()
// This can happen if the de-register was reguested while the connection is
// suspended (registered->de-register requested). SIP stack has removed
// the registration binding, so NOP and transition to Init state.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflDeregisterRequestedState::ErrorOccured(
	MSIPProfileContext& aContext,
	TInt aError)
	{
	//registration was dropped due to the expired refresh while connection is suspended
	HandleError(aContext, aError, iInitState);
	}

// -----------------------------------------------------------------------------
// CSIPPrflDeregisterRequestedState::ConnectionStateChanged()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflDeregisterRequestedState::ConnectionStateChanged(
	MSIPProfileContext& aContext,
	CSIPConnection::TState aState)
	{
	__ASSERT_DEBUG(aContext.Registration()!=0, User::Invariant());
	__ASSERT_DEBUG(aContext.Profile()!=0, User::Invariant());
	switch(aState)
		{
		case CSIPConnection::EActive:
			{
			aContext.SetNextState(*iRegisteredState);
			TRAPD(err, iUser.DeregisterProfileL(*aContext.Profile()));
			if (err)
				{
				HandleError(aContext, err, iInitState);
				}
			break;
			}
		case CSIPConnection::EInactive:
			{
			TUint32 contextId(0);
			CSIPConcreteProfile& profile = DoCleanup(aContext,contextId);
			aContext.SetNextState(*iInitState);
			aContext.AgentObserver().SIPProfileStatusEvent(profile,contextId);
			break;
			}
		/* When State EUnavailable is received, the registration Binding in the SIPStack 
		 * is no longer present. Hence we have to do the cleanup here and propagate the 
		 * error back to clients of profiles.
		 */
		case CSIPConnection::EUnavailable:
			{
			HandleError(aContext, KErrCouldNotConnect, iInitState);
			break;
			}
		default:
			break;
		}

	}
