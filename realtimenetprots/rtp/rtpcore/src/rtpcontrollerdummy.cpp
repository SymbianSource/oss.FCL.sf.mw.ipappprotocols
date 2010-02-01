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
// class for rtp controller v2 ( Dummy Controller for CFRTP)
// 
//

/**
 @file
 @internalComponent
*/

#include <e32base.h>


#include "rtputils.h"
#include "rtpsourceentry.h"
#include "rtpsock.h"
#include "rtpcontrollerdummy.h"
#include "rtcpsender.h"
#include "rtpstatemachine.h"
#include "rtcppacket.h"
#include <in_sock.h>
#include "rtpudphandler.h"
#include "rtpcollisionmng.h"
#include "rtcpsessiondata.h"
#include "rtp_proto_id.h"

// BT can be made configurable,  see rtpbthandler.h for more details...
#include "bt_sock.h"
#include "rtpbthandler.h"

#include "rtpapi.h"
#include <rtpprepostprocessingcallback.h>



// CRtpController Version2 
CRtpControllerBase* CRtpControllerV2::NewL(RSocketServ& aServer, TSockAddr& aLocalAddr,
					   TInt aPriority)
{
	__RTP_LOG(_L("CRtpController::NewL aServer, aLocalAddr, aCNAME"));
	CRtpControllerV2* self = new(ELeave) CRtpControllerV2;
	CleanupStack::PushL(self);
	TUint rtcpLocalPort = aLocalAddr.Port()+1;
	self->ConstructL(aServer, aLocalAddr,rtcpLocalPort,aPriority);
	CleanupStack::Pop(self);
	return self;
}
	
void CRtpControllerV2::ConstructL(RSocketServ& aServer, TSockAddr& aLocalAddr, TUint aRtcpLocalPort,TInt aPriority)
	{	
	iSocketServ = &aServer;
	CRtpHandlerBase* rtpHandler = CRtpHandlerBase::NewBearerL((CRtpControllerBase *)this,
													aLocalAddr, aPriority);
	CleanupStack::PushL(rtpHandler);
	rtpHandler->SetType(ECFRtp);
	User::LeaveIfError(iHandlers.Append(rtpHandler));
	// The handler is owned by iHandlers from now
	CleanupStack::Pop(rtpHandler);
	if(0 != aRtcpLocalPort)
		{
		if(KAfInet == aLocalAddr.Family())
			{
			// port number for RTCP works only for UDP
			// same transport address just different port no
			TInetAddr rtcpAddr(aLocalAddr);
			rtcpAddr.SetPort(aRtcpLocalPort);
			CRtpHandlerBase* rtcpHandler = 
				CRtpHandlerBase::NewBearerL((CRtpControllerBase *)this, rtcpAddr, aPriority);
			
			CleanupStack::PushL(rtcpHandler);
			rtcpHandler->SetType(ECFRtcp);
			User::LeaveIfError(iHandlers.Append(rtcpHandler));
			// The handler is owned by iHandlers from now on
			CleanupStack::Pop(rtcpHandler);
			}
		else 
			{
			// RTCP can't be configured just through the port number for other protocol families
			User::Leave(KErrNotSupported);
			}
		}
	}

 void CRtpControllerV2::ActivateL(TRtpConnectionType aConnectionType, RSubConnection& aSubConnection)
	{	
	switch (aConnectionType)
		{
		case ERtpConnected:
			{
			iConnectionType = aConnectionType;
			// no handler/bearer activation required for connected socket
			return;
			}
		case ERtpCF:
		case ERtpCL:
		case ERtpCOIncoming:
		case ERtpCOOutgoing:
			break;
		case ERtpNotActive:
			// nothing to do here
			return;
		default:
			User::Leave(KErrArgument);
		}
	for(int i=0;i<iHandlers.Count();i++)
		{
		iHandlers[i]->ActivateL(*iSocketServ,aConnectionType,aSubConnection);
		}
	}


/*
Setting Remote address for Rtp and Rtcp Sockets
*/	
void CRtpControllerV2::SetRemoteAddr(const TSockAddr& aRemoteAddr)
	{
	__RTP_LOG1(_L("\t[RemoteAddrPort] == %u"), aRemoteAddr.Port());
	__RTP_LOG1(_L("\t[RemoteAddrFamily] == %u"), aRemoteAddr.Family());
	for(int i=0;i<iHandlers.Count();i++)
		{
		//Seting the remote rtp address and port

		if(iHandlers[i]->Type() == ECFRtp)
			{
			iHandlers[i]->SetRemoteAddress(aRemoteAddr);
			iHandlers[i]->SetDefaultRemoteAddress(aRemoteAddr);
			}
		else
			{
			TSockAddr temp(aRemoteAddr);
			temp.SetPort(aRemoteAddr.Port()+1);
		
			TPckgBuf<TSockAddr> defDstBuf(temp);
			RtcpSocket().SetOpt(KSoDefaultDestAddr,KSolRtcp,defDstBuf);
		
			iHandlers[i]->SetDefaultRemoteAddress(temp);
			iHandlers[i]->SetRemoteAddress(temp);
			}
		}
	}


void CRtpControllerV2::SetRemoteRtcpPort(TUint aPort)
	{
	TSockAddr addr;

	addr = RtcpHandler()->RemoteAddress();
	addr.SetPort(aPort);


	RtcpHandler()->SetRemoteAddress(addr);
	RtcpHandler()->SetDefaultRemoteAddress(addr);

	TPckgBuf<TSockAddr> defDstBuf(addr);

	RtcpSocket().SetOpt(KSoDefaultDestAddr,KSolRtcp,defDstBuf);
	}
/*
Getter Functions ....
*/

TUint32 CRtpControllerV2::GetLocalSSRC()
	{
	TInt localSsrc=0;
	RtpSocket().GetOpt(KSoLocalSSRC,KSolRtp,localSsrc);
	return localSsrc;
	}


CRtpHandlerBase* CRtpControllerV2::RtpHandler()
	{
	ASSERT(iHandlers[0]);
	return iHandlers[0];
	}

CRtpHandlerBase* CRtpControllerV2::RtcpHandler()
	{
	ASSERT(iHandlers[1]);
	return iHandlers[1];
	}


void CRtpControllerV2::SetSessionBandwidth(TUint aBandwidth)
	{
	RtpSocket().SetOpt(KSoRtpBandwidth,KSolRtp,aBandwidth);
	}

TInt CRtpControllerV2::SetRtpStreamParameters(TUint aMaxDropout,
										TUint aMaxMisorder,
										TUint aMinSequential)
	{
	TRtpStreamParams streamParams;
	streamParams.iMinSequential = aMinSequential;
	streamParams.iMaxMisorder   = aMaxMisorder;
	streamParams.iMaxDropout    = aMaxDropout;
	TPckgBuf<TRtpStreamParams> streamParamsBuf(streamParams);
	return RtpSocket().SetOpt(KSoStreamParams,KSolRtp,streamParamsBuf);
	}
	
void CRtpControllerV2::SetRtcpInterval(const TTimeIntervalMicroSeconds32& aInterval)
	{
	TPckgBuf<TTimeIntervalMicroSeconds32> timeInterval(aInterval);
	RtcpSocket().SetOpt(KSoRtcpCNAME,KSolRtcp,timeInterval);
    }

void CRtpControllerV2::SetRTPTimeConversion(TUint aNow, TUint aConversion)
	{
	TRtpTimeConversion rtpTimeConv;
	rtpTimeConv.iRtpTimeNow = aNow;
	rtpTimeConv.iConversion = aConversion;
	TPckgBuf<TRtpTimeConversion> rtpTimeConvBuf(rtpTimeConv);
	RtpSocket().SetOpt(KSoRtpTimeConvertion,KSolRtp,rtpTimeConvBuf);
	}
	
/* To Be Done >>  : The Mother of all TooDoos's Clean up the acts below!!! */
void CRtpControllerV2::SendBYEPacketL(TUint32 aLocalSRC, const TDesC8& aReason)
	{
	/* Will be ever Come here?. And is this BYE packet used as such
	 * by the CFRtp ? */
	RBuf8 bye;
	TRtcpBYEPack byePacket(aReason.Length());
	byePacket.AssignL(bye);
	byePacket.SetSSRC(aLocalSRC);
	byePacket.SetReason(aReason);
	
	TRequestStatus status;
	RtcpHandler()->Send(bye,status);
	User::WaitForRequest(status);
	bye.Close();
	}

void CRtpControllerV2::SendSDESL(TInt aType, const TDesC8& aValue)
	{
	RBuf8 sdes;
	TRtcpSdesPack sdesPacket(aValue.Length());
	sdesPacket.AssignL(sdes);
	sdesPacket.SetAttribute(aType,aValue);

	TRequestStatus status;
	RtcpHandler()->Send(sdes,status);
	User::WaitForRequest(status);
	sdes.Close();
	User::LeaveIfError(status.Int());
	}

/*Goes with Rtcp Socket*/
void CRtpControllerV2::SetPRIVL(const TDesC8& aPrefixString, const TDesC8& aValueString)
	{
	RBuf8 sdes;
	TRtcpSdesPack sdesPacket((aValueString.Length() + aPrefixString.Length() ));
	sdesPacket.AssignL(sdes);
	sdesPacket.SetAttribute(aPrefixString,aValueString);

	TRequestStatus status;
	RtcpHandler()->Send(sdes,status);
	User::WaitForRequest(status);
	sdes.Close();
	}
	
void CRtpControllerV2::SendAppPacketL(const TDesC8& aName, const TDesC8& aAppData, TUint8 aSubType)
	{
	RBuf8 app;
	TRtcpAPPPack appPacket((aName.Length()+aAppData.Length()));
	appPacket.AssignL(app);
	appPacket.SetSubType(aSubType);
	appPacket.SetAppData(aAppData);
	appPacket.SetAppName(aName);
	
	TRequestStatus status;
	RtcpHandler()->Send((TDes8 &)app,status);
	User::WaitForRequest(status);
	app.Close();
	}

void CRtpControllerV2::SendRTCPPacketL(TDesC8& aPacket)
	{
	RBuf8 param;
	TRtpParam paramPacket(aPacket.Length());
	paramPacket.AssignL(param);
	paramPacket.SetRtpParam(ERtcpCompound,aPacket);
	TRequestStatus status;
	RtcpHandler()->Send((TDes8 &)param,status);
	User::WaitForRequest(status);
	param.Close();
	}


TBool CRtpControllerV2::AutoSendOption() const
	{
	TInt autosend;
	RRtpSocket rtcpSocket = iHandlers[1]->RtpSocket();
	rtcpSocket.GetOpt(KSoRtcpAutoSend, KSolRtcp, autosend);
	return autosend;
	}


void CRtpControllerV2::SetAutoSendOption(TBool aOnOrOffTag)
	{
	RtcpSocket().SetOpt(KSoRtcpAutoSend,KSolRtcp,aOnOrOffTag);
	}

void CRtpControllerV2::DoReceiveL(TDes8& aClientBuf, TRtpType aType)
	{	
	if(ECFRtp == aType)
		{
		TInt error = CheckRtpPacket(aClientBuf);
		//check for non RTP data
		if((KErrNone != error) && (iEnableNonRtpData))
			{
			/* Notify that a non RTP packet is received */
			iHandlers[0]->CompleteRead(KErrCorrupt);
			return;
			}
		else
			{
			iHandlers[0]->CompleteRead(KErrNone);
			}
		}
	else if(ECFRtcp == aType)
		{
		TBool isBye = EFalse;
		TInt error = CheckRtcpPacket(aClientBuf,isBye);
		//check for non RTP data
		if((KErrNone != error) && (iEnableNonRtcpData))
			{
			/* Notify that a non RTP packet is received */
			iHandlers[1]->CompleteRead(KErrCorrupt);
			return;
			}
		else
			{
			iHandlers[1]->CompleteRead(KErrNone);
			}
		}
	}

CRtpControllerV2::~CRtpControllerV2()
	{
	/* Thats all?  */
	iHandlers.ResetAndDestroy();
	}

TInt CRtpControllerV2::PacketBeingSent(TDes8& , TRtpType )
	{
	return KErrNotSupported;
	}

 void CRtpControllerV2::StopReceive(TInt /*aRtpOrRtcp*/)
 	{
 	}

 void CRtpControllerV2::DeleteEntryL(TUint32 /*aSRC*/)
 	{
 	}

void CRtpControllerV2::ActivateL(TRtpConnectionType /*aConnectionType*/)
	{
	}

void CRtpControllerV2::ActivateL(TRtpConnectionType /*aConnectionType*/, RConnection& /*aConnection*/)
	{
	}

RRtpSocket& CRtpControllerV2::RtpSocket() 
	{
	CRtpHandlerBase* handlerPtr = RtpHandler();
	__ASSERT_ALWAYS(handlerPtr, Panic(ERtpHandlerIsNotCreated));
	return handlerPtr->RtpSocket();	
	}

RRtpSocket& CRtpControllerV2::RtcpSocket() 
	{
	CRtpHandlerBase* handlerPtr = RtcpHandler();
	__ASSERT_ALWAYS(handlerPtr, Panic(ERtpHandlerIsNotCreated));
	return handlerPtr->RtpSocket();	 
	}
	
TInt CRtpControllerV2::RtcpStatistics(TUint32 aSsrc, TRtcpStatistics &aRtcpStatistics)
	{
	TRtcpStatisticsParams rtcpStatisticsParams;
	/* The SSRC for which Data is to be fetched */
	rtcpStatisticsParams.iSsrc = aSsrc;
	TPckgBuf<TRtcpStatisticsParams> rtcpStatisticsParamsBuf(rtcpStatisticsParams);
	TInt err = RtcpSocket().GetOpt(KSoRtcpStatistics,KSolRtcp,rtcpStatisticsParamsBuf);
	if(KErrNone != err)
		{
		return err;
		}
	rtcpStatisticsParams = rtcpStatisticsParamsBuf();
	aRtcpStatistics.SetPacketsSent(rtcpStatisticsParams.PacketsSent());
	aRtcpStatistics.SetOctetsSent(rtcpStatisticsParams.OctetsSent());
	aRtcpStatistics.SetJitter(rtcpStatisticsParams.Jitter());
	aRtcpStatistics.SetPacketsLost(rtcpStatisticsParams.PacketsLost());
	aRtcpStatistics.SetFractionLost(rtcpStatisticsParams.FractionLost());	
	return err;
	}
	
	
void CRtpControllerV2::SetPrePostProcessingCallback(MPrePostProcessingCallback*)
	{
	}
	
inline void CRtpControllerV2::DisableNonRtpdata() 
	{
	RSocket& rtpsock = RtpSocket();
	rtpsock.SetOpt(KSoReceiveNonRtpData,KSolRtp,0);
	RSocket& rtcpsock = RtcpSocket();
	rtcpsock.SetOpt(KSoReceiveNonRtcpData,KSolRtcp,0);
	}

/**
Send a non-RTP (control) data packet asynchronously
    
@param aSocketType SocketType that tells whether non RTP data has to go through RTP or RTCP socket
    
@param aData Non RTP data that has to be sent
    
@param aStatus Tells whether the data has been sent successfully

*/
void CRtpControllerV2::SendDataL(TBool aSocketType, const TDesC8& aData, TRequestStatus&  aStatus)
	{
	TRequestStatus locStat;
	//Save the application provided aStatus. In CF it is not possible to do a truely
	//Async non-rtp data send. So mimic an Async call here.
	aStatus = KRequestPending;

	if(aSocketType)
		{
		RSocket& sock = RtpSocket();
		/* Enable sending of Non-RTP data */
		sock.SetOpt(KSoSendNonRtpData,KSolRtp,1);
		/* Send the data through the socket to the remote address */
		sock.SendTo(aData,RtpHandler()->RemoteAddress(),0,locStat);
		User::WaitForRequest(locStat);
		/* For CF RTP this API cannot be Asynchronous */
		sock.SetOpt(KSoSendNonRtpData,KSolRtp,0);
		}
	else
		{
		if (!RtcpHandler())
			{
			User::Leave(KErrNotSupported);
			}
		else
			{
			RSocket& sock = RtcpSocket();
			/* Enable sending of Non-RTCP data */
			sock.SetOpt(KSoSendNonRtcpData,KSolRtcp,1);
			/* Send the data through the socket to the remote address */
			sock.SendTo(aData,RtcpHandler()->RemoteAddress(),0,locStat);
			User::WaitForRequest(locStat);
			/* For CF RTP this API cannot be Asynchronous */	
			sock.SetOpt(KSoSendNonRtcpData,KSolRtcp,0);
			}
		}
	TRequestStatus *pStat = &aStatus;
	User::RequestComplete(pStat, locStat.Int());
	}


TInt CRtpControllerV2::SetSamplingRate(TInt aPayloadType, TUint aSamplingRate)
	{
	TSamplingRateInfo samplingRate;
	samplingRate.iPayloadType = aPayloadType;
	samplingRate.iSamplingRate = aSamplingRate;
	TPckgBuf<TSamplingRateInfo> sampRateInfo(samplingRate);
	return RtpSocket().SetOpt(KSoSamplingRate,KSolRtp,sampRateInfo);
	}

