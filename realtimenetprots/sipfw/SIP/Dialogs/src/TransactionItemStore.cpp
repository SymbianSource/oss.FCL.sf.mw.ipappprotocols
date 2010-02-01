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
// Name          : TransactionItemStore.cpp
// Part of       : SIPDialogs
// Version       : SIP/4.0 
//



#include "SipStackServerDefs.h"
#include "TransactionItemStore.h"
#include "TransactionItem.h"
#include "RefreshItem.h"
#include "siprequest.h"

// -----------------------------------------------------------------------------
// CTransactionItemStore::NewL
// -----------------------------------------------------------------------------
//
CTransactionItemStore* CTransactionItemStore::NewL ()
	{
	CTransactionItemStore* self = CTransactionItemStore::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CTransactionItemStore::NewLC
// -----------------------------------------------------------------------------
//
CTransactionItemStore* CTransactionItemStore::NewLC ()
	{
    CTransactionItemStore* self = new(ELeave)CTransactionItemStore();
	CleanupStack::PushL(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CTransactionItemStore::CTransactionItemStore
// -----------------------------------------------------------------------------
//
CTransactionItemStore::CTransactionItemStore () 
: iList(CTransactionItemBase::Offset())
	{
	}

// -----------------------------------------------------------------------------
// CTransactionItemStore::~CTransactionItemStore
// -----------------------------------------------------------------------------
//
CTransactionItemStore::~CTransactionItemStore()
	{
	TSglQueIter<CTransactionItemBase> iter(iList);
	for (CTransactionItemBase* item = iter++; item; item = iter++)
		{
		iList.Remove(*item);
		delete item;
    	}
	}

// -----------------------------------------------------------------------------
// CTransactionItemStore::AddItem
// -----------------------------------------------------------------------------
//
void CTransactionItemStore::AddItem(CTransactionItemBase* aTransactionItem)
	{
	iList.AddLast(*aTransactionItem);
	}

// -----------------------------------------------------------------------------
// CTransactionItemStore::HasItem
// -----------------------------------------------------------------------------
//
TBool CTransactionItemStore::HasItem (TTransactionId aTransactionId)
    {
    if (!FindItem(aTransactionId))
        {
        return EFalse;
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CTransactionItemStore::FindItem
// -----------------------------------------------------------------------------
//
CTransactionItemBase* 
CTransactionItemStore::FindItem (TTransactionId aTransactionId)
    {
    CTransactionItemBase* foundItem = NULL;
	TSglQueIter<CTransactionItemBase> iter(iList); 
	for (CTransactionItemBase* item = iter++; item && !foundItem; item = iter++)
		{
        if (item->Id() == aTransactionId)
            {
            foundItem = item;
            }
		}
	return foundItem;
    }

// -----------------------------------------------------------------------------
// CTransactionItemStore::RemoveItem
// -----------------------------------------------------------------------------
//
TInt CTransactionItemStore::RemoveItem (TTransactionId aTransactionId)
	{
    TInt err = KErrNotFound;	
	CTransactionItemBase* item = FindItem(aTransactionId);
	if (item)
	    {
		iList.Remove(*item);
		delete item;
        err = KErrNone;
		}
    return err;
    }

// -----------------------------------------------------------------------------
// CTransactionItemStore::OverlappingRestricted
// -----------------------------------------------------------------------------
//
TBool CTransactionItemStore::OverlappingRestricted (RStringF aMethod)
    {
	TSglQueIter<CTransactionItemBase> iter(iList);
	for (CTransactionItemBase* item = iter++; item; item = iter++)
		{
		if (item->OverlappingRestricted() && item->Method() == aMethod)
            {
            return ETrue;
		    }
        }
	return EFalse;
    }

// -----------------------------------------------------------------------------
// CTransactionItemStore::HasRefreshedItems
// -----------------------------------------------------------------------------
//
TBool CTransactionItemStore::HasRefreshedItems ()
	{
    TBool found = EFalse;
	TSglQueIter<CTransactionItemBase> iter(iList);
	for (CTransactionItemBase* item = iter++; item && !found; item = iter++)
		{
        if(item->IsRefreshed())
            {
            found = ETrue;
            }
        }
	return found;
	}

// -----------------------------------------------------------------------------
// CTransactionItemStore::FindRefreshItem
// -----------------------------------------------------------------------------
//
CRefreshItem* 
CTransactionItemStore::FindRefreshItem (RStringF aMethod)
    {
    CRefreshItem* foundItem = 0;
	TSglQueIter<CTransactionItemBase> iter(iList);
	for (CTransactionItemBase* item = iter++; item && !foundItem; item = iter++)
		{
		if (item->IsRefreshed() && item->Method() == aMethod)
			{
			foundItem = static_cast<CRefreshItem*>(item);
			}
		}
	return foundItem;
    }

// -----------------------------------------------------------------------------
// CTransactionItemStore::FindRefreshItem
// -----------------------------------------------------------------------------
//
CRefreshItem* CTransactionItemStore::FindRefreshItem (TRefreshId aRefreshId)
    {
    CRefreshItem* foundItem = 0;
	TSglQueIter<CTransactionItemBase> iter(iList);
	for (CTransactionItemBase* item = iter++; item && !foundItem; item = iter++)
		{
		if (item->IsRefreshed())
			{
            CRefreshItem* refreshItem = static_cast<CRefreshItem*>(item);
            if (refreshItem->RefreshId() == aRefreshId)
                {
                foundItem = refreshItem;
                }
			}
		}
	return  foundItem;
    }

// -----------------------------------------------------------------------------
// CTransactionItemStore::RefreshId
// -----------------------------------------------------------------------------
//
TRefreshId CTransactionItemStore::RefreshId (TTransactionId aTransactionId)
    {
    TRefreshId id = KEmptyRefreshId;
	TSglQueIter<CTransactionItemBase> iter(iList);
	for (CTransactionItemBase* item = iter++; item && !id; item = iter++)
		{
		if (item->IsRefreshed() && item->Id() == aTransactionId)
			{
            id = static_cast<CRefreshItem*>(item)->RefreshId();
			}
		}
	return id;
    }

// -----------------------------------------------------------------------------
// CTransactionItemStore::RefreshItemIdsL
// -----------------------------------------------------------------------------
//
RArray<TRefreshId> CTransactionItemStore::RefreshItemIdsL ()
	{
	TSglQueIter<CTransactionItemBase> iter(iList);
    RArray<TRefreshId> ids;
    CleanupClosePushL(ids);
	for (CTransactionItemBase* item = iter++; item; item = iter++)
		{
        if (item->IsRefreshed())
            {
            CRefreshItem* refreshItem = static_cast<CRefreshItem*>(item);
            User::LeaveIfError(ids.Append(refreshItem->RefreshId()));
            }
		}
    CleanupStack::Pop(1); // ids
    return ids;
	}
