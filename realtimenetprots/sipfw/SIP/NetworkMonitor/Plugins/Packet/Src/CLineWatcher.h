/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : CLineWatcher.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/
#ifndef __CLINEWATCHER_H__
#define __CLINEWATCHER_H__

// INCLUDES
#include <etel.h>

// FORWARD DECLARATIONS
class CLineMonitor;
class CCallWatcher;

// CLASS DEFINITION
/**
 *
 */
class CLineWatcher
	: public CBase
	{
	public:	// Constructors and destructor

		/**
		 * Static constructor. Leaves pointer to cleanup stack.
		 *
		 * @return An initialized instance of this class.
		 */
		static CLineWatcher* NewLC( CLineMonitor& aParent,
								    RPhone& aPhone, TDesC& aLineName );

		/// Destructor
		~CLineWatcher();

	public: // New methods

		TBool IsLineIdle() const;

	private: // Constructors

		/// Constructor
		CLineWatcher( CLineMonitor& aParent );

		/// Default constructor. Not implemented.
		CLineWatcher();

		/// 2nd phase constructor
		void ConstructL( RPhone& aPhone, TDesC& aLineName );

	private: // Data

		RLine iLine;
		CLineMonitor& iParent;
		RPointerArray<CCallWatcher> iCallWatchers;

	};

#endif // __CLINEWATCHER_H__
