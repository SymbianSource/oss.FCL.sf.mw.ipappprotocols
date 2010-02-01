// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : registered.cpp
// Part of       : SIPAPI
// Version       : SIP/5.0
//



#include "sip.h"
#include "siperr.h"
#include "SipAssert.h"
#include "registered.h"
#include "RegBindingImplementation.h"
#include "sipresponseelements.h"
#include "sipclienttransaction.h"
#include "sipconnectioncallback.h"


// -----------------------------------------------------------------------------
// CRegistered::NewL
// -----------------------------------------------------------------------------
//
CRegistered* CRegistered::NewL()
	{
	return new (ELeave) CRegistered();
	}

// -----------------------------------------------------------------------------
// CRegistered::CRegistered
// -----------------------------------------------------------------------------
//
CRegistered::CRegistered()
	{
	}

// -----------------------------------------------------------------------------
// CRegistered::~CRegistered
// -----------------------------------------------------------------------------
//
CRegistered::~CRegistered()
	{
	}

// -----------------------------------------------------------------------------
// CRegistered::IsContextActive
// -----------------------------------------------------------------------------
//
TBool CRegistered::IsContextActive() const
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CRegistered::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CRegistered::SetNeighbourStates(CRegistrationState& aUnregistering,
                                     CRegistrationState& aUnregistered)
    {
    iUnregistering = &aUnregistering;
    iUnregistered = &aUnregistered;
    }

// -----------------------------------------------------------------------------
// CRegistered::DeregisterL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction* 
CRegistered::DeregisterL(CRegBindingImplementation& aRegistration,
                         CSIPMessageElements* aElements) const
    {
    CSIPClientTransaction* ta = aRegistration.DoDeregisterL(aElements);
    aRegistration.ChangeState(iUnregistering);

    return ta;
    }

// -----------------------------------------------------------------------------
// CRegistered::UpdateL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CRegistered::UpdateL(CRegBindingImplementation& aRegistration,
                     CSIPMessageElements* aElements,
                     TUint* aExpirationValue) const
    {    
    return aRegistration.DoUpdateL(aElements, aExpirationValue);
    }

// -----------------------------------------------------------------------------
// CRegistered::IncomingResponseL
// When an existing refreshed registration ends with an error response,
// aRequestId is the id of the transaction that received the error, not of the
// transaction that originally created the refresh.
//
// CSIPRefresh can not be deleted even if the refresh ends (300-699 response).
// User might use the same CSIPRegistrationBinding to register again, and the
// CSIPRefresh would be used again.
// -----------------------------------------------------------------------------
//
TBool
CRegistered::IncomingResponseL(CRegBindingImplementation& aRegistration,
                               CSIPResponseElements* aElements,
		                       TUint32 aRequestId,
				               TUint32 /*aRegistrationId*/,
				               TUint32 aRefreshId,
                               CConnectionCallback& aCallback) const
    {    
    __SIP_ASSERT_LEAVE(aElements, KErrArgument);

    TUint statusCode = aElements->StatusCode();

    if (statusCode >= 300)
        {
        aRegistration.RemoveRegisteredContact();
        aRegistration.ChangeState(iUnregistered);        
        }

    if (statusCode >= 200 && statusCode < 300)
        {
        aRegistration.UpdateRegisteredContactL();
        }

    CSIPClientTransaction* ta = aRegistration.FindTransaction(aRequestId);
    if (ta)
        {
        ta->SetResponseElements(aElements);
        aCallback.Set(CConnectionCallback::EIncomingResponseInRegistration,
                      ta,
                      &aRegistration.Binding());
        return ETrue;
        }

    //If a 2xx is received in registered state, it is a response to an updated
    //REGISTER. Now transaction was not found, meaning application has deleted
    //it, indicating it is not interested on the response or if the response is
    //3xx-6xx it may indicate a refresh has ended.
    if (aRefreshId != 0 && statusCode >= 300)
        {
		ta = aRegistration.CreateClientTransactionL();
		ta->SetRequestId(aRequestId);
		ta->SetResponseElements(aElements);
        aRegistration.UpdateRefreshState(aElements->StatusCode());

        aCallback.Set(CConnectionCallback::EErrorOccuredInRegistration,
                      NULL,
                      &aRegistration.Binding(),
                      NULL,
                      NULL,
                      KErrSIPTerminatedWithResponse,
                      ta);
        return ETrue;
        }
    delete aElements;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CRegistered::ErrorOccured
// CSIPRefresh can not be deleted even if the refresh ends. Application might
// use the same CSIPRegistrationBinding to register again, and the CSIPRefresh
// would be used again.
// -----------------------------------------------------------------------------
//
TBool CRegistered::ErrorOccured(CRegBindingImplementation& aRegistration,
                                TInt aError,
                                TUint32 aRequestId,
                                CConnectionCallback& aCallback) const
    {
    return aRegistration.HandleError(aCallback,
    								 aError,
    								 aRequestId,
    								 *iUnregistered);
    }

// -----------------------------------------------------------------------------
// CRegistered::RefreshState
// -----------------------------------------------------------------------------
//
CSIPRefresh::TState CRegistered::RefreshState() const
    {
    return CSIPRefresh::EActive;
    }

// -----------------------------------------------------------------------------
// CRegistered::ConnectionLost
// -----------------------------------------------------------------------------
//
void CRegistered::ConnectionLost(CRegBindingImplementation& aRegistration) const
    {    
    aRegistration.ChangeState(iUnregistered);
    }
