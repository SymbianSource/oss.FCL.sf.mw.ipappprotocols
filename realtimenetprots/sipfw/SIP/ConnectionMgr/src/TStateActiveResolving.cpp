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
// Name        : TStateActiveResolving.cpp
// Part of     : ConnectionMgr
// Implementation
// Version     : SIP/4.0
//



#include "TStateActiveResolving.h"
#include "msipconnectioncontext.h"
#include "SipLogs.h"

// ----------------------------------------------------------------------------
// TStateActiveResolving::Enter
// ----------------------------------------------------------------------------
//
void TStateActiveResolving::Enter( TInt /*aError*/ )
	{
	__SIP_LOG( "TStateActiveResolving::Enter" )
	iTimerValue = KInitialResolvingTimer;
	iCumulativeTimerValue = KInitialResolvingTimer;
	TInt err( KErrNone );

    // Pass KErrNone for StartMonitoringL at this stage
    // since error is needed only for context availability
    // monitoring which is at this point unnecessary
	if ( !iContext.DoTransportResourcesExist() )
	    {
	    TRAP( err,
    		{
    		iContext.CreateTransportResourcesL();
    		iContext.StartMonitoringL( KErrNone );
    		} );
    	if ( err )
    		{
    		EnterState( MStateModel::EUnavailable, err );
    		return;
    		}
	    }

	if ( HasAddress() )
		{
		// Address has to exist, otherwise resolving hasn't been done at this
		// point and we cannot enter to resolved state
		EnterActiveResolvedState();
		}
	else
		{		
		TRAP( err, iTimerId = iTimer.StartL( this, KInitialResolvingTimer ) );
		if( err )
			{
			EnterState( MStateModel::EUnavailable, err );
			}
		}
	}

// ----------------------------------------------------------------------------
// TStateActiveResolving::Exit
// ----------------------------------------------------------------------------
//
void TStateActiveResolving::Exit()

	{
	iTimer.Stop( iTimerId );
	}

// ----------------------------------------------------------------------------
// TStateActiveResolving::NetworkStateChanged
// ----------------------------------------------------------------------------
//
void TStateActiveResolving::NetworkStateChanged(
    MSIPNetworkObserver::TNetworkState aState,
    TInt aError )
	{
	  __SIP_INT_LOG2( "TStateActiveResolving::NetworkStateChanged", aState, aError )
	if( aError)
		{
		if (iContext.ConnectionType()==MSIPConnectionContext::ELanBearer)
		{
		EnterState( MStateModel::EInactiveMonitoring, aError ); // For Any Error on WLAN, Enable Recovery Timers.
		}
		else
		{
		EnterState( MStateModel::EUnavailable, aError ); //For other bearers No recovery possible.
		}
		return;
		}
	if( aState == MSIPNetworkObserver::ENetworkSuspended )
		{
		EnterState( MStateModel::ESuspended );
		}
	else if( aState == MSIPNetworkObserver::ENetworkInactive )
		{
		EnterState( MStateModel::EInactiveMonitoring );
		}
	else if( aState == MSIPNetworkObserver::EConnectionLost )
		{
		EnterState( MStateModel::EInactiveConnecting );
		}		
	else
		{
		// do nothing, "else" required by PC-Lint..
		}
	}

// ----------------------------------------------------------------------------
// TStateActiveResolving::TimerExpiredL
// ----------------------------------------------------------------------------
//
void TStateActiveResolving::TimerExpiredL( TTimerId /*aTimerId*/,
								 		   TAny* /*aTimerParam*/ )
	{
    if( HasAddress() )
        {
        EnterActiveResolvedState();
        }
    else
	    {
		iTimerId = iTimer.StartL( this, CalculateTimerValue() );
		}
	}

// ----------------------------------------------------------------------------
// TStateActiveResolving::HasAddress
// ----------------------------------------------------------------------------
//
TBool TStateActiveResolving::HasAddress()
	{
	TInetAddr address;
	iContext.ResolveLocalAddress( address );
	return ( !address.IsWildAddr() );
	}

// ----------------------------------------------------------------------------
// TStateActiveResolving::CalculateTimerValue
// ----------------------------------------------------------------------------
//
TUint TStateActiveResolving::CalculateTimerValue()
    {
    if ( iCumulativeTimerValue >= KStartDoublingTimerValue )
        {
        if ( iTimerValue < KMaximumResolvingTimer )
            {
            iTimerValue *= 2;
            }
        }
    iCumulativeTimerValue += iTimerValue;    
    return iTimerValue;
    }

// ----------------------------------------------------------------------------
// TStateActiveResolving::EnterActiveResolvedState
// ----------------------------------------------------------------------------
//
void TStateActiveResolving::EnterActiveResolvedState()
    {
    TRAPD( err, iContext.CreateDefaultTransportsL() )
    if ( err )
        {
        EnterState( MStateModel::EUnavailable, err );
        }
    else
        {
        EnterState( MStateModel::EActiveResolved );
        }
    }
