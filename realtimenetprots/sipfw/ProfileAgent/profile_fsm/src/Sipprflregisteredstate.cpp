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
// Name        : sipprflregisteredstate.cpp
// Part of     : sip profile fsm
// implementation
// Version     : %version: 2.1.3 %
//



// INCLUDE FILES
#include "sipprflregisteredstate.h"
#include "sipconcreteprofile.h"
#include "sipregistrationbinding.h"
#include "sipclienttransaction.h"
#include "sipresponseelements.h"
#include "sipmessageelements.h"
#include "sipconcreteprofilecomparison.h"
#include "sipprofile.h"
#include "siperr.h"
#include "sipprofileagentobserver.h"
#include "sipprofilefsmuser.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPPrflRegisteredState::NewL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPPrflRegisteredState* CSIPPrflRegisteredState::NewL(
    MSIPProfileFSMUser& aUser)
    {
    return new (ELeave) CSIPPrflRegisteredState(aUser);
    }

// -----------------------------------------------------------------------------
// CSIPPrflRegisteredState::CSIPPrflRegisteredState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPPrflRegisteredState::CSIPPrflRegisteredState(MSIPProfileFSMUser& aUser):
    CSIPPrflStateBase(aUser,MSIPProfileContext::ERegistered)
    {
    }

// -----------------------------------------------------------------------------
// CSIPPrflRegisteredState::~CSIPPrflRegisteredState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPPrflRegisteredState::~CSIPPrflRegisteredState()
    {
    }

// -----------------------------------------------------------------------------
// CSIPPrflRegisteredState::LinkStates()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPPrflRegisteredState::LinkStates(
    CSIPPrflStateBase& aDeregisterState,
    CSIPPrflStateBase& aDeregistrationInProgressState,
    CSIPPrflStateBase& aInitState,
    CSIPPrflStateBase& aRegisterRequested,
    CSIPPrflStateBase& aRegisterDelayRequested)
    {
    iDeregisterState = &aDeregisterState;
    iDeregistrationInProgressState = &aDeregistrationInProgressState;
    iInitState = &aInitState;
    iRegisterRequestedState = &aRegisterRequested;
    iRegisterDelayRequestedState = &aRegisterDelayRequested;
    }

// -----------------------------------------------------------------------------
// CSIPPrflRegisteredState::ConcreteProfileState()
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::TStatus 
CSIPPrflRegisteredState::ConcreteProfileState() const
    {
    return CSIPConcreteProfile::ERegistered;
    }

// -----------------------------------------------------------------------------
// CSIPPrflRegisteredState::DeregisterL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflRegisteredState::DeregisterL(
    MSIPProfileContext& aContext)
    {
    __ASSERT_DEBUG(aContext.Profile()!=0, User::Invariant());
    __ASSERT_DEBUG(aContext.Registration()!=0, User::Invariant());
    if (aContext.Connection().State()==CSIPConnection::EActive)
        {
        if (iUser.AddProfileIntoQueue(*aContext.Profile()))
            {
            aContext.SetNextState(*iDeregisterState);
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
    else
        {
        aContext.SetNextState(*iDeregisterState);
        }
    aContext.AgentObserver().SIPProfileStatusEvent(
        *aContext.Profile(),
        aContext.Registration()->ContextId());
    }

// -----------------------------------------------------------------------------
// CSIPPrflRegisteredState::UpdateL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflRegisteredState::UpdateL(
    MSIPProfileContext& aContext,
    CSIPConcreteProfile& aNewProfile)
    {
    __ASSERT_DEBUG(aContext.Profile()!=0, User::Invariant());
    
    CSIPConcreteProfile* oldProfile = aContext.Profile();
    
    if (SIPConcreteProfileComparison::ValueModified(
            CSIPConcreteProfile::EOutboundProxyParameters, 
            *oldProfile, aNewProfile) || 
        SIPConcreteProfileComparison::ValuesModified(
            CSIPConcreteProfile::EName|
            CSIPConcreteProfile::EOutboundProxyParameters, 
            *oldProfile, aNewProfile) ||
        SIPConcreteProfileComparison::ValuesModified(
            CSIPConcreteProfile::EName|
            CSIPConcreteProfile::EOutboundProxyParameters|
            CSIPConcreteProfile::EAutoRegistration, 
            *oldProfile, aNewProfile) ||
        SIPConcreteProfileComparison::ValuesModified(
            CSIPConcreteProfile::EOutboundProxyParameters|
            CSIPConcreteProfile::EAutoRegistration, 
            *oldProfile, aNewProfile))
        {
        aContext.SetProfile(&aNewProfile);
        }
    else if (SIPConcreteProfileComparison::ValueModified(
                CSIPConcreteProfile::ERegistrarParameters, 
                *oldProfile, aNewProfile) || 
             SIPConcreteProfileComparison::ValuesModified(
                 CSIPConcreteProfile::EName|
                 CSIPConcreteProfile::ERegistrarParameters, 
                 *oldProfile, aNewProfile) ||
             SIPConcreteProfileComparison::ValuesModified(
                 CSIPConcreteProfile::EName|
                 CSIPConcreteProfile::ERegistrarParameters|
                 CSIPConcreteProfile::EAutoRegistration, 
                 *oldProfile, aNewProfile) ||
             SIPConcreteProfileComparison::ValuesModified(
                 CSIPConcreteProfile::ERegistrarParameters|
                 CSIPConcreteProfile::EAutoRegistration,
                 *oldProfile, aNewProfile))
        {
        aContext.SetProfile(&aNewProfile);
        }
    else if (SIPConcreteProfileComparison::ValuesModified(
                CSIPConcreteProfile::ERegistrarParameters|
                CSIPConcreteProfile::EOutboundProxyParameters,
                *oldProfile, aNewProfile) || 
             SIPConcreteProfileComparison::ValuesModified(
                 CSIPConcreteProfile::EName|
                 CSIPConcreteProfile::ERegistrarParameters|
                 CSIPConcreteProfile::EOutboundProxyParameters,
                 *oldProfile, aNewProfile) ||
             SIPConcreteProfileComparison::ValuesModified(
                 CSIPConcreteProfile::EName|
                 CSIPConcreteProfile::ERegistrarParameters|
                 CSIPConcreteProfile::EOutboundProxyParameters|
                 CSIPConcreteProfile::EAutoRegistration, 
                 *oldProfile, aNewProfile) ||
             SIPConcreteProfileComparison::ValuesModified(
                 CSIPConcreteProfile::ERegistrarParameters|
                 CSIPConcreteProfile::EOutboundProxyParameters|
                 CSIPConcreteProfile::EAutoRegistration, 
                 *oldProfile, aNewProfile))
        {
        aContext.SetProfile(&aNewProfile);
        }
    else if (SIPConcreteProfileComparison::ValuesModified(
                 CSIPConcreteProfile::EContactHeaderParams|
                 CSIPConcreteProfile::EOutboundProxyParameters,
                 *oldProfile, aNewProfile) || 
             SIPConcreteProfileComparison::ValuesModified(
                 CSIPConcreteProfile::EName|
                 CSIPConcreteProfile::EContactHeaderParams|
                 CSIPConcreteProfile::EOutboundProxyParameters, 
                 *oldProfile, aNewProfile) ||
             SIPConcreteProfileComparison::ValuesModified(
                 CSIPConcreteProfile::EName|
                 CSIPConcreteProfile::EContactHeaderParams|
                 CSIPConcreteProfile::EOutboundProxyParameters|
                 CSIPConcreteProfile::EAutoRegistration, 
                 *oldProfile, aNewProfile) ||
             SIPConcreteProfileComparison::ValuesModified(
                 CSIPConcreteProfile::EContactHeaderParams|
                 CSIPConcreteProfile::EOutboundProxyParameters|
                 CSIPConcreteProfile::EAutoRegistration, 
                 *oldProfile, aNewProfile))
        {
        aContext.UpdateContactHeaderParamsL(aNewProfile);
        aContext.SetProfile(&aNewProfile);
        }
   else if (SIPConcreteProfileComparison::ValuesModified(
                CSIPConcreteProfile::EContactHeaderParams|
                CSIPConcreteProfile::ERegistrarParameters,
                *oldProfile, aNewProfile) || 
            SIPConcreteProfileComparison::ValuesModified(
                CSIPConcreteProfile::EName|
                CSIPConcreteProfile::EContactHeaderParams|
                CSIPConcreteProfile::ERegistrarParameters, 
                *oldProfile, aNewProfile) ||
            SIPConcreteProfileComparison::ValuesModified(
                CSIPConcreteProfile::EName|
                CSIPConcreteProfile::EContactHeaderParams|
                CSIPConcreteProfile::ERegistrarParameters|
                CSIPConcreteProfile::EAutoRegistration,
                *oldProfile, aNewProfile) ||
            SIPConcreteProfileComparison::ValuesModified(
                CSIPConcreteProfile::EContactHeaderParams|
                CSIPConcreteProfile::ERegistrarParameters|
                CSIPConcreteProfile::EAutoRegistration,
                *oldProfile, aNewProfile))
        {
        aContext.UpdateContactHeaderParamsL(aNewProfile);
        aContext.SetProfile(&aNewProfile);
        }            
    else if (SIPConcreteProfileComparison::ValueModified(
                 CSIPConcreteProfile::EContactHeaderParams,
                 *oldProfile, aNewProfile) || 
             SIPConcreteProfileComparison::ValuesModified(
                 CSIPConcreteProfile::EName|
                 CSIPConcreteProfile::EContactHeaderParams, 
                 *oldProfile, aNewProfile) ||
             SIPConcreteProfileComparison::ValuesModified(
                 CSIPConcreteProfile::EName|
                 CSIPConcreteProfile::EContactHeaderParams|
                 CSIPConcreteProfile::EAutoRegistration,
                 *oldProfile, aNewProfile) ||
             SIPConcreteProfileComparison::ValuesModified(
                 CSIPConcreteProfile::EContactHeaderParams|
                 CSIPConcreteProfile::EAutoRegistration,
                 *oldProfile, aNewProfile))
        {
        aContext.UpdateContactHeaderParamsL(aNewProfile);
        aContext.SetProfile(&aNewProfile);
        }        
    else if (SIPConcreteProfileComparison::ValueModified(
                 CSIPConcreteProfile::EContactHeaderParams,
                 *oldProfile, aNewProfile) || 
             SIPConcreteProfileComparison::ValuesModified(
                 CSIPConcreteProfile::EName,
                 *oldProfile, aNewProfile) ||
             SIPConcreteProfileComparison::ValuesModified(
                 CSIPConcreteProfile::EName|
                 CSIPConcreteProfile::EAutoRegistration,
                 *oldProfile, aNewProfile) ||
             SIPConcreteProfileComparison::ValuesModified(
                 CSIPConcreteProfile::EAutoRegistration,
                 *oldProfile, aNewProfile))
        {
        aContext.SetProfile(&aNewProfile);
        }
	else if((&aNewProfile)->IsDefault() && !(oldProfile)->IsDefault())
		{
		aContext.SetProfile(&aNewProfile);
		}        
    else
        {
        DeregisterL(aContext);
        }
    }

// -----------------------------------------------------------------------------
// CSIPPrflRegisteredState::ConnectionStateChanged()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflRegisteredState::ConnectionStateChanged(
    MSIPProfileContext& aContext,
    CSIPConnection::TState aState)
    {
    if (aState==CSIPConnection::EInactive)
        {
        //registration was cleared by the SIP stack        
        aContext.SetNextState(*iRegisterRequestedState);
        __ASSERT_DEBUG(aContext.Profile()!=0, User::Invariant());
        __ASSERT_DEBUG(aContext.Registration()!=0, User::Invariant());
        TUint32 contextId(aContext.Registration()->ContextId());        
        aContext.DestroyRegistration();
        aContext.AgentObserver().SIPProfileStatusEvent(
            *aContext.Profile(),contextId);
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
// CSIPPrflRegisteredState::ErrorOccurred()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflRegisteredState::ErrorOccured(
    MSIPProfileContext& aContext,
    TInt aError)
    {
    __ASSERT_DEBUG(aContext.Registration()!=0, User::Invariant());
    CSIPConcreteProfile& profile = *aContext.Profile();
    TUint32 contextId(aContext.Registration()->ContextId());    
    if ( aError == KErrTotalLossOfPrecision )
        {
        iUser.SetInterimProfile(&profile);
        }
    aContext.DestroyRegistration();
    if ( aContext.RetryAfterTimer() )
        {
        //registration failed , re-try later
        aContext.SetTransaction(0);

        if( aContext.RetryTimerInUse() )
            {
            aContext.SetNextState(*iRegisterDelayRequestedState);
            aContext.AgentObserver().SIPProfileStatusEvent(profile,contextId);
            }
        else
            {
            DoCleanup(aContext,contextId);
            aContext.SetNextState(*iRegisterDelayRequestedState);
            aContext.AgentObserver().SIPProfileStatusEvent(profile,contextId);
            aContext.SetNextState(*iInitState); 
            }
        }
    else if ((aError == KErrSIPTransportFailure || 
         aError == KErrSIPResolvingFailure ||
		 aError == KErrSIPICMPFailure ||
		 aError == KErrSIPOutboundProxyNotResponding ) && 
        aContext.Connection().State() == CSIPConnection::ESuspended)
        {
        //registration was dropped due to the expired 
        // refresh while connection is suspended
        aContext.SetNextState(*iRegisterRequestedState);
        aContext.AgentObserver().SIPProfileStatusEvent(profile,contextId);
        }
    else if (aContext.RetryTimerInUse())
        {
        aContext.SetNextState(*iRegisterDelayRequestedState);
        aContext.AgentObserver().SIPProfileErrorEvent(profile,contextId);
        }
    else
        {
        HandleError(aContext, aError, iInitState);
        }
    }
