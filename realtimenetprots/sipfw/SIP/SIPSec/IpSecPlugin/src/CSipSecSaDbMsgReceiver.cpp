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
// Name          : CSipSecSaDbMsgReceiver.cpp
// Part of       : SIPSec
// Version       : SIP/4.0 
//



#include <pfkey_send.h>
#include "CSipSecSaDbMsgReceiver.h"
#include "MSipSecSaDbSendReceiveObserver.h"

// ----------------------------------------------------------------------------
// CSipSecSadbMsgReceiver::NewL
// ----------------------------------------------------------------------------
//
CSipSecSaDbMsgReceiver* CSipSecSaDbMsgReceiver::NewL(
	MSipSecSaDbSendReceiveObserver& aObserver, 
	RSADB& aSadb )
	{
    CSipSecSaDbMsgReceiver*	self = NewLC( aObserver, aSadb );
	CleanupStack::Pop( self );
	return self;
	}
	
// ----------------------------------------------------------------------------
// CSipSecSadbMsgReceiver::NewLC
// ----------------------------------------------------------------------------
//
CSipSecSaDbMsgReceiver* CSipSecSaDbMsgReceiver::NewLC(
	MSipSecSaDbSendReceiveObserver& aObserver, 
	RSADB& aSadb )
	{
	CSipSecSaDbMsgReceiver* self =
	    new (ELeave) CSipSecSaDbMsgReceiver( aObserver, aSadb );
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CSipSecSadbMsgReceiver::CSipSecSaDbMsgReceiver
// ----------------------------------------------------------------------------
//	
CSipSecSaDbMsgReceiver::CSipSecSaDbMsgReceiver( 
	MSipSecSaDbSendReceiveObserver& aObserver, 
	RSADB& aSadb ) :
	CSipSecSaDbMsgTransport( aObserver, aSadb )
	{
	}

// ----------------------------------------------------------------------------
// CSipSecSadbMsgReceiver::~CSipSecSaDbMsgReceiver
// ----------------------------------------------------------------------------
//	
CSipSecSaDbMsgReceiver::~CSipSecSaDbMsgReceiver()
	{
	Cancel();
	}

// ----------------------------------------------------------------------------
// CSipSecSadbMsgReceiver::Receive
// ----------------------------------------------------------------------------
//
void CSipSecSaDbMsgReceiver::Receive( TDes8& aMsg )
	{
	if ( !IsActive() )
		{
		iSaDb.ReadRequest( aMsg, iStatus );
		SetActive();
		}
	}

// ----------------------------------------------------------------------------
// CSipSecSadbMsgReceiver::RunL
// ----------------------------------------------------------------------------
//
void CSipSecSaDbMsgReceiver::RunL()
	{
	iObserver.MessageReceived( iStatus.Int() );
	}

// ----------------------------------------------------------------------------
// CSipSecSadbMsgReceiver::DoCancel
// ----------------------------------------------------------------------------
//    	
void CSipSecSaDbMsgReceiver::DoCancel()
	{
	iSaDb.CancelRecv();
	}
	
// End of File
