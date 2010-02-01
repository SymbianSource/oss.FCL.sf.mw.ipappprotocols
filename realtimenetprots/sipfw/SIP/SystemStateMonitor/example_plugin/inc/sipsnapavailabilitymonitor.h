/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipsnapavailabilitymonitor.h
* Part of     : SIP / SIP System State Monitor
* Version     : SIP/6.0
*
*/



#ifndef CSIPSNAPAVAILABILITYMONITOR_H
#define CSIPSNAPAVAILABILITYMONITOR_H

// INCLUDES
#include <sipsystemstatemonitor.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class MSipSystemStateObserver;

// CLASS DEFINITION
/**
* A class for monitoring the availability of a SNAP.
*/
class CSipSnapAvailabilityMonitor : 
    public CActive
    {
    public:    // Constructors and destructor

        /**
        * Static constructor
        * @param aSnapId the ID of the SNAP to be monitored
        * @return An initialized instance of this class.
        */
        static CSipSnapAvailabilityMonitor* NewL( 
            TUint32 aSnapId,
            MSipSystemStateObserver& aObserver );

        /**
        * Static constructor
        * @param aSnapId the ID of the SNAP to be monitored
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

    public: // New functions
    
        TUint32 SnapId() const;

        CSipSystemStateMonitor::TSnapAvailability SnapAvailability() const;

        void AddObserverL( MSipSystemStateObserver& aObserver );

        void RemoveObserver( MSipSystemStateObserver& aObserver );

        TBool HasObservers() const;

    private: // Constructors

        CSipSnapAvailabilityMonitor( 
            TUint32 aSnapId );
            
        void ConstructL( MSipSystemStateObserver& aObserver );
        
    private: // Private methods

    private: // Data

        CSipSystemStateMonitor::TSnapAvailability iSnapAvailability;
        // The SNAP which is being monitored
        TUint32 iSnapId;
        // Observers not owned
        RPointerArray<MSipSystemStateObserver> iObservers; 

    };

#endif // CSIPSNAPAVAILABILITYMONITOR_H
