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
// Name        : sipprflresolvingproxiesstate.cpp
// Part of     : sip profile fsm
// implementation
// Version     : %version: 2.1.1 %
//



// INCLUDE FILES
#include	"sipprflresolvingproxiesstate.h"
#include	"sipconcreteprofile.h"
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
// CSIPPrflResolvingProxiesState::NewL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPPrflResolvingProxiesState* CSIPPrflResolvingProxiesState::NewL(
    MSIPProfileFSMUser& aUser)
	{
	return new(ELeave)CSIPPrflResolvingProxiesState(aUser);
	}

// -----------------------------------------------------------------------------
// CSIPPrflResolvingProxiesState::CSIPPrflResolvingProxiesState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPPrflResolvingProxiesState::CSIPPrflResolvingProxiesState(
    MSIPProfileFSMUser& aUser) 
    : CSIPPrflStateBase(aUser,MSIPProfileContext::EProxyResolvingInProgress)
	{
	}

// -----------------------------------------------------------------------------
// CSIPPrflResolvingProxiesState::CSIPPrflResolvingProxiesState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPPrflResolvingProxiesState::~CSIPPrflResolvingProxiesState()
	{
	}

// -----------------------------------------------------------------------------
// CSIPPrflResolvingProxiesState::LinkStates()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPPrflResolvingProxiesState::LinkStates(
    CSIPPrflStateBase& aInitState,
	CSIPPrflStateBase& aRegisterRequestedState,
	CSIPPrflStateBase& aRegistrationInProgressState)
	{
	iInit = &aInitState;
	iRegisterRequested = &aRegisterRequestedState;
	iRegistrationInProgress = &aRegistrationInProgressState;
	}
	
// -----------------------------------------------------------------------------
// CSIPPrflResolvingProxiesState::ConcreteProfileState()
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::TStatus 
CSIPPrflResolvingProxiesState::ConcreteProfileState() const
    {
    return CSIPConcreteProfile::ERegistrationInProgress;
    }	
	
// -----------------------------------------------------------------------------
// CSIPPrflResolvingProxiesState::DeregisterL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflResolvingProxiesState::DeregisterL(
	MSIPProfileContext& aContext)
	{
	__ASSERT_DEBUG(aContext.Profile()!=0, User::Invariant());
	
	CSIPConcreteProfile& profile = *aContext.Profile();
	TUint32 contextId = 0;
	
	aContext.ForgetProfile();
	
	CSIPRegistrationBinding* regBinding = aContext.Registration();
	if ( regBinding )
	    {
	    contextId = aContext.Registration()->ContextId();
	    aContext.DestroyRegistration();
	    }
	    
	aContext.CancelProxyResolving();
	    
	aContext.SetNextState(*iInit);
	aContext.AgentObserver().SIPProfileStatusEvent(profile,contextId);
	}
	
// -----------------------------------------------------------------------------
// CSIPPrflResolvingProxiesState::ConnectionStateChanged()
// -----------------------------------------------------------------------------
//
void CSIPPrflResolvingProxiesState::ConnectionStateChanged(
	MSIPProfileContext& aContext,
	CSIPConnection::TState aState)
	{
	switch(aState)
		{
		case CSIPConnection::EActive:
		    {
		    TRAPD( err, aContext.ResolveProxyL() );
		    if ( err )
		        {
		        ProxyResolvingRequestFailed( aContext, err );
		        }
		    break;
		    }
		case CSIPConnection::EInactive:
		    {
            aContext.SetNextState(*iRegisterRequested);
            break;
		    }
		case CSIPConnection::EUnavailable:
		    {
			HandleError(aContext, KErrCouldNotConnect, iInit);
			break;
		    }
		default:
		    {
			break;
		    }
		}
	}	
	
// -----------------------------------------------------------------------------
// CSIPPrflResolvingProxiesState::ProxyResolvingRequestCompleteL()
// -----------------------------------------------------------------------------
//
void CSIPPrflResolvingProxiesState::ProxyResolvingRequestCompleteL(
    MSIPProfileContext& aContext,
    MDesC8Array& aProxies)
    {
    aContext.SetProxiesL(aProxies);
	if (aContext.Connection().State() == CSIPConnection::EActive)
		{
		aContext.SetNextState(*iInit);
		iUser.RegisterProfileL(*aContext.Profile());
		}
	else
		{
		if (aContext.Connection().State() == CSIPConnection::EUnavailable)
		    {
		    HandleError(aContext, KErrCouldNotConnect, iInit);
		    }
		else
		    {
		    aContext.SetNextState(*iRegisterRequested);
		    }
		}    
    }

// -----------------------------------------------------------------------------
// CSIPPrflResolvingProxiesState::ProxyResolvingRequestFailed()
// -----------------------------------------------------------------------------
//
void CSIPPrflResolvingProxiesState::ProxyResolvingRequestFailed(
    MSIPProfileContext& aContext,
    TInt aError)
    {
    aContext.SetNextState(*iInit);
    aContext.HandleProxyResolvingError(aError);
    }
