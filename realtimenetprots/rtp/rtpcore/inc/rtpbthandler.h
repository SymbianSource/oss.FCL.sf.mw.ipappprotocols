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
// Header for class specific to rtp over bluetooth handling
// 
//

/**
 @file
 @internalComponent
*/
#if !defined(RTP_BT_HANDLER_H__)
#define RTP_BT_HANDLER_H__

#include "rtpsock.h"
#include "bt_sock.h"

NONSHARABLE_CLASS(CRtpBluetoothHandler) : public CRtpHandlerBase
	{
	// NOTE: configurability
	// CRtpBluetoothHandler is using BT socket and TBTSockAddr thus it links against bluetooth.lib.
	// We can't really have devices without bluetooth.lib so there isn't any apparent reason why to make it configurable now,
	// however it can be made configurable quite easily.
public:
	static CRtpBluetoothHandler* NewL(TInt aPriority = EPriorityNormal);
	static CRtpBluetoothHandler* NewL(RSocket& aSocket,TInt aPriority = EPriorityNormal);

	// virtual functions from CRtpHandlerBase
	virtual void Send(TDes8& aBuffer, TRequestStatus& aStatus);
	virtual void Read(TDes8& aBuffer);
	
	virtual void ReadMore();
	virtual void ActivateL(RSocketServ& aServer, TRtpConnectionType aConnectionType, RConnection& aConnection);
	virtual void ActivateL(RSocketServ& aServer, TRtpConnectionType aConnectionType, RSubConnection& aConnection);
	virtual void ActivateL(RSocketServ& aServer, TRtpConnectionType aConnectionType);
	
//	virtual void ActivateL(RSocketServ& aServer,RSubConnection& aConnection);
	virtual TBool CompareAddresses(const TSockAddr& aAddr1,const TSockAddr&aAddr2);

	virtual inline void SetLocalAddress(const TSockAddr& aAddr);
	virtual inline void SetRemoteAddress(const TSockAddr& aAddr);
	virtual inline TSockAddr& RemoteAddress();
	virtual inline TSockAddr& LocalAddress();
	virtual inline TSockAddr& RecvAddress();
	
	virtual inline void SetDefaultRemoteAddress(const TSockAddr& aAddr);
	virtual inline TSockAddr& DefaultRemoteAddress();
	
private:
	CRtpBluetoothHandler(TInt aPriority);
	CRtpBluetoothHandler(RSocket& aSocket, TInt aPriority);

	void Cancel();
	virtual void RunL();
	enum TBluetoothState
	  {
	  EConnecting,
	  EAccepting,
	  EConnected,
	  };
private:
	TBluetoothState	iState;
	TBTSockAddr iLocalAddr;
	TBTSockAddr iRemoteAddr;
	TBTSockAddr iDefaultRemoteAddr;
	};


inline void CRtpBluetoothHandler::SetLocalAddress(const TSockAddr& aAddr)
	{
	iLocalAddr = TBTSockAddr(aAddr);
	}

inline void CRtpBluetoothHandler::SetRemoteAddress(const TSockAddr& aAddr)
	{
	iRemoteAddr = TBTSockAddr(aAddr);
	}
	
inline TSockAddr& CRtpBluetoothHandler::RemoteAddress()
	{
	return iRemoteAddr;
	}

inline TSockAddr& CRtpBluetoothHandler::LocalAddress()
	{
	return iLocalAddr;
	}

inline TSockAddr& CRtpBluetoothHandler::RecvAddress()
	{
	return iRemoteAddr;
	}

inline void CRtpBluetoothHandler::SetDefaultRemoteAddress(const TSockAddr& aAddr)
	{
	iDefaultRemoteAddr = TBTSockAddr(aAddr);
	}

inline TSockAddr& CRtpBluetoothHandler::DefaultRemoteAddress()
	{
	return iDefaultRemoteAddr;
	}
		
#endif
