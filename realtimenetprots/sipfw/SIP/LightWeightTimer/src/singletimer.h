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
* Name          : singletimer.h
* Part of       : LightWeightTimer
* Private header file for the Lightweight timer subsystem.
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef C_SINGLETIMER_H
#define C_SINGLETIMER_H

// INCLUDES
#include <e32base.h>
#include "Lwtimer.h"

// FORWARD DECLARATIONS
class MTimerNotifier;

// CLASS DECLARATION
/*
 * The only Timer object used by the timer subsystem.
 */
class CSingleTimer : public CActive
	{
public: // Enumerations

    enum TTimerDuration
        {
		//Longest time (in milliseconds) that can be used with RTimer::After
		KMaxTimerAfterDuration = 2146500,

		//Duration of the internal timer, in milliseconds
        KInternalTimerDuration = KMaxTimerAfterDuration - 5000
        };

public: // Constructors and destructor

	/**
	 * A new timer instance is created.
	 *
	 * @param aObserver When the timer expires, this callbak is used.
	 * @return new CSingleTimer object, ownership is transferred.
	 */
	static CSingleTimer* NewL(MTimerNotifier& aObserver);

	~CSingleTimer();

public: // From CActive

	void DoCancel();
	
	void RunL();
	
	TInt RunError(TInt aError);	

public: // New functions

	/**
	 * Set timer to expired after the specified amount of time (aDelay)
	 *
	 * @param aDelay Duration in milliseconds
	 * @param aTimerId TimerId for the timer	 
	 */
	void SetAfter(TUint32 aDelay, TTimerId aTimerId);	

	/**
	 * Obtain the TimerId of the currently running timer.
	 *	 
	 * @return TimerId of the currently running timer.
	 */
	TTimerId TimerId() const;
	
private: // Enumerations

	//These are estimates how much delay the RTimer::After has in different
	//platforms. Unit is milliseconds.
	enum TTimerDelay
		{
		KEmulatorDelay = 100,
		KTargetHwDelay = 15
		};

private: // Constructors

	CSingleTimer(MTimerNotifier& aObserver);

	void ConstructL();

private: // For internal use

	/**
	 * Makes the CSingleTimer expire as soon as possible. Sets CSingleTimer
	 * active and immediately completes the request, without setting the RTimer
	 * at all. This causes active scheduler to call CSingleTimer::RunL much
	 * quicker than by using RTimer::After(0) which would take at least the
	 * platform dependent delay.
	 */
	void ExpireImmediately();

private: // Data

	//When the timer expires, it is notified using this observer
	MTimerNotifier& iObserver;

	//Id that is passed in the callback function when timer expires.
	//User of CSingleTimer can use this value for its own purposes.
	TTimerId iTimerId;

	//Timer service provider
	RTimer iTimer;

private: // For testing purposes

	void __DbgTestInvariant() const;


#ifdef CPPUNIT_TEST	
	friend class CSingleTimer_test;
#endif
	};

#endif // C_SINGLETIMER_H

// End of File
