/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description : P&S key monitor for communication between SIP Profile 
*               server and VPN client
* Name        : sipvpnmonitorao.cpp
* Part of     : Sip System State Monitor
* Version     : 1.0
*
*/

#ifndef SIPVPNMONITORAO_H
#define SIPVPNMONITORAO_H

// INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <e32property.h>
#include <e32cmn.h>
#include <sipsystemstatemonitor.h>

class CSipVpnMonitorAo : public CActive
    {
    public: // Constructors and destructor
        static CSipVpnMonitorAo* NewL();    
        ~CSipVpnMonitorAo();
        
    private: // Constructors
        CSipVpnMonitorAo();
        void ConstructL();
        
    public: // New Functions
        CSipSystemStateMonitor::TVpnState State() const;
        void AddObserverL( MSipSystemStateObserver& aObserver );
        void RemoveObserver( MSipSystemStateObserver& aObserver );
        void EventProcessingCompleted(MSipSystemStateObserver& aObserver);
        TBool MappedState(TInt aState);
        static TInt TimerExpired(TAny* aSelf);
        
    private:
        void NotifyObservers();
        void EventProcessingCompleted();
    private: // From CActive
        void RunL();
        TInt RunError( TInt aError );
        void DoCancel();

    private: // Data
        TInt                                        iCount;
        // Observers not owned
        RPointerArray<MSipSystemStateObserver> iObservers;
        RProperty iProperty;
        CSipSystemStateMonitor::TVpnState iState;
        CPeriodic* iGuardTimer;
		        
    private: // For testing purposes  
        friend class CSipVpnMonitorAoTestApp;
    };
#endif /* SIPVPNMONITORAO_H */
