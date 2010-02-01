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
// Name          : unregistered.cpp
// Part of       : SIPAPI
// Version       : SIP/5.0
//



#include "unregistered.h"
#include "RegBindingImplementation.h"
#include "sipclienttransaction.h"
#include "sipresponseelements.h"


// -----------------------------------------------------------------------------
// CUnregistered::NewL
// -----------------------------------------------------------------------------
//
CUnregistered* CUnregistered::NewL()
	{
	return new (ELeave) CUnregistered();
	}

// -----------------------------------------------------------------------------
// CUnregistered::CUnregistered
// -----------------------------------------------------------------------------
//
CUnregistered::CUnregistered()
	{
	}

// -----------------------------------------------------------------------------
// CUnregistered::~CUnregistered
// -----------------------------------------------------------------------------
//
CUnregistered::~CUnregistered()
	{
	}

// -----------------------------------------------------------------------------
// CUnregistered::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CUnregistered::SetNeighbourStates(CRegistrationState& aRegistering)                                      
    {    
    iRegistering = &aRegistering;    
    }

// -----------------------------------------------------------------------------
// CUnregistered::RegisterL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CUnregistered::RegisterL(CRegBindingImplementation& aRegistration,
                         CSIPMessageElements* aElements) const
	{    
    CSIPClientTransaction* ta = aRegistration.DoRegisterL(aElements);
    aRegistration.ChangeState(iRegistering);
    return ta;
	}

// -----------------------------------------------------------------------------
// CUnregistered::IncomingResponseL
// Response shouldn't be received in this state
// -----------------------------------------------------------------------------
//
TBool
CUnregistered::IncomingResponseL(CRegBindingImplementation& /*aRegistration*/,
		                         CSIPResponseElements* aElements,
				                 TUint32 /*aRequestId*/,
							     TUint32 /*aRegistrationId*/,
							     TUint32 /*aRefreshId*/,
		                         CConnectionCallback& /*aCallback*/) const
    {    
    delete aElements;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CUnregistered::ErrorOccured
// -----------------------------------------------------------------------------
//
TBool CUnregistered::ErrorOccured(CRegBindingImplementation& aRegistration,
                                  TInt aError,
                                  TUint32 aRequestId,
                                  CConnectionCallback& aCallback) const
    {
    return aRegistration.HandleError(aCallback, aError, aRequestId, *this);
    }

// -----------------------------------------------------------------------------
// CUnregistered::ConnectionLost
// No action.
// -----------------------------------------------------------------------------
//
void CUnregistered::ConnectionLost(
						CRegBindingImplementation& /*aRegistration*/) const
    {
    }
