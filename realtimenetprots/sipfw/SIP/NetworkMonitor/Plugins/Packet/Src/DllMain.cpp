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
// Name        : DllMain.cpp
// Part of     : NetworkMonitor
// Implementation
// Version     : SIP/4.0
//



#include <e32std.h>
#include "EComImplementationProxy.h"
#include "CPacketBearerMonitor.h"

// Map the interface UIDs to implementation factory functions
const TImplementationProxy Implementations[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( 0x102010D9, CPacketBearerMonitor::NewL )
    };

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
//
// Exported proxy for instantiation method resolution
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aCount )
    {
    aCount = sizeof( Implementations ) / sizeof( TImplementationProxy );
    return Implementations;
    }
