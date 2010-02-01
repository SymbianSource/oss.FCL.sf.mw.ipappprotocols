// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : unregistering.cpp
// Part of       : SIPAPI
// Version       : SIP/4.0 
//



#include "sip.h"
#include "siperr.h"
#include "SipAssert.h"
#include "unregistering.h"
#include "RegBindingImplementation.h"
#include "sipclienttransaction.h"
#include "sipresponseelements.h"
#include "sipconnectioncallback.h"


// -----------------------------------------------------------------------------
// CUnregistering::NewL
// -----------------------------------------------------------------------------
//
CUnregistering* CUnregistering::NewL()
	{
	return new (ELeave) CUnregistering();
	}

// -----------------------------------------------------------------------------
// CUnregistering::CUnregistering
// -----------------------------------------------------------------------------
//
CUnregistering::CUnregistering()
	{
	}

// -----------------------------------------------------------------------------
// CUnregistering::~CUnregistering
// -----------------------------------------------------------------------------
//
CUnregistering::~CUnregistering()
	{
	}

// -----------------------------------------------------------------------------
// CUnregistering::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CUnregistering::SetNeighbourStates(CRegistrationState& aRegistered,
							            CRegistrationState& aUnregistered)                                      
	{	
    iRegistered = &aRegistered;
    iUnregistered = &aUnregistered;
    }

// -----------------------------------------------------------------------------
// CUnregistering::IncomingResponseL
// -----------------------------------------------------------------------------
//
TBool
CUnregistering::IncomingResponseL(CRegBindingImplementation& aRegistration,
                                  CSIPResponseElements* aElements,
				                  TUint32 aRequestId,
						          TUint32 /*aRegistrationId*/,
						          TUint32 aRefreshId,
		                          CConnectionCallback& aCallback) const
    {
    CSIPClientTransaction* ta = aRegistration.FindTransaction(aRequestId);    
    CBase* objToDelete = NULL;

    if (!ta)
    	{
    	if (!aRefreshId)
    		{
    		delete aElements;
    		return EFalse;
    		}
    	ta = aRegistration.CreateClientTransactionL();
    	ta->SetRequestId(aRequestId);
      	objToDelete = ta;
    	}

    if (aElements->StatusCode() >= 200)
        {
        aRegistration.RemoveRegisteredContact();
        aRegistration.ChangeState(iUnregistered);
        }  	
  	ta->SetResponseElements(aElements);

	if (objToDelete && aElements->StatusCode() >= 300)
		{
		aCallback.Set(CConnectionCallback::EErrorOccuredInRegistration,
                      NULL,
                      &aRegistration.Binding(),
                      NULL,
                      NULL,
                      KErrSIPTerminatedWithResponse,
                      objToDelete);
        }
    else
        {
        //All responses, also 1xx, are passed to application
        aCallback.Set(CConnectionCallback::EIncomingResponseInRegistration,
	                  ta,
	                  &aRegistration.Binding(),
	                  NULL,
	                  NULL,
	                  KErrNone,
	                  objToDelete);
		}

	return ETrue;
    }

// -----------------------------------------------------------------------------
// CUnregistering::ErrorOccured
// -----------------------------------------------------------------------------
//
TBool CUnregistering::ErrorOccured(CRegBindingImplementation& aRegistration,
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
// CUnregistering::ConnectionLost
// -----------------------------------------------------------------------------
//
void
CUnregistering::ConnectionLost(CRegBindingImplementation& aRegistration) const
    {    
    aRegistration.ChangeState(iUnregistered);
    }
