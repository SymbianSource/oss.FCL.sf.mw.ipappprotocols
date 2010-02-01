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
* Name          : NormalServerTa.h
* Part of       : Transaction
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef NORMALSERVERTA_H
#define NORMALSERVERTA_H

// INCLUDES
#include "ServerTransaction.h"

// FORWARD DECLARATIONS
class CTimerTerminateTa;
class CTimerSend100;

// CLASS DECLARATION
/*
 * This class represents a non-INVITE server transaction. 
 */
class CNormalServerTransaction : public CServerTransaction
	{
public: // Constructors and destructor

	/**		
	 * Creates a new instance of CNormalServerTransaction
	 *
	 * @param aUserAgent UserAgent owning the new transaction being created.
	 * @param aTransmitter CTransmitter instance to use
	 * @param aTimers Timer interface
	 * @param aInitialState Initial state of this transaction	 
	 * @param aTimerValues Values for calculating timer durations
	 * @return New CNormalServerTransaction object, ownership is transferred
	 */
	static CNormalServerTransaction* NewL(CUserAgentBase& aUserAgent,
										  CTransmitter& aTransmitter,
										  MTimerManager& aTimers,
										  CTransactionState& aInitialState,
										  TTimerValues& aTimerValues);

	~CNormalServerTransaction();	

public: // From CServerTransaction

	void DeleteTimer(const CTransactionTimer& aTimer);

public: // New functions	

	TBool IsTimerSend100Running() const;

	/**
	 * Starts the timer F2, unless it is already running.
	 */
	void StartTimerF2UnlessExistsL();

	/**
	 * Starts the timer J, unless it is already running.	 
	 */
	void StartTimerJUnlessExistsL();

	/**
	 * Starts the timer for sending a 100 response
	 *
	 * @pre iTimerSend100 = NULL
	 * @param aRequest SIP Request received from network
	 */
	void StartTimerSend100L(CSIPRequest& aRequest);

	/**
	 * Stops the timer F2.
	 *
	 * @post iTimerF2 = NULL
	 */	
	void StopTimerF2();

	/**
	 * Stops the timer for sending a 100 response
	 *
	 * @post iTimerSend100 = NULL
	 */
	void StopTimerSend100();

	/**
	 * A retransmitted SIP request is received.
	 *
	 * @pre aRequest = NULL
	 * @param aRequest SIP request, ownership is transferred.	 
	 */
	void ReceiveRetransmissionL(CSIPRequest* aRequest);

	static CNormalServerTransaction& Ptr(CTransaction& aTransaction);

private: // Constructor, for internal use

	/**	
	 * @param aUserAgent UserAgent owning this transaction object
	 * @param aTransmitter CTransmitter instance to use
	 * @param aTimers Timer interface
	 * @param aInitialState Initial state of this transaction	 
	 * @param aTimerValues Values for calculating timer durations
	 */
	CNormalServerTransaction(CUserAgentBase& aUserAgent,
							 CTransmitter& aTransmitter,
							 MTimerManager& aTimers,
							 CTransactionState& aInitialState,
							 TTimerValues& aTimerValues);

private: // New functions, for internal use

	/**
	 * Stops timer J
	 *
	 * @post iTimerJ = NULL
	 */
	void StopTimerJ();

    /**
	 * Stops all timers of this transaction
	 *
	 * @post iTimerF2 = NULL, iTimerJ = NULL, iTimerSend100 = NULL
	 */
	void CancelAllTimers();

private: // Data

    // This timer is started when transaction receives the initial request and
    // stopped when a final response is sent. If this timer fires, transaction
    // enters terminated state. Owned.
	CTimerTerminateTa* iTimerF2;

	// Timer J, owned
	CTimerTerminateTa* iTimerJ;

	// Timer to send a 100 response unless application sends a response. Owned.
	CTimerSend100* iTimerSend100;

	void __DbgTestInvariant() const;


#ifdef CPPUNIT_TEST
	friend class CNormalServerTransaction_Test;
	friend class CServerTransaction_Test;
	friend class CNormalSTa_Trying_Test;
	friend class CNormalSTa_Proceeding_Test;
	friend class CNormalSTa_Completed_Test;	
	friend class CTransactionUser_Test;
#endif
	};

#endif // end of NORMALSERVERTA_H

// End of File
