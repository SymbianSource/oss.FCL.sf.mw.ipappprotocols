/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : TransactionReceiver.h
* Part of       : Transaction
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef TRANSACTIONRECEIVER_H
#define TRANSACTIONRECEIVER_H

// INCLUDES
#include <in_sock.h>

#include "CSipConnectionMgr.h"
#include "MReceiverObserver.h"


// CLASS DECLARATION
class MTransactionReceiver : public MReceiverObserver
	{
public: // New functions

	/**
	 * An ICMP error was detected and is passed for handling.	 
	 *
	 * @param aAddress Remote address on which the error occurred
	 * @param aError Error type
	 */
	virtual void IcmpErrorL(const TInetAddr& aAddress,
						    CSipConnectionMgr::TICMPError aError) = 0;
	};

#endif // end of TRANSACTIONRECEIVER_H

// End of File
