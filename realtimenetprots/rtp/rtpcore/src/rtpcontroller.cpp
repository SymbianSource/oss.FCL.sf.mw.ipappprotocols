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
// class for rtp controller
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
#include "rtpcontroller.h"
#include "rtcpsender.h"
#include "rtpstatemachine.h"
#include "rtcppacket.h"
#include <in_sock.h>
#include "rtpudphandler.h"
#include "rtpcollisionmng.h"
#include "rtcpsessiondata.h"
// BT can be made configurable,  see rtpbthandler.h for more details...
#include "bt_sock.h"
#include "rtpbthandler.h"
#include "rtppacket.h"
#include "rtpapi.h"
#include <rtp_proto_id.h>
#include <rtpprepostprocessingcallback.h>


/* Ref: RFC 3551 
 * Table 4: Payload types (PT) for audio encodings
 * Table 5: Payload types (PT) for video and combined encodings
 */
const TUint8 KRtpPayloadClockConversions[KRtpMaxPayloadTypes + 1] =
     {
     125, 0, 0, 125, 125, 125, 63, 125, 125, 63,     // 0-9 
     23, 23, 125, 125, 11, 125, 91, 45, 125, 0,      // 10-19
     0, 0, 0, 0, 0, 11, 11, 0, 11, 0,                // 20-29
     0, 11, 11, 11, 11, 0, 0, 0, 0, 0,               // 30-39
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 40-49
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 50-59
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 60-69
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 70-79
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 80-89
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 90-99
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 100-109
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 110-119
     0, 0, 0, 0, 0, 0, 0, 0                          // 120-127
     };


// CRtpController //
/** Create the controller for RTP and RTCP handlers
If aCNAME is not specified RTCP is not initialised otherwise
handler with port number + 1 is used for RTCP (as recommended in RFC3550)
The controller sets MinSequential, MaxMisorder, and MaxDropout parameters methods 
to default values predefined in rtpsock.h
The parameters can be changed immediately after the controller is created by calling 
SetRtpStreamParameters(TUint aMaxDropout, TUint aMaxMisorder, TUint aMinSequential);
@see CRtpController::SetRtpStreamParameters

@internalTechnology
@param aServer An opened socket server session
@param aLocalAddr The address to bind to at this end. 
@param aCNAME The CNAME is a canonical end-point identifier,
RTCP is not initialised if none is provided.
@param aPriority The priority to be used for internal active objects.
@leave KErrNoMemory, ConstructL
*/
CRtpController* CRtpController::NewL(RSocketServ& aServer, TSockAddr& aLocalAddr,
									const TDesC8& aCNAME, TInt aPriority)

	{	
	__RTP_LOG(_L("CRtpController::NewL aServer, aLocalAddr, aCNAME"));
	CRtpController* self = new(ELeave) CRtpController;
	CleanupStack::PushL(self);
	TUint rtcpLocalPort =0;
	if(0 != aCNAME.Compare(KNullDesC8))
		{
		// CNAME was specified but no RTCP port no specified thus RTP port no + 1
		rtcpLocalPort = aLocalAddr.Port()+1;
		}
	self->ConstructL(aServer, aLocalAddr, aCNAME, rtcpLocalPort,aPriority);
	CleanupStack::Pop(self);
	return self;
	}


/** Create the controller for RTP and RTCP handlers
RTCP is initialised with RTCP local port number (any port number allowed as specified in RFC3550)
The controller sets MinSequential, MaxMisorder, and MaxDropout parameters methods 
to default values predefined in rtpsock.h
The parameters can be changed immediately after the controller is created by calling 
SetRtpStreamParameters(TUint aMaxDropout, TUint aMaxMisorder, TUint aMinSequential);
@see CRtpController::SetRtpStreamParameters
@internalTechnology
@param aServer An opened socket server session
@param aLocalAddr The address to bind to at this end.
@param aRtcpLocalPort The port number used for RTCP
@param aCNAME The CNAME is a canonical end-point identifier.
@param aPriority The priority to be used for internal active objects.
@leave KErrNoMemory, CRtpController::ConstructL
*/
CRtpController* CRtpController::NewL(RSocketServ& aServer, TSockAddr& aLocalAddr,
									TUint aRtcpLocalPort, const TDesC8& aCNAME, TInt aPriority)
	{	
	__RTP_LOG(_L("CRtpController::NewL aServer, aLocalAddr, aRtcpLocalPort, aCNAME"));
	CRtpController* self = new(ELeave) CRtpController;
	CleanupStack::PushL(self);
	self->ConstructL(aServer, aLocalAddr, aCNAME, aRtcpLocalPort,aPriority);
	CleanupStack::Pop(self);
	return self;
	}

/** Create the controller from two (RTP & RTCP) connected sockets
This method is expected to be used for CO services,primarily for BT with RTCP.
The controller sets MinSequential, MaxMisorder, and MaxDropout parameters methods 
to default values predefined in rtpsock.h
The parameters can be changed immediately after the controller is created by calling 
SetRtpStreamParameters(TUint aMaxDropout, TUint aMaxMisorder, TUint aMinSequential);
@see CRtpController::SetRtpStreamParameters
@internalTechnology
@param aRtpSocket An opened and connected RTP socket
@param aRtcpSocket An opened and connected RTCP socket
@param aCNAME The CNAME.
@param aPriority The priority to be used for internal active objects
@pre Sockets are opened and connected
@leave KErrNoMemory,	CRtpController::ConstructL
*/
	
CRtpController* CRtpController::NewL(RSocket& aRtpSocket, RSocket& aRtcpSocket,
									const TDesC8& aCNAME, TInt aPriority)
	{	
	__RTP_LOG(_L("CRtpController::NewL() aRtpSocket, aRtcpSocket, aCNAME"));
	CRtpController* self = new(ELeave) CRtpController;
	CleanupStack::PushL(self);
	self->ConstructL(aRtpSocket, aRtcpSocket,aCNAME, aPriority);
	CleanupStack::Pop(self);
	return self;
	}

/** Create a Controller when the RTP and RTCP handlers are created outside.
    The method is expected to be used by those components who would want to 
    Preprocess/PostProcess the RTP Packets. If the RTCP handler is not passed
    it is assumed that RTCP is absent for the time being. Later on the RTCP handler
    can be added by the API CreateRtcpL. 
    Please note that CreateRtcpL is relevant only in the case of external handlers
@see CRtpController::SetRtpStreamParameters
@internalTechnology
@param aRtphandler A Reference to RTP handler created outside.
@param aPriority The priority to be used for internal active objects
@param aLocalAddr The address to bind to at this end. 
@param aCNAME     The CNAME to be used by the session.
@leave KErrNoMemory, ConstructL
*/
EXPORT_C CRtpController* CRtpController::NewL(CRtpHandlerBase& aRtphandler, TInt aPriority, TSockAddr aLocalAddr, const TDesC8 &aCNAME)
	{	
	__RTP_LOG(_L("CRtpController::NewL aRtphandler "));
	CRtpController* self = new(ELeave) CRtpController;
	CleanupStack::PushL(self);
	self->ConstructL(&aRtphandler,aPriority,aLocalAddr, aCNAME);
	CleanupStack::Pop(self);
	return self;
	}

/** Create the controller from connected RTP socket.
This method is expected to be used for CO services, primarily for BT without RTCP.
The controller sets MinSequential, MaxMisorder, and MaxDropout parameters methods 
to default values predefined in rtpsock.h
The parameters can be changed immediately after the controller is created by calling 
SetRtpStreamParameters(TUint aMaxDropout, TUint aMaxMisorder, TUint aMinSequential);
@see CRtpController::SetRtpStreamParameters
@internalTechnology
@param aRtpSocket An opened and connected socket
@param aPriority The priority to be used for internal active objects
@pre Socket is opened and connected
@leave KErrNoMemory, ConstructL
*/	
CRtpController* CRtpController::NewL(RSocket& aRtpSocket, TInt aPriority)
	{	
	__RTP_LOG(_L("CRtpController::NewL aRtpSocket "));
	CRtpController* self = new(ELeave) CRtpController;
	CleanupStack::PushL(self);
	self->ConstructL(aRtpSocket,aPriority);
	CleanupStack::Pop(self);
	return self;
	}

CRtpController::~CRtpController()
	{
	__RTP_LOG1(_L("CRtpController::~CRtpController 0x%x"), this);
	// delte SM first as it has reference to source entries in the CollMng??
	delete iSourceStateMachine;
	delete iCollisionManager;
	delete iRtcpSender;
	delete iRtcpSessionData;
	iSamplingRatesArray.Close();
	
	// Delete the array of handlers and handlers themselves!
	iHandlers.ResetAndDestroy();
	__RTP_LOG(_L(""));
	__RTP_LOG(_L(""));
	__RTP_LOG(_L(""));
	}

void CRtpController::ActivateL(TRtpConnectionType/* aConnectionType*/, RSubConnection& /*aSubConnection*/)
	{
	
	}

void CRtpController::ActivateL(TRtpConnectionType aConnectionType, RConnection& aConnection)
	{	
	__RTP_LOG1(_L("CRtpController::ActivateL() ..., LocalSSrc => %u "), GetLocalSSRC());
	__RTP_LOG1(_L("\t[TRtpConnectionType] == %d"), static_cast<TInt>(aConnectionType));
	switch (aConnectionType)
		{
		case ERtpConnected:
			{
			if(RtpSocket().HasOwnSocket())
				{
				User::Leave(KErrArgument);
				}
			iConnectionType = aConnectionType;
			// no handler/bearer activation required for connected socket
			return;
			}
		case ERtpCL:
		case ERtpCOIncoming:
		case ERtpCOOutgoing:
			//
			break;
		case ERtpNotActive:
			// nothing to do here
			return;
		default:
			User::Leave(KErrArgument);
		}
	__ASSERT_DEBUG(iSocketServ, Panic(ERtpSocketServerNull));
	for(int i=0;i<iHandlers.Count();i++)
		{
		iHandlers[i]->ActivateL(*iSocketServ,aConnectionType,aConnection);
		}
	iConnectionType = aConnectionType;
	}
	
/** Selects the type of connection and opens sockets
@param aConnctionType The type of connection - Connectionless, Connection mode Incoming,
						Connection mode Outgoing, Connection mode Connected
@internalTechnology
@see TRtpConnectionType
*/
void CRtpController::ActivateL(TRtpConnectionType aConnectionType)
	{	
	__RTP_LOG1(_L("CRtpController::ActivateL() ..., LocalSSrc => %u "), GetLocalSSRC());
	__RTP_LOG1(_L("\t[TRtpConnectionType] == %d"), static_cast<TInt>(aConnectionType));
	switch (aConnectionType)
		{
		case ERtpConnected:
			{
			if(RtpSocket().HasOwnSocket())
				{
				User::Leave(KErrArgument);
				}
			iConnectionType = aConnectionType;
			// no handler/bearer activation required for connected socket
			return;
			}
		case ERtpCL:
		case ERtpCOIncoming:
		case ERtpCOOutgoing:
			//
			break;
		case ERtpNotActive:
			// nothing to do here
			return;
		default:
			User::Leave(KErrArgument);
		}
	__ASSERT_DEBUG(iSocketServ, Panic(ERtpSocketServerNull));
	for(int i=0;i<iHandlers.Count();i++)
		{
		iHandlers[i]->ActivateL(*iSocketServ,aConnectionType);
		}
	iConnectionType = aConnectionType;
	}

/** Set remote address for all handlers.
@internalTechnology
@param aRemoteAddr Remote address for this RTP session
*/

void CRtpController::SetRemoteAddr(const TSockAddr& aRemoteAddr)
	{	
	__RTP_LOG1(_L("CRtpController::SetRemoteAddr() ..., LocalSSrc => %u "), GetLocalSSRC());
	__RTP_LOG1(_L("\t[RemoteAddrPort] == %u"), aRemoteAddr.Port());
	__RTP_LOG1(_L("\t[RemoteAddrFamily] == %u"), aRemoteAddr.Family());
	for(int i=0;i<iHandlers.Count();i++)
		{
		//Seting the remote rtcp address and port
		if(iHandlers[i]->Type() == ERtp)
			{
			iHandlers[i]->SetRemoteAddress(aRemoteAddr);
			iHandlers[i]->SetDefaultRemoteAddress(aRemoteAddr);
			}
		else
			{
			TSockAddr temp(aRemoteAddr);
			temp.SetPort(aRemoteAddr.Port()+1);
			iHandlers[i]->SetDefaultRemoteAddress(temp);
			iHandlers[i]->SetRemoteAddress(temp);
			}
		}
		
	}

/** Set remote rtcp port for RTCP handler.

@internalTechnology
@param aRemoteAddr Remote address for this RTP session
*/
void CRtpController::SetRemoteRtcpPort(TUint aPort)
	{	
	__RTP_LOG1(_L("CRtpController::SetRemoteRtcpPort() ..., LocalSSrc => %u "), GetLocalSSRC());
	CRtpHandlerBase* rtcpHandler = RtcpHandler();
	if(rtcpHandler)
		{
		TSockAddr addr;
		addr = rtcpHandler->RemoteAddress();
		addr.SetPort(aPort);
		rtcpHandler->SetRemoteAddress(addr);
		rtcpHandler->SetDefaultRemoteAddress(addr);
		}
	else
		{
		//LOG
		// event to rtp client??
		}
	}


/**
Send a non-RTP (control) data packet asynchronously
    
@param aSocketType SocketType that tells whether non RTP data has to go through RTP or RTCP socket
    
@param aData Non RTP data that has to be sent
    
@param aStatus Tells whether the data has been sent successfully

*/
void CRtpController::SendDataL(TBool aSocketType, const TDesC8& aData, TRequestStatus&  aStatus)
	{
	if(aSocketType)
		{
		RSocket& sock = RtpSocket();

		/* Send the data through the socket to the remote address */
		sock.SendTo(aData,RtpHandler()->RemoteAddress(),0,aStatus);
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
			/* Send the data through the socket to the remote address */
			sock.SendTo(aData,RtcpHandler()->RemoteAddress(),0,aStatus);	
			}
		}
	}


/** This set methods sets parameters (stored in CRtpStateMachine) 
which are used to determine when a particular source and it's current sequence number 
is valid or not.
The method can be called only if the controller is in either ERtpStreamInitialised  or ERtpStreamInitialising stated,
basically it means that the set operation would be successful only when called immediately after creating the controller.
@internalTechnology
@param aMaxDropout - After a source is considered valid, the sequence number is considered
   valid if it is no more than aMaxDropout ahead of the last sequence number.
@param aMaxMisorder -  nor more than aMaxMisorder behind the last sequence number.
@param aMinSequential - a RTP source is declared valid only after aMinSequential packets 
	have been received in sequence. aMinSequential 1 means no delay and the very first RTP packet 
	from unknown source is processed where aMinSequential 2 means (obviously) a delay of 1 packet and so on...
@return error - returns KErrNone if successful or KErrInUse if already in use.
*/

TInt CRtpController::SetRtpStreamParameters(TUint aMaxDropout,
													TUint aMaxMisorder,
													TUint aMinSequential)
	{	
	__RTP_LOG1(_L("CRtpController::SetRtpStreamParameters() ..., LocalSSrc => %u "), GetLocalSSRC());
	__RTP_LOG1(_L("\t[MaxDropout] == %u"), aMaxDropout);
	__RTP_LOG1(_L("\t[MaxMisorder] == %u"), aMaxMisorder);
	__RTP_LOG1(_L("\t[MinSequential] == %u"), aMinSequential);
	__ASSERT_DEBUG(iSourceStateMachine,Panic(ERtpStateMachineNull));
	__ASSERT_ALWAYS(aMaxDropout>0,Panic(ERtpParametersZeroOrNegative));
	__ASSERT_ALWAYS(aMaxMisorder>0,Panic(ERtpParametersZeroOrNegative));
	__ASSERT_ALWAYS(aMaxMisorder>0,Panic(ERtpParametersZeroOrNegative));

	TInt error = KErrInUse;
	if((ERtpStreamInitialised == iStreamState)||(ERtpStreamInitialising == iStreamState))
		{
		iSourceStateMachine->SetRtpStreamParameters(aMaxDropout,aMaxMisorder,aMinSequential);
		error = KErrNone;
		}
	else
		{
		error = KErrInUse;
		}
	__RTP_LOG1(_L("\t==> %d"), error);
	return error;
	}


/** 
@internalTechnology
*/
TUint32 CRtpController::GetLocalSSRC()
	{
	return iCollisionManager->LocalSSRC();
	}

/** Handle received data according to the type of data
@internalComponent
@param aPacket RTP data packet received from network
@param aType - specifies a type of received data - RTP/RTCP/FEC
*/

void CRtpController::DoReceiveL(TDes8& aClientBuf, TRtpType aType)
	{	
	__RTP_LOG1(_L("CRtpController::DoReceiveL() ..., LocalSSrc => %u "), GetLocalSSRC());
	if(ERtpStreamActive !=iStreamState)
		{
		// If not active yet then upon receiving a packet we can either transition to Active (from SendOnly)
		// or to RcvOnly from any other state
		iStreamState = (ERtpStreamSendOnly==iStreamState ? ERtpStreamActive : ERtpStreamRcvOnly);
		}
	if(ERtp == aType)
		{
		DoRtpReceiveL(aClientBuf);
		}
	else if(ERtcp == aType)
		{
		DoRtcpReceiveL(aClientBuf);
		}
	}

/** Used to indicate that a packet is being sent, state of the stream is updated
@internalComponent
*/
TInt CRtpController::PacketBeingSent(TDes8& aPacket, TRtpType aPacketType)
	{		
	__RTP_LOG1(_L("CRtpController::PacketBeingSent() ..., LocalSSrc => %u "), GetLocalSSRC());
	__RTP_LOG1(_L("\t[StreamState] == %u"), static_cast<TInt>(iStreamState));
	/* Get the legth of the Packet we are sending out */
	TInt pktLength = aPacket.Length();

    if(ERtp == aPacketType)
		{	
		if(ERtpStreamActive !=iStreamState)
			{
			// If not active yet then when sending a packet we can either 
			//	transition to Active (from RcvOnly)
			//	or to SendOnly from any other state
			if(ERtpStreamRcvOnly==iStreamState)
				{
				iStreamState = ERtpStreamActive;
				}
			else
				{
				iStreamState = ERtpStreamSendOnly;
				}
			}
		if (iRtcpSessionData)
			{
			iRtcpSessionData->SetHasSentRtpPacket();
			/* If RTP Time has not been initialized in session data
		 	* it can be done now ( Because we know RtpTimeNow. This will
		 	* remove the dependency on RtpTimeConversion() API
		 	*/
			if(!iRtcpSender->iRtpTime.IsValid())
				{
				TRtpFixedHdrMemLayout header(aPacket);
				TUint32 samplerate = GetSamplingRate(header.PayloadType());
				TUint32 tickPerMusec = 1000*1000/samplerate; 
				iRtcpSender->iRtpTime.SetTimeConversion(header.TimeStamp(), tickPerMusec);
				}
			}
		CRtpSourceEntry* entry = iCollisionManager->GetLocalEntry();
		__ASSERT_DEBUG(entry,Panic(ERtpSourceEntryNull));

		entry->IncOctetsSent(pktLength);
		entry->IncPacketsSent();
	
		__RTP_LOG1(_L("\t==> StreamState == %u"), static_cast<TInt>(iStreamState));
	
		FillRtpFixedHeader(aPacket);

		/* if application has registered for Post Processing do it here */
		if(iPrePostProcessingCallback)
			{
			iPrePostProcessingCallback->PostRtpProcessing(aPacket);
			}
		}
	else if(ERtcp == aPacketType)
		{
		/* if application has registered for Post Processing do it here */
		if(iPrePostProcessingCallback)
			{
			iPrePostProcessingCallback->PostRtcpProcessing(aPacket);
			}
		}
	return KErrNone;
	}

CRtpController::CRtpController() :
								iConnectionType(ERtpNotActive),
								iStreamState(ERtpStreamInitialising),
								iSessionBandwidth(KRtpDefaultSessionBandwidth),
								iRtcpInterval(0),
								iPrePostProcessingCallback(NULL)
	{
	__RTP_LOG(_L("**** Creating new CRtpController ****"));	
	iLocalSocalSrcSeq = RtpUtils::GenerateRandomNumber16();
	}



/** Construct the RTP controller
@internalComponent
@leave KErrNotSupported if protocol family other then KAfInet is using only port number
		to initialise RTCP
@leave One of the system wide error codes if Append of a handler fails
*/
void CRtpController::ConstructL(RSocketServ& aServer, TSockAddr& aLocalAddr,
					const TDesC8& aCNAME, TUint aRtcpLocalPort, TInt aPriority)
	{	
	__RTP_LOG(_L("CRtpController::ConstructL() ... "));
	__RTP_LOG1(_L("\t[LocalAddrPort] == %u"), aLocalAddr.Port());
	__RTP_LOG1(_L("\t[LocalAddrFamily] == %u"), aLocalAddr.Family());
	__RTP_LOG1(_L("\t[RtcpLocalPort] == %u"), aRtcpLocalPort);
	__RTP_LOG1(_L("\t[Priority] == %d"), aPriority);
	iSocketServ = &aServer;
	
	// create the Collision Manager
	CRtpSourceEntry& localEntry = CreateCollisionManagerL(aLocalAddr,aCNAME);
	__ASSERT_DEBUG(iCollisionManager,Panic(ERtpCollisionMgrIsNotCreated));
	//
	CRtpHandlerBase* rtpHandler = CRtpHandlerBase::NewBearerL(this, /*aServer, */
														aLocalAddr, aPriority);
	__RTP_LOG2(_L("Rtp Handler 0x%x Created at port %u"), rtpHandler, rtpHandler->LocalAddress().Port());
	
	CleanupStack::PushL(rtpHandler);
	rtpHandler->SetType(ERtp);
	// lets set SSRC for RTP socket
	User::LeaveIfError(iHandlers.Append(rtpHandler));
	// The handler is owned by iHandlers from now
	CleanupStack::Pop(rtpHandler);
	iSourceStateMachine = CRtpStateMachine::NewL(aPriority);
	if(0 != aRtcpLocalPort)
		{
		if(KAfInet == aLocalAddr.Family())
			{
			// port number for RTCP works only for UDP
			// same transport address just different port no
			TInetAddr rtcpAddr(aLocalAddr);
			rtcpAddr.SetPort(aRtcpLocalPort);
			CRtpHandlerBase* rtcpHandler = 
				CRtpHandlerBase::NewBearerL(this, rtcpAddr, aPriority);
			__RTP_LOG2(_L("Rtcp Handler 0x%x Created at port %u"), rtcpHandler, rtcpHandler->LocalAddress().Port());
	
			CleanupStack::PushL(rtcpHandler);
			rtcpHandler->SetType(ERtcp);
			User::LeaveIfError(iHandlers.Append(rtcpHandler));
			// update the RTCP address in the Collision Manager
			localEntry.SetRtcpSourceAddr(rtcpAddr);
			// The handler is owned by iHandlers from now on
			CleanupStack::Pop(rtcpHandler);
			// and we'll need RTCP sender
			iRtcpSender = CRtcpSender::NewL(rtcpHandler, iCollisionManager);
			}
		else /*if(aCNAME.Compare(KNullDesC8))*/
			{
			// RTCP can't be configured just through the port number for other protocol families
			User::Leave(KErrNotSupported);
			}
		// lets create RTCP controller, with autonomous reports for now
		TUint rtcpBw = iSessionBandwidth*KRtcpBandwidthFraction/100;
		iRtcpSessionData = CRtcpSessionData::NewL(rtcpBw,ETrue,this,aCNAME);
		}
	iStreamState = ERtpStreamInitialised;
	}


void CRtpController::ConstructL(RSocket& aRtpSocket, TInt aPriority)
	{	
	__RTP_LOG(_L("CRtpController::ConstructL() ... "));
	iSocketServ = 0;
	// create the Collision Manager
	TSockAddr localAddress;
	aRtpSocket.LocalName(localAddress);
	CreateCollisionManagerL(localAddress,KNullDesC8);
	__ASSERT_DEBUG(iCollisionManager,Panic(ERtpCollisionMgrIsNotCreated));
	//
	CRtpHandlerBase* rtpHandler = CRtpHandlerBase::NewBearerL(this,aRtpSocket,aPriority);
	CleanupStack::PushL(rtpHandler);
	rtpHandler->SetType(ERtp);
	User::LeaveIfError(iHandlers.Append(rtpHandler));
	// The handler is owned by iHandlers from now on
	CleanupStack::Pop(rtpHandler);
	//
	iSourceStateMachine = CRtpStateMachine::NewL(aPriority);
	iStreamState = ERtpStreamInitialising;
	}

void CRtpController::ConstructL(RSocket& aRtpSocket, RSocket& aRtcpSocket,
					const TDesC8& aCNAME, TInt aPriority)
	{	
	__RTP_LOG(_L("CRtpController::ConstructL() ... "));
	__RTP_LOG1(_L("\t[Priority] == %d"), aPriority);

	TSockAddr localAddress;
	aRtpSocket.LocalName(localAddress);
	CreateCollisionManagerL(localAddress,aCNAME);
	__ASSERT_DEBUG(iCollisionManager,Panic(ERtpCollisionMgrIsNotCreated));
	//
	CRtpHandlerBase* rtpHandler = CRtpHandlerBase::NewBearerL(this,aRtpSocket,aPriority);
	CleanupStack::PushL(rtpHandler);
	rtpHandler->SetType(ERtp);
	User::LeaveIfError(iHandlers.Append(rtpHandler));
	// The handler is owned by iHandlers from now on
	CleanupStack::Pop(rtpHandler);
	//
	iSourceStateMachine = CRtpStateMachine::NewL(aPriority);
	iStreamState = ERtpStreamInitialising;

	//we need to create RTCP handler as well
	CRtpHandlerBase* rtcpHandler = CRtpHandlerBase::NewBearerL(this,aRtcpSocket,aPriority);
	CleanupStack::PushL(rtcpHandler);
	rtcpHandler->SetType(ERtcp);
	User::LeaveIfError(iHandlers.Append(rtcpHandler));
	// The handler is owned by iHandlers from now on
	CleanupStack::Pop(rtcpHandler);
	// and we'll need RTCP sender
	iRtcpSender = CRtcpSender::NewL(rtcpHandler, iCollisionManager);
	// lets create RTCP controller, no autonomous reports for now
	TUint rtcpBw = iSessionBandwidth*KRtcpBandwidthFraction/100;
	iRtcpSessionData = CRtcpSessionData::NewL(rtcpBw,ETrue,this,aCNAME);
	CRtpSourceEntry* localEntry = iCollisionManager->GetLocalEntry();
	TSockAddr rtcpAddress;
	aRtcpSocket.LocalName(rtcpAddress);
	localEntry->SetRtcpSourceAddr(rtcpAddress);
	}


/** Construct the RTP controller
Used when the Handlers are created outside the RTP Stack.
@internalComponent
@leave 
@leave One of the system wide error codes if Append of a handler fails
*/
void CRtpController::ConstructL(CRtpHandlerBase* aRtphandler, TInt aPriority, 
                                     TSockAddr aLocalAddr, const TDesC8 &aCNAME)
	{	
	__RTP_LOG(_L("CRtpController::ConstructL() aRtphandler ... "));
	__RTP_LOG1(_L("\t[Priority] == %d"), aPriority);
	/* We donot own any sessions to it */
	iSocketServ = NULL;
	
	// create the Collision Manager.
    CreateCollisionManagerL(aLocalAddr,aCNAME);
	__ASSERT_DEBUG(iCollisionManager,Panic(ERtpCollisionMgrIsNotCreated));
    
    //Should be set by the calling application.
	if(aRtphandler->Type()!=ERtp)
		{
		User::Leave(KErrArgument);
		}
	
	User::LeaveIfError(iHandlers.Append(aRtphandler));
	
	// The handler is owned by iHandlers from now
	iSourceStateMachine = CRtpStateMachine::NewL(aPriority);
	iStreamState = ERtpStreamInitialised;
	
	/* Also remember handlers are external */
	iExtHandlersUsed = ETrue;
	}

/** Add an RTCP Handler to the existing session. CNAME is
    Mandatory now. 
Used when the Handlers are created outside the RTP Stack.
@internalComponent
@leave 
@leave One of the system wide error codes if Append of a handler fails
*/
void CRtpController::CreateRtcpL(CRtpHandlerBase* aRtcphandler,const TDesC8& aCNAME)

	{
	__RTP_LOG(_L("CRtpController::CreateRtcpL(aRtcphandler,aCNAME)"));
	
	/* RTP handler should also be external */
	ASSERT(iExtHandlersUsed);
	if(aRtcphandler->Type() != ERtcp)
		{
		User::Leave(KErrArgument);
		}
	
	if(RtcpHandler())
		{
		/* So create RTCP was already called. */
	    User::Leave(KErrAlreadyExists);	
		}
	
	
	// The handler is owned by iHandlers from now on
	User::LeaveIfError(iHandlers.Append(aRtcphandler));
	
	// and we'll need RTCP sender
	iRtcpSender = CRtcpSender::NewL(aRtcphandler, iCollisionManager);
	
	// lets create RTCP controller, no autonomous reports for now
	TUint rtcpBw = iSessionBandwidth*KRtcpBandwidthFraction/100;
	iRtcpSessionData = CRtcpSessionData::NewL(rtcpBw,ETrue,this,aCNAME);
	
	iCollisionManager->SetLocalCnameL(aCNAME);
	CRtpSourceEntry* localEntry = iCollisionManager->GetLocalEntry();
    
    //This is bit of a joke..Why do this?
	TSockAddr rtcpAddress;
	localEntry->SetRtcpSourceAddr(rtcpAddress);
	/* Now we have a CNAME. So update the SDES Items list to
	   insert this CNAME in to it */
	iRtcpSessionData->UpdateSdesItemsL(aCNAME);
	}


CRtpHandlerBase* CRtpController::RtcpHandler()
	{
	__RTP_LOG(_L("CRtpController::RtcpHandler() ... "));
	TInt count = iHandlers.Count();
	for(int i = 0;i<count;i++)
		{
		CRtpHandlerBase* handlerPtr = iHandlers[i];
		if(ERtcp == handlerPtr->Type())
			return handlerPtr;
		}
	return NULL;
	}


void CRtpController::DoRtpReceiveL(TDes8& aData)
	{	
	__RTP_LOG1(_L("CRtpController::DoRtpReceiveL() ..., LocalSSrc => %u "), GetLocalSSRC());
	// we need at least one byte to check
	if(1 > aData.Length())
		User::Leave(KErrUnderflow);

	CRtpSourceEntry* entry = NULL;
	// Get the RTP handler
	CRtpHandlerBase* rtpHandler = iHandlers[0];
	// If not valid RTP packet, nothing is indicated to client...
	// client keeps waiting for next RTP packet
	TRtpFixedHdrMemLayout header(aData);
	__RTP_LOG(_L("CRtpController::DoRtpReceive() ... "));
	TInt error = CRtpControllerBase::CheckRtpPacket(aData);
	//check for non RTP data
	if((KErrNone != error) && (iEnableNonRtpData))
		{
		/* Notify that a non RTP packet is received */
		rtpHandler->CompleteRead(KErrCorrupt);
		return;
		}
    
	/* Before we start with any further processing we should check for
	 * PreProcessing requirements. Now that we have made sure the packet\
	 * is RTP we can issue the callback here
	 */
	if(iPrePostProcessingCallback)
		{
		User::LeaveIfError(iPrePostProcessingCallback->PreRtpProcessing(aData));
		}

	if(KErrNone == error)
		{
		if(KRtpFixedNoCSRCHeaderLength > aData.Length())
			error = KErrUnderflow;
		else
			entry = UpdateSourceTableL(ERtp,header.SSRC(),rtpHandler->RecvAddress());
		}
	if(KErrUnderflow == error)
		{
		// may seem strange not to complete with an error...
		// we know it's to short but the bits we have are correct,
		// and rtp.dll is ready to deal with that...
		rtpHandler->CompleteRead(KErrNone);
		return;
		}
	TRtpSourceState state = ERtpInvalid;
	if(entry)
		{
		__ASSERT_DEBUG(iSourceStateMachine,Panic(ERtpStateMachineNotCreated));
		state = iSourceStateMachine->NextL(*entry,
									ERtpPacketReceived,header.SequenceNumber());
		}
	__RTP_LOG1(_L("\tstate == %d"), static_cast<TInt>(state));
	if(ERtpValid == state)
		{
		// we update RTCP stats only for valid RTP packets
		// The 'current' rtp time should be determined by the appropriate
		// SourceEntry. There is no relation between the session wide RTP clock
		// and clock of the entry. Each entry maintains its own RTP time.
		// PayloadType is passed only so that if this is the First RTP ever to be recorded
		// we can initialise rtptime (duh!!)
		entry->UpdateRtcpStatisticsL(header.TimeStamp(),header.PayloadType());
		rtpHandler->CompleteRead(KErrNone);
		}
	else if (ERtpDelete == state)
		{
		__ASSERT_DEBUG(entry,Panic(ERtpSourceEntryNull));
		// we can get here after RTCP BYE received
		// how do we know it's a sender? We should have a filed in entry!! 
		// Sender for now...
		DeleteEntryL(header.SSRC());
		}
	else
		{
		// lets try with another RTP packet...
		rtpHandler->ReadMore();
		}	
	}

// aData is used to send info back the client
// the incoming packet is stored in sessionData
void CRtpController::DoRtcpReceiveL(TDes8& aClientData)
	{	
	__RTP_LOG1(_L("CRtpController::DoRtcpReceiveL() ..., LocalSSrc => %u "), GetLocalSSRC());
	
	// It is fine to use a pointer to TPtr8 
	// as the data is a data memeber of CRtcpSessionData class
	TPtrC8 savedData = aClientData;
	__RTP_LOG2(_L("\tReceived data Length %d, address 0x%x"), savedData.Length(), savedData.Ptr());

	// we need at least one byte to check
	if(1 > savedData.Length())
		User::Leave(KErrUnderflow);

	// Get the RTCP handler
	CRtpHandlerBase* rtcpHandler = RtcpHandler();
	// we get here from Rtcp handler so it shoudn't be null ever but...
	__ASSERT_ALWAYS(rtcpHandler,Panic(ERtpHandlerIsNotCreated));
	__ASSERT_DEBUG(iSourceStateMachine,Panic(ERtpStateMachineNotCreated));

	TInt parsed = 0, totLen = savedData.Length();
	TUint packetType = 0;
	TRtpSourceState state = ERtpInvalid;
	CRtpSourceEntry* entry = NULL;
	TBool isBye = EFalse;
	TInt err = CRtpControllerBase::CheckRtcpPacket(savedData, isBye);
	//check for non RTCP data

	if((KErrNone != err) && (iEnableNonRtcpData))
		{
		/* Notify that a non RTCP packet is received */
		rtcpHandler->CompleteRead(KErrCorrupt);
		return;
		}

	/* Now that we have verified this is a valid RTCP Packet. The PreProcessing
	 * callbacks if any can be issued from here. */
    if(iPrePostProcessingCallback)
		{
		User::LeaveIfError(iPrePostProcessingCallback->PreRtcpProcessing(aClientData));
		}

	while (parsed < totLen)
			{
			TPtrC8 fixed = savedData.Mid(parsed);
			TRtcpFixedHdrMemLayout header(fixed);
			parsed += (header.Length()+1)*4;
			entry = iCollisionManager->FindEntry(header.SSRC());
			packetType = header.PacketType();
			switch(packetType)
				{
				case ERtcpBYE:
					{
					__RTP_LOG(_L("\t ERtcpBYE"));
					const TUint numSrc = header.Count();
					for (TUint count = 1; count <= numSrc; count++)
						{
						const TUint32 ssrc= TRtcpMemLayout::Swap32(*reinterpret_cast<TUint32*>(const_cast<TUint8*>(fixed.Ptr())+ 4*count));
						__RTP_LOG1(_L("\t\t for SSRC %u"), ssrc);
						entry = iCollisionManager->FindEntry(ssrc);
						if(entry)
							{
							state = iSourceStateMachine->NextL(*entry,ERtcpByeReceived,0);
							}
						}
					break;
					}
				case ERtcpSDES:
					{
					if (!entry && !isBye)
						{
						const TInt chunkCount = header.Count();
						TUint32 ssrc = 0;
						TInt parsedInSDes = 0;
						for (TInt indx = 0; indx < chunkCount; indx++)
							{
							TPtrC8 chunk(fixed.Mid(parsedInSDes + KRtcpSDesFixedSize));
							ssrc = TRtcpMemLayout::Swap32(*reinterpret_cast<TUint32*>(const_cast<TUint8*>(chunk.Ptr())));

							__RTP_LOG1(_L("\t ERtcpSDES - Chunk, SSRC = %u"), ssrc);
							
							parsedInSDes += KRtpSsrcSize;
							entry = UpdateSourceTableL(ERtcp, ssrc, rtcpHandler->RecvAddress()); // take care of leave

							FOREVER
								{
								TRtcpSdesItemMemLayout item(chunk, parsedInSDes);
								__RTP_LOG1(_L("\t ERtcpSDES - Item %d"), item.GetType());

								if (item.GetType() == EsdesCNAME)
									{
									entry->SetCnameL(item.GetAttribute());
									}
								parsedInSDes+=item.GetLength() + KRtcpSDesItemFixedSize;
								if (parsedInSDes >= fixed.Length())
									{
									break;
									}
								if (item.GetType() == EsdesNONE)
									{
									break;
									}
								}
							}
						parsed = 0; // reparse the packet
						break;
						}
					// Follow through
					}
				case ERtcpRR:
				case ERtcpSR:
				case ERtcpAPP:
					{
					__RTP_LOG1(_L("\t Type == %u"), packetType);
					if (entry)
						{
						state = iSourceStateMachine->NextL(*entry,ERtcpPacketReceived,0);
						__RTP_LOG1(_L("\t State == %d"), static_cast<TInt>(state));
						if(ERtpValid == state)
							{
							if(ERtcpSR == packetType)
								{
								TRtcpSRSenderInfoMemLayout srSenderInfo(fixed,KRtcpSRHeaderSize);
								// update last SR Timestamp (NTP time from packet)
								TNTPTime ntp(srSenderInfo.NtpMsb(), srSenderInfo.NtpLsb());
								entry->SetLastSRTimestamp(ntp.GetCompactTime());
								// reuse
								RtpUtils::GetNTPTime(ntp);
								// update when actualy the last SR has been recieved
								entry->SetWhenLastSRReceived(ntp.GetCompactTime());
								}
							}
						}
					break;
					}
				default:
					{
					//uknown type should just ignore it!
					if (entry)
						{
						state = iSourceStateMachine->NextL(*entry,ERtcpPacketReceived,0);
						}
					break;
					}
				} // end of switch
			} // while (parsed < totLen)
	rtcpHandler->CompleteRead(err);
	}

/** 
@internalComponent
@param aType - either RTP or RTCP
@param aSRC - Either SSRC or CSRC from a packet
@param aSourceAddr - source address for this source (remote address)
@return
*/
CRtpSourceEntry* CRtpController::UpdateSourceTableL(TRtpType aType, TUint32 aSRC, TSockAddr& aSourceAddr)
	{	
	__RTP_LOG1(_L("CRtpController::UpdateSourceTableL() ..., LocalSSrc => %u "), GetLocalSSRC());
	__RTP_LOG2(_L("\tRemoteSSrc %u, RemotePort %u"), aSRC, aSourceAddr.Port());
	CRtpSourceEntry* entry = NULL;
	// NOT to be used FOR RTCP BYE packet!!!!
	// check if known source
	entry = iCollisionManager->FindEntry(aSRC);
	if(NULL == entry)
		{
		// new source identified!
		entry = iCollisionManager->CreateNewSourceL(aSRC);
		if(ERtp == aType)
			{
			entry->SetRtpSourceAddr(aSourceAddr);
			}
		else if(ERtcp == aType)
			{
			entry->SetRtcpSourceAddr(aSourceAddr);
			}
		// set probation count
		entry->SetProbationCount(iSourceStateMachine->MinSequential());
		
		if((ERtp == aType) && iRtcpSessionData)
			{
			// how do we know it's a sender? 
			// We recieved RTP so must be sender? */
			iRtcpSessionData->UserJoinedSession(ETrue);
			}
		// Should we do this for RTCP SR? But then we'd have to check if already reported when recieved
		// RTP from this source... hmmm...
		}
	else
		{
		if(iCollisionManager->IsCollisionOrLoopback(entry,aSourceAddr,aType))
			{
			//do collision
			// if we should not continue with processing then we do not need entry any more..
			if(!iCollisionManager->ResolveRtpCollisionOrLoopL(aSRC,aSourceAddr,
				ERtpStreamRcvOnly==iStreamState ? aType : aType|KIsAlreadySending))
				entry = NULL;
			}
		}
	return entry;
	}

void CRtpController::DeleteEntryL(TUint32 aSRC)
	{	
	__RTP_LOG2(_L("CRtpController[0x%x]::DeleteEntry() ... %u"), this, aSRC);

	CRtpSourceEntry* entry = iCollisionManager->FindEntry(aSRC);
	if (entry)
		{
		iSourceStateMachine->NextL(*entry, ERtcpByeReceived, 0);
		if(iRtcpSessionData)
			{
			iRtcpSessionData->UserLeftSession(ETrue);
			}
		}
	}
TInt CRtpController::RtcpStatistics(TUint32 aSsrc, TRtcpStatistics &aRtcpStatistics)
	{
	CRtpSourceEntry * rtpSourceEntry = iCollisionManager->FindEntry(aSsrc);
	if(!rtpSourceEntry)
		{
		return KErrNotFound;
		}
	else
		{
		aRtcpStatistics.SetPacketsSent(rtpSourceEntry->PacketsSent());
		aRtcpStatistics.SetOctetsSent(rtpSourceEntry->OctetsSent());
		aRtcpStatistics.SetJitter(rtpSourceEntry->Jitter());
		aRtcpStatistics.SetPacketsLost(TRtcpPacket::NumberOfPacketsLost(*rtpSourceEntry));
		aRtcpStatistics.SetFractionLost(TRtcpPacket::FractionOfPacketsLostInterval(*rtpSourceEntry));	
		return KErrNone;
		}
	}
/**
@param aLocalAddr - local RTP address (port number of RTP expected here)
@return - local entry
*/
	
CRtpSourceEntry& CRtpController::CreateCollisionManagerL(TSockAddr& aLocalAddr,const TDesC8& aCNAME)
	{	
	// we are just creating the collision manager thus we don't have to check 
	// first existence of our own SSRC in the table
	TUint32 ssrc = RtpUtils::GenerateRandomNumber32L();
	iCollisionManager = CRtpCollisionMng::NewL(this,ssrc,aCNAME);
	CRtpSourceEntry* entry = iCollisionManager->GetLocalEntry();
	// we set the RTP source address
	entry->SetRtpSourceAddr(aLocalAddr);
	// and client must set the RTCP one if appropriate
	return *entry;
	}

/** 
Gets a handle to the RTP handler;
@internalTechnology
*/
CRtpHandlerBase* CRtpController::RtpHandler()
	{
	/*
	NOTE: no need for ASSERT(??) since the operator itself raises a USER-130 panic. 
	*/
	__RTP_LOG(_L("CRtpController::RtpHandler() ... "));
	__ASSERT_DEBUG(iHandlers.Count(), Panic(ERtpHandlerIsNotCreated));
	// RTP handler is always first in the array
	CRtpHandlerBase* handlerPtr = iHandlers[0];
	return handlerPtr;
	}

RRtpSocket& CRtpController::RtcpSocket()
	{

	// RRtpSocket creation code still present  to prevent warning
	__RTP_LOG(_L("CRtpController::RtcpSocket() ... "));
	CRtpHandlerBase* handlerPtr = RtcpHandler();
	__ASSERT_ALWAYS(handlerPtr, Panic(ERtpHandlerIsNotCreated));
	return handlerPtr->RtpSocket();	
	}
	
void CRtpController::SetPrePostProcessingCallback(MPrePostProcessingCallback* aPrePostProcessingCallback)
	{
	iPrePostProcessingCallback = aPrePostProcessingCallback;
	}
	

TUint32 CRtpController::CurrentRtpTimeStampL()
	{
	TUint32 time = 0;
	// if sender doesn't exist's we are in RTP only mode thus don't need RTP time anyway
	if(iRtcpSender)
		time = iRtcpSender->iRtpTime.GetTimeStampL();
	
	return time;
	}

/**
   	Supplies information about the profile-specific RTP
    timestamps. 
@internalTechnology    
@param aNow        The time now in RTP format
@param aConversion The number of nanoseconds per RTP timer tick.
*/	
void CRtpController::SetRTPTimeConversion(TUint aNow, TUint aConversion)
	{	
	// if sender doesn't exist's we are in RTP only mode thus don't need RTP time anyway
	if(iRtcpSender)
		iRtcpSender->iRtpTime.SetTimeConversion(aNow,aConversion);
	iSamplingRate = 1000000/aConversion;
	}

/** 
Schedules BYE packet using up-to-date RTCP statistics collected for the specified source.
The local source identifier is used in case we need to send with a different ssrc in case of a conflict
@internalTechnology
@param aSRC - Local source identifier - either SSRC or CSRC
*/
void CRtpController::SendBYEPacketL(TUint32 aLocalSRC, const TDesC8& aReason)
	{
	if(iRtcpSessionData)
		{
		iRtcpSessionData->SetByePendingL(aLocalSRC, aReason);
		}
	}

void CRtpController::SendSDESL(TInt aType, const TDesC8& aValue)
	{
	if(iRtcpSessionData)
		{
		iRtcpSessionData->SetSDesPendingL(GetLocalSSRC(), static_cast<TUint8>(aType), aValue);
		}
	}

void CRtpController::SetPRIVL(const TDesC8& aPrefixString, const TDesC8& aValueString)
	{	
	if(iRtcpSessionData)
		{
		iRtcpSessionData->SetPRIVL(GetLocalSSRC(), aPrefixString, aValueString);
		}
	}

/** 
Sends APP packet for the specified source.
The app packet is the only non-compound packet
@internalTechnology
@return - null
**/
void CRtpController::SendAppPacketL(const TDesC8& aName, const TDesC8& aAppData, TUint8 aSubType)
	{
	if(iRtcpSessionData)
		{
		iRtcpSessionData->WriteAppPacket(aName, aAppData, aSubType);
		SendRTCPPacketL();
		}
	}


/* TO BE DONE: CleanUp this function.
   Maybe come up with a neater way of Handling these requests?
   Can we try using SetParams() ?
   */
void CRtpController::ProcessRTCPRequestL(TDes8& aRequest)
	{	
	__RTP_LOG(_L("CRtpController::ProcessRTCPRequest(aRequest)"));
	ASSERT(iExtHandlersUsed);
	TUint8* headerOctet = const_cast<unsigned char*>(aRequest.Ptr());
         switch((TRtcpPacketType )*(headerOctet))
             {
             case 199:
                 {
                 __RTP_LOG1(_L("ERtcpParam: %d"),*(headerOctet));
                 TRtpParam rtcpCompound(aRequest);
                 TPtrC8  dataPtr = rtcpCompound.GetData();

 				 iRtcpSessionData->SetSDESFlags(dataPtr[0]);

				 iRtcpSessionData->CanSendReportL();
                 break;
                 }
             case ERtcpAPP:
                 {           
                 __RTP_LOG1(_L("ERtcpAPP: %d"),*(headerOctet));
                             // Parsing....
                 TRtcpAPPPack rtcpApp(aRequest);
                 SendAppPacketL(rtcpApp.AppName(),rtcpApp.AppData(),rtcpApp.SubType());
                 break;
                 }           
             case ERtcpBYE:
                 {
                 __RTP_LOG1(_L("ERtcpBYE: %d"),*(headerOctet));
                 TRtcpBYEPack rtcpBye(aRequest);
                 SendBYEPacketL(GetLocalSSRC(),rtcpBye.Reason());
                 break;
                }
             
             case ERtcpSDES:
                 {
                 __RTP_LOG1(_L("ERtcpSDES: %d"),*(headerOctet));
                 TRtcpSdesPack rtcpSdes(aRequest);
                 TUint type = rtcpSdes.Type();
                 if (rtcpSdes.Type() == 8)
                     {
                     SetPRIVL(rtcpSdes.Attribute(),rtcpSdes.PreFixString());
                     }
                 else
                     {
                     SendSDESL(rtcpSdes.Type(),rtcpSdes.Attribute());
                     }
                 break;
                 }
             default:
                 break;
             }
	}

void CRtpController::SendRTCPPacketL(TDesC8& aPacketType)
	{	
	iRtcpSessionData->SetSDESFlags(aPacketType[0]);
	iRtcpSessionData->CanSendReportL();
	}

TBool CRtpController::AutoSendOption() const
	{	
	return iRtcpSessionData->PeriodicReportsStatus();
	}

void CRtpController::SetAutoSendOption(TBool aOnOrOffTag)
	{
	__RTP_LOG2(_L("CRtpController::SetAutoSendOption %d ..., LocalSSrc => %u "), aOnOrOffTag, GetLocalSSRC());

	if(aOnOrOffTag)
		{
		iRtcpSessionData->StartPeriodicReports();
		}
	else
		{
		iRtcpSessionData->StopPeriodicReports();		
		}
	}

/*
 * Stops receiving on the corresponding type of socket
 * If aTag is false, stop receiving on that type of socket
 */
void CRtpController::StopReceive(TInt aRtpOrRtcp)
	{	
	CRtpHandlerBase* cancelHandler = NULL;
	if(aRtpOrRtcp==ERtp)
		{
		cancelHandler = RtpHandler();
		}
	else if(aRtpOrRtcp==ERtcp)
		{
		cancelHandler = RtcpHandler();
		}
	
	if(cancelHandler)
		{
		if(cancelHandler->IsActive())
			{
			//A read is pending on the handler
			//Cancel any outstanding read on it first...
			cancelHandler->CancelRecv();
			}
		cancelHandler->DoReceiveTag(EFalse);
		}
	}

void CRtpController::SendRTCPPacketL()
	{
	__RTP_LOG2(_L("CRtpController::SendRTCPPacketL() of Length %d from LocalSSrc => %u "), iRtcpSessionData->iRtcpSendData->Length(), GetLocalSSRC());
	iRtcpSender->QueueAndSendMessageL(iRtcpSessionData->iRtcpSendData);
	}

void CRtpController::HandleCollision()
	{
	if (iRtcpSessionData)
		{
		TInt err;
		TRAP(err, iRtcpSessionData->UpdateSdesItemsL(iCollisionManager->GetLocalEntry()->Cname()));
		// ignore error - as this was not vital anyway
		(void)err;
		}
	}


TInt CRtpController::SetSamplingRate(TInt aPayloadType, TUint aSamplingRate)
	{	
	TUint ticksPerMuSec = 1000000/aSamplingRate;
	if(aPayloadType < KRtpDynamicPayloadStart)
		{
		/* The static payload mappings should not be changed */
		return (KRtpPayloadClockConversions[aPayloadType] == ticksPerMuSec)?KErrNone:KErrNotSupported;
		}
	
	/* Find if there is an entry for this sampling rate already */
	TInt i =0;
	for(i=0; i< iSamplingRatesArray.Count(); i++)
		{
		if(iSamplingRatesArray[i].iPayloadType == aPayloadType)
			{
			iSamplingRatesArray[i].iSamplingRate = aSamplingRate;
			return KErrNone;
			}
		}
	/* Insert the sampling rate in to the array */
	TSamplingRateInfo sampInfo;
	sampInfo.iPayloadType = aPayloadType;
	sampInfo.iSamplingRate = aSamplingRate;
	return iSamplingRatesArray.Append(sampInfo);
	}


TUint CRtpController::GetSamplingRate(TUint aPayloadType)
	{
	if(aPayloadType < KRtpDynamicPayloadStart && KRtpPayloadClockConversions[aPayloadType])
		{
		/* The static payload mappings can be readily returned */
		return 1000000/KRtpPayloadClockConversions[aPayloadType];
		}

	/* Find if there is an entry for this sampling rate */
	TInt i =0;
	for(i=0; i< iSamplingRatesArray.Count(); i++)
		{
		if(iSamplingRatesArray[i].iPayloadType == aPayloadType)
			{
			return iSamplingRatesArray[i].iSamplingRate;
			}
		}
	/* SetSampling rate was never called. So return the default
	 * sampling rate */
	return iSamplingRate;
	}

/**
Fills some fields of RTP fixed header: version, sequence number and SSRC

@internalComponent
@return 
@param 
@param 
@pre Padding & Extension bits are pre-set by client according to packet type
@post 
*/
void CRtpController::FillRtpFixedHeader(TDes8& aPacket)
	{	
	/*
		TRtpFixedHeader
		0                   1                   2                   3
		0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|V=2|P|X|  CC   |M|     PT      |       sequence number         |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|                           timestamp                           |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|           synchronization source (SSRC) identifier            |
		+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
		|            contributing source (CSRC) identifiers             |
		|                             ....                              |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/

		TRtpFixedHdrMemLayout header(aPacket);
		header.SetVersion2();
		
		//__ASSERT_DEBUG(0 == header.CC(),Panic(ERtpSocketCSRCNotZero));
		
		header.SetSequenceNumber(iLocalSocalSrcSeq++);
		//padding and extension bits set by client!!
		header.SetSSRC(GetLocalSSRC());
	}

/* Updation in local address can happen in the case of external handlers.
   More often when the external handlers are created they maynot know their
   local address for quite some time.
   For eg: the RTPFLOW do not know about its local address untill a Packet is
   send out or until the user calls bind on the socket. 
   For more details see CRtpFlow and CRtcpFlow 
   */
void CRtpController::SetLocalAddress(const TSockAddr& aLocalAddr)
	{
	__RTP_LOG(_L("CRtpController::SetLocalAddress()"));
	ASSERT(iExtHandlersUsed);
	TSockAddr localAddress;
	localAddress = aLocalAddr;
	
	/* Set the local address on the entry corresponding to local SSRC */
	__ASSERT_DEBUG(iCollisionManager,Panic(ERtpCollisionMgrIsNotCreated));
	
	CRtpSourceEntry *entry = iCollisionManager->GetLocalEntry();
	entry->SetRtpSourceAddr(localAddress);
	
	//Assuming RTCP port is RTP + 1
	localAddress.SetPort(aLocalAddr.Port()+1);
	entry->SetRtcpSourceAddr(localAddress);
	}

