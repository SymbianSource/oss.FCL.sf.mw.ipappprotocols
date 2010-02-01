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
// RTP source entry  inline definitions
// 
//

/**
 @file
 @internalComponent
*/

inline TRtpSourceState CRtpSourceEntry::State()
	{
	return iState;	
	}

inline TUint16 CRtpSourceEntry::MaxSequenceNo()
	{
	return iMaxSequenceNo;
	}
inline void CRtpSourceEntry::SetMaxSequenceNo(TUint16 aMaxSeq)
	{
	iMaxSequenceNo = aMaxSeq;
	}
inline TUint32 CRtpSourceEntry::BadSequenceNo()
	{
	return iBadSequenceNo;
	}
inline void CRtpSourceEntry::SetBadSequenceNo(TUint32 aBadSeq)
	{
	iBadSequenceNo = aBadSeq;
	}
inline void CRtpSourceEntry::IncPacketsReceived()
	{
	iPacketsReceived++;
	}

inline void CRtpSourceEntry::IncShiftedCycles(TUint32 aCycles)
	{
	iShiftedCycles += aCycles;
	}

inline void CRtpSourceEntry::SetState(TRtpSourceState aState)
	{
	iState = aState;
	}

inline TDeltaTimerEntry& CRtpSourceEntry::DeltaTimerEntry()
	{
	return iDeltaTimerEntry;
	}

inline TBool CRtpSourceEntry::IsTimerUsed()
	{
	return iIsTimerUsed;
	}

/*
inline void CRtpSourceEntry::SetTimedEntry(struct STimedEntry& aTimedEntry)
	{
	iTimedEntry = aTimedEntry;
	}
*/
TUint CRtpSourceEntry::SRC() const
	{
	return iSRC;
	}

const TSockAddr& CRtpSourceEntry::RtpSourceAddr() const
	{
	return iRtpSourceAddr;
	}

const TSockAddr& CRtpSourceEntry::RtcpSourceAddr() const
	{
	return iRtcpSourceAddr;
	}

inline void CRtpSourceEntry::SetRtpSourceAddr(TSockAddr& aRtpAddr)
	{
	iRtpSourceAddr = aRtpAddr;
	}
inline void CRtpSourceEntry::SetRtcpSourceAddr(TSockAddr& aRtcpAddr)
	{
	iRtcpSourceAddr = aRtcpAddr;
	}
inline TInt CRtpSourceEntry::ProbationCount()
	{
	return iProbationCount;
	}
inline void CRtpSourceEntry::SetProbationCount(TInt aCount)
	{
	iProbationCount = aCount;
	}
inline TInt CRtpSourceEntry::DecProbationCount()
	{
	return iProbationCount > 0 ? --iProbationCount : 0;
	}

inline TUint32 CRtpSourceEntry::ShiftedCycles()
	{
	return iShiftedCycles;
	}

inline TUint32 CRtpSourceEntry::BaseSequenceNo()
	{
	return iBaseSequenceNo;
	}

inline TUint32 CRtpSourceEntry::PacketsReceived()
	{
	return iPacketsReceived;
	}


inline void CRtpSourceEntry::SetExpectedPrior(TUint32 aExpected)
	{
	iExpectedPrior = aExpected;
	}

inline TUint32 CRtpSourceEntry::ExpectedPrior()
	{
	return iExpectedPrior;
	}

inline void CRtpSourceEntry::SetReceivedPrior(TUint32 aReceived)
	{
	iReceivedPrior = aReceived;
	}

inline TUint32 CRtpSourceEntry::ReceivedPrior()
	{
	return iReceivedPrior;
	}

inline TUint32 CRtpSourceEntry::RelTransitTime()
	{
	return iRelTransitTime;
	}

inline TUint32 CRtpSourceEntry::Jitter()
	{
	return iJitter;
	}

inline void CRtpSourceEntry::SetLastSRTimestamp(TUint32 aLsr)
	{
	iLastSRTimestamp = aLsr;
	}

inline void CRtpSourceEntry::SetWhenLastSRReceived(TUint32 aTs)
	{
	iWhenLastSRReceived = aTs;
	}

inline TUint32 CRtpSourceEntry::LastSRTimestamp()
	{
	return iLastSRTimestamp;
	}

inline TUint32 CRtpSourceEntry::WhenLastSRReceived()
	{
	return iWhenLastSRReceived;
	}

inline TUint32 CRtpSourceEntry::PacketsSent()
	{
	return iPacketsSent;
	}

inline TUint32 CRtpSourceEntry::OctetsSent()
	{
	return iOctetsSent;
	}

inline void CRtpSourceEntry::IncPacketsSent()
	{
	iPacketsSent++;
	}

inline void CRtpSourceEntry::IncOctetsSent(const TInt aCount)
	{
	iOctetsSent += aCount;
	}


