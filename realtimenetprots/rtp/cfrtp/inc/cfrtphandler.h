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
// CCFRtp Handler Declarations
// 
//


/**
 @file
 @internalComponent
*/

#if !defined(__CFRTP_HANDLER_H__)
#define __CFRTP_HANDLER_H__

#include <in_sock.h>
#include "rtphandlerbase.h"

// forward declarations

class MRtpDataSender
	{
	public:
	virtual void SendData(TDes8& aBuffer,TSockAddr& aRemoteAddr, TInt aFlags, TRequestStatus& aStatus)=0;
	};


NONSHARABLE_CLASS(CCFRtpHandler) : public CRtpHandlerBase
	{
public:
	static CCFRtpHandler* NewL(MRtpDataSender *aDataSender,	TInt aPriority = EPriorityNormal);

	virtual ~CCFRtpHandler();

	// virtual functions from CRtpHandlerBase
	virtual void Send(TDes8& aBuffer, TRequestStatus& aStatus);
	virtual void Read(TDes8& aBuffer);
	
	void Cancel();
	
	virtual void ActivateL(RSocketServ& aServer,TRtpConnectionType aConnectionType,RConnection& aConnection);
	virtual void ActivateL(RSocketServ& aServer,TRtpConnectionType aConnectionType,RSubConnection& aSubConnection);
	virtual void ActivateL(RSocketServ& aServer,TRtpConnectionType aConnectionType);
	
	virtual TBool CompareAddresses(const TSockAddr& aAddr1,const TSockAddr& aAddr2);

	virtual inline void SetLocalAddress(const TSockAddr& aAddr);
	virtual inline void SetRemoteAddress(const TSockAddr& aAddr);
	virtual inline TSockAddr& RemoteAddress();
	virtual inline TSockAddr& LocalAddress();
	virtual inline TSockAddr& RecvAddress();

	 // from CActive
	 virtual void RunL();
	 virtual TInt RunError(TInt aError);
	
	
	virtual inline void SetDefaultRemoteAddress(const TSockAddr& aAddr);
	virtual inline TSockAddr& DefaultRemoteAddress();
	virtual inline void SetRecvAddress(const TSockAddr &aAddr);
	
	void CancelSend();
	void CancelRecv();

private:
	CCFRtpHandler();
	CCFRtpHandler(TInt aPriority);
	
	// from CHandlerBase
	virtual void ReadMore();
private:
	TInetAddr iLocalAddr;
	// The remote addres is Overloaded
	// The same variable is used to send rtcp packet to various sources
	TInetAddr iRemoteAddr;
	TInetAddr iRecvAddr;
	TInetAddr iDefaultRemoteAddr;
	MRtpDataSender *iDataSender;
	};


inline void CCFRtpHandler::SetLocalAddress(const TSockAddr& aAddr)
	{
	iLocalAddr = TInetAddr(aAddr);
	}

inline void CCFRtpHandler::SetRemoteAddress(const TSockAddr& aAddr)
	{
	iRemoteAddr = TInetAddr(aAddr);
	}

inline TSockAddr& CCFRtpHandler::RemoteAddress()
	{
	return iRemoteAddr;
	}

inline TSockAddr& CCFRtpHandler::LocalAddress()
	{
	return iLocalAddr;
	}

inline void CCFRtpHandler::SetRecvAddress(const TSockAddr &aAddr)
	{
	iRecvAddr = aAddr;
	}

inline TSockAddr& CCFRtpHandler::RecvAddress()
	{
	return iRecvAddr;
	}
inline void CCFRtpHandler::SetDefaultRemoteAddress(const TSockAddr& aAddr)
	{
	iDefaultRemoteAddr = TInetAddr(aAddr);
	}

inline TSockAddr& CCFRtpHandler::DefaultRemoteAddress()
	{
	return iDefaultRemoteAddr;
	}
	
#endif //__CFRTP_HANDLER_H__

