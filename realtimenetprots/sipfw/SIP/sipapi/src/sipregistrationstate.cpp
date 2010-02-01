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
// Name          : sipregistrationstate.cpp
// Part of       : SIPAPI
// Version       : SIP/5.0
//



#include "sip.h"
#include "siperr.h"
#include "sipregistrationstate.h"


// -----------------------------------------------------------------------------
// CRegistrationState::~CRegistrationState
// -----------------------------------------------------------------------------
//
CRegistrationState::~CRegistrationState()
	{
	}

// -----------------------------------------------------------------------------
// CRegistrationState::IsContextActive
// -----------------------------------------------------------------------------
//
TBool CRegistrationState::IsContextActive() const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CRegistrationState::RegisterL
// RegisterL is only allowed in Unregistered state
// -----------------------------------------------------------------------------
CSIPClientTransaction*
CRegistrationState::RegisterL(CRegBindingImplementation& /*aRegistration*/,
                              CSIPMessageElements* /*aElements*/) const
	{    
    User::Leave(KErrSIPInvalidRegistrationState);
    return NULL;
	}

// -----------------------------------------------------------------------------
// CRegistrationState::DeregisterL
// DeregisterL is only allowed in Registered state
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CRegistrationState::DeregisterL(CRegBindingImplementation& /*aRegistration*/,
                                CSIPMessageElements* /*aElements*/) const
    {        
    User::Leave(KErrSIPInvalidRegistrationState);
    return NULL;
    }

// -----------------------------------------------------------------------------
// CRegistrationState::UpdateL
// UpdateL is only allowed in Registered state
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CRegistrationState::UpdateL(CRegBindingImplementation& /*aRegistration*/,
                            CSIPMessageElements* /*aElements*/,
                            TUint* /*aExpirationValue*/) const
    {     
    User::Leave(KErrSIPInvalidRegistrationState);
    return NULL;
    }

// -----------------------------------------------------------------------------
// CRegistrationState::IncomingResponseL
// -----------------------------------------------------------------------------
//
TBool CRegistrationState::IncomingResponseL(
								CRegBindingImplementation& /*aRegistration*/,
		                        CSIPResponseElements* /*aElements*/,
				                TUint32 /*aRequestId*/,
							    TUint32 /*aRegistrationId*/,
							    TUint32 /*aRefreshId*/,
		                        CConnectionCallback& /*aCallback*/) const
    {    
    __ASSERT_DEBUG(EFalse,
        User::Panic(_L("CRegState::IncomingResponseL() no event handler"),
					KErrSIPInvalidRegistrationState));

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CRegistrationState::ErrorOccured
// -----------------------------------------------------------------------------
//
TBool
CRegistrationState::ErrorOccured(CRegBindingImplementation& /*aRegistration*/,
                              	 TInt /*aError*/,
                              	 TUint32 /*aRequestId*/,
                              	 CConnectionCallback& /*aCallback*/) const
    {    
    __ASSERT_DEBUG(EFalse,
        User::Panic(_L("CRegState::ErrorOccured() no event handler"),
					KErrSIPInvalidRegistrationState));

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CRegistrationState::RefreshState
// -----------------------------------------------------------------------------
//
CSIPRefresh::TState CRegistrationState::RefreshState() const
    {
    return CSIPRefresh::ETerminated;
    }
