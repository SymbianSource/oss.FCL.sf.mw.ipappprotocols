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
* Name          : CTransactionStore.h
* Part of       : TransactionUser
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/

#ifndef CTRANSACTIONSTORE_H
#define CTRANSACTIONSTORE_H

// INCLUDES
#include "SipStackServerDefs.h"

#include "MTransactionStore.h"
#include "MTransactionFinder.h"
#include "TransactionInfo.h"

#include <e32base.h>
#include <stringpool.h>

// FORWARD DECLARATIONS
class CTransactionBase;

class MTransactionOwner;
class CUserAgent;
class CUserAgentServer;
class MTransactionHeaders;

// CLASS DECLARATION

/**
 * CTransactionStore contains the information of all existing transactions.
 * This information is used for e.g. routing the incoming SIP messages to the
 * correct transaction or UA.
 */
class CTransactionStore :
    public CBase,
    public MTransactionStore,
	public MTransactionFinder	
	{
public: // Constructors and destructor

	/**
	 * Creates a new object.	 	 
	 * @return New CTransactionStore object, ownership is transferred.
	 */	
	static CTransactionStore* NewL();

	~CTransactionStore();

public: // From MTransactionStore

	TInt UpdateTransactionId(TTransactionId aOldTransactionId,
							 TTransactionId aNewTransactionId);

	void Remove(TTransactionId aTransactionId);

public: // From MTransactionFinder

	MReceiverObserver* Search(CSIPMessage& aMsg);

	void IcmpErrorL(const TInetAddr& aAddress,
					CSipConnectionMgr::TICMPError aError);	

public: // New functions

	/**
	 * Generates a new unique value to be used as a TransactionId.	 
	 * @return value New TransactionId value
	 */
	TTransactionId NewTransactionId();

	/**
	 * Stores information of a transaction.
	 * @pre aUserAgent != NULL
	 * @param aTransactionId transaction identifier
	 * @param aUserAgent UA, ownership is transferred
	 * @param aTransaction Transaction object, can be NULL. Ownership is not
	 *		  transferred,. as UA owns aTransaction.
	 * @param aMsg SIP message which initiated the transaction, can be NULL.
	 *		  Ownership is not transferred.
	 * @param aType Identifies which kind of transaction this is
	 *		  (client, server, etc.)
	 */
	void AddL(TTransactionId aTransactionId,
			  CUserAgent* aUserAgent,
			  CTransactionBase* aTransaction,
			  CSIPMessage* aMsg,
			  CTransactionBase::TTransactionType aType);

	/**
	 * Updates the information of the transaction identified by aTransactionId.
	 * @pre Transaction must exist in the CTransactionStore
	 * @param aTransactionId transaction identifier
	 * @param aTransaction New value for the transaction pointer, can be NULL.
	 *  Ownership is not transferred.
	 * @param aMsg The transaction related headers which are present in the
	 *  aMsg are copied to CTransactionStore. The possibly existing previously
	 *  stored headers discarded. This parameter can be NULL. Ownership is not
	 *  transferred.
	 * @leave KErrNotFound If no such transaction is found
	 */
	void UpdateL(TTransactionId aTransactionId,		 
				 CTransactionBase* aTransaction,
				 CSIPMessage* aMsg);

	/**
	 * Updates the To tag of the transaction identified by the aTransactionId.
	 * @pre Transaction must exists in the CTransactionStore
	 * @param aTransactionId transaction identifier
	 * @param aTag To tag
	 * @leave KErrNotFound If no such transaction is found
	 */
	void UpdateToTagL(TTransactionId aTransactionId, RStringF aTag);

	/**
	 * Clears the pointer to UA by setting it NULL.
	 * The UA is identified by aTransactionId.	 
	 * @param aTransactionId Identifies the UA
	 */
	void ClearUserAgent(TTransactionId aTransactionId);

	/**
	 * Clears the pointer to transaction by setting it NULL. Transaction is
	 * identified by aTransactionId.
	 * @param aTransactionId Identifies the transaction	 
	 */
	void ClearTransaction(TTransactionId aTransactionId);

	/**
	 * Searches the store for an UAS to match the CANCEL.	 
	 * @param aCancel IN: CANCEL request
	 * @param aCancelUAS IN: UA that is searching for an UAS to be canceled.
	 *  TransactionStore checks aCancelUAS to avoid returning the same UAS
	 *  that is handling the CANCEL request.
	 * @return UAS to be canceled, NULL if not found. Ownership isn't
	 *	transferred.
	 */
	CUserAgentServer* SearchUasToCancel(CSIPRequest& aCancel,
										const CUserAgent& aCancelUAS);	

	/**
	 * Searches the store for an UA having the specified aTransactionId.
	 * @param aTransactionId Identifies the UA
	 * @return The found UA or NULL if not found. Ownership is not transferred.
	 */
	CUserAgent* SearchById(TTransactionId aTransactionId);

	/**
	 * Checks if the SIP request is a merged request.
	 * Searches through the stored transactions for a transaction with a
	 * matching From tag, Call-ID and CSeq. Skips the specified UA which is
	 * aUserAgent requesting this search.
	 * @param aUserAgent UA asking if the request is merged
	 * @param aReq SIP request
	 * @return ETrue if the request is merged, EFalse otherwise
	 */
	TBool IsMergedRequest(const CUserAgent& aUserAgent, CSIPRequest& aReq);

	/**
	 * Copies any Record-Route headers possibly present in aReq, to the
	 * specified transaction's stored information.
	 * 
	 * @pre The transaction identified by aTransactionId must not already have
	 *	stored Record-Route headers.
	 * @post
	 *
	 * @param aTransactionId Transaction id used for identifying the
	 *	transaction, which will store the Record-Route headers.
	 * @param aReq SIP request from which the headers are copied from.
	 */
	void StoreRecordRouteHeadersL(TTransactionId aTransactionId,
								  CSIPRequest& aReq);

	/**
	 * Frees the stored Record-Route headers from the specified transaction.
	 * @param aTransactionId Transaction id used for identifying the
	 *	transaction which the Record-Route headers are freed.	 
	 */
	void FreeRecordRouteHeaders(TTransactionId aTransactionId);

	/**
	 * Copies any Contact headers possibly present in aReq, to the specified
	 * transaction's stored information.
	 * @pre The transaction identified by aTransactionId must not already have
	 *	stored Contact headers.
	 * @param aTransactionId Transaction id used for identifying the
	 *	transaction which will store the Contact headers.
	 * @param aReq SIP request from which the headers are copied from.
	 */
	void StoreContactHeadersL(TTransactionId aTransactionId,
							  CSIPRequest& aReq);

	/**
	 * Frees the stored Contact headers from the specified transaction.
	 * @param aTransactionId Transaction id used for identifying the
	 *	transaction which the Contact headers are freed.	 
	 */
	void FreeContactHeaders(TTransactionId aTransactionId);

	/**
	 * Copies certain headers from the CTransactionInfo item, identified by the
	 * aTransactionId, to the response message (aResp).
	 * @param aTransactionId Transaction id used for locating the correct
	 *	CTransactionInfo object	 
	 * @param aResp IN/OUT: SIP response message which will be filled.
	 */
	void CopyHeadersToResponseL(TTransactionId aTransactionId,
								CSIPResponse& aResp);

	/**
	 * Copies certain headers from the CTransactionInfo item, identified by the
	 * aTransactionId, to the aReq.
	 * @param aTransactionId Transaction id used for locating the
	 *	CTransactionInfo object	from which the headers are copied from.
	 * @param aReq SIP request into which the headers are copied
	 * @param aCopyRequestURI ETrue: Request-URI is copied, EFalse: otherwise
	 * @param aCopyViaHeaders ETrue: Via headers are copied, EFalse: otherwise
	 */
	void CopyHeadersToRequestL(TTransactionId aTransactionId,
							   CSIPRequest& aReq,
							   TBool aCopyRequestURI,
							   TBool aCopyViaHeaders);

	/**
	 * Retrieve the transaction related headers of the SIP message that created
	 * the transaction.
	 * @param aTransactionId Identifies the transaction whose SIP headers are
	 *	requested.
	 * @return MTransactionHeaders* Object containing the SIP headers.
	 *	Ownership is transferred. NULL if the transaction was not found.
	 */	
    MTransactionHeaders* TransactionHeadersL(TTransactionId aTransactionId);

	/**
	 * Obtains information from one transaction (not from any specific
	 * transaction). This information will be as a input to generate random
	 * tags.	 
	 * @param aTransactionId Will be filled by a transactionId value, unless
	 *	there are no stored transactions.
	 * @param aUserAgent OUT: Will be set to point to a UA object.
	 *	If there are no stored UAs, this pointer isn't set.
	 *	Ownership isn't transferred.
	 * @param aMsg OUT: Pointer to a SIP Message, might be zero if no SIP
	 *	message was available. Ownership is transferred.	 
	 */
	void RandomTaInfoL(TTransactionId& aTaId,
					   CUserAgent** aUserAgent,
					   CSIPMessage** aMsg);

    /**
	 * Returns the request method of the transaction identified by
     * aTransactionId.
	 * @pre A transaction identified by aTransactionId must exist in the store.	 
	 * @param aTransactionId TransactionId
	 * @return Request method
	 */
	RStringF RequestMethod(TTransactionId aTransactionId);

	/**
	 * Clears the MTransactionOwner pointers from the stored UA objects, which
	 * have a matching MTransactionOwner callback.	 
	 * @pre	aObserver != NULL
 	 * @param aObserver pointer to the MTransactionOwner callback. Ownership
	 *	isn't transferred.
	 * @return KErrNone if successful, KErrNotFound if no UA used the specified
	 *	aObserver callback.
	 */
	TInt ClearTransactionOwner(const MTransactionOwner* aObserver);

	/**
	 * Goes through all stored transactions using the given IAP-id and those
     * which are currently resolving the remote address, are stopped.
	 * @param aIapId IAP-id
	 * @param aReason Reason why transaction is stopped	 
	 */
	void EndResolvingTransactions(TUint32 aIapId, TInt aReason);

    /**
	 * Delete any transactions matching the IAP-id.
     * @param aIapId IAP-id
	 */
    void RemoveItemsByIapId(TUint32 aIapId);

	/**
	 * Check if there is space for a new server transaction.
     * @return ETrue New server transaction can be created
     *		   EFalse otherwise
	 */
	TBool AllowMoreServerTransactions() const;
private: // New functions, for internal use
	void ConstructL();
	CTransactionStore();

	/**
	 * Removes aItem from the list, deletes aItem's contents and the aItem.	 
	 * @pre	aItem != NULL	 
	 * @param aItem Item to delete.
	 */
	void DeleteItem(CTransactionInfo* aItem);

	/**
	 * Searches the store for a CTransactionInfo item of the transaction with
	 * the given aTransactionId.	 
	 * @param aTransactionId identifies the transaction whose information is
	 *	searched for
	 * @return Pointer to the object containing transaction's information,
	 *	NULL if no such transaction is found. Ownership isn't transferred.
	 */
	CTransactionInfo* FindTransactionInfo(TTransactionId aTransactionId);

	/**
	 * Searches the store for an object which should receive a SIP message
	 * with the Branch-ID, Sent-by value and method given as parameters.
	 * Transaction type has to also match.	 
     * @param aMsg SIP message received from remote endpoint
	 * @param aType identifies which kind of transaction this is
	 *	(client, server, etc.)
	 * @param aUserAgent IN: If non-zero: this is the UA that searches for an
	 *	UAS to be canceled. If NULL: the search is done to find a transaction
	 *  to receive an incoming SIP message. Ownership isn't transferred.     
	 * @return Pointer to the receiving interface of the object which should
	 *  receive the SIP message (the actual object can UA or transaction).
	 *	NULL if not found. Ownership is not transferred.
	 */
	MReceiverObserver* SearchByBranch(CSIPMessage& aMsg,
						 			  CTransactionBase::TTransactionType aType,
						 			  const CUserAgent* aUserAgent);

	/**
	 * Searches the store for an object which should receive the SIP request,
	 * using the To, From, CallID, CSeq, topmost Via, Request-URI.
     * Transaction type has to also match. Via Branch is not used for matching.	 
     * @param aReq SIP request received from remote endpoint
	 * @param aType identifies which kind of transaction this is	 
	 * @param aUserAgent IN: If non-NULL: the UA which is searching for an UAS
	 *	to be canceled. If NULL: the search is done to find a transaction to
	 *	receive an incoming SIP message. Ownership isn't transferred.     
	 * @return Receiving interface of the object which should receive the
     *  request (the actual object can UA or transaction). NULL if no matching
     *	object was found. Ownership isn't transferred.
	 */
	MReceiverObserver*
        SearchServerByHeaders(CSIPRequest& aReq,
							  CTransactionBase::TTransactionType aType,
							  const CUserAgent* aUserAgent);

	/**
	 * Check if all of the required headers for Transaction matching are
	 * present in the SIP message.	 
	 * @param aMsg SIP message
	 * @return ETrue if all required headers exist, EFalse otherwise.
	 */
	TBool RequiredHeadersPresent(CSIPMessage& aMsg) const;

    /**
	 * Compares the TTransactionType stored in aInfo with aType.	 
	 * @param aInfo Transaction information
     * @param aType Transaction type
     * @param aSearchCanceledUas Indicates whether the comparison is performed
     *  for searching an UAS to be canceled.
	 * @return ETrue if transaction types match, EFalse otherwise
	 */
	TBool CompareTransactionTypes(const CTransactionInfo& aInfo,
						          CTransactionBase::TTransactionType aType,
						          TBool aSearchCanceledUas) const;

    /**
	 * This function is used to return the receiver interface of the
     * transaction/UA that should be given the SIP message received from the
     * network.
     * If the SIP message is ACK and the UA, pointed by aInfo, has sent a 2xx
     * response, the transaction/UA is ignored as Via Branch must not be used
     * to find the transaction in case of ACK to a 2xx.	 
     * @param aMsg SIP message received from network.
	 * @param aInfo Information of the transaction that should receive a
     *  SIP message received from network. aInfo has been determined by an
     *  earlier search among the existing transactions.
     * @param aUserAgent If NULL, this parameter is ignored. If UA is not NULL,
     *  it is the UA that is searching for an UAS to be canceled. Used to the
     *  avoid returning the MReceiverObserver interface of the same UAS that
     *  initiated the search. Ownership isn't transferred.
	 * @return Object where the SIP message received from network should be
     *  passed next or NULL if there is no suitable object. Ownership is not
     *  transferred.
	 */
    MReceiverObserver* IgnoreAckTo2xx(const CSIPMessage& aMsg,
                                      CTransactionInfo& aInfo,
								      const CUserAgent* aUserAgent) const;

    /**
	 * This function is used to return the receiver interface of the
     * transaction/UA that should be given the SIP message received from the
     * network.	 
	 * @param aInfo Information of the transaction that should receive a
     *  SIP message received from network. aInfo has been determined by an
     *  earlier search among the existing transactions.
     * @param aUserAgent If NULL, this parameter is ignored. If UA is not NULL,
     *	it is the UA that is searching for an UAS to be canceled. Used to the
     *  avoid returning the MReceiverObserver interface of the same UAS that
     *	initiated the search. Ownership isn't transferred.     
	 * @return Object where the SIP message received from network should be
     *  passed next or NULL if there is no suitable object. Ownership is not
     *  transferred.
	 */
	MReceiverObserver* CheckResult(CTransactionInfo& aInfo,
								   const CUserAgent* aUserAgent) const;

	/**
	 * Remove item from list of transactions.
     * @param aItem Item to remove
     */
	void RemoveListItem(CTransactionInfo& aItem);
private: // Data

	// Information of all existing UAs and transactions
	TSglQue<CTransactionInfo> iList;

	// Counter for generating unique numbers to be used as TransactionIds.
	// This will eventually wrap around, but by the time that happens, it is
	// expected the old transactions using the same TransactionId values have
	// been terminated.
	TTransactionId iTransactionIdCounter;
	// Amount of UAS / server transactions in iList
	TInt iServerTransactionCount;
	// Maximum amount of UAS / server transactions in iList
	TInt iMaxServerTransactions;

	// We don't use __DECLARE_TEST because we don't want to export this function.
	void __DbgTestInvariant() const;



#ifdef CPPUNIT_TEST	
	friend class CTransactionStore_Test;	
	friend class CTransactionUser_Test;
	friend class CNormalUAC_ResolveAddress_Test;
	friend class CNormalUAC_WaitResponse_Test;
	friend class CUserAgentServer_Test;
	friend class CUserAgentClient_Test;
#endif
	};

#endif // end of CTRANSACTIONSTORE_H

// End of File
