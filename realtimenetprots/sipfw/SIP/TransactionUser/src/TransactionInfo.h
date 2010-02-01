/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : TransactionInfo.h
* Part of       : TransactionUser
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef TRANSACTIONINFO_H
#define TRANSACTIONINFO_H

// INCLUDES
#include "TransactionBase.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class RStringF;
class CURIContainer;
class CSIPToHeader;
class CSIPFromHeader;
class CSIPViaHeader;
class CSIPMessage;
class CSIPCallIDHeader;
class CSIPRecordRouteHeader;
class CSIPContactHeader;

class CUserAgent;
class MTransactionHeaders;

// CLASS DECLARATION

/**
 * This class contains information identifying a transaction (=certain headers
 * from the SIP request that initiated the transaction), together with the
 * addresses of the CUserAgent and CTransaction objects created for handling
 * that transaction.
 */
class CTransactionInfo : public CBase
	{
public: // Constructor and destructor

	/**
	 * Creates a new object.	 
	 * @param aType Identifies the transaction type
     * @param aIapId IAP-id
	 * @return value New CTransactionInfo object, ownership is transferred.
	 */	
	static CTransactionInfo* NewL(CTransactionBase::TTransactionType aType,
                                  TUint32 aIapId);

	/**
	 * Destructor. Deletes the owned resources, except does NOT delete UA or
	 * transaction.	 
	 * @see FreeContents
	 */
	~CTransactionInfo();

public: // New functions

	/**
	 * Deletes the stored UA and/or transaction.	 
	 * @see ~CTransactionStore
	 */
	void FreeContents();

	/**
	 * Checks whether all the required headers for transaction matching and
	 * constructing a SIP response are present.	 
	 * @return ETrue if required headers exist, EFalse otherwise
	 */
	TBool RequiredHeadersPresent() const;

	/**
	 * Compares the stored iTransactionType to aType.	 
	 * @param aType TransactionType which is compared
	 * @return ETrue if types are same, EFalse otherwise
	 */
	TBool CompareType(CTransactionBase::TTransactionType aType) const;

	/**
	 * Compares the aMethod to the iMethod for determining if the SIP message
	 * with the aMethod belongs to the same transaction.
	 *
	 * If the SIP message that created the transaction was a request, iMethod
	 * is the request method, otherwise it is the method from CSeq.	 
	 * @param aMethod If the SIP message is request, this is the request
	 *	method. Otherwise, this is the method from CSeq.
	 * @return value ETrue if the methods can belong to same transaction
	 */	
	TBool CompareMethod(RStringF aMethod) const;

	/**
	 * Compares the Request-URI and request method to the stored values.	 
	 * @see CompareMethod
	 * @param aReqURI Request-URI
	 * @param aMethod Request method
	 * @param aSearchCanceledUas ETrue: search is done to find an UAS to be
	 *	canceled. EFalse: search is done to find a transaction to receive an
	 *	incoming SIP message.
	 * @return value ETrue: If both aReqURI and aMethod match the stored values
	 *	EFalse: otherwise
	 */
	TBool CompareRequestLine(const CURIContainer& aReqUri,
							 RStringF aMethod,
							 TBool aSearchCanceledUas) const;

	/**
	 * Compares the SIP headers of aReq to the stored headers, and determines
	 * if the transactions match.
	 * @param aReq SIP request
	 * @return ETrue if match, EFalse otherwise
	 */
	TBool CompareHeaders(CSIPRequest& aReq) const;

	/**
	 * Obtains a MTransactionReceiver that should handle events received from
	 * the network. Returns transaction, if it exists, or UA if transaction does
	 * not exist.
	 * @return MTransactionReceiver Pointer to the receiver interface.
	 *								Ownership isn't transferred.
	 */
	MTransactionReceiver* Receiver();

    /**
	 * Returns the UA.
	 * @return Pointer to UA instance. Can be NULL. Ownership isn't transferred.
	 */
	CUserAgent* UserAgent();

	/**
	 * Sets the stored UA.
	 * @post iUserAgent == aUserAgent
	 * @param aUserAgent UA instance, can be NULL. Ownership is transferred.
	 */
	void SetUserAgent(CUserAgent* aUserAgent);

	/**
	 * Sets the stored transaction.
	 * @post iTransaction == aTransaction
	 * @return Pointer to transaction. Can be NULL. Ownership isn't transferred.
	 */
	void SetTransaction(CTransactionBase* aTransaction);

    /**
	 * Obtain the TransactionId	 
	 * @return TransactionId
	 */
	TTransactionId TransactionId() const;

    /**
	 * Set new value for TransactionId	 
	 * @param aTransactionId TransactionId
	 */
	void SetTransactionId(TTransactionId aTransactionId);

    /**
	 * Obtain the IAP-id.	 
	 * @return IAP-id
	 */
    TUint32 IapId() const;

	/**
	 * Returns the topmost of the stored Via headers. Caller must not modify
	 * the Via header.	 
	 * @pre iViaHeaders.Count() > 0	 
	 * @return Topmost Via header
	 */
	CSIPViaHeader& TopVia() const;

	/**
	 * Copies and stores the transaction related headers from the SIP message.
	 * The possibly existing old headers are removed from CTransactionInfo,
	 * regardless whether the same header is present in the aMsg, so that
	 * CTransactionInfo won't contain headers from more than one different SIP
	 * message.	 
	 * @param aMsg SIP message from where the headers are copied, can be NULL.
	 *  Ownership isn't transferred. If aMsg is NULL, nothing is done.
	 */
	void UpdateMessageHeadersL(CSIPMessage* aMsg);

	/**
	 * Updates the To tag.
	 * @pre iTo != NULL, iTo->HasTag() == EFalse	 
	 * @param aTag To tag	 
	 */	
	void UpdateToTagL(RStringF aTag);

	void StoreRecordRouteHeadersL(CSIPRequest& aReq);
	void FreeRecordRouteHeaders();

	void StoreContactHeadersL(CSIPRequest& aReq);
	void FreeContactHeaders();

    /**
	 * Checks a request with the aFrom, aCallID, aCSeqNbr and aCSeqMethod is a
     * merged request by comparing these values to CTransactionInfo's similar
     * values.	 
	 * @param aUserAgent UA initiating the search
     * @param aFrom From header of a received request
     * @param aCallID Call-ID header of a received request
     * @param aCSeqNbr CSeq sequence number of a received request
     * @param aCSeqMethod CSeq method of a received request
	 * @return ETrue if the request is a merged request, EFalse otherwise.
	 */
	TBool IsMergedRequest(const CUserAgent& aUserAgent,
						  CSIPFromHeader& aFrom,
						  CSIPCallIDHeader& aCallID,
						  TUint aCSeqNbr,
						  RStringF aCSeqMethod) const;

    MTransactionHeaders* TransactionHeadersL();

	/**
	 * Copies To, From, Call-ID and CSeq headers from CTransactionInfo into the
	 * SIP message. If some headers already exist in the aMsg, they are not
	 * replaced.	 
	 * @param aMsg SIP message to which the headers are copied	 
	 */
	void CopyToFromCallIDCSeqToMsgL(CSIPMessage& aMsg) const;

	/**
	 * Copies the stored Via headers into the SIP message.	 
	 * @param aMsg SIP message into which where the Vias are copied	 
	 */
	void CopyViaHeadersToMsgL(CSIPMessage& aMsg) const;

	void CopyRequestUriToRequestL(CSIPRequest& aReq) const;

	/**
	 * Creates a SIP request from the stored headers.	 
	 * @return SIP request, or NULL if there is not enough information to
	 *	create it. Ownership is transferred.
	 */
	CSIPRequest* BuildRequestFromStoredInfoL() const;

    /**
	 * Returns the request method.	 
	 * @return Request method
	 */
	RStringF RequestMethod() const;

	TSglQueLink	iLink;

private: // New functions, for internal use

	/**
	 * Constructor	 
	 * @param aType Identifies the transaction type
     * @param aIapId IAP-id	 
	 */	
	CTransactionInfo(CTransactionBase::TTransactionType aType, TUint32 aIapId);

	/**
	 * Clears the SIP message related members
	 */
	void ClearMessageHeaders();

	/**
	 * Checks if the method has been stored.	 
	 * @param ETrue method has been stored, EFalse otherwise
	 */
	TBool MethodExists() const;	

	/**
	 * Copies and stores the Via headers from aMsg, maintaining their order.	 
	 * @param aMsg SIP message
	 */
	void CopyViaHeadersFromMsgL(CSIPMessage& aMsg);
	
private: // Data

	TTransactionId			iTransactionId;
	CTransactionBase::TTransactionType iTransactionType;

    // IAP-id
    TUint32                 iIapId;

	// UA, owned. NULL if iTransaction has been detached from UA.    
	CUserAgent*				iUserAgent;

	// Transaction, can be NULL. Not owned.
	CTransactionBase*		iTransaction;

	// Request-URI, owned.
	CURIContainer*			iRequestURI;
	
	// To header, can be NULL. Owned.
	CSIPToHeader*			iTo;
	// From header, can be NULL. Owned.
	CSIPFromHeader*			iFrom;
	// Call-ID header, can be NULL. Owned.
	CSIPCallIDHeader*		iCallID;

	// Method from Request line (=same as CSeq method), can be NULL
	RStringF				iMethod;

	// CSeq sequence number
	TUint                   iCSeq;

	// All Via headers of the SIP request are copied here.	
	RPointerArray<CSIPViaHeader> iViaHeaders;

	// Record-route headers, owned.
	// Stored only for incoming requests outside of a dialog.
	RPointerArray<CSIPRecordRouteHeader> iRecordRoute;

	// Contact headers, owned.
	// Stored only for incoming requests outside of a dialog.
	RPointerArray<CSIPContactHeader> iContact;	


#ifdef CPPUNIT_TEST	
	friend class CTransactionStore_Test;
	friend class CNormalUAC_ResolveAddress_Test;
	friend class CNormalUAC_WaitResponse_Test;
	friend class CTransactionUser_Test;
	friend class CUserAgentServer_Test;
	friend class CTransactionInfo_Test;
#endif
	};

#endif // end of TRANSACTIONINFO_H

// End of File
