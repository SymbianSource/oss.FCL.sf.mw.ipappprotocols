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
* Description:  Class Implementation, aware of Device System State Changes and perform
*                processing related to profile registration/deregistration
*
*/


#include "sipdevicestateaware.h"
#include <sipsystemstateobserver.h>
#include <ssm/ssmdomaindefs.h>
#include <ssm/ssmstate.h>
#include <ssm/ssmsubstates.hrh>
#include <e32std.h> 

static const TInt KMicroSecondsInSecond = 1000000;
static const TInt KGuardTimerSeconds = 5;

// -----------------------------------------------------------------------------
// CSipDeviceStateAware::NewL
// -----------------------------------------------------------------------------
//
CSipDeviceStateAware* CSipDeviceStateAware::NewL ()
    {
    CSipDeviceStateAware* self = new (ELeave) CSipDeviceStateAware();
    CleanupStack::PushL (self);
    self->ConstructL ();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSipDeviceStateAware::ConstructL
// -----------------------------------------------------------------------------
//
void CSipDeviceStateAware::ConstructL()
	{
	iGuardTimer = CPeriodic::NewL( EPriorityNormal );
    //SIP System connects to Domain Id GenMiddlewareDomain3    
    User::LeaveIfError(iStateAwareSession.Connect(KSM2GenMiddlewareDomain3));
    TSsmState currentState = iStateAwareSession.State();
    if(ESsmNormal == currentState.MainState())
        {
        iState = CSipSystemStateMonitor::ESystemReady;
        }
    iStateAwareSession.RequestStateNotification(iStatus);        
    SetActive();
    }

// -----------------------------------------------------------------------------
// CSipDeviceStateAware::CSipDeviceStateAware
// -----------------------------------------------------------------------------
//
CSipDeviceStateAware::CSipDeviceStateAware():CActive(EPriorityStandard)
    {    
    CActiveScheduler::Add(this);
	iState = CSipSystemStateMonitor::ESystemNotReady;
    iCount = 0;
    }

// -----------------------------------------------------------------------------
// CSipDeviceStateAware::~CSipDeviceStateAware
// -----------------------------------------------------------------------------
//
CSipDeviceStateAware::~CSipDeviceStateAware ()
    {
     iGuardTimer->Cancel();
     delete iGuardTimer;
     CActive::Cancel();     
     iObservers.Close();
    }

// -----------------------------------------------------------------------------
// CSipDeviceStateAware::RunL
// Currently this object only monitor for basically three events, Offline, online
// and shutdown. 
// -----------------------------------------------------------------------------
//
void CSipDeviceStateAware::RunL()
    {
	TBool ackAndRequest = ETrue;
    TInt status = iStatus.Int();
    if(KErrNone == status)
        {
            TSsmState currentState = iStateAwareSession.State();
            if(ESsmNormal == currentState.MainState() )
                {                
                switch (currentState.SubState())
                    {
                    // Device is entering into Offline Mode. Hence as part
                    // of staged offline, SIP tries to deregister all profiles.
                    case ESsmNormalRfOffSubState:
                        {
						if(iState == CSipSystemStateMonitor::ESystemOnline)
							{
                       		iState = CSipSystemStateMonitor::ESystemOffline;
                       		NotifyObservers(CSipSystemStateMonitor::ESystemOffline);
							iGuardTimer->Cancel();
							TTimeIntervalMicroSeconds32 time( KGuardTimerSeconds * KMicroSecondsInSecond );
							iGuardTimer->Start( time, time, TCallBack( TimerExpired, this) );                                        
							ackAndRequest = EFalse;
							}
                        break;
                        }
                    //Device entering into online mode, SIP tries to register all
                    //profiles with registration mode 'always on'
                    case ESsmNormalRfOnSubState:
                        {
						if(iState == CSipSystemStateMonitor::ESystemOffline)
							{
                       		NotifyObservers(CSipSystemStateMonitor::ESystemOnline);
							}
                       	iState = CSipSystemStateMonitor::ESystemOnline;
                        break;
                        }
                    default:
                        {                        
                        break;
                        }
                    }//end switch                
                } //end if
			}//end outer if
	if(ackAndRequest)
    	iStateAwareSession.AcknowledgeAndRequestStateNotification(KErrNone, iStatus);
	else
		iStateAwareSession.RequestStateNotification(iStatus);
    SetActive();
    }

// -----------------------------------------------------------------------------
// CSipDeviceStateAware::RunError
// -----------------------------------------------------------------------------
//     
TInt CSipDeviceStateAware::RunError( TInt /*aError*/ )
    {
    return KErrNone; // RunL cannot leave at the moment
    }

// -----------------------------------------------------------------------------
// CSipDeviceStateAware::DoCancel
// -----------------------------------------------------------------------------
//
void CSipDeviceStateAware::DoCancel()
    {
    iStateAwareSession.Close();
    }

// -----------------------------------------------------------------------------
// CSipDeviceStateAware::AddObserverL
// -----------------------------------------------------------------------------
//
void CSipDeviceStateAware::AddObserverL(MSipSystemStateObserver& aObserver)
    {
    iObservers.InsertInAddressOrderL( &aObserver );
    }

// -----------------------------------------------------------------------------
// CSipDeviceStateAware::RemoveObserver
// -----------------------------------------------------------------------------
//
void CSipDeviceStateAware::RemoveObserver(MSipSystemStateObserver& aObserver)
    {
    TInt index = iObservers.Find( &aObserver );
    if ( index >= 0 )
        {
        iObservers.Remove( index ); 
        } 
    }

// -----------------------------------------------------------------------------
// CSipDeviceStateAware::NotifyObservers
// This function will notify all the observers when the state of the phone changes
// -----------------------------------------------------------------------------
//
void CSipDeviceStateAware::NotifyObservers(
				CSipSystemStateMonitor::TSystemState aState) const
    {
    for ( TInt i = iObservers.Count()-1; i >= 0; i-- )
            {
            iObservers[i]->SystemVariableUpdated( 
                    CSipSystemStateMonitor::ESystemState, 
                    0,
                    aState );
            }
    }

// -----------------------------------------------------------------------------
// CSipDeviceStateAware::TimerExpired
// -----------------------------------------------------------------------------
//
TInt CSipDeviceStateAware::TimerExpired(TAny* aSelf)
    {
	CSipDeviceStateAware* self = reinterpret_cast<CSipDeviceStateAware*>(aSelf);
    self->EventProcessingCompleted();
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CSipDeviceStateAware::EventProcessingCompleted
// -----------------------------------------------------------------------------
//
void CSipDeviceStateAware::EventProcessingCompleted(
        MSipSystemStateObserver& aObserver )
    {
    TInt index = iObservers.Find( &aObserver );
    if ( index >= 0 )
        iCount++;
    if( iObservers.Count() == iCount)
        {
        iGuardTimer->Cancel();
        if(iState == CSipSystemStateMonitor::ESystemOffline)
            {
            iStateAwareSession.AcknowledgeStateNotification(KErrNone);        
            iCount = 0;
            }
        }	
    }

// -----------------------------------------------------------------------------
// CSipDeviceStateAware::EventProcessingCompleted
// -----------------------------------------------------------------------------
//
void CSipDeviceStateAware::EventProcessingCompleted()
    {
    iGuardTimer->Cancel();
    if(iState == CSipSystemStateMonitor::ESystemOffline)
        {
        iStateAwareSession.AcknowledgeStateNotification(KErrNone);   
        iCount = 0;
        }
    }

