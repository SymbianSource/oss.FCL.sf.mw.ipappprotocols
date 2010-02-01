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
* Name          : MReceiverObserver.h
* Part of       : Transaction
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/


#ifndef MRECEIVEROBSERVER_H
#define MRECEIVEROBSERVER_H

class CSIPRequest;
class CSIPResponse;
class TSIPTransportParams;

class MReceiverObserver
	{
public:
	virtual ~MReceiverObserver() {}


    /**
	 * Passes a SIP request received from network to a transaction for
	 * processing. If the function leaves, the ownership of the request is NOT
     * transferred.
	 *
	 * @pre aRequest != NULL
	 *
	 * @param aRequest IN: a SIP request, ownership is transferred
	 */
	virtual void ReceiveL (CSIPRequest* aRequest) = 0;


	/**
	 * Passes a SIP response received from network to a transaction for
	 * processing. If the function leaves, the ownership of the response is NOT
     * transferred.
	 *
	 * @pre aResponse != NULL
	 *
	 * @param aResponse IN: a SIP response, ownership is transferred     
	 */
	virtual void ReceiveL (CSIPResponse* aResponse) = 0;


	/**
	 * Informs the implementation of MReceiverObserver that a leave has
	 * occurred when the user called MReceiverObserver::ReceiveL. This way
	 * TransactionUser knows to remove the transaction in question.
	 *
	 * @pre   aReason != KErrNone
	 * @post
	 *
	 * @see
	 *
	 * @param aReason Leave reason
	 */
	virtual void LeaveOccurred (TInt aReason) = 0;
	
	
	/**
	 * Requests the transport parameters used by the 
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @return TSIPTransportParams The used transport parameters
	 */
	virtual const TSIPTransportParams& TransportParams () const = 0;
	};

#endif // end of MRECEIVEROBSERVER_H

// End of File
