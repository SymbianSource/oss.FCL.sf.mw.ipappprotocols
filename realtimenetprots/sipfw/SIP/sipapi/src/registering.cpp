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
// Name          : registering.cpp
// Part of       : SIPAPI
// Version       : SIP/5.0 
//



#include "SipAssert.h"
#include "registering.h"
#include "RegBindingImplementation.h"
#include "sipclienttransaction.h"
#include "sipresponseelements.h"
#include "sipconnectioncallback.h"


// -----------------------------------------------------------------------------
// CRegistering::NewL
// -----------------------------------------------------------------------------
//
CRegistering* CRegistering::NewL()
	{
	return new (ELeave) CRegistering();
	}

// -----------------------------------------------------------------------------
// CRegistering::CRegistering
// -----------------------------------------------------------------------------
//
CRegistering::CRegistering()
	{
	}

// -----------------------------------------------------------------------------
// CRegistering::~CRegistering
// -----------------------------------------------------------------------------
//
CRegistering::~CRegistering()
	{
	}

// -----------------------------------------------------------------------------
// CRegistering::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CRegistering::SetNeighbourStates(CRegistrationState& aRegistered,
							          CRegistrationState& aUnregistered)                                      
    {    
    iRegistered = &aRegistered;
    iUnregistered = &aUnregistered;
    }

// -----------------------------------------------------------------------------
// CRegistering::IncomingResponseL
// -----------------------------------------------------------------------------
//
TBool CRegistering::IncomingResponseL(CRegBindingImplementation& aRegistration,
                                      CSIPResponseElements* aElements,
		                              TUint32 aRequestId,
				                      TUint32 aRegistrationId,
				                      TUint32 aRefreshId,
                                      CConnectionCallback& aCallback) const
    {
    CSIPClientTransaction* ta = aRegistration.FindTransaction(aRequestId);
    if (ta)
        {
        TUint statusCode = aElements->StatusCode();

		if (aRegistration.SIPRefresh())
			{
			aRegistration.SIPRefresh()->SetRefreshIdIfEmpty(aRefreshId);
			}

        if (statusCode >= 200 && statusCode < 300)
            {
            aRegistration.SetRegistrationId(aRegistrationId);
            aRegistration.UpdateRegisteredContactL();
            aRegistration.ChangeState(iRegistered);
            }

        if (statusCode >= 300)
            {
            aRegistration.ChangeState(iUnregistered);
            }
            
        // Set response elements after calling all the leaving functions    
        ta->SetResponseElements(aElements); 

        aCallback.Set(CConnectionCallback::EIncomingResponseInRegistration,
                      ta,
                      &aRegistration.Binding());
        return ETrue;
        }
    
    delete aElements;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CRegistering::ErrorOccured
// -----------------------------------------------------------------------------
//
TBool CRegistering::ErrorOccured(CRegBindingImplementation& aRegistration,
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
// CRegistering::RefreshState
// Registering has been started but isn't yet active.
// -----------------------------------------------------------------------------
//
CSIPRefresh::TState CRegistering::RefreshState() const
    {
    return CSIPRefresh::EInactive;
    }

// -----------------------------------------------------------------------------
// CRegistering::ConnectionLost
// -----------------------------------------------------------------------------
//
void
CRegistering::ConnectionLost(CRegBindingImplementation& aRegistration) const
    {
    aRegistration.ChangeState(iUnregistered);
    }
