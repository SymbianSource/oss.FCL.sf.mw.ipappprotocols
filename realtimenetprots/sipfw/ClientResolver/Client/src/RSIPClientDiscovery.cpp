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
// Name          : RSIPClientDiscovery.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



// INCLUDES
#include "RSIPClientDiscovery.h"
#include "SIPClientResolverServerStarter.h"
#include "sipclientdiscovery.pan"

// MACROS
#define RETURN_IF_ERROR(err) {TInt _err=err; if(_err!=KErrNone) {return _err;}}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RSIPClientDiscoveryClientDiscovery::RSIPClientDiscovery
// -----------------------------------------------------------------------------
//
RSIPClientDiscovery::RSIPClientDiscovery() 
 : RSessionBase (),
   iConnected (EFalse)
	{
	}

// -----------------------------------------------------------------------------
// RSIPClientDiscovery::Connect
// -----------------------------------------------------------------------------
//
TInt RSIPClientDiscovery::Connect (TUid aSelf)
    {
    RETURN_IF_ERROR(SIPClientResolverServerStarter::Start())
    
    RETURN_IF_ERROR(CreateSession(KSipClientResolverServerName, Version()))
    
    RETURN_IF_ERROR(SendClientUid(aSelf))
    
    iConnected = ETrue;
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// RSIPClientDiscovery::Close
// -----------------------------------------------------------------------------
//
void RSIPClientDiscovery::Close()
    {
    RHandleBase::Close();
    }
   
// -----------------------------------------------------------------------------
// RSIPClientDiscovery::SendReceive
// -----------------------------------------------------------------------------
//    
TInt RSIPClientDiscovery::SendReceive(TSIPCRIpcFunctions aFunction,
                                      const TIpcArgs& aArgs) const
    {   
    return RSessionBase::SendReceive(aFunction,aArgs);
    }   

// -----------------------------------------------------------------------------
// RSIPClientDiscovery::Send
// -----------------------------------------------------------------------------
//
TInt RSIPClientDiscovery::Send (TSIPCRIpcFunctions aFunction, TIpcArgs& aArgs)
	{
	__ASSERT_ALWAYS (iConnected, Panic(ESipClientNotConnected));

	return SendReceive (aFunction, aArgs);
	}
    
// -----------------------------------------------------------------------------
// RSIPClientDiscovery::Receive
// -----------------------------------------------------------------------------
//
void RSIPClientDiscovery::Receive (TIpcArgs& aArgs, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS (iConnected, Panic(ESipClientNotConnected));

	RSessionBase::SendReceive(ESIPCRIpcClientReadyToReceive,aArgs,aStatus);
	}

// -----------------------------------------------------------------------------
// RSIPClientDiscovery::Receive
// -----------------------------------------------------------------------------
//
TInt RSIPClientDiscovery::Receive (TIpcArgs& aArgs)
	{
	__ASSERT_ALWAYS (iConnected, Panic(ESipClientNotConnected));

	return SendReceive(ESIPCRIpcClientReceiveSipResponse, aArgs);
	}

// -----------------------------------------------------------------------------
// RSIPClientDiscovery::CancelReceive
// -----------------------------------------------------------------------------
//
void RSIPClientDiscovery::CancelReceive ()
	{
    __ASSERT_ALWAYS (iConnected, Panic(ESipClientNotConnected));

    SendReceive (ESIPCRIpcClientCancelReceive, TIpcArgs());
	}

// -----------------------------------------------------------------------------
// RSIPClientDiscovery::SendClientUid
// -----------------------------------------------------------------------------
//
TInt RSIPClientDiscovery::SendClientUid (const TUid& aUid) const
	{
    TPckgBuf<TUid> clientUidPckg(aUid);
    TIpcArgs ipcArgs;
    ipcArgs.Set (ESIPCRIpcArgClientUid, &clientUidPckg);
	return SendReceive (ESIPCRIpcSetClientUid, ipcArgs);
	}

// -----------------------------------------------------------------------------
// RSIPClientDiscovery::Version
// -----------------------------------------------------------------------------
//
TVersion RSIPClientDiscovery::Version(void) const
	{
    TVersion version (KSipCRServerMajorVersionNumber,
                      KSipCRServerMinorVersionNumber,
                      KSipCRServerBuildVersionNumber);
    return version;
	}

// -----------------------------------------------------------------------------
// RSIPClientDiscovery::Panic
// -----------------------------------------------------------------------------
//
void RSIPClientDiscovery::Panic (TInt aReason) const
	{
    User::Panic(KSIPClientDiscovery, aReason);
	}

//  End of File  
