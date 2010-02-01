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
// Name        : TStateSuspended.cpp
// Part of     : ConnectionMgr
// Implementation
// Version     : SIP/4.0
//



#include "TStateSuspended.h"

// ----------------------------------------------------------------------------
// TStateSuspended::NetworkStateChanged
// ----------------------------------------------------------------------------
//
void TStateSuspended::NetworkStateChanged(
    MSIPNetworkObserver::TNetworkState aState,
    TInt aError )
	{
	if( aError || aState == MSIPNetworkObserver::ENetworkInactive )
		{
		EnterState( MStateModel::EInactiveMonitoring );
		}
	else if( aState == MSIPNetworkObserver::ENetworkActive )
		{
		EnterState( MStateModel::EActiveResolving );
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
