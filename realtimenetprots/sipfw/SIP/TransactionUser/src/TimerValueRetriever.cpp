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
// Name          : TimerValueRetriever.cpp
// Part of       : TransactionUser
// Version       : SIP/5.0
//



#include <centralrepository.h>
#include "SIPSDKCRKeys.h"
#include "SipAssert.h"
#include "TimerValueRetriever.h"


// -----------------------------------------------------------------------------
// CTimerValueRetriever::NewL
// -----------------------------------------------------------------------------
//
CTimerValueRetriever* CTimerValueRetriever::NewL()
	{
    CTimerValueRetriever* self = new (ELeave) CTimerValueRetriever();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CTimerValueRetriever::CTimerValueRetriever
// -----------------------------------------------------------------------------
//
CTimerValueRetriever::CTimerValueRetriever() :
	CActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

// -----------------------------------------------------------------------------
// CTimerValueRetriever::ConstructL
// -----------------------------------------------------------------------------
//
void CTimerValueRetriever::ConstructL()
	{
	iRepository = CRepository::NewL(KCRUidSIP);
	ReadValuesL();
	for(TUint i = 0; i < EMaxSupportedBearer;i++)
		__ASSERT_ALWAYS(iTimerValues[i].CheckValues(), User::Leave(KErrCorrupt));
	ListenForChangesL();
	}

// -----------------------------------------------------------------------------
// CTimerValueRetriever::~CTimerValueRetriever
// -----------------------------------------------------------------------------
//
CTimerValueRetriever::~CTimerValueRetriever()
	{
	Cancel();
	delete iRepository;
	}

// -----------------------------------------------------------------------------
// CTimerValueRetriever::DoCancel
// -----------------------------------------------------------------------------
//
void CTimerValueRetriever::DoCancel()
	{
	if (iRepository)
		{
		iRepository->NotifyCancelAll();
		}
	}

// -----------------------------------------------------------------------------
// CTimerValueRetriever::RunL
// -----------------------------------------------------------------------------
//
void CTimerValueRetriever::RunL()
	{
	__SIP_ASSERT_LEAVE(iRepository, KErrNotFound);

	// In S60, repository completes the notification request with value 1, even
	// though there is no error.
	if (iStatus.Int() >= KErrNone)
		{
		ReadValuesL();
		}

	ListenForChangesL();
	}

// -----------------------------------------------------------------------------
// CTimerValueRetriever::RunError
// -----------------------------------------------------------------------------
//
TInt CTimerValueRetriever::RunError(TInt aError)
	{
	return (aError == KErrNoMemory) ? aError : KErrNone;
	}

// -----------------------------------------------------------------------------
// CTimerValueRetriever::TimerValues
// -----------------------------------------------------------------------------
//
TTimerValues& CTimerValueRetriever::TimerValues(BearerType aType)
	{
	if(EMaxSupportedBearer > aType)
		return iTimerValues[aType];
	else
		return iTimerValues[EModemBearer];
	}
	
// -----------------------------------------------------------------------------
// CTimerValueRetriever::ReadValuesL
// If the values read from repository are invalid, use the previous values.
// -----------------------------------------------------------------------------
//
void CTimerValueRetriever::ReadValuesL()
	{
	__SIP_ASSERT_LEAVE(iRepository, KErrNotFound);

    TInt t1 = 0;
    TInt t2 = 0;
    TInt t4 = 0;

    User::LeaveIfError(iRepository->Get(KSIPTransactionTimerT1, t1));
    User::LeaveIfError(iRepository->Get(KSIPTransactionTimerT2, t2));
    User::LeaveIfError(iRepository->Get(KSIPTransactionTimerT4, t4));

	TTimerValues newMBValues(t1, t2, t4);
	if (newMBValues.CheckValues())
		{
		iTimerValues[EModemBearer] = newMBValues;
		}
	
	User::LeaveIfError(iRepository->Get(KSIPTransactionTimerLANT1, t1));
	User::LeaveIfError(iRepository->Get(KSIPTransactionTimerLANT2, t2));
	User::LeaveIfError(iRepository->Get(KSIPTransactionTimerLANT4, t4));

	TTimerValues newLBValues(t1, t2, t4);
	if (newLBValues.CheckValues())
		{
		iTimerValues[ELanBearer] = newLBValues;
		}
    }

// -----------------------------------------------------------------------------
// CTimerValueRetriever::ListenForChangesL
// -----------------------------------------------------------------------------
//
void CTimerValueRetriever::ListenForChangesL()
	{
	__SIP_ASSERT_LEAVE(iRepository, KErrNotFound);

	const TUint32 KDummyPartialKey = 0x0;
	//This mask means listening for all keys 
	const TUint32 KMaskIncludingAllKeys = 0x0;

	User::LeaveIfError(iRepository->NotifyRequest(KDummyPartialKey,
												  KMaskIncludingAllKeys,
												  iStatus));
	SetActive();
	}

// -----------------------------------------------------------------------------
// CTimerValueRetriever::HighestT1Value
// -----------------------------------------------------------------------------
//
TUint CTimerValueRetriever::HighestT1()
	{
	TUint t1 = 0;
	for(TUint i = 0; i < EMaxSupportedBearer;i++)
		if(iTimerValues[i].T1() > t1)
			t1 = iTimerValues[i].T1();
	return t1;
	}
