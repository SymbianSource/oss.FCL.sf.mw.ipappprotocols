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
// Name          : SipCRServerMain.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



// INCLUDES
#include "sipCRclientserver.h"
#include "SipCRServerMain.h"
#include "CSipCRServer.h"

// -----------------------------------------------------------------------------
// SipCRServerMain::ThreadFunction
// -----------------------------------------------------------------------------
//
TInt SipCRServerMain::ThreadFunction (TAny* /*aNone*/)
	{
    TInt err = KErrNone;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if (!cleanupStack) 
        {
        PanicServer(ECreateTrapCleanup);
        }
	RSemaphore semaphore;
	err = semaphore.OpenGlobal(KSipClientResolverServerSemaphoreName);
    if (err != KErrNone)
        {
        PanicServer(ESrvCreateServer);
        }
    TRAP(err, ThreadFunctionL(semaphore));
    if (err != KErrNone)
        {
        semaphore.Signal();
        semaphore.Close();
        }

    delete cleanupStack;
    return err;
	}

// -----------------------------------------------------------------------------
// SipCRServerMain::ThreadFunctionL
// -----------------------------------------------------------------------------
//
void SipCRServerMain::ThreadFunctionL (RSemaphore& aSemaphore)
	{
	// Give a name to this thread
    User::LeaveIfError(User::RenameThread(KSipClientResolverServerName));

    // Construct server
    CSIPCRServer::NewLC();

	// Server created ok
	aSemaphore.Signal();
	aSemaphore.Close();

	// Start handling requests
	CActiveScheduler::Start();

    // This will be executed after the active scheduler has been stopped:
    CleanupStack::PopAndDestroy(1); // server
	}

// -----------------------------------------------------------------------------
// SipCRServerMain::PanicServer
// -----------------------------------------------------------------------------
//
void SipCRServerMain::PanicServer (TSipCRServerPanic aPanic)
	{
    User::Panic(KSipCRServerPanic, aPanic);
	}

// -----------------------------------------------------------------------------
// E32Main
// -----------------------------------------------------------------------------
//
TInt E32Main() 
    { 
    return SipCRServerMain::ThreadFunction(NULL);
    }

//  End of File  
