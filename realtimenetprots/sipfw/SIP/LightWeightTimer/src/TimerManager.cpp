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
// Name          : timermanager.cpp
// Part of       : LightWeightTimer
// Version       : SIP/4.0
//



#include "SipAssert.h"
#include "TimerManager.h"
#include "timerrequest.h"
#include "timerstore.h"
#include "singletimer.h"
#include "timerlog.h"


// -----------------------------------------------------------------------------
// CTimerManager::NewL
// -----------------------------------------------------------------------------
//
CTimerManager* CTimerManager::NewL()
	{
	CTimerManager* self = CTimerManager::NewLC();	
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CTimerManager::NewLC
// -----------------------------------------------------------------------------
//
CTimerManager* CTimerManager::NewLC()
	{
	CTimerManager* self = new (ELeave) CTimerManager();
	CleanupStack::PushL(self);	
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CTimerManager::CTimerManager
// -----------------------------------------------------------------------------
//
CTimerManager::CTimerManager() :
	iTimerIdCounter(KReservedTimer),
	iNowExpiring(EFalse)
	{
	}

// -----------------------------------------------------------------------------
// CTimerManager::ConstructL
// -----------------------------------------------------------------------------
//
void CTimerManager::ConstructL()
	{
	iTimerStore = CTimerStore::NewL();
	iTimer = CSingleTimer::NewL(*this);
	}

// -----------------------------------------------------------------------------
// CTimerManager::~CTimerManager
// -----------------------------------------------------------------------------
//
CTimerManager::~CTimerManager()
	{
	delete iTimerStore;
	delete iTimer;
	}

// -----------------------------------------------------------------------------
// CTimerManager::StartL
// -----------------------------------------------------------------------------
//
TTimerId
CTimerManager::StartL(MExpirationHandler* aObserver, TUint aMilliseconds)
	{
	__TEST_INVARIANT;
	__ASSERT_ALWAYS(aObserver, User::Leave(KErrArgument));

	return StartL(aObserver, aMilliseconds, NULL);
	}

// -----------------------------------------------------------------------------
// CTimerManager::StartL
// -----------------------------------------------------------------------------
//
TTimerId CTimerManager::StartL(MExpirationHandler* aObserver,
							   TUint aMilliseconds,
							   TAny* aTimerParam)
	{
	__TEST_INVARIANT;
	__ASSERT_ALWAYS(aObserver, User::Leave(KErrArgument));
	
    TTimerId newTimerId(NewTimerId());

#if defined(WRITE_TIMER_LOG)
	__SIP_INT_LOG2( "LwTimer StartL (id,duration)", newTimerId, aMilliseconds )
#endif

	iTimerStore->AddL(aObserver,
					  MillisecToTTime(aMilliseconds),
					  newTimerId,
					  aTimerParam);		

	//If iNowExpiring is true, timer can't be set now, as execution is inside
	//MExpirationHandler::TimerExpiredL. Timer will be set when the control
	//returns from callback to CTimerManager::TimerExpiredL.
	if (!iNowExpiring)
		{
		CheckIsNewTimerShortestL(newTimerId, aMilliseconds);
		}

	__TEST_INVARIANT;
	return newTimerId;
	}

// -----------------------------------------------------------------------------
// CTimerManager::Stop
// User is not allowed to stop the internal timer
// -----------------------------------------------------------------------------
//
TInt CTimerManager::Stop(TTimerId aTimerId)
	{
	__TEST_INVARIANT;

#if defined(WRITE_TIMER_LOG)
	__SIP_INT_LOG1( "LwTimer Stop, timer id", aTimerId)
#endif

	if (aTimerId == KReservedTimer)
		{
		return KErrNotFound;
		}

	TInt index(0);
	const CTimerRequest* timerReq = iTimerStore->SearchById(aTimerId, index);
	if (!timerReq)
		{
		return KErrNotFound;
		}

	if (!iTimerStore->Remove(index))
		{
		return KErrNotFound;
		}

	//If Stop is called from MExpirationHandler::TimerExpiredL, this condition
	//isn't true, as iTimer has TimerId of the expiring timer which was removed
	//from iTimerStore. So no need to check iNowExpiring flag now.
	if (aTimerId == iTimer->TimerId())
		{
		//The currently running timer was stopped.
		iTimer->Cancel();

		TRAPD(err, SetTimerWithShortestValueL());
		if (err != KErrNone)
			{
			return err;
			}
		}

	__TEST_INVARIANT;
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTimerManager::IsRunning
// Don't reveal the internal timer to user.
// -----------------------------------------------------------------------------
//
TBool CTimerManager::IsRunning(TTimerId aTimerId) const
	{
	__TEST_INVARIANT;

	if (aTimerId == KReservedTimer)
		{
		return EFalse;
		}

	TInt dummyIndex(0);
	return (iTimerStore->SearchById(aTimerId, dummyIndex) != NULL);
	}

// -----------------------------------------------------------------------------
// CTimerManager::ExpiresAfter
// -----------------------------------------------------------------------------
//
TInt CTimerManager::ExpiresAfter(
    TTimerId aTimerId, 
    TUint& aExpiresAfterInMillisecs) const
    {
    __TEST_INVARIANT;
    
    aExpiresAfterInMillisecs = 0;

    TInt index(0);
	const CTimerRequest* timerReq = iTimerStore->SearchById(aTimerId, index);
	if (!timerReq)
		{
		return KErrNotFound;
		}
	
	TTimeToMillisec(timerReq->ExpirationTime(), aExpiresAfterInMillisecs);
		
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CTimerManager::TimerExpiredL
// SearchById can return NULL, e.g. in a following case:
// - User has set two timers with very similar durations and they are the only
//   currently running timers.
// - User stops the shorter timer, CTimerManager::Stop uses
//   SetTimerWithShortestValueL but as the remaining timer's expiration is so
//   close to the first timer, CSingleTimer uses CSingleTimer::ExpireImmediately
//-  Before CSingleTimer::RunL has chance to be executed, user stops also the
//   remaining timer, which is removed from CTimerStore.
//-  Soon CSingleTimer::RunL is called and leads here but the expiring timer has
//   already been removed and CTimerStore::SearchById returns NULL.
// -----------------------------------------------------------------------------
//
void CTimerManager::TimerExpiredL(TTimerId aTimerId)
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aTimerId != KNoSuchTimer, KErrArgument);

	TInt index(0);
	CTimerRequest* timerReq = iTimerStore->SearchById(aTimerId, index);

	if (timerReq)
		{
#if defined(WRITE_TIMER_LOG)
    __SIP_INT_LOG1( "LwTimer TimerExpiredL id", aTimerId )
#endif
		//Store callback and parameter before freeing timerReq
		MExpirationHandler* notifier = timerReq->Observer();
		TAny* timerParam = timerReq->TimerParam();

		//Remove expired item BEFORE calling TimerExpiredL.
		//If user stops the expiring timer withing TimerExpiredL, would crash
		//as the timer would be used here after TimerExpiredL.
		iTimerStore->Remove(index);

		//Notifier can be NULL for the internal timer
		if (notifier)
			{
			iNowExpiring = ETrue;
			notifier->TimerExpiredL(aTimerId, timerParam);
			iNowExpiring = EFalse;
			}		
		}

	SetTimerWithShortestValueL();

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTimerManager::LeaveOccurred
// Do things that were not done in CTimerManager::TimerExpiredL due to leave.
// Clear iNowExpiring flag and start the next timer.
// -----------------------------------------------------------------------------
//
TInt CTimerManager::LeaveOccurred()
	{
    __TEST_INVARIANT;

	iNowExpiring = EFalse;
	TRAPD(err, SetTimerWithShortestValueL());

    __TEST_INVARIANT;
	return err;
	}

// -----------------------------------------------------------------------------
// CTimerManager::NewTimerId
// -----------------------------------------------------------------------------
//
TTimerId CTimerManager::NewTimerId()
	{
	__TEST_INVARIANT;

	iTimerIdCounter++;
	if (iTimerIdCounter == KNoSuchTimer || iTimerIdCounter == KReservedTimer)
		{
		iTimerIdCounter = KReservedTimer + 1;
		}

	__TEST_INVARIANT;
	return iTimerIdCounter;
	}

// -----------------------------------------------------------------------------
// CTimerManager::MillisecToTTime
// -----------------------------------------------------------------------------
//
TTime CTimerManager::MillisecToTTime(TUint aMilliseconds) const
	{
	__TEST_INVARIANT;

	TTime expirationTime;
	expirationTime.HomeTime();

	TTimeIntervalSeconds sec = aMilliseconds/1000;
	expirationTime += sec;

	TTimeIntervalMicroSeconds us = (TInt64)(aMilliseconds % 1000) * 1000;
	expirationTime += us;

	return expirationTime;
	}

// -----------------------------------------------------------------------------
// CTimerManager::TTimeToMillisec
// -----------------------------------------------------------------------------
//
TBool CTimerManager::TTimeToMillisec(TTime aTime, TUint& aMilliseconds) const
	{
	__TEST_INVARIANT;

	TTime now;
	now.HomeTime();

	TTimeIntervalSeconds deltaSeconds(0);
	if (aTime.SecondsFrom(now, deltaSeconds) != KErrNone)
		{
		return EFalse;
		}

	TTimeIntervalMicroSeconds deltaMicrosecs = aTime.MicroSecondsFrom(now);

	if (deltaSeconds.Int() < 0 || deltaMicrosecs.Int64() < 0)
		{
		//Timer should've already expired. Expire now. Can happen in debugger or
		//if there are so many short timers they can't be handled fast enough.
		aMilliseconds = 0;
		return ETrue;
		}

    // Set a value anyway but it should not be used for timer if EFalse is 
    // returned
    aMilliseconds = deltaSeconds.Int() * 1000;
    
	//"+ 1" is the worst case for the millisecond part
    if ((deltaSeconds.Int() + 1) > CSingleTimer::KMaxTimerAfterDuration / 1000)
		{
		return EFalse;
		}

	TInt64 milliseconds = (deltaMicrosecs.Int64()/1000) % 1000;
	//As milliseconds is always 0..999, the low 32 bits are enough
	aMilliseconds += I64LOW(milliseconds);

	return ETrue;
	}

// -----------------------------------------------------------------------------
// CTimerManager::CheckIsNewTimerShortestL
// If duration is too long for CSingleTimer::SetAfter, simulate it by chaining
// many shorter timers together. RTimer::At is avoided as it may end with
// KErrAbort ~23 minutes after setting it.
// -----------------------------------------------------------------------------
//
void CTimerManager::CheckIsNewTimerShortestL(TTimerId aTimerId,
											 TUint aMilliseconds)
	{
	__TEST_INVARIANT;

	TTimerId shortestTimerId(0);
	TTime shortestExpirationTime;

	if (iTimerStore->ShortestTimer(shortestTimerId, shortestExpirationTime))
		{
		if (shortestTimerId == aTimerId)
			{
			//The new timer is shorter than any of the existing, stop iTimer and
			//start it with the new expiration time.
			iTimer->Cancel();

            if (aMilliseconds <= CSingleTimer::KMaxTimerAfterDuration)
				{
				iTimer->SetAfter(aMilliseconds, aTimerId);
				}
			else
				{
				SetInternalTimerL();
				}
			}
		}
	else
		{
		//There must be at least one timer (the one just created).
		__ASSERT_DEBUG(EFalse,
			User::Panic(_L("TimerMgr:CheckIsNewTimerShortestL no timers"),
						KErrNotFound));
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTimerManager::SetTimerWithShortestValueL
// -----------------------------------------------------------------------------
//
void CTimerManager::SetTimerWithShortestValueL()
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(!iTimer->IsActive(), KErrNotReady);

	TTimerId timerId(0);
	TTime expirationTime;
	
	if (iTimerStore->ShortestTimer(timerId, expirationTime))
		{
		TUint milliseconds(0);

		if (TTimeToMillisec(expirationTime, milliseconds))
			{
			iTimer->SetAfter(milliseconds, timerId);
			}
		else
			{
			SetInternalTimerL();
			}
		}

#if defined(WRITE_TIMER_LOG)
	__SIP_INT_LOG1( "LwTimer SetTimerWithShortestValueL id", timerId )
#endif

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTimerManager::SetInternalTimerL
// -----------------------------------------------------------------------------
//
void CTimerManager::SetInternalTimerL() const
	{
    __TEST_INVARIANT;

	iTimerStore->AddL(NULL,
                      MillisecToTTime(CSingleTimer::KInternalTimerDuration),
					  KReservedTimer,
					  NULL);
    iTimer->SetAfter(CSingleTimer::KInternalTimerDuration, KReservedTimer);
	}

// -----------------------------------------------------------------------------
// CTimerManager::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CTimerManager::__DbgTestInvariant() const
	{
	if (!iTimerStore || !iTimer)
		{
		User::Invariant();
		}
	}
