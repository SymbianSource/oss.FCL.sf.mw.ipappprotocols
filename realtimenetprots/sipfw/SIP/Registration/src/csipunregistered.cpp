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
// Name          : csipunregistered.cpp
// Part of       : SIP Registration
// Version       : SIP/4.0 
//



#include "csipunregistered.h"
#include "SipAssert.h"


// -----------------------------------------------------------------------------
// CSIPUnregistered::NewL
// -----------------------------------------------------------------------------
//
CSIPUnregistered* CSIPUnregistered::NewL(MRegistrationContext& aRegistration)
	{
	CSIPUnregistered* self = CSIPUnregistered::NewLC(aRegistration);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPUnregistered::NewLC
// -----------------------------------------------------------------------------
//
CSIPUnregistered* CSIPUnregistered::NewLC(MRegistrationContext& aRegistration)
	{
	CSIPUnregistered* self = new (ELeave) CSIPUnregistered (aRegistration);
	CleanupStack::PushL(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPUnregistered::~CSIPUnregistered
// -----------------------------------------------------------------------------
//
CSIPUnregistered::~CSIPUnregistered()
	{
	}

// -----------------------------------------------------------------------------
// CSIPUnregistered::TransactionEnded
// -----------------------------------------------------------------------------
//
TInt CSIPUnregistered::TransactionEnded(TTransactionId aTransactionId, 
		                                TInt           aReason)
	{
	// pass error to owner.
	return (PassErrorToOwner(aReason, aTransactionId, 
		iRegistration.RegistrationId()));	
	}

// -----------------------------------------------------------------------------
// CSIPUnregistered::CSIPUnregistered
// -----------------------------------------------------------------------------
//
CSIPUnregistered::CSIPUnregistered(MRegistrationContext& aRegistration)
	: CSIPRegistrationState (aRegistration)
	{
	}
