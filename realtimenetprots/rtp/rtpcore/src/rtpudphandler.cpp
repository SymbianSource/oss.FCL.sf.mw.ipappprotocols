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
// class for handling udp data flow
// 
//

/**
 @file
 @internalComponent
*/

#include <e32base.h>
#include <rtp_proto_id.h>
#include "rtpcontroller.h"
#include "rtpudphandler.h"
#include "rtputils.h"

#include "rtpapi.h"
//
#include "rtcpsessiondata.h"

	
CRtpUdpHandler* CRtpUdpHandler::NewL(TInt aPriority)
	{	
	__RTP_LOG(_L("CRtpUdpHandler::NewL() ... "));		
	CRtpUdpHandler* self = new(ELeave) CRtpUdpHandler(aPriority);
	return self;
	}


	
CRtpUdpHandler* CRtpUdpHandler::NewL(RSocket& aSocket, TInt aPriority)
	{	
	__RTP_LOG(_L("CRtpUdpHandler::NewL() ... "));
	// the socket must be already connected in this case!!
	CRtpUdpHandler* self = new(ELeave) CRtpUdpHandler(aSocket, aPriority);
	return self;
	}

void CRtpUdpHandler::Send(TDes8& aBuffer, TRequestStatus& aStatus)
	{	
	
	__RTP_LOG(_L("CRtpUdpHandler::Send() ... "));
	__ASSERT_DEBUG(iRtpController, Panic(ERtpControllerIsNotCreated));
	// update the stream statemachine first
	iRtpController->PacketBeingSent(aBuffer, Type());
	__RTP_LOG_STMT(TBuf<50> buf; iRemoteAddr.Output(buf););
	__RTP_LOG2(_L("\tTo Remote Address %S, Port %u"), &buf, iRemoteAddr.Port());
	__RTP_LOG_STMT(iLocalAddr.Output(buf););
	__RTP_LOG2(_L("\tFrom Local Address %S, Port %u"), &buf, iLocalAddr.Port());
	iRtpSocket.SendTo(aBuffer, iRemoteAddr, 0, aStatus);
	}

// this could only be a genuine read from client
void CRtpUdpHandler::Read(TDes8& aClientBuffer)
	{	
	__RTP_LOG(_L("CRtpUdpHandler::Read() "));

	if( !iRecvOn )
		{
		/* Just return here. The Callback will never be triggered anyway
		 * because we are not initiating a Read here.
		 */
		return;
		}

	/* Always we should not be active. There is no concept of Parallel reads */
	if(IsActive())
	    {
	    Panic(ERtpIsAlreadyActive);
	    }
    // we are not active yet so we can go ahead with read
    SaveReadStatus(aClientBuffer);
    __ASSERT_ALWAYS(iClientDataPtr->MaxLength() > 0,Panic(ERtpReadMoreCalledWithoutData));
		__RTP_LOG_STMT(TBuf<50> buf; iLocalAddr.Output(buf););
		__RTP_LOG2(_L("\tLocalAddress %S, Port %u"), &buf, iLocalAddr.Port());
		iRtpSocket.RecvFrom(*iClientDataPtr, iRecvAddr, 0, iStatus);
		SetActive();
		}

void CRtpUdpHandler::ReadMore()
	{
	__RTP_LOG(_L("CRtpUdpHandler::ReadMore() ... "));
	Read(*iClientDataPtr);
	}

void CRtpUdpHandler::RunL()
	{
	__RTP_LOG(_L("CRtpUdpHandler::RunL() ... Read completed"));
	__RTP_LOG2(_L("\tInternal Data at address 0x%x, %d bytes"),
		iClientDataPtr->Ptr(), iClientDataPtr->Length());
	__RTP_LOG_STMT(TBuf<50> buf; iRecvAddr.Output(buf););
	__RTP_LOG2(_L("\tFrom RemoteAddress %S, Port %u"), &buf, iRecvAddr.Port());
	__RTP_LOG_STMT(iLocalAddr.Output(buf););
	__RTP_LOG2(_L("\tLocalAddress %S, Port %u"), &buf, iLocalAddr.Port());
	Deque();
	CActiveScheduler::Add(this);
	CRtpHandlerBase::RunL();
	}
// Added on 21/04/08

void CRtpUdpHandler::ActivateL(RSocketServ& aServer,TRtpConnectionType aConnectionType,RSubConnection& aSubConnection)
	{	
	
	// No Implementation
	__RTP_LOG(_L("CRtpUdpHandler::ActivateL() ... "));
	__RTP_LOG1(_L("\t[ConnetionType] == %d"), static_cast<TInt>(aConnectionType));
	// connection type is stored in RtpController
	__RTP_LOG_STMT(TBuf<50> buf; iRemoteAddr.Output(buf););
	TInt protoType;
	switch(Type())
		{
		case ECFRtp:
			protoType = KProtocolRtp;
			break;
		case ECFRtcp:
			protoType = KProtocolRtcp;
			break;
		default:
			protoType = KProtocolInetUdp;
			break;
		}
	
	switch (aConnectionType)
		{
		case ERtpConnected:	
			// connected socket provided and owned by client
			// we shouldn't really get here as SocketServ is NULL for conected
			Panic(ERtpHandlerWrongConnType);
		case ERtpCOOutgoing:
			// how do we know address? could do HR now...
			__RTP_LOG(_L("\tUser::LeaveIfError(iRtpSocket.Open(...))"));
			User::LeaveIfError(iRtpSocket.Open(aServer,iLocalAddr.Family(),KSockDatagram,protoType,aSubConnection));
			__RTP_LOG(_L("\tUser::LeaveIfError(iRtpSocket.Bind(...))"));
			User::LeaveIfError(iRtpSocket.Bind(iLocalAddr));
			__RTP_LOG2(_L("\tConnecting To Address %S, Port %u"), &buf, iRemoteAddr.Port());
			iRtpSocket.Connect(iRemoteAddr, iStatus);
			SetActive();
			break;
		case ERtpCOIncoming:
			{
			__RTP_LOG(_L("\tUser::LeaveIfError(KErrNotSupported)"));
			User::Leave(KErrNotSupported);
			}
		case ERtpCL:
		case ERtpCF:
			{
            __RTP_LOG(_L("\tUser::LeaveIfError(iRtpSocket.Open(...))"));
            User::LeaveIfError(iRtpSocket.Open(aServer,KAfInet,KSockDatagram,protoType,aSubConnection));
			__RTP_LOG(_L("\tUser::LeaveIfError(iRtpSocket.Bind(...))"));
			User::LeaveIfError(iRtpSocket.Bind(iLocalAddr));
			break;
			}
		default:
			{
			__RTP_LOG(_L("\tUser::LeaveIfError(KErrNotSupported)"));
			User::Leave(KErrNotSupported);
			}
		}
	
	}


// Added on 24/03
void CRtpUdpHandler::ActivateL(RSocketServ& aServer,TRtpConnectionType aConnectionType, RConnection& aConnection)
	{	
	__RTP_LOG(_L("CRtpUdpHandler::ActivateL() ... "));
	__RTP_LOG1(_L("\t[ConnetionType] == %d"), static_cast<TInt>(aConnectionType));
	// connection type is stored in RtpController
	__RTP_LOG_STMT(TBuf<50> buf; iRemoteAddr.Output(buf););
	switch (aConnectionType)
		{
		case ERtpConnected:	
			// connected socket provided and owned by client
			// we shouldn't really get here as SocketServ is NULL for conected
			Panic(ERtpHandlerWrongConnType);
		case ERtpCOOutgoing:
			// how do we know address? could do HR now...
			__RTP_LOG(_L("\tUser::LeaveIfError(iRtpSocket.Open(...))"));
			User::LeaveIfError(iRtpSocket.Open(aServer,iLocalAddr.Family(),KSockDatagram,KProtocolInetUdp,aConnection));
			__RTP_LOG(_L("\tUser::LeaveIfError(iRtpSocket.Bind(...))"));
			User::LeaveIfError(iRtpSocket.Bind(iLocalAddr));
			__RTP_LOG2(_L("\tConnecting To Address %S, Port %u"), &buf, iRemoteAddr.Port());
			iRtpSocket.Connect(iRemoteAddr, iStatus);
			SetActive();
			break;
		case ERtpCOIncoming:
			{
			__RTP_LOG(_L("\tUser::LeaveIfError(KErrNotSupported)"));
			User::Leave(KErrNotSupported);
			}
		case ERtpCL:
			{
            __RTP_LOG(_L("\tUser::LeaveIfError(iRtpSocket.Open(...))"));
            User::LeaveIfError(iRtpSocket.Open(aServer,KAfInet,KSockDatagram,KProtocolInetUdp,aConnection));
			__RTP_LOG(_L("\tUser::LeaveIfError(iRtpSocket.Bind(...))"));
			User::LeaveIfError(iRtpSocket.Bind(iLocalAddr));
			break;
			}
		default:
			{
			__RTP_LOG(_L("\tUser::LeaveIfError(KErrNotSupported)"));
			User::Leave(KErrNotSupported);
			}
		}
	}


void CRtpUdpHandler::ActivateL(RSocketServ& aServer,TRtpConnectionType aConnectionType)
	{	
	__RTP_LOG(_L("CRtpUdpHandler::ActivateL() ... "));
	__RTP_LOG1(_L("\t[ConnetionType] == %d"), static_cast<TInt>(aConnectionType));
	// connection type is stored in RtpController
	__RTP_LOG_STMT(TBuf<50> buf; iRemoteAddr.Output(buf););
	switch (aConnectionType)
		{
		case ERtpConnected:	
			// connected socket provided and owned by client
			// we shouldn't really get here as SocketServ is NULL for conected
			Panic(ERtpHandlerWrongConnType);
		case ERtpCOOutgoing:
			__RTP_LOG(_L("\tUser::LeaveIfError(iRtpSocket.Open(...))"));
			User::LeaveIfError(iRtpSocket.Open(aServer,iLocalAddr.Family(),KSockDatagram,KProtocolInetUdp));
			__RTP_LOG(_L("\tUser::LeaveIfError(iRtpSocket.Bind(...))"));
			User::LeaveIfError(iRtpSocket.Bind(iLocalAddr));
			__RTP_LOG2(_L("\tConnecting To Address %S, Port %u"), &buf, iRemoteAddr.Port());
			iRtpSocket.Connect(iRemoteAddr, iStatus);
			SetActive();
			break;
		case ERtpCOIncoming:
			{
			__RTP_LOG(_L("\tUser::LeaveIfError(KErrNotSupported)"));
			User::Leave(KErrNotSupported);
			}
		case ERtpCL:
		case ERtpCF:
			{
			__RTP_LOG(_L("\tUser::LeaveIfError(iRtpSocket.Open(...))"));
			User::LeaveIfError(iRtpSocket.Open(aServer,iLocalAddr.Family(),KSockDatagram,KProtocolInetUdp));
			__RTP_LOG(_L("\tUser::LeaveIfError(iRtpSocket.Bind(...))"));
			User::LeaveIfError(iRtpSocket.Bind(iLocalAddr));
			break;
			}
		default:
			{
			__RTP_LOG(_L("\tUser::LeaveIfError(KErrNotSupported)"));
			User::Leave(KErrNotSupported);
			}
		}
	}

/**
Compare two addreesses of KAfInet family type

@internalComponent
@return 
@param 
@param 
@pre 
@post 
*/
	
TBool CRtpUdpHandler::CompareAddresses(const TSockAddr& aAddr1,const TSockAddr& aAddr2)
	{
	if(aAddr1.Family() != aAddr2.Family())
		{
		return EFalse;
		}
	
	
	TInetAddr inetAddr1(aAddr1);
	TInetAddr inetAddr2(aAddr2);
	//Tests the protocol family, IP address, and port values
	return inetAddr1.CmpAddr(inetAddr2);
	}

CRtpUdpHandler::CRtpUdpHandler(TInt aPriority) 
: CRtpHandlerBase(aPriority)
	{
	}

CRtpUdpHandler::CRtpUdpHandler(RSocket& aSocket, TInt aPriority) 
: CRtpHandlerBase(aSocket, aPriority)
	{
	// the socket must be already bind/connected in this case!!
	aSocket.RemoteName(iRemoteAddr);
	}

CRtpUdpHandler::~CRtpUdpHandler() 
	{
	
	}
