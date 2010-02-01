/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  SIP adapter ImplementationGroupProxy
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <ecom/implementationproxy.h>
#include <sipprofileregistryobserver.h>
#include "nsmldmsipadapter.h"

const TUint KNSmlDMSIPAdapterImplUid = 0x10205959;

// ---------------------------------------------------------------------------
// TImplementationProxy ImplementationTable[]
// Needed because of ECOM architecture
// ---------------------------------------------------------------------------
//
#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY( aUid, aFuncPtr ) {{ aUid }, ( aFuncPtr )}
#endif

const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( KNSmlDMSIPAdapterImplUid, 
        CNSmlDmSIPAdapter::NewL )
    };

// ---------------------------------------------------------------------------
// ImplementationGroupProxy()
// Needed because of ECOM architecture
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
    TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( 
        TImplementationProxy );
    return ImplementationTable;
    }


  
