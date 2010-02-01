// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : profilecsserver.pan
// Part of     : SIP Profile Server
// Interface   : private
// Server panic codes
// Version     : 1.0
//



#ifndef PROFILECSSERVER_PAN
#define PROFILECSSERVER_PAN


// Panic Category
//
_LIT(KSipProfileCSServerPanic, "SIPProfileServer");


// Sip Profile CS Server panic codes
//

enum TSIPProfileCSPanic
    {
    EBadRequest,
    EBadDescriptor,
    ESrvCreateServer,
    ECreateTrapCleanup,
    EBadSipSessionHandle,
    ERestoreFailure
    };


#endif
