// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : csipregistrationstate.cpp
// Part of       : SIP Registration
// Version       : SIP/4.0 
//




#include "csipregistrationstate.h"
#include "MRegistrationOwner.h"
#include "siperr.h"


// -----------------------------------------------------------------------------
// CSIPRegistrationState::~CSIPRegistrationState
// -----------------------------------------------------------------------------
//
CSIPRegistrationState::~CSIPRegistrationState()
	{
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationState::CSIPRegistrationState
// -----------------------------------------------------------------------------
//
CSIPRegistrationState::CSIPRegistrationState(
	MRegistrationContext& aRegistration) 
	: iRegistration (aRegistration) 
	{
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationState::RegisterL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationState::RegisterL(TTransactionId&  /*aTransactionId*/, 
		                              CSIPRequest*     /*aRequest*/,
		                              CSIPRouteHeader* /*aOutboundProxy*/)
	{
	User::Leave(KErrSIPInvalidRegistrationState);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationState::UpdateL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationState::UpdateL(TTransactionId& /*aTransactionId*/,
					                CSIPRequest*    /*aRequest*/)
	{
	User::Leave(KErrSIPInvalidRegistrationState);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationState::UnregisterL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationState::UnregisterL(TTransactionId& /*aTransactionId*/,
							            CSIPRequest*    /*aRequest*/)
	{
	User::Leave(KErrSIPInvalidRegistrationState);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationState::ReceiveL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationState::ReceiveL(TTransactionId /*aTransactionId*/,
		                             CSIPResponse*  /*aResponse*/)
	{
	User::Leave(KErrSIPInvalidRegistrationState);
	}
	
	
// -----------------------------------------------------------------------------
// CSIPRegistrationState::Retrying 
// -----------------------------------------------------------------------------
//

TBool CSIPRegistrationState::Retrying() const

	{
	return EFalse;	
	}


// -----------------------------------------------------------------------------
// CSIPRegistrationState::PassResponseToOwnerL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationState::PassResponseToOwnerL(
	CSIPResponse* aResponse,
	TTransactionId aTransactionId,
	TRegistrationId aRegistrationId)
	{
	MRegistrationOwner* owner = 
		const_cast<MRegistrationOwner*>(iRegistration.Owner()); 

	owner->RegistrationReceivedL(aTransactionId,iRegistration.RefreshId(),
		                         aRegistrationId,aResponse);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationState::PassErrorToOwner
// -----------------------------------------------------------------------------
//
TInt CSIPRegistrationState::PassErrorToOwner(
	TInt aReason, 
	TTransactionId  aTransactionId,
	TRegistrationId aRegistrationId)
	{
	MRegistrationOwner* owner = 
		const_cast<MRegistrationOwner*>(iRegistration.Owner());
	return owner->RegistrationError(aTransactionId,aRegistrationId,aReason);
	}
