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
* Name          : sipalrmonitor.h
* Part of       : SIP / SIP ALR Monitor
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPALRMONITOR_H
#define CSIPALRMONITOR_H

//  INCLUDES
#include "sipalrsnapmonitor.h"
#include <in_sock.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class MSipAlrObserver;
class CSipSystemStateMonitor;

// CLASS DECLARATION
/**
* The implementation for SIP ALR monitor.
*/
class CSipAlrMonitor : public CBase								
    {
	public: // Constructors and destructor
	    
        /**
        * Two-phased constructor.
        * @param aSystemStateMonitor used for monitoring SNAP availability 
        * @return a new instance, the ownership is transferred.
        */	    
    	static CSipAlrMonitor* NewL(
    	    CSipSystemStateMonitor& aSystemStateMonitor);
    	
        /**
        * Destructor.
        */    	
    	~CSipAlrMonitor();
    		
    public: // New functions

        /**
        * Starts to monitor IAP availability for the SNAP.
        * The observer will be informed asynchronouysly
        * when any IAP is available for the monitored SNAP. 
        * @param aConfigData SNAP Data to identify the correct SNAP Monitor.
        * @param aObserver observer to be informed when an IAP becomes available
        */
        void MonitorSnapL( TSipSNAPConfigurationData & aConfigData, 
                           MSipAlrObserver& aObserver);


        /**
        * Refreshes the IAP availability for the SNAP.
        * Scans the available IAPs for the SNAP and informs all the observers 
        * if a better IAP than the current one is available.
        * @param aConfigData SNAP Data to identify the correct SNAP Monitor.
        */
        void RefreshIapAvailabilityL( TSipSNAPConfigurationData & aConfigData );
    
        /**
        * Allows migration to a new IAP for the SNAP.
        * @param aConfigData SNAP Data to identify the correct SNAP Monitor.
        * @return KErrNone on success, otherwise a system wide error code.
        */    
        TInt AllowMigration( TSipSNAPConfigurationData & aConfigData );    

        /**
        * Disallows migration to a new IAP for the SNAP.
        * @param aConfigData SNAP Data to identify the correct SNAP Monitor.
        * @return KErrNone on success, otherwise a system wide error code.
        */    
        TInt DisallowMigration( TSipSNAPConfigurationData & aConfigData );
    
        /**
        * The migration has succeeded and the new IAP has been taken into use.
        * @param aConfigData SNAP Data to identify the correct SNAP Monitor.
        * @return KErrNone on success, otherwise a system wide error code.
        */
        TInt NewIapAccepted( TSipSNAPConfigurationData & aConfigData );    

        /**
        * The migration has failed and the new IAP was not taken into use.
        * @param aConfigData SNAP Data to identify the correct SNAP Monitor.
        * @return KErrNone on success, otherwise a system wide error code.
        */    
        TInt NewIapRejected( TSipSNAPConfigurationData & aConfigData );

        /**
        * Frees all the resources reserved for the observer.
        * @param aObserver a observer
        */
        void FreeResources( MSipAlrObserver& aObserver );       

										  
	private: // Constructors
	
		void ConstructL();
	
		CSipAlrMonitor(CSipSystemStateMonitor& aSystemStateMonitor);	
		
	private: // New functions
	
	    CSipAlrSnapMonitor* FindSnapMonitor( TSipSNAPConfigurationData & aConfigData);
	    
	    	
	private: // Data
	
	    // Used for monitoring SNAP availability 
	    CSipSystemStateMonitor& iSystemStateMonitor;
	
		// SNAP monitors, owned
		RPointerArray<CSipAlrSnapMonitor> iSnapMonitors;
    	
    	// Socket server session
    	RSocketServ iSocketServer;
    	
    private: // For testing purposes	
    			
    	#ifdef CPPUNIT_TEST
        	friend class CSipAlrMonitorTest;
    	#endif	
    };
	
#endif // CSIPALRMONITOR_H
