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
// Name          : singletimer.cpp
// Part of       : LightWeightTimer
// Version       : SIP/4.0
//



#include "SipAssert.h"
#include "singletimer.h"
#include "TimerManager.h"
#include "mtimernotifier.h"
#include "timerlog.h"


// -----------------------------------------------------------------------------
// CSingleTimer::NewL
// -----------------------------------------------------------------------------
//
CSingleTimer* CSingleTimer::NewL(MTimerNotifier& aObserver)
	{
	CSingleTimer* self = new (ELeave) CSingleTimer(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSingleTimer::CSingleTimer
// -----------------------------------------------------------------------------
//
CSingleTimer::CSingleTimer(MTimerNotifier& aObserver) :
	CActive(EPriorityStandard),
	iObserver(aObserver),
	iTimerId(MTimerManager::KNoSuchTimer)
	{
	CActiveScheduler::Add(this);
	}

// -----------------------------------------------------------------------------
// CSingleTimer::ConstructL
// -----------------------------------------------------------------------------
//
void CSingleTimer::ConstructL()
	{
	User::LeaveIfError(iTimer.CreateLocal());
	}

// -----------------------------------------------------------------------------
// CSingleTimer::~CSingleTimer
// -----------------------------------------------------------------------------
//
CSingleTimer::~CSingleTimer()
	{	
	Cancel();
	iTimer.Close();
	}

// -----------------------------------------------------------------------------
// CSingleTimer::TimerId
// -----------------------------------------------------------------------------
//
TTimerId CSingleTimer::TimerId() const
	{
	return iTimerId;
	}

// -----------------------------------------------------------------------------
// CSingleTimer::SetAfter
// If the equipment is switched off, timer set with After expires late.
// -----------------------------------------------------------------------------
//
void CSingleTimer::SetAfter(TUint32 aDelay, TTimerId aTimerId)
	{	
    __SIP_ASSERT_RETURN(!IsActive(), KErrNotReady);
    __SIP_ASSERT_RETURN(aDelay <= KMaxTimerAfterDuration, KErrOverflow);
    __SIP_ASSERT_RETURN(aTimerId != CTimerManager::KNoSuchTimer, KErrArgument);

#if defined(WRITE_TIMER_LOG)
	__SIP_INT_LOG2( "LwTimer SetAfter (id,delay)", aTimerId, aDelay )
#endif

	iTimerId = aTimerId;

	//If the duration is very short, expire immediately for better performance.
#ifdef __WINSCW__
	if (aDelay < KEmulatorDelay)
#else
	if (aDelay < KTargetHwDelay)
#endif
		{
		ExpireImmediately();
		}
	else
		{
		iTimer.After(iStatus, aDelay * 1000);
		SetActive();
		}
	}

// -----------------------------------------------------------------------------
// CSingleTimer::DoCancel
// -----------------------------------------------------------------------------
//
void CSingleTimer::DoCancel()
	{
	iTimer.Cancel();
	iTimerId = MTimerManager::KNoSuchTimer;
	}

// -----------------------------------------------------------------------------
// CSingleTimer::RunL
// Even in case of error, continue normally.
// -----------------------------------------------------------------------------
//
void CSingleTimer::RunL()
	{
#if defined(WRITE_TIMER_LOG)
	if (iStatus.Int() != KErrNone)
		{
		__SIP_INT_LOG2( "LwTimer SingleTimer:RunL (status,id)", 
		                iStatus.Int(), iTimerId )
		}
#endif

	iObserver.TimerExpiredL(iTimerId);	
	}

// -----------------------------------------------------------------------------
// CSingleTimer::RunError
// -----------------------------------------------------------------------------
//
TInt CSingleTimer::RunError(TInt aError)
	{
#if defined(WRITE_TIMER_LOG)
	__SIP_INT_LOG1( "LwTimer SingleTimer:RunError error", aError )
#endif

	TInt err = iObserver.LeaveOccurred();

	if (aError == KErrNoMemory || err == KErrNoMemory)
		{
		return KErrNoMemory;
		}

	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CSingleTimer::ExpireImmediately
// Callback is not used from here, as it may lead to recursion if the callback
// sets another timer with delay = 0.
// CSingleTimer doesn't have a flag like CTimerManager::iNowExpiring.
// -----------------------------------------------------------------------------
//
void CSingleTimer::ExpireImmediately()
	{
	//Initiate request
	iStatus = KRequestPending;
	SetActive();

	//Complete request
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}
