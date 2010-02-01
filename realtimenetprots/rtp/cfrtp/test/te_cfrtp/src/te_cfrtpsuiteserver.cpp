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
// Test Suit for RTP SCPR and Rtp Flows.
// @file 
// @internalAll
// 
//

#include "te_cfrtpsuiteserver.h"
#include "cfrtp_basictests.h"

_LIT(KServerName,"te_cfrtpsuite");
CTe_CfrtpSuite* CTe_CfrtpSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_CfrtpSuite * server = new (ELeave) CTe_CfrtpSuite();
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
	CTe_CfrtpSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_CfrtpSuite::NewL());
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
	CTe_CfrtpSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_CfrtpSuite::NewL());
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
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
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
	return err;
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

CTestStep* CTe_CfrtpSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below 
      if(aStepName == KSubConTestStep)
      	{
      	testStep = new CSubConStep();
      	}
      if(aStepName == KRtpSockTestStep)
      	{
      	testStep = new CRtpSockTest();
      	}
      if(aStepName == KRtcpSockTestStep)
      	{
      	testStep = new CRtcpSockTest();
      	}
      if(aStepName == KRtpSendRecvStep)
      	{
      	testStep = new CRtpSendRecv();
      	}
      if(aStepName == KRtcpSendRecvTestStep)
      	{
      	testStep = new CRtcpSendRecv();
      	}
      if(aStepName == KRtpConnectTestStep)
      	{
      	testStep = new CRtpConnect();
      	}
      if(aStepName == KRtcpConnectTestStep)
      	{
      	testStep = new CRtcpConnect();
      	}
      if(aStepName == KRtpNoBindTestStep)
      	{
      	testStep = new CRtpNoBind();
      	}
      if(aStepName == KRtpBindTestStep)
      	{
      	testStep = new CRtpBind();
      	}
      if(aStepName == KRtpSockOptTestStep)
      	{
      	testStep = new CRtpSockOpt();
      	}
      if(aStepName == KRtcpSockOptTestStep)
       	{
       	testStep = new CRtcpSockOpt();
        }
      if(aStepName == KRtpIoctlTestStep)
      	{
      	testStep = new CRtpIoctl();
      	}
      if(aStepName == KBigSendRecvRtp)
        {
        testStep = new CBigSendRecvRtp();
        }
      if(aStepName == KRtpNegTests_01TestStep)
      	{
      	testStep = new CRtpNegTests_01();
      	}
      if(aStepName == KRtpNegTests_02TestStep)
      	{
      	testStep = new CRtpNegTests_02();
      	}
      if(aStepName == KRtpNegTests_03TestStep)
      	{
      	testStep = new CRtpNegTests_03();
      	}
      if(aStepName == KRtpNegTests_04TestStep)
      	{
      	testStep = new CRtpNegTests_04();
      	}
      if(aStepName == KRtpNegTests_05TestStep)
      	{
      	testStep = new CRtpNegTests_05();
      	}
      if(aStepName == KRtpNegTests_FailNoBearer)
      	{
      	testStep = new CRtpNegTestsFailNoBearer();
      	}
      if(aStepName == KRtpShutDownImmediate)
      	{
      	testStep = new CRtpShutDownImmediate();
      	}
      if(aStepName == KRtcpShutDownImmediate)
      	{
      	testStep = new CRtcpShutDownImmediate();
      	}
	  if(aStepName == KShutDownImmediate)
	  	{
		testStep = new CShutDownImmediate();
	  	}
	return testStep;
	}
