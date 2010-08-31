// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : sipalrmonitor.cpp
// Part of       : SIP / SIP ALR Monitor
// Version       : SIP/6.0 
//



#include "sipalrobserver.h"
#include "sipalrmonitor.h"
#include "../../Profile/Inc/SipProfileLog.h"

// -----------------------------------------------------------------------------
// CSipAlrMonitor::NewL
// -----------------------------------------------------------------------------
//
CSipAlrMonitor* CSipAlrMonitor::NewL(
    CSipSystemStateMonitor& aSystemStateMonitor)
	{
    CSipAlrMonitor* self =  new(ELeave)CSipAlrMonitor(aSystemStateMonitor);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSipAlrMonitor::CSipAlrMonitor
// -----------------------------------------------------------------------------
//
CSipAlrMonitor::CSipAlrMonitor(CSipSystemStateMonitor& aSystemStateMonitor) :
    iSystemStateMonitor(aSystemStateMonitor)
	{
	}

// -----------------------------------------------------------------------------
// CSipAlrMonitor::ConstructL
// -----------------------------------------------------------------------------
//
void CSipAlrMonitor::ConstructL()
    {
    User::LeaveIfError ( iSocketServer.Connect() );
	PROFILE_DEBUG1("CSipAlrMonitor::ConstructL")
    }

// -----------------------------------------------------------------------------
// CSipAlrMonitor::~CSipAlrMonitor
// -----------------------------------------------------------------------------
//
CSipAlrMonitor::~CSipAlrMonitor()
	{
	iSocketServer.Close();
	iSnapMonitors.ResetAndDestroy();
	
	PROFILE_DEBUG1("CSipAlrMonitor::~CSipAlrMonitor")	
	}

// -----------------------------------------------------------------------------
// CSipAlrMonitor::RefreshIapAvailabilityL
// -----------------------------------------------------------------------------
//	
void CSipAlrMonitor::RefreshIapAvailabilityL ( TSipSNAPConfigurationData & aConfigData )
	{
    PROFILE_DEBUG3( "CSipAlrMonitorImplementation::RefreshIapAvailabilityL",
            aConfigData.iSnapId )
    
    CSipAlrSnapMonitor* monitor = FindSnapMonitor( aConfigData );
    if ( monitor )
        {
		monitor->RefreshL();
		}
	}
	    
// -----------------------------------------------------------------------------
// CSipAlrMonitor::AllowMigration
// -----------------------------------------------------------------------------
//
TInt CSipAlrMonitor::AllowMigration( TSipSNAPConfigurationData & aConfigData )
    {
    PROFILE_DEBUG3("CSipAlrMonitor::AllowMigration, SnapId",aConfigData.iSnapId)
    PROFILE_DEBUG3("CSipAlrMonitor::AllowMigration, BearerFiltering",aConfigData.iBearerId)
    CSipAlrSnapMonitor* monitor = FindSnapMonitor( aConfigData );
    if ( monitor )
        {
        monitor->AllowMigration();     
        return KErrNone;
		}
	return KErrNotFound;
    }
// -----------------------------------------------------------------------------
// CSipAlrMonitor::DisallowMigration
// -----------------------------------------------------------------------------
//
TInt CSipAlrMonitor::DisallowMigration( TSipSNAPConfigurationData& aConfigData )
    {
    PROFILE_DEBUG3("CSipAlrMonitor::DisallowMigration, SnapId",aConfigData.iSnapId)
    PROFILE_DEBUG3("CSipAlrMonitor::DisallowMigration, BearerFiltering",aConfigData.iBearerId)
    CSipAlrSnapMonitor* monitor = FindSnapMonitor( aConfigData );
    if ( monitor )
        {
        monitor->DisallowMigration();     
        return KErrNone;
		}
	return KErrNotFound;     
    }
    
// -----------------------------------------------------------------------------
// CSipAlrMonitor::NewIapAccepted
// -----------------------------------------------------------------------------
//    
TInt CSipAlrMonitor::NewIapAccepted( TSipSNAPConfigurationData & aConfigData)  
    {
    PROFILE_DEBUG3("CSipAlrMonitor::NewIapAccepted, SnapId",aConfigData.iSnapId)
    
    CSipAlrSnapMonitor* monitor = FindSnapMonitor( aConfigData );
    if ( monitor )
        {
        monitor->NewIapAccepted();     
        return KErrNone;
		}
	return KErrNotFound;       
    }
    
// -----------------------------------------------------------------------------
// CSipAlrMonitor::NewIapRejected
// -----------------------------------------------------------------------------
//  
TInt CSipAlrMonitor::NewIapRejected( TSipSNAPConfigurationData & aConfigData )
    {
    PROFILE_DEBUG3( "CSipAlrMonitor::NewIapRejected, SnapId",aConfigData.iSnapId )
    
    CSipAlrSnapMonitor* monitor = FindSnapMonitor( aConfigData );
    if ( monitor )
        {
        monitor->NewIapRejected();
        return KErrNone;
        }
	return KErrNotFound;      
    }
    
// -----------------------------------------------------------------------------
// CSipAlrMonitor::MonitorSnapL
// -----------------------------------------------------------------------------
//
void CSipAlrMonitor::MonitorSnapL( 
    TSipSNAPConfigurationData& aConfigData,
    MSipAlrObserver& aObserver)
    {
    PROFILE_DEBUG4( "CSipAlrMonitor::MonitorSnapL",
            aConfigData.iSnapId, reinterpret_cast< TUint >( &aObserver ) ) 
                    
 
    CSipAlrSnapMonitor* monitor = FindSnapMonitor( aConfigData);
    if ( monitor )
        {
        // Monitor exists, add observer and get current IAP availability
        monitor->AddObserverL(aObserver);
        monitor->RefreshL();
        
        PROFILE_DEBUG1("CSipAlrMonitor::MonitorSnapL: observer added" )
        }
    else
        {
    	monitor = CSipAlrSnapMonitor::NewLC( 
    	        aConfigData, aObserver, iSocketServer, iSystemStateMonitor);
    	iSnapMonitors.AppendL( monitor );
    	CleanupStack::Pop( monitor );
    	
    	PROFILE_DEBUG1("CSipAlrMonitor::MonitorSnapL: new monitor created" )
        }
    }

// -----------------------------------------------------------------------------
// CSipAlrMonitor::FreeResources
// -----------------------------------------------------------------------------
//
void CSipAlrMonitor::FreeResources ( MSipAlrObserver& aObserver )
    {
    CSipAlrSnapMonitor* monitor= 0;
	for ( TInt i = iSnapMonitors.Count()-1; i >= 0; i-- )
		{
		TBool last = iSnapMonitors[i]->DetachObserver ( aObserver );
		if ( last ) 
			{
            monitor = iSnapMonitors[i];
			iSnapMonitors.Remove(i);
			delete monitor;
			monitor = NULL;
			PROFILE_DEBUG1( "CSipAlrMonitor::FreeResources monitor deleted" )
			}
		}
	iSnapMonitors.Compress();
    }

// -----------------------------------------------------------------------------
// CSipAlrMonitor::FindSnapMonitor
// -----------------------------------------------------------------------------
//
CSipAlrSnapMonitor* CSipAlrMonitor::FindSnapMonitor( TSipSNAPConfigurationData & aConfigData)
    {
    CSipAlrSnapMonitor* monitor = NULL;
	for ( TInt i = 0; i < iSnapMonitors.Count() && !monitor; i++ )
		{
		if ( iSnapMonitors[i]->SnapId() == aConfigData.iSnapId && iSnapMonitors[i]->BearerId() == aConfigData.iBearerId ) //mtr found
		    {
		   	monitor = iSnapMonitors[i];
			}
        }
    return monitor;
    }



