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
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file
*/

#include "te_rtpsuiteserver.h"
#include "rtpstep.h"

_LIT(KServerName,"Te_rtpSuite");
CTe_rtpSuite* CTe_rtpSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_rtpSuite * server = new (ELeave) CTe_rtpSuite();
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
	CTe_rtpSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_rtpSuite::NewL());
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
	CTe_rtpSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_rtpSuite::NewL());
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
	if(err != KErrNone)
		return err;
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

CTestStep* CTe_rtpSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	//CTestStep* testStep = NULL;
	CrtpStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below
              if(aStepName == KrtpStep)
                            testStep = new CrtpStep();
              else if(aStepName == KrtpOpenSessionStep)
                            testStep = new CrtpOpenSessionStep();
              else if(aStepName == KrtpCloseSessionStep)
            	  			testStep = new CrtpCloseSessionStep();
			  #ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
              else if(aStepName == KrtpOpenSessionTwoStep)
            	  			testStep = new CrtpOpenSessionTwoStep();
              else if(aStepName == KrtpCloseSessionTwoStep)
            	  			testStep = new CrtpCloseSessionTwoStep();
              else if(aStepName == KrtpMultipleSessionTwoStep)
            	  			testStep = new CrtpMultipleSessionTwoStep();
              else if(aStepName == KrtpMultipleSessionTwoCloseStep)
  	  						testStep = new CrtpMultipleSessionTwoCloseStep();
			  #endif
              else if(aStepName == KrtpMakePacketStep)
                            testStep = new CrtpMakePacketStep();
              else if(aStepName == KrtpSendPacketStep)
                            testStep = new CrtpSendPacketStep();
              else if(aStepName == KrtpSetMarkerStep)
                            testStep = new CrtpSetMarkerStep();
              else if(aStepName == KrtpCheckMarkerStep)
                            testStep = new CrtpCheckMarkerStep();
              else if(aStepName == KrtpMultipleSessionStep)
                            testStep = new CrtpMultipleSessionStep();
              else if(aStepName == KrtpMultipleSessionCloseStep)
            	  			testStep = new CrtpMultipleSessionCloseStep();
              else if(aStepName == KrtpSetHeaderExtensionStep)
            	  			testStep = new CrtpSetHeaderExtensionStep();
              else if(aStepName == KrtpCheckHeaderExtensionStep)
            	  			testStep = new CrtpCheckHeaderExtensionStep();
              else if(aStepName == KrtpSetPayloadTypeStep)
            	  			testStep = new CrtpSetPayloadTypeStep();
              else if(aStepName == KrtpCheckPayloadTypeStep)
            	  			testStep = new CrtpCheckPayloadTypeStep();
              else if(aStepName == KrtpSetTimestampStep)
            	  			testStep = new CrtpSetPayloadTypeStep();
              else if(aStepName == KrtpCheckTimestampStep)
  	  						testStep = new CrtpCheckPayloadTypeStep();
              else if(aStepName == KrtpRegisterNonRtpDataStep)
  	  						testStep = new CrtpRegisterNonRtpDataStep();
              else if(aStepName == KrtpSendNonRtpDataStep)
  	  						testStep = new CrtpSendNonRtpDataStep();
              else if(aStepName == KrtpReceiveNonRtpDataStep)
  	  						testStep = new CrtpReceiveNonRtpDataStep();
              else if(aStepName == KrtpUnregisterNonRtpDataStep)
  	  						testStep = new CrtpUnregisterNonRtpDataStep();
              else if(aStepName == KrtpRegisterNonRtpAgainStep)
  	  						testStep = new CrtpRegisterNonRtpAgainStep();
              else if(aStepName == KrtpPreAndPostProcessingTestStep)
  	  						testStep = new CrtpPreAndPostProcessingTestStep();
              else if(aStepName == KrtpPreProcessingFailTestStep)
  	  						testStep = new CrtpPreAndPostProcessingTestStep();
              else if(aStepName == KrtpStartSchedulerStep)
  	  						testStep = new CrtpStartSchedulerStep();
              else if(aStepName == KrtpChangeRemoteAddrToLoopStep)
  	  						testStep = new CrtpChangeRemoteAddrToLoopStep();
              else if(aStepName == KrtpSetupLocalRecvSockStep)
  	  						testStep = new CrtpSetupLocalRecvSockStep();
              else if(aStepName == KrtpRecvOnLocalSockStep)
  	  						testStep = new CrtpRecvOnLocalSockStep();
              else if(aStepName == KrtpSetAndGetOptStep)
  	  						testStep = new CrtpSetAndGetOptStep();
              else if(aStepName == KrtpSetSamplingRateStep)
  	  						testStep = new CrtpSetSamplingRateStep();
              else if(aStepName == KrtpCheckRecvPayloadStep)
  	  						testStep = new CrtpCheckRecvPayloadStep();
              
	if (testStep)
		testStep->SetServer(this);
	return testStep;
	}

void CTe_rtpSuite::PacketSentSuccess(CTe_rtpSuite* aPtr, const TRtpEvent& aEvent)
{
	TRtpEventType type;
	type = aEvent.Type();
	if (type == ERtpSendSucceeded)
		{
		//Do Nothing.
		}
	if (aPtr->StopScheduler==1)
		{
		CActiveScheduler::Stop();
		}
}


void CTe_rtpSuite::NonRtpDataReceived(CTe_rtpSuite* , const TRtpEvent& aEvent)
{
	TRtpEventType type;
	type = aEvent.Type();
	if (type == ENonRtpDataReceived)
		{
		//Do Nothing.
		}
	CActiveScheduler::Stop();
}

void CTe_rtpSuite::NewSourceL(CTe_rtpSuite* aPtr, const TRtpEvent& aEvent)
	{

	aPtr->rtpRecvSrc = aEvent.Session().NewReceiveSourceL();
	aPtr->rtpRecvSrc.RegisterEventCallbackL(ERtpPacketReceived, CTe_rtpSuite::PacketReceiveSuccess, aPtr);
	}


void CTe_rtpSuite::PacketReceiveSuccess(CTe_rtpSuite* aPtr, const TRtpEvent& aEvent)
	{
	if (aPtr->StopScheduler==1)
		{
		CActiveScheduler::Stop();
		}
	else
		{
		TUint payloadtype = 0;
		TRAPD(err,payloadtype = aEvent.ReceiveSource().PayLoadTypeL());
		if(KErrNone != err)
			{
			aPtr->iCallBackPass = EFalse;
			}
		aPtr->rtpRecvPacket = aEvent.ReceiveSource().Packet();
		aPtr->iCallBackPass = (payloadtype == aPtr->rtpRecvPacket.PayloadType());
		TPtrC8 writeDesc(aPtr->rtpRecvPacket.Payload().Ptr(),aPtr->rtpRecvPacket.Payload().Length());
		CActiveScheduler::Stop();
		}
	}


void CTe_rtpSuite::ConfigureReflectorModeL(TBool aMode, TInetAddr& aAddr)
	{
    /* Configures the reflector to change the SSRC or not
     */	
	RSocketServ sockServer;
	//Open a connection
	RConnection conn;
	User::LeaveIfError(sockServer.Connect());
	CleanupClosePushL(sockServer);
	
	User::LeaveIfError(conn.Open(sockServer));
	CleanupClosePushL(conn);
	
	//Start the connection
	User::LeaveIfError(conn.Start());
	
	RSocket ctrlSock;
	User::LeaveIfError(ctrlSock.Open(sockServer,KAfInet, KSockDatagram, KProtocolInetUdp, conn));
	CleanupClosePushL(ctrlSock);
	
    TBuf8<64> lCmdBuff;
    TUint8* lPtr = const_cast<TUint8*>(lCmdBuff.Ptr());
    TUint32* lIntData = (TUint32*)lPtr;
    *lIntData = ByteOrder::Swap32(0xFF00BABE);
    if (aMode)
    	{
    	*(lIntData + 1) = ByteOrder::Swap32(0xCD000100);
    	}
    else
    	{
    	*(lIntData + 1) = ByteOrder::Swap32(0xCD000101);
    	}
    lCmdBuff.SetLength(64);
    
    TRequestStatus lMyReqStat;
    ctrlSock.SendTo(lCmdBuff,aAddr,0,lMyReqStat);
    User::WaitForRequest(lMyReqStat);
    
    ctrlSock.Close();
    conn.Close();
    sockServer.Close();
    
    CleanupStack::Pop(3);
	}

