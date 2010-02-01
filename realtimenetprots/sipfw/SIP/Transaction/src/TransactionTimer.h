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
* Name          : TransactionTimer.h
* Part of       : Transaction
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef TRANSACTIONTIMER_H
#define TRANSACTIONTIMER_H

// INCLUDES
#include "TimerBase.h"
#include <stringpool.h>

// FORWARD DECLARATIONS
class CSIPResponse;
class TSIPTransportParams;
class CTransaction;

// CLASS DECLARATION

class CTransactionTimer : public CTimerBase
	{
public: // Destructor

	virtual ~CTransactionTimer() {}

public: // New functions

	/**
	 * Handle the timer expiration.
	 *
	 * @param aTransaction Transaction whose timer expired
	 * @param aTimerId Timer identifier
	 */
	virtual void ExpiredL(CTransaction& aTransaction, TTimerId aTimerId) = 0;

protected: // Constructor

	/**
	 * Creates a timer and starts it. Since there is no StartL() function in
	 * the CTransactionTimer, if a timer is stopped, it can't be restarted.
	 *
	 * @pre aObserver != NULL
	 *
	 * @param aTimerMgr Timer subsystem
	 * @param aObserver Callback for handling timer expiration. Ownership isn't
	 *	transferred.
	 * @param aDuration Timer duration, in milliseconds
	 */
	CTransactionTimer(MTimerManager& aTimerMgr,
					  MExpirationHandler* aObserver,
					  TUint32 aDuration);
	};



/*
 * When this timer expires, client transaction retransmits the previously sent
 * SIP request.
 */
class CTimerRetransmit : public CTransactionTimer
	{
public: // Enumerations

	enum TRetransmitTimerDuration
		{
		// Retransmission interval for sending INVITE using UDP to keep a NAT
		// binding alive. Unit is milliseconds.
		KNATBindingInterval = 28000
		};

public: // Constructor

	static CTimerRetransmit* NewL(MTimerManager& aTimerMgr,
								  MExpirationHandler* aObserver,
								  TUint32 aDuration);

public: // From CTransactionTimer

	void ExpiredL(CTransaction& aTransaction, TTimerId aTimerId);

private: // Constructor, for internal use

	CTimerRetransmit(MTimerManager& aTimerMgr,
					 MExpirationHandler* aObserver,
					 TUint32 aDuration);
	};



/*
 * When this timer expires, transaction will terminate.
 */
class CTimerTerminateTa : public CTransactionTimer
	{
public: // Enumerations

	enum TTransactionTimerDuration
		{
		// How long INVITE client transaction waits in proceeding state.
		// Unit is milliseconds.
		KProceedingTimerDuration = 300000
		};

public: // Constructor

	static CTimerTerminateTa* NewL(MTimerManager& aTimerMgr,
								   MExpirationHandler* aObserver,
								   TUint32 aDuration,
								   TInt aReason = KErrNone);

public: // From CTransactionTimer

	void ExpiredL(CTransaction& aTransaction, TTimerId aTimerId);

private: // Constructor, for internal use

	CTimerTerminateTa(MTimerManager& aTimerMgr,
					  MExpirationHandler* aObserver,
					  TUint32 aDuration,
					  TInt aReason);

private: // Data

	//Reason value which is passed to CTransaction::TerminatedL() when the
	//timer expires
	TInt iReason;
	};



/*
 * When this timer expires, server transaction sends a 100 response.
 */
class CTimerSend100 : public CTransactionTimer
	{
public: // Constructor and destructor

	/**
	 * Creates the timer and starts it timer.
	 *
	 * @pre aObserver != NULL
	 * @pre aResponse != NULL
	 *
	 * @param aTimerMgr Timer subsystem
	 * @param aObserver Callback handling the timer expiration, ownership is
	 *		  not transferred.
	 * @param aDuration Duration until timer expires.
	 * @param aResponse SIP response, ownership is transferred.
	 * @param aTransportProtocol Transport protocol to use when sending the
	 *		  response.
	 * @param aTransportParams Parameters to use when sending the response.
	 */
	static CTimerSend100* NewL(MTimerManager& aTimerMgr,
							   MExpirationHandler* aObserver,
							   TUint32 aDuration,
							   CSIPResponse* aResponse,
							   RStringF aTransportProtocol,
							   TSIPTransportParams& aTransportParams);

	~CTimerSend100();

public: // From CTransactionTimer

	void ExpiredL(CTransaction& aTransaction, TTimerId aTimerId);

private: // Constructor, for internal use

	CTimerSend100(MTimerManager& aTimerMgr,
				  MExpirationHandler* aObserver,
				  TUint32 aDuration,				  
			      RStringF aTransportProtocol,
			      TSIPTransportParams& aTransportParams);

private: // Data

	// Owned
	CSIPResponse* iResponse;
	RStringF iTransportProtocol;
	TSIPTransportParams& iTransportParams;

#ifdef CPPUNIT_TEST
	friend class CTransactionUser_Test;
#endif
	};

#endif // end of TRANSACTIONTIMER_H

// End of File
