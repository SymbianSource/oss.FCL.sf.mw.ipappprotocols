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
// Name          : ClearProxy.cpp
// Part of       : Transaction
// Version       : SIP/5.0
//



#include "SipAssert.h"
#include "ClientTransaction.h"
#include "ClearProxy.h"

// -----------------------------------------------------------------------------
// TClearProxy::TClearProxy
// -----------------------------------------------------------------------------
//
TClearProxy::TClearProxy(CClientTransaction& aTransaction) :
    iTransaction(aTransaction)
    {
    }

// -----------------------------------------------------------------------------
// TClearProxy::CleanupItem
// -----------------------------------------------------------------------------
//
TCleanupItem TClearProxy::CleanupItem()
    {
    return TCleanupItem(ClearOutboundProxy, this);
    }

// -----------------------------------------------------------------------------
// TClearProxy::ClearOutboundProxy
// -----------------------------------------------------------------------------
//
void TClearProxy::ClearOutboundProxy(TAny* aItem)
    {
	__SIP_ASSERT_RETURN(aItem, KErrArgument);

   	reinterpret_cast<TClearProxy*>(aItem)->iTransaction.ClearProxy();   	
	}
