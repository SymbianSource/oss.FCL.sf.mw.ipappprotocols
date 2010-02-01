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
// Name        : sipprflregisterrequested.cpp
// Part of     : sip profile fsm
// implementation
// Version     : %version: 2.1.1 %
//



// INCLUDE FILES
#include	"sipprflregisterrequestedstate.h"
#include	"sipmessageelements.h"
#include	"sipregistrationbinding.h"
#include	"sipconnection.h"
#include	"sipclienttransaction.h"
#include	"sipprofileagentobserver.h"
#include    "sipprofilefsmuser.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPPrflRegisterRequestedState::NewL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPPrflRegisterRequestedState* CSIPPrflRegisterRequestedState::NewL(
    MSIPProfileFSMUser& aUser)
	{
	return new (ELeave) CSIPPrflRegisterRequestedState(aUser);
	}

// -----------------------------------------------------------------------------
// CSIPPrflRegisterRequestedState::CSIPPrflRegisterRequestedState()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPPrflRegisterRequestedState::CSIPPrflRegisterRequestedState(
    MSIPProfileFSMUser& aUser)
    : CSIPPrflStateBase(aUser,MSIPProfileContext::ERegisterRequested)
	{
	}

// -----------------------------------------------------------------------------
// CSIPPrflRegisterRequestedState::~CSIPPrflRegisterRequestedState()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPPrflRegisterRequestedState::~CSIPPrflRegisterRequestedState()
	{
	}

// -----------------------------------------------------------------------------
// CSIPPrflRegisterRequestedState::LinkStates()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPPrflRegisterRequestedState::LinkStates(
    CSIPPrflStateBase& aInitState,
    CSIPPrflStateBase& aResolvingProxiesState,
    CSIPPrflStateBase& aRegistrationInProgressState)
	{
	iInit = &aInitState;
	iResolvingProxies = &aResolvingProxiesState;
	iRegistrationInProgress = &aRegistrationInProgressState;
	}

// -----------------------------------------------------------------------------
// CSIPPrflRegisterRequestedState::ConcreteProfileState()
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::TStatus 
CSIPPrflRegisterRequestedState::ConcreteProfileState() const
    {
    return CSIPConcreteProfile::ERegistrationInProgress;
    }

// -----------------------------------------------------------------------------
// CSIPPrflRegisterRequestedState::DeregisterL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflRegisterRequestedState::DeregisterL(
	MSIPProfileContext& aContext)
	{
	__ASSERT_DEBUG(aContext.Profile()!=0, User::Invariant());
	
	CSIPConcreteProfile* profile = aContext.Profile();
	TUint32 contextId = 0;
	
	aContext.ForgetProfile();
	
	CSIPRegistrationBinding* regBinding = aContext.Registration();
	if ( regBinding )
	    {
	    contextId = aContext.Registration()->ContextId();
	    aContext.DestroyRegistration();
	    }
	    
	aContext.SetNextState(*iInit);
	aContext.AgentObserver().SIPProfileStatusEvent(*profile,contextId);
	}

// -----------------------------------------------------------------------------
// CSIPPrflRegisterRequestedState::ConnectionStateChanged()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflRegisterRequestedState::ConnectionStateChanged(
	MSIPProfileContext& aContext,
	CSIPConnection::TState aState)
	{
	TInt err = KErrNone;
	switch(aState)
		{
		case CSIPConnection::EActive:
			{
			aContext.SetNextState(*iInit);
			TRAP(err, iUser.RegisterProfileL(*aContext.Profile()));
			if (err)
				{
				HandleError(aContext, err, iInit);
				}
			break;
			}
		/* When State EUnavailable is received, the registration Binding in the SIPStack 
		 * is no longer present. Hence we have to do the cleanup here and propagate the 
		 * error back to clients of profiles.
		 */
		case CSIPConnection::EUnavailable:
			{
			HandleError(aContext, KErrCouldNotConnect, iInit);
			break;
			}
		default:
			break;
		}
	}

// -----------------------------------------------------------------------------
// CSIPPrflRegisterRequestedState::RegisterL()
// -----------------------------------------------------------------------------
//
void CSIPPrflRegisterRequestedState::RegisterL(
					MSIPProfileContext& aContext)
	{
	aContext.Connection().RefreshConnection();	
	}
