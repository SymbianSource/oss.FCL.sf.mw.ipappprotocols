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
// Name         : SIPClientResolverServerStarter.cpp
// Part of      : SIP Client Resolver
// Version      : 1.0
//



// INCLUDES
#include <e32math.h>
#include "SIPClientResolverServerStarter.h"
#include "sipCRclientserver.h"


// ----------------------------------------------------------------------------
// SIPClientResolverServerStarter::Start
// ----------------------------------------------------------------------------
//
TInt SIPClientResolverServerStarter::Start ()
	{
	TInt err = KErrNone;

    TFindServer findServer(KSipClientResolverServerName);
    TFullName name;
	if (findServer.Next(name) == KErrNone) 
        {
        return KErrNone; // Server already running
        }
		
	RSemaphore semaphore;
	err = semaphore.CreateGlobal(KSipClientResolverServerSemaphoreName,0);
	if (err != KErrNone)
		{
		return err;
		}

	err = CreateServerProcess(semaphore);
	semaphore.Close ();

    return err;
	}

// ----------------------------------------------------------------------------
// SIPClientResolverServerStarter::CreateServerProcess
// ----------------------------------------------------------------------------
//
TInt SIPClientResolverServerStarter::CreateServerProcess(
    RSemaphore& aSemaphore)
	{
	TInt err = KErrNone;
	const TUidType serverUid (KNullUid, KNullUid, KServerUid3);
	RProcess server;
	err = server.Create(KSipClientResolverServerName,KNullDesC,serverUid);
    if (err)
        {
        return err;
        }
	server.Resume ();
	aSemaphore.Wait();
	TInt exitReason = server.ExitReason();
	server.Close ();
    return exitReason;
	}

//  End of File  
