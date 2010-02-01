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
// class for handling rtp over bluetooth
// 
//

/**
 @file
 @internalComponent
*/
 
#include <e32base.h>
#include "rtpcontroller.h"
#include "rtpbthandler.h"
#include "rtputils.h"
#include "bt_sock.h"



/*
* BT Handler
*/

CRtpBluetoothHandler* CRtpBluetoothHandler::NewL(TInt /*aPriority*/)
	{	
	__RTP_LOG(_L("CRtpBluetoothHandler::NewL() ... "));
	__RTP_LOG(_L("\tUser::Leave(KErrNotSupported)"));
	User::Leave(KErrNotSupported);
	return NULL;
	}
	
CRtpBluetoothHandler* CRtpBluetoothHandler::NewL(RSocket& aSocket, TInt aPriority)
	{
	__RTP_LOG(_L("CRtpBluetoothHandler::NewL() ... "));
	// the socket must be already connected in this case!!
	CRtpBluetoothHandler* self = new(ELeave) CRtpBluetoothHandler(aSocket,aPriority);
	return self;
	}

void CRtpBluetoothHandler::Send(TDes8& aPacket, TRequestStatus& aStatus)
	{	
	__RTP_LOG(_L("CRtpBluetoothHandler::Send() ... "));
	__RTP_LOG1(_L("\t[State] == %d"), static_cast<TInt>(iState));
	if (iState==EConnected)
		{
		// update the stream statemachine first
		__ASSERT_DEBUG(iRtpController, Panic(ERtpControllerIsNotCreated));
		iRtpController->PacketBeingSent(aPacket, Type());
		iRtpSocket.Write(aPacket, aStatus);
		}
	else
		{
		TRequestStatus* rS=&aStatus;
		User::RequestComplete(rS,KErrNotReady);
		}	
	}

void CRtpBluetoothHandler::Read(TDes8& aBuffer)
	{	
	__RTP_LOG(_L("CRtpBluetoothHandler::Read() ... "));
	__RTP_LOG1(_L("\t[State] == %d"), static_cast<TInt>(iState));
    if( !iRecvOn )
        {
        /* Just return here. The Callback will never be triggered anyway
         * because we are not initiating a Read here.
         */
        return;
        }

	if (iState==EConnected)
		{
		if (!IsActive())
			{
			SaveReadStatus(aBuffer);
			__ASSERT_ALWAYS(iClientDataPtr->MaxLength() > 0,Panic(ERtpReadMoreCalledWithoutData));
			iRtpSocket.Read(*iClientDataPtr, iStatus);
			SetActive();
			}
		else
			{
			Panic(ERtpIsAlreadyActive);
			}
		}
	else
		{
		/* Not ready for accepting data yet */
		iDataObserver->DataAvailable(KErrNotReady);
		}
	}


void CRtpBluetoothHandler::ReadMore()
	{
	__RTP_LOG(_L("CRtpBluetoothHandler::ReadMore() ... "));
	__RTP_LOG1(_L("\t[State] == %d"), static_cast<TInt>(EConnected));
	__ASSERT_ALWAYS(iClientDataPtr->MaxLength() > 0,Panic(ERtpReadMoreCalledWithoutData));
	
	Read(*iClientDataPtr);
	}

void CRtpBluetoothHandler::ActivateL(RSocketServ& /*aServer*/, TRtpConnectionType /*aConnectionType*/)
	{
	__RTP_LOG(_L("CRtpBluetoothHandler::ActivateL() ... "));
	// how do we know what role we are - connector/acceptor?
	if (iState == EConnected)
		return;
	//BT always uses (pre)connected sockets
	__RTP_LOG(_L("\tUser::Leave(KErrNotSupported)"));
	User::Leave(KErrNotSupported);
	}

void CRtpBluetoothHandler::ActivateL(RSocketServ& /*aServer*/, TRtpConnectionType /*aConnectionType*/, RConnection& /*aConnection*/)
	{
	__RTP_LOG(_L("CRtpBluetoothHandler::ActivateL() ... "));
	__RTP_LOG(_L("\tUser::Leave(KErrNotSupported"));
	//BT always uses (pre)connected sockets
	User::Leave(KErrNotSupported);
	}

void CRtpBluetoothHandler::ActivateL(RSocketServ&/* aServer*/,TRtpConnectionType ,RSubConnection& /*aConnection*/)
	{
	__RTP_LOG(_L("CRtpBluetoothHandler::ActivateL() ... "));
	__RTP_LOG(_L("\tUser::Leave(KErrNotSupported"));
	//BT always uses (pre)connected sockets
	User::Leave(KErrNotSupported);	
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

TBool CRtpBluetoothHandler::CompareAddresses(const TSockAddr& aAddr1,const TSockAddr& aAddr2)
	{
	__RTP_LOG(_L("CRtpBluetoothHandler::CompareAddresses() ... "));
	if(aAddr1.Family() != aAddr2.Family())
		{
		__RTP_LOG(_L("\t!!! Addresses do not belong to same family !!!"));
		return EFalse; // is this OK enough? Should we rather panic?
		}
	
	TBTSockAddr btAddr1(aAddr1);
	TBTSockAddr btAddr2(aAddr2);
	
	return btAddr1.BTAddr()==btAddr2.BTAddr();
	}


CRtpBluetoothHandler::CRtpBluetoothHandler(TInt aPriority) : 
	CRtpHandlerBase(aPriority)
	{
	}

CRtpBluetoothHandler::CRtpBluetoothHandler(RSocket& aSocket, TInt aPriority) 
: CRtpHandlerBase(aSocket,aPriority),iState(EConnected)
	{
	// the socket must be already connected in this case!!
	}

void CRtpBluetoothHandler::RunL()
	{
	__RTP_LOG(_L("CRtpBluetoothHandler::RunL() ... "));
	__RTP_LOG1(_L("\t[State] == %d"), static_cast<TInt>(iState));
	switch (iState)
		{
		case EConnecting:
			if (iStatus.Int() == KErrNone)
				iState = EConnected;
			//break; fall through??
		case EConnected:
			CRtpHandlerBase::RunL();
			break;
		default:
			break;
		}
		
	}
