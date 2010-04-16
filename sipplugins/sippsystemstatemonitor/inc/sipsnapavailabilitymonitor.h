/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef CSIPSNAPAVAILABILITYMONITOR_H
#define CSIPSNAPAVAILABILITYMONITOR_H

// INCLUDES
#include <sipsystemstatemonitor.h>
#include <rconnmon.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class MSipSystemStateObserver;

// CLASS DEFINITION
/**
* A class for monitoring the availability of a SNAP.
*/
class CSipSnapAvailabilityMonitor : 
    public CActive, 
    public MConnectionMonitorObserver
    {
    public:    // Constructors and destructor

        /**
        * Static constructor
        * @param aSnapId the ID of the SNAP to be monitored
        * @param aPermissionToUseNetwork the current permission to use network
        * @param aObserver observer to be notified about SNAP availability
        * @return An initialized instance of this class.
        */
        static CSipSnapAvailabilityMonitor* NewL( 
            TUint32 aSnapId,
            MSipSystemStateObserver& aObserver );

        /**
        * Static constructor
        * @param aSnapId the ID of the SNAP to be monitored
        * @param aPermissionToUseNetwork the current permission to use network
        * @param aObserver observer to be notified about SNAP availability        
        * @return An initialized instance of this class.
        */
        static CSipSnapAvailabilityMonitor* NewLC( 
            TUint32 aSnapId,           
            MSipSystemStateObserver& aObserver );

        /// Destructor
        ~CSipSnapAvailabilityMonitor();

    public: // From CActive
    
        void RunL();        

        void DoCancel();

    public: // From MConnectionMonitorObserver

        void EventL( const CConnMonEventBase &aConnMonEvent ); 
         
    public: // New functions
    
        TUint32 SnapId() const;

        CSipSystemStateMonitor::TSnapAvailability SnapAvailability() const;

        void AddObserverL( MSipSystemStateObserver& aObserver );

        void RemoveObserver( MSipSystemStateObserver& aObserver );

        TBool HasObservers() const;

    private: // Constructors

        CSipSnapAvailabilityMonitor( TUint32 aSnapId );
            
        void ConstructL( MSipSystemStateObserver& aObserver );
        
    private: // Private methods

        TBool IsSnapAvailable( 
            const TConnMonSNAPInfo& aSnapInfo, 
            TUint32 aSnapId ) const;
            
        void NotifyObservers() const;
        
        TBool SetCurrentState( TBool aSnapAvailable );        

        TBool CanSnapBeUsed() const;

    private: // Data

        TUint32 iSnapId;
        TBool iSnapAvailable;
        // Observers not owned
        RPointerArray<MSipSystemStateObserver> iObservers; 
        /// Connection monitor session. Owned.
        RConnectionMonitor iConnMon;
        TConnMonSNAPInfoBuf iSnapInfoBuf;
        // ETrue if successfully connected to Connection Monitor Server
        // (iConnMon).
        TBool iIsConnected;
        
    private: // For unit testing purposes
    
        friend class CSipSnapAvailabilityMonitorTest;  
    };

#endif // CSIPSNAPAVAILABILITYMONITOR_H
