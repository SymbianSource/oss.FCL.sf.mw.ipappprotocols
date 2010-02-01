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
// Name          : StaticDllEntry.cpp
// Part of       : SIPDigestPlugin
// Version       : SIP/4.0 
//



#include <e32std.h>
#include "CSIPSecDigestPlugin.h"
#include "EComImplementationProxy.h"

const TImplementationProxy ImplementationTable [] =
    {
	//Implementation UID and pointer to instantiation method
    IMPLEMENTATION_PROXY_ENTRY( 0x1020334D, CSIPSecDigestPlugin::NewL )
    };

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy*
ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
