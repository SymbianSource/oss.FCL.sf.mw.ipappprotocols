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
* Name          : sipsystemstatemonitorimpl.h
* Part of       : SIP / SIP System State Monitor  
* Version       : SIP/6.0 
*
*/



#ifndef CSIPSYSTEMSTATEMONITORIMPL_H
#define CSIPSYSTEMSTATEMONITORIMPL_H

// INCLUDES
#include "sipsystemstatemonitorao.h"
#include "sipsnapavailabilitymonitor.h"
#include <sipsystemstateobserver.h>
#include <sipsystemstatemonitor.h>

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

    private: // Constructors

        CSipSystemStateMonitorImpl();
        void ConstructL();

    private: // Private functions
    
        CSipSnapAvailabilityMonitor* FindSnapMonitorById(
            TInt aSnapId ) const;

    private: // Data
    
        CSipSystemStateMonitorAo* iMonitorAo;
        RPointerArray<CSipSnapAvailabilityMonitor> iSnapMonitors;
        
    };

#endif // CSIPSYSTEMSTATEMONITORIMPL_H
