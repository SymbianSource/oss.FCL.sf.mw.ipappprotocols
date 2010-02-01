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
// class for handling rtp sockets
// 
//

/**
 @file
 @internalComponent
*/

#include <es_ver.h>

#include "rtpsourceentry.h"
#include "rtpsock.h"
#include "rtputils.h"
#include "rtpcontroller.h"
#include "rtcppacket.h"
#include "rtppacket.h"


 RRtpSocket::RRtpSocket() : RSocket()
/**
Creates a RTP socket and generates an unique SSRC

@internalTechnology
@pre None
@post RTP socket is now created
*/
	{
	__RTP_LOG(_L("RRtpSocket::RRtpSocket() ... "));
	iOwnSocket = ETrue; // socket not provided thus we do own it	
	iOpenSocket = EFalse;
	}

/**
RTP socket created from provided RSocket

@internalTechnology
@pre None
@post RTP socket is created
*/
 RRtpSocket::RRtpSocket(RSocket& aSocket)
	{
	__RTP_LOG(_L("RRtpSocket::RRtpSocket() ... "));
	RSubSessionBase::operator=(aSocket);	
	//
	iOwnSocket = EFalse; // socket provide thus we do not own it	
	iOpenSocket = EFalse;
	}



// Added  on 24/03
// In case of Rtp Socket ....

 TInt RRtpSocket::Open(RSocketServ &aServer,TUint aAddrFamily,TUint aSockType,TUint aProtocol,RSubConnection& aSubConnection)
	{
	__RTP_LOG(_L("RRtpSocket::Open() ... "));
	__ASSERT_ALWAYS(iOwnSocket, Panic(ERtpSocketNotOwn));
	iOpenSocket = ETrue;
	return RSocket::Open(aServer,aAddrFamily,aSockType,aProtocol,aSubConnection);
	}

// Added on 24/03


/**
Opens a new RTP socket - calls a standard socket API

@internalTechnology 
@return KErrNone if successful, otherwise another of the system-wide error codes.
@param aServer The socket server session.
@param aAddrFamily A valid address constant for a protocol family.
@param aSockType A valid socket type for the protoco.
@param aProtocol A protocol constant which identifies a specific protocol. 
@pre None
@post RTP socket is now opened
*/
 TInt RRtpSocket::Open(RSocketServ &aServer,TUint aAddrFamily,TUint aSockType,TUint aProtocol,RConnection& aConnection)
	{
	__RTP_LOG(_L("RRtpSocket::Open() ... "));
	__ASSERT_ALWAYS(iOwnSocket, Panic(ERtpSocketNotOwn));
	iOpenSocket = ETrue;
	return RSocket::Open(aServer,aAddrFamily,aSockType,aProtocol,aConnection);
	}



/**
Opens a new RTP socket - calls a standard socket API

@internalTechnology 
@return KErrNone if successful, otherwise another of the system-wide error codes.
@param aServer The socket server session.
@param aAddrFamily A valid address constant for a protocol family.
@param aSockType A valid socket type for the protoco.
@param aProtocol A protocol constant which identifies a specific protocol. 
@pre None
@post RTP socket is now opened
*/
 TInt RRtpSocket::Open(RSocketServ &aServer,TUint aAddrFamily,TUint aSockType,TUint aProtocol)
	{
	__RTP_LOG(_L("RRtpSocket::Open() ... "));
	__ASSERT_ALWAYS(iOwnSocket, Panic(ERtpSocketNotOwn));
	iOpenSocket = ETrue;
	return RSocket::Open(aServer,aAddrFamily,aSockType,aProtocol);
	}



 void RRtpSocket::Close()
/**
Closes a RTP socket - blocking close

@internalTechnology 
@pre An opened socket
@post RTP socket is now closed
*/
	{
	__RTP_LOG(_L("RRtpSocket::Close() ... "));
	// iOpenSocket is set only in our own Open() thus we don't need to check iOwnSocket as well..
	if(iOpenSocket)
		RSocket::Close();

	iOpenSocket = EFalse;
	}



/**
Writes data to a connected RTP socket

@internalTechnology 
@param aBuffer A constant data descriptor
@param aStatus On completion, will contain an error code.
		See the system-wide error codes. 
@see RSocket::Write
@pre A connected socket
@post 
*/
void RRtpSocket::Write(TDes8& aBuffer,TRequestStatus& aStatus)
	{
	__RTP_LOG(_L("RRtpSocket::Write() ... "));
	RSocket::Write(aBuffer,aStatus);
	}

/**
Sends data to a (possibly) unconnected RTP socket

@internalTechnology 
@param aBuffer A data descriptor
@param aAddr Address where to send the data
@param someFlags Flags which are passed through to protocol
@param aStatus On completion, will contain an error code.
		See the system-wide error codes. 
@see RSocket::SendTo
@pre A opened socket
@post 
*/
 void RRtpSocket::SendTo(TDes8& aBuffer,TSockAddr& aAddr,TUint someFlags,
								 TRequestStatus& aStatus)
	{	
	__RTP_LOG(_L("RRtpSocket::SendTo() ... "));
	__RTP_LOG1(_L("\t[AddrPort] == %u"), aAddr.Port());
	__RTP_LOG1(_L("\t[AddrFamily] == %u"), aAddr.Family());
	__RTP_LOG1(_L("\t[Flags] == %u"), someFlags);
	RSocket::SendTo(aBuffer,aAddr,someFlags,aStatus);
	}
