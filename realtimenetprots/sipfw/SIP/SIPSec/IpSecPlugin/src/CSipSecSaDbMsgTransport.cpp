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
// Name          : CSipSecSaDbMsgTransport.cpp
// Part of       : SIPSec
// Version       : SIP/4.0 
//



#include <pfkey_send.h>
#include "CSipSecSaDbMsgTransport.h"

// ----------------------------------------------------------------------------
// CSipSecSaDbMsgTransport::CSipSecSaDbMsgTransport
// ----------------------------------------------------------------------------
//
CSipSecSaDbMsgTransport::CSipSecSaDbMsgTransport( 
	MSipSecSaDbSendReceiveObserver& aObserver, 
	RSADB& aSadb,
	TInt aMsgType ) : CActive( EPriorityStandard ),
	iObserver( aObserver ), 
	iSaDb( aSadb ),
	iMsgType( aMsgType )
	{
	}

// ----------------------------------------------------------------------------
// CSipSecSaDbMsgTransport::~CSipSecSaDbMsgTransport
// ----------------------------------------------------------------------------
//	
CSipSecSaDbMsgTransport::~CSipSecSaDbMsgTransport()
	{}
	
// ----------------------------------------------------------------------------
// CSipSecSaDbMsgTransport::ConstructL
// ----------------------------------------------------------------------------
//
void CSipSecSaDbMsgTransport::ConstructL()
	{
	CActiveScheduler::Add( this );	
	}
	
// ----------------------------------------------------------------------------
// CSipSecSaDbMsgTransport::Type
// ----------------------------------------------------------------------------
//
TInt CSipSecSaDbMsgTransport::Type()
	{
	return iMsgType;
	}

// End of File
