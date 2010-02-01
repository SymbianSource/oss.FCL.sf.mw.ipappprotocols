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
// RTP sock inline definitions
// 
//

/**
 @file
 @internalComponent
*/


inline TBool RRtpSocket::HasOwnSocket()
	{
	return iOwnSocket;
	}
/**
@internalTechnology 
*/
inline RRtpSocket& RRtpSocket::operator=(RSocket& aSocket) 
	{
	RSubSessionBase::operator=(aSocket);
	return *this;
	};
/** Cancel a pending send
@internalTechnology 
*/
inline void RRtpSocket::CancelSend()
	{
	RSocket::CancelSend();
	}




/** Receive data on a (possibly) unconnected RTP socket

@internalTechnology 
@param aBuffer A descriptor where data received will be placed.
@param aAddr An address where to receive from
@param someFlags Flags for protocol specific information.
@param aStatus On completion, will contain an error code.
@pre An opened socket
@post One datagram has arrived - even if it is not sufficient to fill the buffer.
@see RSocket::RecvFrom
*/
inline void RRtpSocket::RecvFrom(TDes8& aBuffer,TSockAddr& aAddr,TUint someFlags,
								 TRequestStatus& aStatus)
	{
	// The RTP socket doesn't do any RTP related checks
	// as it's all done in RTP hadler
	RSocket::RecvFrom(aBuffer,aAddr,someFlags,aStatus);
	}


/** Set the local address of a socket. 

@internalTechnology 
@return KErrNone if successful, otherwise another of the system-wide error codes.
@param aAddr Desired local address of socket.
@pre Opened socket
@post 
@see RSocket::Bind
*/		
inline TInt RRtpSocket::Bind(TSockAddr& aAddr)
	{
	return RSocket::Bind(aAddr);
	}

