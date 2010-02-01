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
// Name        : sipsystemstatemonitorimpl.cpp
// Part of     : SIP / SIP System State Monitor
// Version     : SIP/6.0
//



//  INCLUDE FILES
#include "sipsystemstatemonitorimpl.h"

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
        }
    else if ( aVariable == ESnapAvailability )
        {
        CSipSnapAvailabilityMonitor* monitor = FindSnapMonitorById( aObjectId );
        if ( !monitor )
            {
            monitor = CSipSnapAvailabilityMonitor::NewLC( aObjectId, aObserver );
            iSnapMonitors.AppendL( monitor );
            CleanupStack::Pop( monitor );
            }
        else
            {
            monitor->AddObserverL( aObserver );
            }
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
        }
    if ( aVariable == ESnapAvailability )
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
    if ( aVariable == ESnapAvailability )
        {
        CSipSnapAvailabilityMonitor* monitor = FindSnapMonitorById( aObjectId );
        if ( monitor )
            {
            return monitor->SnapAvailability();
            }
        }        
    return KErrNotFound;
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
