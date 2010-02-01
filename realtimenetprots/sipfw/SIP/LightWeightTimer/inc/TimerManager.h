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
* Name          : TimerManager.h
* Part of       : LightWeightTimer
* This header file is included by the subsystem which is 
* responsible for creating the CTimerManager instance.
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef TIMERMANAGER_H
#define TIMERMANAGER_H

// INCLUDES
#include <e32base.h>
#include "Lwtimer.h"
#include "mtimernotifier.h"

// FORWARD DECLARATIONS
class CTimerStore;
class CSingleTimer;

// CLASS DECLARATION

/**
 * CTimerManager is the main class of the Lightweight timer subsystem. It is
 * intended to be used in a way that there is only one instance of the
 * CTimerManager class, and the basic timer services are available through the
 * MTimerManager interface.
 *
 * Several logical timers can run simultaneously, but only one RTimer is used.
 */
class CTimerManager :
    public CBase,
	public MTimerNotifier,
	public MTimerManager
	{
public: // Constructors and destructor

	/**
	 * Creates an instance of CTimerManager.
	 *
	 * @see CTimerManager::NewLC
	 *	
	 * @return value The new CTimerManager object. Ownership is transferred.
	 */
	static CTimerManager* NewL();

	/**
	 * Creates an instance of CTimerManager and pushes it to CleanupStack.
	 *
	 * @see CTimerManager::NewL
	 *
	 * @return value The new CTimerManager object. Ownership is transferred.
	 */
	static CTimerManager* NewLC();

    /**
     * Destructor
     */
	~CTimerManager();

public: //From MTimerNotifier

	void TimerExpiredL(TTimerId aTimerId);

	TInt LeaveOccurred();

public: //From MTimerManager	

	TTimerId StartL(MExpirationHandler* aObserver,
				    TUint aMilliseconds);

	TTimerId StartL(MExpirationHandler* aObserver,
					TUint aMilliseconds,
					TAny* aTimerParam);

	TInt Stop(TTimerId aTimerId);
	
	TBool IsRunning(TTimerId aTimerId) const;
	
	TInt ExpiresAfter( TTimerId aTimerId, TUint& aExpiresAfterInMillisecs) const;

private: // Constructors

	CTimerManager();

	/**
	 * Second phase constructor.	 
	 */
	void ConstructL();

private: // For internal use

	/**
	 * Generates a new unique TTimerId value.
	 *	 
	 * @return value New TimerId value
	 */
	TTimerId NewTimerId();

	/**
	 * Set a timer running with the duration of the shortest timer.
	 *
	 * @pre iTimer must not be active	 
	 */
	void SetTimerWithShortestValueL();

	/**
	 * Checks if the recently added new timer is shorter than any of the
	 * existing timers. If it is, the currently running CSingleTimer is
	 * canceled and CSingleTimer is started again, now with the duration of
	 * the new timer.
	 *
	 * @param aTimerId TimerId of the new timer
	 * @param aMilliseconds Duration of the new timer, in milliseconds
	 */
	void CheckIsNewTimerShortestL(TTimerId aTimerId, TUint aMilliseconds);

	/**
	 * Set a dummy timer to simulate a longer timer without the need to use
	 * RTimer::At.
	 */
	void SetInternalTimerL() const;

	/**
	 * Calculates a TTime value by adding the specified amount of time
	 * (aMilliseconds) to current time.	 
	 *
	 * @param aMilliseconds Amount of milliseconds from current time	 
	 * @return value TTime
	 */
	TTime MillisecToTTime(TUint aMilliseconds) const;

	/**
	 * Calculates the amount of milliseconds between current time and aTime and
	 * fills the result into aMilliseconds.
	 *
	 * @param aTime time
	 * @param aMilliseconds Amount of milliseconds from current time	 
	 * @return value ETrue If successful
	 *				 EFalse The duration between aTime and current time is too
	 *						large to fit into TUint.
	 */
	TBool TTimeToMillisec(TTime aTime, TUint& aMilliseconds) const;	

private: // Data

	//Contains information of existing timer requests. Owned.
	CTimerStore* iTimerStore;

	//Timer used by all the timer requests. Owned.
	CSingleTimer* iTimer;

	//Counter used to produce unique TimerId values
	TTimerId iTimerIdCounter;

	//ETrue if CTimerManager is calling MExpirationHandler::TimerExpiredL.
	//Otherwise value is EFalse.
	//This flag is needed as user of CTimerManager might set a new timer within
	//MExpirationHandler::TimerExpiredL, and CTimerManager has to recognize such
	//a situation.
	TBool iNowExpiring;

private: // For testing purposes

	void __DbgTestInvariant() const;
	

#ifdef CPPUNIT_TEST	
	friend class CTimerManager_test;
	friend class CTimerStore_test;
	friend class CTimerRequest_test;
	friend class CSingleTimer_test;
#endif
	};

#endif // end of TIMERMANAGER_H

// End of File
