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
* Name          : TimerValueRetriever.h
* Part of       : TransactionUser
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef TIMERVALUERETRIEVER_H
#define TIMERVALUERETRIEVER_H

// INCLUDES
#include <e32base.h>
#include "TimerValues.h"

// FORWARD DECLARATIONS
class CRepository;

// CLASS DECLARATION

/**
 * This class reads timer values from Central Repository, and listens ifi they
 * are changed.
 */
class CTimerValueRetriever : public CActive
	{
public: //enum
		/**
		* Bearer types for which specific default registration expiry durations are supported
		*/
	enum BearerType
		{
		EModemBearer = 0,
		ELanBearer,
		EMaxSupportedBearer
		};
public: // Constructors and destructor

	/**
	 * Creates a CTimerValueRetriever object.
	 *
	 * @return value New CTimerValueRetriever object, ownership is transferred.
	 */
	static CTimerValueRetriever* NewL();

	~CTimerValueRetriever();

public: // From CActive

	void DoCancel();

	void RunL();

	TInt RunError(TInt aError);

public: // New functions

	TTimerValues& TimerValues(BearerType aType);
	
	/**
	 * Returns the highest T1 value of all the configured timer values
	 */
	TUint HighestT1();

private: // New functions, for internal use

	CTimerValueRetriever();

	void ConstructL();

    /**
	 * Reads the timer values from repository. If they are not found, function
     * leaves.
	 */
    void ReadValuesL();    

	void ListenForChangesL();

private: // Data

	// Provides access to Central Repository, owned.
	CRepository* iRepository;

	TTimerValues iTimerValues[EMaxSupportedBearer];

	// For testing purposes	

#ifdef CPPUNIT_TEST	
	friend class CTimerValueRetriever_Test;	
#endif	
	};

#endif // end of TIMERVALUERETRIEVER_H

// End of File
