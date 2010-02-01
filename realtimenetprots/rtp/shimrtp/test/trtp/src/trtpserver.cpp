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
 
#include "trtpstep.h"

void RemoveLastSource(TAny* aPtr)
	{
	RArray<RRtpReceiveSource>* array = 
		reinterpret_cast<RArray<RRtpReceiveSource>*>(aPtr);
	array->Remove(array->Count() - 1);
	}

// __EDIT_ME__ - Substitute the name of your test server 
_LIT(KServerName,"Trtp");
// __EDIT_ME__ - Use your own server class name
CTrtpServer* CTrtpServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTrtpServer * server = new (ELeave) CTrtpServer();
	CleanupStack::PushL(server);
	server->ConstructL();
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

void CTrtpServer::ConstructL()
	{
	}

CTrtpServer::~CTrtpServer()
	{
	iRtpSocket.Close();
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
	// __EDIT_ME__ Your server name here
	CTrtpServer* server = NULL;
	// Create the CTestServer derived server . __EDIT_ME__ Your server
	TRAPD(err,server = CTrtpServer::NewL());
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
	CTrtpServer* server = NULL;
	// Create the CTestServer derived server
	// __EDIT_ME__ Your server name
	TRAPD(err,server = CTrtpServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		
		User::SetCritical(User::ESystemCritical);

		sched->Start();
		}
	delete server;
	delete sched;
	}
#endif

// Only a DLL on emulator for typhoon and earlier
#if (defined __WINS__ && !defined EKA2)
GLDEF_C TInt E32Dll(enum TDllReason)
	{
	return 0;
	}
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
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
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


CTestStep* CTrtpServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTrtpStep* testStep = NULL;
	// __EDIT_ME__ - Create your own test steps here
	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	DEF_TEST(KMakePacket, CMakePacket);
	DEF_TEST(KClosePacket, CClosePacket);
	DEF_TEST(KSetPayloadType, CSetPayloadType);
	DEF_TEST(KPayloadType, CPayloadType);
	DEF_TEST(KMarker, CMarker);
	DEF_TEST(KSetMarker, CSetMarker);
	DEF_TEST(KExtension, CExtension);
	DEF_TEST(KFlags, CFlags);
	DEF_TEST(KSetFlags, CSetFlags);
	DEF_TEST(KSequenceNumber, CSequenceNumber);
	DEF_TEST(KTimestamp, CTimestamp);
	DEF_TEST(KSetTimestamp, CSetTimestamp);
	DEF_TEST(KSSRC, CSSRC);
	DEF_TEST(KOpenSession, COpenSession);
	DEF_TEST(KOpenSessionRemoved, COpenSessionRemoved);
	DEF_TEST(KCloseSession, CCloseSession);
	DEF_TEST(KTidyUp, CTidyUp);
	DEF_TEST(KRxPacket, CRxPacket);
	DEF_TEST(KPayload, CPayload);
	DEF_TEST(KSetRxSize, CSetRxSize);
	DEF_TEST(KCheckEvent, CCheckEvent);
	DEF_TEST(KEvent, CEvent);
	DEF_TEST(KOom, COom);
	DEF_TEST(KCSRCs, CCSRCs);
	DEF_TEST(KExtensionContent, CExtensionContent);
	DEF_TEST(KSend, CSend);
	DEF_TEST(KSetPayload, CSetPayload);
	DEF_TEST(KSetAlignment, CSetAlignment);
	DEF_TEST(KSetStreamPayloadType, CSetStreamPayloadType);
	DEF_TEST(KCheckStatus, CCheckStatus);
	DEF_TEST(KWorkConnected, CWorkConnected);
	DEF_TEST(KOpenSessionRConnect, COpenSessionRConnect);
	DEF_TEST(KOpenSessionRtcp, COpenSessionRtcp);
	DEF_TEST(KOpenSessionRtcpRemoved, COpenSessionRtcpRemoved);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY	
	DEF_TEST(KOpenSession2, COpenSession2);
	DEF_TEST(KOpenSessionRemoved2, COpenSessionRemoved2);
	DEF_TEST(KOpenSessionRConnect2, COpenSessionRConnect2);
	DEF_TEST(KOpenSessionRtcp2, COpenSessionRtcp2);
	DEF_TEST(KOpenSessionRtcpRemoved2, COpenSessionRtcpRemoved2);
	DEF_TEST(KCloseSession2, CCloseSession2);
	DEF_TEST(KTidyUp2, CTidyUp2);
#endif

	if (testStep)
		testStep->SetServer(this);

	return testStep;
	}

void CTrtpServer::StopScheduler(CTrtpServer* aPtr, const TRtpEvent& aEvent)
	{
	aPtr->iLastEvent = aEvent;
	if (!aPtr->iStopping && !--aPtr->iStopCount)
		{
		CActiveScheduler::Stop();
		aPtr->iStopping = ETrue;
		}
	}

void CTrtpServer::NewSourceL(CTrtpServer* aPtr, const TRtpEvent& aEvent)
	{
	aPtr->iCurrentStep->iIgnoreIncrease = ETrue;
	RRtpReceiveSource r;
	r = aEvent.Session().NewReceiveSourceL();
	CleanupClosePushL(r);
	User::LeaveIfError(aPtr->iSources.Append(r));
	CleanupStack::PushL(TCleanupItem(RemoveLastSource, &aPtr->iSources));
	r.RegisterEventCallbackL(ERtpPacketReceived, CTrtpServer::GetPacketL, aPtr);
	r.RegisterEventCallbackL(ERtpAnyEvent, CTrtpServer::StopScheduler, aPtr);
	r.RegisterEventCallbackL(ERtpSourceFail, CTrtpServer::NotifyError, aPtr, 
							 ERtpOneShot, KErrNoMemory);
	r.RegisterEventCallbackL(ERtpCodecChange, CTrtpServer::NoteEventStatus, aPtr);
	CleanupStack::Pop(2);
	}

void CTrtpServer::GetPacketL(CTrtpServer* aPtr, const TRtpEvent& aEvent)
	{
	aPtr->iReceivePacket.Close();
	aPtr->iReceivePacket = aEvent.ReceiveSource().Packet();
	aPtr->iPacket = &aPtr->iReceivePacket;
	aPtr->iLastActiveSource = aEvent.ReceiveSource();
	if (aEvent.ReceiveSource().Packet().IsOpen())
		User::Leave(KErrGeneral);
	}

void CTrtpServer::Panic(CTrtpServer* /*aPtr*/, const TRtpEvent& /*aEvent*/)
	{
	// To check that irrelevant callbacks don't get called!
	User::Panic(_L("TRTP"), 1);
	}

void CTrtpServer::LeaveL(CTrtpServer* /*aPtr*/, const TRtpEvent& /*aEvent*/)
	{
	User::Leave(KErrTotalLossOfPrecision);
	}

void CTrtpServer::NewSourceLeaveL(CTrtpServer* aPtr, const TRtpEvent& aEvent)
	{
	RRtpReceiveSource r = aEvent.Session().NewReceiveSourceL();
	aPtr->iSources.Append(r);
	r.RegisterEventCallbackL(ERtpSourceFail, CTrtpServer::StopScheduler, aPtr, 
							 ERtpNotOneShot, KErrTotalLossOfPrecision);
	r.RegisterEventCallbackL(ERtpPacketReceived, CTrtpServer::LeaveL, aPtr);
	}

void CTrtpServer::NotifyError(CTrtpServer* aPtr, const TRtpEvent& aEvent)
	{
	aPtr->iCurrentStep->iError = aEvent.Status();
	if (!aPtr->iStopping && !--aPtr->iStopCount)
		{
		CActiveScheduler::Stop();
		aPtr->iStopping = ETrue;
		}
	}

void CTrtpServer::CloseSession(CTrtpServer* aPtr, const TRtpEvent& aEvent)
	{
	// If it's a receive stream event, close the stream first. We
	// should be allowed to do this.
	if (aEvent.IsReceiveSourceEvent())
		aEvent.ReceiveSource().Close();
	if (aEvent.IsSendSourceEvent())
		aEvent.SendSource().Close();

	aPtr->iSession.Close();
	if (!aPtr->iStopping && !--aPtr->iStopCount)
		{
		CActiveScheduler::Stop();
		aPtr->iStopping = ETrue;
		}	
	}

void CTrtpServer::NewSourceCloseSessionL(CTrtpServer* aPtr, const TRtpEvent& aEvent)
	{
	RRtpReceiveSource r = aEvent.Session().NewReceiveSourceL();
	aPtr->iSources.Append(r);
	r.RegisterEventCallbackL(ERtpPacketReceived, CTrtpServer::CloseSession,
							 aPtr);
	r.RegisterEventCallbackL(ERtpPacketReceived, CTrtpServer::Panic,
							 aPtr);
	}

void CTrtpServer::NewSourceDelayedCloseSessionL(CTrtpServer* aPtr, const TRtpEvent& aEvent)
	{
	RRtpReceiveSource r = aEvent.Session().NewReceiveSourceL();
	aPtr->iSources.Append(r);
	r.RegisterEventCallbackL(ERtpPacketReceived, CTrtpServer::CloseSession,
							 aPtr, ERtpNotOneShot, 2);
	r.RegisterEventCallbackL(ERtpPacketReceived, CTrtpServer::Panic,
							 aPtr, ERtpNotOneShot, 2);
	r.RegisterEventCallbackL(ERtpPacketReceived, CTrtpServer::StopScheduler,
							 aPtr);
	}

void CTrtpServer::NewSourceLeaveCloseL(CTrtpServer* aPtr, const TRtpEvent& aEvent)
	{
	RRtpReceiveSource r = aEvent.Session().NewReceiveSourceL();
	aPtr->iSources.Append(r);
	r.RegisterEventCallbackL(ERtpPacketReceived, CTrtpServer::LeaveL,
							 aPtr);
	r.RegisterEventCallbackL(ERtpPacketReceived, CTrtpServer::Panic,
							 aPtr);
	r.RegisterEventCallbackL(ERtpSourceFail, CTrtpServer::CloseSession,
							 aPtr);
	r.RegisterEventCallbackL(ERtpSourceFail, CTrtpServer::Panic,
							 aPtr);
	}

void CTrtpServer::LeaveAfterCloseL(CTrtpServer* aPtr, const TRtpEvent& /*aEvent*/)
	{
	aPtr->iSession.Close();
	if (!aPtr->iStopping && !--aPtr->iStopCount)
		CActiveScheduler::Stop();
	User::Leave(KErrTotalLossOfPrecision);
	}

void CTrtpServer::CloseSourceFromSourceL(CTrtpServer* aPtr, const TRtpEvent& aEvent)
	{
	RRtpReceiveSource r = aEvent.Session().NewReceiveSourceL();
	r.RegisterEventCallbackL(ERtpPacketReceived, CTrtpServer::CloseSource,
							 aPtr);
	}

void CTrtpServer::CloseSourceFromSourceLeaveL(CTrtpServer* aPtr, const TRtpEvent& aEvent)
	{
	RRtpReceiveSource r = aEvent.Session().NewReceiveSourceL();
	r.RegisterEventCallbackL(ERtpPacketReceived, CTrtpServer::LeaveL,
							 aPtr);
	r.RegisterEventCallbackL(ERtpSourceFail, CTrtpServer::CloseSource, aPtr);
	}


void CTrtpServer::CloseSource(CTrtpServer* aPtr, const TRtpEvent& aEvent)
	{
	aEvent.ReceiveSource().Close();
	if (!aPtr->iStopping && !--aPtr->iStopCount)
		{
		CActiveScheduler::Stop();
		aPtr->iStopping = ETrue;
		}
	}

void CTrtpServer::CloseSendSource(CTrtpServer* aPtr, const TRtpEvent& /*aEvent*/)
	{
	aPtr->iSendSource.Close();
	if (!aPtr->iStopping && !--aPtr->iStopCount)
		{
		CActiveScheduler::Stop();
		aPtr->iStopping = ETrue;
		}
	}


void CTrtpServer::NoteEventStatus(CTrtpServer* aPtr, const TRtpEvent& aEvent)
	{
	aPtr->iEventStatus = aEvent.Status();
	}

TUint16 CTrtpServer::Swap16(TUint16 aVal)
	{
	return static_cast<TUint16>(aVal >> 8 | (aVal & 0xff) << 8);
	}

TUint32 CTrtpServer::Swap32(TUint32 aVal)
	{
	return (Swap16(static_cast<TUint16>(aVal >> 16)) | 
			Swap16(static_cast<TUint16>(aVal & 0xffff)) << 16);
	}


/* for RTCP tests*/

void CTrtpServer::NewSrcForRtcpL(CTrtpServer* aPtr, const TRtpEvent& aEvent)
	{
	RRtpReceiveSource r = aEvent.Session().NewReceiveSourceL();
	aPtr->iSources.Append(r);

	r.RegisterEventCallbackL(ERtpPacketReceived, CTrtpServer::GetPacketL, aPtr);
	r.RegisterEventCallbackL(ERtpAnyEvent, CTrtpServer::StopScheduler, aPtr);
	// SDES
	r.RegisterEventCallbackL(ERtpSDES, CTrtpServer::CbERtpSDES, aPtr);
	// SR
	r.RegisterEventCallbackL(ERtpSR, CTrtpServer::CbERtpSR, aPtr);
	// App
	r.RegisterEventCallbackL(ERtpAPP, CTrtpServer::CbERtpAPP, aPtr);
	// Bye
	r.RegisterEventCallbackL(ERtpBYE, CTrtpServer::CbERtpBYE, aPtr);

	r.RegisterEventCallbackL(ERtpSourceFail, CTrtpServer::Panic, aPtr);
	}

void CTrtpServer::CbERtpSDES(CTrtpServer* aPtr, const TRtpEvent& aEvent)
	{
	aPtr->iCurrentStep->GotEvent(aEvent.Type());
	}

void CTrtpServer::CbERtpAPP(CTrtpServer* aPtr, const TRtpEvent& aEvent)
	{
	aPtr->iCurrentStep->GotEvent(aEvent.Type());
	}

void CTrtpServer::CbERtpSR(CTrtpServer* aPtr, const TRtpEvent& aEvent)
	{
	aPtr->iCurrentStep->GotEvent(aEvent.Type());
	}

void CTrtpServer::CbERtpRR(CTrtpServer* aPtr, const TRtpEvent& aEvent)
	{
	aPtr->iCurrentStep->GotEvent(aEvent.Type());
	}

void CTrtpServer::CbERtpBYE(CTrtpServer* aPtr, const TRtpEvent& aEvent)
	{
	aPtr->iCurrentStep->GotEvent(aEvent.Type());
	}

//
//
//
CWaiter* CWaiter::NewLC(CTrtpServer* aServer)
	{
	CWaiter* that = new (ELeave) CWaiter(aServer);
	CleanupStack::PushL(that);
	return that;
	}

void CWaiter::RunL()
	{
	if (!iServer->iStopping && !--iServer->iStopCount)
		{
		CActiveScheduler::Stop();
		iServer->iStopping = ETrue;
		}
	}

void CWaiter::DoCancel()
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrCancel);
	}


CWaiter::~CWaiter()
	{
	Cancel();
	}
	
	
