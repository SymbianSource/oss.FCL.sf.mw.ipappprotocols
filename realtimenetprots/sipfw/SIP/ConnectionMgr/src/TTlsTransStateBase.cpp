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
// Name        : TTlsTransStateBase.cpp
// Part of     : ConnectionMgr
// implementation
// Version     : SIP/5.0
//



#include "TTlsTransStateBase.h"
#include "MTlsTransStateOwner.h"
#include "CSender.h"


// -----------------------------------------------------------------------------
// TTlsTransStateBase::EnterL
// -----------------------------------------------------------------------------
//
void TTlsTransStateBase::EnterL()
    {
    }

// -----------------------------------------------------------------------------
// TTlsTransStateBase::ConnectionOpenL
// -----------------------------------------------------------------------------
//
TBool TTlsTransStateBase::ConnectionOpenL()
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TTlsTransStateBase::DisconnectedL
// -----------------------------------------------------------------------------
//  	
TBool TTlsTransStateBase::DisconnectedL()
    {
    iOwner.Destroy();
    return ETrue;
    }
    
// -----------------------------------------------------------------------------
// TTlsTransStateBase::SendToNetL
// -----------------------------------------------------------------------------
//
void TTlsTransStateBase::SendToNetL(
    const TSIPTransportParams& aParams,
    const TInetAddr& aAddress, 
    CSIPMessage& aMessage, 
    TUint aOrigTransport,
    TRequestStatus& aStatus )
    {
    // Send operations are queued until connection is established.
    iOwner.GetSender().SendL( aParams,
	                          aMessage,
							  aAddress,
			                  aOrigTransport,
			                  aStatus,
			                  ETrue,
			                  EFalse );
    }

// -----------------------------------------------------------------------------
// TTlsTransStateBase::TTlsTransStateBase
// -----------------------------------------------------------------------------
//
TTlsTransStateBase::TTlsTransStateBase( 
    MTlsTransStateOwner& aOwner ) : iOwner( aOwner )
    {
    }

// End of File

