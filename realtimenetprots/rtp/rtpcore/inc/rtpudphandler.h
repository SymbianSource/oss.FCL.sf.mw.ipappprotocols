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
// Header for class specific to udp handling
// 
//

/**
 @file
 @internalComponent
*/

#if !defined(RTP_UDP_HANDLER_H__)
#define RTP_UDP_HANDLER_H__

#include "rtpsock.h"
#include <es_sock.h>
#include "in_sock.h"

// forward declarations

NONSHARABLE_CLASS(CRtpUdpHandler) : public CRtpHandlerBase
	{
public:
	static CRtpUdpHandler* NewL(TInt aPriority = EPriorityNormal);
	static CRtpUdpHandler* NewL(RSocket& aSocket, TInt aPriority = EPriorityNormal);
	virtual ~CRtpUdpHandler();

	// virtual functions from CRtpHandlerBase
	virtual void Send(TDes8& aBuffer, TRequestStatus& aStatus);
	virtual void Read(TDes8& aBuffer);
	void Cancel();
	virtual void ActivateL(RSocketServ& aServer,TRtpConnectionType aConnectionType,RConnection& aConnection);
	// added on 21/04
	virtual void ActivateL(RSocketServ& aServer,TRtpConnectionType aConnectionType,RSubConnection& aSubConnection);
	// added on 24/03
	virtual void ActivateL(RSocketServ& aServer,TRtpConnectionType aConnectionType);
	virtual TBool CompareAddresses(const TSockAddr& aAddr1,const TSockAddr& aAddr2);

	virtual inline void SetLocalAddress(const TSockAddr& aAddr);
	virtual inline void SetRemoteAddress(const TSockAddr& aAddr);
	virtual inline TSockAddr& RemoteAddress();
	virtual inline TSockAddr& LocalAddress();
	virtual inline TSockAddr& RecvAddress();
	virtual void RunL();
	
	virtual inline void SetDefaultRemoteAddress(const TSockAddr& aAddr);
	virtual inline TSockAddr& DefaultRemoteAddress();
	
	

private:
	CRtpUdpHandler();
	CRtpUdpHandler(TInt aPriority);
	CRtpUdpHandler(RSocket& aSocket,TInt aPriority);
	// from CHandlerBase
	virtual void ReadMore();
private:
	TInetAddr iLocalAddr;
	// The remote addres is Overloaded
	// The same variable is used to send rtcp packet to various sources
	TInetAddr iRemoteAddr;
	TInetAddr iRecvAddr;
	TInetAddr iDefaultRemoteAddr;
	};


inline void CRtpUdpHandler::SetLocalAddress(const TSockAddr& aAddr)
	{
	iLocalAddr = TInetAddr(aAddr);
	}

inline void CRtpUdpHandler::SetRemoteAddress(const TSockAddr& aAddr)
	{
	iRemoteAddr = TInetAddr(aAddr);
	}

inline TSockAddr& CRtpUdpHandler::RemoteAddress()
	{
	return iRemoteAddr;
	}

inline TSockAddr& CRtpUdpHandler::LocalAddress()
	{
	return iLocalAddr;
	}

inline TSockAddr& CRtpUdpHandler::RecvAddress()
	{
	return iRecvAddr;
	}
inline void CRtpUdpHandler::SetDefaultRemoteAddress(const TSockAddr& aAddr)
	{
	iDefaultRemoteAddr = TInetAddr(aAddr);
	}

inline TSockAddr& CRtpUdpHandler::DefaultRemoteAddress()
	{
	return iDefaultRemoteAddr;
	}
	
#endif

