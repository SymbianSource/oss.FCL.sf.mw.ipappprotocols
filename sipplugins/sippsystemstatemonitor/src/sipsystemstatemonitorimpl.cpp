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


//  INCLUDE FILES
#include "sipsystemstatemonitorimpl.h"
#include "sipdevicestateaware.h"
#include "siprfsmonitorao.h"
#include "sipvpnmonitorao.h"
#include <featmgr.h>          // for Feature Manager

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorImpl::NewL
// -----------------------------------------------------------------------------
//
CSipSystemStateMonitorImpl* CSipSystemStateMonitorImpl::NewL()
    {
    CSipSystemStateMonitorImpl* self =
		new( ELeave )CSipSystemStateMonitorImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorImpl::ConstructL
// -----------------------------------------------------------------------------
//
void CSipSystemStateMonitorImpl::ConstructL()
    {
    iMonitorAo = CSipSystemStateMonitorAo::NewL();
    iSipDeviceAwareObject = CSipDeviceStateAware::NewL();
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorImpl::CSipSystemStateMonitorImpl
// -----------------------------------------------------------------------------
//
CSipSystemStateMonitorImpl::CSipSystemStateMonitorImpl()
    {
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorImpl::~CSipSystemStateMonitorImpl
// -----------------------------------------------------------------------------
//
CSipSystemStateMonitorImpl::~CSipSystemStateMonitorImpl()
    {
    // iVpnMonitor is created in StartMonitoringL().
    if ( FeatureManager::FeatureSupported( KFeatureIdFfImsDeregistrationInVpn ) )
        {
        delete iVpnMonitor;
        iVpnMonitor = NULL;
        }
		
    delete iMonitorAo;
    iSnapMonitors.ResetAndDestroy();
    delete iRfsMonitor;
	delete iSipDeviceAwareObject;
    }
    
// -----------------------------------------------------------------------------
// CSipSystemStateMonitorImpl::State
// -----------------------------------------------------------------------------
//
CSipSystemStateMonitor::TSystemState CSipSystemStateMonitorImpl::State() const
	{
	return iMonitorAo->State();
	}

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorImpl::StartMonitoringL
// -----------------------------------------------------------------------------
//
void CSipSystemStateMonitorImpl::StartMonitoringL(
    TSystemVariable aVariable,
    TInt aObjectId,
    MSipSystemStateObserver& aObserver )
    {
    if ( aVariable == ESystemState )
        {
        iMonitorAo->AddObserverL( aObserver );
        iSipDeviceAwareObject->AddObserverL( aObserver );
        }
    else if ( aVariable == ESnapAvailability )
        {
        CSipSnapAvailabilityMonitor* monitor = FindSnapMonitorById( aObjectId );
        if ( !monitor )
            {
            monitor = CSipSnapAvailabilityMonitor::NewLC( 
                aObjectId, aObserver );
            iSnapMonitors.AppendL( monitor );
            CleanupStack::Pop( monitor );
            }
        else
            {
            monitor->AddObserverL( aObserver );
            }
        }
    else if ( aVariable == ERfsState )
        {
        iRfsMonitor = iRfsMonitor?iRfsMonitor:CSipRfsMonitorAo::NewL();
        iRfsMonitor->AddObserverL( aObserver );
        }
    // CSipVpnMonitorAo is created for P&S key change monitoring.
    else if ( FeatureManager::FeatureSupported( KFeatureIdFfImsDeregistrationInVpn )
         && ( aVariable == EVpnState ) )
        {
        if ( !iVpnMonitor )
            {
            iVpnMonitor = CSipVpnMonitorAo::NewL();
            }
        iVpnMonitor->AddObserverL( aObserver );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorImpl::StopMonitoring
// -----------------------------------------------------------------------------
//
void CSipSystemStateMonitorImpl::StopMonitoring(
    TSystemVariable aVariable, 
    TInt aObjectId,
    MSipSystemStateObserver& aObserver )
    {
    if ( aVariable == ESystemState )
        {    
        iMonitorAo->RemoveObserver( aObserver );
        iSipDeviceAwareObject->RemoveObserver ( aObserver );
        }
    else if ( aVariable == ESnapAvailability )
        {
        CSipSnapAvailabilityMonitor* monitor = FindSnapMonitorById( aObjectId );
        if ( monitor )
            {            
            monitor->RemoveObserver( aObserver );
            if ( !monitor->HasObservers() )
                {
                iSnapMonitors.Remove( iSnapMonitors.Find( monitor ) );
                delete monitor;
                }
            }
        }
    else if ( aVariable == ERfsState )
        {
        if(iRfsMonitor)
            iRfsMonitor->RemoveObserver( aObserver );
        }
    // Remove the client as an observer when stops VPN P&S key monitoring.
    else if ( FeatureManager::FeatureSupported( KFeatureIdFfImsDeregistrationInVpn )
         && ( aVariable == EVpnState ) )
        {
        if ( iVpnMonitor )
            {
            iVpnMonitor->RemoveObserver( aObserver );
            }
        }
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorImpl::CurrentValue
// -----------------------------------------------------------------------------
//
TInt CSipSystemStateMonitorImpl::CurrentValue( 
    TSystemVariable aVariable,
    TInt aObjectId ) const
    {
    if ( aVariable == ESystemState )
        {
        return iMonitorAo->State();
        }
    else if ( aVariable == ESnapAvailability )
        {
        CSipSnapAvailabilityMonitor* monitor = FindSnapMonitorById( aObjectId );
        if ( monitor )        
            return monitor->SnapAvailability();
        }
    else if ( aVariable == ERfsState )
        {
        if(iRfsMonitor)
            iRfsMonitor->State();
        }
    else if ( FeatureManager::FeatureSupported( KFeatureIdFfImsDeregistrationInVpn )
         && ( aVariable == EVpnState ) )
        {
        if( iVpnMonitor )
            {
            return iVpnMonitor->State();
            }
        }
		
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorImpl::EventProcessingCompleted
// -----------------------------------------------------------------------------
//
void CSipSystemStateMonitorImpl::EventProcessingCompleted(
        TSystemVariable aVariable,
        TInt /*aObjectId*/,
        MSipSystemStateObserver& aObserver)
    {
    if ( iRfsMonitor && aVariable == ERfsState )
        {
        iRfsMonitor->EventProcessingCompleted(aObserver);
        }
	else if ( iSipDeviceAwareObject && aVariable == ESystemState )
        {
        iSipDeviceAwareObject->EventProcessingCompleted(aObserver);
        }
    // SIP deregistration for VPN session has been completed. 
    else if ( FeatureManager::FeatureSupported( KFeatureIdFfImsDeregistrationInVpn )
         && ( aVariable == EVpnState ) 
         && iVpnMonitor )
        {
        iVpnMonitor->EventProcessingCompleted(aObserver);
        }
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorImpl::FindSnapMonitorById
// -----------------------------------------------------------------------------
//
CSipSnapAvailabilityMonitor* CSipSystemStateMonitorImpl::FindSnapMonitorById( 
    TInt aSnapId ) const
    {
    CSipSnapAvailabilityMonitor* monitor = NULL;
    for ( TInt i=0; i < iSnapMonitors.Count() && !monitor; i++ )
        {
        CSipSnapAvailabilityMonitor* tmp = iSnapMonitors[i];
        if ( tmp->SnapId() == aSnapId )
            {
            monitor = tmp;
            }
        }
    return monitor;    
    }
