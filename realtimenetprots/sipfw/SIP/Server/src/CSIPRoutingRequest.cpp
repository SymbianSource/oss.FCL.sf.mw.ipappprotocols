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
// Name          : CSIPRoutingRequest.cpp
// Part of       : SIP Server Core
// Version       : SIP/4.0 
//



#include "CSIPRoutingRequest.h"

// -----------------------------------------------------------------------------
// CSIPRoutingRequest::NewLC
// -----------------------------------------------------------------------------
//
CSIPRoutingRequest* CSIPRoutingRequest::NewLC(
    TUint32 aIapId,
    MSIPRequestRouterObserver& aObserver)
	{
	CSIPRoutingRequest* self = new(ELeave)CSIPRoutingRequest(aIapId,aObserver);
	CleanupStack::PushL(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRoutingRequest::NewL
// -----------------------------------------------------------------------------
//
CSIPRoutingRequest* CSIPRoutingRequest::NewL(
    TUint32 aIapId,
    MSIPRequestRouterObserver& aObserver)
	{
	CSIPRoutingRequest* self = CSIPRoutingRequest::NewLC(aIapId,aObserver);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRoutingRequest::CSIPRoutingRequest
// -----------------------------------------------------------------------------
//
CSIPRoutingRequest::CSIPRoutingRequest(
    TUint32 aIapId,
    MSIPRequestRouterObserver& aObserver)
 : iIapId(aIapId),
   iObserver(aObserver)
    {    
    }

// -----------------------------------------------------------------------------
// CSIPRoutingRequest::~CSIPRoutingRequest
// -----------------------------------------------------------------------------
//
CSIPRoutingRequest::~CSIPRoutingRequest()
    {
    }
    
// -----------------------------------------------------------------------------
// CSIPRoutingRequest::LinkOffset
// -----------------------------------------------------------------------------
//
TInt CSIPRoutingRequest::LinkOffset()
	{
	return _FOFF(CSIPRoutingRequest,iLink);
	}    

// -----------------------------------------------------------------------------
// CSIPRoutingRequest::SetRequestId
// -----------------------------------------------------------------------------
//
void CSIPRoutingRequest::SetRequestId(TUint32 aId)
	{
	iRequestId = aId;
	}
    
// -----------------------------------------------------------------------------
// CSIPRoutingRequest::RequestId
// -----------------------------------------------------------------------------
//
TUint32 CSIPRoutingRequest::RequestId() const
	{
	return iRequestId;		
	}
    
// -----------------------------------------------------------------------------
// CSIPRoutingRequest::IapId
// -----------------------------------------------------------------------------
// 
TUint32 CSIPRoutingRequest::IapId() const
	{
	return iIapId;
	}
	
// -----------------------------------------------------------------------------
// CSIPRoutingRequest::Observer
// -----------------------------------------------------------------------------
// 	
MSIPRequestRouterObserver& CSIPRoutingRequest::Observer()
	{
	return iObserver;
	}
