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
#include "sipsystemstatemonitorao.h"
#include <startupdomainpskeys.h>

#ifndef RD_STARTUP_CHANGE
    #include <sysstartup.h>
    const TInt KSipSystemShuttingDown = ESWStateShuttingDown;
#else
    const TInt KSipSystemShuttingDown = ESwStateShuttingDown;
#endif

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
void CSipSystemStateMonitorAo::ConstructL ()
    {
	User::LeaveIfError( 
	    iProperty.Attach( KPSUidStartup, KPSGlobalSystemState ) );
	TInt state ( 0 );    
	if ( iProperty.Get( state ) == KErrNone && IsSystemReady( state ) )
	    {
        iState = CSipSystemStateMonitor::ESystemReady;
	    }
	iProperty.Subscribe( iStatus );
	SetActive();
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
	iProperty.Close();
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
    iObservers.InsertInAddressOrderL( &aObserver );
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorAo::RemoveObserver
// -----------------------------------------------------------------------------
//    
void CSipSystemStateMonitorAo::RemoveObserver(
    MSipSystemStateObserver& aObserver )
    {
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
	TInt state ( 0 );  
	if ( KErrNone == iProperty.Get( state ) )
        {
        if ( IsSystemReady( state ) )
            {
            iState = CSipSystemStateMonitor::ESystemReady;
            NotifyObservers();
            }
        else
            {
            if ( state == KSipSystemShuttingDown )
                {
			    iState = CSipSystemStateMonitor::ESystemShuttingDown;
                NotifyObservers();
				return; // Stop monitoring
                }
			}
		}
    iProperty.Subscribe( iStatus );
	SetActive();	
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
	iProperty.Cancel();
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorAo::NotifyObservers
// -----------------------------------------------------------------------------
//
void CSipSystemStateMonitorAo::NotifyObservers()
    {
    for ( TInt i = iObservers.Count()-1; i >= 0; i-- )
        {
        iObservers[i]->SystemVariableUpdated( 
            CSipSystemStateMonitor::ESystemState, 
            0,
            iState );
        }     
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitorAo::IsSystemReady
// -----------------------------------------------------------------------------
//
TBool CSipSystemStateMonitorAo::IsSystemReady( TInt aSystemState ) const
    {
    return ( aSystemState == ESwStateNormalRfOn ||
             aSystemState == ESwStateNormalRfOff ||
             aSystemState == ESwStateNormalBTSap ||
	         aSystemState == ESwStateEmergencyCallsOnly ||
             aSystemState == ESwStateSecurityCheck);
    }
