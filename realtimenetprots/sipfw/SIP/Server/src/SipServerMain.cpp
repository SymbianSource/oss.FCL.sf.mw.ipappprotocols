// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : SipServerMain.cpp
// Part of       : SIPServerCore
// Version       : SIP/3.0 
//




#include "sipclientserver.h"
#include "SipServerMain.h"
#include "CSipServerCore.h"

// -----------------------------------------------------------------------------
// SipServerMain::ThreadFunction
// -----------------------------------------------------------------------------
//
TInt SipServerMain::ThreadFunction (TAny* /*aNone*/)
	{
    TInt err = KErrNone;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if (!cleanupStack) 
        {
        PanicServer(ECreateTrapCleanup);
        }
	RSemaphore semaphore;
	err = semaphore.OpenGlobal(KSipServerSemaphoreName);
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
// SipServerMain::ThreadFunctionL
// -----------------------------------------------------------------------------
//
void SipServerMain::ThreadFunctionL (RSemaphore& aSemaphore)
	{
	// Give a name to this thread
    User::LeaveIfError(User::RenameThread(KSipServerName));

    // Construct server
    CSipServerCore::NewLC();

	// Server created ok
	aSemaphore.Signal();
	aSemaphore.Close();

	// Start handling requests
	CActiveScheduler::Start();

    // This will be executed after the active scheduler has been stopped:
    CleanupStack::PopAndDestroy(1); // server
	}

// -----------------------------------------------------------------------------
// SipServerMain::PanicServer
// -----------------------------------------------------------------------------
//
void SipServerMain::PanicServer (TSipCSServerPanic aPanic)
	{
    User::Panic(KSipCSServerPanic, aPanic);
	}

// -----------------------------------------------------------------------------
// E32Main
// -----------------------------------------------------------------------------
//
TInt E32Main() 
    { 
    return SipServerMain::ThreadFunction(NULL);
    }
