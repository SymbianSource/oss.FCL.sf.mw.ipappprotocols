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
// Example CTestStep derived implementation
// 
//

/**
 @file
*/

#include "rtpstep.h"
#include "te_rtpsuitedefs.h"
#include "te_rtpsuiteserver.h"
#include <in_sock.h>
#include "rtp.h"
#include <utf.h>

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
#include "rtp_subconparams.h"
#endif

CrtpStep::~CrtpStep()
/**
 * Destructor
 */
	{
	}

CrtpStep::CrtpStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KrtpStep);
	}


TVerdict CrtpStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class Crtp0Step"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CrtpStep::doTestStepL()
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

TVerdict CrtpStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

TVerdict CrtpOpenSessionStep::doTestStepL()
	{
		SetTestStepResult(EFail);
		INFO_PRINTF1(_L("Open RTP Session."));
		TInt localPort;
		TInt remotePort;
		TInt defaultMaxPcktSize;

		TPtrC remoteIP(0,0);
		TPtrC cnameData(0,0);
		TInetAddr localAddress;
		TInetAddr remoteAddress;

		GetIntFromConfig(ConfigSection(),_L("LocalPort"), localPort);
		GetIntFromConfig(ConfigSection(),_L("RemotePort"), remotePort);
		GetStringFromConfig(ConfigSection(),_L("RemoteIP"), remoteIP);
		GetIntFromConfig(ConfigSection(),_L("DefaultMaxPcktSize"), defaultMaxPcktSize);
		GetStringFromConfig(ConfigSection(),_L("CNAME"), cnameData);
		TBuf8<100> iCnameBuf;
		CnvUtfConverter::ConvertFromUnicodeToUtf8(iCnameBuf,cnameData);

		remoteAddress.SetPort(remotePort);
		remoteAddress.Input(remoteIP);

		localAddress.SetPort(localPort);
		localAddress.SetAddress(KInetAddrAny);

		iServer->sched = new(ELeave) CActiveScheduler;
		CActiveScheduler::Install(iServer->sched);

		iServer->sockServ.Connect();
		iServer->rConn.Open(iServer->sockServ,KAfInet);
		iServer->rConn.Start();
		//TRAPD(result, iServer->rtpSession.OpenL(iServer->sockServ, localAddress,remoteAddress, KDefaultMaxPcktSize,iServer->rConn, EPriorityNormal, KCname));
		TRAPD(result, iServer->rtpSession.OpenL(iServer->sockServ, localAddress,remoteAddress, defaultMaxPcktSize, iServer->rConn, EPriorityNormal, iCnameBuf));

		if (KErrNone!=result)
		{
			INFO_PRINTF1(_L("RTP Session Open failed."));
			SetTestStepResult(EFail);
		}
		else
		{
			INFO_PRINTF1(_L("RTP Session Open successful."));
			SetTestStepResult(EPass);
		}
		/* By default the reflector is configured to change SSRC */
		TInt remoteCmdPort;
		TPtrC remoteIP2(0,0);
		TInetAddr remoteAddr;

		GetIntFromConfig(ConfigSection(),_L("RemoteCommandPort"), remoteCmdPort);
		GetStringFromConfig(ConfigSection(),_L("RemoteIP"), remoteIP2);
		

		remoteAddr.Input(remoteIP2);
		remoteAddr.SetPort(remoteCmdPort);

		iServer->ConfigureReflectorModeL(ETrue, remoteAddr);

		return TestStepResult();
	}
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
TVerdict CrtpOpenSessionTwoStep::doTestStepL()
	{
		SetTestStepResult(EFail);

		INFO_PRINTF1(_L("Open RTP Session with subconnection."));

		TInt localPort;
		TInt remotePort;
		TPtrC remoteIP(0,0);
		TInetAddr localAddress;
		TInetAddr remoteAddress;
		TPtrC lCnameData(0,0);
		TInt lBandwidth;
		TInt lRTPTimeNow;
		TInt lRTPTimeConversion;
		TInt lRTCPAutoSend;
		TInt lRtcpInterval;
		TInt lMinSequential;
		TInt lMaxMisorder;
		TInt lMaxDropout;
		TInt lMaxRXSize;


		GetIntFromConfig(ConfigSection(),_L("LocalPort"), localPort);
		GetIntFromConfig(ConfigSection(),_L("RemotePort"), remotePort);
		GetStringFromConfig(ConfigSection(),_L("RemoteIP"), remoteIP);
		GetStringFromConfig(ConfigSection(),_L("CNAME"), lCnameData);
		GetIntFromConfig(ConfigSection(),_L("MaxRXSize"), lMaxRXSize);
		GetIntFromConfig(ConfigSection(),_L("Bandwidth"), lBandwidth);
		GetIntFromConfig(ConfigSection(),_L("RTPTimeNow"), lRTPTimeNow);
		GetIntFromConfig(ConfigSection(),_L("RTPTimeConversion"), lRTPTimeConversion);
		GetIntFromConfig(ConfigSection(),_L("RtcpInterval"), lRtcpInterval);
		GetIntFromConfig(ConfigSection(),_L("MinSequential"), lMinSequential);
		GetIntFromConfig(ConfigSection(),_L("MaxMisorder"), lMaxMisorder);
		GetIntFromConfig(ConfigSection(),_L("MaxDropout"), lMaxDropout);
		GetIntFromConfig(ConfigSection(),_L("RTCPAutoSend"), lRTCPAutoSend);

		remoteAddress.SetPort(remotePort);
		remoteAddress.Input(remoteIP);

		localAddress.SetPort(localPort);
		localAddress.SetAddress(KInetAddrAny);

		TBuf8<100> lCnameBuf;
		CnvUtfConverter::ConvertFromUnicodeToUtf8(lCnameBuf,lCnameData);

		iServer->sched = new(ELeave) CActiveScheduler;
		CActiveScheduler::Install(iServer->sched);
				
		iServer->sockServ.Connect();
		iServer->rConn.Open(iServer->sockServ,KAfInet);
		iServer->rConn.Start();

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
		lReqGenericParams->SetDefDestinationAddr(remoteAddress);
		lReqGenericParams->SetRtcpReportInterval(lRtcpInterval);
		lReqGenericParams->SetServiceProtocolID(KProtocolInetUdp);
		lReqGenericParams->SetLowerSubConnType(RSubConnection::EAttachToDefault);

		TUid lidTier = TUid::Uid(2048);
		lReqGenericParams->SetTierID(lidTier);
		iServer->rSubConn.Open(iServer->sockServ,RSubConnection::ECreateNew,iServer->rConn);

		// Set Properties of the sub-connection
		iServer->rSubConn.SetParameters(lSubconParams);
		// Destroy parameters
		CleanupStack::PopAndDestroy();         // subconParams
		TRAPD(result,iServer->rtpSession.OpenL(iServer->sockServ, localAddress, remoteAddress, lMaxRXSize , iServer->rSubConn, EPriorityNormal, lCnameBuf));

		if (KErrNone!=result)
		{
			INFO_PRINTF1(_L("RTP Session Open with subconnection failed."));
			SetTestStepResult(EFail);
		}
		else
		{
			INFO_PRINTF1(_L("RTP Session Open with subconnection successful."));
			SetTestStepResult(EPass);
		}
		/* By default the reflector is configured to change SSRC */
		TInt remoteCmdPort;
		TPtrC remoteIP2(0,0);
		TInetAddr remoteAddr;

		GetIntFromConfig(ConfigSection(),_L("RemoteCommandPort"), remoteCmdPort);
		GetStringFromConfig(ConfigSection(),_L("RemoteIP"), remoteIP2);
		

		remoteAddr.Input(remoteIP2);
		remoteAddr.SetPort(remoteCmdPort);

		iServer->ConfigureReflectorModeL(ETrue, remoteAddr);

		return TestStepResult();
	}
#endif

TVerdict CrtpMakePacketStep::doTestStepL()
	{
		SetTestStepResult(EFail);
		TInt headerExtensionTest;
		GetIntFromConfig(ConfigSection(),_L("HeaderExtensionTest"), headerExtensionTest);
		iServer->rtpSendSrc = iServer->rtpSession.NewSendSourceL();
		iServer->rtpSendSrc.SetDefaultPayloadSize(100);
		iServer->rtpSendSrc.SetPayloadType(15); //15->G728 

		if (headerExtensionTest==1)
			{
			//Do nothing.
			}
		else
			{
			iServer->rtpSendPacket = iServer->rtpSendSrc.NewSendPacketL();
			}

		iServer->rtpSession.SetRTPTimeConversion(100,100);
		iServer->rtpSendSrc.RegisterEventCallbackL(ERtpSendSucceeded, CTe_rtpSuite::PacketSentSuccess, iServer);
		iServer->rtpSession.RegisterEventCallbackL(ERtpNewSource, CTe_rtpSuite::NewSourceL, iServer);

		SetTestStepResult(EPass);

		return TestStepResult();
	}

TVerdict CrtpSendPacketStep::doTestStepL()
	{
	SetTestStepResult(EFail);

	TPtrC8 payloadTemp(_L8("This is a test rtp packet"));
	TPtr8 payload(const_cast<TUint8*>(iServer->rtpSendPacket.WritePayload().Ptr()),100,100);
	payload = payloadTemp;
	if (payload.Length()>0)
		{
		TInt len = payload.Length();
		iServer->rtpSendPacket.WritePayload().SetLength(len);
		iServer->rtpSendPacket.Send();
		}

	SetTestStepResult(EPass);
	return TestStepResult(); 
	}

TVerdict CrtpStartSchedulerStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Starting the ActiveScheduler"));  //Block start
	CActiveScheduler::Start();
	SetTestStepResult(EPass);
	return TestStepResult(); 
	}

TVerdict CrtpSetMarkerStep::doTestStepL()
	{
	SetTestStepResult(EPass);
	TInt setMarker;
	GetIntFromConfig(ConfigSection(),_L("Marker"),setMarker);
	INFO_PRINTF2(_L("Set Marker Bit to %d"), setMarker);
	iServer->rtpSendPacket.SetMarker(setMarker);
	return TestStepResult();
	}

TVerdict CrtpCheckMarkerStep::doTestStepL()
	{
	SetTestStepResult(EPass);
	TInt markerBit;
	GetIntFromConfig(ConfigSection(),_L("Marker"),markerBit);

	TInt getMarker = iServer->rtpRecvPacket.Marker();
	INFO_PRINTF2(_L("Check if Marker Bit is set to %d "), markerBit);
	if (getMarker != markerBit)
		SetTestStepResult(EFail);

	return TestStepResult();
	}

TVerdict CrtpSetTimestampStep::doTestStepL()
	{
	SetTestStepResult(EPass);
	TInt setTimestamp;
	GetIntFromConfig(ConfigSection(),_L("Timestamp"),setTimestamp);
	INFO_PRINTF2(_L("Set Marker Bit to %d"), setTimestamp);
	iServer->rtpSendPacket.SetTimestamp(setTimestamp);
	return TestStepResult();
	}

TVerdict CrtpCheckTimestampStep::doTestStepL()
	{
	SetTestStepResult(EPass);
	TInt timestamp;
	GetIntFromConfig(ConfigSection(),_L("Timestamp"),timestamp);
	TInt getTimestamp = iServer->rtpRecvPacket.Timestamp();
	INFO_PRINTF2(_L("Check if Timestamp is set to %d "), timestamp);
	if (getTimestamp != timestamp)
		SetTestStepResult(EFail);

	return TestStepResult();
	}

TVerdict CrtpSetPayloadTypeStep::doTestStepL()
	{
	SetTestStepResult(EPass);
	TInt setPayloadType;
	GetIntFromConfig(ConfigSection(),_L("PayloadType"),setPayloadType);
	INFO_PRINTF2(_L("Setting PayloadType to %d"), setPayloadType);
	iServer->rtpSendPacket.SetPayloadType(setPayloadType);
	return TestStepResult();
	}

TVerdict CrtpCheckPayloadTypeStep::doTestStepL()
	{
	SetTestStepResult(EPass);
	TInt payloadType;
	GetIntFromConfig(ConfigSection(),_L("PayloadType"),payloadType);
	TInt getPayloadType = iServer->rtpRecvPacket.PayloadType();
	INFO_PRINTF2(_L("PayloadType is %d "), getPayloadType);
	if (payloadType != getPayloadType)
		SetTestStepResult(EFail);
	return TestStepResult();
	}

TVerdict CrtpMultipleSessionStep::doTestStepL()
	{
		SetTestStepResult(EFail);

		INFO_PRINTF1(_L("Multiple session step."));
		iServer->sched = new(ELeave) CActiveScheduler;
		CActiveScheduler::Install(iServer->sched);

		TInetAddr localAddressOne;
		TInetAddr localAddressTwo;
		TInetAddr remoteAddressOne;
		TInetAddr remoteAddressTwo;

		TInt sessionOneLocalPort;
		TInt sessionOneRemotePort;
		TInt sessionTwoLocalPort;
		TInt sessionTwoRemotePort;
		TInt defaultMaxPcktSize;
		TPtrC cnameData(0,0);

		GetIntFromConfig(ConfigSection(),_L("SessionOneLocalPort"), sessionOneLocalPort);
		GetIntFromConfig(ConfigSection(),_L("SessionOneRemotePort"), sessionOneRemotePort);
		GetIntFromConfig(ConfigSection(),_L("SessionTwoLocalPort"), sessionTwoLocalPort);
		GetIntFromConfig(ConfigSection(),_L("SessionTwoRemotePort"), sessionTwoRemotePort);
		GetIntFromConfig(ConfigSection(),_L("DefaultMaxPcktSize"), defaultMaxPcktSize);
		GetStringFromConfig(ConfigSection(),_L("CNAME"), cnameData);

		TBuf8<100> iCnameBuf;
		CnvUtfConverter::ConvertFromUnicodeToUtf8(iCnameBuf,cnameData);

		localAddressOne.SetPort(sessionOneLocalPort);
		localAddressOne.SetAddress(KInetAddrLoop);
		remoteAddressOne.SetPort(sessionOneRemotePort);
		remoteAddressOne.SetAddress(KInetAddrLoop);

		localAddressTwo.SetPort(sessionTwoLocalPort);
		localAddressTwo.SetAddress(KInetAddrLoop);
		remoteAddressTwo.SetPort(sessionTwoRemotePort);
		remoteAddressTwo.SetAddress(KInetAddrLoop);

		iServer->sockServTwo.Connect();
		iServer->rConnTwo.Open(iServer->sockServTwo,KAfInet);
		iServer->rConnTwo.Start();
		iServer->rtpSessionTwo.OpenL(iServer->sockServTwo, localAddressTwo ,remoteAddressTwo, defaultMaxPcktSize,iServer->rConnTwo, EPriorityNormal, iCnameBuf);
		iServer->rtpSessionTwo.SetRTPTimeConversion(100,100);
		iServer->rtpSessionTwo.RegisterEventCallbackL(ERtpNewSource, CTe_rtpSuite::NewSourceL, iServer);

		iServer->sockServOne.Connect();
		iServer->rConnOne.Open(iServer->sockServOne,KAfInet);
		iServer->rConnOne.Start();
		iServer->rtpSessionOne.OpenL(iServer->sockServOne, localAddressOne, remoteAddressOne, defaultMaxPcktSize,iServer->rConnOne, EPriorityNormal, iCnameBuf);
		iServer->rtpSessionOne.SetRTPTimeConversion(100,100);

		iServer->rtpSendSrc = iServer->rtpSessionOne.NewSendSourceL();
		iServer->rtpSendPacket = iServer->rtpSendSrc.NewSendPacketL(100);
		iServer->rtpSendSrc.RegisterEventCallbackL(ERtpSendSucceeded, CTe_rtpSuite::PacketSentSuccess, iServer);
		iServer->rtpSendSrc.SetDefaultPayloadSize(100);
		iServer->rtpSendSrc.SetPayloadType(15); //15->G728 

		if (iServer->rtpSessionOne == iServer->rtpSessionTwo)
			{
			INFO_PRINTF1(_L("Session One and Session Two are equal."));
			}

		
		if (iServer->rtpSessionOne != iServer->rtpSessionTwo)
			{
			INFO_PRINTF1(_L("Session One and Session Two are not equal."));
			}

		TPtrC8 payloadTemp(_L8("This is a test rtp packet"));
		TPtr8 payload(const_cast<TUint8*>(iServer->rtpSendPacket.WritePayload().Ptr()),100,100);
		payload = payloadTemp;
		if (payload.Length()>0)
		{
			TInt len = payload.Length();
			iServer->rtpSendPacket.WritePayload().SetLength(len);
			iServer->rtpSendPacket.Send();
			INFO_PRINTF1(_L("Starting the ActiveScheduler"));  //Block start
			CActiveScheduler::Start();
		}

		INFO_PRINTF1(_L("Multiple session test success."));

		SetTestStepResult(EPass);
		return TestStepResult();
	}

TVerdict CrtpMultipleSessionCloseStep::doTestStepL()
	{
		SetTestStepResult(EFail);

		INFO_PRINTF1(_L("Close Multiple RTP Sessions step."));
		iServer->rtpSendPacket.Close();
		iServer->rtpRecvPacket.Close();
		iServer->rtpSendSrc.Close();
		iServer->rtpSessionOne.Close();
		iServer->rtpSessionTwo.Close();
		iServer->rConnOne.Close();
		iServer->rConnTwo.Close();
		iServer->sockServOne.Close();
		iServer->sockServTwo.Close();
		delete iServer->sched;
		iServer->sched = 0;
		INFO_PRINTF1(_L("Multiple RTP Sessions closed successful."));

		SetTestStepResult(EPass);
		return TestStepResult();
	}

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
TVerdict CrtpMultipleSessionTwoStep::doTestStepL()
	{
		SetTestStepResult(EFail);

		INFO_PRINTF1(_L("Multiple session step."));
		iServer->sched = new(ELeave) CActiveScheduler;
		CActiveScheduler::Install(iServer->sched);

		TInetAddr localAddressOne;
		TInetAddr localAddressTwo;
		TInetAddr remoteAddressOne;
		TInetAddr remoteAddressTwo;

		TInt sessionOneLocalPort;
		TInt sessionOneRemotePort;
		TInt sessionTwoLocalPort;
		TInt sessionTwoRemotePort;

		TPtrC lCnameData(0,0);
		TInt lBandwidth;
		TInt lRTPTimeNow;
		TInt lRTPTimeConversion;
		TInt lRTCPAutoSend;
		TInt lRtcpInterval;
		TInt lMinSequential;
		TInt lMaxMisorder;
		TInt lMaxDropout;
		TInt lMaxRXSize;

		GetIntFromConfig(ConfigSection(),_L("SessionOneLocalPort"), sessionOneLocalPort);
		GetIntFromConfig(ConfigSection(),_L("SessionOneRemotePort"), sessionOneRemotePort);
		GetIntFromConfig(ConfigSection(),_L("SessionTwoLocalPort"), sessionTwoLocalPort);
		GetIntFromConfig(ConfigSection(),_L("SessionTwoRemotePort"), sessionTwoRemotePort);

		GetStringFromConfig(ConfigSection(),_L("CNAME"), lCnameData);
		GetIntFromConfig(ConfigSection(),_L("MaxRXSize"), lMaxRXSize);
		GetIntFromConfig(ConfigSection(),_L("Bandwidth"), lBandwidth);
		GetIntFromConfig(ConfigSection(),_L("RTPTimeNow"), lRTPTimeNow);
		GetIntFromConfig(ConfigSection(),_L("RTPTimeConversion"), lRTPTimeConversion);
		GetIntFromConfig(ConfigSection(),_L("RtcpInterval"), lRtcpInterval);
		GetIntFromConfig(ConfigSection(),_L("MinSequential"), lMinSequential);
		GetIntFromConfig(ConfigSection(),_L("MaxMisorder"), lMaxMisorder);
		GetIntFromConfig(ConfigSection(),_L("MaxDropout"), lMaxDropout);
		GetIntFromConfig(ConfigSection(),_L("RTCPAutoSend"), lRTCPAutoSend);

		TBuf8<100> lCnameBuf;
		CnvUtfConverter::ConvertFromUnicodeToUtf8(lCnameBuf,lCnameData);

		localAddressOne.SetPort(sessionOneLocalPort);
		localAddressOne.SetAddress(KInetAddrLoop);
		remoteAddressOne.SetPort(sessionOneRemotePort);
		remoteAddressOne.SetAddress(KInetAddrLoop);

		localAddressTwo.SetPort(sessionTwoLocalPort);
		localAddressTwo.SetAddress(KInetAddrLoop);
		remoteAddressTwo.SetPort(sessionTwoRemotePort);
		remoteAddressTwo.SetAddress(KInetAddrLoop);

		iServer->sockServTwo.Connect();
		iServer->rConnTwo.Open(iServer->sockServTwo,KAfInet);
		iServer->rConnTwo.Start();

		RSubConParameterBundle lSubconParamsTwo;
		CSubConParameterFamily* lSubConFamilyTwo = CSubConParameterFamily::NewL(lSubconParamsTwo, KProtocolExtensionFamily);
		CSubConRTPGenericParamSet* lReqGenericParamsTwo = CSubConRTPGenericParamSet::NewL(*lSubConFamilyTwo, CSubConParameterFamily::ERequested);
		TUid lidRTPTwo = TUid::Uid(KRtpFlowImplementationUid);

		// Set the requested Generic Parameters
		lReqGenericParamsTwo->SetProtocolUIDRtp(lidRTPTwo);
		lReqGenericParamsTwo->SetBandWidth(lBandwidth);
		lReqGenericParamsTwo->SetRtptimeConversion(lRTPTimeConversion);
		lReqGenericParamsTwo->SetRtpAutoSend(lRTCPAutoSend);
		lReqGenericParamsTwo->SetRtpTimeNow(lRTPTimeNow);
		lReqGenericParamsTwo->SetCNAMEL(lCnameBuf);
		lReqGenericParamsTwo->SetMaxDropOut(lMaxDropout);
		lReqGenericParamsTwo->SetMinSequencial(lMinSequential);
		lReqGenericParamsTwo->SetMaxMisorder(lMaxMisorder);
		lReqGenericParamsTwo->SetDefDestinationAddr(remoteAddressTwo);
		lReqGenericParamsTwo->SetRtcpReportInterval(lRtcpInterval);
		lReqGenericParamsTwo->SetServiceProtocolID(KProtocolInetUdp);
		lReqGenericParamsTwo->SetLowerSubConnType(RSubConnection::EAttachToDefault);

		TUid lidTierTwo = TUid::Uid(2048);
		lReqGenericParamsTwo->SetTierID(lidTierTwo);

		iServer->rSubConnTwo.Open(iServer->sockServTwo,RSubConnection::ECreateNew,iServer->rConnTwo);
		iServer->rSubConnTwo.SetParameters(lSubconParamsTwo);
		iServer->rtpSessionTwo.OpenL(iServer->sockServTwo, localAddressTwo, remoteAddressTwo, lMaxRXSize , iServer->rSubConnTwo, EPriorityNormal, KNullDesC8);
		iServer->rtpSessionTwo.SetRTPTimeConversion(100,100);
		iServer->rtpSessionTwo.RegisterEventCallbackL(ERtpNewSource, CTe_rtpSuite::NewSourceL, iServer);
		lSubconParamsTwo.Close();

		iServer->sockServOne.Connect();
		iServer->rConnOne.Open(iServer->sockServOne,KAfInet);
		iServer->rConnOne.Start();

		RSubConParameterBundle lSubconParamsOne;
		CSubConParameterFamily* lSubConFamilyOne = CSubConParameterFamily::NewL(lSubconParamsOne, KProtocolExtensionFamily);
		CSubConRTPGenericParamSet* lReqGenericParamsOne = CSubConRTPGenericParamSet::NewL(*lSubConFamilyOne, CSubConParameterFamily::ERequested);
		TUid lidRTPOne = TUid::Uid(KRtpFlowImplementationUid);

		// Set the requested Generic Parameters
		lReqGenericParamsOne->SetProtocolUIDRtp(lidRTPOne);
		lReqGenericParamsOne->SetBandWidth(lBandwidth);
		lReqGenericParamsOne->SetRtptimeConversion(lRTPTimeConversion);
		lReqGenericParamsOne->SetRtpAutoSend(lRTCPAutoSend);
		lReqGenericParamsOne->SetRtpTimeNow(lRTPTimeNow);
		lReqGenericParamsOne->SetCNAMEL(lCnameBuf);
		lReqGenericParamsOne->SetMaxDropOut(lMaxDropout);
		lReqGenericParamsOne->SetMinSequencial(lMinSequential);
		lReqGenericParamsOne->SetMaxMisorder(lMaxMisorder);
		lReqGenericParamsOne->SetDefDestinationAddr(remoteAddressOne);
		lReqGenericParamsOne->SetRtcpReportInterval(lRtcpInterval);
		lReqGenericParamsOne->SetServiceProtocolID(KProtocolInetUdp);
		lReqGenericParamsOne->SetLowerSubConnType(RSubConnection::EAttachToDefault);

		TUid lidTierOne = TUid::Uid(2048);
		lReqGenericParamsOne->SetTierID(lidTierOne);
		iServer->rSubConnOne.Open(iServer->sockServOne,RSubConnection::ECreateNew,iServer->rConnOne);
		iServer->rSubConnOne.SetParameters(lSubconParamsOne);
		iServer->rtpSessionOne.OpenL(iServer->sockServOne, localAddressOne, remoteAddressOne, lMaxRXSize , iServer->rSubConnOne, EPriorityNormal, KNullDesC8);
		iServer->rtpSessionOne.SetRTPTimeConversion(100,100);

		iServer->rtpSendSrc = iServer->rtpSessionOne.NewSendSourceL();
		iServer->rtpSendPacket = iServer->rtpSendSrc.NewSendPacketL(100);
		iServer->rtpSendSrc.RegisterEventCallbackL(ERtpSendSucceeded, CTe_rtpSuite::PacketSentSuccess, iServer);
		iServer->rtpSendSrc.SetDefaultPayloadSize(100);
		iServer->rtpSendSrc.SetPayloadType(15); //15->G728 
		lSubconParamsOne.Close();

		if (iServer->rtpSessionOne != iServer->rtpSessionTwo)
			{
			INFO_PRINTF1(_L("Session One and Session Two are unequal."));
			}
		else
			{
			INFO_PRINTF1(_L("Session One and Session Two are equal."));
			}

		TPtrC8 payloadTemp(_L8("This is a test rtp packet"));
		TPtr8 payload(const_cast<TUint8*>(iServer->rtpSendPacket.WritePayload().Ptr()),100,100);
		payload = payloadTemp;
		if (payload.Length()>0)
		{
			TInt len = payload.Length();
			iServer->rtpSendPacket.WritePayload().SetLength(len);
			iServer->rtpSendPacket.Send();
			INFO_PRINTF1(_L("Starting the ActiveScheduler"));  //Block start
			CActiveScheduler::Start();
		}
        /* A silly check to see if ReceiveSource is really open. Cud have used
         * IsOpen() but we want to hit operator== and operator!= 
         */
		RRtpReceiveSource lRcvSrc;
		if(iServer->rtpRecvSrc != lRcvSrc)
			{
			INFO_PRINTF1(_L("Multiple session Receive Source is Open."));
			}
		INFO_PRINTF1(_L("Multiple session test success."));

		SetTestStepResult(EPass);
		return TestStepResult();
	}

TVerdict CrtpMultipleSessionTwoCloseStep::doTestStepL()
	{
		SetTestStepResult(EFail);

		INFO_PRINTF1(_L("Close Multiple RTP Sessions step."));
		iServer->rtpSendPacket.Close();
		iServer->rtpRecvPacket.Close();
		iServer->rtpSendSrc.Close();
		iServer->rtpSessionOne.Close();
		iServer->rtpSessionTwo.Close();
		iServer->rSubConnOne.Close();
		iServer->rSubConnTwo.Close();
		iServer->rConnOne.Close();
		iServer->rConnTwo.Close();
		iServer->sockServOne.Close();
		iServer->sockServTwo.Close();
		delete iServer->sched;
		iServer->sched = 0;
		INFO_PRINTF1(_L("Multiple RTP Sessions closed successful."));

		SetTestStepResult(EPass);
		return TestStepResult();
	}
#endif

TVerdict CrtpSetHeaderExtensionStep::doTestStepL()
	{

	TInt lBufSize;
	GetIntFromConfig(ConfigSection(),_L("BufferSize"),lBufSize);

	TPtrC lConfig(0,0);
	TBool lExtension;
	TInt  lType = 0;
	TInt  lLen = 1;

	lExtension = GetStringFromConfig(ConfigSection(), _L("HeaderExtension"),lConfig);
	TLex  lLex(lConfig);

	if (lExtension)
		{
		lLex.Val(lType);
		if (!lLex.Eos() && lLex.Peek() == ',')
			lLex.Get();

		lLex.Val(lLen);
		if (!lLex.Eos() && lLex.Peek() == ',')
			lLex.Get();
		}

	iServer->rtpSendPacket = iServer->rtpSendSrc.NewSendPacketL(lBufSize, lLen);
	if (lExtension)
		{
		// Just create the extension now; its existence is checked in
		// CExtension
		RRtpHeaderExtension extension = iServer->rtpSendPacket.Extension();
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

	return TestStepResult();
	}

TVerdict CrtpCheckHeaderExtensionStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	INFO_PRINTF1(_L("HeaderExtension Check."));

	TPtrC lConfig(0,0);
	TBool lExtension;
	TInt  lType = 0;
	TInt  lLen = 1;
	lExtension = GetStringFromConfig(ConfigSection(), _L("HeaderExtension"),lConfig);
	TLex  lLex(lConfig);
	TInt val;

	if (lExtension)
		{
		lLex.Val(lType);
		if (!lLex.Eos() && lLex.Peek() == ',')
			lLex.Get();

		lLex.Val(lLen);
		if (!lLex.Eos() && lLex.Peek() == ',')
			lLex.Get();

		TInt i = 0;
		while (!lLex.Eos() && i < lLen)
			{
			lLex.Val(val);
			if (!lLex.Eos() && lLex.Peek() == ',')
				lLex.Get();
			i++;
			}
		}

	TBool isHeaderExtensionSet;
	isHeaderExtensionSet = iServer->rtpRecvPacket.ExtensionPresent();
	if (isHeaderExtensionSet == 1)
		{
		RRtpHeaderExtension getHeaderExtension = iServer->rtpRecvPacket.Extension();
		TUint16 headerExtensionType = getHeaderExtension.Type();
		TPtrC8 headerExtensionData = getHeaderExtension.Data();
		TInt len = headerExtensionData.Length();

		TUint8* dataOne = const_cast<TUint8*>(&headerExtensionData[0]);
		TUint32* dataInt = reinterpret_cast<TUint32*>(dataOne);
		TUint32 flags = *dataInt;

		if (lType == headerExtensionType)
			{
			if (val == flags)
				{
				SetTestStepResult(EPass);
				}
			}

		}
	return TestStepResult();
	}

TVerdict CrtpCloseSessionStep::doTestStepL()
	{
		SetTestStepResult(EFail);

		INFO_PRINTF1(_L("Close RTP Session."));
		iServer->rtpSendPacket.Close();
		iServer->rtpRecvPacket.Close();
		iServer->rtpSendSrc.Close();
		iServer->rtpSession.Close();
		iServer->rConn.Close();
		iServer->sockServ.Close();
		delete iServer->sched;
		iServer->sched = 0;
		INFO_PRINTF1(_L(" RTP Session closed successful."));
		SetTestStepResult(EPass);

		return TestStepResult();
	}

TVerdict CrtpCloseSessionTwoStep::doTestStepL()
	{
		SetTestStepResult(EFail);
		INFO_PRINTF1(_L("Close RTP Session with subconnection."));
		iServer->rtpSendPacket.Close();
		iServer->rtpRecvPacket.Close();
		iServer->rtpSendSrc.Close();
		iServer->rtpSession.Close();
		iServer->rSubConn.Close();
		iServer->rConn.Close();
		iServer->sockServ.Close();
		delete iServer->sched;
		iServer->sched = 0;
		INFO_PRINTF1(_L("RTP Session with subconnection closed successful."));

		SetTestStepResult(EPass);

		return TestStepResult();
	}

TVerdict CrtpRegisterNonRtpDataStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Register NonRTP Data Step."));
	//Configure the reflector to NOT channge the SSRC and then
	//Register for Non RTP data callback.
	TInt remoteCmdPort;
	TPtrC remoteIP(0,0);
	TInetAddr remoteAddr;

	GetIntFromConfig(ConfigSection(),_L("RemoteCommandPort"), remoteCmdPort);
	GetStringFromConfig(ConfigSection(),_L("RemoteIP"), remoteIP);

	remoteAddr.SetPort(remoteCmdPort);
	remoteAddr.Input(remoteIP);
	
	iServer->ConfigureReflectorModeL(EFalse, remoteAddr);
	
	iServer->rtpSession.RegisterEventCallbackL(ENonRtpDataReceived, CTe_rtpSuite::NonRtpDataReceived, iServer);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CrtpSendNonRtpDataStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Send Non RTP Data Step..."));
	
	TBuf8<100> nonRtpData;
	nonRtpData.SetLength(100);
	nonRtpData.Fill('Z');
	
	TRequestStatus stat;
	iServer->rtpSession.SendDataL(ETrue,nonRtpData, stat);
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());
	/* Start the Active Scheduler and Wait for the Reflected data */
	CActiveScheduler::Start();
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CrtpReceiveNonRtpDataStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Send Non RTP Data Step..."));
	
	TBuf8<100> nonRtpData;
	nonRtpData.SetLength(100);
	nonRtpData.Fill('Z');
	
	TRequestStatus stat;
	const TDesC8& des = iServer->rtpSession.NonRtpDataL();
	if(0 != des.Compare(nonRtpData))
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

void CTimedStepRtp::InitTimerL(TThreadPriority aPriority)
	{
	iPeriodic = CPeriodic::New(aPriority);
	iTimerExpired = EFalse;
	}

TInt CTimedStepRtp::LocalTimerCallback(TAny *aPtr)
	{
	CTimedStepRtp *pThis = reinterpret_cast<CTimedStepRtp*>(aPtr);
	pThis->iTimerExpired = ETrue;
	CActiveScheduler::Stop();
	pThis->iPeriodic->Cancel();
	return KErrNone;
	}

void CTimedStepRtp::DeInitTimerL()
	{
	delete iPeriodic;
	iPeriodic = 0;
	}

/* Unregisters for NonRTP data and then sends a Non RTP data to reflector.
 * If the local timer timeout the packet is assumed to be lost
 */
TVerdict CrtpUnregisterNonRtpDataStep::doTestStepPreambleL()
	{
	SetTestStepResult(EFail);
    InitTimerL(EPriorityNormal);	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CrtpUnregisterNonRtpDataStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Unregister RTP Receive..."));
	
	iServer->rtpSession.DisableNonRtpData();
	
	/* Send Non-RTP data */
	TBuf8<100> nonRtpData;
	nonRtpData.SetLength(100);
	nonRtpData.Fill('Z');
	
	TRequestStatus stat;
	iServer->rtpSession.SendDataL(ETrue,nonRtpData, stat);
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());

	iPeriodic->Start(5000000, 500000, TCallBack(LocalTimerCallback,this));
	CActiveScheduler::Start();
	
	if(iTimerExpired)
		{
		/* The Packet was not received by stack */
		SetTestStepResult(EPass);
		}
	else
		{
		iPeriodic->Cancel();
		}
	return TestStepResult();
	}

TVerdict CrtpUnregisterNonRtpDataStep::doTestStepPostambleL()
	{
	DeInitTimerL();
	return TestStepResult();
	}

/* 
 * Registers for Non RTP data again
 */
TVerdict CrtpRegisterNonRtpAgainStep::doTestStepPreambleL()
	{
	SetTestStepResult(EFail);
    InitTimerL(EPriorityNormal);
    //Is this the only way to register again ??
    //The Test case will fail because the Callback wil be issued twice.
	iServer->rtpSession.RegisterEventCallbackL(ENonRtpDataReceived, CTe_rtpSuite::NonRtpDataReceived, iServer);
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CrtpRegisterNonRtpAgainStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Register for Receive again..."));
	
	/* Send Non-RTP data */
	TBuf8<100> nonRtpData;
	nonRtpData.SetLength(100);
	nonRtpData.Fill('Z');
	
	TRequestStatus stat;
	iServer->rtpSession.SendDataL(ETrue,nonRtpData, stat);
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());

	iPeriodic->Start(5000000, 500000, TCallBack(LocalTimerCallback,this));
	CActiveScheduler::Start();
	
	if(iTimerExpired)
		{
		/* The Packet was not received by stack. So RTP data
		 * was not reflected.*/
		INFO_PRINTF1(_L("Time out .. so Failing the test case.. Do Check the reflector!!!"));
		SetTestStepResult(EFail);
		}
	else
		{
		SetTestStepResult(EPass);
		iPeriodic->Cancel();
		}
	return TestStepResult();
	}

TVerdict CrtpRegisterNonRtpAgainStep::doTestStepPostambleL()
	{
	DeInitTimerL();
	return TestStepResult();
	}

TInt CrtpPreAndPostProcessingTestStep::PreRtpProcessing(TDes8& aRtpData)
	{
	/* Check for custom Footer */
	TPtrC8 ptrCustomData = aRtpData.Right(100);
	/* Attach our Custom Footer to this RTP Packet. */
	TBuf8<100> customData;
	customData.SetLength(100);
	customData.Fill('Z');
	
	if(0 == customData.Compare(ptrCustomData) && iPassCallBack)
		{
		iProcessingOK = ETrue;
		aRtpData.SetLength(aRtpData.Length()-100);
		}
	
	return iPassCallBack?KErrNone:KErrGeneral;
	}

void CrtpPreAndPostProcessingTestStep::PostRtpProcessing(TDes8& aRtpData)
	{
	/* Attach our Custom Footer to this RTP Packet. */
	TBuf8<100> customData;
	customData.SetLength(100);
	customData.Fill('Z');
	
	aRtpData.Append(customData);
	}

TInt CrtpPreAndPostProcessingTestStep::PreRtcpProcessing(TDes8&)
	{
	//No Handling done
	return KErrNone;
	}

void CrtpPreAndPostProcessingTestStep::PostRtcpProcessing(TDes8&)
	{
	//No handling done
    return;
	}
 
TVerdict CrtpPreAndPostProcessingTestStep::doTestStepPreambleL()
	{
	SetTestStepResult(EFail);
	iProcessingOK = EFalse;
	/* Create a send Source */
	iServer->rtpSendSrc = iServer->rtpSession.NewSendSourceL();
	iServer->rtpSendSrc.SetDefaultPayloadSize(1500); //sz = Payload + custom data
	iServer->rtpSendSrc.SetPayloadType(15); //15->G728 
	
	TInt remoteCmdPort;
	TPtrC remoteIP(0,0);
	TInetAddr remoteAddr;

	GetIntFromConfig(_L("ConfigOne"),_L("RemoteCommandPort"), remoteCmdPort);
	GetStringFromConfig(_L("ConfigOne"),_L("RemoteIP"), remoteIP);
	GetIntFromConfig(ConfigSection(),_L("PassData"),iPassCallBack);

	remoteAddr.SetPort(remoteCmdPort);
	remoteAddr.Input(remoteIP);

	iServer->ConfigureReflectorModeL(ETrue, remoteAddr);
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CrtpPreAndPostProcessingTestStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	iServer->rtpSendPacket = iServer->rtpSendSrc.NewSendPacketL();
	iServer->rtpSendSrc.RegisterEventCallbackL(ERtpSendSucceeded, CTe_rtpSuite::PacketSentSuccess, iServer);
	iServer->rtpSession.RegisterEventCallbackL(ERtpNewSource, CTe_rtpSuite::NewSourceL, iServer);
	iServer->rtpSession.SetRTPTimeConversion(0xFFA0, 8000);
	
	iServer->rtpSession.SetPrePostProcessingRegisterCallback(this);
	
	TDes8& rtpData = iServer->rtpSendPacket.WritePayload();
	rtpData.SetLength(500);
	rtpData.Fill('X');
	iServer->rtpSendPacket.SetPayloadType(97);
	iServer->rtpSendPacket.SetTimestamp(0x100);
	/* Lets test the SendSync() function as well */
	iServer->rtpSendPacket.SendSync();
	if(!iPassCallBack)
		{
		/* Data will be dropped by the stack */
		InitTimerL(EPriorityNormal);
		iPeriodic->Start(5000000, 500000, TCallBack(LocalTimerCallback,this));
		}
	CActiveScheduler::Start();
	
	if(iProcessingOK)
		{
		SetTestStepResult(EPass);
		}
	if(!iPassCallBack)
		{
		if(iTimerExpired)
			{
			SetTestStepResult(EPass);
			}
		DeInitTimerL();
		}
	return TestStepResult();
	}

/* Unregisters for NonRTP data and then sends a Non RTP data to reflector.
 * If the local timer timeout the packet is assumed to be lost
 */
TVerdict CrtpChangeRemoteAddrToLoopStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	
	/* Change the RTP remote address */
	TInetAddr locAddr;
	locAddr.SetAddress(INET_ADDR(127,0,0,1));
	
	TInt locDataPort;
	GetIntFromConfig(ConfigSection(),_L("LocalDataPort"), locDataPort);
	locAddr.SetPort(locDataPort);
	
	iServer->rtpSession.SetRemoteAddress(locAddr);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CrtpSetupLocalRecvSockStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	
	iServer->iDataSocket.Open(iServer->sockServ, KAfInet, KSockDatagram, KProtocolInetUdp);
	
	TInetAddr locAddr;
	locAddr.SetAddress(INET_ADDR(127,0,0,1));
	
	TInt locDataPort;
	GetIntFromConfig(ConfigSection(),_L("LocalDataPort"), locDataPort);
	locAddr.SetPort(locDataPort);
	
	User::LeaveIfError(iServer->iDataSocket.Bind(locAddr));
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CrtpRecvOnLocalSockStep::doTestStepL()
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

TVerdict CrtpSetAndGetOptStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	
	RSocket *rtpSock = iServer->rtpSession.RtpSocket();
	
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


TVerdict CrtpSetSamplingRateStep::doTestStepL()
	{
	
    /* Set the Sampling rate on the session */
	
	//Test1 = Setting a Dynamic Payload
	if(iServer->rtpSession.SetSamplingRate(97,90000)!= KErrNone)
		{
		SetTestStepResult(EFail);
		}

	//Test2 = Setting a Static payload Type ( Correct Value )
	//Ref: RFC 3551
	if(iServer->rtpSession.SetSamplingRate(5,8000)!= KErrNone)
		{
		SetTestStepResult(EFail);
		}
	
	//Test2 = Setting a Static payload Type ( InCorrect Value )
	//Ref: RFC 3551
	if(iServer->rtpSession.SetSamplingRate(1,8000)== KErrNone)
		{
		SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}

TVerdict CrtpCheckRecvPayloadStep::doTestStepL()
	{
 	SetTestStepResult(EPass);
	if(!iServer->iCallBackPass)
		{
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

	

