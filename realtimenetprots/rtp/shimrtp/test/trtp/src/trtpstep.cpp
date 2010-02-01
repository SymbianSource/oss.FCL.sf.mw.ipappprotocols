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
// class for the rtp unit test cases
// 
//

/**
 @file
 @internalComponent
*/

#include "trtpstep.h"
#include <test/testexecutelog.h>
#include "rtp.h"



#include <in_sock.h>
#include <commdbconnpref.h>

#include <cs_subconparams.h>
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
_LIT8(KRtpSessionCNAME, "Hello There!");
#include "rtp_subconparams.h"
#endif

TVerdict CTrtpStep::doTestStepL()
	{
	iServer->iCurrentStep = this;
	iServer->iStopping = EFalse;
	iServer->iStopCount = 1;

#if defined EKA2
		User::SetCritical(User::ESystemCritical);
#endif

	if (!iServer->iOom)
		return trtpDoTestStepL();
	else
		{
		TVerdict verdict = EFail;
		for (TInt oomCount = 0; ; oomCount++)
			{
			__UHEAP_RESET;
			__UHEAP_SETFAIL(RHeap::EDeterministic, oomCount);
			TInt countBefore = User::CountAllocCells();
			INFO_PRINTF2(_L("countbefore at %d"), countBefore);
			iServer->iStopping = EFalse;
			iServer->iStopCount = 1;
			iIgnoreIncrease = EFalse;
			iError = KErrNone;
			TRAPD(error, verdict = trtpDoTestStepL());
			TInt countAfter = User::CountAllocCells();
			INFO_PRINTF2(_L("countAfter at %d"), countAfter);
			__UHEAP_RESET;
			if (error != KErrNoMemory && iError != KErrNoMemory)
				{
				break;
				}
			else
				{
				if (countBefore != countAfter && !iIgnoreIncrease)
					{
					INFO_PRINTF2(_L("OOM Failed at %d"), oomCount);
					verdict = EFail;
					break;
					}
				}
			}
		SetTestStepResult(verdict);
		return verdict;
		}
	}

void CTrtpStep::GotEvent(TRtpEventType aType)
	{
	switch(aType)
		{
		case ERtpSDES:
			iGotSdes = ETrue;
			break;

		case ERtpAPP: 
			iGotApp = ETrue;
			break;

		case ERtpSR:
			iGotSr = ETrue;
			break;

		case ERtpRR:
			iGotRR = ETrue;
			break;

		case ERtpBYE:
			iGotBye = ETrue;
			break;

		default:
			break;
		}
	}

void CTrtpStep::ResetEvents()
	{
	iGotSdes = EFalse;	
	iGotApp = EFalse;
	iGotSr = EFalse;
	iGotRR = EFalse;
	iGotBye = EFalse;
	}

TVerdict CMakePacket::trtpDoTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini file section
 */
	{
	TInt bufSize;
	GetIntFromConfig(ConfigSection(),_L("BufferSize"),bufSize);
	INFO_PRINTF2(_L("Creating buffer of size %d"), bufSize);
	SetTestStepResult(EPass);
	// Check that RRtpSendPacket objects are created closed
	if (iServer->iSendPacket.IsOpen())
		SetTestStepResult(EFail);

	TPtrC config(0,0);
	TBool extension;
	TInt type = 0;
	TInt len = KRtpNoExtension;
	extension = GetStringFromConfig(ConfigSection(), _L("HeaderExtension"),config);
	TLex lex(config);
	if (extension)
		{		
		lex.Val(type);
		if (!lex.Eos() && lex.Peek() == ',')
			lex.Get();
		
		lex.Val(len);
		if (!lex.Eos() && lex.Peek() == ',')
			lex.Get();
		}

	iServer->iSendPacket = iServer->iSendSource.NewSendPacketL(bufSize, len);
	iServer->iPacket = &iServer->iSendPacket;

	if (extension)
		{
		// Just create the extension now; its existence is checked in
		// CExtension
		RRtpHeaderExtension extension = iServer->iSendPacket.Extension();
		extension.SetType((TUint16)type);
		TInt* array = const_cast<TInt*>(reinterpret_cast<const TInt*>(
											extension.Data().Ptr()));

		TInt i = 0;
		while (!lex.Eos() && i < len)
			{
			TInt val;
			lex.Val(val);

			array[i] = val;
			
			if (!lex.Eos() && lex.Peek() == ',')
				lex.Get();
			i++;
			}
		}


	if (!iServer->iPacket->IsOpen())
		SetTestStepResult(EFail);

	return TestStepResult();
	}



TVerdict CClosePacket::trtpDoTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini file section
 */
	{
	SetTestStepResult(EPass);
	iIgnoreIncrease = 1;
	iServer->iSendPacket.Close();
	if (iServer->iSendPacket.IsOpen())
	{
		SetTestStepResult(EFail);
	}
	iServer->iPacket = NULL;
	return TestStepResult();
	}



TVerdict CSetPayloadType::trtpDoTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini file section
 */
	{
	SetTestStepResult(EPass);

	TInt payloadType;
	GetIntFromConfig(ConfigSection(),_L("PayloadType"),payloadType);
	INFO_PRINTF2(_L("Setting payload type to %d"), payloadType);

	iServer->iSendPacket.SetPayloadType(payloadType);

	return TestStepResult();
	}

TVerdict CPayloadType::trtpDoTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini file section
 */
	{
	SetTestStepResult(EPass);

	TInt payloadType;
	GetIntFromConfig(ConfigSection(),_L("PayloadType"),payloadType);

	TInt gotPayloadType = iServer->iPacket->PayloadType();
	INFO_PRINTF3(_L("Payload type is %d should be %d"), gotPayloadType, payloadType);
	if (gotPayloadType != payloadType)
		SetTestStepResult(EFail);

	return TestStepResult();
	}

TVerdict CSetMarker::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TInt marker;
	GetIntFromConfig(ConfigSection(),_L("Marker"),marker);

	INFO_PRINTF2(_L("Setting marker to %d"), marker);

	iServer->iSendPacket.SetMarker(marker);

	return TestStepResult();
	}

TVerdict CMarker::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TInt marker;
	GetIntFromConfig(ConfigSection(),_L("Marker"),marker);

	TInt gotMarker = iServer->iPacket->Marker();
	INFO_PRINTF3(_L("Marker is %d should be %d"), gotMarker, marker);
	if (gotMarker != marker)
		SetTestStepResult(EFail);

	return TestStepResult();
	}

TVerdict CExtension::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TInt extension;
	GetIntFromConfig(ConfigSection(),_L("Extension"),extension);

	TInt gotExtension = iServer->iPacket->ExtensionPresent();
	INFO_PRINTF3(_L("Extension is %d should be %d"), gotExtension, extension);
	if (gotExtension != extension)
		SetTestStepResult(EFail);

	return TestStepResult();
	}

TVerdict CFlags::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TInt rawFlags; // big-endian from packet
	GetIntFromConfig(ConfigSection(),_L("Flags"),rawFlags);
	TUint32 flags = CTrtpServer::Swap32(static_cast<TUint32>(rawFlags));
	
	TUint32 gotFlags = iServer->iPacket->Flags();
	INFO_PRINTF3(_L("Flags is %d should be %d"), gotFlags, flags);
	if (gotFlags != flags)
		SetTestStepResult(EFail);

	return TestStepResult();
	}

TVerdict CSetFlags::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TInt rawFlags;
	GetIntFromConfig(ConfigSection(),_L("Flags"),rawFlags);
	TUint32 flags = CTrtpServer::Swap32(static_cast<TUint32>(rawFlags));

	INFO_PRINTF2(_L("Setting flags to %d"), flags);

	iServer->iSendPacket.SetFlags(flags);

	return TestStepResult();
	}

TVerdict CSequenceNumber::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TInt sequenceNumber;
	GetIntFromConfig(ConfigSection(),_L("SequenceNumber"),sequenceNumber);

	TInt gotSequenceNumber = iServer->iPacket->SequenceNumber();
	INFO_PRINTF3(_L("SequenceNumber is %d should be %d"), gotSequenceNumber, sequenceNumber);
	if (gotSequenceNumber != sequenceNumber)
		SetTestStepResult(EFail);

	return TestStepResult();
	}

TVerdict CTimestamp::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TInt timestamp;
	GetIntFromConfig(ConfigSection(),_L("Timestamp"),timestamp);

	TInt gotTimestamp = iServer->iPacket->Timestamp();
	INFO_PRINTF3(_L("Timestamp is %d should be %d"), gotTimestamp, timestamp);
	if (gotTimestamp != timestamp)
		SetTestStepResult(EFail);

	return TestStepResult();
	}

TVerdict CSSRC::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TInt SSRC;
	GetIntFromConfig(ConfigSection(),_L("SSRC"),SSRC);

	TInt gotSSRC = iServer->iPacket->SSRC();
	INFO_PRINTF3(_L("SSRC is %d should be %d"), gotSSRC, SSRC);
	if (gotSSRC != SSRC)
		SetTestStepResult(EFail);

	return TestStepResult();
	}

TVerdict CSetTimestamp::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TInt timestamp;
	GetIntFromConfig(ConfigSection(),_L("Timestamp"),timestamp);

	INFO_PRINTF2(_L("Setting timestamp to %d"), timestamp);

	iServer->iSendPacket.SetTimestamp(timestamp);

	return TestStepResult();
	}

TVerdict COpenSession::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);

	User::LeaveIfError(iServer->iFs.Connect());
	User::LeaveIfError(iServer->iSocketServer.Connect());
	iServer->iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iServer->iScheduler);
	return TestStepResult();
	}

TVerdict COpenSession::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);
	iIgnoreIncrease = 1;

	TInetAddr localAddress(KInetAddrLoop, 9000);
	TInetAddr remoteAddress(KInetAddrLoop, 9002);

	if (!iServer->iConnected)
		{
		iServer->iSession.OpenL(iServer->iSocketServer, localAddress, remoteAddress, 
								KDefaultMaxPcktSize);
		}
	else
		{
		TRequestStatus status;
		User::LeaveIfError(iServer->iRtpSocket.Open(iServer->iSocketServer, 
									   localAddress.Family(),
									   KSockDatagram, KProtocolInetUdp));
		User::LeaveIfError(iServer->iRtpSocket.Bind(localAddress));
		iServer->iRtpSocket.Connect(remoteAddress, status);
		User::WaitForRequest(status);
		iServer->iSession.OpenL(iServer->iRtpSocket, KDefaultMaxPcktSize);
		}
	CleanupClosePushL(iServer->iSession);

	// Check that send sources can be opened and closed
	iServer->iSendSource = iServer->iSession.NewSendSourceL();
	iServer->iSendSource.Close();

	iServer->iSendSource = iServer->iSession.NewSendSourceL();
	iServer->iSendSource.SetDefaultPayloadSize(KDefaultMaxPcktSize);
	CleanupStack::Pop();

	return TestStepResult();
	}

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
TVerdict COpenSession2::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);

	User::LeaveIfError(iServer->iFs.Connect());
	User::LeaveIfError(iServer->iSocketServer.Connect());
	iServer->iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iServer->iScheduler);
	return TestStepResult();
	}

TVerdict COpenSession2::trtpDoTestStepL()
	{
	
	SetTestStepResult(EPass);
	iIgnoreIncrease = 1;

	TInetAddr localAddress(KInetAddrLoop, 9000);
	TInetAddr remoteAddress(KInetAddrLoop, 9002);

	//RConnection iConnection;
	TRequestStatus status;
	User::LeaveIfError(iServer->iConnect.Open(iServer->iSocketServer));
	iServer->iConnect.Start(status);
	User::WaitForRequest(status);
	if(status.Int() != KErrNone)
	{
		SetTestStepResult(EFail);
		return TestStepResult();
	}
	
	// Create the container for all sub connection parameters
	RSubConParameterBundle subconParams;
	CleanupClosePushL(subconParams);
 
 	// Create a container for RTP sub connection parameters (Param bundle takes ownership)
	CSubConParameterFamily* subConFamily = CSubConParameterFamily::NewL(subconParams, KProtocolExtensionFamily);

 	// Create the requested generic parameter set for RTP (RTP family takes ownership)
	CSubConRTPGenericParamSet* reqGenericParams = CSubConRTPGenericParamSet::NewL(*subConFamily, CSubConParameterFamily::ERequested);
	
	
	TUid idRTP = TUid::Uid(KRtpFlowImplementationUid);
	// Set the requested Generic Parameters
	reqGenericParams->SetProtocolUIDRtp(idRTP);
	reqGenericParams->SetBandWidth(1234);
	reqGenericParams->SetRtptimeConversion(100);
	reqGenericParams->SetRtpAutoSend(EFalse);
	reqGenericParams->SetRtpTimeNow(0);
	reqGenericParams->SetMaxDropOut(3000);
	reqGenericParams->SetMinSequencial(3);
	reqGenericParams->SetMaxMisorder(100);
	reqGenericParams->SetServiceProtocolID(KProtocolInetUdp);
	reqGenericParams->SetLowerSubConnType(RSubConnection::EAttachToDefault);
	reqGenericParams->SetCNAMEL(KRtpSessionCNAME());
		
	TUid idTier = TUid::Uid(2048);
	reqGenericParams->SetTierID(idTier);
	
	User::LeaveIfError(iServer->iSubConnect.Open(iServer->iSocketServer,RSubConnection::ECreateNew,iServer->iConnect));
	
	// Set Properties of the sub-connection
	User::LeaveIfError(iServer->iSubConnect.SetParameters(subconParams));

	// Destroy parameters
	CleanupStack::PopAndDestroy();         // subconParams

	if (!iServer->iConnected)
		{
								
		iServer->iSession.OpenL(iServer->iSocketServer, localAddress, remoteAddress,
									KDefaultMaxPcktSize, iServer->iSubConnect,
									EPriorityNormal, KNullDesC8);
		}
/*	else
		{
		TRequestStatus status;
		User::LeaveIfError(iServer->iRtpSocket.Open(iServer->iSocketServer, 
									   localAddress.Family(),
									   KSockDatagram, KProtocolInetUdp));
									   
		User::LeaveIfError(iServer->iRtpSocket.Bind(localAddress));
		iServer->iRtpSocket.Connect(remoteAddress, status);
		User::WaitForRequest(status);
		iServer->iSession.OpenL(iServer->iSubConnect,iServer->iRtpSocket, KDefaultMaxPcktSize);
		}*/
		
	CleanupClosePushL(iServer->iSession);

	// Check that send sources can be opened and closed
	iServer->iSendSource = iServer->iSession.NewSendSourceL();
	iServer->iSendSource.Close();

	iServer->iSendSource = iServer->iSession.NewSendSourceL();
	iServer->iSendSource.SetDefaultPayloadSize(KDefaultMaxPcktSize);
	CleanupStack::Pop();

	return TestStepResult();
	}
	
TVerdict CCloseSession2::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	iServer->iSession.Close();
	iServer->iSubConnect.Close();
	iServer->iConnect.Close();
	
	iServer->iRtpSocket.Close();
	iServer->iRtcpSocket.Close();
	iServer->iFs.Close();
	iServer->iSocketServer.Close();
	CActiveScheduler::Install(NULL);
	delete iServer->iScheduler;
	iServer->iScheduler = 0;
	iServer->iSources.Close();
	iServer->iReceivePacket.Close();
	return TestStepResult();
	}
#endif

TVerdict CCloseSession::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	iServer->iSession.Close();
	iServer->iRtpSocket.Close();
	iServer->iRtcpSocket.Close();
	iServer->iFs.Close();
	iServer->iSocketServer.Close();
	CActiveScheduler::Install(NULL);
	delete iServer->iScheduler;
	iServer->iScheduler = 0;
	iServer->iSources.Close();
	iServer->iReceivePacket.Close();
	return TestStepResult();
	}
	



// Close everything except the session - Used to tidy up after close
// tests to check everything is closed. This closes everything except
// the session, so if this is called after a test that closed the
// session, then the session is re-opened, nothing is leaked.
TVerdict CTidyUp::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	iServer->iFs.Close();
	INFO_PRINTF1(_L("iFs.Close"));
	
	iServer->iReceivePacket.Close();
	INFO_PRINTF1(_L("iReceivePacket.Close"));

	INFO_PRINTF2(_L("iSources.Close %d sources"), iServer->iSources.Count());
	iServer->iSources.Close();
	INFO_PRINTF1(_L("iSources.Close"));

	//Dont do this, as this will cause problems, 
	//The sendsource is closed as a part of the session.
//	iServer->iSendSource.Close();
//	INFO_PRINTF1(_L("iSendSource.Close"));


	iServer->iSession.Close();
	INFO_PRINTF1(_L("iSession.Close"));

	iServer->iConnect.Close();
	INFO_PRINTF2(_L("iConnect.Close %d"), iServer->iConnect.SubSessionHandle());

	iServer->iSocketServer.Close();
	INFO_PRINTF1(_L("iSocketServer.Close"));

	CActiveScheduler::Install(NULL);
	delete iServer->iScheduler;
	iServer->iScheduler = 0;
	INFO_PRINTF1(_L("Deleted AS"));

	return TestStepResult();
	}
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
TVerdict CTidyUp2::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	iServer->iFs.Close();
	INFO_PRINTF1(_L("iFs.Close"));
	
	iServer->iReceivePacket.Close();
	INFO_PRINTF1(_L("iReceivePacket.Close"));

	INFO_PRINTF2(_L("iSources.Close %d sources"), iServer->iSources.Count());
	iServer->iSources.Close();
	INFO_PRINTF1(_L("iSources.Close"));

	//Dont do this, as this will cause problems, 
	//The sendsource is closed as a part of the session.
//	iServer->iSendSource.Close();
//	INFO_PRINTF1(_L("iSendSource.Close"));

	iServer->iSession.Close();
	
	iServer->iSubConnect.Close();
	
	INFO_PRINTF1(_L("iSession.Close"));

	iServer->iConnect.Close();
	INFO_PRINTF2(_L("iConnect.Close %d"), iServer->iConnect.SubSessionHandle());

	iServer->iSocketServer.Close();
	INFO_PRINTF1(_L("iSocketServer.Close"));

	CActiveScheduler::Install(NULL);
	delete iServer->iScheduler;
	iServer->iScheduler = 0;
	INFO_PRINTF1(_L("Deleted AS"));

	return TestStepResult();
	}
#endif

TVerdict CRxPacket::trtpDoTestStepL()
	{
	// some of the receive test are failing oom
	// as CRtpSSRCEntry gets allocated in rtpcore for every new SSRC 
	// and is kept beyond lifetime of this particular test (destroyed when session closed)
	// thus we have to ignore mem increase in some tests...
	TInt rxPacketIgnoreMemIncrease =1 ;
	if(!GetIntFromConfig(ConfigSection(),_L("RxPacketIgnoreMemIncrease"),rxPacketIgnoreMemIncrease))
		rxPacketIgnoreMemIncrease = 0;

	if(1 == rxPacketIgnoreMemIncrease)
		{
		// tels to TRtpserver to ignore increase for oom tests
		iIgnoreIncrease = ETrue;
		}

	SetTestStepResult(EPass);
	TRequestStatus status;

	// A quick slightly-related test. Check that a receive stream is
	// created closed and a closed receive stream can be closed.
	RRtpReceiveSource r;
	if (r.IsOpen())
		SetTestStepResult(EFail);
	r.Close();
	if (r.IsOpen())
		SetTestStepResult(EFail);	

	TPtrC filename(0,0);
	GetStringFromConfig(ConfigSection(),_L("PacketFile"),filename);
	RFile file;
	User::LeaveIfError(file.Open(iServer->iFs, filename, EFileRead));
	CleanupClosePushL(file);
	TInt size;
	User::LeaveIfError(file.Size(size));
	
	HBufC8* packet = HBufC8::NewMaxLC(size);
	TPtr8 pPacket(packet->Des());
	User::LeaveIfError(file.Read(pPacket));

	TInt port;
	if (iServer->iConnected || 
		!GetIntFromConfig(ConfigSection(), _L("SendPort"), port))
		port = 9002;
	TInetAddr remoteAddress(KInetAddrLoop, port);
	User::LeaveIfError(iServer->iSocket.Open(iServer->iSocketServer, KAfInet, 
									   KSockDatagram, KProtocolInetUdp));
	CleanupClosePushL(iServer->iSocket);
	User::LeaveIfError(iServer->iSocket.Bind(remoteAddress));

	TInetAddr localAddress(KInetAddrLoop, 9000);
	iServer->iSocket.SendTo(pPacket, localAddress, 0, status);
	// Waiting synchronously like this is probably OK
	User::WaitForRequest(status);
	CleanupStack::PopAndDestroy(3);// packet, file, iServer->iSocket
	
	// The scheduler will stop when an event is received.
	CActiveScheduler::Start();

	TInt streamNo;
	if (GetIntFromConfig(ConfigSection(), _L("SourceNo"), streamNo))
		{
		if (iServer->iSources.Count() > streamNo && iServer->iSources[streamNo] != iServer->iLastActiveSource)
			{
			TInt i;
			for (i = 0; i < iServer->iSources.Count(); i++)
				if (iServer->iLastActiveSource == iServer->iSources[i])
					break;

			SetTestStepResult(EFail);
			INFO_PRINTF3(_L("Source ID is wrong. Is %d should be %d"), i, streamNo);
			}
		else
			{
			INFO_PRINTF1(_L("Read apparently succeeded on correct stream"));
			}
		}

	TInt ssrc;
	if (GetIntFromConfig(ConfigSection(), _L("SSRC"), ssrc))
		{
		// If everything seems to have worked (there is a received
		// packet, there is a received source, there doesn't seem to
		// have been an error) then check the source SSRC is right.
		if (iError != KErrNoMemory && iServer->iLastActiveSource.IsOpen() && 
			iServer->iPacket->IsOpen() && 
			iServer->iLastActiveSource.SSRC() != static_cast<TUint>(ssrc))
			{
			SetTestStepResult(EFail);
			INFO_PRINTF3(_L("ssrc is wrong. Is %d should be %u"), 
						 iServer->iLastActiveSource.SSRC(), ssrc);
			}
		}
	

	iServer->iSocket.Close();
	return TestStepResult();
	}

TVerdict CRxPacket::doTestStepPostambleL()
	{
	iIgnoreIncrease = EFalse;	
	return TestStepResult();
	}

TVerdict CPayload::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TPtrC payload16(0,0);
	GetStringFromConfig(ConfigSection(),_L("Payld"),payload16);

	HBufC8* payload = HBufC8::NewMaxLC(payload16.Length());
	payload->Des().Copy(payload16);
	TPtrC8 gotPayload = iServer->iPacket->Payload();
	//INFO_PRINTF3(_L("payload is '%S' should be '%S'"), &gotPayload, payload);
	if (gotPayload != *payload)
		SetTestStepResult(EFail);

	CleanupStack::PopAndDestroy(payload);

	return TestStepResult();
	}

TVerdict CSetRxSize::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TInt rxSize;
	GetIntFromConfig(ConfigSection(),_L("RxSize"),rxSize);

	INFO_PRINTF2(_L("Setting rx size to %d"), rxSize);

	iServer->iSession.SetMaxRXSize(rxSize);

	if (iServer->iSession.MaxRXSize() != rxSize)
		SetTestStepResult(EFail);

	return TestStepResult();
	}

TVerdict CCheckEvent::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TInt event;
	GetIntFromConfig(ConfigSection(),_L("Event"),event);

	INFO_PRINTF3(_L("Event is %d should be %d\n"),iServer->iLastEvent.Type(),event);

	if (iServer->iLastEvent.Type() != event)
		SetTestStepResult(EFail);

	return TestStepResult();
	}

TVerdict CEvent::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TInt event;
	GetIntFromConfig(ConfigSection(),_L("Event"),event);

	TInt param = KRtpNoParameter;
	GetIntFromConfig(ConfigSection(),_L("Param"),param);

	TBool oneShot = EFalse;
	GetBoolFromConfig(ConfigSection(),_L("OneShot"),oneShot);

	TBool sendSource = EFalse;
	GetBoolFromConfig(ConfigSection(),_L("SendSource"),sendSource);

	TPtrC function(0,0);
	GetStringFromConfig(ConfigSection(),_L("Function"),function);
	
	void (*fn)(CTrtpServer*, const TRtpEvent&) = 0;
	if (function == _L("StopScheduler"))
		fn = CTrtpServer::StopScheduler;
	if (function == _L("NewSource"))
		fn = CTrtpServer::NewSourceL;
	if (function == _L("Panic"))
		fn = CTrtpServer::Panic;
	if (function == _L("Leave"))
		fn = CTrtpServer::LeaveL;
	if (function == _L("NewSourceLeave"))
		fn = CTrtpServer::NewSourceLeaveL;
	if (function == _L("NotifyError"))
		fn = CTrtpServer::NotifyError;
	if (function == _L("CloseSession"))
		fn = CTrtpServer::CloseSession;
	if (function == _L("NewSourceCloseSession"))
		fn = CTrtpServer::NewSourceCloseSessionL;
	if (function == _L("NewSourceDelayedCloseSession"))
		fn = CTrtpServer::NewSourceDelayedCloseSessionL;
	if (function == _L("NewSourceLeaveClose"))
		fn = CTrtpServer::NewSourceLeaveCloseL;
	if (function == _L("LeaveAfterClose"))
		fn = CTrtpServer::LeaveAfterCloseL;
	if (function == _L("CloseSourceFromSource"))
		fn = CTrtpServer::CloseSourceFromSourceL;
	if (function == _L("CloseSourceFromSourceLeave"))
		fn = CTrtpServer::CloseSourceFromSourceLeaveL;
	if (function == _L("CloseSendSource"))
		fn = CTrtpServer::CloseSendSource;
	if (function == _L("NoteEventStatus"))
		fn = CTrtpServer::NoteEventStatus;

	if (function == _L("NewSrcForRtcp"))
		fn = CTrtpServer::NewSrcForRtcpL;
	if (function == _L("ERtpSDES"))
		fn = CTrtpServer::CbERtpSDES;
	if (function == _L("ERtpRR"))
		fn = CTrtpServer::CbERtpRR;	
	
	INFO_PRINTF4(_L("Adding session event for %d param %d one-shotness %d\n"),
				 event, param, oneShot);

	if (!sendSource)
		{
		iServer->iSession.RegisterEventCallbackL(static_cast<TRtpEventType>(event),
												 fn, iServer, 
												 oneShot? ERtpOneShot:ERtpNotOneShot, 
												 param);
		}
	else
		{
		iServer->iSendSource.RegisterEventCallbackL(static_cast<TRtpEventType>(event),
												 fn, iServer, oneShot? ERtpOneShot:ERtpNotOneShot, param);
		}

	return TestStepResult();
	}

TVerdict COom::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TBool oom;
	GetBoolFromConfig(ConfigSection(),_L("Oom"),oom);

	INFO_PRINTF2(_L("Setting OOM mode to %d"), oom);

	iServer->iOom = oom;

	return TestStepResult();
	}

TVerdict CCSRCs::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TPtrC config(0,0);
	TBool gotConfig = GetStringFromConfig(ConfigSection(),_L("CSRC"),config);

	if (!gotConfig)
		{
		// No CSRCs defined. Check there aren't any.
		RRtpCSRCs c = iServer->iPacket->CSRCs();
		if (c.Count() != 0)
			{
			INFO_PRINTF2(_L("There are %d CSRCs. Should be 0"), c.Count());
			SetTestStepResult(EFail);
			}
		}
	else
		{
		RRtpCSRCs c = iServer->iPacket->CSRCs();
		TLex lex(config);
		TInt i = 0;
		while (!lex.Eos())
			{
			TInt val;
			lex.Val(val);
			if (c.Count() <= i)
				{
				INFO_PRINTF2(_L("There are %d CSRCs. Should be more"), 
							 c.Count());
				SetTestStepResult(EFail);
				}
			else if (c[i] != (TUint)val)
				{
				INFO_PRINTF3(_L("CSRC is %d should be %d"), c[i], val);
				SetTestStepResult(EFail);
				}
			if (!lex.Eos() && lex.Peek() == ',')
				lex.Get();
			i++;
			}
		if (i != c.Count())
			{
			INFO_PRINTF3(_L("There are %d CSRCs. Should be %d"), c.Count(), i);
			SetTestStepResult(EFail);
			}
		}

	return TestStepResult();
	}

TVerdict CExtensionContent::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TPtrC config(0,0);
	GetStringFromConfig(ConfigSection(), _L("HeaderExtension"),config);

	RRtpHeaderExtension e = iServer->iPacket->Extension();
	
	TLex lex(config);
	TInt type;
	lex.Val(type);
	if (!lex.Eos() && lex.Peek() == ',')
		lex.Get();
	if (e.Type() != type)
		{
		INFO_PRINTF3(_L("Extension type is %d should be %d"), e.Type(), type);
		SetTestStepResult(EFail);		
		}
	
	TPtrC8 extension = e.Data();

	TInt len;
	lex.Val(len);
	if (!lex.Eos() && lex.Peek() == ',')
		lex.Get();
	if (extension.Length() != len * 4)
		{
		INFO_PRINTF3(_L("Extension length is %d should be %d"), 
					 extension.Length(), len * 4);
		SetTestStepResult(EFail);		
		}

	TInt i = 0;
	while (!lex.Eos() && i < len)
		{
		TInt val;
		lex.Val(val);

		if (((TInt*)extension.Ptr())[i] != val)
			{
			INFO_PRINTF3(_L("Extension word is %d should be %d"),
						 (TInt*)extension.Ptr()[i] , val);
			SetTestStepResult(EFail);
			}
		if (!lex.Eos() && lex.Peek() == ',')
			lex.Get();
		i++;
		}

	return TestStepResult();
	}

TVerdict CSend::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	if (iServer->iSendSource.IsSending())
		SetTestStepResult(EFail);

	TBuf8<KDefaultMaxPcktSize> buf; // To put the packet in
	
	// A number of different stop events need to happen before the
	// scheduler stops. Typically, these are the sent packet being
	// successfuly sent and the sent packet being received.
	TInt stopCount;
	if (!GetIntFromConfig(ConfigSection(), _L("StopCount"), stopCount))
		stopCount = 2;

	TInt port;
	if (!GetIntFromConfig(ConfigSection(), _L("SendPort"), port))
		port = 9002;

	// Create a socket to listen for for the sent packet on.
	TInetAddr remoteAddress(KInetAddrLoop, port);
	User::LeaveIfError(iServer->iSocket.Open(iServer->iSocketServer, KAfInet, 
									   KSockDatagram, KProtocolInetUdp));
	CleanupClosePushL(iServer->iSocket);
	User::LeaveIfError(iServer->iSocket.Bind(remoteAddress));

	// Create an object to stop the active scheduler when the packet
	// is received
	TSockAddr addr;
	CWaiter* waiter = CWaiter::NewLC(iServer);
	iServer->iSocket.RecvFrom(buf,addr, 0,waiter->iStatus);
	
	iServer->iStopCount = stopCount;
	iServer->iSendPacket.Send();
	waiter->Ready();
	if (!iServer->iSendSource.IsSending())
		SetTestStepResult(EFail);
	CActiveScheduler::Start();

	if (stopCount == 1)
		{
		// The waiter probably hasn't finished (i.e. the packet has
		// been sent but not received). Cancel the read
		iServer->iSocket.CancelRecv();
		iServer->iStopCount = 1;
		iServer->iStopping = 0;
		CActiveScheduler::Start();
		}
	CleanupStack::PopAndDestroy(2); // socket, waiter

	TBool check = EFalse;
	if (GetBoolFromConfig(ConfigSection(), _L("Check"), check) && 
		check == EFalse)
		{
		iServer->iSendPacket.Close();
		return TestStepResult();
		}

	// Now check that the packet has the correct value
	TPtrC filename(0,0);
	GetStringFromConfig(ConfigSection(),_L("PacketFile"),filename);
	RFile file;
	User::LeaveIfError(file.Open(iServer->iFs, filename, EFileRead));
	CleanupClosePushL(file);
	TInt size;
	User::LeaveIfError(file.Size(size));
	HBufC8* packet = HBufC8::NewMaxLC(size);
	TPtr8 pPacket(packet->Des());
	User::LeaveIfError(file.Read(pPacket));
	for (TInt i = 0; i < packet->Length(); i++)
		{
		// Don't compare the SSRC or timestamp as it's random.
		if ((i < 8 || i > 11) && (i != 2 && i != 3))
			if ((*packet)[i] != buf[i])
				SetTestStepResult(EFail);
				
		}
	CleanupStack::PopAndDestroy(2);
	iServer->iSendPacket.Close();

	return TestStepResult();
	}

TVerdict CSetPayload::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TPtrC payload16(0,0);
	GetStringFromConfig(ConfigSection(),_L("Payld"),payload16);
	
	HBufC8* payload = HBufC8::NewMaxLC(payload16.Length());
	payload->Des().Copy(payload16);
	iServer->iSendPacket.WritePayload().Copy(*payload);

	if (iServer->iPacket->Payload() != *payload)
		SetTestStepResult(EFail);
	
	CleanupStack::PopAndDestroy(payload);
	
	return TestStepResult();
	}

TVerdict CSetAlignment::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TInt alignment;
	GetIntFromConfig(ConfigSection(),_L("Alignment"),alignment);
	
	iServer->iSendSource.SetAlignment(alignment);

	if (iServer->iSendSource.Alignment() != alignment)
		SetTestStepResult(EFail);
	
	return TestStepResult();
	}

TVerdict CSetStreamPayloadType::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TInt payloadType;
	GetIntFromConfig(ConfigSection(),_L("PayloadType"),payloadType);
	INFO_PRINTF2(_L("Setting payload type to %d"), payloadType);

	iServer->iSendSource.SetPayloadType(payloadType);

	return TestStepResult();
	}

TVerdict CCheckStatus::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TInt status;
	GetIntFromConfig(ConfigSection(),_L("Status"),status);
	if (iServer->iEventStatus != status)
		SetTestStepResult(EFail);

	return TestStepResult();
	}

TVerdict CWorkConnected::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);

	TBool connected = EFalse;
	GetBoolFromConfig(ConfigSection(),_L("Connectedness"),connected);

	iServer->iConnected = connected;
	
	return TestStepResult();
	}

TVerdict COpenSessionRConnect::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);

	User::LeaveIfError(iServer->iFs.Connect());
	User::LeaveIfError(iServer->iSocketServer.Connect());
	iServer->iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iServer->iScheduler);
	return TestStepResult();
	}

TVerdict COpenSessionRConnect::trtpDoTestStepL()
	{

	SetTestStepResult(EPass);
	iIgnoreIncrease = 1;

	TInetAddr localAddress(KInetAddrLoop, 9000);
	TInetAddr remoteAddress(KInetAddrLoop, 9002);
	TRequestStatus status;
	TCommDbConnPref prefs;
	prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	
	iServer->iConnect.Close();
	User::LeaveIfError(iServer->iConnect.Open(iServer->iSocketServer, KConnectionTypeDefault));
	iServer->iConnect.Start(prefs, status);
	User::WaitForRequest(status);
	if(status.Int() != KErrNone)
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	if (!iServer->iConnected)
		{
		iServer->iSession.OpenL(iServer->iSocketServer, localAddress, remoteAddress, 
								KDefaultMaxPcktSize, iServer->iConnect);
		}
	else
		{
		TRequestStatus status;
		User::LeaveIfError(iServer->iRtpSocket.Open(iServer->iSocketServer, 
									   localAddress.Family(),
									   KSockDatagram, KProtocolInetUdp, iServer->iConnect));
		User::LeaveIfError(iServer->iRtpSocket.Bind(localAddress));
		iServer->iRtpSocket.Connect(remoteAddress, status);
		User::WaitForRequest(status);
		iServer->iSession.OpenL(iServer->iRtpSocket, KDefaultMaxPcktSize);
		}

	return TestStepResult();
	}
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
TVerdict COpenSessionRConnect2::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);

	User::LeaveIfError(iServer->iFs.Connect());
	User::LeaveIfError(iServer->iSocketServer.Connect());
	iServer->iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iServer->iScheduler);
	return TestStepResult();
	}	
TVerdict COpenSessionRConnect2::trtpDoTestStepL()
	{

	SetTestStepResult(EPass);
	iIgnoreIncrease = 1;

	TInetAddr localAddress(KInetAddrLoop, 9000);
	TInetAddr remoteAddress(KInetAddrLoop, 9002);
	TRequestStatus status;
	TCommDbConnPref prefs;
	prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	
	iServer->iConnect.Close();
	User::LeaveIfError(iServer->iConnect.Open(iServer->iSocketServer, KConnectionTypeDefault));
	iServer->iConnect.Start(prefs, status);
	User::WaitForRequest(status);
	if(status.Int() != KErrNone)
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
		// Create the container for all sub connection parameters
	RSubConParameterBundle subconParams;
	CleanupClosePushL(subconParams);
 
 	// Create a container for RTP sub connection parameters (Param bundle takes ownership)
	CSubConParameterFamily* subConFamily = CSubConParameterFamily::NewL(subconParams, KProtocolExtensionFamily);

 	// Create the requested generic parameter set for RTP (RTP family takes ownership)
	CSubConRTPGenericParamSet* reqGenericParams = CSubConRTPGenericParamSet::NewL(*subConFamily, CSubConParameterFamily::ERequested);
	
	
	TUid idRTP = TUid::Uid(KRtpFlowImplementationUid);
	// Set the requested Generic Parameters
//	reqGenericParams->SetProtocolIDRtp(500);
	reqGenericParams->SetProtocolUIDRtp(idRTP);
	reqGenericParams->SetBandWidth(1234);
	reqGenericParams->SetRtptimeConversion(100);
	reqGenericParams->SetRtpAutoSend(EFalse);
	reqGenericParams->SetRtpTimeNow(0);
	
	reqGenericParams->SetServiceProtocolID(KProtocolInetUdp);
	reqGenericParams->SetLowerSubConnType(RSubConnection::EAttachToDefault);
	reqGenericParams->SetCNAMEL(KRtpSessionCNAME());
	
	TUid idTier = TUid::Uid(2048);
	reqGenericParams->SetTierID(idTier);
	
	User::LeaveIfError(iServer->iSubConnect.Open(iServer->iSocketServer,RSubConnection::ECreateNew,iServer->iConnect));
	
	// Set Properties of the sub-connection
	User::LeaveIfError(iServer->iSubConnect.SetParameters(subconParams));

	// Destroy parameters
	CleanupStack::PopAndDestroy();         // subconParams
	

	if (!iServer->iConnected)
		{
		iServer->iSession.OpenL(iServer->iSocketServer, localAddress, remoteAddress,
									KDefaultMaxPcktSize, iServer->iSubConnect,
									EPriorityNormal, KNullDesC8);
	
		}
/*	else
		{
		TRequestStatus status;
		User::LeaveIfError(iServer->iRtpSocket.Open(iServer->iSocketServer, 
									   localAddress.Family(),
									   KSockDatagram, KProtocolInetUdp, iServer->iConnect));
		User::LeaveIfError(iServer->iRtpSocket.Bind(localAddress));
		iServer->iRtpSocket.Connect(remoteAddress, status);
		User::WaitForRequest(status);
		iServer->iSession.OpenL(iServer->iSubConnect,iServer->iRtpSocket, KDefaultMaxPcktSize);

		}*/

	return TestStepResult();
	}
#endif

TVerdict COpenSessionRtcp::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	
	User::LeaveIfError(iServer->iFs.Connect());
	User::LeaveIfError(iServer->iSocketServer.Connect());
	iServer->iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iServer->iScheduler);
	return TestStepResult();
	}

TVerdict COpenSessionRtcp::trtpDoTestStepL()
	{

	SetTestStepResult(EPass);
	iIgnoreIncrease = 1;

	TInetAddr localAddress(KInetAddrLoop, 9000);
	TInetAddr localRtcpAddress(KInetAddrLoop, 9001);

	TInetAddr remoteAddress(KInetAddrLoop, 9002);
	TInetAddr remoteRtcpAddress(KInetAddrLoop, 9003);

//	TRequestStatus status;
	TCommDbConnPref prefs;
	prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);

	_LIT8(KCName, "CNameNull");
	if (!iServer->iConnected)
		{
		
		
		iServer->iSession.OpenL(iServer->iSocketServer, localAddress, remoteAddress, 
								KDefaultMaxPcktSize, KDefaultPriority, KCName);
								
			
		
	//	iServer->iSession.OpenL(iServer->iSocketServer, localAddress, remoteAddress, 
	//							KDefaultMaxPcktSize);
		}
	else
		{
		TRequestStatus status;
		User::LeaveIfError(iServer->iRtpSocket.Open(iServer->iSocketServer, 
									   localAddress.Family(),
									   KSockDatagram, KProtocolInetUdp));
		User::LeaveIfError(iServer->iRtpSocket.Bind(localAddress));
		iServer->iRtpSocket.Connect(remoteAddress, status);
		User::WaitForRequest(status);

		User::LeaveIfError(iServer->iRtcpSocket.Open(iServer->iSocketServer, 
									   localRtcpAddress.Family(),
									   KSockDatagram, KProtocolInetUdp));
		User::LeaveIfError(iServer->iRtcpSocket.Bind(localRtcpAddress));
		iServer->iRtcpSocket.Connect(remoteRtcpAddress, status);
		User::WaitForRequest(status);

		iServer->iSession.OpenL(iServer->iRtpSocket, KDefaultMaxPcktSize, iServer->iRtcpSocket, KDefaultPriority, KCName);
		}

	iServer->iSession.SetRTPTimeConversion(100, 100);

	CleanupClosePushL(iServer->iSession);

	// Check that send sources can be opened and closed
	iServer->iSendSource = iServer->iSession.NewSendSourceL();
	iServer->iSendSource.Close();

	iServer->iSendSource = iServer->iSession.NewSendSourceL();
	iServer->iSendSource.SetDefaultPayloadSize(KDefaultMaxPcktSize);
	CleanupStack::Pop();

	return TestStepResult();
	}
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
TVerdict COpenSessionRtcp2::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	
	User::LeaveIfError(iServer->iFs.Connect());
	User::LeaveIfError(iServer->iSocketServer.Connect());
	iServer->iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iServer->iScheduler);
	return TestStepResult();
	}

TVerdict COpenSessionRtcp2::trtpDoTestStepL()
	{

	SetTestStepResult(EPass);
	iIgnoreIncrease = 1;

	TRequestStatus status;
	
	//RConnection iConnection;
	User::LeaveIfError(iServer->iConnect.Open(iServer->iSocketServer));
	iServer->iConnect.Start(status);
	User::WaitForRequest(status);
	if(status.Int() != KErrNone)
	{
		SetTestStepResult(EFail);
		return TestStepResult();
	}
	
	// Create the container for all sub connection parameters
	RSubConParameterBundle subconParams;
	CleanupClosePushL(subconParams);
 
 	// Create a container for RTP sub connection parameters (Param bundle takes ownership)
	CSubConParameterFamily* subConFamily = CSubConParameterFamily::NewL(subconParams, KProtocolExtensionFamily);

 	// Create the requested generic parameter set for RTP (RTP family takes ownership)
	CSubConRTPGenericParamSet* reqGenericParams = CSubConRTPGenericParamSet::NewL(*subConFamily, CSubConParameterFamily::ERequested);
	
	
	TUid idRTP = TUid::Uid(KRtpFlowImplementationUid);
	// Set the requested Generic Parameters
//	reqGenericParams->SetProtocolIDRtp(500);
	reqGenericParams->SetProtocolUIDRtp(idRTP);
	reqGenericParams->SetBandWidth(1234);
	reqGenericParams->SetRtptimeConversion(100);
	reqGenericParams->SetRtpAutoSend(EFalse);
	reqGenericParams->SetRtpTimeNow(0);
	
	reqGenericParams->SetServiceProtocolID(KProtocolInetUdp);
	reqGenericParams->SetLowerSubConnType(RSubConnection::EAttachToDefault);
	reqGenericParams->SetCNAMEL(KRtpSessionCNAME());
	
	TUid idTier = TUid::Uid(2048);
	reqGenericParams->SetTierID(idTier);
	
	User::LeaveIfError(iServer->iSubConnect.Open(iServer->iSocketServer,RSubConnection::ECreateNew,iServer->iConnect));
	
	// Set Properties of the sub-connection
	User::LeaveIfError(iServer->iSubConnect.SetParameters(subconParams));

	// Destroy parameters
	CleanupStack::PopAndDestroy();         // subconParams

	TInetAddr localAddress(KInetAddrLoop, 9000);
	TInetAddr localRtcpAddress(KInetAddrLoop, 9001);

	TInetAddr remoteAddress(KInetAddrLoop, 9002);
	TInetAddr remoteRtcpAddress(KInetAddrLoop, 9003);

	TCommDbConnPref prefs;
	prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);

	//For removing warning 11/06/2008
	//_LIT8(KCName, "CNameNull");
	if (!iServer->iConnected)
		{
		
		
		//iServer->iSession.OpenL(iServer->iSocketServer, localAddress, remoteAddress, 
		//						KDefaultMaxPcktSize, KDefaultPriority, KCName);
								
		iServer->iSession.OpenL(iServer->iSocketServer,	localAddress, remoteAddress,
									KDefaultMaxPcktSize,iServer->iSubConnect,
									EPriorityNormal,KRtpSessionCNAME);
			
		}
/*	else
		{
		TRequestStatus status;
		User::LeaveIfError(iServer->iRtpSocket.Open(iServer->iSocketServer, 
									   localAddress.Family(),
									   KSockDatagram, KProtocolInetUdp));
		User::LeaveIfError(iServer->iRtpSocket.Bind(localAddress));
		iServer->iRtpSocket.Connect(remoteAddress, status);
		User::WaitForRequest(status);

		User::LeaveIfError(iServer->iRtcpSocket.Open(iServer->iSocketServer, 
									   localRtcpAddress.Family(),
									   KSockDatagram, KProtocolInetUdp));
		User::LeaveIfError(iServer->iRtcpSocket.Bind(localRtcpAddress));
		iServer->iRtcpSocket.Connect(remoteRtcpAddress, status);
		User::WaitForRequest(status);

		iServer->iSession.OpenL(iServer->iSubConnect,iServer->iRtpSocket, KDefaultMaxPcktSize, iServer->iRtcpSocket, KDefaultPriority, KRtpSessionCNAME);
		
		}*/

	iServer->iSession.SetRTPTimeConversion(100, 100);

	CleanupClosePushL(iServer->iSession);

	// Check that send sources can be opened and closed
	iServer->iSendSource = iServer->iSession.NewSendSourceL();
	iServer->iSendSource.Close();

	iServer->iSendSource = iServer->iSession.NewSendSourceL();
	iServer->iSendSource.SetDefaultPayloadSize(KDefaultMaxPcktSize);
	CleanupStack::Pop();

	return TestStepResult();
	}
#endif


// Test Openning a session with control when RTP removed

TVerdict COpenSessionRtcpRemoved::trtpDoTestStepL()
	{

	SetTestStepResult(EPass);
	iIgnoreIncrease = 1;

	TInetAddr localAddress(KInetAddrLoop, 9000);
	TInetAddr localRtcpAddress(KInetAddrLoop, 9001);

	TInetAddr remoteAddress(KInetAddrLoop, 9002);
	TInetAddr remoteRtcpAddress(KInetAddrLoop, 9003);
	
	_LIT8(KCName, "CNameNull");
	
	TInt err(KErrNone);
	if (!iServer->iConnected)
		{
		TRAP(err,iServer->iSession.OpenL(iServer->iSocketServer, localAddress, remoteAddress,KDefaultMaxPcktSize, KDefaultPriority, KCName));															
		if (err!=KErrNotSupported)
			{
			SetTestStepResult(EFail);
			}
		else
			{	
			return TestStepResult();
			}							
		}
	else
		{
		TRequestStatus status;
		User::LeaveIfError(iServer->iRtpSocket.Open(iServer->iSocketServer,localAddress.Family(),KSockDatagram, KProtocolInetUdp));
		User::LeaveIfError(iServer->iRtpSocket.Bind(localAddress));
		User::WaitForRequest(status);
		User::LeaveIfError(iServer->iRtcpSocket.Open(iServer->iSocketServer,localRtcpAddress.Family(),KSockDatagram, KProtocolInetUdp));
		User::LeaveIfError(iServer->iRtcpSocket.Bind(localRtcpAddress));
		User::WaitForRequest(status);

		TRAP(err,iServer->iSession.OpenL(iServer->iRtpSocket, KDefaultMaxPcktSize, iServer->iRtcpSocket, KDefaultPriority, KCName));
		if (err!=KErrNotSupported)
			{
			SetTestStepResult(EFail);
			}
		}

	INFO_PRINTF2(_L("RTP not openned due to  %d"),err);
	
	return TestStepResult();	
	}
// Test Openning a session with control when RTP removed
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
TVerdict COpenSessionRtcpRemoved2::trtpDoTestStepL()
	{

	SetTestStepResult(EPass);
	iIgnoreIncrease = 1;
	
	TRequestStatus status;
	
	//RConnection iConnection;
	User::LeaveIfError(iServer->iConnect.Open(iServer->iSocketServer));
	iServer->iConnect.Start(status);
	User::WaitForRequest(status);
	if(status.Int() != KErrNone)
	{
		SetTestStepResult(EFail);
		return TestStepResult();
	}
	
	// Create the container for all sub connection parameters
	RSubConParameterBundle subconParams;
	CleanupClosePushL(subconParams);
 
 	// Create a container for RTP sub connection parameters (Param bundle takes ownership)
	CSubConParameterFamily* subConFamily = CSubConParameterFamily::NewL(subconParams, KProtocolExtensionFamily);

 	// Create the requested generic parameter set for RTP (RTP family takes ownership)
	CSubConRTPGenericParamSet* reqGenericParams = CSubConRTPGenericParamSet::NewL(*subConFamily, CSubConParameterFamily::ERequested);
	
	
	TUid idRTP = TUid::Uid(KRtpFlowImplementationUid);
	// Set the requested Generic Parameters
//	reqGenericParams->SetProtocolIDRtp(500);
	reqGenericParams->SetProtocolUIDRtp(idRTP);
	reqGenericParams->SetBandWidth(1234);
	reqGenericParams->SetRtptimeConversion(100);
	reqGenericParams->SetRtpAutoSend(EFalse);
	reqGenericParams->SetRtpTimeNow(0);
	
	reqGenericParams->SetServiceProtocolID(KProtocolInetUdp);
	reqGenericParams->SetLowerSubConnType(RSubConnection::EAttachToDefault);
	reqGenericParams->SetCNAMEL(KRtpSessionCNAME());
	
	TUid idTier = TUid::Uid(2048);
	reqGenericParams->SetTierID(idTier);
	
	User::LeaveIfError(iServer->iSubConnect.Open(iServer->iSocketServer,RSubConnection::ECreateNew,iServer->iConnect));
	
	// Set Properties of the sub-connection
	User::LeaveIfError(iServer->iSubConnect.SetParameters(subconParams));

	// Destroy parameters
	CleanupStack::PopAndDestroy();         // subconParams


	TInetAddr localAddress(KInetAddrLoop, 9000);
	TInetAddr localRtcpAddress(KInetAddrLoop, 9001);

	TInetAddr remoteAddress(KInetAddrLoop, 9002);
	TInetAddr remoteRtcpAddress(KInetAddrLoop, 9003);
	
	//for removing warning
	//_LIT8(KCName, "CNameNull");
	
	TInt err(KErrNone);
	if (!iServer->iConnected)
		{
		TRAP(err,iServer->iSession.OpenL(iServer->iSocketServer, localAddress, remoteAddress,KDefaultMaxPcktSize, iServer->iSubConnect,KDefaultPriority, KRtpSessionCNAME));															
		
		if (err!=KErrNotSupported)
			{
			SetTestStepResult(EFail);
			}
		else
			{	
			return TestStepResult();
			}							
		}
/*	else
		{
		TRequestStatus status;
		User::LeaveIfError(iServer->iRtpSocket.Open(iServer->iSocketServer,localAddress.Family(),KSockDatagram, KProtocolInetUdp));
		User::LeaveIfError(iServer->iRtpSocket.Bind(localAddress));
		User::WaitForRequest(status);
		User::LeaveIfError(iServer->iRtcpSocket.Open(iServer->iSocketServer,localRtcpAddress.Family(),KSockDatagram, KProtocolInetUdp));
		User::LeaveIfError(iServer->iRtcpSocket.Bind(localRtcpAddress));
		User::WaitForRequest(status);
	
		TRAP(err,iServer->iSession.OpenL(iServer->iSubConnect,iServer->iRtpSocket, KDefaultMaxPcktSize, iServer->iRtcpSocket, KDefaultPriority, KRtpSessionCNAME));
		if (err!=KErrNotSupported)
			{
			SetTestStepResult(EFail);
			}
		}
	*/

	INFO_PRINTF2(_L("RTP not openned due to  %d"),err);
	
	return TestStepResult();	
	}
#endif
TVerdict COpenSessionRemoved::trtpDoTestStepL()
	{
	SetTestStepResult(EPass);
	iIgnoreIncrease = 1;

	TInetAddr localAddress(KInetAddrLoop, 9000);
	TInetAddr remoteAddress(KInetAddrLoop, 9002);

	TInt err(KErrNone);
	
	if (!iServer->iConnected)
		{
		TRAP(err,iServer->iSession.OpenL(iServer->iSocketServer, localAddress, remoteAddress,KDefaultMaxPcktSize));
		if (err!=KErrNotSupported)
			{
			SetTestStepResult(EFail);
			}
		}
	else
		{
		TRequestStatus status;
		User::LeaveIfError(iServer->iRtpSocket.Open(iServer->iSocketServer,localAddress.Family(),KSockDatagram, KProtocolInetUdp));
		User::LeaveIfError(iServer->iRtpSocket.Bind(localAddress));
		User::WaitForRequest(status);
		TRAP(err,iServer->iSession.OpenL(iServer->iRtpSocket, KDefaultMaxPcktSize));
		
		if (err!=KErrNotSupported)
			{
			SetTestStepResult(EFail);
			}
		}
		
	return TestStepResult();
	}

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY 
TVerdict COpenSessionRemoved2::trtpDoTestStepL()
	{
	
	SetTestStepResult(EPass);
	iIgnoreIncrease = 1;
	
	TRequestStatus status;
	
	//RConnection iConnection;
	User::LeaveIfError(iServer->iConnect.Open(iServer->iSocketServer));
	iServer->iConnect.Start(status);
	User::WaitForRequest(status);
	if(status.Int() != KErrNone)
	{
		SetTestStepResult(EFail);
		return TestStepResult();
	}
	
	// Create the container for all sub connection parameters
	RSubConParameterBundle subconParams;
	CleanupClosePushL(subconParams);
 
 	// Create a container for RTP sub connection parameters (Param bundle takes ownership)
	CSubConParameterFamily* subConFamily = CSubConParameterFamily::NewL(subconParams, KProtocolExtensionFamily);

 	// Create the requested generic parameter set for RTP (RTP family takes ownership)
	CSubConRTPGenericParamSet* reqGenericParams = CSubConRTPGenericParamSet::NewL(*subConFamily, CSubConParameterFamily::ERequested);
	
	
	TUid idRTP = TUid::Uid(KRtpFlowImplementationUid);
	// Set the requested Generic Parameters
//	reqGenericParams->SetProtocolIDRtp(500);
	reqGenericParams->SetProtocolUIDRtp(idRTP);
	reqGenericParams->SetBandWidth(1234);
	reqGenericParams->SetRtptimeConversion(100);
	reqGenericParams->SetRtpAutoSend(EFalse);
	reqGenericParams->SetRtpTimeNow(0);
	
	reqGenericParams->SetServiceProtocolID(KProtocolInetUdp);
	reqGenericParams->SetLowerSubConnType(RSubConnection::EAttachToDefault);
	reqGenericParams->SetCNAMEL(KRtpSessionCNAME());
	
	TUid idTier = TUid::Uid(2048);
	reqGenericParams->SetTierID(idTier);
	
	User::LeaveIfError(iServer->iSubConnect.Open(iServer->iSocketServer,RSubConnection::ECreateNew,iServer->iConnect));
	
	// Set Properties of the sub-connection
	User::LeaveIfError(iServer->iSubConnect.SetParameters(subconParams));

	// Destroy parameters
	CleanupStack::PopAndDestroy();         // subconParams


	TInetAddr localAddress(KInetAddrLoop, 9000);
	TInetAddr remoteAddress(KInetAddrLoop, 9002);

	TInt err(KErrNone);
	
	if (!iServer->iConnected)
		{
		TRAP(err,iServer->iSession.OpenL(iServer->iSocketServer, localAddress, remoteAddress,KDefaultMaxPcktSize,
			iServer->iSubConnect,EPriorityNormal,KNullDesC8));
		if (err!=KErrNotSupported)
			{
			SetTestStepResult(EFail);
			}
		}
/*	else
		{
		TRequestStatus status;
		User::LeaveIfError(iServer->iRtpSocket.Open(iServer->iSocketServer,localAddress.Family(),KSockDatagram, KProtocolInetUdp));
		User::LeaveIfError(iServer->iRtpSocket.Bind(localAddress));
		User::WaitForRequest(status);
		TRAP(err,iServer->iSession.OpenL(iServer->iSubConnect,iServer->iRtpSocket, KDefaultMaxPcktSize));
		if (err!=KErrNotSupported)
			{
			SetTestStepResult(EFail);
			}
		}*/
		
	return TestStepResult();
	}

#endif

