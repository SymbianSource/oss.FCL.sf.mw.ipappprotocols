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
// Name          : StopUserAgent.cpp
// Part of       : TransactionUser
// Version       : SIP/6.0
//



#include "SipAssert.h"
#include "StopUserAgent.h"
#include "CUserAgent.h"
#include "CTransactionStore.h"

// -----------------------------------------------------------------------------
// TStopUserAgent::TStopUserAgent
// -----------------------------------------------------------------------------
//
TStopUserAgent::TStopUserAgent(TTransactionId aTransactionId,
                               CTransactionStore& aStore,
                               TInt aReason) :
    iTransactionId(aTransactionId),
    iStore(aStore),
    iReason(aReason)
    {
    }

// -----------------------------------------------------------------------------
// TStopUserAgent::CleanupItem
// -----------------------------------------------------------------------------
//
TCleanupItem TStopUserAgent::CleanupItem()
    {
    return TCleanupItem(StopIfFound, this);
    }

// -----------------------------------------------------------------------------
// TStopUserAgent::StopIfFound
// -----------------------------------------------------------------------------
//
void TStopUserAgent::StopIfFound(TAny* aStopUserAgent)
    {
	__SIP_ASSERT_RETURN(aStopUserAgent, KErrArgument);

    TStopUserAgent* s = reinterpret_cast<TStopUserAgent*>(aStopUserAgent);
    CUserAgent* ua = s->iStore.SearchById(s->iTransactionId);
    if (ua)
        {
        ua->Stop(s->iReason);
        }
	}
