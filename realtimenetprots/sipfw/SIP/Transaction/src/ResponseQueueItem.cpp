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
// Name          : ResponseQueueItem.cpp
// Part of       : Transaction
// Version       : SIP/4.0
//



#include "SipAssert.h"
#include "sipresponse.h"

#include "ResponseQueueItem.h"

// -----------------------------------------------------------------------------
// CResponseQueueItem::NewLC
// -----------------------------------------------------------------------------
//
CResponseQueueItem*
CResponseQueueItem::NewLC(CSIPResponse* aResp,
						  const TSIPTransportParams& aParams,
						  TBool aRespToInvite)
	{
    __SIP_ASSERT_LEAVE(aResp, KErrArgument);

	CResponseQueueItem* self = new (ELeave) CResponseQueueItem(aResp,
															   aParams,
															   aRespToInvite);
    CleanupStack::PushL(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CResponseQueueItem::CResponseQueueItem
// -----------------------------------------------------------------------------
//
CResponseQueueItem::CResponseQueueItem(CSIPResponse* aResp,
									   const TSIPTransportParams& aParams,
									   TBool aRespToInvite) :
	iResp(aResp),
	iParams(aParams),
	iRespToInvite(aRespToInvite),
	iOwnsResponse(EFalse)	
	{	
	}

// -----------------------------------------------------------------------------
// CResponseQueueItem::~CResponseQueueItem
// -----------------------------------------------------------------------------
//
CResponseQueueItem::~CResponseQueueItem()
	{
	if (iOwnsResponse)
		{		
		delete iResp;
		}
	}

// -----------------------------------------------------------------------------
// CResponseQueueItem::SetResponseOwnership
// CResponseQueueItem can never own a 2xx response to INVITE.
// -----------------------------------------------------------------------------
//
void CResponseQueueItem::SetResponseOwnership(TBool aOwnsResponse)
	{
	if (!(iResp->Type() == CSIPResponse::E2XX && iRespToInvite))
		{
		iOwnsResponse = aOwnsResponse;
		}
	}

// -----------------------------------------------------------------------------
// CResponseQueueItem::Response
// -----------------------------------------------------------------------------
//
CSIPResponse& CResponseQueueItem::Response()
	{	
	return *iResp;
	}

// -----------------------------------------------------------------------------
// CResponseQueueItem::TransportParams
// -----------------------------------------------------------------------------
//
const TSIPTransportParams& CResponseQueueItem::TransportParams() const
	{
	return iParams;
	}

// -----------------------------------------------------------------------------
// CResponseQueueItem::IsRespToInvite
// -----------------------------------------------------------------------------
//
TBool CResponseQueueItem::IsRespToInvite() const
	{
	return iRespToInvite;
	}
