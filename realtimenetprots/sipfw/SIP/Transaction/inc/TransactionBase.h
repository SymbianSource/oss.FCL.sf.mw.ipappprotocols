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
* Name          : TransactionBase.h
* Part of       : Transaction
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/


#ifndef TRANSACTIONBASE_H
#define TRANSACTIONBASE_H

// INCLUDES
#include "SipStackServerDefs.h"
#include "sipinternalstates.h"
#include "TransactionReceiver.h"

#ifdef CPPUNIT_TEST
#include "TestCleanupStack.h"
#endif

// FORWARD DECLARATIONS
class RStringF;
class CUri8;
class CSIPRequest;
class CSIPResponse;

// CLASS DECLARATION
/*
 * Transaction interface to TransactionUser side
 */
class CTransactionBase :
    public CBase,
	public MTransactionReceiver
	{
public: // Enumerations

	//Different types of the transaction state machines
	enum TTransactionType
        {
	    KUndefinedTransaction = 0,	//Initialization value
	    KServerTransaction,			//non-INVITE
		KServerInviteTransaction,
	    KClientTransaction,			//non-INVITE
		KClientInviteTransaction
	    };

public: // Destructor

	virtual ~CTransactionBase() {}

public: // From MTransactionReceiver

    virtual void ReceiveL(CSIPRequest* aRequest) = 0;

	virtual void ReceiveL(CSIPResponse* aResponse) = 0;

	virtual void LeaveOccurred(TInt aReason) = 0;

	virtual const TSIPTransportParams& TransportParams() const = 0;

	virtual void IcmpErrorL(const TInetAddr& aAddress,
						    CSipConnectionMgr::TICMPError aError) = 0;

public: // New pure virtual functions

	/**
	 * Send a SIP request to the transaction.
	 *	 
	 * @param aReq SIP request message
	 * @param aAddress Address where the request is sent
	 * @param aProtocol Transport protocol to use
	 * @param aForceUDP ETrue if transport protocol can't be changed from UDP
	 * @param aParams Transport parameters
	 * @param aOutboundProxy Outbound proxy, NULL pointer if not used.
     *        The ownership is transferred.
	 */
	virtual void SendRequestL(CSIPRequest& aReq,
							  const TInetAddr& aAddress,
							  RStringF aProtocol,
							  TBool aForceUDP,
							  const TSIPTransportParams& aParams,
							  CUri8* aOutboundProxy) = 0;

	/**
	 * Send a SIP response message to the transaction.
	 *
	 * @pre aResp != NULL
	 *
	 * @param aResp SIP response message. Ownership is transferred.	 
	 * @param aProtocol Transport protocol to use
	 * @param aParams Transport parameters	 
	 */
	virtual void SendResponseL(CSIPResponse* aResp,
							   RStringF aProtocol,
							   const TSIPTransportParams& aParams) = 0;

	/**
	 * Clears the send buffer pointer (iOutgoingMsg), unless it contains ACK,
	 * which is the case of CInviteClientTransaction which has created the ACK
	 * by itself and owns it.
	 *	 	 
	 * @return value ETrue if the transaction can continue even without the
	 *	outgoing SIP message, EFalse otherwise.
	 */
	virtual TBool ClearSendBuffer() = 0;

    /**
	 * Obtains the transaction's current state.
	 *	 
	 * @param aState OUT: Transaction's state
	 */
    virtual void GetState(CSIPInternalStates::TState& aState) = 0;

	/**
	 * Force transaction to terminate, meaning it enters the Terminated state.
	 */
	virtual void Terminated() = 0;

	/**
	 * Force transaction to terminate, meaning it enters the Terminated state.
	 *
	 * @param aReason Reason why the transaction ends.	 
	 */
	virtual void TerminatedL(TInt aReason) = 0;

	/**
	 * Check if the transaction state machine has reached the terminated state.
	 *
	 * @return ETrue:  Transaction has terminated
     * 		   EFalse: Otherwise
	 */
	virtual TBool HasTerminated() const = 0;

	/**
	 * Clear transaction's callback to UserAgent, so that when the transaction
	 * terminates, it won't send TransactionEndsL event to UserAgent, because
	 * the UserAgent may have created another transaction for sending a SIP
	 * request.	 
	 */
	virtual void DetachFromUserAgent() = 0;
	
	/**
	 * Return the transport protocol used by the transaction.
	 *
	 * @return Transport protocol
	 */
	virtual RStringF TransportProtocol() const = 0;
	};

#endif // end of TRANSACTIONBASE_H

// End of File
