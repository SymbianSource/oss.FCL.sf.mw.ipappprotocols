/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  See class definition below.
*
*/

#ifndef __CWLANBEARERMONITOR_H__
#define __CWLANBEARERMONITOR_H__

// INCLUDES
#include "sipbearermonitor.h"
#include "MWLanConnUsagePermissionObserver.h"
#include <rconnmon.h>

// FORWARD DECLARATIONS
class CWLanConnUsagePermissionMonitor;

// CLASS DEFINITION
/**
 * CWLanBearerMonitor implements WLan iap monitoring.
 * Network state is reported as follows (states and conditions):
 *
 * MSIPNetworkObserver::ENetworkInactive:
 * 	- IF ConnectionMonitor repors that iap id monitored is NOT in the list
 *    of available EBearerIdWLAN iaps
 *
 * MSIPNetworkObserver::ENetworkActive:
 * 	- IF ConnectionMonitor repors that iap id monitored is in the list
 *    of available EBearerIdWLAN iaps
 */
class CWLanBearerMonitor
	: public CSIPBearerMonitor,
	  public MConnectionMonitorObserver,
	  public MWLanConnUsagePermissionObserver
	{
	public:	// Constructors and destructor

		/**
		 * Static constructor
		 *
		 * @param aParams ECom instantiation parameters
		 * @return An initialized instance of this class.
		 */
		static CWLanBearerMonitor* NewL( TAny* aParams );

		/// Destructor
		~CWLanBearerMonitor();

	public: // from MSIPBearerMonitorObserver

		void MonitorStateChanged( TInt /*aError*/ ) {};
		
	public: // from CSIPBearerMonitor
        
        void RefreshL( TInt aError );
        
        TBool ContinueMonitoring( TInt aError );

	public: // from MConnectionMonitorObserver

		void EventL( const CConnMonEventBase &aConnMonEvent );
		
    public: // from MWLanConnUsagePermissionObserver
    
        void UsagePermissionChanged( TBool aPermissionToUse, TInt aError );
        
    public:
    
        static TInt CompletionCallback( TAny* aAny );

	private: // Constructors

		/**
		 * Constructor.
		 *
		 * @param aParams ECom instantiation parameters
		 */
		CWLanBearerMonitor( TAny* aParams );

		/// Default constructor. Not implemented
		CWLanBearerMonitor();

		/// 2nd phase constructor
		void ConstructL();
		
    private: // Private methods
    
        void SetCurrentAvailabilityL();
        
        TBool IsAvailable( const TConnMonIapInfo& aIapInfo, TInt aIapId );
        
        void IssueActivityNotification( TInt aError = KErrNone );
        
        void InformActivity();
        
        TBool IapSettingsInvalid( TInt aError ) const;

	private: // Data

		/// Connection monitor session. Owned.
		RConnectionMonitor iConnMon;
		
		// Owned.
		CWLanConnUsagePermissionMonitor* iUsagePermissionMonitor;
		
		// Owned.
		CDeltaTimer* iAsyncCompletionTimer;
		
		TBool iUsagePermission;
		
		TCallBack iCallBack;
		
		TDeltaTimerEntry iTimerEntry;
		
		TInt iLastError;
		
#ifdef CPPUNIT_TEST
    friend class CSIPWlanPluginTest;
#endif
	};

#endif // __CWLANBEARERMONITOR_H__
