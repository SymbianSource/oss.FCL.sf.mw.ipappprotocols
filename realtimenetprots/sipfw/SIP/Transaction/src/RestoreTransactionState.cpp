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
// Name          : RestoreTransactionState.cpp
// Part of       : Transaction
// Version       : SIP/5.0
//



#include "SipAssert.h"
#include "ClientTransaction.h"
#include "RestoreTransactionState.h"
#include "TimerBase.h"

// -----------------------------------------------------------------------------
// TRestoreTransactionState::TRestoreTransactionState
// -----------------------------------------------------------------------------
//
TRestoreTransactionState::TRestoreTransactionState(
                                   CClientTransaction& aTransaction,
                                   const CTransactionState& aState,
                                   CTransactionTimer* aTimer,
                                   CTransactionTimer* aTimer2) :
    iTransaction(aTransaction),
    iState(aState),
    iTimer(aTimer),
    iTimer2(aTimer2)
    {
    }

// -----------------------------------------------------------------------------
// TRestoreTransactionState::CleanupItem
// -----------------------------------------------------------------------------
//
TCleanupItem TRestoreTransactionState::CleanupItem()
    {
    return TCleanupItem(Restore, this);
    }

// -----------------------------------------------------------------------------
// TRestoreTransactionState::Restore
// -----------------------------------------------------------------------------
//
void TRestoreTransactionState::Restore(TAny* aRestoreState)
    {
	__SIP_ASSERT_RETURN(aRestoreState, KErrArgument);

    TRestoreTransactionState* restore =
        reinterpret_cast<TRestoreTransactionState*>(aRestoreState);

	restore->iTransaction.ChangeState(restore->iState);

	if (restore->iTimer)
    	{
    	restore->iTransaction.DeleteTimer(*restore->iTimer);
        }
    if (restore->iTimer2)
    	{
    	restore->iTransaction.DeleteTimer(*restore->iTimer2);
        }
	}
