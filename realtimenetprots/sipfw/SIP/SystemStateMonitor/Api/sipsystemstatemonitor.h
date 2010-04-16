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
* Name          : sipsystemstatemonitor.h
* Part of       : SIP
* Version       : SIP/6.0 
*
*/



#ifndef CSIPSYSTEMSTATEMONITOR_H
#define CSIPSYSTEMSTATEMONITOR_H

// INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>

// FORWARD DECLARATIONS
class MSipSystemStateObserver;

// CONSTANTS
/** Interface UID of this ECOM interface */
const TUid KSipSystemStateMonitorIfUid = { 0x10283310 };

// CLASS DECLARATION
/**
* @publishedPartner
* @released
*
* Interface for SIP System State Monitor ECOM plug-in(s).
*/
class CSipSystemStateMonitor : public CBase
    {
    public: // Enumerations
    
        /** System variables */
        enum TSystemVariable
            {
            /** System state event */
            ESystemState=0,
            /** SNAP availability event */
            ESnapAvailability,
            /** RFS State */
            ERfsState,
            /** VPN State */
            EVpnState
            };    
    
        /** System states */
        enum TSystemState 
            {
            /** System is starting up */
            ESystemNotReady=0,
            /** System has been started up */
            ESystemReady,
            /** System is shutting down */
            ESystemShuttingDown,
			/** System is in Offline state */
			ESystemOffline,
			/** System is in Online state */
			ESystemOnline
            };
    
        /** Snap availability notifications */
        enum TSnapAvailability 
            {
            /** SNAP not available */
            ESnapNotAvailable=0,
            /** SNAP available */
            ESnapAvailable
            };

        /**RFS States */
        enum TRfsState
            {
            /** Rfs Started */
            ERfsStarted=0,
            /** Rfs Failed */
            ERfsFailed,
            /** Rfs Completed */
            ERfsCompleted
            };

        enum TVpnState
            {
            /** Vpn is about to be started */
            EVpnInitiating = 0,                
            /** Vpn ended */
            EVpnTerminated
            };
        
    public: // Constructors and destructor
    
        /**
        * Destructor.
        */
        virtual inline ~CSipSystemStateMonitor();

    public: // New functions

        /**
        * Gets current state of the system        
        * @return current state of the system
        */
        virtual TSystemState State() const = 0;

        /**
        * Starts monitoring for a system variable.
        * @param aVariable system variable to be monitored
        * @param aObjectId identifies the monitored object,
        *        for ESystemState the value should be zero,
        *        for ESnapAvailability the ID of the SNAP
        * @param aObserver observer to be informed about the changes 
        *        in the values of system variables.
        * @leave KErrNoMemory if out-of-memory
        * @leave KErrAlreadyExists if monitoring for the variable 
        *        has already been started
        */
        virtual void StartMonitoringL( TSystemVariable aVariable,
                                       TInt aObjectId,
                                       MSipSystemStateObserver& aObserver );

        /**
        * Stops monitoring for a system variable.
        * @param aVariable system variable that was monitored
        * @param aObjectId identifies the monitored object,
        *        for ESystemState value should be zero,
        *        for ESnapAvailability the ID of the SNAP
        * @param aObserver observer to be informed about the changes 
        *        in the values of system variables.
        */
        virtual void StopMonitoring( TSystemVariable aVariable, 
                                     TInt aObjectId,
                                     MSipSystemStateObserver& aObserver );

        /**
        * Gets current value of a particular system variable
        * @param aVariable system variable that was monitored
        * @param aObjectId identifies the monitored object,
        *        for ESystemState the value should be zero,
        *        for ESnapAvailability the ID of the SNAP
        * @return current value of the system variable
        */
        virtual TInt CurrentValue( TSystemVariable aVariable,
                                   TInt aObjectId ) const;
        
        /**
        * Gets current value of a particular system variable
        * @param aVariable system variable that was monitored
        * @param aObjectId identifies the monitored object,
        *        for ESystemState the value should be zero,
        *        for ESnapAvailability the ID of the SNAP
        * @return current value of the system variable
        */
        virtual void EventProcessingCompleted( TSystemVariable aVariable,
                                TInt aObjectId,
                                MSipSystemStateObserver& aObserver);

    public: // Data

        /// Unique key for implementations of this interface.
        TUid iInstanceKey;

    protected: // Constructors:

        inline CSipSystemStateMonitor();
    };
    
#include "sipsystemstatemonitor.inl"

#endif // CSIPSYSTEMSTATEMONITOR_H
