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
// @file  TS_Te_Cap_SipServerServer.cpp
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

// EPOC includes
#include <e32base.h>
#include <e32cmn.h>

#if (!defined TS_Te_Cap_SipServer_SERVER_H_)
	#include "CSuite.h"
#endif


//ADD TEST STEP HEADERS HERE
#include "ESipItcTerminateDialog1_CStep.h"
#include "ESipItcTerminateRefresh1_CStep.h"
#include "ESipItcTerminateDialogRefresh1_CStep.h"
#include "ESipItcTerminateRegistration1_CStep.h"
#include "ESipItcSupportedSecurityMechanisms1_CStep.h"
#include "ESipItcNegotiatedSecurityMechanism1_CStep.h"
#include "ESipItcIsSigCompSupported1_CStep.h"
#include "ESipItcGetTransactionState1_CStep.h"
#include "ESipItcGetDialogState1_CStep.h"
#include "ESipItcGetCompartmentCount1_CStep.h"
#include "ESipItcGetFromHeader1_CStep.h"
#include "ESipItcGetRefreshInterval1_CStep.h"
#include "ESipItcGetCallIDHeader1_CStep.h"
#include "ESipItcGetLocalTag1_CStep.h"
#include "ESipItcOutboundProxy1_CStep.h"
#include "ESipItcGetRegisteredContact1_CStep.h"
#include "ESipItcGetLocalAddr1_CStep.h"
#include "ESipItcSetOutboundProxy1_CStep.h"
#include "ESipItcRemoveOutboundProxy1_CStep.h"
#include "ESipItcSetRefreshInterval1_CStep.h"
#include "ESipItcSetHttpDigestObserver1_CStep.h"
#include "ESipItcSetCredentials1_CStep.h"
#include "ESipItcRemoveCredentials1_CStep.h"
#include "ESipItcIgnoreChallenge1_CStep.h"
#include "ESipItcSetSecurityHandling1_CStep.h"
#include "ESipItcResetDialogState1_CStep.h"
#include "ESipItcSetAppUid1_CStep.h"
#include "ESipItcOpenSubSession1_CStep.h"
#include "ESipItcCloseSubSession1_CStep.h"
#include "ESipItcRegister1_CStep.h"
#include "ESipItcRegisterAndSetOutboundProxy1_CStep.h"
#include "ESipItcUpdateRegistration1_CStep.h"
#include "ESipItcUnRegister1_CStep.h"
#include "ESipItcFetchRegistrations1_CStep.h"
#include "ESipItcSendRequest1_CStep.h"
#include "ESipItcSendRequestAndUpdateRefresh1_CStep.h"
#include "ESipItcSendRequestAndTerminateRefresh1_CStep.h"
#include "ESipItcSendResponse1_CStep.h"
#include "ESipItcSendCancel1_CStep.h"
#include "ESipItcSendResponseAndCreateDialog1_CStep.h"
#include "ESipItcSendResponseInDialog1_CStep.h"
#include "ESipItcSendRequestAndCreateDialog1_CStep.h"
#include "ESipItcSendRequestInDialog1_CStep.h"
#include "SpItcSendRequestInDialogAndUpdateRefresh1_CStep.h"
#include "ESipItcClientReadyToReceive1_CStep.h"
#include "ESipItcClientReceiveSipMessage1_CStep.h"
#include "ESipItcClientCancelReceive1_CStep.h"
#include "ESipItcConnectionReadyToReceive1_CStep.h"
#include "ESipItcConnectionReceiveSipMessage1_CStep.h"
#include "ESipItcConnectionCancelReceive1_CStep.h"

_LIT(KSipServerStarterMutex, "SipServerStarterMutex");
_LIT(KSipServerSemaphoreName, "SipServerSemaphore");
_LIT(KSipServerName,"SipServer");
const TUid KServerUid3 = {0x101f5d36};
#define RETURN_IF_ERROR(err) {TInt _err=err; if(_err!=KErrNone) {return _err;}}


// __EDIT_ME__ - Substitute the name of your test server
_LIT(KServerName,"Cap_Te_Cap_SipServer_sc");
// __EDIT_ME__ - Use your own server class name

CTestTe_Cap_SipServerServer* CTestTe_Cap_SipServerServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
// __EDIT_ME__ - Use your own server class name
	CTestTe_Cap_SipServerServer * server = new (ELeave) CTestTe_Cap_SipServerServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

CTestStep* CTestTe_Cap_SipServerServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	 CTestStep* testStep = NULL;
	// add test steps
		if (aStepName == _L("CESipItcTerminateDialog1Step"))
			{
			testStep =  new(ELeave) CESipItcTerminateDialog1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcTerminateRefresh1Step"))
			{
			testStep =  new(ELeave) CESipItcTerminateRefresh1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcTerminateDialogRefresh1Step"))
			{
			testStep =  new(ELeave) CESipItcTerminateDialogRefresh1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcTerminateRegistration1Step"))
			{
			testStep =  new(ELeave) CESipItcTerminateRegistration1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcSupportedSecurityMechanisms1Step"))
			{
			testStep =  new(ELeave) CESipItcSupportedSecurityMechanisms1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcNegotiatedSecurityMechanism1Step"))
			{
			testStep =  new(ELeave) CESipItcNegotiatedSecurityMechanism1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcIsSigCompSupported1Step"))
			{
			testStep =  new(ELeave) CESipItcIsSigCompSupported1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcGetTransactionState1Step"))
			{
			testStep =  new(ELeave) CESipItcGetTransactionState1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcGetDialogState1Step"))
			{
			testStep =  new(ELeave) CESipItcGetDialogState1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcGetCompartmentCount1Step"))
			{
			testStep =  new(ELeave) CESipItcGetCompartmentCount1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcGetFromHeader1Step"))
			{
			testStep =  new(ELeave) CESipItcGetFromHeader1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcGetRefreshInterval1Step"))
			{
			testStep =  new(ELeave) CESipItcGetRefreshInterval1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcGetCallIDHeader1Step"))
			{
			testStep =  new(ELeave) CESipItcGetCallIDHeader1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcGetLocalTag1Step"))
			{
			testStep =  new(ELeave) CESipItcGetLocalTag1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcOutboundProxy1Step"))
			{
			testStep =  new(ELeave) CESipItcOutboundProxy1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcGetRegisteredContact1Step"))
			{
			testStep =  new(ELeave) CESipItcGetRegisteredContact1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcGetLocalAddr1Step"))
			{
			testStep =  new(ELeave) CESipItcGetLocalAddr1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcSetOutboundProxy1Step"))
			{
			testStep =  new(ELeave) CESipItcSetOutboundProxy1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcRemoveOutboundProxy1Step"))
			{
			testStep =  new(ELeave) CESipItcRemoveOutboundProxy1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcSetRefreshInterval1Step"))
			{
			testStep =  new(ELeave) CESipItcSetRefreshInterval1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcSetHttpDigestObserver1Step"))
			{
			testStep =  new(ELeave) CESipItcSetHttpDigestObserver1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcSetCredentials1Step"))
			{
			testStep =  new(ELeave) CESipItcSetCredentials1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcRemoveCredentials1Step"))
			{
			testStep =  new(ELeave) CESipItcRemoveCredentials1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcIgnoreChallenge1Step"))
			{
			testStep =  new(ELeave) CESipItcIgnoreChallenge1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcSetSecurityHandling1Step"))
			{
			testStep =  new(ELeave) CESipItcSetSecurityHandling1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcResetDialogState1Step"))
			{
			testStep =  new(ELeave) CESipItcResetDialogState1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcSetAppUid1Step"))
			{
			testStep =  new(ELeave) CESipItcSetAppUid1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcOpenSubSession1Step"))
			{
			testStep =  new(ELeave) CESipItcOpenSubSession1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcCloseSubSession1Step"))
			{
			testStep =  new(ELeave) CESipItcCloseSubSession1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcRegister1Step"))
			{
			testStep =  new(ELeave) CESipItcRegister1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcRegisterAndSetOutboundProxy1Step"))
			{
			testStep =  new(ELeave) CESipItcRegisterAndSetOutboundProxy1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcUpdateRegistration1Step"))
			{
			testStep =  new(ELeave) CESipItcUpdateRegistration1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcUnRegister1Step"))
			{
			testStep =  new(ELeave) CESipItcUnRegister1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcFetchRegistrations1Step"))
			{
			testStep =  new(ELeave) CESipItcFetchRegistrations1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcSendRequest1Step"))
			{
			testStep =  new(ELeave) CESipItcSendRequest1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcSendRequestAndUpdateRefresh1Step"))
			{
			testStep =  new(ELeave) CESipItcSendRequestAndUpdateRefresh1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcSendRequestAndTerminateRefresh1Step"))
			{
			testStep =  new(ELeave) CESipItcSendRequestAndTerminateRefresh1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcSendResponse1Step"))
			{
			testStep =  new(ELeave) CESipItcSendResponse1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcSendCancel1Step"))
			{
			testStep =  new(ELeave) CESipItcSendCancel1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcSendResponseAndCreateDialog1Step"))
			{
			testStep =  new(ELeave) CESipItcSendResponseAndCreateDialog1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcSendResponseInDialog1Step"))
			{
			testStep =  new(ELeave) CESipItcSendResponseInDialog1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcSendRequestAndCreateDialog1Step"))
			{
			testStep =  new(ELeave) CESipItcSendRequestAndCreateDialog1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcSendRequestInDialog1Step"))
			{
			testStep =  new(ELeave) CESipItcSendRequestInDialog1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CSpItcSendRequestInDialogAndUpdateRefresh1Step"))
			{
			testStep =  new(ELeave) CSpItcSendRequestInDialogAndUpdateRefresh1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcClientReadyToReceive1Step"))
			{
			testStep =  new(ELeave) CESipItcClientReadyToReceive1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcClientReceiveSipMessage1Step"))
			{
			testStep =  new(ELeave) CESipItcClientReceiveSipMessage1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcClientCancelReceive1Step"))
			{
			testStep =  new(ELeave) CESipItcClientCancelReceive1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcConnectionReadyToReceive1Step"))
			{
			testStep =  new(ELeave) CESipItcConnectionReadyToReceive1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcConnectionReceiveSipMessage1Step"))
			{
			testStep =  new(ELeave) CESipItcConnectionReceiveSipMessage1Step  ;
			return testStep;
			 }
		if (aStepName == _L("CESipItcConnectionCancelReceive1Step"))
			{
			testStep =  new(ELeave) CESipItcConnectionCancelReceive1Step  ;
			return testStep;
			 }
		return testStep;
	 }



// Secure variants much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
// __EDIT_ME__ - Use your own server class name
	CTestTe_Cap_SipServerServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTestTe_Cap_SipServerServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}


GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }


TVerdict CCapabilityTestStep::doTestStepPreambleL( void )
	{
	//If Preamble is not required just pass a success value
	TVerdict testResult = CTestStep::doTestStepPreambleL();


	return TestStepResult();

	}
TVerdict CCapabilityTestStep::doTestStepPostambleL( void )
	{
	//If Postamble is not required just pass a success value
	TVerdict testResult = CTestStep::doTestStepPostambleL();

	return TestStepResult();

	}


// Moved from CStep.cpp

enum TVerdict CCapabilityTestStep::doTestStepL()
	{
	//DEF! INFO_PRINTF2(_L("%S - Starting ..."), &iTestStepName);

	//The MainThread()creates a separate thread that executes SendReceive
	TVerdict vResult = MainThread();

	SetTestStepResult(vResult);
	return TestStepResult();
	}

/*
ThreadStartFn:
Called by:	The Child thread
Function:	Calls the Exec_SendReceive
*/
static TInt ThreadStartFn( TAny * ptr )
	{
	return(((CCapabilityTestStep *)ptr)->Exec_SendReceive());
	}

/*
TVerdict GetVerdict(TInt aAPIretValue)

Called by: "MainThread" for returning verdict

Parameters(TInt aRetValue) :	0 if API call gets thru without any rejection
								1 if API call is rejected for capability error
*/
enum TVerdict CCapabilityTestStep::GetVerdict(TInt aAPIretValue)
	{
	TVerdict vVerdict[] = {EPass, EFail};

	//please leave the following if/else block as the information printed by INFO_PRINTF1 is used bu CapTestSumm
	if(iExpect_Rejection)//[Inverse Test] EPass for 1 while EFail for 0
		{
		INFO_PRINTF1(_L("Test Expected to Fail due to lack of capabilities"));
		return vVerdict[(aAPIretValue)?0:1];

		}
	else //[Direct Test] EPass for 0 while EFail for 1
		{
		INFO_PRINTF1(_L("Test Expected to Pass with correct capabilities"));
		return vVerdict[(aAPIretValue)?1:0];
		}
	}


/*
TVerdict MainThread()

Called by: "doTestStepL"

Purpose:	Creates the child thread(which calls the respective function with regard
			to the server and also implements the Message Call). Then this fn.waits for the
			completion of the childthread( doesnt matter how the thread did die!)

Return Value(Verdict of the TestStep):

			A.Reporting PASS/FAIL
				Direct Test:
						When a message call gets thru. Please note that in such cases
						actually the implementation of the message has started. As we
						are passing "0" Parameters, server may panic, though our botheration
						stops once the call gets thru.
						NOTE:	The style is the same when CONNECTION capabilities
								are tested, the only diff is you dont have to expect a
								panic from server
				Inverse Test:
						The call should be either failed or panicked with
						"KErrPermissionDenied" flag.

				General Case:
						If a thread creation failed or if the server couldnt be connected
						apart from the above two cases, then a FAIL is reported

			B.Reporting INCONCLUSIVE
						Any panic say from unexpected source (eg:KERN-EXEC) will be
						reported INCONCLUSIVE
*/
TVerdict CCapabilityTestStep::MainThread()
	{

	TBuf<100>	tExitCategory;
	TInt		tExitReason = 0;
	TBuf<100>	TestStyle;

	iExpect_Rejection?TestStyle = _L("Inverse"):TestStyle = _L("Direct");
	TCapabilitySet theCaps =  TSecurityInfo(RProcess()).iCaps ;
	const TInt KMaxTestThreadHeapSize = 0x10000;

	//Initialize return values
	iResult_SR = iResult_Server = KErrNone;


	// Create a child thread, with a new heap
	TInt nRes_Thread =	tChildThread.Create(
						ChildThread_SR,
						ThreadStartFn,
						KDefaultStackSize,
						KMinHeapSize,
						KMaxTestThreadHeapSize,
						this,
						EOwnerProcess);


	if(nRes_Thread == KErrNone)//Thread Created
		{

		//Let me know when the thread is dead
		TRequestStatus ThreadStatus;
		tChildThread.Logon(ThreadStatus);
		tChildThread.Resume();
		//Is the thread dead?
		User::WaitForRequest( ThreadStatus );

		//yes, he is dead. RIP!  Now the Killer's profile
		tExitCategory	=	tChildThread.ExitCategory();
		tExitReason		=	tChildThread.ExitReason();


		//Somebody Please say what are we testing!!
		if(iSessionCreated && (SR_MESSAGE_ID >=0))//Flag set by Child thread when connected to Server
		{
			//DEF INFO_PRINTF5(_L("Connected to Server(%S) for %S Test [MessageID: %d,Req.Cap: 0x%x,Present.Cap: 0x%x]"),&SR_ServerName,&TestStyle,SR_MESSAGE_ID,iStepCap,TSecurityInfo(RProcess()));
		}
		else if(SR_MESSAGE_ID < 0)
		{
			//DEF INFO_PRINTF5(_L("Testing Connection capabilities[%S Test] for Server(%S)  [Req.Cap: 0x%x,Present.Cap: 0x%x]"),&TestStyle,
			//&SR_ServerName,TSecurityInfo(RProcess()));
		}
		else if(!iSessionCreated)// NO Connection
			{
			INFO_PRINTF4(_L("Couldnt connect to the Server(%S) ErrorCode - ServerRet: %d C32ret: %d"),&SR_ServerName,iResult_Server,iResult_C32);
			//INFO_PRINTF3(_L("Child Thread: ExitCategory : %S ExitReason : %d"),&tExitCategory,tExitReason);
  			return EFail;
 			}



		switch(tChildThread.ExitType())
			{
			case EExitPanic:
				//1.A Panic from the connected Server
				//2.A CServer Panic normally for capability rejection
				//3.A kernel Panic (consider yourself doomed!)
				if((tExitReason == KErrPermissionDenied) ||
					//DEF ? 	it's old version (tExitReason == CServer::EClientDoesntHaveRequiredCaps))//Rejected for Insufficient Cap.
					// is it correct ?
					(tExitReason == CServer2::EClientDoesntHaveRequiredCaps))//Rejected for Insufficient Cap.
					{
					INFO_PRINTF2(_L("Rejected for insufficient capabilities [Return Value : %d] "),tExitReason);
					return(GetVerdict(API_RetValue_PermissionDenied));
					}
				else if(tExitCategory == iServer_Panic) //Panic from Server
					{
					INFO_PRINTF2(_L("Server(%S) Panic to child thread"),&tExitCategory);
					INFO_PRINTF3(_L("Child Thread: ExitCategory : %S ExitReason : %d"),&tExitCategory,tExitReason);
					return(GetVerdict(API_RetValue_ServerPanic));
					}
				else//A kernel Panic possibly
					{
					INFO_PRINTF3(_L("Child Thread: Panic from unexpected source (ExitCategory: %S ExitReason : %d)!"),&tExitCategory,tExitReason);
					return EInconclusive;
					}

					//break;
			case EExitKill:
				if(iResult_SR != KErrPermissionDenied)
					{
					INFO_PRINTF2(_L("A Successfull call (Return Value : %d)"),((SR_MESSAGE_ID >=0)?iResult_SR:iResult_Server));
					return(GetVerdict(API_RetValue_NoCapError));
					}
				else
					{
					INFO_PRINTF2(_L("Rejected for insufficient capabilities [Return Value : %d] "),((SR_MESSAGE_ID >=0)?iResult_SR:iResult_Server));
					return(GetVerdict(API_RetValue_PermissionDenied));
					}

					//break;
			default:
					break;
			}
		}
	else //Our thread couldnt start	:o(
		{
		INFO_PRINTF2(_L("ERROR: Failed to create Child thread,  ErrorCode:(%d)"),nRes_Thread);
		return EFail;
		}

	return EInconclusive;
	}

/*
TInt CCapabilityTestStep::StartServer()
{
	TInt err =  KErrNone ;
	// EKA2 is simple No path required
	TBuf<32> serverFile;
	serverFile.Copy(_L("Te_Cap_SipServer"));
	_LIT(KExe,".exe");
	serverFile.Append(KExe);
	RProcess server;
	err = server.Create(serverFile,_L(""));
	if(err != KErrNone)
		return err;
	// Synchronise with the server
	TRequestStatus reqStatus;
	server.Rendezvous(reqStatus);
	server.Resume();
	 //Server will call the reciprocal static synchronise call
	User::WaitForRequest(reqStatus);
	//server.Close();
	if(reqStatus.Int() != KErrNone)
		return reqStatus.Int();
	return err;
}
*/

TInt CCapabilityTestStep::StartServer()
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

TInt CCapabilityTestStep::CreateServerProcess (RSemaphore& aSemaphore)
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

 	TInt CCapabilityTestStep::TestDebugHeap(TInt* iDbgIPCNo)
 	{

 	//TDbgFns  {MarkHeapStart, MarkHeapEnd, CheckHeap, FailNext, ResetFailNext};
 	TInt aFnToTest= iDbgIPCNo[5];


 	TInt iResult_SR [6] ={0};
 	TInt i = 1;
 	TInt testedFn = 0;

 	TInt dbgTestSequence[5][6]	=	{	{MarkHeapStart	,2,0,1,-1,-1},
 										{MarkHeapEnd	,2,0,1,-1,-1},
 										{CheckHeap		,3,0,2, 1,-1},
 										{FailNext		,4,0,3, 4, 1},
 										{ResetFailNext	,4,0,3, 4, 1}

 									};


 	TInt aCount = dbgTestSequence[aFnToTest][i];

 	while(aCount--  )
 		{
 		testedFn =  dbgTestSequence[aFnToTest][(++i)];

			iResult_SR[testedFn ]= SendReceive( iDbgIPCNo[testedFn],TIpcArgs(((iDbgIPCNo[testedFn]==3 )?4:0),0,0,0));


 		if( ((testedFn !=aFnToTest)?iResult_SR[testedFn]:KErrNone) == KErrPermissionDenied)

			User::Panic(_L("Failed at Initialization"),iResult_SR[testedFn]);

		}

	return iResult_SR[aFnToTest];
	}
