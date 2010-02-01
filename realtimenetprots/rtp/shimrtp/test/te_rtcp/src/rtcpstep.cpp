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
// class for the rtcp integration test cases
// 
//

/**
 @file
 @internalComponent
*/
 
#include "rtcpstep.h"
#include <in_sock.h>
#include <commdbconnpref.h>
#include <hal.h>
#include <utf.h>
#include <e32base.h>
#include <e32cons.h>
#include <f32file.h>
#include <es_sock.h>
#include <nifman.h>
#include <cs_subconparams.h>

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
#include "rtp_subconparams.h"
#endif


CRTCPStep::~CRTCPStep()
 	{
 	}

CRTCPStep::CRTCPStep()
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KRTCPStep);
	}

TVerdict CRTCPStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
    iServer->iCurrentStep = this;
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CRTCPStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
 	{
    SetTestStepResult(EPass);	
    return TestStepResult();
 	}

TVerdict CRTCPStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
	
TVerdict COpenSession::doTestStepPreambleL()
	{
	/**
	Installs the active scheduler, Initialize the file system
	Configures the remote Test Instrument
	*/
	iServer->iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iServer->iScheduler);
	User::LeaveIfError(iServer->iSocketServer.Connect());	
	User::LeaveIfError(iServer->iFs.Connect());
	
	//reset all the events
	iServer->ResetEvents();		
	//reset all the count values
	iServer->iRTCPAvgInterval=0;
	iServer->iRTCPMinInterval=0;
	iServer->iRTCPMaxInterval=0;
	iServer->iRTCPSRRecvCount=0;
	iServer->iAPPReceiveCount=0;
	iServer->iBYEReceiveCount=0;
	iServer->iRTCPRRRecvCount=0;
	iServer->iSDESReceiveCount=0;
	iServer->iRTPSendCount=0;
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict COpenSession::doTestStepL()
{
	/**
	Reads the session configuration from the ini file
	Opens the session using the RTPShim interface without using the subconnection flow
	*/
	TRequestStatus lStatus;
	TCommDbConnPref lPrefs;
	SetTestStepResult(EFail);
	TInetAddr lLocalAddress;
	TInetAddr lRemoteAddress;
	TInetAddr lReflectorAddress;
	TInt lReflectorControlPort;
	TInt lReflectorMode;
	TInt lSourceRTPPort;
	TInt lDestinationRTPPort;
	TPtrC lDestinationIP(0,0);
	TPtrC lCnameData(0,0);
	TInt lLoopBack;
	TInt lMaxRXSize;
	GetStringFromConfig(ConfigSection(),_L("CNAME"), lCnameData);
	GetIntFromConfig(ConfigSection(),_L("SourceRTPPort"), lSourceRTPPort);
	GetIntFromConfig(ConfigSection(),_L("DestinationRTPPort"), lDestinationRTPPort);
	GetStringFromConfig(ConfigSection(),_L("DestinationIP"), lDestinationIP);
	GetIntFromConfig(ConfigSection(),_L("LoopBack"), lLoopBack);
	GetIntFromConfig(ConfigSection(),_L("MaxRXSize"), lMaxRXSize);	
	GetIntFromConfig(ConfigSection(),_L("ReflectorMode"), lReflectorMode);	
	GetIntFromConfig(ConfigSection(),_L("ReflectorControlPort"), lReflectorControlPort);
	
	TBuf8<100> lCnameBuf;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(lCnameBuf,lCnameData);
	lRemoteAddress.SetPort(lDestinationRTPPort);
	lRemoteAddress.Input(lDestinationIP);
	lReflectorAddress.SetPort(lReflectorControlPort);
	lReflectorAddress.Input(lDestinationIP);
	lLocalAddress.SetPort(lSourceRTPPort);
	lLocalAddress.SetAddress(KInetAddrAny);
	lPrefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	
	//set the relector to change or not change the SSRC 
	iServer->SetRemoteClientModeL(iServer,lReflectorMode,lReflectorAddress);

	
	User::LeaveIfError(iServer->iConnect.Open(iServer->iSocketServer, KConnectionTypeDefault));
	iServer->iConnect.Start(lPrefs, lStatus);
	User::WaitForRequest(lStatus);
	if(lStatus.Int() != KErrNone)
		{
		return TestStepResult();
		}
	
	if (!iServer->iConnected)
		{
		
		TInt lVersionValue;
		GetIntFromConfig(ConfigSection(),_L("Version"), lVersionValue);	
		if (1 == lVersionValue)
			{
			TInetAddr	lLocalRtcpAddress;
			TInetAddr	lRemoteRtcpAddress;
			TInt	lSourceRTCPPort;
			TInt	lDestinationRTCPPort;
			GetIntFromConfig(ConfigSection(),_L("SourceRTCPPort"), lSourceRTCPPort);
			GetIntFromConfig(ConfigSection(),_L("DestinationRTCPPort"), lDestinationRTCPPort);
			lLocalRtcpAddress.SetPort(lSourceRTCPPort);
			lRemoteRtcpAddress.Input(lDestinationIP);
			lRemoteRtcpAddress.SetPort(lDestinationRTCPPort);
			
			if(1 == lLoopBack)
				{
				lLocalAddress.SetAddress(KInetAddrLoop);
				lLocalRtcpAddress.SetAddress(KInetAddrLoop);
				}
			else 
				{
				lLocalAddress.SetAddress(KInetAddrAny);
				lLocalRtcpAddress.SetAddress(KInetAddrAny);
				}
				
			User::LeaveIfError(iServer->iRtpSocket.Open(iServer->iSocketServer, 
										   lLocalAddress.Family(),
										   KSockDatagram, KProtocolInetUdp));
			User::LeaveIfError(iServer->iRtpSocket.Bind(lLocalAddress));
			iServer->iRtpSocket.Connect(lRemoteAddress, lStatus);
			User::WaitForRequest(lStatus);

			User::LeaveIfError(iServer->iRtcpSocket.Open(iServer->iSocketServer, 
										   lLocalRtcpAddress.Family(),
										   KSockDatagram, KProtocolInetUdp));
			User::LeaveIfError(iServer->iRtcpSocket.Bind(lLocalRtcpAddress));
			iServer->iRtcpSocket.Connect(lRemoteRtcpAddress, lStatus);
			User::WaitForRequest(lStatus);
				
			iServer->iSession.OpenL(iServer->iRtpSocket,lMaxRXSize, iServer->iRtcpSocket,EPriorityNormal, lCnameBuf);
			}
		
		if (2 == lVersionValue)
			{
			//ONLY RTP enabled
			if(1 == lLoopBack)
				{
				lLocalAddress.SetAddress(KInetAddrLoop);
				}
			else 
				{
				lLocalAddress.SetAddress(KInetAddrAny);
				}
		
			User::LeaveIfError(iServer->iRtpSocket.Open(iServer->iSocketServer,lLocalAddress.Family(),
										   KSockDatagram, KProtocolInetUdp));
			User::LeaveIfError(iServer->iRtpSocket.Bind(lLocalAddress));
			iServer->iRtpSocket.Connect(lRemoteAddress, lStatus);
			User::WaitForRequest(lStatus);
		
			iServer->iSession.OpenL(iServer->iRtpSocket,lMaxRXSize, EPriorityNormal);
			}
		
		if (3 == lVersionValue)
				{
			
			iServer->iSession.OpenL(iServer->iSocketServer, lLocalAddress, lRemoteAddress, 
								lMaxRXSize, iServer->iConnect, EPriorityNormal, lCnameBuf);
				}
		if (4 == lVersionValue)	
				{
			
			//with default RConnection
			iServer->iSession.OpenL(iServer->iSocketServer, lLocalAddress, lRemoteAddress, 
								lMaxRXSize, EPriorityNormal, lCnameBuf);
				}
		}
			
		//Set the RRtpSession parameters 
		TInt lBandwidth;
		TInt lRTPTimeNow;
		TInt lRTPTimeConversion;
		TInt lPayloadType;
		TInt lRTCPAutoSend;
		TInt lRTCPEnabled;
		TInt lRtcpInterval;
		TInt lMinSequential;
		TInt lMaxMisorder;
		TInt lMaxDropout;
		TInt lDontReceive;
		GetIntFromConfig(ConfigSection(),_L("MaxRXSize"), lMaxRXSize);	
		GetIntFromConfig(ConfigSection(),_L("Bandwidth"), lBandwidth);	
		GetIntFromConfig(ConfigSection(),_L("RTPTimeNow"), lRTPTimeNow);	
		GetIntFromConfig(ConfigSection(),_L("RTPTimeConversion"), lRTPTimeConversion);	
		GetIntFromConfig(ConfigSection(),_L("RtcpInterval"), lRtcpInterval);	
		GetIntFromConfig(ConfigSection(),_L("MinSequential"), lMinSequential);	
		GetIntFromConfig(ConfigSection(),_L("MaxMisorder"), lMaxMisorder);	
		GetIntFromConfig(ConfigSection(),_L("MaxDropout"), lMaxDropout);
		GetIntFromConfig(ConfigSection(),_L("RTCPAutoSend"), lRTCPAutoSend);
		GetIntFromConfig(ConfigSection(),_L("RTCPEnabled"), lRTCPEnabled);	
		GetIntFromConfig(ConfigSection(),_L("DontReceive"), lDontReceive);	
		GetIntFromConfig(ConfigSection(),_L("PayloadType"), lPayloadType);		
	   	
	   	if(lDontReceive == 0) //RTP
	   		{
	   		iServer->iSession.DontReceive(0);
	   		}
	   	if(lDontReceive == 1) //RTCP 
	   		{
	   		iServer->iSession.DontReceive(1);
	   		}
	   	if(lRTCPEnabled == 1)
	   		{
	   		if(lRTCPAutoSend == 1)
	   			{
	   		   	iServer->iSession.SetRTCPAutoSend(ETrue);	
		   		if(!iServer->iSession.RTCPAutoSend())
		   			{
		   			return TestStepResult(); 	   	
		   			}
	   			}
			else
				{				
				iServer->iSession.SetRTCPAutoSend(EFalse);	
				if(iServer->iSession.RTCPAutoSend())	
					{
					return TestStepResult(); 	   	
					}
	   			}	
				
			TTimeIntervalMicroSeconds32 lInterval(lRtcpInterval);
			iServer->iSession.SetRtcpInterval(lInterval);
	   	
	   		}
	   	
		iServer->iSession.SetRTPTimeConversion(lRTPTimeNow, lRTPTimeConversion);
		iServer->iSession.SetRtpStreamParameters(lMinSequential,lMaxMisorder,lMaxDropout);
		iServer->iSession.SetBandwidth(lBandwidth);
		iServer->iSession.SetMaxRXSize(lMaxRXSize);
		TUint lRXSize = iServer->iSession.MaxRXSize();
			
		CleanupClosePushL(iServer->iSession);
		iServer->iSession.NewSendSourceL();
		CleanupStack::Pop();
		iServer->iSendSource = iServer->iSession.SendSource();
		iServer->iSendSource.SetDefaultPayloadSize(lMaxRXSize);
		iServer->iSendSource.SetPayloadType(lPayloadType);
		
		SetTestStepResult(EPass);
		return TestStepResult();
}


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
TVerdict COpenSession2::doTestStepPreambleL()
	{
	/**
	Installs the active scheduler, Initialize the file system
	Configures the remote Test Instrument
	*/
	INFO_PRINTF1(_L("COpenSession2 preamble"));
	iServer->iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iServer->iScheduler);
	User::LeaveIfError(iServer->iSocketServer.Connect());	
	User::LeaveIfError(iServer->iFs.Connect());
	
	//reset all the events
	iServer->ResetEvents();		
	//reset all the count values
	iServer->iRTCPAvgInterval=0;
	iServer->iRTCPMinInterval=0;
	iServer->iRTCPMaxInterval=0;
	iServer->iRTCPSRRecvCount=0;
	iServer->iAPPReceiveCount=0;
	iServer->iBYEReceiveCount=0;
	iServer->iRTCPRRRecvCount=0;
	iServer->iSDESReceiveCount=0;
	iServer->iRTPSendCount=0;
	
	SetTestStepResult(EPass);
	return TestStepResult();
	
	}

TVerdict COpenSession2::doTestStepL()
	{
	/**
	Reads the session configuration from the ini file
	Opens the session using the using the subconnection flow
	*/
	SetTestStepResult(EFail);
	TRequestStatus lStatus;
	TInetAddr lLocalAddress;
	TInetAddr lRemoteAddress;
	TInetAddr lRemoteRtcpAddress;
	TInetAddr lReflectorAddress;
	TInt lReflectorControlPort;
	TInt lReflectorMode;
	TInt lSourceRTPPort;
	TInt lDestinationRTPPort;
	TPtrC lDestinationIP(0,0);
	TPtrC lCnameData(0,0);
	TInt lLoopBack;
	TInt lBandwidth;
	TInt lRTPTimeNow;
	TInt lRTPTimeConversion;
	TInt lRTCPAutoSend;
	TInt lRtcpInterval;
	TInt lMinSequential;
	TInt lMaxMisorder;
	TInt lMaxDropout;
	TInt lMaxRXSize;
	
	
	GetStringFromConfig(ConfigSection(),_L("CNAME"), lCnameData);
	GetIntFromConfig(ConfigSection(),_L("SourceRTPPort"), lSourceRTPPort);
	GetIntFromConfig(ConfigSection(),_L("DestinationRTPPort"), lDestinationRTPPort);
	GetStringFromConfig(ConfigSection(),_L("DestinationIP"), lDestinationIP);
	GetIntFromConfig(ConfigSection(),_L("LoopBack"), lLoopBack);
	GetIntFromConfig(ConfigSection(),_L("MaxRXSize"), lMaxRXSize);	
	GetIntFromConfig(ConfigSection(),_L("Bandwidth"), lBandwidth);	
	GetIntFromConfig(ConfigSection(),_L("RTPTimeNow"), lRTPTimeNow);	
	GetIntFromConfig(ConfigSection(),_L("RTPTimeConversion"), lRTPTimeConversion);	
	GetIntFromConfig(ConfigSection(),_L("RtcpInterval"), lRtcpInterval);	
	GetIntFromConfig(ConfigSection(),_L("MinSequential"), lMinSequential);	
	GetIntFromConfig(ConfigSection(),_L("MaxMisorder"), lMaxMisorder);	
	GetIntFromConfig(ConfigSection(),_L("MaxDropout"), lMaxDropout);
	GetIntFromConfig(ConfigSection(),_L("RTCPAutoSend"), lRTCPAutoSend);
	GetIntFromConfig(ConfigSection(),_L("ReflectorMode"), lReflectorMode);	
	GetIntFromConfig(ConfigSection(),_L("ReflectorControlPort"), lReflectorControlPort);
		
	TBuf8<100> lCnameBuf;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(lCnameBuf,lCnameData);
	lRemoteAddress.SetPort(lDestinationRTPPort);
	lRemoteAddress.Input(lDestinationIP);
	lLocalAddress.SetPort(lSourceRTPPort);
	lLocalAddress.SetAddress(KInetAddrAny);
	lReflectorAddress.SetPort(lReflectorControlPort);
	lReflectorAddress.Input(lDestinationIP);
	lRemoteRtcpAddress.SetPort(lDestinationRTPPort);
	lRemoteRtcpAddress.Input(lDestinationIP);
	
	iServer->SetRemoteClientModeL(iServer,lReflectorMode,lReflectorAddress);

	User::LeaveIfError(iServer->iConnect.Open(iServer->iSocketServer));
	iServer->iConnect.Start(lStatus);
	User::WaitForRequest(lStatus);
	if(lStatus.Int() != KErrNone)
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	// Create the container for all sub connection parameters
	RSubConParameterBundle lSubconParams;
	CleanupClosePushL(lSubconParams);
 
 	// Create a container for RTP sub connection parameters (Param bundle takes ownership)
	CSubConParameterFamily* lSubConFamily = CSubConParameterFamily::NewL(lSubconParams, KProtocolExtensionFamily);

 	// Create the requested generic parameter set for RTP (RTP family takes ownership)
	CSubConRTPGenericParamSet* lReqGenericParams = CSubConRTPGenericParamSet::NewL(*lSubConFamily, CSubConParameterFamily::ERequested);
	
	TUid lidRTP = TUid::Uid(KRtpFlowImplementationUid);
	// Set the requested Generic Parameters
	lReqGenericParams->SetProtocolUIDRtp(lidRTP);
	lReqGenericParams->SetBandWidth(lBandwidth);
	lReqGenericParams->SetRtptimeConversion(lRTPTimeConversion);
	lReqGenericParams->SetRtpAutoSend(lRTCPAutoSend);
	lReqGenericParams->SetRtpTimeNow(lRTPTimeNow);
	lReqGenericParams->SetCNAMEL(lCnameBuf);
	lReqGenericParams->SetMaxDropOut(lMaxDropout);
	lReqGenericParams->SetMinSequencial(lMinSequential);
	lReqGenericParams->SetMaxMisorder(lMaxMisorder);
	lReqGenericParams->SetDefDestinationAddr(lRemoteRtcpAddress);
	lReqGenericParams->SetRtcpReportInterval(lRtcpInterval);
	lReqGenericParams->SetServiceProtocolID(KProtocolInetUdp);
	lReqGenericParams->SetLowerSubConnType(RSubConnection::EAttachToDefault);
	
	TUid lidTier = TUid::Uid(2048);
	lReqGenericParams->SetTierID(lidTier);
	User::LeaveIfError(iServer->iSubConnect.Open(iServer->iSocketServer,RSubConnection::ECreateNew,iServer->iConnect));
	
	// Set Properties of the sub-connection
	User::LeaveIfError(iServer->iSubConnect.SetParameters(lSubconParams));
	// Destroy parameters
	CleanupStack::PopAndDestroy();         // subconParams
	
	TInt lVersion;
	GetIntFromConfig(ConfigSection(),_L("Version"), lVersion);	

	if (2 == lVersion)//Uses TSockAddr, without RTCP
		{
		
		iServer->iSession.OpenL(iServer->iSocketServer, 
				lLocalAddress, lRemoteAddress,lMaxRXSize,iServer->iSubConnect,
									EPriorityNormal,KNullDesC8);
		}	
	if (3 == lVersion)//Uses TSockAddr,with RTCP
		{

		iServer->iSession.OpenL(iServer->iSocketServer, 
		lLocalAddress, lRemoteAddress,lMaxRXSize,iServer->iSubConnect,
									EPriorityNormal,lCnameBuf);
		}
	
	CleanupClosePushL(iServer->iSession);
	iServer->iSession.NewSendSourceL();
	CleanupStack::Pop();
	iServer->iSendSource = iServer->iSession.SendSource();
	iServer->iSendSource.SetDefaultPayloadSize(lMaxRXSize);
    SetTestStepResult(EPass);
    return TestStepResult();
	
	}
	
TVerdict COpenSession2::doTestStepPostambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
#endif

TVerdict CMakeRTPPacket::doTestStepL()
	{
	/**
	Creates an empty RTP packet of a specific size
	*/
	TInt lBufSize;
	GetIntFromConfig(ConfigSection(),_L("BufferSize"),lBufSize);
	// Check that RRtpSendPacket objects are created closed
	if (iServer->iSendPacket.IsOpen())
		{
		SetTestStepResult(EFail);
		}
	
	TPtrC lConfig(0,0);
	TBool lExtension;
	TInt  lType = 0;
	TInt  lLen = KRtpNoExtension;
	TLex  lLex(lConfig);

	lExtension = GetStringFromConfig(ConfigSection(), _L("HeaderExtension"),lConfig);

	if (lExtension)
		{		
		lLex.Val(lType);
		if (!lLex.Eos() && lLex.Peek() == ',')
			lLex.Get();
		
		lLex.Val(lLen);
		if (!lLex.Eos() && lLex.Peek() == ',')
			lLex.Get();
		}
	
	iServer->iSendPacket = iServer->iSendSource.NewSendPacketL(lBufSize, lLen);
	iServer->iPacket = &iServer->iSendPacket;
	
	if (lExtension)
		{
		// Just create the extension now; its existence is checked in
		// CExtension
		RRtpHeaderExtension extension = iServer->iSendPacket.Extension();
		extension.SetType((TUint16)lType);
		TInt* array = const_cast<TInt*>(reinterpret_cast<const TInt*>(
											extension.Data().Ptr()));

		TInt i = 0;
		while (!lLex.Eos() && i < lLen)
			{
			TInt val;
			lLex.Val(val);
			array[i] = val;
			if (!lLex.Eos() && lLex.Peek() == ',')
				lLex.Get();
			i++;
			}
		}	
	
	SetTestStepResult(EPass);
	if (!iServer->iPacket->IsOpen())
		{
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CStartSendingPackets::doTestStepPreambleL()
	{
	/**
	Postamble 
	Initializes the timer to the current system clock time
	*/
	iServer->iCurrentStep = this;
	SetTestStepResult(EPass);
	HAL::Get(HALData::ESystemTickPeriod, iServer->iSystemTickPeriod);
	/* Initialize the Periodic timer for Sending RTP Packets */
	iPeriodic = CPeriodic::NewL(CActive::EPriorityStandard);
	return TestStepResult();
	}

TVerdict CStartSendingPackets::doTestStepL()
	{
	/**
	Send the first packet and start the scheduler once 
	*/
	
	SetTestStepResult(EFail);
	
	if (iServer->iSendSource.IsSending())
		{	
		return TestStepResult();
		}
	if (iServer->iSendSource.IsOpen())
		{
		iServer->iSendPacket.Send();
		}
	iServer->iRTPSendCount = 0;
	
	/* Start the periodic timer */
	iPeriodic->Start(200000,200000,TCallBack(TimerCallback,this));
	CActiveScheduler::Start();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


void CStartSendingPackets::GotEvent(TRtpEventType aType)
	{
	INFO_PRINTF2(_L("CStartSendingPackets::GotEvent %u"),aType);
	/**
	Continues sending packets on receiving the SendSucceeded event 
	Stops the scheduler on reaching the maximum send count limit
	@param aType The TrtpEventType, but not used 
	*/
	if(ERtpSendSucceeded == aType)
		{
		iServer->iRTPSendCount++;
		}
	/* Ignore write failures and send again */
	iSendComplete = ETrue;
	
	TInt lRTPSendCountMax;
	GetIntFromConfig(ConfigSection(),_L("RTPSendCountMax"),lRTPSendCountMax);
	INFO_PRINTF2(_L("CStartSendingPackets::GotEvent lRTPSendCountMax %d"),lRTPSendCountMax);
	if(lRTPSendCountMax && iServer->iRTPSendCount >= lRTPSendCountMax) 
		{
		CActiveScheduler::Stop();
		/* cancel periodic timer timeouts */
		iPeriodic->Cancel();
		iSendComplete = EFalse;
		SetTestStepResult(EPass);
		return;
		}
	}

TInt CStartSendingPackets::TimerCallback(TAny* aPtr)
	{
	CStartSendingPackets *pThis = reinterpret_cast<CStartSendingPackets*>(aPtr);
	if(pThis->iSendComplete)
		{
		pThis->iServer->iSendPacket.Send();
		pThis->iSendComplete = EFalse;
		}
	return KErrNone;
	}

TVerdict CStartSendingPackets::doTestStepPostambleL()
	{
	delete iPeriodic;
	iPeriodic = 0;
	return TestStepResult();
	}


TVerdict CCheckForSRPacket::doTestStepL()
	{
	/**
	Checks if we have already recieved an RTCP SR packet
	*/
	if(iServer->iGotSR )
		{
		SetTestStepResult(EPass);	
		}
	else 
		{
		SetTestStepResult(EFail);	
		}
	return TestStepResult();
	}  


TVerdict CCheckForSSRCConsistencyRR::doTestStepL()
	{
	/**
	Checks if the SSRC for the RTP and RTCP RR packet are same
	*/	
	if(iServer->iRTPSSRC ==  iServer->iRRItemAboutSSRC)
		{
		SetTestStepResult(EPass);	
		}
	else 
		{
		SetTestStepResult(EFail);	
		}
		
	SetTestStepResult(EPass);	
	return TestStepResult();
	} 
  
TVerdict CSetPayloadType::doTestStepL()
	{
	/**
	Sets the payload type of the RTP packet to be sent. 
	The type is read from the ini file
	*/
	SetTestStepResult(EPass);
	TInt lPayloadType;
	GetIntFromConfig(ConfigSection(),_L("PayloadType"),lPayloadType);
	iServer->iSendPacket.SetPayloadType(lPayloadType);
	return TestStepResult();
	}

TVerdict CSetTimestamp::doTestStepL()
	{
	/**
	Sets the time stamp  of the RTP packet to be sent. 
	The timestamp value is read from the ini file
	*/
	SetTestStepResult(EPass);
	TInt lTimestamp;
	GetIntFromConfig(ConfigSection(),_L("Timestamp"),lTimestamp);
	iServer->iSendPacket.SetTimestamp(lTimestamp);
	return TestStepResult();
	}
	
TVerdict CSetPayload::doTestStepL()
	{
	/**
	Sets the payload of the RTP packet to be sent. 
	The payload data location, size, data is read from the ini file
	*/
	SetTestStepResult(EPass);
	TPtrC lFileName(0,0);
	GetStringFromConfig(ConfigSection(),_L("PayloadFile"), lFileName);
	TInt lBytesToRead;
	GetIntFromConfig(ConfigSection(),_L("BytesToRead"), lBytesToRead);
	TInt lBufferSize;
	GetIntFromConfig(ConfigSection(),_L("BufferSize"), lBufferSize);
	User::LeaveIfError(iServer->iFile.Open(iServer->iFs,
							lFileName,EFileRead));
	TInt lFileSize;
	User::LeaveIfError(iServer->iFile.Size(lFileSize));
	//iServer->iSendPacket = iServer->iSendSource.NewSendPacketL(lBufferSize);
	HBufC8* lPacket = HBufC8::NewMaxLC(lBufferSize);
	TPtr8 lPtrPacket(lPacket->Des());
	User::LeaveIfError(iServer->iFile.Read(lPtrPacket));
	iServer->iSendPacket.WritePayload().Copy(*lPacket);
	CleanupStack::PopAndDestroy(lPacket);
	iServer->iFile.Close();
	return TestStepResult();
	}
	
TVerdict CSetMarker::doTestStepL()
	{
	/**
	Sets the marker bit of the RTP packet to be sent. 
	The value is read from the ini file
	*/
	SetTestStepResult(EPass);
	TInt lMarker;
	GetIntFromConfig(ConfigSection(),_L("Marker"),lMarker);
	iServer->iSendPacket.SetMarker(lMarker);
	return TestStepResult();
	}

TVerdict CSetFlags::doTestStepL()
	{
	/**
	Sets the payload flags of the RTP packet to be sent. 
	The value is read from the ini file
	*/
	SetTestStepResult(EPass);
	TInt lRawFlags;
	GetIntFromConfig(ConfigSection(),_L("Flags"),lRawFlags);
	TUint32 lFlags = CTe_RTCPSuite::Swap32(static_cast<TUint32>(lRawFlags));
	iServer->iSendPacket.SetFlags(lFlags);

	return TestStepResult();
	}
	
TVerdict CStartScheduler::doTestStepL()
	{
	/**
	Stasts the currently installed active scheduler
	*/
	iServer->StartScheduler();
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CEvent::doTestStepL()
	{
	/**
	Registers the event to be monitored for, from the RTP stack
	Two types of events 1) Session level and 2) Send source level
	are registered here	.
	*/
	SetTestStepResult(EPass);
	TInt lEvent;
	GetIntFromConfig(ConfigSection(),_L("Event"),lEvent);
	TInt lParam = KRtpNoParameter;
	GetIntFromConfig(ConfigSection(),_L("Param"),lParam);
	TBool lBoolOneShot = EFalse;
	TInt  lIntOneShot = 0;
	GetIntFromConfig(ConfigSection(),_L("OneShot"),lIntOneShot);
	if(lIntOneShot == 1) lBoolOneShot = ETrue;
	TBool lBoolSendSource = EFalse;
	TInt  lIntSendSource = 0;
	GetIntFromConfig(ConfigSection(),_L("SendSource"),lIntSendSource);
	if(lIntSendSource == 1) lBoolSendSource = ETrue;
	TPtrC lFunction(0,0);
	GetStringFromConfig(ConfigSection(),_L("Function"),lFunction);
	
	void (*fn)(CTe_RTCPSuite*, const TRtpEvent&) = 0;
	//session level
	if (lFunction == _L("ERtpSessionFail"))
		fn = CTe_RTCPSuite::CbERtpSessionFail;
	if (lFunction == _L("ERtpBufferOverflow"))
		fn = CTe_RTCPSuite::CbERtpBufferOverflow;
	if (lFunction == _L("ERtpUndersizedPacket"))
		fn = CTe_RTCPSuite::CbERtpUndersizedPacket;
	if (lFunction == _L("ERtpNewRR"))   
		fn = CTe_RTCPSuite::CbERtpNewRR;
	if (lFunction == _L("ERtpNewSource"))
		fn = CTe_RTCPSuite::CbERtpNewSourceL;
	if (lFunction == _L("ERtpSDES"))
		fn = CTe_RTCPSuite::CbERtpSDES;//working on session level, when the stack is a sender
	if (lFunction == _L("ERtpBYE"))
		fn = CTe_RTCPSuite::CbERtpBYE;//working on session level, when the stack is a sender

	//sendsource level
	if (lFunction == _L("ERtpSendSucceeded"))
		fn = CTe_RTCPSuite::CbERtpSendSucceeded;
	if (lFunction == _L("ERtpSendFail"))
		fn = CTe_RTCPSuite::CbERtpSendFail;

	if (!lBoolSendSource)
		{
		if(lParam == KRtpNoParameter)
			{
			//registering session based call backs 
			iServer->iSession.RegisterEventCallbackL(static_cast<TRtpEventType>(lEvent),
												 fn, iServer, 
												 lBoolOneShot? ERtpOneShot:ERtpNotOneShot,lParam);
			}
			else 
			{
			//registering session based call backs 
			iServer->iSession.RegisterEventCallbackL(static_cast<TRtpEventType>(lEvent),
													 fn, iServer, 
													 lBoolOneShot? ERtpOneShot:ERtpNotOneShot);
			}
												
		}
	else
		{
		if(lParam == KRtpNoParameter)
			{
			//registering callbacks for the sending source 
			iServer->iSendSource.RegisterEventCallbackL(static_cast<TRtpEventType>(lEvent),
									 fn, iServer, lBoolOneShot? ERtpOneShot:ERtpNotOneShot, lParam);
									 
			}
			else 
			{
			iServer->iSendSource.RegisterEventCallbackL(static_cast<TRtpEventType>(lEvent),
									 fn, iServer, lBoolOneShot? ERtpOneShot:ERtpNotOneShot);
		
			}
		}
	return TestStepResult();
	}
	

TVerdict CCheckRTCPMinReq::doTestStepL()
	{
	/**
	Check if we have got an RTCP packets with an SR/SR and SDES-CNAME 
	component
	*/
	SetTestStepResult(EFail);
	if ((iServer->iGotSR|iServer->iGotRR) && iServer->iGotSdes && iServer->iEsdesCNAME )
		{
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}

TVerdict CSendAPPPacket::doTestStepL()
	{
	/**
	Send an application defined APP packet as an RTCP packet
	*/
	_LIT8(KCname,"MyAP");
	SetTestStepResult(EFail);
	TBuf8<4> lMybuf(KCname);
	//send will not work until we get an SSRC entry in the collision manager
	iServer->iAPPReceiveCount = 0;
	iServer->iSession.SendAPPL(lMybuf);
	CActiveScheduler::Start();
	return TestStepResult();
	}
	
TVerdict CSendRTCPPacket::doTestStepL()
	{
	/**
	Sends an application created RTCP packet (RR/SR) 
	The binary  RTCP packet is read from the configuration file.
	The Scheduler is started and needs to be stopped on a related 
	event-callback.
	*/
	SetTestStepResult(EFail);
	//We will send an SR packet here 
	iServer->iRTCPSRRecvCount = 0;
	TPtrC lFileName(0,0);
	GetStringFromConfig(ConfigSection(),_L("PayloadFile"), lFileName);
	User::LeaveIfError(iServer->iFile.Open(iServer->iFs,lFileName,EFileRead));
	TInt lFsize;
	User::LeaveIfError(iServer->iFile.Size(lFsize));
	HBufC8* lPacket = HBufC8::NewMaxLC(lFsize);
	TPtr8 lPtrPacket(lPacket->Des());
	User::LeaveIfError(iServer->iFile.Read(lPtrPacket));
	
	_LIT8(KPrefix,"MYPREFIX"); 
	TBufC8<8> lMybuf2(KPrefix);
	_LIT8(KValue,"MYPRIVATEDATA1234567890"); 
	TBufC8<23> lMybuf3(KValue);
	
	//iServer->iSession.SetRTCPAutoSend(1);
	User::LeaveIfError(iServer->iSession.SetPRIVL(lMybuf2, lMybuf3));
	_LIT8(KReason1,"NAME"); 
	TBufC8<4> lMybuf1(KReason1);
	User::LeaveIfError(iServer->iSession.SetSDESL(2, lMybuf1));
	TBuf8<3> flag;
	// 0x is 128 + 2 --> 1000 0010
	flag.Append(0x82);
	 	
	//TBuf8<3> buf(_L8("1"));
	iServer->iSession.SendRTCPPacketL(flag);
	iServer->iFile.Close();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(lPacket);
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CSendBYEPacket::doTestStepL()
	{	
	/**
	Sends an application defined BYE packet with a reason  
	The binary  RTCP packet is read from the configuration file.
	The Scheduler is started and needs to be stopped on a related 
	event-callback.
	*/
	SetTestStepResult(EFail);
	TPtrC lReasonText(0,0);
	GetStringFromConfig(ConfigSection(),_L("ReasonText"), lReasonText);
	TBuf8<256> lMybuf;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(lMybuf,lReasonText);
	iServer->iBYEReceiveCount = 0;
	iServer->iSendSource.ByeL(lMybuf);
	CActiveScheduler::Start();
	return TestStepResult();
	}

TVerdict CSetSDESPacket::doTestStepL()
	{
	/**
	Configures an application defined SDES packet with private data  
	The binary  RTCP packet is read from the configuration file.
	The Scheduler is started and needs to be stopped on a related 
	event-callback.
	*/
	SetTestStepResult(EFail);
	iServer->iSDESReceiveCount = 0;
	_LIT8(KPrefix,"MYPREFIX"); 
	TBufC8<8> lMybuf2(KPrefix);
	_LIT8(KValue,"MYPRIVATEDATA1234567890"); 
	TBufC8<23> lMybuf3(KValue);
	
	iServer->iSession.SetRTCPAutoSend(0);
	User::LeaveIfError(iServer->iSession.SetPRIVL(lMybuf2, lMybuf3));
	_LIT8(KReason1,"NAME"); 
	TBufC8<4> lMybuf1(KReason1);
	User::LeaveIfError(iServer->iSession.SetSDESL(2, lMybuf1));
	
	/* Try a combination of values and See if they are all success */
    User::LeaveIfError(iServer->iSession.SetSDESL(6, _L8("TheRightTool")));	
    User::LeaveIfError(iServer->iSession.SetSDESL(7, _L8("TheRightNote")));
    User::LeaveIfError(iServer->iSession.SetSDESL(3, _L8("snail@email.com")));
    
	/* Create a Duplicate should leave with KErrAlreadyExists */
	TRAPD(errDup,iServer->iSession.SetSDESL(6, _L8("DuplicateTool")));
	if(KErrAlreadyExists != errDup)
		{
		INFO_PRINTF1(_L("SetSDESL - Failed to detect duplicates!"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	/* A Non exitant sdes Iem */
	TRAPD(errNotThere,iServer->iSession.SetSDESL(9, _L8("IDontExist!")));
	if(KErrArgument != errNotThere)
		{
		INFO_PRINTF1(_L("SetSDESL - Failed to detect Non exitant Item!"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	TBuf8<2> flag;
	flag.Append(0x80| 0x02);

	iServer->iSession.SendRTCPPacketL(flag);
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCheckRTCPInterval::doTestStepL()
	{	
	/**
	Calculates the average RTCP sending interval
	*/
	SetTestStepResult(EFail);
	if (iServer->DoRTCPIntervalTest(iServer))
		{
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}


void CTimedStepRtcp::InitTimerL(TThreadPriority aPriority)
	{
	iPeriodic = CPeriodic::New(aPriority);
	iTimerExpired = EFalse;
	}

TInt CTimedStepRtcp::LocalTimerCallback(TAny *aPtr)
	{
	CTimedStepRtcp *pThis = reinterpret_cast<CTimedStepRtcp*>(aPtr);
	pThis->iTimerExpired = ETrue;
	CActiveScheduler::Stop();
	pThis->iPeriodic->Cancel();
	return KErrNone;
	}

void CTimedStepRtcp::DeInitTimerL()
	{
	delete iPeriodic;
	iPeriodic = 0;
	}


/* Non RTCP data Send and Receive steps */
TVerdict CRegisterNonRtcpDataStep::doTestStepL()
	{	
	SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Register NonRTCP Data Step."));
	TInt lEvent;
	GetIntFromConfig(ConfigSection(),_L("Event"),lEvent);
	
	//Register for Non RTP data callback.	
	iServer->iSession.RegisterEventCallbackL(ENonRtcpDataReceived, CTe_RTCPSuite::CbNonRtcpPacketReceivedL, iServer);

	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CSendNonRtcpDataStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Send Non RTCP Data Step..."));
	
	TBuf8<100> nonRtcpData;
	nonRtcpData.SetLength(100);
	nonRtcpData.Fill('Z');
	
	TRequestStatus stat;
	iServer->iSession.SendDataL(EFalse,nonRtcpData, stat);
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());
	/* Start the Active Scheduler and Wait for the Reflected data */
	CActiveScheduler::Start();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CReceiveNonRtcpDataStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Receive Non RTCP Data Step..."));
	
	TBuf8<100> nonRtcpData;
	nonRtcpData.SetLength(100);
	nonRtcpData.Fill('Z');
	
	TRequestStatus stat;
	const TDesC8& des = iServer->iSession.NonRtcpDataL();
	if(0 != des.Compare(nonRtcpData))
		{
		INFO_PRINTF1(_L("Data Not Matching .."));
		SetTestStepResult(EFail);
		}
	else
		{
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}

TInt CRtcpPreAndPostProcessingTestStep::PreRtpProcessing(TDes8& )
	{
	return KErrNone;
	}

void CRtcpPreAndPostProcessingTestStep::PostRtpProcessing(TDes8& )
	{
	return;
	}

TInt CRtcpPreAndPostProcessingTestStep::PreRtcpProcessing(TDes8& aRtpData)
	{
	INFO_PRINTF1(_L("CRtcpPreAndProcessingTestStep::PreRtcpProcessing..."));
	/* Check for custom Footer */
	TPtrC8 ptrCustomData = aRtpData.Right(100);
	/* Attach our Custom Footer to this RTP Packet. */
	TBuf8<100> customData;
	customData.SetLength(100);
	customData.Fill('Z');
	
	if(0 == customData.Compare(ptrCustomData) && iPassCallBack)
		{
		INFO_PRINTF1(_L("Data Matches..."));
		iProcessingOK = ETrue;
		aRtpData.SetLength(aRtpData.Length()-100);
		INFO_PRINTF1(_L("Stopping the scheduler .."));
		CActiveScheduler::Stop();
		}
	else
		{
		INFO_PRINTF1(_L("Data Doesn't Match...Probably a Previous RTCP Packet?"));
		}
	return iPassCallBack?KErrNone:KErrGeneral;
	}

void CRtcpPreAndPostProcessingTestStep::PostRtcpProcessing(TDes8& aRtpData)
	{
	INFO_PRINTF1(_L("CRtcpPreAndProcessingTestStep::PostRtcpProcessing..."));
	/* Attach our Custom Footer to this RTP Packet. */
	TBuf8<100> customData;
	customData.SetLength(100);
	customData.Fill('Z');
	aRtpData.Append(customData);
	}

TVerdict CRtcpPreAndPostProcessingTestStep::doTestStepPreambleL()
	{
	SetTestStepResult(EFail);
	iProcessingOK = EFalse;
	GetIntFromConfig(ConfigSection(),_L("PassCallBack"),iPassCallBack);
	InitTimerL(EPriorityNormal);
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CRtcpPreAndPostProcessingTestStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	iServer->iSession.SetPrePostProcessingRegisterCallback(this);
	
	iPeriodic->Start(20000000, 20000000, TCallBack(LocalTimerCallback,this));

	/* Now wait for the stack to generate an RTCP Report */
	CActiveScheduler::Start();
	
	if(iProcessingOK)
		{
		SetTestStepResult(EPass);
		}
	DeInitTimerL();
	/* Unregister the callback as we are going out of Scope */
	iServer->iSession.SetPrePostProcessingRegisterCallback(0);
	return TestStepResult();
	}


/* Changes the Remote RTCP port and sees if the RTCP report reaches the
 * new destination.
 */
TVerdict CChangeRemoteAddrToLoopStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	
	/* Change the RTP remote address */
	TInetAddr locAddr;
	locAddr.SetAddress(INET_ADDR(127,0,0,1));
	//locAddr.SetAddress(INET_ADDR(10,253,2,95));
	
	TInt locDataPortRtp;
	GetIntFromConfig(ConfigSection(),_L("LocalDataPortRtp"), locDataPortRtp);
	locAddr.SetPort(locDataPortRtp);

	TInt locDataPortRtcp;
	GetIntFromConfig(ConfigSection(),_L("LocalDataPortRtcp"), locDataPortRtcp);

	iServer->iSession.SetRemoteAddress(locAddr);
	iServer->iSession.SetRemoteRtcpPort(locDataPortRtcp);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CSetupLocalRecvSockStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	
	iServer->iDataSocket.Open(iServer->iSocketServer, KAfInet, KSockDatagram, KProtocolInetUdp);
	
	TInetAddr locAddr;
	locAddr.SetAddress(INET_ADDR(127,0,0,1));
	
	TInt locDataPortRtcp;
	GetIntFromConfig(ConfigSection(),_L("LocalDataPortRtcp"), locDataPortRtcp);
	locAddr.SetPort(locDataPortRtcp);
	
	User::LeaveIfError(iServer->iDataSocket.Bind(locAddr));
	
	/* Send an RTCP data out */
   	TBuf8<2> flag;
   	flag.Append(0x80| 0x02);
 		
   	iServer->iSession.SendRTCPPacketL(flag);

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CRecvOnLocalSockStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	
	TBuf8<1000> recvBuf;
	TInetAddr recvAddr;
	TRequestStatus stat;
	iServer->iDataSocket.RecvFrom(recvBuf, recvAddr, 0, stat);
	User::WaitForRequest(stat);
	
	if(KErrNone == stat.Int())
		{
		SetTestStepResult(EPass);
		}
	
	iServer->iDataSocket.Close();
	return TestStepResult();
	}

/* Try grabbing the RTCP Socket and go some funny things on it */
TVerdict CSetAndGetOptStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	
	RSocket *rtpSock = iServer->iSession.RtcpSocket();
	
	User::LeaveIfNull(rtpSock); //Well this is the real test
	
	TInt opt=0;
	rtpSock->GetOpt(KSoUdpReceiveICMPError, KSolInetUdp, opt);
	
	if(opt)
		{
		INFO_PRINTF1(_L("WARNING:!! By default ICMP Error Should be off !!!"));
		}
	rtpSock->SetOpt(KSoUdpReceiveICMPError, KSolInetUdp, opt);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/* Check the RTCP Statistics API. 
 * Is it sane ?
 */
TVerdict CRtcpStatisticsStep::doTestStepL()
	{
	SetTestStepResult(EPass);
	
	TUint32 lSenderSSRC = iServer->iReceiveSource.SSRC();
	
	TRtcpStatistics lRtcpStat;
	User::LeaveIfError(iServer->iSession.RtcpStatistics(lSenderSSRC, lRtcpStat));
	
	/* We should try to validate all these fileds. But its more like a job of
	 * RTCP RR report sanity checks.
	 * As of now we will print the values and try to catch some very obvious errors. 
	 */
	INFO_PRINTF2(_L("Octect Sent value is %u"),lRtcpStat.OctetsSent());
	
	if(0 == lRtcpStat.Jitter())
		{
		/* A Zero for jitter is very very unlikely */
		SetTestStepResult(EFail);
		}
	INFO_PRINTF2(_L("The Jitter value is %u"),lRtcpStat.Jitter());
	
	//Add more sanity checks for these two.
	INFO_PRINTF2(_L("The PacketsLost value is %u"),lRtcpStat.PacketsLost());
	INFO_PRINTF2(_L("The FractionLost value is %d"),lRtcpStat.FractionLost());
	
	return TestStepResult();
	}

TVerdict CCloseSession::doTestStepL()
	{
	/**
	Closes the RTP session created without using the subconnection
	*/
	TBuf8<50> byeReason((TUint8*)"GoodBye");
	
	SetTestStepResult(EPass);
	iServer->iFile.Close();
	iServer->iFs.Close();
	iServer->iReceivePacket.Close();
	iServer->iSendPacket.Close();
	if(iServer->iSendSource.IsOpen())
		{
		iServer->iSendSource.ByeL(byeReason);
		}

	if (iServer->iSendSource.IsSending())
		{
		iServer->iSendSource.Cancel();
		}
	iServer->iSendSource.Close();
	iServer->iLastActiveSource.Close();
	iServer->iRtcpSocket.Close();
	iServer->iRtpSocket.Close();
	iServer->iConnect.Close();
	if(iServer->iSession.IsOpen() )
		{
		iServer->iSession.Close();
		}
	
	iServer->iSocketServer.Close();
	CActiveScheduler::Install(NULL);
	if(iServer->iScheduler)
		{
		delete iServer->iScheduler;
		iServer->iScheduler = 0;
		}
	return TestStepResult();
	}

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
TVerdict CCloseSession2::doTestStepL()
	{
	/**
	Closes the RTP session created using the subconnection
	*/
	TBuf8<50> byeReason((TUint8*)"GoodBye Cruel World..");
	SetTestStepResult(EPass);
	iServer->iFile.Close();
	iServer->iFs.Close();
	iServer->iReceivePacket.Close();
	iServer->iSendPacket.Close();
	if(iServer->iSendSource.IsOpen())
		{
		iServer->iSendSource.ByeL(byeReason);
		}

	if (iServer->iSendSource.IsSending())
		{
		iServer->iSendSource.Cancel();
		}
	iServer->iSendSource.Close();
	iServer->iLastActiveSource.Close();
	iServer->iRtcpSocket.Close();
	iServer->iRtpSocket.Close();
	if(iServer->iSession.IsOpen() )
		{
		iServer->iSession.Close();
		}
	iServer->iSubConnect.Close();
	iServer->iConnect.Close();
	iServer->iSocketServer.Close();
	CActiveScheduler::Install(NULL);
	delete iServer->iScheduler;
	iServer->iScheduler = 0;
	return TestStepResult();
	}
#endif

/* What additional condition does this test case cover ? */
TVerdict CrtcpSetandGetRtcpSocket::doTestStepL()
	{
	SetTestStepResult(EFail);
	
	RSocket *rtcpSock = iServer->iSession.RtcpSocket();
	
	User::LeaveIfNull(rtcpSock);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/* What additional condition does this test case cover ? */
TVerdict CRtcpStatisticsUnitTestStep::doTestStepL()
	{
	
	TUint32 lSenderSSRC = iServer->iReceiveSource.SSRC();
	
	TRtcpStatistics lRtcpStat;
	TInt err = iServer->iSession.RtcpStatistics(lSenderSSRC, lRtcpStat);
	
	if(err != KErrNone)
		{
		SetTestStepResult(EFail);
		}
	
	TUint32 ssrc = 0xFADE;
	TRtcpStatistics rtcpStatistics;
	err = iServer->iSession.RtcpStatistics(ssrc, rtcpStatistics);
	
	if(err != KErrNotFound)
		{
		SetTestStepResult(EFail);
		}
	
	rtcpStatistics.SetFractionLost(5);
	rtcpStatistics.SetJitter(10);
	rtcpStatistics.SetOctetsSent(15);
	rtcpStatistics.SetPacketsLost(20);
	rtcpStatistics.SetPacketsSent(100);
	
	if(rtcpStatistics.FractionLost() != 5)
		{
		SetTestStepResult(EFail);
		}
	
	if(rtcpStatistics.Jitter() != 10)
		{
		SetTestStepResult(EFail);
		}
	
	if(rtcpStatistics.OctetsSent() != 15)
		{
		SetTestStepResult(EFail);
		}
	
	if(rtcpStatistics.PacketsLost() != 20)
		{
		SetTestStepResult(EFail);
		}
	
	if(rtcpStatistics.PacketsSent() != 100)
		{
		SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}

 
TVerdict CSetRtcpIntervalStep::doTestStepL()
	{
	SetTestStepResult(EFail);
		
	TTimeIntervalMicroSeconds32 interval = 5000000;
	iServer->iSession.SetRtcpInterval(interval);
	SetTestStepResult(EPass);
	
	return TestStepResult();
	}


TVerdict CSetRtpStreamParamsStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	TInt minSequential;
	TInt maxMisorder;
	TInt maxDropout;
	GetIntFromConfig(ConfigSection(),_L("MinSequential"), minSequential);
	GetIntFromConfig(ConfigSection(),_L("MaxMisorder"), maxMisorder);
	GetIntFromConfig(ConfigSection(),_L("MaxDropout"), maxDropout);
	
	iServer->iSession.SetRtpStreamParameters(minSequential,maxMisorder,maxDropout);
	SetTestStepResult(EPass);
		
	return TestStepResult();
	}


TVerdict CSetandGetAutoSendOptionStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	TInt autoSend;
	GetIntFromConfig(ConfigSection(),_L("RTCPAutoSend"), autoSend);
	iServer->iSession.SetRTCPAutoSend(autoSend);
	
	TBool rtcpAutoSend = iServer->iSession.RTCPAutoSend();
	
	if(rtcpAutoSend == autoSend)
		{
		SetTestStepResult(EPass);
		}
	
	return TestStepResult();
	}

TVerdict CSetSessionBandwidthStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	TInt bandwidth;
	GetIntFromConfig(ConfigSection(),_L("Bandwidth"), bandwidth);	
	iServer->iSession.SetBandwidth(bandwidth);
	SetTestStepResult(EPass);
	
	return TestStepResult();
	}

