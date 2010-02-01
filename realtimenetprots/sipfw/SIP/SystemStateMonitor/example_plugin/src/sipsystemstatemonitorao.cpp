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
// Name        : sipsystemstatemonitorao.cpp
// Part of     : SIP / SIP System State Monitor
// Version     : SIP/6.0
//



//  INCLUDE FILES
#include "sipsystemstatemonitorao.h"

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorAo::NewL
// -----------------------------------------------------------------------------
//
CSipSystemStateMonitorAo* CSipSystemStateMonitorAo::NewL()
    {
    CSipSystemStateMonitorAo* self = new( ELeave )CSipSystemStateMonitorAo();
    CleanupStack::PushL ( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorAo::ConstructL
// -----------------------------------------------------------------------------
//
void CSipSystemStateMonitorAo::ConstructL()
    {
    // Check the current system state and set iState accordingly.
    // Subscribe to an asynchronous service that provides the system state
    // information.
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorAo::CSipSystemStateMonitorAo
// -----------------------------------------------------------------------------
//
CSipSystemStateMonitorAo::CSipSystemStateMonitorAo():
	CActive(EPriorityStandard)
    {
	CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorAo::~CSipSystemStateMonitorAo
// -----------------------------------------------------------------------------
//
CSipSystemStateMonitorAo::~CSipSystemStateMonitorAo()
    {
	CActive::Cancel();
	iObservers.Close();
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorAo::State
// -----------------------------------------------------------------------------
//
CSipSystemStateMonitor::TSystemState CSipSystemStateMonitorAo::State() const
    {
    return iState;
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorAo::AddObserverL
// -----------------------------------------------------------------------------
//
void CSipSystemStateMonitorAo::AddObserverL( 
    MSipSystemStateObserver& aObserver )
    {
    // Add observer to the observer list
    iObservers.InsertInAddressOrderL( &aObserver );
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorAo::RemoveObserver
// -----------------------------------------------------------------------------
//
void CSipSystemStateMonitorAo::RemoveObserver(
    MSipSystemStateObserver& aObserver )
    {
    // Remove observer from the observer list
    TInt index = iObservers.Find( &aObserver );
    if ( index >= 0 )
        {
        iObservers.Remove( index ); 
        }
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorAo::RunL
// -----------------------------------------------------------------------------
//
void CSipSystemStateMonitorAo::RunL()
    {
    // Set value for iState according to the changed system state, 
    // re-subscribe to the asynchronous service that provides the system state
    // information and call CSipSystemStateMonitorAo::NotifyObservers.
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorAo::RunError
// -----------------------------------------------------------------------------
//
TInt CSipSystemStateMonitorAo::RunError( TInt /*aError*/ )
    {
    return KErrNone; // RunL cannot leave at the moment
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorAo::DoCancel
// -----------------------------------------------------------------------------
//
void CSipSystemStateMonitorAo::DoCancel()
    {

    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorAo::NotifyObservers
// -----------------------------------------------------------------------------
//
void CSipSystemStateMonitorAo::NotifyObservers()
    {
    // Notify each observer of the change to the SIP System State
    for ( TInt i = iObservers.Count()-1; i >= 0; i-- )
        {
        iObservers[i]->SystemVariableUpdated( 
            CSipSystemStateMonitor::ESystemState, 
            0,
            iState );
        }     
    }
