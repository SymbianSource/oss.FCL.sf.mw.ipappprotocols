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
// Header for class specific to rtp controller2 ( Dummy Controller for CFRTP)
// 
//

/**
 @file
 @internalComponent
*/

#if !defined(__RTP_CONTROLLER_DUMMY_H__)
#define __RTP_CONTROLLER_DUMMY_H__

#include "rtpcontrollerbase.h"

/* We are a Version up. But no functionality has been added!.
 * TO BE DONE: Rename this class to a more meaning full
 * CRtpControllerProxy/CRtpControllerDummy
 * because this class is more or less a Wrapper around the Handlers */
NONSHARABLE_CLASS(CRtpControllerV2) : public CRtpControllerBase
{
public:

	static CRtpControllerBase* NewL(RSocketServ& aServer, TSockAddr& aLocalAddr,
					   TInt aPriority = EPriorityNormal);
	
	
	void ConstructL (RSocketServ& aServer, TSockAddr& aLocalAddr,
					   TUint aRtcpLocalPort,TInt aPriority = EPriorityNormal);
	
	virtual void ActivateL(TRtpConnectionType aConnectionType,RSubConnection& aConnection);
		
	virtual void ActivateL(TRtpConnectionType aConnectionType);
	virtual void ActivateL(TRtpConnectionType aConnectionType,RConnection& aConnection);
	
	
	virtual void SetRemoteAddr(const TSockAddr& aRemoteAddr);
	virtual void SetRemoteRtcpPort(TUint aPort);
	virtual void SendDataL(TBool aSocketType, const TDesC8& aData, TRequestStatus&  aStatus);
	
	virtual TInt SetRtpStreamParameters(TUint aMaxDropout,
										TUint aMaxMisorder,
										TUint aMinSequential);
	
	virtual TUint32 GetLocalSSRC();


	virtual CRtpHandlerBase* RtpHandler();
	virtual CRtpHandlerBase* RtcpHandler();
	
	virtual RRtpSocket& RtpSocket();
	virtual RRtpSocket& RtcpSocket();
	
	//Pre and Post processing callback 
	virtual void SetPrePostProcessingCallback(MPrePostProcessingCallback* aPrePostProcessingCallback);
	virtual void DoReceiveL(TDes8& aData,TRtpType aType);

	virtual void SetSessionBandwidth(TUint aBandwidth);
	virtual void SetRtcpInterval(const TTimeIntervalMicroSeconds32& aInterval);

	virtual void SetRTPTimeConversion(TUint aNow, TUint aConversion);
	virtual void SendBYEPacketL(TUint32 aLocalSRC, const TDesC8& aReason = KNullDesC8);	
	virtual void SendSDESL(TInt aType, const TDesC8& aValue);
	virtual void SetPRIVL(const TDesC8& aPrefixString, const TDesC8& aValueString);
	virtual void SendAppPacketL(const TDesC8& aName, const TDesC8& aAppData, TUint8 aSubType);
	virtual void SendRTCPPacketL(TDesC8& aPacket);
	virtual TBool AutoSendOption() const; 
	virtual void SetAutoSendOption(TBool aOnOrOffTag);
	
	
	virtual TInt PacketBeingSent(TDes8& aPacket, TRtpType aPacketType);
 	virtual void StopReceive(TInt aRtpOrRtcp);
 	virtual void DeleteEntryL(TUint32 aSRC);
 	TInt RtcpStatistics(TUint32 aSsrc, TRtcpStatistics &aRtcpStatistics);
 	inline void DisableNonRtpdata();
 	
  	/* Sets the sampling rate for a particular payload type */
  	virtual TInt SetSamplingRate(TInt aPayloadType, TUint aSamlingRate);	

private:
	
	virtual ~CRtpControllerV2();
	
	TRtpConnectionType iConnectionType;
	
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
	RSocketServ* iSocketServ;
};

#endif

