/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : sipcrclientserver.h
* Part of       : SIP Client Discovery API
* Version       : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef SIPCRCLIENTSERVER_H
#define SIPCRCLIENTSERVER_H

#include <e32std.h>

_LIT(KSipClientResolverServerName,"siprsvsrv");
_LIT(KSipClientResolverServerFilename, "siprsvsrv");
_LIT(KSipClientResolverServerSemaphoreName, "siprsvsrvSemaphore");

const TUid KServerUid3 = { 0x1020334f };

#if (defined (__WINS__) || defined(__WINSCW__))
static const TUint KServerMinHeapSize =  0x1000;  //   4K
static const TUint KServerMaxHeapSize = 0x80000;  // 512K (Should be enough)
#endif

// The server version.
// A version must be specifyed when creating a session with the server
const TUint KSipCRServerMajorVersionNumber=0;
const TUint KSipCRServerMinorVersionNumber=1;
const TUint KSipCRServerBuildVersionNumber=1;


// A container for incoming buffer sizes
class TSIPCRMessageBufSizes
    {
public:

    TSIPCRMessageBufSizes()
        : iHeaderBufSize(0),
          iContentBufSize(0) {}

    TSIPCRMessageBufSizes(TInt aHeaderBufSize, TInt aContentBufSize)
        : iHeaderBufSize(aHeaderBufSize),
          iContentBufSize(aContentBufSize) {}

	TSIPCRMessageBufSizes(const TSIPCRMessageBufSizes& aSizes)
        : iHeaderBufSize(aSizes.iHeaderBufSize),
          iContentBufSize(aSizes.iContentBufSize) {}

    TInt iHeaderBufSize;
    TInt iContentBufSize;
    };


// Oper. codes used in passing IPC messages between C/S-client and C/S-server
enum TSIPCRIpcFunctions
    {
    // No capabilities required 0-3
    ESIPCRIpcClientReadyToReceive, // Asynchronous receive
    ESIPCRIpcClientCancelReceive,
    ESIPCRIpcCancelRequest,
    ESIPCRIpcCancelAllRequests,
    // WriteDeviceData capabilities required 4-6
    ESIPCRIpcSetClientUid, // Set the client's UID3
    ESIPCRIpcRegister,
    ESIPCRIpcDeregister,
    // ReadDeviceData capabilities required 7-9
    ESIPCRIpcClientReceiveSipResponse, // Synchronous receive
    ESIPCRIpcChannel,
    ESIPCRIpcChannelWithResolver
    };


// Indexes used in IPC parameter array passed between C/S-client and C/S-server
//
// All the parameters except ESIPCRIpcArgMessage are packed into package buffers
// to enable IPC parameter in-out behaviour.
//
// NOTE: Remember to update the types in comments if the actual types change.
//
enum TSIPCRIpcArguments
    {
    ESIPCRIpcArgReqId = 0,             // Type: TUint32
    ESIPCRIpcArgClientUid = 0,         // Type: TUid
    ESIPCRIpcArgClientResolved = 1,    // Type: TInt
    ESIPCRIpcArgSipResponse = 1,       // Type: TDes8/TDesC8
    ESIPCRIpcArgSipRequest = 1,        // Type: TDesC8
    ESIPCRIpcArgBufSizes = 2,          // Type: TSIPCRMessageBufSizes
    ESIPCRIpcArgSipMessageContent = 2, // Type: TDes8/TDesC8
    ESIPCRIpcArgResolverUid = 3,       // Type: TUid
    };
    
enum TSIPCRIpcResponses
    {
    ESIPCRChannelComplete = 1,
    ESIPCRClientNotFound
    };

#endif // SIPCRCLIENTSERVER_H

// End of File
