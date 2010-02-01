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
// Name          : pendingtransaction.cpp
// Part of       : SIPAPI
// Version       : SIP/4.0
//



#include "SipAssert.h"
#include "PendingTransaction.h"


// -----------------------------------------------------------------------------
// CPendingTransaction::NewLC
// -----------------------------------------------------------------------------
//
CPendingTransaction*
CPendingTransaction::NewLC(CSIPServerTransaction* aTransaction, TUint32 aIapId)
    {
    __SIP_ASSERT_LEAVE(aTransaction, KErrArgument);

	CPendingTransaction* self =
		new (ELeave) CPendingTransaction(aTransaction, aIapId);
	CleanupStack::PushL(self);
	return self;
    }

// -----------------------------------------------------------------------------
// CPendingTransaction::CPendingTransaction
// -----------------------------------------------------------------------------
//
CPendingTransaction::CPendingTransaction(CSIPServerTransaction* aTransaction,
                                    	 TUint32 aIapId) :
    iTransaction(aTransaction),
    iIapId(aIapId)
    {
    __ASSERT_DEBUG(aTransaction,
		User::Panic(_L("CPendingTa::CPendingTa() ta = 0"),
		KErrArgument));
    }

// -----------------------------------------------------------------------------
// CPendingTransaction::~CPendingTransaction
// -----------------------------------------------------------------------------
//
CPendingTransaction::~CPendingTransaction()
    {    
    }

// -----------------------------------------------------------------------------
// CPendingTransaction::Transaction
// -----------------------------------------------------------------------------
//
CSIPServerTransaction* CPendingTransaction::Transaction()
    {
    return iTransaction;
    }

// -----------------------------------------------------------------------------
// CPendingTransaction::IapId
// -----------------------------------------------------------------------------
//	
TUint32 CPendingTransaction::IapId() const
    {
    return iIapId;
    }
