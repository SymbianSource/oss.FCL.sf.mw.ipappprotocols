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
// class for handling rtp data flow
// 
//

#include <e32base.h>
#include <in_sock.h>
#include <bt_sock.h>

#include "rtputils.h"
#include "rtpsock.h"
#include "rtpcontroller.h"

#include "rtpudphandler.h"

// BT can be made configurable,  see rtpbthandler.h for more details...
#include "rtpbthandler.h"
#include "rtpapi.h"

CRtpHandlerBase* CRtpHandlerBase::NewBearerL(CRtpControllerBase* aController,
											 RSocket& aSocket, TInt aPriority)
	{	
	__RTP_LOG(_L("CRtpHandlerBase::NewBearerL() ... "));
	__ASSERT_DEBUG(aController, Panic(ERtpControllerIsNotCreated));
	CRtpHandlerBase* handler = NULL;
	TSockAddr localAddr;
	aSocket.LocalName(localAddr);
	switch (localAddr.Family())
		{
		// BT can be made configurable,  see rtpbthandler.h for more details...
		case KBTAddrFamily:
			{
			handler = CRtpBluetoothHandler::NewL(aSocket, aPriority);
			handler->iRtpController = (CRtpControllerBase *)aController;
			break;
			}
		case KAfInet:
		default:
			{
			handler = CRtpUdpHandler::NewL(aSocket, aPriority);		
			handler->iRtpController = (CRtpControllerBase *)aController;
			break;
			}
		
		}
	handler->SetLocalAddress(localAddr);
	return handler;
	}

CRtpHandlerBase* CRtpHandlerBase::NewBearerL(CRtpControllerBase* aController, /*RSocketServ& aServer,*/
											 TSockAddr aLocalAddr, TInt aPriority)
	{

	__RTP_LOG1(_L("CRtpHandlerBase::NewBearerL() ... at port %u"), aLocalAddr.Port());
	__ASSERT_DEBUG(aController, Panic(ERtpControllerIsNotCreated));
	CRtpHandlerBase* handler = NULL;
	switch (aLocalAddr.Family())
		{
		// BT can be made configurable,  see rtpbthandler.h for more details...
		case KBTAddrFamily:
			{
			handler = CRtpBluetoothHandler::NewL(aPriority);
			handler->iRtpController = aController;
			break;
			}
		case KAfInet:
		default:
			{
			handler = CRtpUdpHandler::NewL(aPriority);
			handler->iRtpController =(CRtpControllerBase *) aController;
			break;
			}
		}
	handler->SetLocalAddress(aLocalAddr);
	return handler;
	}


EXPORT_C CRtpHandlerBase::CRtpHandlerBase(TInt aPriority) 
	: CActive(aPriority), iRecvOn(ETrue), iClientDataPtr(NULL)
	{
	__RTP_LOG(_L("CRtpHandlerBase::CRtpHandlerBase() ..."));
	CActiveScheduler::Add(this);
	// default type
	iRtpSocket.iType = ERtp;
	}

EXPORT_C CRtpHandlerBase::CRtpHandlerBase(RSocket& aSocket, TInt aPriority) 
: CActive(aPriority), iRecvOn(ETrue), iRtpSocket(aSocket), iClientDataPtr(NULL)
	{
	__RTP_LOG1(_L("CRtpHandlerBase [0x%x]::CRtpHandlerBase() ... "), this);
	CActiveScheduler::Add(this);
	// default type
	iRtpSocket.iType = ERtp;
	}
 
EXPORT_C CRtpHandlerBase::~CRtpHandlerBase()
	{
	__RTP_LOG1(_L("CRtpHandlerBase::~CRtpHandlerBase() 0x%x"), this);
	Cancel();
	if (iRtpSocket.iOwnSocket)
		iRtpSocket.Close();
	}

EXPORT_C void CRtpHandlerBase::DoCancel()
	{
	__RTP_LOG(_L("CRtpHandlerBase::DoCancel() .. Cancelling read "));
	iRtpSocket.CancelRecv();
	}

EXPORT_C void CRtpHandlerBase::RunL()
	{
	__RTP_LOG1(_L("CRtpHandlerBase [0x%x]::RunL() ... "), this);
	__ASSERT_DEBUG(iRtpController,Panic(ERtpControllerIsNotCreated));
	// When the read completes, the data received should get copied to this buffer
	
	iRtpController->DoReceiveL(*iClientDataPtr,iRtpSocket.iType);
	}

EXPORT_C TInt CRtpHandlerBase::RunError(TInt aError)
	{	
	Cancel();
	CompleteRead(aError);
	return KErrNone;
	}

EXPORT_C void CRtpHandlerBase::CompleteRead(const TInt aError)
	{	
	__RTP_LOG1(_L("CRtpHandleBase [0x%x]::CompleteRead() ... "), this);
	__RTP_LOG1(_L("\t[Error] == %d"), aError);
	__RTP_LOG_STMT(if (iClientDataPtr))
    __RTP_LOG2(_L("\taClientBuffer at 0x%x, of Length %d"), iClientDataPtr->Ptr(), iClientDataPtr->Length());
	
	/* If there is no Data observer it means App is not interested in data */
	if(iDataObserver)
	    {
	    iDataObserver->DataAvailable(aError);
	    }
	}

EXPORT_C void CRtpHandlerBase::SaveReadStatus(TDes8& aClientBuf)
	{	
	__RTP_LOG1(_L("CRtpHandlerBase [0x%x]::SaveClientStatus"), this);
	__RTP_LOG2(_L("\taClientBuffer at 0x%x, of Length %d"), aClientBuf.Ptr(), aClientBuf.Length());
	ASSERT(iDataObserver); //With out DataObserver the app shouldnot attempt a read
	iClientDataPtr = &aClientBuf;
	}

EXPORT_C void CRtpHandlerBase::SetDataObserver(MRtpDataObserver *aDataObserver)
    {
    iDataObserver = aDataObserver;
    }

void CRtpHandlerBase::DoReceiveTag(TBool aTag)
	{	
	iRecvOn = aTag;
	}

void CRtpHandlerBase::CancelSend()
	{
	iRtpSocket.CancelSend();
	}

void CRtpHandlerBase::CancelRecv()
	{
	iRtpSocket.CancelRecv();
	}
