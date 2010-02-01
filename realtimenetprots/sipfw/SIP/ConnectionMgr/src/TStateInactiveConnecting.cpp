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
// Name        : TStateInactiveConnecting.cpp
// Part of     : ConnectionMgr
// Implementation
// Version     : SIP/4.0
//



#include "TStateInactiveConnecting.h"
#include "msipconnectioncontext.h"
#include "SipLogs.h"

// 20000 ms = twenty seconds
const TUint KTimerTwentySeconds( 20000 );

// 4000 ms = four seconds
const TUint KTimerFourSeconds( 4000 );

// ----------------------------------------------------------------------------
// TStateInactiveConnecting::Enter
// ----------------------------------------------------------------------------
//
void TStateInactiveConnecting::Enter( TInt /*aError*/ )
	{
	
	__SIP_LOG( "TStateInactiveConnecting::Enter" )
	// Don't release all resources since we have to monitor connection's
	// total closure.
	iContext.ReleaseTransportResources( EFalse );
	
    // Start waiting for total RConnection closure. If connection is tried to
	// be started right after it started going down, restarting may
	// fail unintentionally.
	TInt err = iContext.MonitorConnectionClosure();
	if( err )
		{
		EnterState( MStateModel::EUnavailable, err );
		return;
		}
		
    // Start also guard timer which will cause connection attempt after
    // long waiting (i.e. connection closure event wasn't never received). 
	StartTimer( KTimerTwentySeconds );		
	}
	
// ----------------------------------------------------------------------------
// TStateInactiveConnecting::Exit
// ----------------------------------------------------------------------------
//
void TStateInactiveConnecting::Exit()
	{
	iTimer.Stop( iTimerId );	
	}

// ----------------------------------------------------------------------------
// TStateInactiveConnecting::NetworkStateChanged
// ----------------------------------------------------------------------------
//
void TStateInactiveConnecting::NetworkStateChanged(
    MSIPNetworkObserver::TNetworkState aState,
    TInt aError )
	{
	 __SIP_INT_LOG2( "TStateInactiveConnecting::NetworkStateChanged", aState, aError )
	// Some connection types might not want to try anymore monitoring at this 
	// point, instead they go straight to unavailable state if error occured 
	if ( aError && !iContext.ContinueMonitoring( aError ) && (iContext.ConnectionType()!=MSIPConnectionContext::ELanBearer) )
	    {
	    EnterState( MStateModel::EUnavailable, aError );
		return;
	    }
	
	// Some connection types will continue monitoring even if error occured     
    if( aError ||
        aState == MSIPNetworkObserver::ENetworkInactive ||
	    aState == MSIPNetworkObserver::ENetworkSuspended )
		{
		// Needed to recover from offline mode:
		EnterState( MStateModel::EInactiveMonitoring, aError );
		}
    else if ( aState == MSIPNetworkObserver::EConnectionLost )
	    {
	    // Connection should be ready to be started. Wait still a bit.
	    StartTimer( KTimerFourSeconds );
	    }
	else if( aState == MSIPNetworkObserver::ENetworkConnected )
		{
		EnterState( MStateModel::EActiveResolving );
		} 
	else
		{
		// do nothing, "else" required by PC-Lint..
		}					
	}
	
// ----------------------------------------------------------------------------
// TStateInactiveConnecting::TimerExpiredL
// ----------------------------------------------------------------------------
//
void TStateInactiveConnecting::TimerExpiredL( TTimerId /*aTimerId*/,
								 		      TAny* /*aTimerParam*/ )
	{	
	
	__SIP_LOG( "TStateInactiveConnecting::TimerExpiredL" )
    OpenConnection();
	}

// ----------------------------------------------------------------------------
// TStateInactiveConnecting::OpenConnection
// ----------------------------------------------------------------------------
//	
void TStateInactiveConnecting::OpenConnection()
	{
	TInt err = iContext.OpenConnection();	
	if( err )
		{
		EnterState( MStateModel::EUnavailable, err );
		}	
	}
	
// ----------------------------------------------------------------------------
// TStateInactiveConnecting::StartTimer
// ----------------------------------------------------------------------------
//	
void TStateInactiveConnecting::StartTimer( TInt aTimeout )
	{
	iTimer.Stop( iTimerId );
	
	TRAPD( err, iTimerId = iTimer.StartL( this, aTimeout ) );
	if( err )
		{
		EnterState( MStateModel::EUnavailable, err );
		}	
	}
