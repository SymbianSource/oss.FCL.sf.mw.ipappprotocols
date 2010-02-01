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
#include "CSystemStateConnUsagePermissionMonitor.h"
#include "sipdevicestateaware.h"
#include "siprfsmonitorao.h"

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
    iUsagePermissionMonitor = 
        CSystemStateConnUsagePermissionMonitor::NewL();
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
    delete iMonitorAo;
    iSnapMonitors.ResetAndDestroy();
    delete iUsagePermissionMonitor;
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
            TInt permissionToUseNetwork = 
                iUsagePermissionMonitor->CurrentUsagePermission();
            User::LeaveIfError( permissionToUseNetwork ); 
            monitor = CSipSnapAvailabilityMonitor::NewLC( 
                aObjectId, permissionToUseNetwork, aObserver );
            iUsagePermissionMonitor->AddObserverL( *monitor );
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
            iUsagePermissionMonitor->RemoveObserver( *monitor );
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
            {
            TInt permissionToUseNetwork = 
                iUsagePermissionMonitor->CurrentUsagePermission();
            if ( permissionToUseNetwork < 0 )
                {
                return permissionToUseNetwork;
                }        
            return permissionToUseNetwork && monitor->SnapAvailability();
            }
        }
    else if ( aVariable == ERfsState )
        {
        if(iRfsMonitor)
            iRfsMonitor->State();
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
