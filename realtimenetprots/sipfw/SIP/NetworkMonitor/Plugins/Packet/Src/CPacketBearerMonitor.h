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
* Name        : CPacketBearerMonitor.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/
#ifndef CPACKETBEARERMONITOR_H
#define CPACKETBEARERMONITOR_H

// INCLUDES
#include "sipbearermonitor.h"
#include "MAccessNetworkObserver.h"


// FORWARD DECLARATIONS
class CLineMonitor;
class CPacketServiceMonitor;
class CRegistrationMonitor;
class CPacketContextMonitor;
class CAccessNetworkMonitor;

// CLASS DEFINITION
/**
 * CPacketBearerMonitor implements Packet class B network monitoring.
 * Network state is reported as follows (states and conditions):
 *
 * MSIPNetworkObserver::ENetworkInactive:
 * 	- IF not all lines are idle (i.e. one or more cs calls)
 *    OR device is not registered (home nor roaming) to network
 *	  OR packet service status is something else than Active or Suspended
 *
 * MSIPNetworkObserver::ENetworkActive:
 * 	- IF all lines are idle (i.e. no cs calls)
 *    AND device is registered (home or roaming) to network
 *    AND packet service status is Active
 *
 * MSIPNetworkObserver::ENetworkSuspended:
 * 	- IF all lines are idle (i.e. no cs calls)
 *    AND device is registered (home or roaming) to network
 *    AND packet service status is Suspended
 */
class CPacketBearerMonitor :
	public CSIPBearerMonitor,
	public MAccessNetworkInfoObserver
	{
	public:	// Constructors and destructor

		/**
		 * Static constructor
		 *
		 * @param aParams ECom instantiation parameters
		 * @return An initialized instance of this class.
		 */
		static CPacketBearerMonitor* NewL( TAny* aParams );

		/// Destructor
		~CPacketBearerMonitor();

	public: // from MSIPBearerMonitorObserver

		void MonitorStateChanged( TInt aError );
		
    public: // from CSIPBearerMonitor
        
        void RefreshL( TInt aError );
        
        TBool ContinueMonitoring( TInt aError );

	public: // from MAccessNetworkInfoObserver
		
		void InfoChanged( const TSIPAccessNetworkInfo& aInfo );

	private: // Constructors

		/**
		 * Constructor.
		 *
		 * @param aParams ECom instantiation parameters
		 */
		CPacketBearerMonitor( TAny* aParams );

		/// Default constructor. Not implemented
		CPacketBearerMonitor();

		/// 2nd phase constructor
		void ConstructL();
		
	private: // New functions	
		
		void CreateLineMonitor();
		
		void RemoveLineMonitor();
		
		MSIPNetworkObserver::TNetworkState CombineStates();
		
		TBool AllLinesIdle();

	private: // Data

		/// CS Line status monitor
		CLineMonitor* iLineMonitor;

		/// Packet service monitor
		CPacketServiceMonitor* iPacketServiceMonitor;

		/// Network registration monitor
		CRegistrationMonitor* iRegistrationMonitor;
		
		/// Packet Context monitor
		CPacketContextMonitor* iContextMonitor;
		
		// Access Network monitor
		CAccessNetworkMonitor* iAccessNetworkMonitor;
	};

#endif // CPACKETBEARERMONITOR_H
