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
// Name        : sipprflregisterdelayrequested.cpp
// Part of     : sip profile fsm
// implementation
// Version     : %version: 3.1.1 %
//



// INCLUDE FILES
#include	"sipprflregisterdelayrequestedstate.h"
#include	"sipconcreteprofile.h"
#include	"sipmessageelements.h"
#include	"sipregistrationbinding.h"
#include	"sipconnection.h"
#include	"sipclienttransaction.h"
#include	"sipprofileagentobserver.h"
#include    "SipProfileLog.h"
#include    "sipprofilefsmuser.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPPrflRegisterDelayRequestedState::NewL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPPrflRegisterDelayRequestedState* 
CSIPPrflRegisterDelayRequestedState::NewL(MSIPProfileFSMUser& aUser)
	{
	return new (ELeave) CSIPPrflRegisterDelayRequestedState(aUser);
	}

// -----------------------------------------------------------------------------
// CSIPPrflRegisterDelayRequestedState::CSIPPrflRegisterDelayRequestedState()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPPrflRegisterDelayRequestedState::CSIPPrflRegisterDelayRequestedState(
    MSIPProfileFSMUser& aUser)
    : CSIPPrflStateBase(aUser,MSIPProfileContext::EDelayedRegisterRequested)
	{
	}

// -----------------------------------------------------------------------------
// CSIPPrflRegisterDelayRequestedState::~CSIPPrflRegisterDelayRequestedState()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPPrflRegisterDelayRequestedState::~CSIPPrflRegisterDelayRequestedState()
	{
	}

// -----------------------------------------------------------------------------
// CSIPPrflRegisterDelayRequestedState::LinkStates()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPPrflRegisterDelayRequestedState::LinkStates(
    CSIPPrflStateBase& aInitState)
	{
	iInitState = &aInitState;
	}

// -----------------------------------------------------------------------------
// CSIPPrflRegisterDelayRequestedState::ConcreteProfileState()
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::TStatus 
CSIPPrflRegisterDelayRequestedState::ConcreteProfileState() const
    {
    return CSIPConcreteProfile::ERegistrationInProgress;
    }

// -----------------------------------------------------------------------------
// CSIPPrflRegisterDelayRequestedState::DeregisterL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflRegisterDelayRequestedState::DeregisterL(
	MSIPProfileContext& aContext)
	{
	PROFILE_DEBUG1("CSIPPrflRegisterDelayRequestedState::DeregisterL")
	__ASSERT_DEBUG(aContext.Profile()!=0, User::Invariant());
	
	CSIPConcreteProfile& profile = *aContext.Profile();
	TUint32 contextId = 0;
	
	aContext.ForgetProfile();
	
	// aContext.ForgetProfile() does not set profile to NULL since
	// iRetryPossible is ETrue. so must set profile to NULL here.
	// because soon the profile will be deleted from function
	// aItem.ClearOldProfile
    aContext.SetProfile(NULL);
    aContext.SetTransaction(NULL);	
	aContext.RemoveDeltaTimerEntry();
	
	CSIPRegistrationBinding* regBinding = aContext.Registration();
	if ( regBinding )
	    {
	    contextId = aContext.Registration()->ContextId();
	    aContext.DestroyRegistration();
	    }
	    
	aContext.SetNextState(*iInitState);
	aContext.AgentObserver().SIPProfileStatusEvent(profile,contextId);
	}

// -----------------------------------------------------------------------------
// CSIPPrflRegisterDelayRequestedState::ChangeRegistrationStateL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflRegisterDelayRequestedState::ChangeRegistrationStateL(
	MSIPProfileContext& aContext)
	{
	PROFILE_DEBUG1(
	    "CSIPPrflRegisterDelayRequestedState::ChangeRegistrationStateL")
	TUint32	contextId(0);
	CSIPConcreteProfile& profile = DoCleanup(aContext,contextId);
	aContext.SetNextState(*iInitState);
	iUser.RetryProfileRegistrationL(profile);
	}

// -----------------------------------------------------------------------------
// CSIPPrflRegisterDelayRequestedState::RegisterL()
// -----------------------------------------------------------------------------
//
void CSIPPrflRegisterDelayRequestedState::RegisterL(
					MSIPProfileContext& aContext)
	{
	aContext.Connection().RefreshConnection();	
	}

void CSIPPrflRegisterDelayRequestedState::ConnectionStateChanged(
		MSIPProfileContext& aContext,CSIPConnection::TState aState)
	{
	switch(aState)
 		{
		case CSIPConnection::EActive:
			{
			aContext.RemoveDeltaTimerEntry();
			TRAPD(err, ChangeRegistrationStateL(aContext));		
			if (err)
				{
				HandleError(aContext, err, iInitState);
				}
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
		default:break;
		}

	}
