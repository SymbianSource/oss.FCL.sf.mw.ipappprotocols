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
// Name        : TStateActiveResolved.cpp
// Part of     : ConnectionMgr
// Implementation
// Version     : SIP/4.0
//



#include "TStateActiveResolved.h"
#include "msipconnectioncontext.h"
#include "SipLogs.h"
#include <featmgr.h>


// ----------------------------------------------------------------------------
// TStateActiveResolved::SendL
// ----------------------------------------------------------------------------
//
void TStateActiveResolved::SendL( const TSIPTransportParams& aParams,
                                  CSIPRequest& aRequest,
                                  TBool aForceUDP,
                                  const CUri8* aOutboundProxy,
								  const TInetAddr& aRemoteAddr,
								  TRequestStatus& aStatus )
	{
	iContext.SendToTransportL( 
	    aParams, aRequest, aForceUDP, aOutboundProxy, aRemoteAddr, aStatus );
	}

// ----------------------------------------------------------------------------
// TStateActiveResolved::SendL
// ----------------------------------------------------------------------------
//
void TStateActiveResolved::SendL( const TSIPTransportParams& aParams,
                                  CSIPResponse& aResponse,
								  TTransactionId aId,
								  TRequestStatus& aStatus )
	{
	iContext.SendToTransportL( aParams, aResponse, aId, aStatus );
	}

// ----------------------------------------------------------------------------
// TStateActiveResolved::NetworkStateChanged
// ----------------------------------------------------------------------------
//
void TStateActiveResolved::NetworkStateChanged(
    MSIPNetworkObserver::TNetworkState aState,
    TInt aError )
	{
	 __SIP_INT_LOG2( "TStateActiveResolved::NetworkStateChanged", aState, aError )
	if( aError)
		{
	if(FeatureManager::FeatureSupported( KFeatureIdFfSipCustomPdpHandling ))
		{
			if(iContext.ConnectionType() != MSIPConnectionContext::ELanBearer)
				{
				if( aState == MSIPNetworkObserver::EConnectionLost )
					{
					if(iContext.ContinueMonitoring( aError ))
						EnterState( MStateModel::EInactiveMonitoring, aError );
					}
				}
		}
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
