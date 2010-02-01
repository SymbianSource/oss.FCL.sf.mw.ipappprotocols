/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : UserAgentTimer.h
* Part of       : TransactionUser
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

// INCLUDES
#ifndef USERAGENTTIMER_H
#define USERAGENTTIMER_H

#include "TimerBase.h"

// FORWARD DECLARATIONS
class CUserAgent;


// CLASS DECLARATION

class CUserAgentTimer : public CTimerBase
	{
public:
	virtual ~CUserAgentTimer() {}


	/**
	 * Handle the timer expiration.
	 *	 
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @param aUserAgent UserAgent whose timer expired. Ownership is not
	 *	transferred.
	 * @param aTimerId Timer identifier
	 * @return value -
	 */	
	virtual void ExpiredL(CUserAgent* aUserAgent, TTimerId aTimerId) = 0;

protected:
	/**
	 * Creates a timer and starts it.
	 *	 
	 * Note: since there isn't a StartL() function in the CUserAgentTimer,
	 * then if a timer is stopped, it can't be restarted.
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @param aTimerMgr Timer subsystem
	 * @param aObserver Callback for handling timer expiration. Ownership is
     *  not transferred.
	 * @param aDuration Timer duration in milliseconds
	 * @return value -
	 */
	CUserAgentTimer(MTimerManager& aTimerMgr,
				    MExpirationHandler* aObserver,
				    TUint32 aDuration);
	};



/*
 * This timer is used for stopping UserAgent. When the timer expires, UserAgent
 * is stopped with the given aReason value.
 */
class CTimerStopUA : public CUserAgentTimer
	{
public:
	//Creates and starts the timer
	static CTimerStopUA* NewL(MTimerManager& aTimerMgr,
							  MExpirationHandler* aObserver,
							  TUint32 aDuration,
							  TInt aReason);
	void ExpiredL(CUserAgent* aUserAgent, TTimerId aTimerId);

private:
	CTimerStopUA(MTimerManager& aTimerMgr,
				 MExpirationHandler* aObserver,
				 TUint32 aDuration,
				 TInt aReason);

	//Reason value which is passed to CUserAgent::Stop() when the timer expires
	TInt iReason;
	};



/*
 * InviteUAS uses this timer for retransmitting the 2xx response until it
 * receives ACK.
 */
class CTimerRetransmit2xx : public CUserAgentTimer
	{
public:
	//Creates and starts the timer
	static CTimerRetransmit2xx* NewL(MTimerManager& aTimerMgr,
									 MExpirationHandler* aObserver,
									 TUint32 aDuration);

	void ExpiredL(CUserAgent* aUserAgent, TTimerId aTimerId);

private:
	CTimerRetransmit2xx(MTimerManager& aTimerMgr,
						MExpirationHandler* aObserver,
						TUint32 aDuration);	
	};



/*
 * UAS uses this timer for send error response to remote endpoint when
 * AssocMgr does not return MTransactionOwner of response in time.
 */
class CTimerOwnerResolver : public CUserAgentTimer
	{
public:
	//Creates and starts the timer
	static CTimerOwnerResolver* NewL(MTimerManager& aTimerMgr,
									 MExpirationHandler* aObserver,
									 TUint32 aDuration);

	void ExpiredL(CUserAgent* aUserAgent, TTimerId aTimerId);

private:
	CTimerOwnerResolver(MTimerManager& aTimerMgr,
					    MExpirationHandler* aObserver,
					    TUint32 aDuration);	
	};



/*
 * InviteUAC uses this timer when SIPSec processed the received SIP response
 * synchronously.
 * SIP stack should send the the ACK to the error response before the updated
 * INVITE is sent. Using a dummy timer with duration zero, introduces an
 * asynchronous break before UAC processes the SIP response further, causing
 * the control to return back to the transaction which received the response,
 * before InviteUAC may create a new transaction for sending the updated INVITE.
 * This way the old transaction sends the ACK before the new transaction sends
 * the INVITE.
 */
class CTimerAsyncSIPSec : public CUserAgentTimer
	{
public:
	//Creates and starts the timer
	static CTimerAsyncSIPSec* NewL(MTimerManager& aTimerMgr,
							       MExpirationHandler* aObserver);

	void ExpiredL(CUserAgent* aUserAgent, TTimerId aTimerId);

private:
	CTimerAsyncSIPSec(MTimerManager& aTimerMgr,
					  MExpirationHandler* aObserver);
	};

#endif // end of USERAGENTTIMER_H

// End of File
