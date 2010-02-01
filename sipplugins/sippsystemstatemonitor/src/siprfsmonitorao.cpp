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

static const TInt KMicroSecondsInSecond = 1000000;
static const TInt KGuardTimerSeconds = 20;

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
    iGuardTimer = CPeriodic::NewL( EPriorityNormal );
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
	if(iGuardTimer)
		{
    	iGuardTimer->Cancel();
    	delete iGuardTimer;
		iGuardTimer = NULL;
		}
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
    if(iObservers.Count() && iState == CSipSystemStateMonitor::ERfsStarted)
        {
        iGuardTimer->Cancel();
        iGuardTimer->Start(
			TTimeIntervalMicroSeconds32( KGuardTimerSeconds * KMicroSecondsInSecond ),
            TTimeIntervalMicroSeconds32( KGuardTimerSeconds * KMicroSecondsInSecond ),
            TCallBack( TimerExpired, this ) );
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
        MSipSystemStateObserver& /*aObserver*/ )
    {
    }

// -----------------------------------------------------------------------------
// CSipRfsMonitorAo::EventProcessingCompleted
// -----------------------------------------------------------------------------
//
void CSipRfsMonitorAo::EventProcessingCompleted()
    {
    iGuardTimer->Cancel();
    iProperty.Set(KPSSipRfsUid, KSipRfsState, ESipRfsEventProcessingCompleted );
    iCount = 0;
    }

// -----------------------------------------------------------------------------
// CSipRfsMonitorAo::TimerExpired
// -----------------------------------------------------------------------------
//
TInt CSipRfsMonitorAo::TimerExpired(TAny* aSelf)
    {
    CSipRfsMonitorAo* self = reinterpret_cast<CSipRfsMonitorAo*>(aSelf);
    self->EventProcessingCompleted();
    return ETrue;
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
