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
// Name          : RSIPConnection.cpp
// Part of       : SIPClient
// Version       : SIP/3.0 
//



#include "RSIPConnection.h"
#include "RSIP.h"

// -----------------------------------------------------------------------------
// RSIPConnection::RSIPConnection
// -----------------------------------------------------------------------------
//
RSIPConnection::RSIPConnection(TUint32 aIapId)
 : RSubSessionBase (),
   iOpen (EFalse),
   iIapId (aIapId),
   iState (CSIPConnection::EInit)
	{
	}

// -----------------------------------------------------------------------------
// RSIPConnection::Open
// -----------------------------------------------------------------------------
//
TInt RSIPConnection::Open (RSIP& aSip)
	{
    TPckgBuf<TSIPIds> sipIdsPckg;
    sipIdsPckg().iIapId = iIapId;
	
    TIpcArgs args;
    args.Set (ESipItcArgIds, &sipIdsPckg);

    TPckgBuf<CSIPConnection::TState> statePckg;
    args.Set (ESipItcArgConnectionState, &statePckg);

    TInt err = CreateSubSession (aSip,ESipItcOpenSubSession,args);
    if (err == KErrNone)
        {
        iOpen = ETrue;
        iState = statePckg();
        }
    return err;
	}

// -----------------------------------------------------------------------------
// RSIPConnection::Close
// -----------------------------------------------------------------------------
//
void RSIPConnection::Close()
	{
    if (iOpen)
        {
        iOpen = EFalse;
        CloseSubSession(ESipItcCloseSubSession);
        }
    iState = CSIPConnection::EInit;
	}

// -----------------------------------------------------------------------------
// RSIPConnection::SetState
// -----------------------------------------------------------------------------
//
void RSIPConnection::SetState (CSIPConnection::TState aState)
    {
    iState = aState;
    }

// -----------------------------------------------------------------------------
// RSIPConnection::State
// -----------------------------------------------------------------------------
//
CSIPConnection::TState RSIPConnection::State () const
    {
    return iState;
    }

// -----------------------------------------------------------------------------
// RSIPConnection::Send
// From MSIPITC
// -----------------------------------------------------------------------------
//
TInt RSIPConnection::Send(TSipItcFunctions aFunction, TIpcArgs& aArgs) const
	{
	// Sockopts can be set already before connection is active
    if (!iOpen || 
       ( iState != CSIPConnection::EActive && 
         aFunction != ESipItcSetSIPSockOpt && 
         aFunction != ESipItcGetConnectionError) )
        {
        return KErrNotReady;
        }
	return SendReceive(aFunction,aArgs);
	}

// -----------------------------------------------------------------------------
// RSIPConnection::Receive
// From MSIPITC
// -----------------------------------------------------------------------------
//
void RSIPConnection::Receive (TIpcArgs& aArgs,
                              TRequestStatus& aStatus)
	{
	SendReceive(ESipItcConnectionReadyToReceive,aArgs,aStatus);
	}

// -----------------------------------------------------------------------------
// RSIPConnection::Receive
// From MSIPITC
// -----------------------------------------------------------------------------
//
TInt RSIPConnection::Receive (TIpcArgs& aArgs)
	{
	return SendReceive(ESipItcConnectionReceiveSipMessage,aArgs);
	}

// -----------------------------------------------------------------------------
// RSIPConnection::CancelReceive
// From MSIPITC
// -----------------------------------------------------------------------------
//
void RSIPConnection::CancelReceive ()
	{
    // Add dummy parameters. Server expects TSIPIds for every request. 
    TPckgBuf<TSIPIds> sipIdsPckg;
    TIpcArgs args;
    args.Set (ESipItcArgIds, &sipIdsPckg);

    SendReceive (ESipItcConnectionCancelReceive,args);
	}
