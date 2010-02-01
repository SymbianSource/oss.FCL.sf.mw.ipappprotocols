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
* Name          : MTransactionOwner.h
* Part of       : TransactionUser
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef MTRANSACTIONOWNER_H
#define MTRANSACTIONOWNER_H

// INCLUDES
#include "SipStackServerDefs.h"

// FORWARD DECLARATIONS
class CSIPRequest;
class CSIPResponse;
class MSIPSecUser;


// CLASS DECLARATION
/**
 * This class defines the events which the Transaction subsystem will send
 * to upper layers. Any subsystem which is using Transaction subsystem, has to
 * implement this class.
 */
class MTransactionOwner
    {
public:
	virtual ~MTransactionOwner() {}

	/**
	 * Transaction has received a SIP request from a remote endpoint
     * and passes it to the upper layer.
	 *
	 * @pre						aRequest != 0
	 *
     * @param aIapId            The IAP using which the request was received.
	 * @param aTransactionId	Identifies the transaction
	 * @param aRequest		    IN: SIP request message. The ownership is
	 *							transferred.
	 * If the function leaves, the ownership of aRequest is NOT transferred.
	 */
	virtual void ReceiveL(TUint32 aIapId,
                          TTransactionId aTransactionId,
                          CSIPRequest* aRequest) = 0;

	/**
	 * Transaction has received a SIP response message from remote endpoint and
	 * passes it to the upper layer. 
	 *
	 * @pre						aResponse != 0
	 *
	 * @param aTransactionId	Identifies the transaction
	 * @param aResponse			IN: SIP response message. The ownership is
	 *							transferred.
	 * If the function leaves, the ownership of aResponse is NOT transferred.
	 */
	virtual void ReceiveL(TTransactionId aTransactionId,
                          CSIPResponse* aResponse) = 0;

	/**
	 * The specified transaction has ended and no more events from that
	 * transaction will come through the MTransactionOwner interface.
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @param aIapId The IAP used.
	 * @param aTransaction Identifies the transaction that ended
	 * @param aReason Tells why the transaction ended. KErrNone means the
	 *	transaction ended normally.
	 * @return value KErrNone if successful, otherwise a system wide error code
	 */
	virtual TInt TransactionEnded(TUint32 aIapId,
	                              TTransactionId aTransaction,
								  TInt aReason) = 0;

	/**
	 * TransactionUser is going to send the SIP request either after
	 * modifications to the request, or to another address, and asks the next
	 * CSeq sequence number to use for the request.
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @param aCSeq an in-out parameter. While entering the function contains
     *        the current CSeq of the caller. On return contains the
	 *		  incremented CSeq of the callee. If the subsystem implementing the
	 *		  interface does not keep track of the CSeqs it should simply
	 *		  increment aCSeq by one.
     *
     * @return KErrNone or a system wide error code.	 
	 */
	virtual TInt NextCSeq(TUint& aCSeq) = 0;
	
	/**
	 * Returns a pointer to the MSIPSecUser that is used to map the
	 * SIPSec cache entries to their users and can be used to query credentials
	 * from the user. 
     *
     * @return A pointer to MSIPSecUser. The ownership is not transferred.	 
	 */	
	virtual const MSIPSecUser* SIPSecUser() const = 0;
    };

#endif // end of MTRANSACTIONOWNER_H

// End of File
