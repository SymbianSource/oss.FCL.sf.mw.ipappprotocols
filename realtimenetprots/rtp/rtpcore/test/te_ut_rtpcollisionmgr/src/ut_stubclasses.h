// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// @internalAll
// 
//

#if !defined(__UT_STUB_CLASSESS_H__)
#define __UT_STUB_CLASSESS_H__

#include <es_sock.h>


#ifndef __RTP_LOG
		#define __RTP_LOG_STMT(stmt)
		#define __RTP_LOG(text)
		#define __RTP_LOG1(text, a)
		#define __RTP_LOG2(text, a, b)
		#define __RTP_LOG3(text, a, b, c)
		#define __RTP_LOG4(text, a, b, c, d)
		#define __RTP_LOG5(text, a, b, c, d, e)
		#define __RTP_LOG6(text, a, b, c, d, e, f)
		#define __RTP_LOG7(text, a, b, c, d, e, f, g)
#endif
/* Copy of Enums needed by the stubs */
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

/**
@internalComponent */
struct TNTPTime
	{
	TUint iSeconds;
	TUint iMicroSeconds;
	TNTPTime(TUint32 aSeconds,TUint32 aMicroSeconds):iSeconds(aSeconds),iMicroSeconds(aMicroSeconds)
		{
		}
	};



_LIT8(KRtcpByeReasonCollision,"RTP collision detected  ");

/* Helper Class Dummy */
class RtpUtils
	{
public:
	static TUint32 GenerateRandomNumber32L();
	static TNTPTime GetNTPTimeL();
private:
	RtpUtils();
};


class CRtpHandlerBase : public CBase
	{
public:
	
	static CRtpHandlerBase* NewL();
	~CRtpHandlerBase();
	
	TBool CompareAddresses(const TSockAddr& aAddr1,const TSockAddr& aAddr2);
	
private:
	CRtpHandlerBase();
	void ConstructL();		
		
	};


/* Dummy Class for RTP Controller */
/* Add more functionalities later on. Currently this will serve the
 * collision manager module. */
/* USE: Unit Testing */
class CRtpController : public CBase
	{
public:
	CRtpHandlerBase* iRtp;
	CRtpHandlerBase* iRtcp;
	TUint32 iSSRC;
	
	static CRtpController* NewL();

	~CRtpController();

	void HandleCollision();
	void SendBYEPacketL(TUint32 aLocalSRC, const TDesC8& aReason);
	void SetLocalSSRC(TUint32 aSSRC);
	
	CRtpHandlerBase* RtpHandler();
	CRtpHandlerBase* RtcpHandler();

private:
	CRtpController();
	void ConstructL();
	};


/* Stub for SourceEntry. Contains the functionality needed by CollisionManager
 * */
class CRtpSourceEntry : public CBase
	{
public:
	static CRtpSourceEntry* NewLC(TUint32 aSSRC,const TDesC8& aCNAME,CRtpController* aController);
	~CRtpSourceEntry();

	const TDesC8& Cname();

	TUint SRC() const;
	//const TSockAddr& RtpSourceAddr() const;
	//const TSockAddr& RtcpSourceAddr() const;

	void SetRtpSourceAddr(TSockAddr& aRtpAddr);
	void SetRtcpSourceAddr(TSockAddr& aRtcpAddr);

	static TInt CompareSSRC(const CRtpSourceEntry& aFirst,const CRtpSourceEntry& aSecond);
		
	const TSockAddr& RtpSourceAddr() const;
	const TSockAddr& RtcpSourceAddr() const;
	
	// access to RTP statistics
	void SetState(TRtpSourceState aState);
	void SetCnameL(const TDesC8& aCNAME);
	TRtpSourceState State();
	
public:
	CRtpSourceEntry(TUint32 aSRC);
	void ConstructL(const TDesC8& aCNAME);
	void SetSRC(TUint32 aSRC);
public:
	
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
	};



enum TRtpType
	{
	/** It is a RTP socket */
	ERtp,
	/** It is a RTCP socket */
	ERtcp
	};
	

/**
@internalComponent

A set of panic codes.

In release mode, different panic codes, such as
KERN-EXEC 3 may be generated instead.
*/
enum TRtpCorePanicCode
	{
	ERtpNotSupported = 0,
	ERtpParameterNegative = 10,
	ERtpParameterOutOfRange = 11,
	ERtpSourceEntryCount = 12,
	// Socket
	ERtpSocketNoMemory = 100,
	ERtpSocketCSRCNotZero = 101,
	ERtpSocketTypeNotSupported = 102,
	// Handler
	ERtpSocketOpenedWithoutHandler = 200,
	ERtpReadMoreCalledWithoutData = 201,
	ERtpHandlerWrongConnType = 202,
	ERtpSocketNotReady = 203,
	ERtpSocketNotOwn = 204,
	ERtpIsAlreadyActive = 205,
	ERtcpInternalReadAlreadyActive = 206,
		
	// Controller
	ERtpControllerIsNotCreated = 300,
	ERtpHandlerIsNotCreated = 301,
	ERtpHandlerFamilyMismatch =302,
	ERtpHandlerSSRCMismatch =303,
	ERtpCollisionMgrIsNotCreated = 304,
	ERtpSocketServerNull = 305,
	ERtpStateMachineNull = 306,
	ERtpSessionDataNull = 307,
	ERtpParametersZeroOrNegative = 308,
	// RTCP
	ERtcpByePacketNotPadded = 400,
	ERtcpInvalidState = 401,
	ERtcpRecvOnlyMode = 402, //System is in recv-only mode wrt RTCP send/recv pkts
	// SSRC
	ERtpSourceEntryDeleted = 500,
	ERtpSourceControllerNull = 501,
	ERtpSourceEntryNull = 502,
	ERtpSourceMismatch = 503,
	// state machine
	ERtpStateMachineNotCreated = 600,
	ERtpStateMachineUnknownState = 601,
	ERtpStateMachineWrongState = 602,
	ERtpStateMachineZeroTimeout = 603,
	// Collision Manager
	ERtpCollisionMgr = 700,
	ERtpCollisionMgrWrongType = 701,
	ERtpCollisionMgrNoAddr = 702,
	ERtpCollisionMgrNoLocalSSRC = 703,
	ERtpSingleEntryCacheNull = 704,
	// Timers
	ERtpTimeoutNoEntry = 800,
	// RTCP
	ERtcpDataNull = 900,
	ERtcpBufferToSmall = 901,
	ERtcpSenderNull = 902,
	ERtcpTickPeriodZero = 903,
	ERtcpInvalidAppName = 904
	};
	
	
void PanicL(TRtpCorePanicCode aPanicCode);
#endif
