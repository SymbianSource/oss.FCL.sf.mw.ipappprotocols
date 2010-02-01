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
// Header for class specific to RTCP session handling
// 
//

/**
 @file
 @internalComponent
*/

#if !defined(RTCP_SESSION_DATA_H__)
#define RTCP_SESSION_DATA_H__

#include <e32base.h>
#include <es_sock.h>
#include "rtpsock.h"
#include "rtputils.h"
#include "rtcpinternals.h"

/**
RFC 3550 - a session participant must maintain several pieces of state 
and this class does exactly that.
@internalComponent
*/
NONSHARABLE_CLASS(CRtcpSessionData) : public CTimer
	{
public:
	static CRtcpSessionData* NewL(TUint aRtcpBw,TBool aSendReports,
								CRtpController* aRtpController,const TDesC8& aCname,TInt aPriority = EPriorityMore);
	virtual ~CRtcpSessionData();
	
	void UpdateSdesItemsL(const TDesC8& aCname);

	void StartPeriodicReports();
	void StopPeriodicReports();
	inline TBool PeriodicReportsStatus() const { return iSendPeriodicReports; }
	void UserJoinedSession(TBool aIsSender);
	void UserLeftSession(TBool aWasSender);
	//
	TInt ComposeRTCPPacketL();
	
	void SetByePendingL(TUint32 aLocalSRC, const TDesC8& aReason);
	void SetReportPendingL(TInt aReportType);
	void SetSDesPendingL(TUint32 aSSRCOrCSRC, const TUint8 aItemType, const TDesC8& aItemValue);
	void SetPRIVL(TUint32 aSSRCOrCSRC, const TDesC8& aPrefixString, const TDesC8& aValueString);
	void WriteAppPacket(const TDesC8& aName, const TDesC8& aAppData, TUint8 aSubType);
	
	inline void SetHasSentRtpPacket();
	
	void CanSendReportL();
	
private:
	CRtcpSessionData(TUint aRtcpBw,TBool aSendReports,TInt aPriority, CRtpController* aRtpController);
	void ConstructL(const TDesC8& aCname);
	// from CActive
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
	//
	void DoTimerReconsideration();
	void DoReverseTimerReconsiderationL();
	void StartTimer();
	void OnExpireL();
	void SendRtcpL();

	void WriteReportL(TPtr8& aBuf, TUint aLocalSsrc);
	void SetSDESFlags(TUint8 aFlags);
private:
	/** The last time an RTCP packet was transmitted */
	TNTPTime iLastTxTime;
	/** The next scheduled transmission time of an RTCP packet */
	TNTPTime iNextTxTime;
	/** Original current RTP time when session was initialzed **/
	TNTPTime iInitCurrTime;
	/** The estimated number of session members at the time tn was last recomputed */
	TUint iLastMembersEstimate; 
	/** The most current estimate for the number of session members */
	TUint iCurrentMembersEstimate;
	/** The most current estimate for the number of senders in the session */
	TUint iCurrentSendersEstimate;
	/**
	The target RTCP bandwidth, i.e., the total bandwidth
    that will be used for RTCP packets by all members of this session,
    in octets per second.  This will be a specified fraction of the
    "session bandwidth" parameter supplied at startup.*/
	TUint iRtcpBandwidth;
	/**
	Flag that is true if the application has sent data
    since the 2nd previous RTCP report was transmitted. */
	/** 
	    Has this source sent any RTP data packet? This info is needed in taking the dections
	    whether an SR or RR packet should be sent while sending a compund packet
		Initialised in= C'tor of this class to be false
		Set in		  = CRtpHandlerBase::RunL()
		Re-set in	  = CRtcpSessionData::RunL()
	*/
	TBool iSentRecently;
	/**
	The average compound RTCP packet size, in octets,
	over all RTCP packets sent and received by this participant.  The
	size includes lower-layer transport and network protocol headers
	(e.g., UDP and IP) as explained in Section 6.2. of RFC */
	TUint iAverageRtcpSize;
	/** Flag that is true if the application has not yet sent an RTCP packet.*/
	TBool iNotSendingYet;
	/** If set then controler will send RTCP reports autonomously */
	TBool iSendPeriodicReports;
	
	friend void CRtpController::SendRTCPPacketL();
	friend void CRtpController::SendRTCPPacketL(TDesC8& aPacketType);
	friend void CRtpController::ProcessRTCPRequestL(TDes8& aRequest);

	CRtpController* iRtpController;
	// random seed used for random number generation of RTCP time interval in RtpUtils
	TInt64 iRandSeed;
	
	HBufC8*	iByeData;
	TPtr8	iByePtr;
	TUint32 iByeSrc;
	//RTCP data buffers
	HBufC8*	iRtcpSendData;
		
	TSdesItemArray iSdesItems;
	TUint8 iSDESFlags;
	};

inline void CRtcpSessionData::SetHasSentRtpPacket()
	{
	iSentRecently = ETrue;
	iNotSendingYet = EFalse;
	}

#endif

