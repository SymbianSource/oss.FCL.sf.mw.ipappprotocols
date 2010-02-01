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
* Name          : ServerTransaction.h
* Part of       : Transaction
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef SERVERTRANSACTION_H
#define SERVERTRANSACTION_H

// INCLUDES
#include "CTransaction.h"

// FORWARD DECLARATIONS
class CResponseQueue;
class CResponseQueueItem;

// CLASS DECLARATION
/*
 * This class represents a server transaction. It is created for handling an
 * incoming SIP request message and the SIP response message associated with it.
 */
class CServerTransaction : public CTransaction
	{
public: // Destructor

	~CServerTransaction();	

public:	//From CTransaction

	void IcmpErrorL(const TInetAddr& aAddress,
					CSipConnectionMgr::TICMPError aError);

	virtual void DeleteTimer(const CTransactionTimer& aTimer) = 0;

	virtual void TerminatedL(TInt aReason);

public: //New functions

	void PassRequestToUserAgentL(CSIPRequest* aReq);

	TBool RequestPassedToUA() const;

	/**
	 * Creates a 100 response to the SIP request and places it into send
	 * buffer (CTransaction::iOutgoingMsg).
	 *
	 * @pre iOutgoingMsg = NULL
	 *
	 * @param aReq SIP request which has been received
	 */
	void Create100L(CSIPRequest& aReq);

	void SendToTransmitterL();

	/**
	 * Sends a SIP response using the currently selected transport protocol and
	 * stores the response and its transport parameters.
	 *
	 * @pre aRespItem != NULL	 
	 *
	 * @param aRespItem Response item containing the SIP response to be sent.
	 *	Ownership is transferred.	 
	 */	
	void SendAndStoreResponseL(CResponseQueueItem* aRespItem);

	/**
	 * Sends a SIP response using the transport protocol specified by
	 * aTransportProtocol and stores the response and its transport
	 * parameters.
	 *
	 * @pre aRespItem != NULL
	 * @pre CSIPMessageUtility::CheckTransport(aTransportProtocol) == ETrue
	 *
	 * @param aRespItem Response item which is stored in the send queue,
	 *	ownership is transferred
	 * @param aTransportProtocol Transport protocol to use
	 */	
	void SendAndStoreResponseL(CResponseQueueItem* aRespItem,
							   RStringF aTransportProtocol);
	
	/**	
	 * Adds the response item into the send queue.
	 *
	 * @pre aRespItem != NULL
	 *
	 * @param aRespItem Response item which is stored in the send queue,
	 *	ownership is transferred	 
	 */	
	void AddResponseToSendQueueL(CResponseQueueItem* aRespItem) const;

	/**
	 * Extracts the oldest response from iSendQueue.
	 *
	 * @return value Response item, or NULL if there are no SIP responses in
	 *	the queue. Ownership is transferred.
	 */
	CResponseQueueItem* GetResponseFromSendQueue() const;	

protected:

	CServerTransaction(CUserAgentBase& aUserAgent,
					   CTransmitter& aTransmitter,
					   MTimerManager& aTimers,
					   CTransactionState& aInitialState,
					   TTimerValues& aTimerValues);

	void ConstructServerTaL();

private:

	void CancelAllTimers() = 0;

private: // Data

	//Contains the SIP responses that haven't been sent to ConnectionMgr yet.
	//If several responses are passed to ServerTransaction quicker than it can
	//send them forward, responses are stored here until they can be sent.
	//
	//CServerTransaction owns the queue. The ownership of the SIP responses
	//depends on the value of CResponseQueueItem::iOwnsResponse.
	CResponseQueue* iSendQueue;

	//Contains the previously sent responses. They can't be deleted when a next
	//response comes from UAS as ConnectionMgr might be sending them in case of
	//an ICMP error.
	//CServerTransaction owns the queue. The ownership of the SIP responses
	//depends on the value of CResponseQueueItem::iOwnsResponse.
	CResponseQueue* iSentResponses;

	//ETrue if the SIP request from network has been passed to UserAgent.
	//Used to filter out retransmissions.
	TBool iRequestPassedToUA;	

	void __DbgTestInvariant() const;


#ifdef CPPUNIT_TEST
	friend class CServerTransaction_Test;
	friend class CTransactionUser_Test;
#endif
	};

#endif // end of SERVERTRANSACTION_H

// End of File
