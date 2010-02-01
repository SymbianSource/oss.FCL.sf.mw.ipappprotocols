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
// Header for class specific to rtp controller base classes
// 
//

/**
 @file
 @internalComponent
*/
 
#ifndef RTPCONTROLLERBASE_H
#define RTPCONTROLLERBASE_H


#include "rtpsock.h"
#include "rtp_proto_id.h"

const TInt KRtcpDataDefaultSize = 2560;

// forward declarations
class CRtpSourceEntry;
class TRtpConflictEntry;
class CRtpStateMachine;
class CRtcpSender;
class CRtpCollisionMng;
class CRtcpSessionData;
class MRtpDataObserver;


/** The session bandwidth is the nominal data bandwidth plus the IP, UDP and RTP headers  (40 bytes). 
For example, for 64 kb/s PCM audio packetized in 20 ms increments, 
the session bandwidth would be (160 + 40) / 0.02 bytes/second or 80 kb/s. 
NOTE: in OCTETS per seconds 
@internalTechnology */
const TUint KRtpDefaultSessionBandwidth = 10000;
/** The control traffic bandwidth is in addition to the session bandwidth for the data traffic.
It is RECOMMENDED that the fraction of the session bandwidth added for RTCP be fixed at 5%. 
@internalTechnology */
const TUint KRtcpBandwidthFraction = 5;

//
const TUint KRtpVersion2 = 2;
// Sequence number default values
const TUint KRtpDefaultMaxDropout = 3000;
const TUint KRtpDefaultMaxMisorder = 100;
/** MinSequential 1 means no delay and a very first RTP packet from unknown source 
is processed where MinSequential 2 means (obviously) a delay of 1 packet and so on... 
@internalTechnology */
const TUint KRtpDefaultMinSequential = 2;

// Range of Payload types for RTP
const TInt KRtpMaxPayloadTypes = 127; //
/* Ref: RFC 3551 
 * 6.  Payload Type Definitions
 */
const TInt KRtpDynamicPayloadStart = 96; 


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
	ERtpBandWidthNotSet =309,
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

/** Defines the types of RTP connections
@internalTechnology
*/
enum TRtpConnectionType
	{
	/** Connection not activated yet.*/
	ERtpNotActive,
	/** Connectionless mode; a default option for UDP.*/
	ERtpCL,
	/** Connection mode - listens for incoming connections.*/
	ERtpCOIncoming,
	/** Connection mode - connects to a remote host. */
	ERtpCOOutgoing,
	/** Connection mode - connected sockets are owned by client;
	a default option for BT */
	ERtpConnected,
	/** Connection Mode - Connects two sockets to Rtp SubConnection*/
	ERtpCF
	};

class MSockAddrHelper
/* Abstract class used to provide Address family Specific Operations. 
 * Needs to be implemented by all handlers. 
 * @since sphinx
 * @internalComponent */
	{
	/* Compares two SockAddrresses and returns TRUE if they are matching. *
	 * The implementation should ideally offload the work to approriate   *
	 * address family classes                                             */
	virtual TBool CompareAddresses(const TSockAddr& aAddr1,const TSockAddr& aAddr2) = 0;
	};

//frwrd declarations.
class MPrePostProcessingCallback;
class TRtcpStatistics;
/* So this base class creates all its Child Class. A better way to do this is to
 * delegate the creation aspect to a factory. Also its really not obvious by
 * looking at the parameters what type of controller is getting created and why.
 * While it can be argued that this is exactly the functionality we are trying
 * to offer this argument do not hold good in most cases because the creator of
 * the RTP Controller knows exactly what type of controller he wants .
 * For eg The RTP Flow knows that he wants a controller with External
 * Handlers. The RtpSession knows that if CfRtp is used he just needs a
 * DummyController rather that a full fledged one. Whats happening now is black
 * magic. This should be avoided */
NONSHARABLE_CLASS(CRtpControllerBase) : public CBase
{
public:
	enum TRtpStreamState
	{
		ERtpStreamInitialising,
		ERtpStreamInitialised,
		ERtpStreamSendOnly,
		ERtpStreamRcvOnly,
		ERtpStreamActive
	};

	IMPORT_C static CRtpControllerBase* NewL(RSocketServ& aServer, TSockAddr& aLocalAddr,
					                         TInt aPriority = EPriorityNormal);

	IMPORT_C static CRtpControllerBase* NewL(RSocketServ& aServer, TSockAddr& aLocalAddr,
                                             const TDesC8& aCNAME, TInt aPriority = EPriorityNormal);
	
	IMPORT_C static CRtpControllerBase* NewL(RSocketServ& aServer, TSockAddr& aLocalAddr,
					   TUint aRtcpLocalPort, const TDesC8& aCNAME, 
					   TInt aPriority = EPriorityNormal);
	
	IMPORT_C static CRtpControllerBase* NewL(RSocket& aRtpSocket, TInt aPriority = EPriorityNormal);
	
	IMPORT_C static CRtpControllerBase* NewL(RSocket& aRtpSocket,RSocket& aRtcpSocket,
	                                         const TDesC8& aCNAME, TInt aPriority = EPriorityNormal);

	
	/* Utility functions*/
	static TInt CheckRtpPacket(TDesC8& aPacket);
	static TInt CheckRtcpPacket(TDesC8& aPacket, TBool& aIsBye);
	
	virtual void SetNonRtpData(TBool aValue);
	virtual void SetNonRtcpData(TBool aValue);

		
	// Version1
	virtual void ActivateL(TRtpConnectionType aConnectionType)=0;
	virtual void ActivateL(TRtpConnectionType aConnectionType, RConnection& aConnection)=0;

	virtual void ActivateL(TRtpConnectionType aConnectionType,RSubConnection& aSubConnection)=0;
	
	

	virtual void SetRemoteAddr(const TSockAddr& aRemoteAddr)=0;
	virtual void SetRemoteRtcpPort(TUint aPort)=0;
	virtual void SendDataL(TBool aSocketType, const TDesC8& aData, TRequestStatus&  aStatus)=0;
	

	// RTP Parameters
	virtual TInt SetRtpStreamParameters(TUint aMaxDropout,
										TUint aMaxMisorder,
										TUint aMinSequential)=0;
	virtual void SetSessionBandwidth(TUint aBandwidth)=0;
	virtual void SetRtcpInterval(const TTimeIntervalMicroSeconds32& aInterval)=0;

	//	virtual TUint32 CurrentRtpTimeStampL()=0;
	virtual void SetRTPTimeConversion(TUint aNow, TUint aConversion)=0;

	//  Get Functions.....
	virtual TUint32 GetLocalSSRC()=0;

	// Socket Related.....
	virtual CRtpHandlerBase* RtpHandler()=0;
	virtual CRtpHandlerBase* RtcpHandler()=0;
	
	virtual RRtpSocket& RtpSocket()=0;
	virtual RRtpSocket& RtcpSocket()=0;
	
	//Pre and Post processing callback 
	virtual void SetPrePostProcessingCallback(MPrePostProcessingCallback* aPrePostProcessingCallback)=0;
    //Disable the receipt of Non RTP data
	virtual void DisableNonRtpdata()=0;
	
	
	virtual void DoReceiveL(TDes8& aData,TRtpType aType)=0;
		
	// RTCP Sockets Messages
	virtual void SendBYEPacketL(TUint32 aLocalSRC, const TDesC8& aReason = KNullDesC8)=0;	
	virtual void SendSDESL(TInt aType, const TDesC8& aValue)=0;
	virtual void SetPRIVL(const TDesC8& aPrefixString, const TDesC8& aValueString)=0;
	virtual void SendAppPacketL(const TDesC8& aName, const TDesC8& aAppData, TUint8 aSubType)=0;
	virtual void SendRTCPPacketL(TDesC8& aPacket)=0;
	virtual TBool AutoSendOption() const=0; 
	virtual void SetAutoSendOption(TBool aOnOrOffTag)=0;

	/* The return Value indicates if the Processing of this packet was
	 * Successful or not. Handlers are expected to drop the Packet if
	 * the processing is failed
	 * The PacketType denotes RTP or RTCP */
	virtual TInt PacketBeingSent(TDes8& aPacket, TRtpType aPacketType)=0;	
 	virtual void StopReceive(TInt aRtpOrRtcp)=0;
 	virtual void DeleteEntryL(TUint32 aSRC)=0;
 	virtual TInt RtcpStatistics(TUint32 aSsrc, TRtcpStatistics &aRtcpStatistics)=0;
  	virtual ~CRtpControllerBase()
  	{
  		
  	}
  	/* Sets the sampling rate for a particular payload type */
  	virtual TInt SetSamplingRate(TInt aPayloadType, TUint aSamlingRate)=0;
  	
protected:
	//non RTP packet flag
	TBool iEnableNonRtpData;
	//non RTCP packet flag
	TBool iEnableNonRtcpData;
};

#endif
