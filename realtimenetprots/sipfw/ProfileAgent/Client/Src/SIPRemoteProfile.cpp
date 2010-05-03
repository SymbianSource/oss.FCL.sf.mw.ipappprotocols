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
// Name        : sipremoteprofile
// Part of     : SIP Profile Client
// implementation
// Version     : 1.0
// INCLUDE FILES
//



#include "SIPRemoteProfile.h"
#include "sipprofilecs.h"
#include "SIPProfileServerStarter.h"
#include "sipprofileclient.pan"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RSIPProfile::RSIPProfile
// -----------------------------------------------------------------------------
//
RSIPProfile::RSIPProfile()
 : RSessionBase (), iConnected (EFalse)
	{
	}

// -----------------------------------------------------------------------------
// RSIPProfile::Version
// -----------------------------------------------------------------------------
//
TVersion RSIPProfile::Version(void) const
	{
    TVersion version (KSipProfileServerMajorVersionNumber,
                      KSipProfileServerMinorVersionNumber,
                      KSipProfileServerBuildVersionNumber);
    return version;
	}

// -----------------------------------------------------------------------------
// RSIPProfile::Connect
// -----------------------------------------------------------------------------
//
TInt RSIPProfile::Connect()
	{
	TBool stopTrying(EFalse);
    TInt sessionConnectErr(KErrNone);
    TInt serverStartErr(KErrNone);
    const TInt KMaxRetries(10);
    TInt retryCount(0);
    
    while (!stopTrying && retryCount < KMaxRetries)
        {
        sessionConnectErr = CreateSession(KSipProfileServerName,Version());
        if ((sessionConnectErr != KErrNotFound) 
                && (sessionConnectErr != KErrServerTerminated))
            {
            stopTrying = ETrue;
            }
        else
            {
            serverStartErr = TSIPProfileServerStarter::Start();
            if ((serverStartErr != KErrNone) && (serverStartErr != KErrAlreadyExists))
                {
                stopTrying = ETrue;
                }
            }
        retryCount++;
	    }
	    
    iConnected = (sessionConnectErr == KErrNone);
    return sessionConnectErr;
	}

// -----------------------------------------------------------------------------
// RSIPProfile::Close
// -----------------------------------------------------------------------------
//
void RSIPProfile::Close()
	{
    iConnected = EFalse;
    RHandleBase::Close();
	}

// -----------------------------------------------------------------------------
// RSIPProfile::Send
// -----------------------------------------------------------------------------
//
TInt RSIPProfile::Send(TIpcArgs& aArgs, TSipProfileItcFunctions aFunction)
	{
	__ASSERT_ALWAYS (iConnected, Panic(ESipClientNotConnected));

    return SendReceive (aFunction, aArgs);
	}

// -----------------------------------------------------------------------------
// RSIPProfile::Receive
// -----------------------------------------------------------------------------
//
void RSIPProfile::Receive(TIpcArgs& aArgs, TRequestStatus& aStatus)

	{
	__ASSERT_ALWAYS (iConnected, Panic(ESipClientNotConnected));

    SendReceive(ESipProfileItcOpReadyToReceive,aArgs,aStatus);
	}

// -----------------------------------------------------------------------------
// RSIPProfile::CancelReceive
// -----------------------------------------------------------------------------
//
void RSIPProfile::CancelReceive()
	{
    __ASSERT_ALWAYS (iConnected, Panic(ESipClientNotConnected));

    SendReceive (ESipProfileItcOpCancelReceive, TIpcArgs());
	}

// -----------------------------------------------------------------------------
// RSIPProfile::Panic
// -----------------------------------------------------------------------------
//
void RSIPProfile::Panic(TInt aReason)
	{
    User::Panic(KSipProfileClient, aReason);
	}
