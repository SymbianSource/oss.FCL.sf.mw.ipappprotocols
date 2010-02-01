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
* Name        : CLineMonitor.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/
#ifndef __CLINEMONITOR_H__
#define __CLINEMONITOR_H__

// INCLUDES
#include <etelmm.h>
#include <etelpckt.h>
#include "sipnetworkobserver.h"

// FORWARD DECLARATIONS
class CLineWatcher;
class MSIPBearerMonitorObserver;

// CLASS DEFINITION
/**
 *
 */
class CLineMonitor
	: public CBase
	{
	public:	// Constructors and destructor

		/**
		 * Static constructor.
		 *
		 * @return An initialized instance of this class.
		 */
		static CLineMonitor* NewL( MSIPBearerMonitorObserver& aObserver );

		/// Destructor
		~CLineMonitor();

	public: // New methods

		TBool AllLinesIdle() const;
		void WatcherStateChanged( TInt aError );

	private: // Constructors

		/// Constructor
		CLineMonitor( MSIPBearerMonitorObserver& aObserver );
		
		/// Default constructor, not implemented.
		CLineMonitor();

		/// 2nd phase constructor
		void ConstructL();

	private: // Data

		MSIPBearerMonitorObserver& iObserver;
		RTelServer iTelServer;
		RMobilePhone iPhone;
		RPacketService iPacketService;
		RPacketService::TStatus iPacketServiceStatus;
		CArrayPtrFlat< CLineWatcher > iLineWatchers;
		TBool iAllLinesIdle;

	};

#endif // __CLINEMONITOR_H__
