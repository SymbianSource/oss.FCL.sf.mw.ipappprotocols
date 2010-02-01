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
// Name          : SIPServerStarter.cpp
// Part of       : SIPClient
// Version       : SIP/4.0 
//




#include <e32math.h>
#include "SIPServerStarter.h"
#include "sipclientserver.h"
#include "sipclient.pan"

_LIT(KSipServerStarterMutex, "SipServerStarterMutex");

#define RETURN_IF_ERROR(err) {TInt _err=err; if(_err!=KErrNone) {return _err;}}

// -----------------------------------------------------------------------------
// SIPServerStarter::Start
// -----------------------------------------------------------------------------
//
TInt SIPServerStarter::Start ()
	{
	TInt err = KErrNone;

    RMutex mutex;
    err = mutex.CreateGlobal(KSipServerStarterMutex);
    if (err != KErrNone)
        {
        err = mutex.OpenGlobal(KSipServerStarterMutex);
        if (err != KErrNone)
            {
            return err;
            }
        }
    mutex.Wait();   
        {
        // Protected with a mutex
        TFindServer findServer(KSipServerName);
        TFullName name;
	    if (findServer.Next(name) == KErrNone) 
            {
            mutex.Signal();
            mutex.Close();
            return KErrNone; // Server already running
            }
		
	    RSemaphore semaphore;
	    err = semaphore.CreateGlobal(KSipServerSemaphoreName,0);
	    if (err == KErrNone)
		    {
	        err = CreateServerProcess(semaphore);
	        semaphore.Close();
		    }
        }
    mutex.Signal();
    mutex.Close();

    return err;
	}

// -----------------------------------------------------------------------------
// SIPServerStarter::CreateServerProcess
// -----------------------------------------------------------------------------
//
TInt SIPServerStarter::CreateServerProcess (RSemaphore& aSemaphore)
	{
	const TUidType serverUid (KNullUid, KNullUid, KServerUid3);
	RProcess server;
	RETURN_IF_ERROR(server.Create(KSipServerName,KNullDesC,serverUid))
	server.Resume ();
	aSemaphore.Wait();
	TInt exitReason = server.ExitReason();
	server.Close ();
    return exitReason;
	}
