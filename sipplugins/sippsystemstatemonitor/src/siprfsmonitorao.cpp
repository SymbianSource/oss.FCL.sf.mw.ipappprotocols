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
#include "siprfsmonitorao.h"
#include <sipsystemstatemonitorpskeys.h>
#include <sipsystemstateobserver.h>

_LIT_SECURITY_POLICY_PASS( KSIPRfsAlwaysPass );

// -----------------------------------------------------------------------------
// CSipRfsMonitorAo::NewL
// -----------------------------------------------------------------------------
//
CSipRfsMonitorAo* CSipRfsMonitorAo::NewL()
    {
    CSipRfsMonitorAo* self = new( ELeave )CSipRfsMonitorAo();
    CleanupStack::PushL ( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSipRfsMonitorAo::ConstructL
// -----------------------------------------------------------------------------
//
void CSipRfsMonitorAo::ConstructL ()
    {
    TInt err = iProperty.Define( KPSSipRfsUid, KSipRfsState, RProperty::EInt,
				   KSIPRfsAlwaysPass, KSIPRfsAlwaysPass);
    if ( KErrNone != err && KErrAlreadyExists != err && 
            KErrPermissionDenied != err )
        {
        User::Leave( err );
        }
    User::LeaveIfError( iProperty.Attach(KPSSipRfsUid, KSipRfsState));
    iProperty.Subscribe( iStatus );
    SetActive();

    }

// -----------------------------------------------------------------------------
// CSipRfsMonitorAo::CSipRfsMonitorAo
// -----------------------------------------------------------------------------
//
CSipRfsMonitorAo::CSipRfsMonitorAo():
    CActive(EPriorityStandard)
    {
    CActiveScheduler::Add( this );
    iCount = 0;
    }

// -----------------------------------------------------------------------------
// CSipRfsMonitorAo::~CSipRfsMonitorAo
// -----------------------------------------------------------------------------
//
CSipRfsMonitorAo::~CSipRfsMonitorAo()
    {
    CActive::Cancel();
    iProperty.Close();
	iProperty.Delete(KPSSipRfsUid,KSipRfsState);
    iObservers.Close();
    }

// -----------------------------------------------------------------------------
// CSipRfsMonitorAo::AddObserverL
// -----------------------------------------------------------------------------
//
void CSipRfsMonitorAo::AddObserverL( 
    MSipSystemStateObserver& aObserver )
    {
    iObservers.InsertInAddressOrderL( &aObserver );
    }

// -----------------------------------------------------------------------------
// CSipRfsMonitorAo::RemoveObserver
// -----------------------------------------------------------------------------
//    
void CSipRfsMonitorAo::RemoveObserver(
    MSipSystemStateObserver& aObserver )
    {
    TInt index = iObservers.Find( &aObserver );
    if ( index >= 0 )
        {
        iObservers.Remove( index ); 
        }
    }

// -----------------------------------------------------------------------------
// CSipRfsMonitorAo::NotifyObservers
// -----------------------------------------------------------------------------
//
void CSipRfsMonitorAo::NotifyObservers()
    {
    for ( TInt i = iObservers.Count()-1; i >= 0; i-- )
        {
        iObservers[i]->SystemVariableUpdated( 
            CSipSystemStateMonitor::ERfsState, 
            0,
            iState);
        }           
    }

// -----------------------------------------------------------------------------
// CSipRfsMonitorAo::State
// -----------------------------------------------------------------------------
//
CSipSystemStateMonitor::TRfsState CSipRfsMonitorAo::State() const
    {
    return iState;
    }

// -----------------------------------------------------------------------------
// CSipRfsMonitorAo::EventProcessingCompleted
// -----------------------------------------------------------------------------
//
void CSipRfsMonitorAo::EventProcessingCompleted(
        MSipSystemStateObserver& aObserver )
    {
    if (iState == CSipSystemStateMonitor::ERfsStarted)
        {
            TInt index = iObservers.Find( &aObserver );
            if ( index >= 0 )
            {
                iCount++;
                if( iObservers.Count() == iCount)
                {
                    iProperty.Set(KPSSipRfsUid, KSipRfsState, ESipRfsEventProcessingCompleted );
                    iCount = 0;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CSipRfsMonitorAo::RunL
// -----------------------------------------------------------------------------
//
void CSipRfsMonitorAo::RunL()
    {
    TInt state ( 0 );  
    if ( KErrNone == iProperty.Get( state ) )
        {
        if (MappedState(state))
            NotifyObservers();
        }
    iProperty.Subscribe( iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CSipRfsMonitorAo::RunError
// -----------------------------------------------------------------------------
//     
TInt CSipRfsMonitorAo::RunError( TInt /*aError*/ )
    {
    return KErrNone; // RunL cannot leave at the moment
    }

// -----------------------------------------------------------------------------
// CSipRfsMonitorAo::DoCancel
// -----------------------------------------------------------------------------
//  
void CSipRfsMonitorAo::DoCancel()
    {
    iProperty.Cancel();
    }

// -----------------------------------------------------------------------------
// CSipRfsMonitorAo::MappedState
// -----------------------------------------------------------------------------
//
TBool CSipRfsMonitorAo::MappedState(TInt aState)
    {
    TBool ret = ETrue;
    switch(aState)
        {
        case CSipSystemStateMonitor::ERfsStarted:
            iState = CSipSystemStateMonitor::ERfsStarted;
            break;
        case CSipSystemStateMonitor::ERfsFailed:
            iState = CSipSystemStateMonitor::ERfsFailed;
            break;
        case CSipSystemStateMonitor::ERfsCompleted:
            iState = CSipSystemStateMonitor::ERfsCompleted;
            break;
        default:
            ret = EFalse;
        }
    return ret;
    }
