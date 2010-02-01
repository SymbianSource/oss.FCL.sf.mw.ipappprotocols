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
* Name          : NormalClientTa.h
* Part of       : Transaction
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef NORMALCLIENTTA_H
#define NORMALCLIENTTA_H

// INCLUDES
#include "ClientTransaction.h"

// FORWARD DECLARATIONS
class CTimerRetransmit;
class CTimerTerminateTa;


// CLASS DECLARATION
/*
 * This class is used for sending a non-INVITE (and non-ACK) request, and
 * receiving the responses.
 */
class CNormalClientTransaction : public CClientTransaction
	{
public: // Constructors and destructor

	/**
	 * Creates a new instance of CNormalClientTransaction
	 *
	 * @param aUserAgent UserAgent owning the new transaction being created.
	 * @param aTransmitter CTransmitter used for sending messages
	 * @param aTimers LwTimer subsystem
	 * @param aInitialState Initial state of the CTransaction object
	 * @param aTimerValues Values for calculating timer durations
	 * @param aTransactionStore TransactionStore
	 * @return CNormalClientTransaction object, ownership is transferred
	 */
	static CNormalClientTransaction* NewL(CUserAgentBase& aUserAgent,
									  CTransmitter& aTransmitter,
									  MTimerManager& aTimers,
									  CTransactionState& aInitialState,
									  TTimerValues& aTimerValues,
									  MTransactionStore& aTransactionStore);

	~CNormalClientTransaction();

public: // From CClientTransaction

	void DeleteTimer(const CTransactionTimer& aTimer);
	
public: // New functions

	/**
	 * Sets the duration used for timer E to maximum (=T2).
	 *	 
	 * @pre iTimerE = NULL
	 */		
	void IncreaseTimerEtoT2();

	/**
	 * Starts timer E.
	 *	 
	 * @pre iTimerE = NULL
	 */
	void StartTimerEL();
	
	/**
	 * Starts timer F, unless it has already been started.	 
	 */
	void StartTimerFUnlessExistsL();	

	static CNormalClientTransaction& Ptr(CTransaction& aTransaction);

    /**
	 * Receive a SIP response.
	 *	 
	 * @pre aResponse != NULL
	 *
     * @param aResponse SIP response, ownership is transferred
     * @param aCurrentState Transaction's current state
     * @param aProceeding Proceeding state of transaction state machine
     * @param aCompleted Completed state of transaction state machine
	 * @return
	 */
    void ReceiveResponseL(CSIPResponse* aResponse,
                          const CTransactionState& aCurrentState,
                          const CTransactionState& aProceeding,
                          const CTransactionState& aCompleted);

private: // Constructor, for internal use

	CNormalClientTransaction(CUserAgentBase& aUserAgent,
							 CTransmitter& aTransmitter,
							 MTimerManager& aTimers,
							 CTransactionState& aInitialState,
							 TTimerValues& aTimerValues,
							 MTransactionStore& aTransactionStore);

private: // New functions, for internal use

	/**
	 * Starts timer K.
	 *
	 * @pre iTimerK = NULL
	 */
	void StartTimerKL();

    /**
	 * Stops timer E
	 *	
	 * @post iTimerE = NULL	 
	 */
	void StopTimerE();

    /**
	 * Stops timer F
	 *
	 * @post iTimerF = NULL
	 */
	void StopTimerF();

    /**
	 * Stops timer K
	 *	 
	 * @post iTimerK = NULL	 
	 */
	void StopTimerK();

    /**
	 * Stops all timers of this transaction
	 *
	 * @post iTimerE = NULL, iTimerF = NULL, iTimerK = NULL	 
	 */
	void CancelAllTimers();

private: // Data

	//Duration (ms) of timer E
	TUint iTimerEDuration;

	//Timer E, owned
	CTimerRetransmit* iTimerE;
	//Timer F, owned
	CTimerTerminateTa* iTimerF;
	//Timer K, owned
	CTimerTerminateTa* iTimerK;

private: // For testing purposes

	void __DbgTestInvariant() const;



#ifdef CPPUNIT_TEST
	friend class CTransactionUser_Test;
	friend class CNormalCTa_Trying_Test;
	friend class CNormalCTa_Proceeding_Test;
	friend class CNormalCTa_Completed_Test;
	friend class ClientTransaction_Test;
#endif
	};

#endif // end of NORMALCLIENTTA_H

// End of File
