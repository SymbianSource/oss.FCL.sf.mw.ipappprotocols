// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : dll.cpp
// Part of     : deflatecomp
// main dll entry, and ecom proxy
// Version     : 1.0
//



#include <e32base.h>
#include <e32std.h>
#include <ecom/implementationproxy.h>

#include "DeflateComp.h"

// Disabled PC-Lint warning for "suspicious typecast" caused by Symbian's
// ECom declarations
/*lint -e611 */

// 8.x ECOM compatibility - EABI requires the use of IMPLEMENTATION_PROXY_ENTRY
// (i.e. define IMPLEMENTATION_PROXY_ENTRY locally on pre 8.x systems)
#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY(aUid, aFuncPtr)	{{aUid},aFuncPtr}
#endif

const TImplementationProxy KImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( 0x101F9583, CDeflateComp::NewL )
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
                            TInt& aTableCount)
    {
    aTableCount = sizeof(KImplementationTable) / sizeof(TImplementationProxy);
    return KImplementationTable;
    }
