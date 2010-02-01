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
* Name          : MTransactionUser.h
* Part of       : TransactionUser
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/


#ifndef MTRANSACTIONUSER_H
#define MTRANSACTIONUSER_H

// INCLUDES
#include "SipStackServerDefs.h"
#include <in_sock.h>

// FORWARD DECLARATIONS
class CSIPRequest;
class CSIPResponse;
class CURIContainer;
class MTransactionOwner;
class MTransactionHeaders;
class TSIPTransportParams;
class MSIPNATBindingObserver;


// CLASS DECLARATION

/**
 * Transaction user interface for sending SIP messages, retrieving state of a
 * particular transaction and detaching from transaction.
 */
class MTransactionUser
	{
public:
	enum TRandomStringLength
		{
		// Amount of characters in a To/From header's tag when TransactionUser
		// creates it
		KTagLength = 20,

		// Amount of characters in a Call-ID when TransactionUser creates it
		KCallIDLength = 30
		};

	virtual ~MTransactionUser() {}

	/**
	 * Send a SIP request message. In case of error, function leaves and 
	 * the ownership of the SIP Request isn't transferred.
	 *
	 * @pre aRequest!=NULL
	 * @pre aObserver!=NULL
	 *
	 * @param aTransactionId   IN: If the request is ACK, this parameter should
     *                         be a TransactionId of the INVITE transaction.
	 *						   OUT: If the request is not ACK, a new transaction
	 *						   is created and its TransactionId is filled here.
	 * @param aRegistrationId  IN: a registration id by which TU tries to find
	 *                         an outbound proxy for the request.
	 * @param aRequest	       IN: SIP request message. Ownership is transferred.
	 * @param aObserver		   IN: Callback where the transaction will send
	 *						   events. Ownership isn't transferred.	 
	 * @param aRemoteTarget    IN: Remote target of the request.
     * @param aTransportParams IN: Transport parameters to be used when handing
     *                         the request to SIP Connection Mgr.
	 * @param aDeleteRequest   If ETrue, TransactionUser will delete aRequest
	 *						   when the transaction ends. If EFalse, aRequest is
	 *						   not deleted by TransactionUser.
	 */
	virtual void SendL (TTransactionId& aTransactionId,
	                    TRegistrationId aRegistrationId,
					    CSIPRequest* aRequest,
					    MTransactionOwner* aObserver,
						CURIContainer& aRemoteTarget,
                        const TSIPTransportParams& aTransportParams,
					    TBool aDeleteRequest) = 0;

	/**
	 * Send a SIP request message. In case of error, function leaves and 
	 * the ownership of the SIP Request isn't transferred.
	 *
	 * @pre aRequest!=NULL
	 * @pre aObserver!=NULL
	 *
	 * @param aTransactionId   IN: If the request is ACK, this parameter should
     *                         be a TransactionId of the INVITE transaction.
	 *						   OUT: If the request is not ACK, a new transaction
	 *						   is created and its TransactionId is filled here.
	 * @param aRegistrationId  IN: a registration id by which TU tries to find
	 *                         an outbound proxy for the request.
	 * @param aRequest	       IN: SIP request message. Ownership is transferred.
	 * @param aObserver		   IN: Callback where the transaction will send events.
	 *						   Ownership isn't transferred.
	 * @param aRemoteTarget    IN: Remote target of the request.
     * @param aTransportParams IN: Transport parameters to be used when handing
     *                         the request to SIP Connection Mgr.
	 * @param aDeleteRequest   If ETrue, TransactionUser will delete aRequest
	 *						   when the transaction ends. If EFalse, aRequest is
	 *						   not deleted by TransactionUser.
	 * @returns                A pointer to copies of transaction related headers
	 *                         stored and possibly created by transactions.
	 *                         The ownership is transferred.
	 */
	virtual MTransactionHeaders*
        SendAndGetHeadersL (TTransactionId& aTransactionId,
		                    TRegistrationId aRegistrationId,
	                        CSIPRequest* aRequest,
	                        MTransactionOwner* aObserver,
					        CURIContainer& aRemoteTarget,
                            const TSIPTransportParams& aTransportParams,
                            TBool aDeleteRequest) = 0;

	/**
	 * Send a SIP response message, using an existing transaction.
	 * Fills From- and To-header and other transaction related headers
	 * if they are missing. In case of error, function leaves and
	 * the ownership of aResponse is NOT transferred.
	 *
	 * @pre transaction identified by aTransactionId exists,
	 * @pre aResponse!=NULL
	 *
	 * @param aTransactionId   Identifies the transaction
	 * @param aResponse	       IN: SIP response message.
	 *                         The ownership is transferred.
     * @param aTransportParams IN: Transport parameters to be used when handing
     *                         the response to SIP Connection Mgr.
	 */
	virtual void SendL (TTransactionId aTransactionId,
					    CSIPResponse* aResponse,
					    const TSIPTransportParams& aTransportParams) const = 0;


	/**
	 * Send a SIP response using an existing transaction and specify a new
	 * callback for the transaction to use.
	 * Fills From- and To-header and other transaction related headers
	 * if they are missing. In case of error, function leaves and
	 * the ownership of aResponse is NOT transferred.
	 *
	 * @pre transaction identified by aTransactionId exists
	 * @pre aResponse!=NULL
	 * @pre aNewObserver!=NULL
	 *
	 * @param aTransactionId   Identifies the transaction
	 * @param aResponse		   IN: SIP response message. Ownership transferred.
	 * @param aNewObserver     IN: New callback replacing the existing callback
	 *						   used by the the transaction. Ownership isn't
	 *						   transferred.
     * @param aTransportParams IN: Transport parameters to be used when handing
     *                         the response to SIP Connection Mgr.	 
	 */		
	virtual void SendL (TTransactionId aTransactionId,
					    CSIPResponse* aResponse,
					    MTransactionOwner* aNewObserver,
					    const TSIPTransportParams& aTransportParams) const = 0;

	/**
	 * Send a CANCEL request. In case of error, function leaves and 
	 * the ownership of the aCancel isn't transferred. CANCEL can only be sent
	 * when the INVITE client transaction has received a provisional response,
	 * but hasn't yet received a final response.
	 *
	 * @pre aCancel!=NULL
	 * @pre aObserver!=NULL
	 *
	 * @param aInviteTaId	  TransactionId of the INVITE client transaction to
	 *						  be canceled.
	 * @param aCancelTaId	  OUT: TransactionId of the transaction sending
	 *						  CANCEL	 
	 * @param aObserver		  IN: Callback where the CANCEL transaction will
	 *						  send events. Ownership isn't transferred.	 
	 */
	virtual void SendCancelL (TTransactionId aInviteTaId,
							  TTransactionId& aCancelTaId,							  
							  MTransactionOwner* aObserver) = 0;

	/**
	 * The transaction with the specified transaction id clears its
	 * MTransactionOwner callback and won't send any more events.
	 * Transaction won't be stopped prematurely, it runs silently to completion.
	 *
	 * This function doesn't leave in case of error, as it is thought this
	 * function is usually also called from destructors.
	 *	 
	 * @pre aTransactionId != KSIPEmptyTransactionId
	 *
	 * @param aTransactionId Identifies the transaction	 
	 * @returns KErrNone if successful
	 *          KErrNotFound if no such transaction exists
	 */	
	virtual TInt
        ClearTransactionOwner (TTransactionId aTransactionId) const = 0;

	/**
	 * Transactions using the specified callback clear their MTransactionOwner
     * callback and won't send any more events.
	 * Transactions won't be stopped prematurely, they run silently to
	 * completion.
	 *
	 * This function doesn't leave in case of error, as it is thought this
	 * function is usually also called from destructors.
	 *
	 * @pre aObserver != NULL
	 *
	 * @param aObserver	IN: Callback address. Ownership isn't transferred.
	 * @returns KErrNone if successful
	 *          KErrNotFound if no such transaction exists
	 */
	virtual TInt ClearTransactionOwner (MTransactionOwner* aObserver) const = 0;

	/**
	 * Retrieve the transaction related headers of the SIP message that created
	 * the transaction. Leaves if memory allocation for the requested headers
	 * fails.
	 *
	 * @param aTransactionId the transaction whose SIP headers are requested.
	 *
	 * @returns NULL if no such transaction exists,
	 *          otherwise an object implementing MTransactionHeaders interface,
	 *          that contains copies of the transaction related SIP headers.
	 *          Ownership is transferred.
	 */
	virtual MTransactionHeaders*
        TransactionHeadersL (TTransactionId aTransactionId) const = 0;

	/**
	 * Fills the descriptor with random characters.
	 *
	 * @param aTag Descriptor into which the random characters are added.
	 *        The existing characters are not overwritten.
	 *        Adds as many characters as there is space left in the aTag.
	 */
	virtual void MakeTagL (TDes8& aTag) const = 0;

    /**
 	 * Obtain the local address associated with the given IAP identifier.
	 *
	 * @param aIapId IAP identifier
     * @param aAddr Local address will be written here
	 */
    virtual void GetLocalAddress (TUint32 aIapId, TInetAddr& aAddr) const = 0;

    /**
    * Frees all the resources reserved for the observer.
    * @param aSIPNATBindingObserver a observer
    */
    virtual void
    	FreeResources (MSIPNATBindingObserver& aSIPNATBindingObserver) = 0;
    
    /**
    * Obtains  the next hop address associated with the given transaction id.
	*
    * @param aTransactionId transaction identifier
	*
    * @param aAddr Local address will be written here
	*
	* @returns nex hop IP
    */
    virtual TInt 
    	NextHopIP(const TTransactionId& aTransactionId, TInetAddr& aAddr) = 0;
	};

#endif // MTRANSACTIONUSER_H

// End of File
