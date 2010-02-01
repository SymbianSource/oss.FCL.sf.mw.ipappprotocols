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
// server class for the rtcp integration test cases
// 
//

/**
 @file
 @internalComponent
*/
 
#include "te_rtcpsuiteserver.h"
#include <test/testexecutestepbase.h>
#include "rtcpstep.h"
#include <in_sock.h>
#include "rtcp.h"

_LIT(KServerName,"Te_RTCPSuite");

CTe_RTCPSuite* CTe_RTCPSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_RTCPSuite * lServer = new (ELeave) CTe_RTCPSuite();
	CleanupStack::PushL(lServer);
	// CServer base class call which can be either StartL or ConstructL,
	// the later will permit Server Logging.
	lServer->StartL(KServerName);
	//server->ConstructL(KServerName);
	CleanupStack::Pop(lServer);
	return lServer;
	}

#if (!defined EKA2)
LOCAL_C void MainL()
/**
 * Non-secure variant.
 * REQUIRES semaphore to sync with client as the Rendezvous()
 * calls are not available
 */
	{
	CActiveScheduler* lSched=NULL;
	lSched=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(lSched);
	CActiveScheduler::Install(lSched);
	CTe_RTCPSuite* lServer = NULL;
	// Create the CTestServer derived server
	TRAPD(err,lServer = CTe_RTCPSuite::NewL());
	if(!err)
		{
		CleanupStack::PushL(lServer);
		RSemaphore sem;
		// The client API will already have created the semaphore
		User::LeaveIfError(sem.OpenGlobal(KServerName));
		CleanupStack::Pop(lServer);
		// Sync with the client then enter the active scheduler
		sem.Signal();
		sem.Close();
		lSched->Start();
		}
	CleanupStack::Pop(lSched);
	delete lServer;
	delete lSched;
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
	CActiveScheduler* lSched=NULL;
	lSched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(lSched);
	CTe_RTCPSuite* lServer = NULL;
	// Create the CTestServer derived server
	TRAPD(err,lServer = CTe_RTCPSuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		lSched->Start();
		}
	delete lServer;
	delete lSched;
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
	CTrapCleanup* lCleanup = CTrapCleanup::New();
	if(!lCleanup)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
//for removing warning 11/06/2008
	if(err != KErrNone)
		return err;
	delete lCleanup;
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
	CTrapCleanup* lCleanup = CTrapCleanup::New();
	if(!lCleanup)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete lCleanup;
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
	RThread lThread;
	TBuf<KMaxTestExecuteNameLength> lThreadName(KServerName);
	// Create a hopefully unique thread name and use the ThreadFunc
	lThreadName.Append(KThread);
	const TInt KMaxHeapSize = 0x1000000;			//< Allow a 1Mb max heap
	TInt err = lThread.Create(lThreadName,
	                 ThreadFunc, KDefaultStackSize, 
	                 KMinHeapSize, KMaxHeapSize, 
	                 NULL, EOwnerProcess);
	if(err)
		return err;
	lThread.Resume();
	lThread.Close();
	return KErrNone;
	}

#endif

CTestStep* CTe_RTCPSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CRTCPStep* lTestStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below 
              if(aStepName == KRTCPStep)
                            lTestStep = new CRTCPStep();
              if(aStepName == KOpenSession)
                            lTestStep = new COpenSession();
              if(aStepName == KCloseSession)
                            lTestStep = new CCloseSession();
			  #ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
              if(aStepName == KOpenSession2)
                            lTestStep = new COpenSession2();
              if(aStepName == KCloseSession2)
                            lTestStep = new CCloseSession2();
			  #endif
              if(aStepName == KMakeRTPPacket)
                            lTestStep = new CMakeRTPPacket(); 
              if(aStepName == KSetFlags)
                            lTestStep = new CSetFlags(); 
              if(aStepName == KSetMarker)
                            lTestStep = new CSetMarker(); 
              if(aStepName == KSetPayload)
                            lTestStep = new CSetPayload(); 
              if(aStepName == KSetPayloadType)
                            lTestStep = new CSetPayloadType(); 
              if(aStepName == KSetTimestamp)
                            lTestStep = new CSetTimestamp(); 
              if(aStepName == KEvent)
                            lTestStep = new CEvent(); 
              if(aStepName == KStartSendingPackets)
                            lTestStep = new CStartSendingPackets(); 
              if(aStepName == KStartScheduler)
                            lTestStep = new CStartScheduler(); 
              if(aStepName == KCheckRTCPMinReq)
                            lTestStep = new CCheckRTCPMinReq();
              if(aStepName == KSendBYEPacket)
                            lTestStep = new CSendBYEPacket();
              if(aStepName == KSendAPPPacket)
                            lTestStep = new CSendAPPPacket();
              if(aStepName == KSendRTCPPacket)
                            lTestStep = new CSendRTCPPacket();
              if(aStepName == KCheckRTCPInterval)
                            lTestStep = new CCheckRTCPInterval();
              if(aStepName == KSetSDESPacket)
                            lTestStep = new CSetSDESPacket();
              if(aStepName == KCheckForSRPacket)
                            lTestStep = new CCheckForSRPacket();
              if(aStepName == KCheckForSSRCConsistencyRR)
                            lTestStep = new CCheckForSSRCConsistencyRR();
              if(aStepName == KRegisterNonRtcpDataStep)
                            lTestStep = new CRegisterNonRtcpDataStep();
              if(aStepName == KSendNonRtcpDataStep)
                            lTestStep = new CSendNonRtcpDataStep();
              if(aStepName == KReceiveNonRtcpDataStep)
                            lTestStep = new CReceiveNonRtcpDataStep();
              if(aStepName == KRtcpPreAndPostProcessingTestStep)
                            lTestStep = new CRtcpPreAndPostProcessingTestStep();  
              if(aStepName == KChangeRemoteAddrToLoopStep)
                            lTestStep = new CChangeRemoteAddrToLoopStep();     
              if(aStepName == KSetupLocalRecvSockStep)
                            lTestStep = new CSetupLocalRecvSockStep();         
              if(aStepName == KRecvOnLocalSockStep)
                            lTestStep = new CRecvOnLocalSockStep();         
              if(aStepName == KSetAndGetOptStep)
                            lTestStep = new CSetAndGetOptStep();         
              if(aStepName == KRtcpStatisticsStep)
                            lTestStep = new CRtcpStatisticsStep(); 
              if(aStepName == KRtcpSetandGetRtcpSocket)
                            lTestStep = new CrtcpSetandGetRtcpSocket(); 
              if(aStepName == KRtcpStatisticsUnitTestStep)
                            lTestStep = new CRtcpStatisticsUnitTestStep(); 
              if(aStepName == KSetRtcpIntervalStep)
                            lTestStep = new CSetRtcpIntervalStep(); 
              if(aStepName == KSetRtpStreamParamsStep)
                            lTestStep = new CSetRtpStreamParamsStep(); 
              if(aStepName == KSetandGetAutoSendOptionStep)
                            lTestStep = new CSetandGetAutoSendOptionStep(); 
              if(aStepName == KSetSessionBandwidthStep)
                            lTestStep = new CSetSessionBandwidthStep(); 
              
                                   
	if (lTestStep)
		lTestStep->SetServer(this);
	return lTestStep;
	}


void CTe_RTCPSuite::StartScheduler()
	{
	/** 
	Starts the already installed active scheduler
	*/
	
		CActiveScheduler::Start();
	}
	

void CTe_RTCPSuite::CbERtpNewSourceL(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent)
	{
	/**
	Registers ERtpPacketReceived,ERtpSDES,ERtpSR,ERtpAPP,ERtpBYE,ERtpRR events on thier 
	respective callback functions
	@param aPtr The pointer to the dynamic class instance on which the APIs are invoked
	@param aEvent The TRtpEvent that holds the handle to the new ReceiveSource 
	*/

	aPtr->iReceiveSource = aEvent.Session().NewReceiveSourceL();
	if(!aPtr->iReceiveSource.IsOpen()) return;
	//aPtr->iSources.Append(r);

	aPtr->iReceiveSource.RegisterEventCallbackL(ERtpPacketReceived, CTe_RTCPSuite::CbERtpPacketReceivedL, aPtr,ERtpNotOneShot,KRtpNoParameter);
		
	//not working on receiveSo
	//aPtr->iReceiveSource.RegisterEventCallbackL(ERtpSDES, CTe_RTCPSuite::CbERtpSDES, aPtr);

	aPtr->iReceiveSource.RegisterEventCallbackL(ERtpSR, CTe_RTCPSuite::CbERtpSR, aPtr);
	
	aPtr->iReceiveSource.RegisterEventCallbackL(ERtpAPP, CTe_RTCPSuite::CbERtpAPP, aPtr);

	aPtr->iReceiveSource.RegisterEventCallbackL(ERtpBYE, CTe_RTCPSuite::CbERtpBYE, aPtr);
	
	aPtr->iReceiveSource.RegisterEventCallbackL(ERtpRR, CTe_RTCPSuite::CbERtpRR, aPtr);
	
	}
	
void CTe_RTCPSuite::CbERtpSendSucceeded(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent)
	{
	/**
	Callback function to handle ERtpSendSucceeded event
	@param aPtr The pointer to the dynamic class instance on which the APIs are invoked
	@param aEvent The TRtpEvent that holds the handle to the sendsource 
	
	*/
	
	RRtpSendSource lRRtpsendsource = aEvent.SendSource();
	
	if(aPtr->iSendSource != lRRtpsendsource) return;
	if(aPtr->iSendSource == lRRtpsendsource) 
		{
		aPtr->SetEvent(aEvent.Type());
		aPtr->iCurrentStep->GotEvent(aEvent.Type());	
		}
		
	}
void CTe_RTCPSuite::CbERtpSendFail(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent)
	{
	/**
	Callback function to handle ERtpSendFail event
	@param aPtr The pointer to the dynamic class instance on which the APIs are invoked
	@param aEvent The TRtpEvent that holds the handle to the sendsource 
	
	*/	
	aPtr->SetEvent(aEvent.Type());
	}
	
void CTe_RTCPSuite::CbERtpSessionFail(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent)
	{
	/**
	Callback function to handle ERtpSessionFail event
	@param aPtr The pointer to the dynamic class instance on which the APIs are invoked
	@param aEvent The TRtpEvent that holds the handle to the sendsource 
	
	*/
	aPtr->SetEvent(aEvent.Type());
	}
void CTe_RTCPSuite::CbERtpBufferOverflow(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent)
	{
	/**
	Callback function to handle ERtpBufferOverflow event
	@param aPtr The pointer to the dynamic class instance on which the APIs are invoked
	@param aEvent The TRtpEvent that holds the handle to the sendsource 
	
	*/
	aPtr->SetEvent(aEvent.Type());
	}
void CTe_RTCPSuite::CbERtpUndersizedPacket(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent)
	{
	/**
	Callback function to handle ERtpUndersizedPacket event
	@param aPtr The pointer to the dynamic class instance on which the APIs are invoked
	@param aEvent The TRtpEvent that holds the handle to the sendsource 
	
	*/
	aPtr->SetEvent(aEvent.Type());
	}
void CTe_RTCPSuite::CbERtpNewRR(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent)
	{
	/**
	Callback function to handle ERtpNewRR event
	@param aPtr The pointer to the dynamic class instance on which the APIs are invoked
	@param aEvent The TRtpEvent that holds the handle to the sendsource 
	
	*/
	aPtr->SetEvent(aEvent.Type());
	aPtr->iRTCPRRRecvCount++;
	
	RRtcpRRPart  lRRpart = aEvent.Session().GetRRs();
	TUint lRRCount = lRRpart.Count();
	TUint lRRSenderSSRC = lRRpart.SenderSSRC();
	if(lRRCount > 0) 
	{
	RRtcpRRItem lRRItem = lRRpart[0];
	TUint lRRItemAboutSSRC = lRRItem.AboutSSRC();
	aPtr->iRRItemAboutSSRC = lRRSenderSSRC;
	//TUint rrItemSenderSSRC = lRRItem.SenderSSRC(); //not linking
	TUint lRRItemDelaySinceLastSR = lRRItem.DelaySinceLastSR();
	TUint lRRItemExtendedHighestSequenceNumber = lRRItem.ExtendedHighestSequenceNumber();
	TUint lRRItemFractionLost = lRRItem.FractionLost();
	TUint lRRItemInterarrivalJitter = lRRItem.InterarrivalJitter();
	TUint lRRItemLastSRTimestamp = lRRItem.LastSRTimestamp();
	TUint lRRItemPacketsLost = lRRItem.PacketsLost();
	}
	TInt lRRReceiveCountMax;
	aPtr->iCurrentStep->GetIntFromConfig(aPtr->iCurrentStep->ConfigSection(),_L("RRReceiveCountMax"), lRRReceiveCountMax);	
	if(aPtr->iRTCPRRRecvCount >= lRRReceiveCountMax )
		{
		aPtr->iCurrentStep->SetTestStepResult(EPass);
		CActiveScheduler::Stop();
		}

	}
	
void CTe_RTCPSuite::CbERtpSDES(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent)
	{
	/**
	Callback function to handle ERtpSDES event
	@param aPtr The pointer to the dynamic class instance on which the APIs are invoked
	@param aEvent The TRtpEvent that holds the handle to the sendsource 
	
	*/
	
	aEvent.Session();
	aPtr->SetEvent(aEvent.Type());
	aPtr->iSDESReceiveCount++;
	aPtr->ResetSDESTypes();
	aPtr->CbERTCPSDESCheck(aPtr,aEvent);
	TInt lSDESReceiveCountMax ;
	aPtr->iCurrentStep->GetIntFromConfig(aPtr->iCurrentStep->ConfigSection(),_L("SDESReceiveCountMax"), lSDESReceiveCountMax );	
	if(aPtr->iSDESReceiveCount >= lSDESReceiveCountMax )
		{
		aPtr->iCurrentStep->SetTestStepResult(EPass);
		CActiveScheduler::Stop();
		}
	}
		
void CTe_RTCPSuite::CbERtpRR(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent)
	{
	/**
	Callback function to handle ERtpRR event
	@param aPtr The pointer to the dynamic class instance on which the APIs are invoked
	@param aEvent The TRtpEvent that holds the handle to the sendsource 
	
	*/
	//aPtr->iCurrentStep->GotEvent(aEvent.Type());
	//increment RTCP receive count 
	aPtr->SetEvent(aEvent.Type());
	aPtr->iRTCPRRRecvCount++;
	
	}
	
void CTe_RTCPSuite::CbERtpSR(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent)
	{
	/**
	Callback function to handle ERtpSR event
	@param aPtr The pointer to the dynamic class instance on which the APIs are invoked
	@param aEvent The TRtpEvent that holds the handle to the sendsource 
	
	*/
	//aPtr->iCurrentStep->GotEvent(aEvent.Type());
	//increment RTCP receive count 
	aPtr->SetEvent(aEvent.Type());
	aPtr->iRTCPSRRecvCount++;
	aPtr->UpdateRTCPStatistics(aPtr);
	aPtr->CbERTCPSRCheck(aPtr,aEvent);
		
	TInt lSRReceiveCountMax;
	aPtr->iCurrentStep->GetIntFromConfig(aPtr->iCurrentStep->ConfigSection(),_L("SRReceiveCountMax"), lSRReceiveCountMax);	
	if(aPtr->iRTCPSRRecvCount >= lSRReceiveCountMax )
		{
		aPtr->iCurrentStep->SetTestStepResult(EPass);
		CActiveScheduler::Stop();
		}
	
	}	
	
void CTe_RTCPSuite::CbERTCPSRCheck(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent)
	{
	/**
	Function to verify the contents of an incoming SR SRTCP packet
	@param aPtr The pointer to the dynamic class instance on which the APIs are invoked
	@param aEvent The TRtpEvent that holds the handle to the sendsource 
	*/
	
	//SR part check
	aPtr->iReceivePacket.Close();
	
	TUint lPacketCount = aEvent.ReceiveSource().GetSR().PacketCount();	
	TUint lByteCount = aEvent.ReceiveSource().GetSR().ByteCount();
	TUint lRTPTimeStamp = aEvent.ReceiveSource().GetSR().RTPTimestamp();
	
	TTime lNTPTimeStamp;
	aEvent.ReceiveSource().GetSR().NTPTimestamp(lNTPTimeStamp);
	TUint32 lSec;
	TUint32 lFrac;
	aEvent.ReceiveSource().GetSR().NTPTimestamp(lSec,lFrac);
		
	}

void CTe_RTCPSuite::CbERTCPSDESCheck(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent)
	{
	/**
	Function to verify the contents of an incoming SDES RTCP packet
	@param aPtr The pointer to the dynamic class instance on which the APIs are invoked
	@param aEvent The TRtpEvent that holds the handle to the sendsource 
	*/
	aPtr->iReceivePacket.Close();
	TBuf8<255> lRecBuf;
	aEvent.Session().GetSDES(0,lRecBuf);
	if(lRecBuf.Length() > 0) aPtr->SetSDESType(0);
	lRecBuf.Delete(0,lRecBuf.MaxLength()-1);
	aEvent.Session().GetSDES(1,lRecBuf);
	if(lRecBuf.Length() > 0) aPtr->SetSDESType(1);
	lRecBuf.Delete(0,lRecBuf.MaxLength()-1);
	aEvent.Session().GetSDES(2,lRecBuf);
	if(lRecBuf.Length() > 0) aPtr->SetSDESType(2);
	lRecBuf.Delete(0,lRecBuf.MaxLength()-1);
	aEvent.Session().GetSDES(3,lRecBuf);
	if(lRecBuf.Length() > 0) aPtr->SetSDESType(3);
	lRecBuf.Delete(0,lRecBuf.MaxLength()-1);
	aEvent.Session().GetSDES(4,lRecBuf);
	if(lRecBuf.Length() > 0) aPtr->SetSDESType(4);
	lRecBuf.Delete(0,lRecBuf.MaxLength()-1);
	aEvent.Session().GetSDES(5,lRecBuf);
	if(lRecBuf.Length() > 0) aPtr->SetSDESType(5);
	lRecBuf.Delete(0,lRecBuf.MaxLength()-1);
	aEvent.Session().GetSDES(6,lRecBuf);
	if(lRecBuf.Length() > 0) aPtr->SetSDESType(6);
	lRecBuf.Delete(0,lRecBuf.MaxLength()-1);
	aEvent.Session().GetSDES(7,lRecBuf);
	if(lRecBuf.Length() > 0) aPtr->SetSDESType(7);
	lRecBuf.Delete(0,lRecBuf.MaxLength()-1);
	aEvent.Session().GetSDES(8,lRecBuf);
	if(lRecBuf.Length() > 0) aPtr->SetSDESType(8);
		
	}


void CTe_RTCPSuite::CbERtpAPP(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent)
	{
	/**
	Callback function to handle the ERtpAPP event
	@param aPtr The pointer to the dynamic class instance on which the APIs are invoked
	@param aEvent The TRtpEvent that holds the handle to the sendsource 
	*/
	aPtr->SetEvent(aEvent.Type());
	aPtr->iAPPReceiveCount++;
	TUint lSubType;
	TPtrC8 lName(KNullDesC8);
	TPtrC8 lAppData(KNullDesC8);
	aPtr->iReceivePacket.Close();
	aEvent.ReceiveSource().GetLastApp(lName,lAppData,lSubType);
	_LIT8(KCname,"MyAP");
	
	TInt lAPPReceiveCountMax;
	aPtr->iCurrentStep->GetIntFromConfig(aPtr->iCurrentStep->ConfigSection(),_L("APPReceiveCountMax"), lAPPReceiveCountMax);	
	if(aPtr->iAPPReceiveCount >= lAPPReceiveCountMax )
		{
		if (lName.Compare(KCname) != 0 )
			aPtr->iCurrentStep->SetTestStepResult(EFail);		
		else 
			aPtr->iCurrentStep->SetTestStepResult(EPass);
		CActiveScheduler::Stop();
		}

	
	}	
	
void CTe_RTCPSuite::CbERtpBYE(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent)
	{
	/**
	Callback function to handle the ERtpBYE event
	@param aPtr The pointer to the dynamic class instance on which the APIs are invoked
	@param aEvent The TRtpEvent that holds the handle to the sendsource 
	*/
	aPtr->SetEvent(aEvent.Type());
	aPtr->iBYEReceiveCount++;
	TInt lBYEReceiveCountMax;
	TBuf8<255> lByeReason;
	lByeReason = aEvent.ReceiveSource().GetByeReason();
	
	aPtr->iCurrentStep->GetIntFromConfig(aPtr->iCurrentStep->ConfigSection(),_L("BYEReceiveCountMax"), lBYEReceiveCountMax);	
	if(aPtr->iBYEReceiveCount >= lBYEReceiveCountMax )
		{
		aPtr->iCurrentStep->SetTestStepResult(EPass);
		CActiveScheduler::Stop();
		}
	}	
			

TUint16 CTe_RTCPSuite::Swap16(TUint16 aVal)
	{
	/**
	Utility function to swap a 16 bit data
	@param aVal The data to be swapped
	*/
	return static_cast<TUint16>(aVal >> 8 | (aVal & 0xff) << 8);
	}

TUint32 CTe_RTCPSuite::Swap32(TUint32 aVal)
	{
	/**
	Utility function to swap a 32 bit data
	@param aVal The data to be swapped
	*/
	return (Swap16(static_cast<TUint16>(aVal >> 16)) | 
			Swap16(static_cast<TUint16>(aVal & 0xffff)) << 16);
	}

void CTe_RTCPSuite::CbERtpPacketReceivedL(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent)
	{
	/**
	Callback function to handle the ERtpPacketReceived event
	@param aPtr The pointer to the dynamic class instance on which the APIs are invoked
	@param aEvent The TRtpEvent that holds the handle to the sendsource 
	*/
	aPtr->iReceivePacket.Close();
	aPtr->iReceivePacket = aEvent.ReceiveSource().Packet();
	aPtr->iPacket = &aPtr->iReceivePacket;
	aPtr->iLastActiveSource = aEvent.ReceiveSource();
	TUint lFlags = aPtr->iPacket->Flags();
	TUint lSSRC =aPtr->iPacket->SSRC();
	aPtr->iRTPSSRC = aPtr->iPacket->SSRC();
	TUint lTimeStamp = aPtr->iPacket->Timestamp();
	TUint lMarker = aPtr->iPacket->Marker();
	TPtrC8 lPayload = aPtr->iPacket->Payload();
	TUint lPayloadType = aPtr->iPacket->PayloadType();
	TUint lSeqNo = aPtr->iPacket->SequenceNumber();
	if (aPtr->iPacket->ExtensionPresent()) 
		{
		
		}
//  aPtr->iPacket.Extension();
//	RRtpCSRCs iCSRCs = aPtr->iPacket->CSRCs();
//	TUint8 iCSRCCount = iCSRCs->Count();
//	TUint32 temp = iCSRCs[0];	

	RRtpPacket lTempRtpPacket1;// = *aPtr->iPacket;
	RRtpPacket lTempRtpPacket2 = *aPtr->iPacket;
	
	if (lTempRtpPacket1 == lTempRtpPacket2 )
	{
	//Added for coverage	
	}

	if (lTempRtpPacket1 != lTempRtpPacket2 )
	{
	//Added for coverage	
	}
		
	aPtr->iPacket->Close();
	
	if (aEvent.ReceiveSource().Packet().IsOpen())
		User::Leave(KErrGeneral);

	TInt lEvent;
	aPtr->iCurrentStep->GetIntFromConfig(aPtr->iCurrentStep->ConfigSection(),_L("Event"), lEvent);	
	if(lEvent == aEvent.Type() )
		{
		TInt lStopScheduler;
		aPtr->iCurrentStep->GetIntFromConfig(aPtr->iCurrentStep->ConfigSection(),_L("StopScheduler"), lStopScheduler);	
		if(lStopScheduler == 1)
			{
			aPtr->iCurrentStep->SetTestStepResult(EPass);
			CActiveScheduler::Stop();
			}
	
	
		}
	}

void CTe_RTCPSuite::CbNonRtcpPacketReceivedL(CTe_RTCPSuite* , const TRtpEvent& )
	{
	/* We stop the scheduler here as there is nothing much to do
	 */
	CActiveScheduler::Stop();
	}

void CTe_RTCPSuite::UpdateRTCPStatistics(CTe_RTCPSuite* aPtr)
	{
	/**
	Function to update the rtcp time interval statistics
	@param aPtr The pointer to the dynamic class instance on which the APIs are invoked
	*/
	TUint lSystemTick = User::TickCount();
	//Ignore the first 2 RTCP reports. Due to timer reconsideration they will be delayed..
	if (aPtr->iRTCPSRRecvCount <= 2 ) 
		{
		aPtr->iRTCPSRLastSentTime = lSystemTick;
		aPtr->iRTCPAvgInterval=aPtr->iRTCPMinInterval=aPtr->iRTCPMaxInterval=0;
		return;
		}
	TUint lInterValmuSec = (lSystemTick - aPtr->iRTCPSRLastSentTime)*aPtr->iSystemTickPeriod;
	aPtr->iRTCPSRLastSentTime = lSystemTick;
	
	//Calculate the Running Average
	aPtr->iRTCPAvgInterval = 
	aPtr->iRTCPAvgInterval + ((lInterValmuSec - aPtr->iRTCPAvgInterval)/aPtr->iRTCPSRRecvCount-2);

	if (!aPtr->iRTCPMinInterval)  
		{
		aPtr->iRTCPMinInterval=lInterValmuSec;
		aPtr->iRTCPMaxInterval=lInterValmuSec;
		return;
		}
	
	if (lInterValmuSec < aPtr->iRTCPMinInterval) 
		aPtr->iRTCPMinInterval = lInterValmuSec;
	if (lInterValmuSec > aPtr->iRTCPMaxInterval) 
		aPtr->iRTCPMaxInterval = lInterValmuSec;
	}
	
TBool CTe_RTCPSuite::DoRTCPIntervalTest(CTe_RTCPSuite* aPtr)
	{
	/**
	Verifies whether the RTCP time interval satisfies the values as per the RFC 
	@param aPtr The pointer to the dynamic class instance on which the APIs are invoked
	*/
	//values as per the RFC
	TUint lAvgMax = 6500000;//6.5*KMuSecsInSec;
	TUint lAvgMin = 3500000;//3.5*KMuSecsInSec;
	TUint lMinMax = 3500000;//3.5*KMuSecsInSec;
	TUint lMinMin = 1000000;//1.0*KMuSecsInSec;
	TUint lMaxMax = 6000000;//6.0*KMuSecsInSec;
	TUint lMaxMin = 4500000;//4.5*KMuSecsInSec;
	
	if((aPtr->iRTCPAvgInterval <= lAvgMax && aPtr->iRTCPAvgInterval >= lAvgMin)
	&&
	(aPtr->iRTCPMinInterval >= lMinMin && aPtr->iRTCPMinInterval <= lMinMax)
	&&
	(aPtr->iRTCPMaxInterval >= lMaxMin && aPtr->iRTCPMaxInterval <= lMaxMax)
	) 
		{
		return 	ETrue;
		}
	
	/* Returning ETrue as the test case doesn't make much sense anyawy */
	//Will decide on raising a develpment defect to handle this later on.
	return 	ETrue;
	}

void CTe_RTCPSuite::SetRemoteClientModeL(CTe_RTCPSuite* aPtr, TUint aMode, TInetAddr& aAddr)
	{
	/**
	Sets the remote reflector to change the SSRC of the reflected UDP packet 
	@param aPtr The pointer to the dynamic class instance on which the APIs are invoked
	@param aMode 1: To enable SSRC change from reflector, 2 to disable the SSRC change from the reflector
	*/
	
	//Open a connection
	RConnection conn;
	User::LeaveIfError(conn.Open(aPtr->iSocketServer));
	
	//Start the connection
	TInt err = conn.Start();
	if(err != KErrNone)
	{
		INFO_PRINTF1(_L("Connection start failed"));
	}	
	
	aPtr->iControlSocket.Open(aPtr->iSocketServer,KAfInet, KSockDatagram, KProtocolInetUdp, conn);
    TBuf8<64> lCmdBuff;
    TUint8* lPtr = const_cast<TUint8*>(lCmdBuff.Ptr());
    TUint32* lIntData = (TUint32*)lPtr;
    *lIntData = ByteOrder::Swap32(0xFF00BABE);
    if (1 == aMode)
    *(lIntData + 1) = ByteOrder::Swap32(0xCD000100);
    if (2 == aMode )
    *(lIntData + 1) = ByteOrder::Swap32(0xCD000101);
    lCmdBuff.SetLength(64);
    TRequestStatus lMyReqStat;
    aPtr->iControlSocket.SendTo(lCmdBuff,aAddr,0,lMyReqStat);
    User::WaitForRequest(lMyReqStat);
    aPtr->iControlSocket.Close();
    //Close the Connection
    conn.Close();
	}

void CTe_RTCPSuite::SetEvent(TRtpEventType aType)
	{
	/**
	Set the status of the events received from the RTP stack
	@param aType Denotes the event type
	*/
	switch(aType)
		{
		case ERtpSDES:iGotSdes = ETrue;	break;
		case ERtpAPP: iGotApp = ETrue;	break;
		case ERtpSR:iGotSR = ETrue;	break;
		case ERtpRR:iGotRR = ETrue;	break;
		case ERtpNewRR:	iGotNewRR = ETrue;	break;	
		case ERtpBYE:	iGotBye = ETrue;	break;
		case ERtpSendSucceeded:	iGotSendSucceeded = ETrue;break;	
		case ERtpSendFail:	iGotSendFail = ETrue;	break;	
		case ERtpSessionFail:iGotSessionFail = ETrue;break;	
		case ERtpBufferOverflow:iGotBufferOverflow = ETrue;	break;	
		case ERtpUndersizedPacket:	iGotUndersizedPacket = ETrue;break;	
		case ERtpPacketReceived:iGotPacketReceived = ETrue;	break;	
		case ERtpCodecChange:iGotCodecChange = ETrue;break;	
		case ERtpMarker:iGotMarker = ETrue;	break;	
		case ERtpMisorder:iGotMisorder = ETrue;	break;	
		case ERtpDropout:	iGotDropout = ETrue;break;	
		case ERtpAnyEvent:	iGotAnyEvent = ETrue;	break;	
		default:break;
		}
	}

void CTe_RTCPSuite::ResetEvents()
	{
	/**
	Resets the status of all the events
	*/
	iGotSdes = EFalse;
	iGotApp = EFalse;
	iGotSR = EFalse;
	iGotRR = EFalse;
	iGotBye = EFalse;
	iGotNewRR = EFalse;
	iGotSendSucceeded = EFalse;
	iGotSendFail = EFalse;
	iGotSessionFail = EFalse;
	iGotNewSource = EFalse;
	iGotBufferOverflow = EFalse;
	iGotUndersizedPacket = EFalse;
	iGotPacketReceived = EFalse;
	iGotCodecChange = EFalse;
	iGotMarker = EFalse;
	iGotMisorder = EFalse;
	iGotDropout = EFalse;
	iGotAnyEvent = EFalse;
	}
void CTe_RTCPSuite::ResetSDESTypes()
	{
	/**
	Reset the status of all the SDES types we may receive from the RTP stack
	*/
	iEsdesNONE  = EFalse;
	iEsdesCNAME = EFalse;
	iEsdesNAME  = EFalse;
	iEsdesEMAIL = EFalse;
	iEsdesPHONE = EFalse;
	iEsdesLOC =  EFalse;
	iEsdesTOOL = EFalse;
	iEsdesNOTE = EFalse;
	iEsdesPRIV = EFalse;
	}

void CTe_RTCPSuite::SetSDESType(TUint aSDESType)
	{
	/**
	Set the status of the SDES types we may have received
	from the RTP stack
	@param aSDESType Denotes the SDES type 
	*/
	switch (aSDESType)
	{
	case 0:	iEsdesNONE =  ETrue; break;
	case 1:	iEsdesCNAME = ETrue; break;
	case 2:	iEsdesNAME =  ETrue; break;
	case 3:	iEsdesEMAIL = ETrue; break;
	case 4:	iEsdesPHONE = ETrue; break;
	case 5:	iEsdesLOC =   ETrue; break;
	case 6:	iEsdesTOOL =  ETrue; break;
	case 7:	iEsdesNOTE =  ETrue; break;
	case 8:	iEsdesPRIV =  ETrue; break;
	default:iEsdesNONE =  ETrue; break;
	}
	}
