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
// Name        : sipprflinitstate.cpp
// Part of     : sip profile fsm
// implementation
// Version     : %version: 2.1.1 %
//



// INCLUDE FILES
#include	"sipprflinitstate.h"
#include	"sipprofileagentobserver.h"
#include	"sipmessageelements.h"
#include	"sipregistrationbinding.h"
#include	"sipclienttransaction.h"
#include	"sipprflinitstate.h"
#include	"sipprflregisterrequestedstate.h"
#include	"sipprflregistrationinprogressstate.h"
#include	"sipprflregisteredstate.h"
#include	"sipprflderegistrationinprogressstate.h"
#include	"sipprflderegisterrequestedstate.h"
#include	"sipprflstatebase.h"
#include    "sipprofilefsmuser.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPPrflInitState::NewL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPPrflInitState* CSIPPrflInitState::NewL(MSIPProfileFSMUser& aUser)
	{
	return new (ELeave) CSIPPrflInitState(aUser);
	}

// -----------------------------------------------------------------------------
// CSIPPrflInitState::CSIPPrflInitState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPPrflInitState::CSIPPrflInitState(MSIPProfileFSMUser& aUser) 
    : CSIPPrflStateBase(aUser,MSIPProfileContext::EInit)
	{
	}

// -----------------------------------------------------------------------------
// CSIPPrflInitState::CSIPPrflInitState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPPrflInitState::~CSIPPrflInitState()
	{
	}

// -----------------------------------------------------------------------------
// CSIPPrflInitState::LinkStates()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPPrflInitState::LinkStates(
    CSIPPrflStateBase& aResolvingProxiesState,
	CSIPPrflStateBase& aRegisterRequestedState,
	CSIPPrflStateBase& aRegistrationInProgressState)
	{
	iResolvingProxies = &aResolvingProxiesState;
	iRegisterRequested = &aRegisterRequestedState;
	iRegistrationInProgress = &aRegistrationInProgressState;
	}

// -----------------------------------------------------------------------------
// CSIPPrflInitState::ConcreteProfileState()
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::TStatus CSIPPrflInitState::ConcreteProfileState() const
    {
    return CSIPConcreteProfile::EUnregistered;
    }

// -----------------------------------------------------------------------------
// CSIPPrflInitState::RegisterL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflInitState::RegisterL(
	MSIPProfileContext& aContext)
	{
	__ASSERT_ALWAYS(
	    aContext.Connection().State() != CSIPConnection::EUnavailable, 
		User::Leave (KErrCouldNotConnect));
	__ASSERT_DEBUG(aContext.Profile()!=0, User::Invariant());
	
	if (aContext.IsProxyResolvingEnabled() && 
	    !aContext.ProxiesAlreadyResolved())
	    {
	    aContext.ResolveProxyL();
	    aContext.SetNextState(*iResolvingProxies);
	    }
	else
	    {
	    if (aContext.Connection().State() == CSIPConnection::EActive)
		    {
		    if (!aContext.Registration())
		        {
		        aContext.CreateRegistrationL();
		        }
		    CSIPMessageElements* elems = aContext.CreateMsgElementsLC();
		    aContext.SetTransaction(aContext.Registration()->RegisterL(elems));
		    if (elems)
			    {
			    CleanupStack::Pop(elems);
			    }
		    aContext.SetNextState(*iRegistrationInProgress);
		    }
	    else
		    {
		    aContext.SetNextState(*iRegisterRequested);
		    }
	    }
	}

// -----------------------------------------------------------------------------
// CSIPPrflInitState::IsIdle
// From MSIPProfileFSMUser
// -----------------------------------------------------------------------------
//
TBool CSIPPrflInitState::IsIdle(
    MSIPProfileContext& aContext)
    {
    if (!aContext.Profile())
    	{
    	return ETrue;
    	}
    return !( iUser.IsInQueue( *aContext.Profile() ) );
	}
