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
* Name          : timerstore.h
* Part of       : LightWeightTimer
* Private header file for the Lightweight timer subsystem.
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef C_TIMERSTORE_H
#define C_TIMERSTORE_H

// INCLUDES
#include <e32base.h>
#include "Lwtimer.h"

// FORWARD DECLARATIONS
class CTimerRequest;

// CLASS DECLARATION
/*
 * This class manages a sorted array containing all existing timer requests
 */
class CTimerStore : public CBase
	{
public: // Constructor and destructor

	/**
	 * A new CTimerStore instance is created.
	 *
	 * @return new CTimerStore object, ownership is transferred.
	 */
	static CTimerStore* NewL();

	~CTimerStore();

public: // New functions

	/**
	 * Adds a new timer into the store.
	 *
	 * @pre	aObserver != NULL
	 *
	 * @param aObserver When the timer expires, this callback is used.
	 *	Ownership isn't transferred.
	 * @param aExpirationTime Time when the timer expires
	 * @param aTimerId TimerId of the timer
	 * @param aTimerParam User specified value which is passed to aObserver
	 *	when the timer expires. Ownership isn't transferred.	 
	 */
	void AddL(MExpirationHandler* aObserver,
			  TTime aExpirationTime,
			  TTimerId aTimerId,
			  TAny* aTimerParam);

	/**
	 * Searches for a timer with the given TimerId.
	 *
	 * @param aTimerId TimerId of the timer to be searched
	 * @param aIndex Position of the found timer in the store	 
	 * @return value Pointer to the found CTimerRequest containing timer's
	 *	data. NULL if no such timer was found.
	 *  The found CTimerRequest object is not removed from the store, and its
	 *  ownership isn't transferred.
	 */
	CTimerRequest* SearchById(TTimerId aTimerId, TInt& aIndex) const;

	/**
	 * Obtains the TimerId and expiration time of the next expiring timer.
	 * The shortest timer is located in the first item. In case several timers
	 * would expire at the same time, the information of one of them is
	 * returned. The timer is not removed from the store.
	 *
	 * @param aTimerId OUT: TimerId of the timer with shortest duration
	 * @param aExpirationTime OUT: Expiration time of the timer with shortest
	 *	duration
	 * @return value ETrue if at least one timer exists and its information was
	 *	filled to aTimerId and aExpirationTime.
	 *	EFalse in case there are no timers running.	 
	 */
	TBool ShortestTimer(TTimerId& aTimerId, TTime& aExpirationTime) const;

	/**
	 * Removes the timer request at the specified position from the store and
	 * deletes its information.
	 *
	 * @param aIndex Position in the store, of the timer request to be removed	 
	 * @return value ETrue if successful, EFalse if there were no timer request
	 *	in the specified position
	 */
	TBool Remove(TInt aIndex);

private: // Constructors

	CTimerStore();

	void ConstructL();

private: // Data

	//Used for searching a stored request by TimerId. Owned.
	CTimerRequest* iSearchCond;

	//Array containing the timer requests, sorted in order of expiration time.
	//Shortest timer is first, longest timer last. If more than one timer
	//has the same expiration time, the order in which they are stored is
	//undefined. The items in the array are owned.
	RPointerArray<CTimerRequest> iTimerRequests;

private: // For testing purposes

#ifdef CPPUNIT_TEST	
	friend class CTimerManager_test;
	friend class CTimerStore_test;
#endif
	};

#endif // C_TIMERSTORE_H

// End of File
