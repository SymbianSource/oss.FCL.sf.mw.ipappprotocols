/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description : P&S key monitor for communication between SIP Profile 
*               server and VPN client
* Name        : sipvpnmonitorao.cpp
* Part of     : Sip System State Monitor
* Version     : 1.0
*
*/

//  INCLUDE FILES
#include "sipvpnmonitorao.h"
#include <vpnsipobserverpskeys.h>
#include <sipsystemstateobserver.h>

_LIT_SECURITY_POLICY_PASS( KSIPVpnAlwaysPass );

static const TInt KMicroSecondsInSecond = 1000000;
static const TInt KGuardTimerSeconds = 10; 

// -----------------------------------------------------------------------------
// CSipVpnMonitorAo::NewL
// -----------------------------------------------------------------------------
//
CSipVpnMonitorAo* CSipVpnMonitorAo::NewL()
    {
    CSipVpnMonitorAo* self = new( ELeave )CSipVpnMonitorAo();
    
    CleanupStack::PushL ( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// CSipVpnMonitorAo::ConstructL
// -----------------------------------------------------------------------------
//
void CSipVpnMonitorAo::ConstructL()
    {
    iGuardTimer = CPeriodic::NewL( EPriorityNormal );
    
    // Define a P&S key for communication between SIP Profile Server and VPN client.
    TInt err = iProperty.Define( KPSVpnSipUid, KVpnSipState, RProperty::EInt, 
                                 KSIPVpnAlwaysPass, KSIPVpnAlwaysPass );
    if ( KErrNone != err && KErrAlreadyExists != err && 
            KErrPermissionDenied != err )
        {
        User::Leave( err );
        }

    User::LeaveIfError( iProperty.Attach( KPSVpnSipUid, KVpnSipState ) );

    iProperty.Subscribe( iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CSipVpnMonitorAo::CSipVpnMonitorAo
// -----------------------------------------------------------------------------
//
CSipVpnMonitorAo::CSipVpnMonitorAo():
    CActive(EPriorityStandard)
    {
    CActiveScheduler::Add( this );
    iCount = 0;
    }

// -----------------------------------------------------------------------------
// CSipVpnMonitorAo::~CSipVpnMonitorAo
// -----------------------------------------------------------------------------
//
CSipVpnMonitorAo::~CSipVpnMonitorAo()
    {
    if ( iGuardTimer )
        {
        iGuardTimer->Cancel();
        delete iGuardTimer;
		iGuardTimer = NULL;
        }
        
    CActive::Cancel();
    
    iProperty.Close();
    iProperty.Delete( KPSVpnSipUid, KVpnSipState );
    
    iObservers.Close();
    }

// -----------------------------------------------------------------------------
// CSipVpnMonitorAo::AddObserverL
// -----------------------------------------------------------------------------
//
void CSipVpnMonitorAo::AddObserverL( 
    MSipSystemStateObserver& aObserver )
    {
    iObservers.InsertInAddressOrderL( &aObserver );
    }

// -----------------------------------------------------------------------------
// CSipVpnMonitorAo::RemoveObserver
// -----------------------------------------------------------------------------
//    
void CSipVpnMonitorAo::RemoveObserver(
    MSipSystemStateObserver& aObserver )
    {
    TInt index = iObservers.Find( &aObserver );
    if ( index >= 0 )
        {
        iObservers.Remove( index ); 
        }
    }

// -----------------------------------------------------------------------------
// CSipVpnMonitorAo::NotifyObservers
// -----------------------------------------------------------------------------
//
void CSipVpnMonitorAo::NotifyObservers()
    {
    // Notify observers (SIP Profile Server) about the P&S key change.
    for ( TInt i = iObservers.Count()-1; i >= 0; i-- )
        {
        iObservers[i]->SystemVariableUpdated( 
            CSipSystemStateMonitor::EVpnState, 
            0,
            iState);
        }

    // Start a guard timer so that VPN client don't wait forever for completion
    // of deregistration.
    if ( iObservers.Count() && iState == CSipSystemStateMonitor::EVpnInitiating )
        {
        iGuardTimer->Cancel();
        iGuardTimer->Start( 
            TTimeIntervalMicroSeconds32( KGuardTimerSeconds * KMicroSecondsInSecond ),
            TTimeIntervalMicroSeconds32( KGuardTimerSeconds * KMicroSecondsInSecond ),
            TCallBack( TimerExpired, this ) );
        }
    }

// -----------------------------------------------------------------------------
// CSipVpnMonitorAo::State
// -----------------------------------------------------------------------------
//
CSipSystemStateMonitor::TVpnState CSipVpnMonitorAo::State() const
    {
    return iState;
    }

// -----------------------------------------------------------------------------
// CSipVpnMonitorAo::EventProcessingCompleted
// -----------------------------------------------------------------------------
//
void CSipVpnMonitorAo::EventProcessingCompleted(
        MSipSystemStateObserver& aObserver )
    {
    if (iState == CSipSystemStateMonitor::EVpnInitiating)
        {
            TInt index = iObservers.Find( &aObserver );
            if ( index >= 0 )
            {
                iCount++;
                if( iObservers.Count() == iCount)
                {
                    iGuardTimer->Cancel();
                    iProperty.Set(KPSVpnSipUid, KVpnSipState, ESipDeregisterCompleted );
                    iCount = 0;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CSipVpnMonitorAo::EventProcessingCompleted
// -----------------------------------------------------------------------------
//
void CSipVpnMonitorAo::EventProcessingCompleted()
    {
    // SIP deregistration has been completed. Stop the guard timer and tell
    // the VPN client about it.
    iGuardTimer->Cancel();
    if (iState == CSipSystemStateMonitor::EVpnInitiating)
        {
        iProperty.Set( KPSVpnSipUid, KVpnSipState, ESipDeregisterCompleted );
        iCount = 0;
        }
    }

// -----------------------------------------------------------------------------
// CSipVpnMonitorAo::TimerExpired
// -----------------------------------------------------------------------------
//
TInt CSipVpnMonitorAo::TimerExpired(TAny* aSelf)
    {
    // Guard timer expired. Tell VPN client to proceed its work without
    // further waiting.
    CSipVpnMonitorAo* self = reinterpret_cast<CSipVpnMonitorAo*>(aSelf);
    self->EventProcessingCompleted();
    
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CSipVpnMonitorAo::RunL
// -----------------------------------------------------------------------------
//
void CSipVpnMonitorAo::RunL()
    {
    TInt state( 0 );  
    
    // VPN client notifies that it has started a VPN session.
    if ( KErrNone == iProperty.Get( state ) )
        {
        if ( MappedState( state ) )
            {
            NotifyObservers();
            }
        }
        
    iProperty.Subscribe( iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CSipVpnMonitorAo::RunError
// -----------------------------------------------------------------------------
//     
TInt CSipVpnMonitorAo::RunError( TInt /*aError*/ )
    {
    return KErrNone; // RunL cannot leave at the moment
    }

// -----------------------------------------------------------------------------
// CSipVpnMonitorAo::DoCancel
// -----------------------------------------------------------------------------
//  
void CSipVpnMonitorAo::DoCancel()
    {
    iProperty.Cancel();
    }

// -----------------------------------------------------------------------------
// CSipVpnMonitorAo::MappedState
// -----------------------------------------------------------------------------
//
TBool CSipVpnMonitorAo::MappedState( TInt aState )
    {
    TBool ret = ETrue;
    // Maps P&S key value to VPN state.
    switch( aState )
        {
        case EVpnInitiating:
            iState = CSipSystemStateMonitor::EVpnInitiating;
            break;
        case EVpnTerminated:
            iState = CSipSystemStateMonitor::EVpnTerminated;
            break;
        // Other P&S key values are not mapped to VPN state.
        // Not an error situation.
        default:
            ret = EFalse;
        }
        
    return ret;
    }
