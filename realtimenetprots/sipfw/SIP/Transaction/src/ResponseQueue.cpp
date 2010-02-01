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
// Name          : ResponseQueue.cpp
// Part of       : Transaction
// Version       : SIP/4.0 
//



#include "SipAssert.h"
#include "sipresponse.h"

#include "ResponseQueue.h"
#include "ResponseQueueItem.h"


// -----------------------------------------------------------------------------
// CResponseQueue::NewL
// -----------------------------------------------------------------------------
//
CResponseQueue* CResponseQueue::NewL()
	{
	return new (ELeave) CResponseQueue();
	}

// -----------------------------------------------------------------------------
// CResponseQueue::CResponseQueue
// -----------------------------------------------------------------------------
//
CResponseQueue::CResponseQueue()
#ifdef CPPUNIT_TEST
    //For unit tests the granularity of arrays is set to 1 to cause them to
    //allocate memory every time an item is appended to array
    : iQueue(1)
#endif
    {
    }

// -----------------------------------------------------------------------------
// CResponseQueue::~CResponseQueue
// -----------------------------------------------------------------------------
//
CResponseQueue::~CResponseQueue()
	{
	iQueue.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CResponseQueue::Add
// -----------------------------------------------------------------------------
//
TInt CResponseQueue::Add(CResponseQueueItem* aRespItem)
	{	
	if (!aRespItem)
        {
        return KErrArgument;
        }

	TInt status = iQueue.Append(aRespItem);
	if (status == KErrNone)
		{
		//Ownership of the SIP response is passed to aRespItem
		aRespItem->SetResponseOwnership(ETrue);
		}
	return status;
	}

// -----------------------------------------------------------------------------
// CResponseQueue::GetNext
// -----------------------------------------------------------------------------
//
CResponseQueueItem* CResponseQueue::GetNext()
	{
	if (iQueue.Count() > 0)
		{
		CResponseQueueItem* respItem = iQueue[0];

		__ASSERT_DEBUG(respItem,
			User::Panic(_L("CRespQueue::GetNext() respItem = 0"), KErrNotFound));

		iQueue.Remove(0);
		return respItem;
		}

	return NULL;
	}
