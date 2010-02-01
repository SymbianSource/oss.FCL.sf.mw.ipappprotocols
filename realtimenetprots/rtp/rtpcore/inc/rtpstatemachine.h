// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header for class specific to state machine handling
// 
//

/**
 @file
 @internalComponent
*/

#if !defined(RTP_STATEMACHINE_H__)
#define RTP_STATEMACHINE_H__

#include <e32std.h>
#include <e32base.h>
#include "rtpsourceentry.h"

/**
Entries MAY be deleted from the table when an RTCP BYE packet with the corresponding SSRC identifier is received, 
except that some straggler data packets might arrive after the BYE 
and cause the entry to be recreated.  
Instead, the entry SHOULD be marked as having received a BYE and then deleted 
after an appropriate delay.
*/
const TUint KRtcpByeTimeoutMultiplier = 5;
const TUint KRtcpShortTimeoutMultiplier = 5;
//
/** Long timeout of 30 min in microseconds */
const TInt KRtpLongTimeout = 1800000000;
//
class CRtpStateMachine;


NONSHARABLE_CLASS( CRtpState) : public CBase
	{
	
public:

	//virtual TRtpSourceState ProcessL(TRtpSourceEvent aEvent) =0;
	virtual TRtpSourceState ProcessL(CRtpSourceEntry& aSourceEntry,TRtpSourceEvent aEvent,
										 TUint16 aSequenceNo) =0;

	inline TRtpSourceState State();
protected:
	CRtpState(TRtpSourceState aState,CRtpStateMachine& aStateMachine);
	TRtpSourceState iState;

	CRtpStateMachine* iStateMachine;	
};

NONSHARABLE_CLASS(CRtpInitialisingState) : public CRtpState
	{
public:
	static CRtpInitialisingState* NewLC(CRtpStateMachine& aStateMachine);
	// from CRtpState
	virtual TRtpSourceState ProcessL(CRtpSourceEntry& aSourceEntry,TRtpSourceEvent aEvent,
										 TUint16 aSequenceNo);
private:
	CRtpInitialisingState(CRtpStateMachine& aStateMachine);
	};

NONSHARABLE_CLASS(CRtpProbationState) : public CRtpState
	{
	friend class CRtpStateMachine;
public:
	static CRtpProbationState* NewLC(CRtpStateMachine& aStateMachine/*,TUint aMinSequential*/);
	// from CRtpState
	virtual TRtpSourceState ProcessL(CRtpSourceEntry& aSourceEntry,TRtpSourceEvent aEvent,
									TUint16 aSequenceNo);
private:
	CRtpProbationState(CRtpStateMachine& aStateMachine/*,TUint aMinSequential*/);
	//void SetMinSequential(TUint aMinSequential);
private:
	// Sequence of packets required till source is valid
	//const TInt iProbationCount;
	};

NONSHARABLE_CLASS(CRtpValidState) : public CRtpState
	{
public:
	static CRtpValidState* NewLC(CRtpStateMachine& aStateMachine);
	// from CRtpState
	virtual TRtpSourceState ProcessL(CRtpSourceEntry& aSourceEntry,TRtpSourceEvent aEvent,
										 TUint16 aSequenceNo);
private:
	CRtpValidState(CRtpStateMachine& aStateMachine);
	};

NONSHARABLE_CLASS( CRtpInvalidState) : public CRtpState
	{
public:
	static CRtpInvalidState* NewLC(CRtpStateMachine& aStateMachine);
	// from CRtpState
	virtual TRtpSourceState ProcessL(CRtpSourceEntry& aSourceEntry,TRtpSourceEvent aEvent,
									TUint16 aSequenceNo);
private:
	CRtpInvalidState(CRtpStateMachine& aStateMachine);
	};

NONSHARABLE_CLASS(CRtpInactiveState) : public CRtpState
	{
public:
	static CRtpInactiveState* NewLC(CRtpStateMachine& aStateMachine);
	// from CRtpState
	virtual TRtpSourceState ProcessL(CRtpSourceEntry& aSourceEntry,TRtpSourceEvent aEvent,
									TUint16 aSequenceNo);
private:
	CRtpInactiveState(CRtpStateMachine& aStateMachine);
	};

NONSHARABLE_CLASS(CRtpByeState) : public CRtpState
	{
public:
	static CRtpByeState* NewLC(CRtpStateMachine& aStateMachine);
	// from CRtpState
	virtual TRtpSourceState ProcessL(CRtpSourceEntry& aSourceEntry,TRtpSourceEvent aEvent,
										 TUint16 aSequenceNo);
private:
	CRtpByeState(CRtpStateMachine& aStateMachine);
	};

NONSHARABLE_CLASS( CRtpDeletedState ): public CRtpState
	{
public:
	static CRtpDeletedState* NewLC(CRtpStateMachine& aStateMachine);
	// from CRtpState
	virtual TRtpSourceState ProcessL(CRtpSourceEntry& aSourceEntry,TRtpSourceEvent aEvent,
										 TUint16 aSequenceNo);
private:
	CRtpDeletedState(CRtpStateMachine& aStateMachine);
	};



NONSHARABLE_CLASS(CRtpStateMachine) : public CBase

	{
friend class CRtcpSessionData; 
public:
	static CRtpStateMachine* NewL(TInt aPriority);
	virtual ~CRtpStateMachine();

	void SetRtpStreamParameters(TUint aMaxDropout,TUint aMaxMisorder,
								TUint aMinSequential);
	void StartL();
	
	//TRtpSourceState NextL(CRtpSSRCEntry& aSourceEntry,TRtpSourceEvent aEvent,
	TRtpSourceState NextL(CRtpSourceEntry& aSourceEntry,TRtpSourceEvent aEvent,
						TUint16 aSequenceNo);
	//
	inline TUint MaxDropout();
    inline TUint MaxMisorder();
    inline TUint MinSequential();
	//
	TTimeIntervalMicroSeconds32 ByeTimeout();
	TTimeIntervalMicroSeconds32 ShortTimeout();
	TTimeIntervalMicroSeconds32 LongTimeout();
protected:
	
	CRtpStateMachine();
	
	void ConstructL(TInt aPriority);
	
	inline void SetRtcpTransmissionInterval(TTimeIntervalMicroSeconds32& aInterval);
private:
	
	TUint iMaxDropout;
    TUint iMaxMisorder;
    TUint iMinSequential;
	// RTCP interval is reqularly updated from CRtcpSessionData::StartTimer()
	TTimeIntervalMicroSeconds32 iRtcpTransmissionInterval;
	
	CDeltaTimer* iTimerQueue;
	RPointerArray<CRtpState> iStateHandlers;
	
};


inline TRtpSourceState CRtpState::State()
	{
	return iState;
	}

inline TUint CRtpStateMachine::MaxDropout()
	{
	return iMaxDropout;
	}

inline TUint CRtpStateMachine::MaxMisorder()
	{
	return iMaxMisorder;
	}

inline TUint CRtpStateMachine::MinSequential()
	{
	return iMinSequential;
	}



inline CRtpStateMachine::CRtpStateMachine()
	{
	}

inline void CRtpStateMachine::SetRtcpTransmissionInterval(TTimeIntervalMicroSeconds32& aInterval)
	{
	iRtcpTransmissionInterval = aInterval;
	}

#endif

