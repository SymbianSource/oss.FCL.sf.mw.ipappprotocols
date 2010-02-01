// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSipSecSaDbMsgSender.cpp
// Part of       : SIPSec
// Version       : SIP/4.0 
//



#include <pfkey_send.h>
#include "CSipSecSaDbMsgSender.h"
#include "MSipSecSaDbSendReceiveObserver.h"

// ----------------------------------------------------------------------------
// CSipSecSaDbMsgSender::NewL
// ----------------------------------------------------------------------------
//
CSipSecSaDbMsgSender* CSipSecSaDbMsgSender::NewL(
	MSipSecSaDbSendReceiveObserver& aObserver, 
	RSADB& aSadb )
	{
	CSipSecSaDbMsgSender* 
		self = CSipSecSaDbMsgSender::NewLC( aObserver, aSadb );
	CleanupStack::Pop( self );
	return self;
	}
	
// ----------------------------------------------------------------------------
// CSipSecSaDbMsgSender::NewLC
// ----------------------------------------------------------------------------
//
CSipSecSaDbMsgSender* CSipSecSaDbMsgSender::NewLC(
	MSipSecSaDbSendReceiveObserver& aObserver, 
	RSADB& aSadb )
	{
	CSipSecSaDbMsgSender* 
		self = new (ELeave) CSipSecSaDbMsgSender( aObserver, aSadb );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CSipSecSaDbMsgSender::CSipSecSaDbMsgSender
// ----------------------------------------------------------------------------
//
CSipSecSaDbMsgSender::CSipSecSaDbMsgSender( 
	MSipSecSaDbSendReceiveObserver& aObserver, 
	RSADB& aSadb ) :
	CSipSecSaDbMsgTransport( aObserver, aSadb )
	{
	}
	
// ----------------------------------------------------------------------------
// CSipSecSaDbMsgSender::~CSipSecSaDbMsgSender
// ----------------------------------------------------------------------------
//
CSipSecSaDbMsgSender::~CSipSecSaDbMsgSender()
	{
	Cancel();
	}

// ----------------------------------------------------------------------------
// CSipSecSaDbMsgSender::SendL
// ----------------------------------------------------------------------------
//
void CSipSecSaDbMsgSender::SendL( TPfkeySendMsgBase* aMessage, TInt aMsgType )
	{
	__ASSERT_ALWAYS( !IsActive(), User::Leave( KErrInUse ) );
	
	// Sender takes ownership of aMessage
	StoreMessageL( aMessage, aMsgType ); 
	iSaDb.FinalizeAndSend( *iMsg, iStatus );
	SetActive();
	}

// ----------------------------------------------------------------------------
// CSipSecSaDbMsgSender::RunL
// ----------------------------------------------------------------------------
//
void CSipSecSaDbMsgSender::RunL()
	{
    RemoveMessage();
	iObserver.MessageSentL( iStatus.Int(), iMsgType );
	}
	
// -----------------------------------------------------------------------------
// CSipSecSaDbMsgSender::RunError
// -----------------------------------------------------------------------------
//
TInt CSipSecSaDbMsgSender::RunError( TInt aError )
	{
	if ( aError != KErrNoMemory )
		{
		return KErrNone;
		}
	return aError;
	}

// ----------------------------------------------------------------------------
// CSipSecSaDbMsgSender::DoCancel
// ----------------------------------------------------------------------------
//
void CSipSecSaDbMsgSender::DoCancel()
	{
	iSaDb.CancelSend();
	RemoveMessage();
	}

// ----------------------------------------------------------------------------
// CSipSecSaDbMsgSender::StoreMessageL
// ----------------------------------------------------------------------------
//	
void CSipSecSaDbMsgSender::StoreMessageL( 
    TPfkeySendMsgBase* aMessage, 
    TInt aMsgType )
    {
    __ASSERT_ALWAYS( aMessage, User::Leave( KErrArgument ) );
    
    RemoveMessage();
    iMsg = aMessage;
    iMsgType = aMsgType;
    }
// ----------------------------------------------------------------------------
// CSipSecSaDbMsgSender::RemoveMessage
// ----------------------------------------------------------------------------
//	
void CSipSecSaDbMsgSender::RemoveMessage()
    {
    delete iMsg;
    iMsg = 0;
    }
	
// End of File
