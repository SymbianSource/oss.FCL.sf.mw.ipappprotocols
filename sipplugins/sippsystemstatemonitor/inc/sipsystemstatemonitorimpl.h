/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CSIPSYSTEMSTATEMONITORIMPL_H
#define CSIPSYSTEMSTATEMONITORIMPL_H

// INCLUDES
#include "sipsystemstatemonitorao.h"
#include "sipsnapavailabilitymonitor.h"
#include <sipsystemstateobserver.h>
#include <sipsystemstatemonitor.h>

// FORWARD DECLARATIONS
class CSipDeviceStateAware;
class CSipRfsMonitorAo;
class CSipVpnMonitorAo;

// CLASS DECLARATION
/**
* The default implementation for Sip System State Monitor.
*
* @lib sipsystemstatemonitor.lib
*/
class CSipSystemStateMonitorImpl : public CSipSystemStateMonitor
    {
    public: // Constructors and destructor

        static CSipSystemStateMonitorImpl* NewL();
        ~CSipSystemStateMonitorImpl();
        
    public: // From CSipSystemStateMonitor

		CSipSystemStateMonitor::TSystemState State() const;

        void StartMonitoringL( TSystemVariable aVariable,
                               TInt aObjectId,
                               MSipSystemStateObserver& aObserver );

        void StopMonitoring( TSystemVariable aVariable, 
                             TInt aObjectId,
                             MSipSystemStateObserver& aObserver );

        TInt CurrentValue( TSystemVariable aVariable,
                           TInt aObjectId ) const;
        
        void EventProcessingCompleted( TSystemVariable aVariable,
                            TInt aObjectId,
                            MSipSystemStateObserver& aObserver);

    private: // Constructors

        CSipSystemStateMonitorImpl();
        void ConstructL();

    private: // Private functions
    
        CSipSnapAvailabilityMonitor* FindSnapMonitorById(
            TInt aSnapId ) const;

    private: // Data
    
        CSipSystemStateMonitorAo* iMonitorAo;
        RPointerArray<CSipSnapAvailabilityMonitor> iSnapMonitors;
        CSipRfsMonitorAo* iRfsMonitor;
		CSipDeviceStateAware* iSipDeviceAwareObject;
		
        // P&S monitor for SIP / VPN communication
        CSipVpnMonitorAo* iVpnMonitor;
		    
    private: // For testing purposes
    
        friend class CSipSystemStateMonitorImplTest;
    };
    

#endif // CSIPSYSTEMSTATEMONITORIMPL_H
