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
// CCFRtp Handler Implementation
// 
//


/**
 @file
 @internalComponent
*/

#include <e32base.h>
#include "cfrtphandler.h"

CCFRtpHandler* CCFRtpHandler::NewL(MRtpDataSender *aDataSender, TInt aPriority)
	{	
	CCFRtpHandler* self = new(ELeave) CCFRtpHandler(aPriority);
	self->iDataSender = aDataSender;
	return self;
	}

CCFRtpHandler::CCFRtpHandler(TInt aPriority) 
: CRtpHandlerBase(aPriority)
	{
	}

CCFRtpHandler::~CCFRtpHandler()
	{
	}

void CCFRtpHandler::Send(TDes8& aBuffer, TRequestStatus& aStatus)
	{	
	/* Pass it on to our Data sender. The "Normal" handlers will send this to 
	 * iRtpSocket */
	iDataSender->SendData(aBuffer, iRemoteAddr, 0, aStatus);
	}

// this could only be a genuine read from client
void CCFRtpHandler::Read(TDes8& )
	{	
    //Read?. This is not expected in the case of CF Rtp Handlers */
	ASSERT(0);
	}

void CCFRtpHandler::ReadMore()
	{
    //We are anyways reading always! 
	}

void CCFRtpHandler::RunL()
	{
	/* We should never have an Active Request pending */
	ASSERT(0);
	}

TInt CCFRtpHandler::RunError(TInt)
	{
	/* We cannot hit RunL() so cannot hit here as well */
	ASSERT(0);
	return KErrNone;
	}

void CCFRtpHandler::ActivateL(RSocketServ& ,TRtpConnectionType ,RSubConnection& )
	{	
	User::LeaveIfError(KErrNotSupported);
	}


void CCFRtpHandler::ActivateL(RSocketServ& ,TRtpConnectionType , RConnection& )
	{	
	User::LeaveIfError(KErrNotSupported);
	}



void CCFRtpHandler::ActivateL(RSocketServ& ,TRtpConnectionType )
	{	
	User::LeaveIfError(KErrNotSupported);
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
TBool CCFRtpHandler::CompareAddresses(const TSockAddr& aAddr1,const TSockAddr& aAddr2)
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

void CCFRtpHandler::CancelSend()
	{
	/* The request is already completed */
	return;
	}

void CCFRtpHandler::CancelRecv()
	{
	/* No read .. So no cancel for read */
	return;
	}
