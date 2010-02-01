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
* Name          : InviteServerTa.h
* Part of       : Transaction
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef INVITESERVERTA_H
#define INVITESERVERTA_H

// INCLUDES
#include "ServerTransaction.h"

// FORWARD DECLARATIONS
class CTimerTerminateTa;
class CTimerRetransmit;

// CLASS DECLARATION
/*
 * This class represents an INVITE server transaction. 
 */
class CInviteServerTransaction : public CServerTransaction
	{
public: // Constructors and destructor

	/**
	 * Creates a new instance of CInviteServerTransaction
	 *
	 * @param aUserAgent UserAgent owning the new transaction being created.
	 * @param aTransmitter CTransmitter used for sending messages
	 * @param aTimers LwTimer subsystem
	 * @param aInitialState Initial state of the CTransaction object
	 * @param aTimerValues Values for calculating timer durations	 
	 * @return New CInviteServerTransaction object, ownership is transferred
	 */
	static CInviteServerTransaction* NewL(CUserAgentBase& aUserAgent,
										  CTransmitter& aTransmitter,
										  MTimerManager& aTimers,
										  CTransactionState& aInitialState,
										  TTimerValues& aTimerValues);

	~CInviteServerTransaction();

public: // From CServerTransaction

	/**
	 * InviteServerTransaction enters terminated state. If the
	 * InviteServerTransaction has been given a 2xx response to send, it must
	 * clear any pointers to the 2xx since InviteUAS owns the 2xx.
	 *
	 * @param aReason Reason why the InviteServerTransaction ends	 
	 */
	void TerminatedL(TInt aReason);

	void DeleteTimer(const CTransactionTimer& aTimer);

	TBool IsInviteTransaction() const;

public: // New functions	

	/**	 
	 * A SIP response has been sent to network. Depending on the response code,
	 * transaction enters another state.
	 *
	 * @param aRespCode Response code of the received SIP response
	 * @param aSending2xx State where CTransaction moves, if aRespCode is
     *  200-299
	 * @param aCompleted State where CTransaction moves, if aRespCode is >= 300	 
	 */
	void ResponseSent(TUint aRespCode,
					  CTransactionState& aSending2xx,
					  CTransactionState& aCompleted);

	/**
	 * Transaction enters Confirmed state. Stop timers G and H and start
	 * timer I.	 
	 *
	 * @param aConfirmed Confirmed state
	 */
	void EnterConfirmedStateL(CTransactionState& aConfirmed);

	/**
	 * When CInviteServerTransaction ends after sending the first 2xx,
	 * InviteUAS starts retransmitting the 2xx. The pointers to 2xx from
	 * CInviteServerTransaction are cleared.
	 */
	void Leave2xxForUAS();

	/**
	 * Starts timer G, unless it is already running.
	 */
	void StartTimerGUnlessExistsL();

	/**
	 * Starts timer H.
	 *
	 * @pre iTimerH = NULL	 
	 */
	void StartTimerHL();

	static CInviteServerTransaction& Ptr(CTransactionBase& aTransaction);

private: // Constructors, for internal use

	CInviteServerTransaction(CUserAgentBase& aUserAgent,
							 CTransmitter& aTransmitter,
							 MTimerManager& aTimers,
							 CTransactionState& aInitialState,
							 TTimerValues& aTimerValues);

	void ConstructL();

private: // New functions, for internal use

	void FreeResponsesFromQueues() const;

	void StartTimerIL();

	void StopTimerG();
	void StopTimerH();
	void StopTimerI();	

	void CancelAllTimers();

private: // Data

	//Duration (ms) for timer G
	TUint iTimerGDuration;

	//Timer G, owned.
	CTimerRetransmit* iTimerG;
	//Timer H, owned.
	CTimerTerminateTa* iTimerH;
	//Timer I, owned.
	CTimerTerminateTa* iTimerI;	

	void __DbgTestInvariant() const;


#ifdef CPPUNIT_TEST
	friend class CTransactionUser_Test;
	friend class CInviteServerTa_Test;
#endif
	};

#endif // end of INVITESERVERTA_H

// End of File
