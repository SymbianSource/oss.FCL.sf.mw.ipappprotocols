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
* Name          : Lwtimer.h
* Part of       : LightWeightTimer
* This header file is included by the subsystems needing timer
* services, but are not responsible for creating the
* CTimerManager instance.
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/


#ifndef LWTIMER_H
#define LWTIMER_H

// INCLUDES
#include <e32def.h>

// TYPEDEFS

//Each timer is associated with an identifier of this type. When a timer is
//startedt, TTimerId is returned to the user.
//When a timer expires, its TTimerId is passed in the callback.
//If the user wishes to stop a certain timer, it must pass the TTimerId of the
// timer in question to the Lightweight timer subsystem.
typedef TUint32 TTimerId;


// CLASS DECLARATION

/**
 * When a timer expires, this interface is called by the Lightweight timer
 * subsystem.
 */
class MExpirationHandler
	{
public: // Virtual destructor

	virtual ~MExpirationHandler() {}

public: // New pure virtual functions

	/**
	 * Indicates that a timer has expired.
	 *
	 * @param aTimerId Identifies the expired timer
	 * @param aTimerParam User specified value which was given when the timer
	 *	was set. It can be used to identify the timer in case multiple timers
	 *  are running simultaneously. Value is NULL if the timer was set using
	 *  CTimerManager::StartL without parameter aTimerParam. Ownership isn't
     *  transferred.
	 */
	virtual void TimerExpiredL(TTimerId aTimerId, TAny* aTimerParam) = 0;
	};


/**
 * MTimerManager provides methods for starting and stopping timers.
 */
class MTimerManager
	{
public: // Enumerations
	enum TTimerIdValues
        {
		//TimerId value for a non-existent timer. This value is never returned
        //to the user of the Lightweight timer subsystem.
		KNoSuchTimer = 0,

		//TimerId for an internal dummy timer, which is used simulate a longer
        //timer without having to use RTimer::At(). This value is never
        //returned to the user of the Lightweight timer subsystem.
		KReservedTimer = 1
	    };

public: // New pure virtual functions

	/**
	 * Creates a new timer and starts it.
	 *
	 * @pre aObserver != NULL	 
	 *
	 * @see See also this method 'MExpirationHandler::TimerExpiredL'
	 *
	 * @param aObserver	IN: Callback to use when timer expires. Ownership is
	 *	not transferred.
	 * @param aMilliseconds	Timer duration in milliseconds (with 32 bits, the
	 *	max value is ~50 days)
	 * @return value TimerId value identifying the new timer	 
	 */
	virtual TTimerId StartL(MExpirationHandler* aObserver,
							TUint aMilliseconds) = 0;
	
	/**
	 * Creates a new timer and starts it.
	 *
	 * @pre aObserver != NULL	 
	 *
	 * @see See also this method 'MExpirationHandler::TimerExpiredL'
	 *
	 * @param aObserver	IN: Callback to use when timer expires. Ownership is
	 *	not transferred.
	 * @param aMilliseconds	Timer duration in milliseconds (with 32 bits, the
	 *	max value is ~50 days)
	 * @param aTimerParam User specified value which will be passed to the
	 *	aObserver function MExpirationHandler::TimerExpiredL when the timer
	 *	expires. Ownership isn't transferred.
	 * @return value TimerId value identifying the new timer
	 */
	virtual TTimerId StartL(MExpirationHandler* aObserver,
						    TUint aMilliseconds,
						    TAny* aTimerParam) = 0;

	/**
	 * Stops the specified timer.
	 *
	 * This function doesn't leave in case of error, as it is thought that this
	 * function is usually also called from destructors.
	 *
	 * @param aTimerId Identifies the timer to be stopped	 
	 * @return value KErrNone: successful
     *               KErrNotFound : no such timer exists
	 */	
	virtual TInt Stop(TTimerId aTimerId) = 0;

	/**
	 * Checks if there exists a timer which hasn't expired yet, with the
	 * specified TimerId.
	 *
	 * @param aTimerId Identifies the timer	 
	 * @return value ETrue: timer exists, KFalse: no such timer
	 */	
	virtual TBool IsRunning(TTimerId aTimerId) const = 0;
	
	/**
	 * Checks how long it will take until the timer expires, with the
	 * specified TimerId.
	 *
	 * @param aTimerId Identifies the timer	 
	 * @param aExpiresAfterInMillisecs on return contains expires after
	 *  value as milliseconds
	 * @return value KErrNone: successful
     *               KErrNotFound : no such timer exists
	 */	
	virtual TInt ExpiresAfter(TTimerId aTimerId, 
	                          TUint& aExpiresAfterInMillisecs) const = 0;
	};

#endif // end of LWTIMER_H

// End of File
