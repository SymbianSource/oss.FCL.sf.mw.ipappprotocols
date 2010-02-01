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
* Name          : ClientTransaction.h
* Part of       : Transaction
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef CLIENTTRANSACTION_H
#define CLIENTTRANSACTION_H

// INCLUDES
#include "CTransaction.h"

// FORWARD DECLARATIONS
class CDeleteMgr;
class MTransactionStore;

// CLASS DECLARATION
/*
 * This class represents a client transaction. It is created for handling an
 * outgoing SIP request message and the SIP response message associated with it.
 */
class CClientTransaction : public CTransaction
	{
public: // Destructor

	~CClientTransaction();
	
public:	// From CTransaction

	void IcmpErrorL(const TInetAddr& aAddress,
				    CSipConnectionMgr::TICMPError aError);

	virtual void DeleteTimer(const CTransactionTimer& aTimer) = 0;

	/**
	 * ClientTransaction enters terminated state. If transaction has been
	 * detached from the UAC, it uses DeleteMgr to free itself.
	 *
	 * @param aReason Reason why ClientTransaction ends	 
	 */
	void TerminatedL(TInt aReason);
	
	/**
	 * Sends the SIP request in the send buffer (iOutgoingMsg) to CTransmitter.
	 *
	 * CClientTransaction passes a reference to the iOutgoingMsg for the
	 * CTransmitter, which in turn gives a reference to the request for the
	 * ConnectionMgr.
     *
	 * UAC owns the request, unless it is an ACK created by
	 * CInviteClientTransaction instead of UAC.
	 *
	 * @pre iOutgoingMsg != NULL, iTransmitter isn't active
	 */
	void SendToTransmitterL();

private: // From CTransaction

	virtual void CancelAllTimers() = 0;

public: // New functions

	TTransactionId TransactionId() const;

	/**
	 * Stores SIP request and related parameters.
	 *
	 * @pre !iOutgoingMsg && !iOutboundProxy
	 *
	 * @param aReq SIP request
	 * @param aAddress Address where the request will be sent to
	 * @param aParams Transport parameters to use when sending the SIP request	 
	 * @param aTransportProtocol The transport protocol used for sending
	 * @param aForceUDP ETrue if transport protocol can't be changed from UDP
	 * @param aOutboundProxy Outbound proxy, NULL pointer if not used.
     *        The ownership is transferred.
	 */
	void StoreRequest(CSIPRequest& aReq,
					  const TInetAddr& aAddress,
					  const TSIPTransportParams& aParams,
					  RStringF aTransportProtocol,
					  TBool aForceUDP,
					  CUri8* aOutboundProxy);

	/**
	 * Clears, but does not delete, the outbound proxy.
	 *
	 * @post iOutboundProxy == NULL	 
	 */
	void ClearProxy();

	/**
	 * Filter out retransmissions and invalid SIP responses.
	 *
	 * @param aResp Received SIP response
	 * @return ETrue: Transaction will handle the response, ownership of aResp
	 *	isn't transferred.
	 *	EFalse: Response is dropped, ownership of aResp has been transferred.
	 */
	TBool ShouldTaHandleResponse(CSIPResponse* aResp) const;

    /**
	 * Forwards the SIP response to UAC.
	 *
	 * @pre iUserAgent != NULL, aResp != NULL
	 *
	 * @param aResp SIP response, ownership is transferred	 
	 */
	void PassResponseToUserAgentL(CSIPResponse* aResp);

	/**
	 * CTransaction will run independently of UAC.
	 *
	 * CNormalClientTransaction absorbs response retransmissions.
	 *
	 * CInviteClientTransaction sends ACK to a non-2xx response by itself, and
	 * takes ownership of CTransaction::iTransmitter.
	 *
	 * @param aTransactionId TransactionId that CTransaction will use to
	 *	remove its information from MTransactionStore
	 * @param aDeleteMgr DeleteMgr to use to free the transaction when it
	 *  terminates. Ownership isn't transferred.
	 * @param aClearTransmitter If ETrue, transaction detaches itself from
	 *	CTransmitter and clears its pointer to it.
	 */
	void RunIndependently(TTransactionId aTransactionId,
						  CDeleteMgr* aDeleteMgr,
						  TBool aClearTransmitter);	

	/**
	 * Check if ConnectionMgr has changed the transport protocol in the top
	 * Via header from the value that transaction used. If transport changes,
	 * it affects some transaction timers.	 
	 */
	void CheckForTransportChange();	

protected:

	/**
	 * Constructor
	 *
	 * @param aUserAgent UAC owning the new transaction being created.
	 * @param aTransmitter CTransmitter for sending requests
	 * @param aTimers Timer services
	 * @param aInitialState Initial state of the transaction
	 * @param aTimerValues Values for calculating timer durations
	 * @param aTransactionStore TransactionStore
	 */
	CClientTransaction(CUserAgentBase& aUserAgent,
					   CTransmitter& aTransmitter,
					   MTimerManager& aTimers,
					   CTransactionState& aInitialState,
					   TTimerValues& aTimerValues,
					   MTransactionStore& aTransactionStore);

	/**
	 * Check whether the received SIP response is ok.
	 *
	 * @param aResp SIP response
	 * @return value ETrue  the response is ok,
	 *	             EFalse otherwise
	 */
	TBool CheckReceivedResponse(CSIPResponse& aResp) const;

protected: // Data

	//Used for deleting CClientTransaction if it has been detached from the UAC.
	//This is non-NULL only when UAC decided to send the request again, using
	//another transaction instance.
	//If iDeleteMgr is non-NULL, CInviteClientTransaction owns the
	//CTransaction::iTransmitter.
	CDeleteMgr*	iDeleteMgr;

private: // Data

	//The response code of the most recently received SIP response. Used to
    //filter the retransmissions from being sent to UserAgent.
	TUint iReceivedResponseCode;

	//Address where the SIP request is sent. Obtained from UserAgent and
	//stored here for retransmissions.
	TInetAddr iRemoteAddr;

	//ETrue if transport protocol can't be changed from UDP
	TBool iForceUDP;

    //iTransactionId and iTransactionStore are needed by a detached transaction
    //when UserAgent has created another transaction to send an updated request.

	//Identifies the transaction
	TTransactionId iTransactionId;

	//A detached transaction must remove its information from MTransactionStore
	//when terminating, since UAC is attached to the new transaction, and no
	//longer to the old transaction(s).
	MTransactionStore& iTransactionStore;

	//Outbound proxy, or NULL if proxy is not used. Owned.
	CUri8* iOutboundProxy;

private: // For testing purposes

	void __DbgTestInvariant() const;


#ifdef CPPUNIT_TEST
	friend class ClientTransaction_Test;
	friend class CNormalCTa_Trying_Test;
	friend class CTransactionUser_Test;
	friend class CInviteCTa_Calling_Test;
#endif
	};

#endif // end of CLIENTTRANSACTION_H

// End of File
