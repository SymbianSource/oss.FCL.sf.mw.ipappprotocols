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
// Name        : TStateBase.cpp
// Part of     : ConnectionMgr
// Implementation
// Version     : SIP/4.0
//



#include "TStateBase.h"
#include "siperr.h"

// ----------------------------------------------------------------------------
// TStateBase::TStateBase
// ----------------------------------------------------------------------------
//
TStateBase::TStateBase( MStateModel& aOwner, MSIPConnectionContext& aContext )
	: iOwner( aOwner ), iContext( aContext )
	{
	}

// ----------------------------------------------------------------------------
// TStateBase::SendL
// ----------------------------------------------------------------------------
//
void TStateBase::SendL( const TSIPTransportParams& /*aParams*/,
                        CSIPRequest& /*aRequest*/,
                        TBool /*aForceUDP*/,
                        const CUri8* /*aOutboundProxy*/,
						const TInetAddr& /*aRemoteAddr*/,
						TRequestStatus& aStatus )
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete( status, KErrSIPTransportFailure );
	}

// ----------------------------------------------------------------------------
// TStateBase::SendL
// ----------------------------------------------------------------------------
//
void TStateBase::SendL( const TSIPTransportParams& /*aParams*/,
                        CSIPResponse& /*aResponse*/,
						TTransactionId /*aId*/,
						TRequestStatus& aStatus )
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete( status, KErrSIPTransportFailure );
	}

// ----------------------------------------------------------------------------
// TStateBase::Enter
// ----------------------------------------------------------------------------
//
void TStateBase::Enter( TInt /*aError*/ ) 
	{
	// Do nothing
	}

// ----------------------------------------------------------------------------
// TStateBase::Exit
// ----------------------------------------------------------------------------
//
void TStateBase::Exit()
	{
	// Do nothing
	}

// ----------------------------------------------------------------------------
// TStateBase::NetworkStateChanged
// ----------------------------------------------------------------------------
//
void TStateBase::NetworkStateChanged( 
    MSIPNetworkObserver::TNetworkState /*aState*/,
    TInt /*aError*/ )
	{
	// Do nothing
	}

// ----------------------------------------------------------------------------
// TStateBase::EnterState
// ----------------------------------------------------------------------------
//
void TStateBase::EnterState( MStateModel::TState aState, TInt aError )
	{
	iOwner.EnterState( aState, aError );
	}
