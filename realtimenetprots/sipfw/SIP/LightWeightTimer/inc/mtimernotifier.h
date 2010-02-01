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
* Name          : mtimernotifier.h
* Part of       : LightWeightTimer
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef M_TIMERNOTIFIER_H
#define M_TIMERNOTIFIER_H

// INCLUDES
#include <e32base.h>
#include "Lwtimer.h"

// CLASS DECLARATION

/**
 * Defines how CSingleTimer notifies about its expiration.
 */
class MTimerNotifier
	{
public: // Virtual destructor

	virtual ~MTimerNotifier() {}

public: // New pure virtual functions

	/**
	 * Indicates the CSingleTimer has expired.
	 *
	 * @param aTimerId Identifies the timer which expired	 
	 */
	virtual void TimerExpiredL(TTimerId aTimerId) = 0;

	/**
	 * Called when leave occurred in TimerExpiredL.
	 *
	 * @return value KErrNone if it is possible to continue, otherwise a
	 *	system wide error code.
	 */
	virtual TInt LeaveOccurred() = 0;
	};

#endif // M_TIMERNOTIFIER_H
