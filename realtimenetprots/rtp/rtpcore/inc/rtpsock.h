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
// Header for class specific to rtp sock
// 
//

/**
 @file
 @internalComponent
*/
 
#if !defined(__RTP_SOCK_H__)
#define __RTP_SOCK_H__

#include <e32std.h>
#include <e32base.h>
#include <es_sock.h>


enum TRtpType
	{
	/** It is a RTP socket */
	ERtp,
	/** It is a RTCP socket */
	ERtcp,
	ECFRtp,
	ECFRtcp,
	/** It is a FEC socket */
	ERtpFec
	};

class CRtpHandlerBase;

/**
@internalTechnology 
*/
NONSHARABLE_CLASS(RRtpSocket) : public RSocket
	{
private:
	friend class CRtpHandlerBase;
public:
	 RRtpSocket();
	 RRtpSocket(RSocket& aSocket);
	 TInt Open(RSocketServ& aServer,TUint addrFamily,TUint sockType,TUint protocol,RConnection& aConnection);
	 TInt Open(RSocketServ &aServer,TUint addrFamily,TUint sockType,TUint protocol,RSubConnection& aSubConnection);
	 TInt Open(RSocketServ& aServer,TUint addrFamily,TUint sockType,TUint protocol);
	 
	 
	inline void CancelSend();
	void Write(TDes8& aDesc,TRequestStatus& aStatus);
	void SendTo(TDes8& aDesc,TSockAddr& aAddr,TUint flags,TRequestStatus& aStatus);
	inline void RecvFrom(TDes8& aDesc,TSockAddr& aAddr,TUint flags,TRequestStatus& aStatus);
	inline TInt Bind(TSockAddr& aAddr);
	void Close();
	void Shutdown(RSocket::TShutdown aHow,const TDesC8& aDisconnectDataOut,TDes8& aDisconnectDataIn,TRequestStatus& aStatus);
	inline RRtpSocket& operator=(RSocket& aSocket);
	inline TBool HasOwnSocket();

private:

	void SetHandler(CRtpHandlerBase* aHandler);
	
private:
	//Pls NOte ERtpFec is not supported yet
    TRtpType iType;
	TBool    iOwnSocket;
	TBool    iOpenSocket;
	};


#include "rtpsock.inl"

#endif	//__RTP_SOCK_H__

