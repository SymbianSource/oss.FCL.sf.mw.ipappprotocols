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
// server class for the rtp unit test cases
// 
//

/**
 @file
 @internalComponent
*/
 
#include "trtpcoreserver.h"
#include "trtpcorestep.h"

//for removing warning 11/06/2008
//static const TInt KStartOfItemValue = 2;
//static const TInt KStartOfFirstItemValue = 10;


// __EDIT_ME__ - Substitute the name of your test server 
_LIT(KServerName,"trtp2");
// __EDIT_ME__ - Use your own server class name
CTRtpCoreServer* CTRtpCoreServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTRtpCoreServer* server = new (ELeave) CTRtpCoreServer();
	CleanupStack::PushL(server);
	//CServer call
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

CTRtpCoreServer::CTRtpCoreServer()
	{
//	iUsingConnectedSocket = EFalse;
	}

CTRtpCoreServer::~CTRtpCoreServer()
	{
	delete iScheduler;
	//
//	delete iController;
//	delete iSendController;
	}

#if (!defined EKA2)
LOCAL_C void MainL()
/**
 * REQUIRES semaphore to sync with client as the Rendezvous()
 * calls are not available
 */
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	CTRtpCoreServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTRtpCoreServer::NewL());
	if(!err)
		{
		CleanupStack::PushL(server);
		RSemaphore sem;
		// The client API will already have created the semaphore
		User::LeaveIfError(sem.OpenGlobal(KServerName));
		CleanupStack::Pop(server);
		// Sync with the client then enter the active scheduler
		sem.Signal();
		sem.Close();
		sched->Start();
		}
	CleanupStack::Pop(sched);
	delete server;
	delete sched;
	}
#else
// EKA2 much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	// __EDIT_ME__ Your server name
	CTRtpCoreServer* server = NULL;
	// Create the CTestServer derived server
	// __EDIT_ME__ Your server name
	TRAPD(err,server = CTRtpCoreServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}
#endif

// Only a DLL on emulator for typhoon and earlier
#if (defined __WINS__ && !defined EKA2)
// and E32Dll(enum TDllReason) defined in rtputils.cpp
/*
GLDEF_C TInt E32Dll(enum TDllReason)
	{
	return 0;
	}
*/
#else
GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on exit
 */
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		{
		return KErrNoMemory;
		}
	TInt err = 0;
	TRAP(err,MainL());
	delete cleanup;
	return err;
	}
#endif

// Create a thread in the calling process
// Emulator typhoon and earlier
#if (defined __WINS__ && !defined EKA2)
TInt ThreadFunc (TAny* /*aParam*/)
/**
 * @return - Server exit code
 * @param - unused
 * Server Thread function. Guts of the code in the MainL() function
 */
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	return KErrNone;
	}

EXPORT_C TInt NewServer() 
/**
 * @return - Standard Epoc error codes
 * 1st and only ordinal, called by the client API to initialise the server
 */
	{
	_LIT(KThread,"Thread");
	RThread thread;
	// __EDIT_ME__ - Make sure the TBuf is large enough
	TBuf<KMaxTestExecuteNameLength> threadName(KServerName);
	// Create a hopefully unique thread name and use the ThreadFunc
	threadName.Append(KThread);
	const TInt KMaxHeapSize = 0x1000000;			//< Allow a 1Mb max heap
	TInt err = thread.Create(threadName, ThreadFunc, KDefaultStackSize,
													KMinHeapSize, KMaxHeapSize,
													NULL, EOwnerProcess);
	if(err)
		return err;
	thread.Resume();
	thread.Close();
	return KErrNone;
	}
#endif

#define DEF_TEST(_n, _f) if (aStepName == (_n)) { testStep = new _f; testStep->SetTestStepName((_n));}


CTestStep* CTRtpCoreServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTRtpCoreStep* testStep = NULL;
	// __EDIT_ME__ - Create your own test steps here
	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created

	DEF_TEST(KOom,COom);
	DEF_TEST(KSDESSetAttribute,CSDESSetAttribute);
	DEF_TEST(KSDESGetLength,CSDESGetLength);
	DEF_TEST(KSDESGetAttribute,CSDESGetAttribute);
	DEF_TEST(KSDESGetType,CSDESGetType);
	DEF_TEST(KAPPSetSubType,CAPPSetSubType);
	DEF_TEST(KAPPGetSubType,CAPPGetSubType);
	DEF_TEST(KAPPGetAppName,CAPPGetAppName);
	DEF_TEST(KBYESetSSRC,CBYESetSSRC);
	DEF_TEST(KBYESetReason,CBYESetReason);
	DEF_TEST(KBYEGetSSRC,CBYEGetSSRC);
	DEF_TEST(KBYEGetReason,CBYEGetReason);
	DEF_TEST(KSMNewL,CSMNewL);
	DEF_TEST(KSMSetRtpStreamParameters,CSMSetRtpStreamParameters);
	DEF_TEST(KSMMaxDropout,CSMMaxDropout);
	DEF_TEST(KSMMaxMisorder,CSMMaxMisorder);
	DEF_TEST(KSMMinSequential,CSMMinSequential);
	DEF_TEST(KSMByeTimeout,CSMByeTimeout);
	DEF_TEST(KSMShortTimeout,CSMShortTimeout);
	DEF_TEST(KSMLongTimeout,CSMLongTimeout);
	DEF_TEST(KSMNext,CSMNext);
	
	DEF_TEST(KRtpApiTest,CRtpApiTest);
	DEF_TEST(KRtpUtilsTest,CRtpUtilsTest);
	
	if (testStep)
		testStep->SetServer(this);
	

	return testStep;
	}
