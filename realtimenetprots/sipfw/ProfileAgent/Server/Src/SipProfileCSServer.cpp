// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : SIPProfileCSServer.cpp
// Part of     : SIP Profile Server
// implementation
// Version     : 1.0
//



// INCLUDE FILES
#include "SipProfileActiveScheduler.h"
#include "SipProfileCSServer.h"
#include "SipProfileCSSession.h"
#include "SipProfileCSServer.pan"
#include "SipProfileCSServerCloseTimer.h"
#include "SipProfileSecurityPolicy.h"
#include "SipProfileLog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileCSServer::NewL
// -----------------------------------------------------------------------------
//
CSIPProfileCSServer*
CSIPProfileCSServer::NewL(CSIPProfileServerCore& aServerCore)
	{
    CSIPProfileCSServer* self = CSIPProfileCSServer::NewLC(aServerCore);
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSServer::NewLC
// -----------------------------------------------------------------------------
//
CSIPProfileCSServer*
CSIPProfileCSServer::NewLC(CSIPProfileServerCore& aServerCore)
	{
	CSIPProfileCSServer* self =
	    new (ELeave) CSIPProfileCSServer(aServerCore,EPriorityHigh);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSServer::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSServer::ConstructL()
	{
	const TUint	KSIPProfileServerTimerValue = 5;
    StartL(KSipProfileServerName);	
	if (iServerCore.CanServerStop()) 
		{
		PROFILE_DEBUG1("ProfileServer shutdown timer started")
		iCloseTimer = CSipProfileCSServerCloseTimer::NewL();
		iCloseTimer->StopActiveSchedulerAfter(KSIPProfileServerTimerValue);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSServer::CSIPProfileCSServer
// -----------------------------------------------------------------------------
//
CSIPProfileCSServer::CSIPProfileCSServer(
    CSIPProfileServerCore& aServerCore,
    TInt aPriority)
 : CPolicyServer(aPriority, TSipProfilePlatSecPolicy),
   iServerCore(aServerCore)
	{
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSServer::~CSIPProfileCSServer
// -----------------------------------------------------------------------------
//
CSIPProfileCSServer::~CSIPProfileCSServer()
	{
	delete iCloseTimer;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSServer::NewSessionL
// -----------------------------------------------------------------------------
//
CSession2* CSIPProfileCSServer::NewSessionL(const TVersion &aVersion,
                                            const RMessage2& /*aMessage*/) const
	{
	PROFILE_DEBUG1("CSIPProfileCSServer::NewSessionL")

	// Check we're the right version
	if (!User::QueryVersionSupported(
		TVersion(KSipProfileServerMajorVersionNumber,
                 KSipProfileServerMinorVersionNumber,
                 KSipProfileServerBuildVersionNumber),
                 aVersion))
		{
		User::Leave(KErrNotSupported);
		}

	if (iServerCore.BackupInProgress())
		{
		User::Leave(KErrLocked);
		}

	PROFILE_DEBUG1("CSIPProfileCSServer::NewSessionL, exit")
    return CSIPProfileCSSession::NewL(*const_cast<CSIPProfileCSServer*>(this),
                                      iServerCore);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSServer::IncrementSessions
// -----------------------------------------------------------------------------
//
void CSIPProfileCSServer::IncrementSessions()
	{
    iSessionCount++;
	if (iCloseTimer)
		{
		PROFILE_DEBUG1("ProfileServer shutdown timer stopped")
		delete iCloseTimer;
		iCloseTimer = NULL;
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSServer::DecrementSessions
// -----------------------------------------------------------------------------
//
void CSIPProfileCSServer::DecrementSessions()
	{
    iSessionCount--;

	if (iSessionCount == 0 && iServerCore.CanServerStop())
		{
	    CActiveScheduler::Stop();
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSServer::ServerCanStop
// -----------------------------------------------------------------------------
//
void CSIPProfileCSServer::ServerCanStop()
	{
	if (iSessionCount == 0)
		{
		CActiveScheduler::Stop();
		}	
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::ShutdownL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSServer::ShutdownL()
	{
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::CleanShutdownL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSServer::CleanShutdownL()
	{
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSServer::RunError
// -----------------------------------------------------------------------------
//
TInt CSIPProfileCSServer::RunError(TInt aError)
	{
    Message().Complete(aError);
	// The leave will result in an early return from CServer::RunL(), skipping
	// the call to request another message. So do that now in order to keep the
	// server running.
	ReStart();
	return KErrNone; // handled the error fully
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSServer::PanicClient
// -----------------------------------------------------------------------------
//
void CSIPProfileCSServer::PanicClient(const RMessage2& aMessage,
                                      TSIPProfileCSPanic aPanic)
	{
    aMessage.Panic(KSipProfileCSServerPanic, aPanic);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSServer::PanicServer
// -----------------------------------------------------------------------------
//
void CSIPProfileCSServer::PanicServer(TSIPProfileCSPanic aPanic)
	{
    User::Panic(KSipProfileCSServerPanic, aPanic);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSServer::ThreadFunctionL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSServer::ThreadFunctionL(RMutex& aMutex)
	{
	// Give a name to this thread
	User::LeaveIfError(User::RenameThread(KSipProfileServerName));

    // Construct our server
    CActiveScheduler* activeScheduler = new (ELeave) CSIPProfileActiveScheduler;
    CActiveScheduler::Install(activeScheduler);
    CleanupStack::PushL(activeScheduler);
    CSIPProfileServerCore::NewLC();
	if (SignalClientSemaphore() == KErrNone)
		{
		// Semaphore signalled ok
		PROFILE_DEBUG1("Start initiated by client")
		}
	else
		{
		PROFILE_DEBUG1("Start initiated by non-client")
		// Server started by non-client. No need to signal.
		}	
	aMutex.Signal();
    aMutex.Close();

	// Start handling requests
	CActiveScheduler::Start();

    // This will be executed after the active scheduler has been stopped:
    CleanupStack::PopAndDestroy(2); // server, activeScheduler
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSServer::ThreadFunction
// -----------------------------------------------------------------------------
//
TInt CSIPProfileCSServer::ThreadFunction(TAny* /*aNone*/)
	{
	_LIT(KSipProfileServerMutexName, "SipProfileServerMutex");

    CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if (!cleanupStack) 
	    {
	    PanicServer(ECreateTrapCleanup);
	    }

	// Protect server starting code with a mutex.
	RMutex mutex;
	TInt err = mutex.CreateGlobal(KSipProfileServerMutexName);
    if (err != KErrNone)
        {
        err = mutex.OpenGlobal(KSipProfileServerMutexName);
        if (err != KErrNone)
            {
            delete cleanupStack;
            PanicServer(ESrvCreateServer);
            return err; // silence warnings
            }
        }
    mutex.Wait(); // Wait here until nobody else executes the code below.

    TFindServer findServer(KSipProfileServerName);
    TFullName name;
	if (findServer.Next(name) == KErrNone) 
        {
        // Server already running.
        SignalClientSemaphore();
        mutex.Signal();
        mutex.Close();
        }
    else
        {
        // Server not running. Try to start it.
        TRAP(err, ThreadFunctionL(mutex));
        if (err != KErrNone)
            {
            SignalClientSemaphore();
	        mutex.Signal();
	        mutex.Close();	
            } 
        }
	               
    delete cleanupStack;
    return err;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSServer::SignalClientSemaphore
// -----------------------------------------------------------------------------
//
TInt CSIPProfileCSServer::SignalClientSemaphore()
    {
    RSemaphore semaphore;
	TInt err = semaphore.OpenGlobal(KSipProfileServerSemaphoreName);
	if (err == KErrNone)
	    {
		semaphore.Signal();
		semaphore.Close();
		}
	return err; 
    }

// -----------------------------------------------------------------------------
// E32Main
// -----------------------------------------------------------------------------
//
TInt E32Main() 
    { 
	PROFILE_DEBUG1("Process created")
    return CSIPProfileCSServer::ThreadFunction(NULL);
    }
