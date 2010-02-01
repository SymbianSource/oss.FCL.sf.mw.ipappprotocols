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
// Name        : implementationproxy.cpp
// Part of     : sip ims agent
// implementation
// Version     : 1.0
//



#include "sipimsprofileagent.h"
#include <ecom/implementationproxy.h>

// Disabled PC-Lint warning for "suspicious typecast" caused by Symbian's
// ECom declarations
/*lint -e611 */

const TImplementationProxy ImplementationTable[] =
    {
	//Implementation UID and pointer to instantiation method
	IMPLEMENTATION_PROXY_ENTRY( 0x10203358, CSIPIMSProfileAgent::NewL )
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }
