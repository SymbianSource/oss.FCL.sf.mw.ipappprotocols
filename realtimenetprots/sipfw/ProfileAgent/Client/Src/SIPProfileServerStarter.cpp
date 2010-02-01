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
// Name        : sipprofileserverstarter
// Part of     : SIP Profile Client
// implementation
// Version     : 1.0
// INCLUDE FILES
//



#include <e32math.h>
#include "SIPProfileServerStarter.h"
#include "sipprofilecs.h"
#include "sipprofileclient.pan"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TSIPProfileServerStarter::Start
// -----------------------------------------------------------------------------
//
TInt TSIPProfileServerStarter::Start()
	{	
    TFindServer findServer(KSipProfileServerName);
    TFullName name;
	TInt err = findServer.Next(name);
	if (err == KErrNone) 
	    {
	    return KErrNone;
	    }
	// Start the server 
	RSemaphore semaphore;
	err = semaphore.CreateGlobal(KSipProfileServerSemaphoreName,0); 
    if (err != KErrNone)
        {
        err = semaphore.OpenGlobal(KSipProfileServerSemaphoreName);
        if (err != KErrNone)
            {
            return err;
            }
        }
    err = CreateServerProcess();
    if (err == KErrNone) 
        {
        semaphore.Wait();
	    }
	semaphore.Close();
    return err;
	}

// -----------------------------------------------------------------------------
// TSIPProfileServerStarter::CreateServerProcess
// -----------------------------------------------------------------------------
//
TInt TSIPProfileServerStarter::CreateServerProcess()
	{
    TInt err;
	const TUidType serverUid(KNullUid, KNullUid, KServerUid3);
	RProcess server;
	err = server.Create(KSipProfileServerName, _L(""), serverUid);
    if (err != KErrNone) 
        {
        return err;
        }
	server.Resume();
	server.Close();
    return  KErrNone;
	}
