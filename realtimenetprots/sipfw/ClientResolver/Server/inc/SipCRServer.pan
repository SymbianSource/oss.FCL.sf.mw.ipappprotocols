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
// Name          : SipCRServer.pan
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#ifndef __SIP_CR_SERVER_PAN__
#define __SIP_CR_SERVER_PAN__

#include <e32std.h>

// Panic Category
//
_LIT(KSipCRServerPanic, "SipCRServer");


// Sip CR Server panic codes
//
enum TSipCRServerPanic
    {
    EBadRequest,
    EBadDescriptor,
    ESrvCreateServer,
    ECreateTrapCleanup,
    EBadSipSessionHandle,
    EBadSipSubSessionHandle,
    EDatFileCorrupted
    };

#endif // __SIP_CR_SERVER_PAN__
