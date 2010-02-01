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
* Name        : CPacketServiceMonitor.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/
#ifndef __CPACKETSERVICEMONITOR_H__
#define __CPACKETSERVICEMONITOR_H__

// INCLUDES
#include <etelmm.h>
#include <etelpckt.h>
#include "sipnetworkobserver.h"

// FORWARD DECLARATIONS
class MSIPBearerMonitorObserver;
class CMsClassWatcher;

// CLASS DEFINITION
/**
 *
 */
class CPacketServiceMonitor
	: public CActive
	{
	public:	// Constructors and destructor

		/**
		 * Static constructor..
		 *
		 * @return An initialized instance of this class.
		 */
		static CPacketServiceMonitor* NewL( 
		    MSIPBearerMonitorObserver& aObserver );

		/// Destructor
		~CPacketServiceMonitor();

	public: // New methods

		TBool IsSuspended() const;
		TBool IsLineMonitoringNeeded() const;
		TBool IsSupportedMSClass() const;
		void WatcherStateChanged( TInt aError );

	protected: // From CActive

		void DoCancel();
		void RunL();

	private: // New methods

		TBool IsSuspended ( RPacketService::TStatus aStatus ) const;

	private: // Constructors

		/// Constructor
		CPacketServiceMonitor( MSIPBearerMonitorObserver& aObserver );

		/// Default ccnstructor, not implemented.
		CPacketServiceMonitor();

		/// 2nd phase constructor
		void ConstructL();

	private: // Data

		MSIPBearerMonitorObserver& iObserver;
		RTelServer iTelServer;
		RMobilePhone iPhone;
		RPacketService iPacketService;
		RPacketService::TStatus iPacketServiceStatus;
		TBool iSuspended;
		CMsClassWatcher* iMsClassWatcher;

	};

#endif // __CPACKETSERVICEMONITOR_H__
