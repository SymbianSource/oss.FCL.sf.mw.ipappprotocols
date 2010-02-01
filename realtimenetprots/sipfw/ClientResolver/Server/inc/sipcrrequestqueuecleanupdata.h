/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Name          : sipcrrequestqueuecleanupdata.h
* Part of       : SIP / SIP Client Resolver
* Version       : %version: 2.1.1 %
*
*/




/**
 @internalComponent
*/

#ifndef TSIPCRREQUESTQUEUECLEANUPDATA_H
#define TSIPCRREQUESTQUEUECLEANUPDATA_H

// INCLUDES
#include <e32std.h>


class TSipCrRequestQueueCleanupData
    {
    public: // Data

        CSIPCRServerSession* iSession; // Not owned
        CSIPCRRequestItem* iRequestItem; // Not owned
    };

#endif // TSIPCRREQUESTQUEUECLEANUPDATA_H

// End of File
