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
// Name          : timerrequest.cpp
// Part of       : LightWeightTimer
// Version       : SIP/4.0
//



#include "SipAssert.h"
#include "timerrequest.h"

// -----------------------------------------------------------------------------
// CTimerRequest::NewL
// -----------------------------------------------------------------------------
//
CTimerRequest* CTimerRequest::NewL()
	{
	return new (ELeave) CTimerRequest();
	}

// -----------------------------------------------------------------------------
// CTimerRequest::NewL
// aObserver is NULL when aTimerId == MTimerManager::KReservedTimer
// -----------------------------------------------------------------------------
//
CTimerRequest* CTimerRequest::NewL(TTimerId aTimerId,
								   TTime aExpirationTime,
							       MExpirationHandler* aObserver,
							       TAny* aTimerParam)
	{
    __SIP_ASSERT_LEAVE(aTimerId != MTimerManager::KNoSuchTimer, KErrArgument);	
    __SIP_ASSERT_LEAVE(aObserver || aTimerId == MTimerManager::KReservedTimer,
                       KErrArgument);

	return new (ELeave) CTimerRequest(aTimerId,
								      aExpirationTime,
							          aObserver,
							          aTimerParam);
	}

// -----------------------------------------------------------------------------
// CTimerRequest::CTimerRequest
// iObserver is left NULL as in this case CTimerRequest is created for searches.
// -----------------------------------------------------------------------------
//
CTimerRequest::CTimerRequest() : iTimerId(MTimerManager::KNoSuchTimer)
	{
	}

// -----------------------------------------------------------------------------
// CTimerRequest::CTimerRequest
// -----------------------------------------------------------------------------
//
CTimerRequest::CTimerRequest(TTimerId aTimerId,
							 TTime aExpirationTime,
							 MExpirationHandler* aObserver,
							 TAny* aTimerParam) :
	iTimerId(aTimerId),
	iExpirationTime(aExpirationTime),
	iObserver(aObserver),
	iTimerParam(aTimerParam)
	{	
	}

// -----------------------------------------------------------------------------
// CTimerRequest::~CTimerRequest
// -----------------------------------------------------------------------------
//
CTimerRequest::~CTimerRequest()
	{
	}

// -----------------------------------------------------------------------------
// CTimerRequest::Compare
// -----------------------------------------------------------------------------
//
TInt CTimerRequest::Compare(const CTimerRequest& aItem,
							const CTimerRequest& aItem2)
	{
	if (aItem.iExpirationTime < aItem2.iExpirationTime)
        {
		return -1;
        }

	if (aItem.iExpirationTime > aItem2.iExpirationTime)
        {
		return 1;
        }

	return 0;
	}

// -----------------------------------------------------------------------------
// CTimerRequest::CompareId
// -----------------------------------------------------------------------------
//
TBool CTimerRequest::CompareId(const CTimerRequest& aItem,
							   const CTimerRequest& aItem2)
	{
	return aItem.iTimerId == aItem2.iTimerId;
	}

// -----------------------------------------------------------------------------
// CTimerRequest::TimerId
// -----------------------------------------------------------------------------
//
TTimerId CTimerRequest::TimerId() const
	{
	return iTimerId;
	}

// -----------------------------------------------------------------------------
// CTimerRequest::SetTimerId
// -----------------------------------------------------------------------------
//
void CTimerRequest::SetTimerId(TTimerId aTimerId)
	{
	iTimerId = aTimerId;
	}

// -----------------------------------------------------------------------------
// CTimerRequest::ExpirationTime
// -----------------------------------------------------------------------------
//
TTime CTimerRequest::ExpirationTime() const
	{
	return iExpirationTime;
	}

// -----------------------------------------------------------------------------
// CTimerRequest::Observer
// -----------------------------------------------------------------------------
//
MExpirationHandler* CTimerRequest::Observer()
	{
	return iObserver;
	}

// -----------------------------------------------------------------------------
// CTimerRequest::TimerParam
// -----------------------------------------------------------------------------
//
TAny* CTimerRequest::TimerParam()
	{
	return iTimerParam;
	}
