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
* Name          : InviteClientTa.h
* Part of       : Transaction
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef INVITECLIENTTA_H
#define INVITECLIENTTA_H

// INCLUDES
#include "ClientTransaction.h"

// FORWARD DECLARATIONS
class CTimerBase;
class CTimerRetransmit;
class CTimerTerminateTa;

// CLASS DECLARATION
/*
 * This class is used for sending INVITE, receiving responses to it and sending
 * ACK.
 */
class CInviteClientTransaction : public CClientTransaction
	{
public: // Constructors and destructor

	/**
	 * Creates a new instance of CInviteClientTransaction	 
	 *
	 * @param aUserAgent UserAgent owning the new transaction being created.
	 * @param aTransmitter CTransmitter used for sending messages
	 * @param aTimers Timer services interface
	 * @param aInitialState Initial state of the CTransaction object
	 * @param aTimerValues Values for calculating timer durations
	 * @param aTransactionStore TransactionStore
	 * @param aRetransmitInvite If ETrue and using UDP, INVITE is retransmitted
	 *		  even after receiving a provisional response.
	 * @return New CInviteClientTransaction object, ownership is transferred
	 */
	static CInviteClientTransaction*
		NewL(CUserAgentBase& aUserAgent,
			 CTransmitter& aTransmitter,
			 MTimerManager& aTimers,
			 CTransactionState& aInitialState,
			 TTimerValues& aTimerValues,
			 MTransactionStore& aTransactionStore,
			 TBool aRetransmitInvite);

	~CInviteClientTransaction();

public: // From CClientTransaction:

	void DeleteTimer(const CTransactionTimer& aTimer);

	TBool IsInviteTransaction() const;

public: // New functions

	/**
	 * Receive a response.
	 *	 
	 * @pre aResponse != NULL
	 *
     * @param aResponse SIP response, ownership is transferred
     * @param aCurrent Transaction's current state
     * @param aCompleted Completed state of transaction state machine
     * @param aTimer If non-NULL, this timer is stopped if leave occurs.
     *		  Ownership is not transferred.
     * @param aTimer2 If non-NULL, this timer is stopped if leave occurs.
     *		  Ownership is not transferred.
	 */
	void ReceiveResponseL(CSIPResponse* aResponse,
						  const CTransactionState& aCurrent,                            
                          const CTransactionState& aCompleted,
                          CTransactionTimer* aTimer=NULL,
                          CTransactionTimer* aTimer2=NULL);

	/**
	 * Send ACK created by transaction to network.
	 *
	 * @post iOutgoingMsg = aAck
	 *
	 * @param aAck ACK request, ownership is transferred.	 
	 */	
	void SendAck(CSIPRequest* aAck);

	/**
	 * Starts timer A.
	 *	 
	 * @pre iTimerA = NULL	 
	 */
	void StartTimerAL();

	/**
	 * Starts timer B, unless it is already running.	 
	 */
	void StartTimerBUnlessExistsL();

	/**
	 * Starts timer D, unless it is already running.
	 */
	void StartTimerDUnlessExistsL();

	/**
	 * Starts timer for waiting a final response in the Proceeding state.
	 *	 
	 * @pre iTimerProceeding = NULL
	 *
	 * @return The new timer, ownership is not transferred
	 */
	CTransactionTimer* StartTimerProceedingL();

	/**
	 * Depending on the value of iRetransmitInvite, starts timer for INVITE
	 * retransmissions to keep NAT binding alive.
	 *
	 * @pre iTimerNAT = NULL
	 *
	 * @return The new timer, ownership is not transferred. NULL if the timer
	 *	was not started.
	 */
	CTransactionTimer* StartTimerNATL();

	/**
	 * Stops timer A.
	 *
	 * @post iTimerA = NULL	 
	 */
	void StopTimerA();

	/**
	 * Stops timer B
	 *
	 * @post iTimerB = NULL	 
	 */
	void StopTimerB();

	/**
	 * Stops timer used in Proceeding state.
	 *	 
	 * @post iTimerProceeding = NULL	 
	 */
	void StopTimerProceeding();
	
	/**
	 * Stops NAT binding keepalive timer.
	 *
	 * @post iTimerNAT = NULL	 
	 */
	void StopTimerNAT();

	static CInviteClientTransaction& Ptr(CTransaction& aTransaction);

private: // Constructor, for internal use

	CInviteClientTransaction(CUserAgentBase& aUserAgent,
							 CTransmitter& aTransmitter,
							 MTimerManager& aTimers,
							 CTransactionState& aInitialState,
							 TTimerValues& aTimerValues,
							 MTransactionStore& aTransactionStore,
							 TBool aRetransmitInvite);

private: // New functions, for internal use

	/**
	 * Creates an ACK request and pushes it to CleanupStack.
	 *
	 * @param aResp SIP response for which the ACK is an acknowledgement
	 * @return ACK request, ownership is transferred
	 */	
	CSIPRequest* MakeAckLC(CSIPResponse& aResp) const;

	/**
	 * Copies information from the INVITE into ACK. Copies Request-URI, From,
	 * Call-ID and the top Via header, that has been completely filled by
	 * ConnectionMgr.
	 *
	 * @param aAck ACK request
	 */
	void CopyHeadersFromInviteToAckL(CSIPRequest& aAck) const;	

	/**
	 * Stops timer D
	 *	 
	 * @post iTimerD = NULL
	 */
	void StopTimerD();

	/**
	 * Stops all timers of this transaction
	 *
	 * @post iTimerA = NULL, iTimerB = NULL, iTimerD = NULL	 
	 */
	void CancelAllTimers();

private: // Data

	//Tells if transaction owns the SIP request in CTransaction::iOutgoingMsg.
	//Value is normally EFalse. Only when CInviteClientTransaction (instead of
	//UAC) creates ACK and puts it to CTransaction::iOutgoingMsg, the value is
	//ETrue.
	TBool iTaOwnsRequest;

	//Duration (ms) of timer A
	TUint iTimerADuration;

	//Retransmission timer
	CTimerRetransmit* iTimerA;

	//Transaction timeout timer
	CTimerTerminateTa* iTimerB;

	//Timer used to stop transaction waiting from response retransmissions in
	//the Completed state
	CTimerTerminateTa* iTimerD;

	//Timer used to stop transaction if no final response is received in
	//Proceeding state
	CTimerTerminateTa* iTimerProceeding;

	//If ETrue and using unreliable transport, INVITE is retransmitted even
	//after receiving a provisional response, to keep the NAT binding alive.
	TBool iRetransmitInvite;

	//Retransmission timer to keep NAT binding alive.
	CTimerRetransmit* iTimerNAT;

	//Error code that will be passed while starting the Timer D
	TInt iDTimerErrCode;

private: // For testing purposes

	void __DbgTestInvariant() const;


#ifdef CPPUNIT_TEST
	friend class CTransactionUser_Test;
	friend class CInviteCTa_Calling_Test;
	friend class CInviteCTa_Completed_Test;
#endif
	};

#endif // end of INVITECLIENTTA_H

// End of File
