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
// Name          : timerstore.cpp
// Part of       : LightWeightTimer
// Version       : SIP/4.0
//



#include "SipAssert.h"
#include "timerstore.h"
#include "timerrequest.h"


// -----------------------------------------------------------------------------
// CTimerStore::NewL
// -----------------------------------------------------------------------------
//
CTimerStore* CTimerStore::NewL()
	{
	CTimerStore* self = new (ELeave) CTimerStore();
	CleanupStack::PushL(self);	
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CTimerStore::CTimerStore
// For unit tests the array granularity is set to 1 to cause it to allocate
// memory every time an item is added.
// -----------------------------------------------------------------------------
//
CTimerStore::CTimerStore()
#ifdef CPPUNIT_TEST
    : iTimerRequests(1)
#endif
	{
	}

// -----------------------------------------------------------------------------
// CTimerStore::ConstructL
// -----------------------------------------------------------------------------
//
void CTimerStore::ConstructL()
	{
	iSearchCond = CTimerRequest::NewL();
	}

// -----------------------------------------------------------------------------
// CTimerStore::~CTimerStore
// -----------------------------------------------------------------------------
//
CTimerStore::~CTimerStore()
	{
	delete iSearchCond;
	
    iTimerRequests.ResetAndDestroy();	
	}

// -----------------------------------------------------------------------------
// CTimerStore::AddL
// aObserver is NULL when aTimerId == MTimerManager::KReservedTimer
// -----------------------------------------------------------------------------
//
void CTimerStore::AddL(MExpirationHandler* aObserver,
					   TTime aExpirationTime,
				       TTimerId aTimerId,
					   TAny* aTimerParam)
	{    
    __SIP_ASSERT_LEAVE(aObserver || aTimerId == MTimerManager::KReservedTimer,
		               KErrArgument);
    __SIP_ASSERT_LEAVE(aTimerId != MTimerManager::KNoSuchTimer, KErrArgument);

	CTimerRequest* newItem = CTimerRequest::NewL(aTimerId,
												 aExpirationTime,
												 aObserver,
												 aTimerParam);
	TLinearOrder<CTimerRequest> order(CTimerRequest::Compare);
	CleanupStack::PushL(newItem);
	User::LeaveIfError(iTimerRequests.InsertInOrderAllowRepeats(newItem,
                                                                order));
	CleanupStack::Pop(newItem);
	}

// -----------------------------------------------------------------------------
// CTimerStore::SearchById
// -----------------------------------------------------------------------------
//
CTimerRequest* CTimerStore::SearchById(TTimerId aTimerId, TInt& aIndex) const
	{
	//There is never a timer in store with the "empty" TimerId value
	if (aTimerId == MTimerManager::KNoSuchTimer)
        {
		return NULL;
        }

	iSearchCond->SetTimerId(aTimerId);
	TIdentityRelation<CTimerRequest> compareId(CTimerRequest::CompareId);
	aIndex = iTimerRequests.Find(iSearchCond, compareId);

	if (aIndex == KErrNotFound)
        {
		return NULL;
        }

	return iTimerRequests[aIndex];    
	}

// -----------------------------------------------------------------------------
// CTimerStore::ShortestTimer
// -----------------------------------------------------------------------------
//
TBool CTimerStore::ShortestTimer(TTimerId& aTimerId,
								 TTime& aExpirationTime) const
	{
	if (iTimerRequests.Count() < 1)
        {
		return EFalse;
        }

	aTimerId = iTimerRequests[0]->TimerId();
	aExpirationTime = iTimerRequests[0]->ExpirationTime();

	return ETrue;	
	}

// -----------------------------------------------------------------------------
// CTimerStore::Remove
// -----------------------------------------------------------------------------
//
TBool CTimerStore::Remove(TInt aIndex)
	{
    __SIP_ASSERT_RETURN_VALUE(aIndex >= 0, KErrArgument);

	if (aIndex >= iTimerRequests.Count())
        {
		return EFalse;
        }

	CTimerRequest* req = iTimerRequests[aIndex];
	iTimerRequests.Remove(aIndex);
	delete req;

	iTimerRequests.Compress();

	return ETrue;
	}
