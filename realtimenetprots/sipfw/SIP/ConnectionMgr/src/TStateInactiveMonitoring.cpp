// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : TStateInactiveMonitoring.cpp
// Part of     : ConnectionMgr
// Implementation
// Version     : SIP/4.0
//



#include "TStateInactiveMonitoring.h"
#include "msipconnectioncontext.h"
#include "SipLogs.h"



const TUint KTimerThreeHours (10800000);//180 minutes.
const TUint KTimerFiveMinutes( 300000 ); // 5minutes
const TUint KBaseInterval( 600000 ); //10minutes
const TUint KTimerSixtyMinutes(3600000); // 60 minutes



// ----------------------------------------------------------------------------
// TStateInactiveMonitoring::Enter
// ----------------------------------------------------------------------------
//
void TStateInactiveMonitoring::Enter( TInt aError )
	{
	__SIP_LOG( "TStateInactiveMonitoring::Enter" )

	iContext.ReleaseTransportResources( ETrue );	
	
	TRAPD( err, iContext.StartMonitoringL( aError ) );
	if( err )
	{
	EnterState( MStateModel::EUnavailable, err );
	return;
	}

	if(aError && (iContext.ConnectionType()==MSIPConnectionContext::ELanBearer))
	{
	__SIP_INT_LOG1( "TStateInactiveMonitoring::Enter, Retrying for WLAN Connection", aError)
	StartRecovery(aError);
	}	 
	}

// ----------------------------------------------------------------------------
// TStateInactiveMonitoring::NetworkStateChanged
// ----------------------------------------------------------------------------
//
void TStateInactiveMonitoring::NetworkStateChanged(
    MSIPNetworkObserver::TNetworkState aState,
    TInt aError )
	{
	__SIP_INT_LOG2( "TStateInactiveMonitoring::NetworkStateChanged", aState, aError )
	if( aError )
		{
		if(iContext.ConnectionType()==MSIPConnectionContext::ELanBearer)
		{
		EnterState( MStateModel::EInactiveMonitoring, aError ); // For Any Error on WLAN, Enable Recovery Timers.
		}
		else
		{
		EnterState( MStateModel::EUnavailable, aError ); //For other bearers No recovery possible.
		}
		return;
		}

	if( aState == MSIPNetworkObserver::ENetworkActive )
		{
		EnterState( MStateModel::EInactiveConnecting );
		}
	if( aState == MSIPNetworkObserver::ENetworkConnected )
		{
		ResetTimer();
		EnterState( MStateModel::EActiveResolving );
		} 
	}
	
void TStateInactiveMonitoring::	StartTimer( TInt aTimeout )
{	__SIP_LOG( "TStateInactiveMonitoring::StartTimer" )
	__SIP_INT_LOG1("TStateInactiveMonitoring::StartTimer: Value",aTimeout)
	iTimer.Stop( iTimerId );
	
	TRAPD( err, iTimerId = iTimer.StartL( this, aTimeout ) );
	if( err )
		{
		EnterState( MStateModel::EUnavailable, err );
		}	

}

void TStateInactiveMonitoring::	TimerExpiredL( TTimerId /*aTimerId*/,
											   TAny* /*aTimerParam */)
{ 	
	if(iLessFatal)
		{
		iRetryCount++;
		iLessFatal=EFalse;
		}

	__SIP_LOG( "TStateInactiveMonitoring::TimerExpiredL" )
	TInt err = iContext.OpenConnection();	
	if( err )
		{
		EnterState( MStateModel::EUnavailable, err );
		}	
}

void TStateInactiveMonitoring::Exit()
	{
	iTimer.Stop( iTimerId );	
	}

void TStateInactiveMonitoring::StartRecovery(TInt aError)
{
if(iContext.ContinueMonitoring(aError))
	{
	   if(iRetryCount<3)
          {
          iLessFatal = ETrue;
	   __SIP_INT_LOG2( "TStateInactiveMonitoring::Recovering For less Fatal", aError, iRetryCount )
	   StartTimer(KTimerFiveMinutes + (KBaseInterval *iRetryCount)); //Retry Timer in increasing interval for less Fatal Errors.
          }
	  else
	   {
	   __SIP_LOG( "TStateInactiveMonitoring::Recovering For Less Fatal once every 60 mins...." )
	    StartTimer(KTimerSixtyMinutes);//Retry Timer for 60 minutes
	   }	
	 }		
else
 	{
 	__SIP_LOG( "TStateInactiveMonitoring::Recovering for More Fatal" )
	StartTimer(KTimerThreeHours);    // Retry Timer for More Fatal Errors.
 	}		
}


void TStateInactiveMonitoring::ResetTimer()
{__SIP_LOG( "TStateInactiveMonitoring::ResetTimer" )
iRetryCount=0;
 iLessFatal = EFalse;
 __SIP_INT_LOG2( "TStateInactiveMonitoring::Reset Value...", iRetryCount, iLessFatal )
}
