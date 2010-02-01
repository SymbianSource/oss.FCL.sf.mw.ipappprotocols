/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : MServerTaFactory.h
* Part of       : TransactionUser
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/


#ifndef MSERVERTAFACTORY_H
#define MSERVERTAFACTORY_H

#include "SipStackServerDefs.h"

class CSIPRequest;
class MReceiverObserver;
class TSIPTransportParams;


class MServerTaFactory
	{
public:
	virtual ~MServerTaFactory() {}

	/**
	 * Creates resources for a new transaction to handle a SIP request that
	 * was received from the network.
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @param aRequest IN: SIP request for which a new transaction should be
	 *	created. MServerTaFactory inspects the request to determine the type
	 *  of the transaction that will be created. Ownership is not transferred.
     * @param aParams The transport params including the IAP ID.
	 * @param aTransactionId OUT: TransactionId identifying the new transaction.
	 * @return value Pointer to the receive interface of the new transaction.
	 *	Caller should use MReceiverObserver::ReceiveL() after creating the
	 *  transaction. Ownership is not transferred.
	 *  If NULL, then the aRequest should be dropped.
	 */
	virtual MReceiverObserver* NewTransactionL(
	                                CSIPRequest& aRequest,
                                    const TSIPTransportParams& aParams,
		                            TTransactionId& aTransactionId) = 0;
	};

#endif // MSERVERTAFACTORY_H

// End of File
