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
//



#include <e32std.h>
#include <implementationproxy.h>
#include "SIPExResolverPlugin.h"

// Map the interface UID to implementation factory function
const TImplementationProxy Implementations[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KSIPExResolverPluginUID,
		                        CSIPExResolverPlugin::NewL )
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

// -----------------------------------------------------------------------------
// E32Dll (Removed in EKA2) 
// -----------------------------------------------------------------------------
//
#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
	{
	return(KErrNone);
	}
#endif

