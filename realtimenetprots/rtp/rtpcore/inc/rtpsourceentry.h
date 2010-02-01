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
// Header for class specific to rtp source statistics
// 
//

/**
 @file
 @internalComponent
*/

#if !defined(RTP_SOURCE_ENTRY_H__)
#define RTP_SOURCE_ENTRY_H__

#include <e32base.h>
#include <es_sock.h>
#include "rtpsock.h"
#include "rtputils.h"

//

const TInt KRtpFixedNoCSRCHeaderLength = 12;

const TInt KRtpSequenceMod = (1<<16);

//#define RTP_MAX_SDES 255      // maximum text length for SDES


//
class RRtpSocket;
class CRtpStateMachine;

enum TRtpSourceState
	{
		ERtpInitialising,
		ERtpProbation,
		ERtpValid,
		ERtpInvalid,
		ERtpInactive,
		ERtpBye,
		ERtpDelete
	};

enum TRtpSourceEvent
	{
		ERtpNone,
		ERtpPacketReceived,
		ERtpShortTimeout,
		ERtpLongTimeout,
		ERtpByeTimeout,
		ERtcpPacketReceived,
		ERtcpByeReceived
	};
//



class CRtpController;

NONSHARABLE_CLASS(CRtpSourceEntry ): public CBase
	{
	friend class CRtpCollisionMng;
public:
	static CRtpSourceEntry* NewLC(TUint32 aSSRC,const TDesC8& aCNAME,CRtpController* aController);
	virtual ~CRtpSourceEntry();

	static TInt HandleTimeoutL(TAny* aSourceEntry);

	void SetCnameL(const TDesC8& aCNAME);
	const TDesC8& Cname();

	inline TUint SRC() const;
	inline const TSockAddr& RtpSourceAddr() const;
	inline const TSockAddr& RtcpSourceAddr() const;
	// is it OK to be public for addr setters???
	inline void SetRtpSourceAddr(TSockAddr& aRtpAddr);
	inline void SetRtcpSourceAddr(TSockAddr& aRtcpAddr);

	TBool IsValidSequence(TUint16 aSequenceNo,TUint aMaxDropout,TUint aMaxMisorder);

	static TInt CompareSSRC(const CRtpSourceEntry& aFirst,const CRtpSourceEntry& aSecond);
	// access to RTP statistics
	void Reset(TRtpSourceState aState,TUint aProbationCount = KRtpDefaultMinSequential);
	inline TRtpSourceState State();
	inline TUint16 MaxSequenceNo();
	inline void SetMaxSequenceNo(TUint16 aMaxSeq);
	inline TUint32 BadSequenceNo();
	inline void SetBadSequenceNo(TUint32 aBadSeq);
	inline void IncPacketsReceived();
	inline void IncShiftedCycles(TUint32 aCycles);
	inline TDeltaTimerEntry& DeltaTimerEntry();
	void SetDeltaTimerEntry(TCallBack& aCallBack,TRtpSourceEvent aEvent);
	void ClearDeltaTimerEntry();
	inline TBool IsTimerUsed();
	void InitialiseSequenceNo(TUint16 aSequenceNo);
	TBool IsSequenceInitialised();
	void ResetRtcpStatistics();
	void UpdateRtcpStatisticsL(TUint aRtpTimeFromPacket,TUint aPayloadType);
	inline void SetLastSRTimestamp(TUint32 aLsr); 
	inline void SetWhenLastSRReceived(TUint32 aTs);
	inline void SetExpectedPrior(TUint32 aExpected);
    inline void SetReceivedPrior(TUint32 aReceived);
	inline void IncOctetsSent(const TInt aCount);
	inline void IncPacketsSent();
	//
	inline TInt ProbationCount();
	inline void SetProbationCount(TInt aCount);
	inline TInt DecProbationCount();
	// other RTCP getters
	inline TUint32 ShiftedCycles();
    inline TUint32 BaseSequenceNo();
    inline TUint32 PacketsReceived();
    inline TUint32 ExpectedPrior();
    inline TUint32 ReceivedPrior();
    inline TUint32 RelTransitTime();
    inline TUint32 Jitter();
	inline TUint32 LastSRTimestamp(); 
	inline TUint32 WhenLastSRReceived();
	inline TUint32 PacketsSent();
	inline TUint32 OctetsSent();
	inline void SetState(TRtpSourceState aState);
		
private:
	CRtpSourceEntry(TUint32 aSRC);
	void ConstructL(const TDesC8& aCNAME);
	void SetSRC(TUint32 aSRC);
	//inline void SetRtpSourceAddr(TSockAddr& aSourceAddr);
	//inline void SetRtcpSourceAddr(TSockAddr& aSourceAddr);
	// delta timer stuff
	TInt DoHandleTimeoutL();
private:
	CRtpController* iController;
	
	// source idetifier - either SSRC or CSRC
	TUint32 iSRC;
	TSockAddr iRtpSourceAddr;
	TSockAddr iRtcpSourceAddr;
	/** CNAME: Canonical end-point identifier SDES item */
	HBufC8* iCNAME;
	TBool iIsInConflictList;
	TUint iCollisionCount;
	TUint iLoopbackCount;
	TRtpSourceState iState;
	TInt iProbationCount;
	//timer entry for this source
	TDeltaTimerEntry iDeltaTimerEntry;
	TRtpSourceEvent iTimerEvent; // event associated with the delta timer
	TBool iIsTimerUsed;
	//
	/** The highest seq. number seen for this source in this session */
	TUint16 iMaxSequenceNo;        // max_seq
	/** shifted count of seq. number cycles */
    TUint32 iShiftedCycles; //cycles
	/** base seq number */
    TUint32 iBaseSequenceNo; //base_seq;
	/** last 'bad' seq number + 1 */
    TUint32 iBadSequenceNo; //bad_seq;
	/** packets received */
    TUint32 iPacketsReceived; //received;
	/** packet expected at last interval */
    TUint32 iExpectedPrior; // expected_prior
	/** packet received at last interval */
    TUint32 iReceivedPrior; // received_prior
	/** relative trans time for prev pkt */
    TUint32 iRelTransitTime; // transit
	/** estimated jitter */
    TUint32 iJitter; // jitter
	/** RTCP stats */
	// do we need that or have we got enough in above?
	//struct SRtcpRRBlock iRtcpRRBlock;
	/** The middle 32 bits out of 64 in the NTP timestamp received as part 
	of the most recent RTCP sender report (SR) packet from this source 
	NOTE: Time as recorded in the SR packet*/
	TUint32 iLastSRTimestamp; // (LSR): 32 bits
	/** The middle 32 bits out of 64 in the NTP timestamp whe the most recent RTCP sender report 
	(SR) packet from this source was received
	NOTE: Actual time when the SR packet received*/
	TUint32 iWhenLastSRReceived;

	/** sender's packet count
    The total number of RTP data packets transmitted by the sender
    since starting transmission.
    The count is reset if the sender changes its SSRC identifier. */
	TUint32 iPacketsSent;
	/** sender's octet count
    The total number of payload octets (i.e., not including header or padding) 
	transmitted in RTP data packets by the sender since starting transmission 
	The count is reset if the sender changes its SSRC identifier. */
    TUint32 iOctetsSent;
    /* Tracks the Receiver Specific RTP Time */
    TRTPTime iRtpTime;
	};


#include "rtpsourceentry.inl"

#endif

