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
// Name          : RestoreUaState.cpp
// Part of       : TransactionUser
// Version       : SIP/4.0
//



#include "SipAssert.h"
#include "RestoreUaState.h"
#include "CUserAgent.h"

// -----------------------------------------------------------------------------
// TRestoreUaState::TRestoreUaState
// Either both aUserAgent and aTimer must be defined, or neither can.
// -----------------------------------------------------------------------------
//
TRestoreUaState::TRestoreUaState(TBool& aFinalRespPassed,
								 CUserAgent* aUserAgent,
								 CUserAgentTimer* aTimer) :
    iFinalRespPassed(aFinalRespPassed),
    iOriginalValue(iFinalRespPassed),
    iUserAgent(aUserAgent),
    iTimer(aTimer)
    {
    __ASSERT_DEBUG((aUserAgent && aTimer) || (!aUserAgent && !aTimer),
    	User::Panic(_L("TRestoreUaState::TRestoreUaState() UA/timer mismatch"),
    	KErrArgument));
    }

// -----------------------------------------------------------------------------
// TRestoreUaState::CleanupItem
// -----------------------------------------------------------------------------
//
TCleanupItem TRestoreUaState::CleanupItem()
    {
    return TCleanupItem(Restore, this);
    }

// -----------------------------------------------------------------------------
// TRestoreUaState::Restore
// -----------------------------------------------------------------------------
//
void TRestoreUaState::Restore(TAny* aRestoreState)
    {
	__SIP_ASSERT_RETURN(aRestoreState, KErrArgument);

    TRestoreUaState* restore =
    	reinterpret_cast<TRestoreUaState*>(aRestoreState);

	restore->iFinalRespPassed = restore->iOriginalValue;

	if (restore->iUserAgent && restore->iTimer)
    	{
    	restore->iUserAgent->DeleteTimer(*restore->iTimer);
        }
	}
