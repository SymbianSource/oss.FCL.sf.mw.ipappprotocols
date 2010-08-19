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
* Name          : sipalrsnapmonitor.h
* Part of       : SIP / SIP ALR Monitor
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPALRSNAPMONITOR_H
#define CSIPALRSNAPMONITOR_H

// INCLUDES
#include "sipalrobserver.h"
#include <sipsystemstateobserver.h>
#include <e32base.h>
#include <in_sock.h>
#include <connpref.h>
#include <extendedconnpref.h>

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
#include <comms-infras/es_mobility_apiext.h>
#else
#include <comms-infras/cs_mobility_apiext.h>
#endif

// FORWARD DECLARATIONS
class CSipSystemStateMonitor;

// CLASS DECLARATION
/**
* The implementation for SIP ALR SNAP monitor.
*
*/
class CSipAlrSnapMonitor : public CActive,
                           public MMobilityProtocolResp,
                           public MSipSystemStateObserver
	{
	public: // Constructors and destructor
	    
    	static CSipAlrSnapMonitor* NewLC( 
    	    TSipSNAPConfigurationData aSnapData,
            MSipAlrObserver& aObserver,
            RSocketServ& aSocketServer,
            CSipSystemStateMonitor& aSystemStateMonitor);
            
    	~CSipAlrSnapMonitor();
    
	public: // From CActive
	
	    void RunL();
	
		void DoCancel();
		
		TInt RunError( TInt aError );   
    	
    public: // From MMobilityProtocolResp
    	
        void PreferredCarrierAvailable(
            TAccessPointInfo aOldAP, 
            TAccessPointInfo aNewAP, 
            TBool aIsUpgrade,
            TBool aIsSeamless );
	    
	    void NewCarrierActive( 
	        TAccessPointInfo aNewAP, 
	        TBool aIsSeamless );
	    
	    void Error( TInt aError );
	    
	public: // From MSipSystemStateObserver

        void SystemVariableUpdated( 
		    CSipSystemStateMonitor::TSystemVariable aVariable,
		    TInt aObjectId,
		    TInt aValue ); 
    
	public: // New functions
			
		TUint32 SnapId() const;	
								  
		void AddObserverL( MSipAlrObserver& aObserver );
		
		TBool DetachObserver( MSipAlrObserver& aObserver );	
				
		void RefreshL();
		
		void AllowMigration();    
   
        void DisallowMigration();

        void NewIapAccepted();    
   
        void NewIapRejected();  
        
        TBool BearerId();
        
	private: // New functions	
		
		TInt StartSnap();
		
		void CreateMobilityAoL();
		
		void ResetState();
		
		void DestroyMobilityAo();
		
		void NotifyObservers( TUint aIapId );
		
		void NotifyInitializedObservers( 
		    TUint32 aIapId,
            MSipAlrObserver::TEvent aEvent );
		
		void NotifyNewObservers( 
		    TUint32 aIapId );
		
		TBool IsSnapAvailable() const;
		
	private: // Constructors
	
		CSipAlrSnapMonitor( 
		    TSipSNAPConfigurationData aSnapData,
		    RSocketServ& aSocketServer,
		    CSipSystemStateMonitor& aSystemStateMonitor);
		
    	void ConstructL( 
    	    MSipAlrObserver& aObserver );
   
    private: // Helper classes
    	
    	class TSipAlrSnapObserverInfo
    	    {
    	    public:
    		    MSipAlrObserver* iObserver;
    		    TBool iInitialEventDone;		
    	    };
  			
    private: // Data
    
    	TSipSNAPConfigurationData iSnapData;	
		RSocketServ& iSocketServer;
        CSipSystemStateMonitor& iSystemStateMonitor;				
		RConnection iConnection;
		TConnSnapPref iPrefs;
		CActiveCommsMobilityApiExt* iCommsMobilityAO;
		RArray<TSipAlrSnapObserverInfo> iObservers;
		TBool iPreferredCarrierAvailableCalled;
		TBool iMigrationAllowedByClient;
		TBool iFirstStartHasSucceeded;
		TBool iConnectionActive;
		TBool iDying;
		TConnPrefList  iPrefList;
	    TExtendedConnPref iExtPrefs;
		
	private: // For testing purposes	
		
    	#ifdef CPPUNIT_TEST
    		friend class CSipAlrMonitorTest;
        	friend class CSipAlrSnapMonitorTest;
    	#endif
	};

#endif //CSIPALRSNAPMONITOR_H
