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
* Name        : CPacketContextMonitor.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/
#ifndef __CPACKETCONTEXTMONITOR_H__
#define __CPACKETCONTEXTMONITOR_H__

// INCLUDES
#include <etelmm.h>
#include <etelpckt.h>
#include <in_sock.h>

#include "sipnetworkobserver.h"

// FORWARD DECLARATIONS
class CPacketContextWatcher;
class MSIPBearerMonitorObserver;

// CLASS DEFINITION
/**
 *
 */
class CPacketContextMonitor : public CBase
	{
	public:	// Constructors and destructor

        enum TContextMonitorState
            {
            EIdle,
            EEnumerateContexts,
            EGetContextInfo
            };
        
		/**
		 * Static constructor.
		 *
		 * @return An initialized instance of this class.
		 */
		static CPacketContextMonitor* NewL( 
		    MSIPBearerMonitorObserver& aObserver,
		    TInt aError );

		/// Destructor
		~CPacketContextMonitor();

	public: // New methods
	    
	    /// Creates new context watcher(s) if needed
	    void RefreshL( TInt aError );
	    
		/// Check whether context monitoring should be done after the error
		TBool ContinueMonitoring( TInt aError );

	    TBool IsPDPCtxError( TInt aError );
	    
	    /// Watchers use this for notifying the monitor about state change
		void WatcherStateChanged( TInt aError );

	private: // Constructors

		/// Constructor
		CPacketContextMonitor( MSIPBearerMonitorObserver& aObserver );
		
		/// Default constructor, not implemented.
		CPacketContextMonitor();

		/// 2nd phase constructor
		void ConstructL( TInt aError );

    private:
    

		TBool IsTemporaryErrorRequiringRetryTimer( TInt aError );
        
        TBool IsTemporaryErrorRequiringContextMonitoring( TInt aError );
        
        TBool IsTemporaryErrorRequiringOtherMonitoring( TInt aError );
        
        void StartMonitoringL( TInt aError );
		
		void Reset();
		
		void StartRetryTimer();
		
		void OtherMonitoring();
		
		void StopRetryTimer();
		
		static TInt RetryTimerExpired( TAny* aAny );

		void CreateLineWatchersL( TInt aError );

	private: // Data

		MSIPBearerMonitorObserver& iObserver;
		
		RPointerArray<CPacketContextWatcher> iContextWatchers;
		
		RSocketServ iSocketSrv;
		
		TBool iIsAvailable;

		CDeltaTimer* iRetryTimer;
		
		TCallBack iRetryTimerCallback;
		
		TDeltaTimerEntry iRetryTimerEntry;
		
		TUint32 iRetryAfter;
		
		TInt iOtherMonitoringRetryCount;
		
#ifdef CPPUNIT_TEST 
        friend class CPacketContextMonitorTest;
#endif

	};
	
#endif // __CPACKETCONTEXTMONITOR_H__
