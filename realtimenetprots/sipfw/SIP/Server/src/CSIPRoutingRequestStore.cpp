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
// Name          : CSIPRoutingRequestStore.cpp
// Part of       : SIP Server Core
// Version       : SIP/4.0 
//



#include "CSIPRoutingRequestStore.h"
#include "CSIPRoutingRequest.h"
#include "MSIPRequestRouterObserver.h"
#include "CSipServerCore.h"
#include "sipresponse.h"


// ----------------------------------------------------------------------------
// CSIPRoutingRequestStore::NewL
// ----------------------------------------------------------------------------
//
CSIPRoutingRequestStore* CSIPRoutingRequestStore::NewL(
    CSipServerCore& aServerCore)
    {
    CSIPRoutingRequestStore* self = 
        CSIPRoutingRequestStore::NewLC(aServerCore);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPRoutingRequestStore::NewLC
// ----------------------------------------------------------------------------
//
CSIPRoutingRequestStore* CSIPRoutingRequestStore::NewLC(
    CSipServerCore& aServerCore)
    {
	CSIPRoutingRequestStore* self = 
		new(ELeave)CSIPRoutingRequestStore(aServerCore);
    CleanupStack::PushL(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPRoutingRequestStore::CSIPRoutingRequestStore
// ----------------------------------------------------------------------------
//
CSIPRoutingRequestStore::CSIPRoutingRequestStore (CSipServerCore& aServerCore)
: iServerCore(aServerCore),
  iList(CSIPRoutingRequest::LinkOffset())
    {
    }

// ----------------------------------------------------------------------------
// CSIPRoutingRequestStore::~CSIPRoutingRequestStore
// ----------------------------------------------------------------------------
//
CSIPRoutingRequestStore::~CSIPRoutingRequestStore ()
    {
	TSglQueIter<CSIPRoutingRequest> iter(iList);
	for (CSIPRoutingRequest* request = iter++; request; request = iter++)
		{
		iList.Remove(*request);
		delete request;
    	}    
    }

// ----------------------------------------------------------------------------
// CSIPRoutingRequestStore::TransactionOwnerL
// From MSIPRequestRouter
// ----------------------------------------------------------------------------
//
TUint32 CSIPRoutingRequestStore::TransactionOwnerL(
    CSIPRequest& aRequest,
    TUint32 aIapId,
    MSIPRequestRouterObserver& aObs)
	{
	CSIPRoutingRequest* request = CSIPRoutingRequest::NewLC(aIapId,aObs);
	TUint32 requestId = iServerCore.RequestHandler().ChannelL(aRequest);
	CleanupStack::Pop(request);
	request->SetRequestId(requestId);
	iList.AddLast(*request);
	return requestId;
	}

// ----------------------------------------------------------------------------
// CSIPRoutingRequestStore::Cancel
// From MSIPRequestRouter
// ----------------------------------------------------------------------------
//   
void CSIPRoutingRequestStore::Cancel(TUint32 aRequestId)
	{
	iServerCore.RequestHandler().Cancel(aRequestId);
	RemoveAndDelete(aRequestId);
	}
	
// ----------------------------------------------------------------------------
// CSIPRoutingRequestStore::ClientFoundL
// From MSIPRequestHandlerObserver	
// ----------------------------------------------------------------------------
// 	
void CSIPRoutingRequestStore::ClientFoundL(TUint32 aRequestId, TUid aUid)
	{
	CSIPRoutingRequest* request = Find(aRequestId);
	if (request)
		{
		TUint32 iapId = request->IapId();
		MTransactionOwner* owner = iServerCore.FindOwner(iapId,aUid);
		if (owner)
			{
			request->Observer().OwnerFoundL(aRequestId,owner);
			}
		else
			{
			request->Observer().ErrorOccurred(aRequestId,KErrNotFound);	
			}
		RemoveAndDelete(aRequestId);
		}	
	}

// ----------------------------------------------------------------------------
// CSIPRoutingRequestStore::ClientNotFoundL
// From MSIPRequestHandlerObserver	
// ----------------------------------------------------------------------------
//
void CSIPRoutingRequestStore::ClientNotFoundL(TUint32 aRequestId,
                                              CSIPResponse* aSIPResponse)
	{
	CSIPRoutingRequest* request = Find(aRequestId);
	if (request)
		{
		request->Observer().OwnerNotFoundL(aRequestId,aSIPResponse);
		RemoveAndDelete(aRequestId);
		}
	else
		{
		delete aSIPResponse;
		}
	}

// ----------------------------------------------------------------------------
// CSIPRoutingRequestStore::ClientFoundL
// From MSIPRequestHandlerObserver	
// ----------------------------------------------------------------------------
//
void CSIPRoutingRequestStore::ErrorOccurred(TUint32 aRequestId, TInt aError)
	{
	CSIPRoutingRequest* request = Find(aRequestId);
	if (request)
		{
		request->Observer().ErrorOccurred(aRequestId,aError);
		RemoveAndDelete(aRequestId);		
		}	
	}
	
// ----------------------------------------------------------------------------
// CSIPRoutingRequestStore::Find
// ----------------------------------------------------------------------------
//	
CSIPRoutingRequest* CSIPRoutingRequestStore::Find(TUint32 aRequestId)
	{
	TSglQueIter<CSIPRoutingRequest> iter(iList);
	for (CSIPRoutingRequest* request = iter++; request; request = iter++)
		{
        if (request->RequestId() == aRequestId)
            {
            return request;
            }
		}
	return NULL;
	}

// ----------------------------------------------------------------------------
// CSIPRoutingRequestStore::RemoveAndDelete
// ----------------------------------------------------------------------------
//	
void CSIPRoutingRequestStore::RemoveAndDelete(TUint32 aRequestId)
	{
	CSIPRoutingRequest* request = Find(aRequestId);
	if (request)
	    {
		iList.Remove(*request);
		delete request;		
		}		
	}
