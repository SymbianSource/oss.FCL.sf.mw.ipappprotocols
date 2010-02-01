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
* Name          : sipclientserver.h
* Part of       : SIPCommon
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/


#ifndef SIPCLIENTSERVER_H
#define SIPCLIENTSERVER_H

#include <e32std.h>

_LIT(KSipServerName,"SipServer");
_LIT(KSipServerFilename, "SipServer");
_LIT(KSipServerSemaphoreName, "SipServerSemaphore");

const TUid KServerUid3 = {0x101f5d36};

#if (defined (__WINS__) || defined(__WINSCW__))
static const TUint KServerMinHeapSize =  0x1000;  //   4K
static const TUint KServerMaxHeapSize = 0x80000;  // 512K !?!?
#endif

// The server version.
// A version must be specifyed when creating a session with the server
const TUint KSipServerMajorVersionNumber=0;
const TUint KSipServerMinorVersionNumber=1;
const TUint KSipServerBuildVersionNumber=1;


// A container for SIP IDs
class TSIPIds
	{
public:

	TSIPIds()
		: iIapId(0),
          iRequestId(0),
		  iRegistrationId(0),
		  iDialogId(0),
		  iRefreshId(0),
		  iRefresh(EFalse),
		  iTargetRefresh(EFalse),
		  iSendWithExpires(ETrue),
		  iCacheOutboundProxyIP(EFalse),
		  iConnectionError(KErrNone){}

	TSIPIds(const TSIPIds& aIds)
		: iIapId(aIds.iIapId),
          iRequestId(aIds.iRequestId),
		  iRegistrationId(aIds.iRegistrationId),
		  iDialogId(aIds.iDialogId),
		  iRefreshId(aIds.iRefreshId),
		  iRefresh(aIds.iRefresh),
		  iTargetRefresh(aIds.iTargetRefresh),
		  iSendWithExpires(aIds.iSendWithExpires),
		  iCacheOutboundProxyIP(aIds.iCacheOutboundProxyIP),
		  iConnectionError(aIds.iConnectionError){}

    TUint32 iIapId;
	TUint32 iRequestId;
	TUint32 iRegistrationId;
	TUint32 iDialogId;
	TUint32 iRefreshId;
	TBool iRefresh;
	TBool iTargetRefresh;
	TBool iSendWithExpires;
	TBool iCacheOutboundProxyIP;
	TInt iConnectionError;
	};


// A container for incoming buffer sizes
class TSIPMessageBufSizes
    {
public:

    TSIPMessageBufSizes()
        : iHeaderBufSize(0),
          iContentBufSize(0) {}

    TSIPMessageBufSizes(TInt aHeaderBufSize, TInt aContentBufSize)
        : iHeaderBufSize(aHeaderBufSize),
          iContentBufSize(aContentBufSize) {}

	TSIPMessageBufSizes(const TSIPMessageBufSizes& aSizes)
        : iHeaderBufSize(aSizes.iHeaderBufSize),
          iContentBufSize(aSizes.iContentBufSize) {}

    TInt iHeaderBufSize;
    TInt iContentBufSize;
    };
    
    
// A container for SIP socket options
class TSIPSockOpt
    {
public:

    TSIPSockOpt()
        : iOptionName(0),
          iOptionLevel(0),
          iOption(0) {}

    TSIPSockOpt(TUint aOptionName, TUint aOptionLevel, TInt aOption=0)
        : iOptionName(aOptionName),
          iOptionLevel(aOptionLevel),
          iOption(aOption) {}

    TUint iOptionName;
    TUint iOptionLevel;
    TInt iOption;
    };    


// HTTP Digest observer type
enum TSipHttpDigestObserverType
	{
    ESipNoHttpDigestObserver = 0,
    ESipRealmHttpDigestObserver = 1,
    ESipResponseHttpDigestObserver = 2
	};
	
	
// Oper. codes used in passing IPC messages between C/S-client and C/S-server
enum TSipItcFunctions
    {
	// 0-23: No capabilities required
    ESipItcTerminateDialog,
    ESipItcTerminateRefresh,
    ESipItcTerminateDialogRefresh,
    ESipItcTerminateRegistration,
    ESipItcSupportedSecurityMechanisms,
    ESipItcNegotiatedSecurityMechanism,
    ESipItcIsSigCompSupported,
    ESipItcGetTransactionState,
    ESipItcGetDialogState,
    ESipItcGetCompartmentCount,
    ESipItcGetFromHeader,
    ESipItcGetRefreshInterval,
    ESipItcGetCallIDHeader,
    ESipItcGetLocalTag,
    ESipItcOutboundProxy,
    ESipItcGetRegisteredContact,
    ESipItcGetLocalAddr,
    ESipItcSetOutboundProxy,
    ESipItcRemoveOutboundProxy,
    ESipItcSetRefreshInterval,
    ESipItcSetHttpDigestObserver,
    ESipItcSetCredentials,
    ESipItcRemoveCredentials,
    ESipItcIgnoreChallenge,
    ESipItcSetSecurityHandling,
    ESipItcResetDialogState,
    // 26: WriteDeviceData capability required
    ESipItcSetAppUid,
    // 27-28: // Custom check: same capabilities as SIP server required
    ESipItcSetSIPSockOpt,
    ESipItcSetSIPSockOptWithDes,
    // 29-51: NetworkServices capability required
    ESipItcOpenSubSession,
    ESipItcCloseSubSession,    
    ESipItcRegister,
    ESipItcRegisterAndSetOutboundProxy,
    ESipItcUpdateRegistration,
    ESipItcUnRegister,
    ESipItcFetchRegistrations,
    ESipItcSendRequest,
    ESipItcSendRequestAndUpdateRefresh,
    ESipItcSendRequestAndTerminateRefresh,
    ESipItcSendResponse,
    ESipItcSendCancel,
    ESipItcSendResponseAndCreateDialog,
    ESipItcSendResponseInDialog,
    ESipItcSendRequestAndCreateDialog,
    ESipItcSendRequestInDialog,
    ESipItcSendRequestInDialogAndUpdateRefresh,
    ESipItcClientReadyToReceive, // Asynchronous receive
    ESipItcClientReceiveSipMessage, // Synchronous receive
    ESipItcClientCancelReceive,      
    ESipItcConnectionReadyToReceive, // Asynchronous receive
    ESipItcConnectionReceiveSipMessage, // Synchronous receive
    ESipItcConnectionCancelReceive,
    ESipItcRefreshConnection,
    ESipItcGetConnectionError
    };	


// Indexes used in IPC parameter array passed between C/S-client and C/S-server
//
// NOTE: Remember to update the types in comments if the actual types change.
//
enum TSipItcArguments
	{
    ESipItcArgConnectionState = 0,         // Type: TInt
    ESipItcArgInternalState = 0,           // Type: TInt
    ESipItcArgRefreshInterval = 0,         // Type: TInt
    ESipItcArgMessageHeaders = 0,          // Type: TDes8/TDesC8
    ESipItcArgRealm = 0,                   // Type: TDes8/TDesC8
    ESipItcArgAuthenticationMechanism = 0, // Type: TDesC8
    ESipItcArgSockOpt = 0,                 // Type: TSIPSockOpt
    ESipItcArgLocalAddr = 0,               // Type: TInetAddr
    ESipItcArgLocalTag = 0,                // Type: TDesC8
    ESipItcArgHttpDigestObserverType = 0,  // Type: TSipHttpDigestObserverType
    ESipItcArgSecurityHandling = 0,        // Type: TBool
    ESipItcArgAppUid = 1,                  // Type: TInt
    ESipItcArgIds = 1,                     // Type: TSIPIds
    ESipItcArgNextHop = 1,                 // Type: TDesC8    
    ESipItcArgBufSizes = 2,                // Type: TSIPMessageBufSizes
    ESipItcArgMessageContent = 2,          // Type: TDes8/TDesC8
    ESipItcArgSockOptDes = 2,              // Type: TDesC8
    ESipItcArgCredentials = 2,             // Type: TDesC8
    ESipItcArgSubSessionHandle = 3         // Type: TInt
	};

#endif // SIPCLIENTSERVER_H

// End of File
