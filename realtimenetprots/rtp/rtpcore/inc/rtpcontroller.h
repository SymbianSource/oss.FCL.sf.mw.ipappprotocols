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
// Header for class specific to rtp controller
// 
//

/**
 @file
 @internalComponent
*/

#if !defined(__RTP_CONTROLLER_H__)
#define __RTP_CONTROLLER_H__

#include "rtpsock.h"
#include <es_sock.h>
#include "rtpcontrollerbase.h"
#include "rtphandlerbase.h"

/**
@internalTechnology 
*/

/** RTP controller. 
This is a representation of a RTP 'session' on the socket-level.
It provides services such as SSRC allocation, sequence number allocation,
collision detection of source identifiers within the stream etc.
The RTP stream contains at least RTP socket and can include a RTCP socket and 
(in later releases) FEC socket.

The controller represents exactly one endpoint (sender) on the device however the stream
can accomodate more then one (remote) sender and receiver.

The class assumes the active scheduler is created and running, and
will not correctly function if it isn't.

@internalTechnology
*/
NONSHARABLE_CLASS(CRtpController) : public CRtpControllerBase

	{
	friend class CRtpHandlerBase;
	friend class CRtpUdpHandler;
	friend class CRtpBluetoothHandler;
	friend class CRtcpSessionData;
public:
	
	static CRtpController* NewL(RSocketServ& aServer, TSockAddr& aLocalAddr,
					   const TDesC8& aCNAME = KNullDesC8, TInt aPriority = EPriorityNormal);
	
	static CRtpController* NewL(RSocketServ& aServer, TSockAddr& aLocalAddr,
					   TUint aRtcpLocalPort, const TDesC8& aCNAME, 
					   TInt aPriority = EPriorityNormal);
	
	static CRtpController* NewL(RSocket& aRtpSocket,
										TInt aPriority = EPriorityNormal);
	
	static CRtpController* NewL(RSocket& aRtpSocket,RSocket& aRtcpSocket,
										const TDesC8& aCNAME = KNullDesC8, TInt aPriority = EPriorityNormal);
	
	IMPORT_C static CRtpController* NewL(CRtpHandlerBase& aRtphandler, 
	                                    TInt aPriority, TSockAddr aLocalAddr, const TDesC8 &aCNAME);

	virtual ~CRtpController();

	virtual void ActivateL(TRtpConnectionType aConnectionType);
	virtual void ActivateL(TRtpConnectionType aConnectionType, RConnection& aConnection);
	virtual void ActivateL(TRtpConnectionType aConnectionType, RSubConnection& aSubConnection);
	
	virtual void SetRemoteAddr(const TSockAddr& aRemoteAddr);
	virtual void SetRemoteRtcpPort(TUint aPort);

	
	virtual void SendDataL(TBool aSocketType, const TDesC8& aData, TRequestStatus&  aStatus);

	//Disable the receipt of Non RTP data
	inline void DisableNonRtpdata();

	//Pre and Post processing callback 
	virtual void SetPrePostProcessingCallback(MPrePostProcessingCallback* aPrePostProcessingCallback);
	
	virtual TInt SetRtpStreamParameters(TUint aMaxDropout,
										TUint aMaxMisorder,
										TUint aMinSequential);
	
	virtual TUint32 GetLocalSSRC();

	virtual RRtpSocket& RtpSocket();
	virtual RRtpSocket& RtcpSocket();

	virtual CRtpHandlerBase* RtpHandler();
	virtual CRtpHandlerBase* RtcpHandler();
	

	virtual inline void SetSessionBandwidth(TUint aBandwidth);
	virtual inline void SetRtcpInterval(const TTimeIntervalMicroSeconds32& aInterval);
	
	virtual TUint32 CurrentRtpTimeStampL();
	virtual void SetRTPTimeConversion(TUint aNow, TUint aConversion);
	
	virtual void SendBYEPacketL(TUint32 aLocalSRC, const TDesC8& aReason = KNullDesC8);	
	virtual void SendSDESL(TInt aType, const TDesC8& aValue);
	virtual void SetPRIVL(const TDesC8& aPrefixString, const TDesC8& aValueString);
	virtual void SendAppPacketL(const TDesC8& aName, const TDesC8& aAppData, TUint8 aSubType);
	virtual void SendRTCPPacketL(TDesC8& aPacket);
	virtual TBool AutoSendOption() const; 
	virtual void SetAutoSendOption(TBool aOnOrOffTag);
	virtual void StopReceive(TInt aRtpOrRtcp);

	virtual void DeleteEntryL(TUint32 aSRC);

	inline CRtpStateMachine* SourceStateMachine() const;
	
	inline CRtpCollisionMng* CollisionManager() const;
	
	virtual void CreateRtcpL(CRtpHandlerBase* aRtcphandler,const TDesC8& aCNAME);
	virtual void SetLocalAddress(const TSockAddr& aLocalAddr);
	
	virtual void HandleCollision();
	
	virtual TInt PacketBeingSent(TDes8& aPacket, TRtpType aPacketType);
	
	virtual void DoReceiveL(TDes8& aData,TRtpType aType);
	
	virtual void ProcessRTCPRequestL(TDes8& aRequest);
	
	virtual TInt RtcpStatistics(TUint32 aSsrc, TRtcpStatistics &aRtcpStatistics);

  	/* Sets the sampling rate for a particular payload type */
  	virtual TInt SetSamplingRate(TInt aPayloadType, TUint aSamlingRate);
	
	TUint GetSamplingRate(TUint aPayloadType);	

private:
	
	
	CRtpController();
	
	void ConstructL(RSocketServ& aServer, TSockAddr& aLocalAddr,
					const TDesC8& aCNAME, TUint aRtcpLocalPort, TInt aPriority);
	void ConstructL(RSocket& aRtpSocket, TInt aPriority);
	void ConstructL(RSocket& aRtpSocket, RSocket& aRtcpSocket,
					const TDesC8& aCNAME, TInt aPriority);
	void ConstructL(CRtpHandlerBase* aRtphandler, TInt aPriority, 
	                TSockAddr aLocalAddr, const TDesC8 &aCNAME);
	                
	// receive functionality
	void DoRtpReceiveL(TDes8& aData);
	void DoRtcpReceiveL(TDes8& aData);
	
	void SendRTCPPacketL();
	
	CRtpSourceEntry& CreateCollisionManagerL(TSockAddr& aLocalAddr,const TDesC8& aCNAME);
	TBool CompareAddresses(TSockAddr& aAddr1,TSockAddr& aAddr2);
	CRtpSourceEntry* UpdateSourceTableL(TRtpType aType,TUint32 aSRC,TSockAddr& aSourceAddr);
	
	inline TBool UserSetInterval(TTimeIntervalMicroSeconds32& aInterval);
	
	/* To fill the FixedHeader Part of our own RTP Packet */	
	void FillRtpFixedHeader(TDes8& aPacket);
	
private:
	CRtpCollisionMng* iCollisionManager;
	/**
	An array of handlers for this session.
	The first handler is always RTP handler and is always present; 
	the array may contain RTCP and/or FEC handler as well.
	*/
	RPointerArray<CRtpHandlerBase> iHandlers;
	/**
	A type of connection for this session; either CL or CO Incoming/Outgoing.
	@see TRtpConnectionType
	*/
	TRtpConnectionType iConnectionType;
	RSocketServ* iSocketServ;
	TRtpStreamState iStreamState;
	CRtpStateMachine* iSourceStateMachine;
	/** We need another active object in order to send RTCP packets out */
	CRtcpSender* iRtcpSender;
	/** For each session, it is assumed that the data traffic is subject to
	an aggregate limit called the "session bandwidth" to be divided among the participants.
	This bandwidth might be reserved and the limit enforced by the network.
	In octets/seconds
	*/
	TUint iSessionBandwidth;
	// RTCP stuff
	CRtcpSessionData* iRtcpSessionData;
	// Time interval set by user
	TTimeIntervalMicroSeconds32 iRtcpInterval;
	//The Sequence number is put here. Ideally this should be
	//a part of SessionData. But let go for now..
	TUint16  iLocalSocalSrcSeq;
	/* We are using external handlers */
	TBool    iExtHandlersUsed;
	
	// pre and post processing callback
	MPrePostProcessingCallback* iPrePostProcessingCallback;
	
	RArray<TSamplingRateInfo> iSamplingRatesArray;
	
	/* For compatibility with the old RTP TimeConversionApi 
	 * ie Old applications will not be calling SetSamplingRate()
	 */
	TUint iSamplingRate;	
	};

#include "rtpcontroller.inl"

#endif

