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
// Name        : TStateInit.cpp
// Part of     : ConnectionMgr
// Implementation
// Version     : SIP/4.0
//



#include "TStateInit.h"
#include "msipconnectioncontext.h"

#include <etelpckt.h>
#include "SipLogs.h"

// ----------------------------------------------------------------------------
// TStateInit::Enter
// ----------------------------------------------------------------------------
//
void TStateInit::Enter( TInt /*aError*/ )
	{
	__SIP_LOG( "TStateInit::Enter" )
	TInt err = iContext.OpenConnection();
	if( err )
		{
		EnterState( MStateModel::EUnavailable, err );
		}
	}

// ----------------------------------------------------------------------------
// TStateInit::NetworkStateChanged
// ----------------------------------------------------------------------------
//
void TStateInit::NetworkStateChanged( 
    MSIPNetworkObserver::TNetworkState aState,
    TInt aError )
	{	
	  __SIP_INT_LOG2( "TStateInit::NetworkStateChanged", aState, aError )
	if( aState == MSIPNetworkObserver::ENetworkConnected )
		{
		if( aError == KErrNone )
			{
			EnterState( MStateModel::EActiveResolving );
			}
		else if( ( aError == KErrNotFound ) ||
				 ( aError == KErrBadName ) ||
				 ( aError == KErrNotSupported ) ||
				 ( aError == KErrGprsMissingorUnknownAPN ) ||
				 ( aError == KErrGprsInsufficientResources ) )
			{
			EnterState( MStateModel::EUnavailable, aError );
			}
		else
			{
			EnterState( MStateModel::EInactiveMonitoring, aError );
			}
		}
	}
