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
// Name        : CStateModel.cpp
// Part of     : ConnectionMgr
// Implementation
// Version     : SIP/4.0
//



#include "CStateModel.h"
#include "TStateInit.h"
#include "TStateInactiveMonitoring.h"
#include "TStateInactiveConnecting.h"
#include "TStateActiveResolving.h"
#include "TStateActiveResolved.h"
#include "TStateSuspended.h"
#include "TStateUnavailable.h"
#include "msipconnectioncontext.h"
#include "SipLogs.h"

// ----------------------------------------------------------------------------
// CStateModel::NewL
// ----------------------------------------------------------------------------
//
CStateModel* CStateModel::NewL( MSIPConnectionContext& aContext,
						  		MTimerManager& aTimer )
	{
	CStateModel* self = new( ELeave ) CStateModel( aContext, aTimer );

	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );

	return self;
	}

// ----------------------------------------------------------------------------
// CStateModel::~CStateModel
// ----------------------------------------------------------------------------
//
CStateModel::~CStateModel()
	{
	// Lets not call State() if we are not initialized properly
	if( iStates.Count() == EMaxStates )
		{
		State().Exit();
		}
	}

// ----------------------------------------------------------------------------
// CStateModel::SendL
// ----------------------------------------------------------------------------
//
void CStateModel::SendL( const TSIPTransportParams& aParams,
                         CSIPRequest& aRequest,
                         TBool aForceUDP,
                         const CUri8* aOutboundProxy,
						 const TInetAddr& aRemoteAddr,
						 TRequestStatus& aStatus )
	{
	State().SendL( 
	    aParams, aRequest, aForceUDP, aOutboundProxy, aRemoteAddr, aStatus );
	}

// ----------------------------------------------------------------------------
// CStateModel::SendL
// ----------------------------------------------------------------------------
//
void CStateModel::SendL( const TSIPTransportParams& aParams,
                         CSIPResponse& aResponse,
						 TTransactionId aId,
						 TRequestStatus& aStatus )
	{
	State().SendL( aParams, aResponse, aId, aStatus );
	}

// ----------------------------------------------------------------------------
// CStateModel::StateValue
// ----------------------------------------------------------------------------
//
MStateModel::TState CStateModel::StateValue() const
	{
	return iStateValue;
	}

// ----------------------------------------------------------------------------
// CStateModel::EnterState
// ----------------------------------------------------------------------------
//
void CStateModel::EnterState( MStateModel::TState aState, TInt aError )
	{ 
    __SIP_INT_LOG2( "CStateModel::EnterState(state,error)", aState, aError )	

	// Exit the old state
	State().Exit();

	// Select next state, notify context
	iStateValue = aState;
	iContext.StateModelChanged( aState );

	// Enter the next state
	State().Enter( aError );
	}

// ----------------------------------------------------------------------------
// CStateModel::NetworkStateChanged
// ----------------------------------------------------------------------------
//
void CStateModel::NetworkStateChanged( 
    MSIPNetworkObserver::TNetworkState aState,
    TInt aError )
	{
	__SIP_INT_LOG2( "CStateModel::NetworkStateChanged(state,error)", 
	                aState, aError )
	iError = aError;
	State().NetworkStateChanged( aState, aError );
	}

// ----------------------------------------------------------------------------
// CStateModel::State
// ----------------------------------------------------------------------------
//
TStateBase& CStateModel::State() const
	{
	return const_cast< TStateBase& >( iStates.At( iStateValue ) );
	}

// ----------------------------------------------------------------------------
// CStateModel::Error
// ----------------------------------------------------------------------------
//
TInt CStateModel::Error() const
	{
	return iError;
	}

// ----------------------------------------------------------------------------
// CStateModel::CStateModel
// ----------------------------------------------------------------------------
//
CStateModel::CStateModel( MSIPConnectionContext& aContext,
						  MTimerManager& aTimer )
	: iContext( aContext ), iTimer( aTimer ), iStates( EMaxStates ),
	  iStateValue( MStateModel::EUnavailable ),iError(KErrNone)
	{
	}

// ----------------------------------------------------------------------------
// CStateModel::ConstructL
// ----------------------------------------------------------------------------
//
void CStateModel::ConstructL()
	{
	iStates.AppendL( TStateInit( *this, iContext ),
					 sizeof( TStateInit ) );
	iStates.AppendL( TStateInactiveMonitoring( *this, iContext,iTimer ),
					 sizeof( TStateInactiveMonitoring ) );
	iStates.AppendL( TStateInactiveConnecting( *this, iContext, iTimer ),
					 sizeof( TStateInactiveConnecting ) );
	iStates.AppendL( TStateActiveResolving( *this, iContext, iTimer ),
					 sizeof( TStateActiveResolving ) );
	iStates.AppendL( TStateActiveResolved( *this, iContext ),
					 sizeof( TStateActiveResolved ) );
	iStates.AppendL( TStateSuspended( *this, iContext ),
					 sizeof( TStateSuspended ) );
	iStates.AppendL( TStateUnavailable( *this, iContext ),
					 sizeof( TStateUnavailable ) );
	}
