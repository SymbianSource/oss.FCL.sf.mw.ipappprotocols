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
// Name          : DialogsStore.cpp
// Part of       : SIPDialogs
// Version       : SIP/4.0 
//




#include "DialogsStore.h"
#include "TransactionItemStore.h"
#include "DeleteMgr.h"
#include "sipmessage.h"
#include "SipStackServerDefs.h"
#include "sipfromheader.h"
#include "siptoheader.h"
#include "sipcallidheader.h"
#include "sipcseqheader.h"
#include "sipaddress.h"
#include "sipuri.h"
#include "sipresponse.h"
#include "siprequest.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "TSIPTransportParams.h"


// -----------------------------------------------------------------------------
// CDialogsStore::NewL
// -----------------------------------------------------------------------------
//
CDialogsStore* CDialogsStore::NewL()
	{
	CDialogsStore* self = new (ELeave) CDialogsStore;
	return self;
	}

// -----------------------------------------------------------------------------
// CDialogsStore::CDialogsStore
// -----------------------------------------------------------------------------
//
CDialogsStore::CDialogsStore()
 : iList(CSipDialogItem::Offset())
	{
	iTagParamName = SIPStrings::StringF(SipStrConsts::ETag);
	}

// -----------------------------------------------------------------------------
// CDialogsStore::~CDialogsStore
// -----------------------------------------------------------------------------
//
CDialogsStore::~CDialogsStore()
	{
	CSipDialogItem* listItem = iList.First();

	while (iList.IsFirst(listItem) && !iList.IsEmpty())
		{
		iList.Remove(*listItem);
        delete listItem;
		listItem = iList.First();
		}

	}

// -----------------------------------------------------------------------------
// CDialogsStore::AddDialogItem
// -----------------------------------------------------------------------------
//
void CDialogsStore::AddDialogItem(CSipDialogItem* aDialogItem)
	{
	iList.AddLast(*aDialogItem);
	}

// -----------------------------------------------------------------------------
// CDialogsStore::DialogItemsCount
// -----------------------------------------------------------------------------
//
TInt CDialogsStore::DialogItemsCount()
	{
	TSglQueIter<CSipDialogItem> listIter(iList);
	TInt counter = 0;
	for (CSipDialogItem* listItem = listIter++; listItem; listItem = listIter++)
	    {
	    counter++;
	    }
	return counter;
	}

// -----------------------------------------------------------------------------
// CDialogsStore::Remove
// -----------------------------------------------------------------------------
//
TInt CDialogsStore::Remove (CSipDialogItem* aDialogItem)
	{
	TInt err = aDialogItem->DeleteMgr().AddDeleteRequest(aDialogItem);
	if (!err)
        {
		iList.Remove(*aDialogItem);
        }
	return err;
	}

// -----------------------------------------------------------------------------
// CDialogsStore::RemoveAllWith
// -----------------------------------------------------------------------------
//
TInt CDialogsStore::RemoveAllWith (const MDialogOwner* aDialogOwner)
    {
	TInt err = KErrNotFound;
	TSglQueIter<CSipDialogItem> listIter(iList);
	for (CSipDialogItem* listItem = listIter++; listItem; listItem = listIter++)
		{
		if (listItem->DialogOwner() == aDialogOwner)  
            {
            iList.Remove(*listItem);
            delete listItem;
			err = KErrNone;
			}
		}
	return err;
    }

// -----------------------------------------------------------------------------
// CDialogsStore::RemoveAllEarlyDialogsWith
// -----------------------------------------------------------------------------
//
TInt CDialogsStore::RemoveAllEarlyDialogsWith (TTransactionId aTransactionId)
	{
	TInt err = KErrNotFound;
	TSglQueIter<CSipDialogItem> listIter(iList);
	for (CSipDialogItem* item = listIter++; item; item = listIter++)
		{
		if (item->HasTransactionItem(aTransactionId) && item->IsEarly())
			{
			err = Remove(item);
			}
		}
	return err;
	}

// -----------------------------------------------------------------------------
// CDialogsStore::DeleteItemsWithThisIapId
// -----------------------------------------------------------------------------
//
void CDialogsStore::DeleteItemsWithThisIapId(TUint32 aIapId)
	{
	TSglQueIter<CSipDialogItem> listIter(iList);
	for (CSipDialogItem* listItem = listIter++; listItem; listItem = listIter++)
		{
		if (listItem->IapId() == aIapId)
			{
			iList.Remove(*listItem);
			delete listItem;
			}
		}
	}

// -----------------------------------------------------------------------------
// CDialogsStore::FindDialogItem
// -----------------------------------------------------------------------------
//
CSipDialogItem* CDialogsStore::FindDialogItem(const TDialogId& aDialogId)
	{
	TSglQueIter<CSipDialogItem> iter(iList);
	for (CSipDialogItem* item = iter++; item; item = iter++)
		{
		if (item->DialogId() == aDialogId)
		    {
		    return item;
		    }
		}
	return NULL;
	}


// -----------------------------------------------------------------------------
// CDialogsStore::FindDialogItem
// -----------------------------------------------------------------------------
//
CSipDialogItem* CDialogsStore::FindDialogItem(const TDialogId& aDialogId,
                                              const MDialogOwner* aDialogOwner)
	{
	TSglQueIter<CSipDialogItem> iter(iList);
	for (CSipDialogItem* item = iter++; item; item = iter++)
		{
		if (item->DialogId() == aDialogId &&
		    item->DialogOwner() == aDialogOwner)
		    {
		    return item;
		    }
		}
	return NULL;
	}

// -----------------------------------------------------------------------------
// CDialogsStore::FindDialogItemByTransaction
// -----------------------------------------------------------------------------
//
CSipDialogItem* CDialogsStore::FindDialogItemByTransaction(
    const TTransactionId& aTransactionId)
    {
	TSglQueIter<CSipDialogItem> iter(iList);
	for (CSipDialogItem* item = iter++; item; item = iter++)
		{
		if (item->HasTransactionItem(aTransactionId))
		    {
		    return item;
		    }
		}
	return NULL;
    }

// -----------------------------------------------------------------------------
// CDialogsStore::FindDialogItemForPass
// -----------------------------------------------------------------------------
//
CSipDialogItem* 
CDialogsStore::FindDialogItemForPass (TTransactionId aTransactionId, 
									  CSIPResponse* aResponse)
	{
	CSIPToHeader* to = aResponse->To();
	if (to)
		{
		TSglQueIter<CSipDialogItem> listIter(iList);
		for (CSipDialogItem* item = listIter++; item; item = listIter++)
			{
			if (item->RemoteTagSet() &&
				item->RemoteTag() == to->ParamValue(iTagParamName) &&
                item->HasTransactionItem(aTransactionId))
                {
                return item;
                }
			}
		}
	return 0;	
	}

// -----------------------------------------------------------------------------
// CDialogsStore::DialogItemForRequest
// -----------------------------------------------------------------------------
//
CSipDialogItem* CDialogsStore::DialogItemForRequest(CSIPRequest* aRequest)
	{
	if (!aRequest)
        {
		return NULL;
        }

	TSglQueIter<CSipDialogItem> listIter(iList);
	for (CSipDialogItem* item = listIter++; item; item = listIter++)
		{
		if (item->Match(*aRequest))
		    {
		    return item;
		    }
		}
	return NULL;
	}

// -----------------------------------------------------------------------------
// CDialogsStore::FindDialogItemByCallId
// -----------------------------------------------------------------------------
//
CSipDialogItem* CDialogsStore::FindDialogItemByCallId(CSIPMessage* aMessage)
	{
	if (aMessage)
        {
	    TSglQueIter<CSipDialogItem> listIter(iList);
	    CSIPCallIDHeader* callId = aMessage->CallID();
	
    	for (CSipDialogItem* item = listIter++; item; item = listIter++)
	    	{
	    	if (*callId == *item->CallId())
                {
                return item;
                }
	    	}
        }
    return 0;
    }

// -----------------------------------------------------------------------------
// CDialogsStore::NewDialogId
// -----------------------------------------------------------------------------
//
TDialogId CDialogsStore::NewDialogId()
	{
	if (iDialogIdCounter < KMaxDialogId)
		{
		iDialogIdCounter++;
		}
	else
		{
		iDialogIdCounter = KMinDialogId;
		}	
	return iDialogIdCounter;
	}


// -----------------------------------------------------------------------------
// CDialogsStore::TransactionEnded
// -----------------------------------------------------------------------------
//
void CDialogsStore::TransactionEnded (TTransactionId aTransactionId,
                                      TInt aReason)
	{
	TSglQueIter<CSipDialogItem> iter(iList);
	for (CSipDialogItem* dialog = iter; dialog; dialog = iter++)
		{
		if (dialog->TransactionItems().HasItem(aTransactionId))
			{
			dialog->EndTransaction(aTransactionId,aReason);
			}
		}
	}
