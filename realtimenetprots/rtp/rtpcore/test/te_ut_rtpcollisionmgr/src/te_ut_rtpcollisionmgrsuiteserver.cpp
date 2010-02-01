// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// @internalAll
// 
//

#include "te_ut_rtpcollisionmgrsuiteserver.h"
#include "changessrcstep.h"
#include "createnewsourcelstep.h"
#include "deleteentrystep.h"
#include "findentrystep.h"
#include "getlocalentrystep.h"
#include "getvalidentriesstep.h"
#include "iscollisionorloopbackstep.h"
#include "newlstep.h"
#include "resolvertpcollisionorlooplstep.h"


_LIT(KServerName,"te_ut_rtpcollisionmgrsuite");
CTe_UT_RtpCollisionMgrSuite* CTe_UT_RtpCollisionMgrSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_UT_RtpCollisionMgrSuite * server = new (ELeave) CTe_UT_RtpCollisionMgrSuite();
	CleanupStack::PushL(server);
	// CServer base class call which can be either StartL or ConstructL,
	// the later will permit Server Logging.

	server->StartL(KServerName);
	//server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

#if (!defined EKA2)
LOCAL_C void MainL()
/**
 * Non-secure variant.
 * REQUIRES semaphore to sync with client as the Rendezvous()
 * calls are not available
 */
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	CTe_UT_RtpCollisionMgrSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_UT_RtpCollisionMgrSuite::NewL());
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

// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTe_UT_RtpCollisionMgrSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_UT_RtpCollisionMgrSuite::NewL());
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

#if (defined __WINS__ && !defined EKA2)
// Non-secure variants
GLDEF_C TInt E32Dll(enum TDllReason)
	{
	return 0;
	}
#else

GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	//for removing warning 11/06/2008
	if(err != KErrNone)
	{
		return err;	
	}
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }
#endif

#if (defined __WINS__ && !defined EKA2)

TInt ThreadFunc (TAny* /*aParam*/)
/**
 * @return - Server exit code
 * @param - unused
 * Non-secure only
 * Server Thread function. Guts of the code in the MainL() function
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}

EXPORT_C TInt NewServer() 
/**
 * @return - Standard Epoc error codes
 * Non-secure variant only
 * 1st and only ordinal, called by the client API to initialise the server
 */
	{
	_LIT(KThread,"Thread");
	RThread thread;
	TBuf<KMaxTestExecuteNameLength> threadName(KServerName);
	// Create a hopefully unique thread name and use the ThreadFunc
	threadName.Append(KThread);
	const TInt KMaxHeapSize = 0x1000000;			//< Allow a 1Mb max heap
	TInt err = thread.Create(threadName, ThreadFunc, KDefaultStackSize, KMinHeapSize, KMaxHeapSize, NULL, EOwnerProcess);
	if(err)
		return err;
	thread.Resume();
	thread.Close();
	return KErrNone;
	}

#endif

CTestStep* CTe_UT_RtpCollisionMgrSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below 
              if(aStepName == KchangessrcStep)
                            testStep = new CchangessrcStep();
              else if(aStepName == KcreatenewsourcelStep)
                            testStep = new CcreatenewsourcelStep();
              else if(aStepName == KdeleteentryStep)
                            testStep = new CdeleteentryStep();
              else if(aStepName == KfindentryStep)
                            testStep = new CfindentryStep();
              else if(aStepName == KgetlocalentryStep)
                            testStep = new CgetlocalentryStep();
              else if(aStepName == KgetvalidentriesStep)
                            testStep = new CgetvalidentriesStep();
              else if(aStepName == KiscollisionorloopbackStep)
                            testStep = new CiscollisionorloopbackStep();
              else if(aStepName == KnewlStep)
                            testStep = new CnewlStep();
              else if(aStepName == KresolvertpcollisionorlooplStep)
                            testStep = new CresolvertpcollisionorlooplStep();

	return testStep;
	}
