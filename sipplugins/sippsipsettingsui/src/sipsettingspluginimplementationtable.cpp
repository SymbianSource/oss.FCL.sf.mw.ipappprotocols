/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ECOM proxy table for this plugin
*
*/


// INCLUDES
#include <e32std.h>
#include <ecom/implementationproxy.h>
#include "sipsettingsplugin.h"
#include "gssippluginlogger.h"


// CONSTANTS

// Disable PC-lint warning #611, as the cause is in Symbian code and can't be fixed
/*lint -e611 */
const TImplementationProxy KSIPSettingsPluginImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(0x101FD6D0,	CSIPSettingsPlugin::NewL)
	};
/*lint +e611 */	

// ---------------------------------------------------------------------------
// ImplementationGroupProxy
//
// Gate/factory function
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
															TInt& aTableCount)
	{
    __GSLOGSTRING( "[GSSIPSettingsPlugin] ImplementationGroupProxy()" )
	aTableCount = sizeof( KSIPSettingsPluginImplementationTable ) 
        / sizeof( TImplementationProxy );
	return KSIPSettingsPluginImplementationTable;
	}


// ---------------------------------------------------------------------------
// E32Dll
// EKA1 entry point
//
// ---------------------------------------------------------------------------
//
#ifndef EKA2
GLDEF_C TInt E32Dll( TDllReason /*aReason*/ )
	{
	return( KErrNone );
	}
#endif

// End of File
