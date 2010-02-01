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
// Name        : TTlsTransTcpConnecting.cpp
// Part of     : ConnectionMgr
// implementation
// Version     : SIP/5.0
//



#include "TTlsTransTcpConnecting.h"
#include "MTlsTransStateOwner.h"

// -----------------------------------------------------------------------------
// TTlsTransTcpConnecting::EnterL
// -----------------------------------------------------------------------------
//
void TTlsTransTcpConnecting::EnterL()
    {
    iOwner.ConnectTcpL();
    }
    
// -----------------------------------------------------------------------------
// TTlsTransTcpConnecting::ConnectionOpenL
// -----------------------------------------------------------------------------
//
TBool TTlsTransTcpConnecting::ConnectionOpenL()
    {
    iOwner.ChangeStateL( TTlsTransStateBase::ETlsConnecting );
    
    // Deny receiving until tls handshake has been completed.
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TTlsTransTcpConnecting::TTlsTransTcpConnecting
// -----------------------------------------------------------------------------
//
TTlsTransTcpConnecting::TTlsTransTcpConnecting( 
    MTlsTransStateOwner& aOwner ) : TTlsTransStateBase( aOwner )
    {
    }

// End of File

