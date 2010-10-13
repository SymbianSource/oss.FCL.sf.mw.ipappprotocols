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
void CSipAlrMonitor::RefreshIapAvailabilityL ( TUint32 aSnapId )
	{
    PROFILE_DEBUG3( "CSipAlrMonitorImplementation::RefreshIapAvailabilityL",
	                 aSnapId )
    
    CSipAlrSnapMonitor* monitor = FindSnapMonitor( aSnapId );
    if ( monitor )
        {
		monitor->RefreshL();
		}
	}
	    
// -----------------------------------------------------------------------------
// CSipAlrMonitor::AllowMigration
// -----------------------------------------------------------------------------
//
TInt CSipAlrMonitor::AllowMigration( TUint32 aSnapId )
    {
    PROFILE_DEBUG3("CSipAlrMonitor::AllowMigration, SnapId",aSnapId)
    
    CSipAlrSnapMonitor* monitor = FindSnapMonitor( aSnapId );
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
TInt CSipAlrMonitor::DisallowMigration( TUint32 aSnapId )
    {
    PROFILE_DEBUG3("CSipAlrMonitor::DisallowMigration, SnapId",aSnapId)
    
    CSipAlrSnapMonitor* monitor = FindSnapMonitor( aSnapId );
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
TInt CSipAlrMonitor::NewIapAccepted( TUint32 aSnapId )  
    {
    PROFILE_DEBUG3("CSipAlrMonitor::NewIapAccepted, SnapId",aSnapId)
    
    CSipAlrSnapMonitor* monitor = FindSnapMonitor( aSnapId );
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
TInt CSipAlrMonitor::NewIapRejected( TUint32 aSnapId )
    {
    PROFILE_DEBUG3( "CSipAlrMonitor::NewIapRejected, SnapId",aSnapId )
    
    CSipAlrSnapMonitor* monitor = FindSnapMonitor( aSnapId );
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
    TUint32 aSnapId,
    MSipAlrObserver& aObserver )
    {
    PROFILE_DEBUG4( "CSipAlrMonitor::MonitorSnapL",
                    aSnapId, reinterpret_cast< TUint >( &aObserver ) )    
    
    CSipAlrSnapMonitor* monitor = FindSnapMonitor( aSnapId );
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
    	    aSnapId, aObserver, iSocketServer, iSystemStateMonitor );
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
	for ( TInt i = iSnapMonitors.Count()-1; i >= 0; i-- )
		{
		TBool last = iSnapMonitors[i]->DetachObserver ( aObserver );
		if ( last ) 
			{
			delete iSnapMonitors[i];
			iSnapMonitors.Remove ( i );
			PROFILE_DEBUG1( "CSipAlrMonitor::FreeResources monitor deleted" )
			}
		}
    }

// -----------------------------------------------------------------------------
// CSipAlrMonitor::FindSnapMonitor
// -----------------------------------------------------------------------------
//
CSipAlrSnapMonitor* CSipAlrMonitor::FindSnapMonitor( TUint32 aSnapId )
    {
    CSipAlrSnapMonitor* monitor = NULL;
	for ( TInt i = 0; i < iSnapMonitors.Count() && !monitor; i++ )
		{
		if ( iSnapMonitors[i]->SnapId() == aSnapId )
			{    
			monitor = iSnapMonitors[i];
			}
        }
    return monitor;
    }
