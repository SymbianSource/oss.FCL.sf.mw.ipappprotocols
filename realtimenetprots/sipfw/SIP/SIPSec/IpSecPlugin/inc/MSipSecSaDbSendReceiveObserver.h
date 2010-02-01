/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name          : MSipSecSaDbSendReceiveObserver.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef MSIPSECSADBSENDRECEIVEOBSERVER_H
#define MSIPSECSADBSENDRECEIVEOBSERVER_H

class CSipSecSaDbMsgTransport;

/**
 * @brief Observer of callbacks for SADB send and receive functions
 * 
 * 
 */
class MSipSecSaDbSendReceiveObserver
	{
public:

	virtual void MessageSentL( TInt aStatus, TInt aMsgType ) = 0;

	virtual void MessageReceived( TInt aStatus ) = 0;

	};

#endif // MSIPSECSADBSENDRECEIVEOBSERVER_H


// End of File
