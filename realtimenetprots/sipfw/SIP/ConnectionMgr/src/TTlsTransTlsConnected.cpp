// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : TTlsTransTlsConnected.cpp
// Part of     : ConnectionMgr
// implementation
// Version     : SIP/5.0
//



#include "TTlsTransTlsConnected.h"
#include "MTlsTransStateOwner.h"
#include "CSender.h"

// -----------------------------------------------------------------------------
// TTlsTransTlsConnected::EnterL
// -----------------------------------------------------------------------------
//
void TTlsTransTlsConnected::EnterL()
    {
    // Stop TLS connecting guard timer.
    iOwner.StopTimer();
    
    // Kick-start sending of possibly existing queued messages
    iOwner.GetSender().SendNextL();
    
    iOwner.StartTimerL( iIdleTimeout );
    }
    
// -----------------------------------------------------------------------------
// TTlsTransTlsConnected::DisconnectedL
// -----------------------------------------------------------------------------
//  	
TBool TTlsTransTlsConnected::DisconnectedL()
    {
    if ( iOwner.Retry() )
        {
        iOwner.ResetL();
        iOwner.ChangeStateL( TTlsTransStateBase::ETcpConnecting );
        }
    else
        {
        iOwner.Destroy();
        }
    return ETrue;
    }
    
// -----------------------------------------------------------------------------
// TTlsTransTlsConnected::SendToNetL
// -----------------------------------------------------------------------------
//
void TTlsTransTlsConnected::SendToNetL(
    const TSIPTransportParams& aParams,
    const TInetAddr& aAddress, 
    CSIPMessage& aMessage, 
    TUint aOrigTransport,
    TRequestStatus& aStatus )
    {
    // Connection is established, send is immediately possible
    iOwner.GetSender().SendL( aParams,
	                          aMessage,
							  aAddress,
			                  aOrigTransport,
			                  aStatus,
			                  EFalse,
			                  EFalse );
    
    iOwner.StartTimerL( iIdleTimeout );
    }


// -----------------------------------------------------------------------------
// TTlsTransTlsConnected::TTlsTransTlsConnected
// -----------------------------------------------------------------------------
//
TTlsTransTlsConnected::TTlsTransTlsConnected( 
    MTlsTransStateOwner& aOwner, TInt aIdleTimeout ) : 
    TTlsTransStateBase( aOwner ), 
    iIdleTimeout( aIdleTimeout )
    {
    }

// End of File

