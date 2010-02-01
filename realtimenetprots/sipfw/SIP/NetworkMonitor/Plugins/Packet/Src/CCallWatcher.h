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
* Name        : CCallWatcher.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/
#ifndef __CCALLWATCHER_H__
#define __CCALLWATCHER_H__

// INCLUDES
#include <etel.h>

// FORWARD DECLARATIONS
class CLineMonitor;

// CLASS DEFINITION
/**
 *
 */
class CCallWatcher
	: public CActive
	{
	public:	// Constructors and destructor

		/**
		 * Static constructor. Leaves pointer to cleanup stack.
		 *
		 * @return An initialized instance of this class.
		 */
		static CCallWatcher* NewLC( CLineMonitor& aParent,
								    RLine& aLine,
									TDesC& aCallName );

		/// Destructor
		~CCallWatcher();

	public: // New methods

		TBool IsIdle() const;

	protected: // From CActive
	
		void DoCancel();
		void RunL();

	private: // Constructors

		/// Constructor
		CCallWatcher( CLineMonitor& aParent );

		/// Default constructor. Not implemented.
		CCallWatcher();

		/// 2nd phase constructor
		void ConstructL( RLine& aLine, TDesC& aCallName );

	private: // Data

		CLineMonitor& iParent;
		RCall iCall;
		RCall::TStatus iCallStatus;
	};

#endif //__CCALLWATCHER_H__
