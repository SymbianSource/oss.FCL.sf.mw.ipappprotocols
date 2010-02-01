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
// Name          : RSIP.cpp
// Part of       : SIPClient
// Version       : SIP/3.0 
//




#include "sipclient.h"
#include "RSIP.h"
#include "sipclientserver.h"
#include "SIPServerStarter.h"
#include "sipclient.pan"
#include "apaid.h"
#include "apgcli.h"

#define RETURN_IF_ERROR(err) {TInt _err=err; if(_err!=KErrNone) {return _err;}}

// -----------------------------------------------------------------------------
// RSIP::RSIP
// -----------------------------------------------------------------------------
//
RSIP::RSIP() 
 : RSessionBase (),
   iConnected (EFalse)
	{
	}

// -----------------------------------------------------------------------------
// RSIP::Version
// -----------------------------------------------------------------------------
//
TVersion RSIP::Version(void) const
	{
    TVersion version (KSipServerMajorVersionNumber,
                      KSipServerMinorVersionNumber,
                      KSipServerBuildVersionNumber);
    return version;
	}

// -----------------------------------------------------------------------------
// RSIP::Connect
// -----------------------------------------------------------------------------
//
TInt RSIP::Connect (const TUid& aUid)
	{
    RETURN_IF_ERROR (SIPServerStarter::Start ())

    RETURN_IF_ERROR (CreateSession (KSipServerName,Version()))

	RETURN_IF_ERROR (SendClientUid(aUid))

    iConnected = ETrue;
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// RSIP::Connect
// -----------------------------------------------------------------------------
//
TInt RSIP::Connect ()
	{
    RETURN_IF_ERROR (SIPServerStarter::Start ())

    RETURN_IF_ERROR (CreateSession (KSipServerName,Version()))

    iConnected = ETrue;
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// RSIP::Close
// -----------------------------------------------------------------------------
//
void RSIP::Close()
	{
    iConnected = EFalse;
    RHandleBase::Close();
	}

// -----------------------------------------------------------------------------
// RSIP::Send
// From MSIPITC
// -----------------------------------------------------------------------------
//
TInt RSIP::Send (TSipItcFunctions aFunction, TIpcArgs& aArgs) const
	{
	__ASSERT_ALWAYS (iConnected, Panic(ESipClientNotConnected));

	return SendReceive(aFunction, aArgs);
	}

// -----------------------------------------------------------------------------
// RSIP::Receive
// From MSIPITC
// -----------------------------------------------------------------------------
//
void RSIP::Receive (TIpcArgs& aArgs, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS (iConnected, Panic(ESipClientNotConnected));

	SendReceive(ESipItcClientReadyToReceive,aArgs,aStatus);
	}

// -----------------------------------------------------------------------------
// RSIP::Receive
// From MSIPITC
// -----------------------------------------------------------------------------
//
TInt RSIP::Receive (TIpcArgs& aArgs)
	{
	__ASSERT_ALWAYS (iConnected, Panic(ESipClientNotConnected));

	return SendReceive(ESipItcClientReceiveSipMessage,aArgs);
	}

// -----------------------------------------------------------------------------
// RSIP::CancelReceive
// From MSIPITC
// -----------------------------------------------------------------------------
//
void RSIP::CancelReceive ()
	{
    __ASSERT_ALWAYS (iConnected, Panic(ESipClientNotConnected));

    SendReceive(ESipItcClientCancelReceive, TIpcArgs());
	}

// -----------------------------------------------------------------------------
// RSIP::SendAppUid
// -----------------------------------------------------------------------------
//
TInt RSIP::SendClientUid (const TUid& aUid) const
	{
    TPckgBuf<TInt> clientUidPckg(aUid.iUid);
    TIpcArgs itcMsgArgs;
    itcMsgArgs.Set (ESipItcArgAppUid, &clientUidPckg);

	return SendReceive(ESipItcSetAppUid, itcMsgArgs);
	}

// -----------------------------------------------------------------------------
// RSIP::Panic
// -----------------------------------------------------------------------------
//
void RSIP::Panic (TInt aReason) const
	{
    User::Panic(KSipClient, aReason);
	}
