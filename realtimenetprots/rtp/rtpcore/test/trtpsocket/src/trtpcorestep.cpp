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
// class for the rtp core unit test cases
// 
//

/**
 @file
 @internalComponent
*/

#include <test/testexecutelog.h>
#include <in_sock.h>
#include <bt_sock.h>
#include <sys/time.h>
#include <badesca.h>
#include <random.h>

#include "trtpcorestep.h"

#include "trtpcoreserver.h"
#include "rtpsock.h"
#include "rtpcollisionmng.h"
#include "rtpbthandler.h"


// CF Rtp Related..
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY 
#include <rtp_uid.h>
#include "trtpsock_utils.h"
#endif

TVerdict CTRtpCoreStep::doTestStepL()
	{
	iServer->iCurrentStep = this;
	iServer->iStopping = EFalse;
	iServer->iStopCount = 1;
	
	if (!iServer->iOom)
		return doRtpTestStepL();
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
			TRAPD(error, verdict = doRtpTestStepL());
			TInt countAfter = User::CountAllocCells();
			INFO_PRINTF2(_L("countafter at %d"), countAfter);
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

TVerdict COom::doRtpTestStepL()
	{
	SetTestStepResult(EPass);

	TBool oom;
	GetBoolFromConfig(ConfigSection(),_L("Oom"),oom);

	INFO_PRINTF2(_L("Setting OOM mode to %d"), oom);

	iServer->iOom = oom;

	return TestStepResult();
	}

TVerdict COpenRtpSocket::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	User::LeaveIfError(iServer->iSocketServer.Connect());
	return TestStepResult();
	}


TVerdict COpenRtpSocket::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini file section
 */
	{
	SetTestStepResult(EPass);

	TInetAddr localAddress(KInetAddrLoop, 9000);
	// family KAfInet
	// protocol should be KProtocolInetRtp rather then UDP
	iServer->iController->RtpSocket().Open(iServer->iSocketServer, localAddress.Family(), 
											KSockDatagram,KProtocolInetUdp);
	
	return TestStepResult();
	}


TVerdict CCreateRtpController::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
		if(iServer->iScheduler)
		{
		SetTestStepResult(ETestSuiteError);	
		return TestStepResult();
		}
	iServer->iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iServer->iScheduler);
	//
	TInt err = iServer->iSocketServer.Connect();
	INFO_PRINTF2(_L("iSocketServer.Connect()  err = %d"), err);
	User::LeaveIfError(err);
	User::LeaveIfError(iServer->iConnection.Open(iServer->iSocketServer));
	// open a socket just in case we need it...
	TInt addrFamily; // either KAfInet or BT
	GetIntFromConfig(ConfigSection(),_L("AddressFamily"),addrFamily);
	if(KBTAddrFamily == static_cast<TUint>(addrFamily))
		{
		User::LeaveIfError(iServer->iRtpSocket.Open(iServer->iSocketServer,_L("L2CAP")));
		User::LeaveIfError(iServer->iRtcpSocket.Open(iServer->iSocketServer,_L("L2CAP")));
		//TInt error = iServer->iRtpSocket.Open(iServer->iSocketServer,_L("L2CAP"));
		}
		else
	if(KAfInet == static_cast<TUint>(addrFamily))
		{
		//User::LeaveIfError(iServer->iRtpSocket.Open(iServer->iSocketServer,_L("udp")));
		User::LeaveIfError(iServer->iRtpSocket.Open(iServer->iSocketServer,
													KAfInet, KSockDatagram, KProtocolInetUdp));
		User::LeaveIfError(iServer->iRtcpSocket.Open(iServer->iSocketServer,
													KAfInet, KSockDatagram, KProtocolInetUdp));
		}

	return TestStepResult();
	}

TVerdict CCreateRtpController::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini file section
 */
	{
	iIgnoreIncrease = 1;
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KCreateRtpController);
	SetTestStepResult(EFail);
	
	TPtrC controllerType;
	GetStringFromConfig(ConfigSection(),_L("Type"),controllerType);
	
	TInt addrFamily; // either KAfInet or BT
	GetIntFromConfig(ConfigSection(),_L("AddressFamily"),addrFamily);

	TSockAddr localAddr;
	localAddr.SetFamily(addrFamily);
	TInt rtpPort;
	GetIntFromConfig(ConfigSection(),_L("RtpPortNumber"),rtpPort);
	localAddr.SetPort(rtpPort);
	TPtrC cname16;
	//
	GetStringFromConfig(ConfigSection(),_L("CNAME"),cname16);	
	HBufC8* cname = HBufC8::NewMaxLC(cname16.Length());
	cname->Des().Copy(cname16);
	//
	TInt address;
	GetIntFromConfig(ConfigSection(),_L("LocalAddress"),address);
	
	switch(addrFamily)
		{
		case KAfInet:
			{
			TInetAddr bearerAddr(localAddr);
			bearerAddr.SetAddress(address);
			//bearerAddr.SetAddress(KInetAddrAny);
			localAddr = bearerAddr;
			break;
			}
		case KBTAddrFamily:
			{
			TBTSockAddr bearerAddr(localAddr);
			TBTDevAddr btDevAddr(address);
			bearerAddr.SetBTAddr(btDevAddr);
			localAddr = bearerAddr;
			break;
			}
		default:
			SetTestStepResult(ETestSuiteError);
			CleanupStack::PopAndDestroy(cname);
			return TestStepResult();
		}
	
	//
	TBool setRtpStreamParameters = EFalse;
	TInt maxDropout = 0;
	TInt maxMisorder = 0;
	TInt minSequential = 0;
	if(GetIntFromConfig(ConfigSection(),_L("MaxDropout"),maxDropout))
		setRtpStreamParameters = ETrue;
	if(GetIntFromConfig(ConfigSection(),_L("MaxMisorder"),maxMisorder))
		setRtpStreamParameters = ETrue;
	if(GetIntFromConfig(ConfigSection(),_L("MinSequential"),minSequential))
		setRtpStreamParameters = ETrue;
	//
	if (controllerType == _L("ControllerFromSockServ"))
		{
		SetTestStepResult(EPass);
		iServer->iUsingConnectedSocket = EFalse;
				
		// create RTP socket only from TSockAddr
		CRtpControllerBase* controller = CRtpControllerBase::NewL(iServer->iSocketServer,localAddr,KNullDesC8);
		CleanupStack::PushL(controller);
		// lets check a few things...
				
		CRtpHandlerBase* rtpHandler = controller->RtpHandler();
		if(ERtp != rtpHandler->Type())
			SetTestStepResult(EFail);
		// lets test address in handler
		localAddr.Family();
		TSockAddr tmp = rtpHandler->LocalAddress();
		tmp.Family();
		if(!rtpHandler->CompareAddresses(localAddr,rtpHandler->LocalAddress()))
			SetTestStepResult(EFail);

		iServer->iController = (CRtpControllerBase *)controller;
		CleanupStack::Pop(controller);
		}
	if (controllerType == _L("ControllerFromSockServWithCNAME"))
		{
		SetTestStepResult(EPass);
		iServer->iUsingConnectedSocket = EFalse;
		
		CRtpControllerBase* controller = CRtpControllerBase::NewL(iServer->iSocketServer,localAddr, *cname);
		CleanupStack::PushL(controller);
		// lets check a few things...
		CRtpHandlerBase* rtpHandler = controller->RtpHandler();
		CRtpHandlerBase* rtcpHandler = controller->RtcpHandler();
		TInt rtpPortTest = rtpHandler->LocalAddress().Port();
		TInt rtcpPortTest = 0;
		if(0 != rtcpHandler)
			{
			rtcpPortTest = rtcpHandler->LocalAddress().Port();
			}
				
		if(ERtp != rtpHandler->Type())
			SetTestStepResult(EFail);
		if(!rtpHandler->CompareAddresses(localAddr,rtpHandler->LocalAddress()))
			SetTestStepResult(EFail);
		
		if(0 != rtcpHandler)
			{
			if(ERtcp != rtcpHandler->Type())
				SetTestStepResult(EFail);
			if(rtpPortTest != (rtcpPortTest-1))
				SetTestStepResult(EFail);
			}
		else
			SetTestStepResult(EFail);
		
		iServer->iController = controller;
		CleanupStack::Pop(controller);
		}
	if (controllerType == _L("ControllerFromSockServWithRtcp"))
		{
		SetTestStepResult(EPass);
		iServer->iUsingConnectedSocket = EFalse;
		
		TInt rtcpPort;
		GetIntFromConfig(ConfigSection(),_L("RtcpPortNumber"),rtcpPort);
		// create RTP and RTCP sockets from TSockAddr and rtcp port number
		CRtpControllerBase* controller = CRtpControllerBase::NewL(iServer->iSocketServer,localAddr,rtcpPort,*cname);
		CleanupStack::PushL(controller);
		// lets check a few things...
		CRtpHandlerBase* rtpHandler = controller->RtpHandler();
		CRtpHandlerBase* rtcpHandler = controller->RtcpHandler();
//		TInt rtpPortTest = rtpHandler->LocalAddress().Port();
		TInt rtcpPortTest = 0;
		if(0 != rtcpHandler)
			{
			rtcpPortTest = rtcpHandler->LocalAddress().Port();
			}

		if(ERtp != rtpHandler->Type())
			SetTestStepResult(EFail);
		if(!rtpHandler->CompareAddresses(localAddr,rtpHandler->LocalAddress()))
			SetTestStepResult(EFail);
		if(0 != rtcpHandler)
			{
			if(ERtcp != rtcpHandler->Type())
				SetTestStepResult(EFail);
			if(rtcpPort != rtcpPortTest)
				SetTestStepResult(EFail);
			}
		else
			SetTestStepResult(EFail);

		iServer->iController = controller;
		CleanupStack::Pop(controller);
		}
	if (controllerType == _L("ControllerFromSocket"))
		{
		SetTestStepResult(EPass);
		iServer->iUsingConnectedSocket = ETrue;
		
		User::LeaveIfError(iServer->iRtpSocket.Bind(localAddr));
		
		// create RTP socket only from TSockAddr
		iServer->iController = CRtpControllerBase::NewL(iServer->iRtpSocket);
		// lets check a few things...
		CRtpHandlerBase* rtpHandler = iServer->iController->RtpHandler();
		if(ERtp != rtpHandler->Type())
			SetTestStepResult(EFail);
		// lets test address in handler
		if(!rtpHandler->CompareAddresses(localAddr,rtpHandler->LocalAddress()))
			SetTestStepResult(EFail);
		// and address in socket itself
		TSockAddr testAddr;
		rtpHandler->RtpSocket().LocalName(testAddr);
		if(!rtpHandler->CompareAddresses(localAddr,testAddr))
			SetTestStepResult(EFail);
		
		}
	if (controllerType == _L("ControllerFromSocketWithRtcp"))
		{
		SetTestStepResult(EPass);
		iServer->iUsingConnectedSocket = ETrue;
		
		}
	
	if(setRtpStreamParameters)
		{
		if(KErrNone != iServer->iController->SetRtpStreamParameters(maxDropout,maxMisorder,minSequential))
			SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(cname);
	return TestStepResult();
	}

TVerdict CActivateRtpController::doRtpTestStepL()
	{
	TInt type; // the type of RTP connections
	if(!GetIntFromConfig(ConfigSection(),_L("ConnectionType"),type))
		{
		SetTestStepResult(ETestSuiteError);
		return TestStepResult();
		}
	
	if(iServer->iController)
		SetTestStepResult(EPass);
	else
		{
		SetTestStepResult(ETestSuiteError);
		return TestStepResult();
		}

	iServer->iController->SetRTPTimeConversion(100, 100);
	TRAPD(error,iServer->iController->ActivateL(TRtpConnectionType(type)));
	//const TInt KErrInUse=(-14);
	//const TInt KErrServerTerminated=(-15);
	if(KErrNone != error)
		{
		/* We are interested in this particular error since it's returned from socket 
		when trying to activate already active handler/socket. */
		if((KErrInUse == error)||(KErrArgument == error))
			{
			SetTestStepResult(EFail);
			}
		else if(KErrNotSupported == error)
			{
			// returned for COInConnection over UDP
			SetTestStepResult(EFail);
			}
		else
			User::Leave(error);
		}
	else
		{
		// lets test RTP socket now 
		CRtpHandlerBase* rtpHandler = iServer->iController->RtpHandler();
		TSockAddr testAddr;
		rtpHandler->RtpSocket().LocalName(testAddr);
		if(!rtpHandler->CompareAddresses(rtpHandler->LocalAddress(),testAddr))
			SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CDeleteRtpController::doRtpTestStepL()
	{
	if(iServer->iController)
		SetTestStepResult(EPass);
	else
		{
		SetTestStepResult(ETestSuiteError);
		return TestStepResult();
		}
	TSockAddr rtpAddr;
	if(iServer->iUsingConnectedSocket)
		{
		iServer->iController->RtpHandler()->RtpSocket().LocalName(rtpAddr);
		}
	delete iServer->iController;
	iServer->iController = 0;
	
	if(iServer->iUsingConnectedSocket)
		{
		// lets check we did not take down the sockets with controller
		// buffers
		TBuf8<1024> sendBuf;
		sendBuf.SetLength(10);
		TBuf8<1024> recvBuf;
		recvBuf.SetLength(10);
		// prepare reciver (RTP socket used previsously by controller)
		CWaiter* waiter = CWaiter::NewLC(iServer);
		TSockAddr dummyAddr;
		iServer->iRtpSocket.RecvFrom(recvBuf,dummyAddr, 0,waiter->iStatus);
		// sender
		User::LeaveIfError(iServer->iSocket.Open(iServer->iSocketServer, KAfInet, 
									   KSockDatagram, KProtocolInetUdp));
		CleanupClosePushL(iServer->iSocket);
		CWaiter* sender = CWaiter::NewLC(iServer);
		iServer->iSocket.SendTo(sendBuf,rtpAddr,0,sender->iStatus);
		
		User::WaitForRequest(sender->iStatus);
		//
		User::WaitForRequest(waiter->iStatus);
		//
		CleanupStack::PopAndDestroy(3); // socket, waiter, sender
		}
	iServer->iSocket.Close();
	iServer->iRtpSocket.Close();
	iServer->iRtcpSocket.Close();
	iServer->iUsingConnectedSocket = EFalse;
	// NOTE: This wil panic if already closed!!! Defect in ESock?!?
	iServer->iSocketServer.Close();
	//
	
	CActiveScheduler::Install(NULL);
	delete iServer->iScheduler;
	iServer->iScheduler = 0;
	return TestStepResult();
	}

// Close everything except the session - Used to tidy up after close
// tests to check evrything is closed.
TVerdict CTidyUp::doRtpTestStepL()
	{
	SetTestStepResult(EPass);

	iServer->iSocketServer.Close();
	CActiveScheduler::Install(NULL);
	delete iServer->iScheduler;
	iServer->iScheduler = 0;
	return TestStepResult();
	}


TVerdict CBasicSend::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CBasicSend::doRtpTestStepL()
	{
	if(0 == CActiveScheduler::Current())
		{
		SetTestStepResult(ETestSuiteError);	
		return TestStepResult();
		}
	if(0 == iServer->iController)
		{
		SetTestStepResult(ETestSuiteError);	
		return TestStepResult();
		}

	SetTestStepResult(EPass);

	TBuf8<1024> sendBuf;
	sendBuf.SetLength(50);
	TInt i;
	for(i=0;i<50;i++)
		sendBuf[i]=0;
	TBuf8<1024> recvBuf; // To put the packet in
	recvBuf.SetLength(50);
	for(i=0;i<50;i++)
		recvBuf[i]=55;
	
	
	TInt stopCount;
	if (!GetIntFromConfig(ConfigSection(), _L("StopCount"), stopCount))
		stopCount = 2;
	
	TInt port;
	if (!GetIntFromConfig(ConfigSection(), _L("SendPort"), port))
		port = 9001;
	TInetAddr remoteAddress(KInetAddrLoop, port);
	User::LeaveIfError(iServer->iSocket.Open(iServer->iSocketServer, KAfInet, 
									   KSockDatagram, KProtocolInetUdp));
	CleanupClosePushL(iServer->iSocket);
	User::LeaveIfError(iServer->iSocket.Bind(remoteAddress));

	TSockAddr addr;
	CWaiter* waiter = CWaiter::NewLC(iServer);
	iServer->iSocket.RecvFrom(recvBuf,addr, 0,waiter->iStatus);
		
	CRtpHandlerBase* rtpHandler= iServer->iController->RtpHandler();
	rtpHandler->SetRemoteAddress(remoteAddress);
	
	CWaiter* sender = CWaiter::NewLC(iServer);
	sendBuf.SetLength(12);
	TRtpFixedHdrMemLayout sendHeader(sendBuf);
	struct timeval tm;
	gettimeofday(&tm,NULL);
	TUint32 t1=tm.tv_sec;
	sendHeader.SetTimeStamp(t1);

	rtpHandler->Send(sendBuf,sender->iStatus);
	
	User::WaitForRequest(sender->iStatus);
	
	User::WaitForRequest(waiter->iStatus);
	//CActiveScheduler::Start();

	if (stopCount == 1)
		{
		// The waiter probably hasn't finished. Cancel the read
		iServer->iRtpSocket.CancelRecv();
		iServer->iStopCount = 1;
		iServer->iStopping = 0;
		CActiveScheduler::Start();
		}
	CleanupStack::PopAndDestroy(3); // socket, waiter, sender

	
	
	TRtpFixedHdrMemLayout header(sendBuf);
	if(!CTRtpCoreServer::CompareRtpHeadersFromBuf(sendBuf,recvBuf))
		SetTestStepResult(EFail);
	if(!CTRtpCoreServer::CompareRtpHeaderWithBuf(header,recvBuf))
		SetTestStepResult(EFail);


	iServer->iSocket.Close();

	return TestStepResult();
}


TVerdict CFixedHeader::doRtpTestStepL()
	{
	SetTestStepResult(EPass);

	TBuf8<1024> sendBuf;
	sendBuf.SetLength(50);
	TInt i;
	for(i=0;i<50;i++)
		sendBuf[i]=0;
	TBuf8<1024> recvBuf; // To put the packet in
	recvBuf.SetLength(50);
	for(i=0;i<50;i++)
		recvBuf[i]=55;
	
	
	TInt port;
	if (!GetIntFromConfig(ConfigSection(), _L("SendPort"), port))
		port = 9001;
	TInetAddr remoteAddress(KInetAddrLoop, port);
	User::LeaveIfError(iServer->iRtpSocket.Open(iServer->iSocketServer, KAfInet, 
									   KSockDatagram, KProtocolInetUdp));
	CleanupClosePushL(iServer->iRtpSocket);
	User::LeaveIfError(iServer->iRtpSocket.Bind(remoteAddress));

	TSockAddr addr;
	CWaiter* waiter = CWaiter::NewLC(iServer);
	iServer->iRtpSocket.RecvFrom(recvBuf,addr, 0,waiter->iStatus);
	
	CRtpHandlerBase* rtpHandler= iServer->iController->RtpHandler();
	rtpHandler->SetRemoteAddress(remoteAddress);
	
	CWaiter* sender = CWaiter::NewLC(iServer);
	sendBuf.SetLength(12);
	TRtpFixedHdrMemLayout sendHeader(sendBuf);
	struct timeval tm;
	gettimeofday(&tm,NULL);
	TUint32 t1=tm.tv_sec;
	sendHeader.SetTimeStamp(t1);

	rtpHandler->Send(sendBuf,sender->iStatus);
	TInt length = sendBuf.Size();
	length = recvBuf.Size();
	User::WaitForRequest(sender->iStatus);
	// lets get the send packet into our class
	CRtpFixedHeader sendFixHeaderTest(sendBuf);
	// wait for receive
	User::WaitForRequest(waiter->iStatus);
	CleanupStack::PopAndDestroy(3); // socket, waiter, sender

	
	
	CRtpFixedHeader recvFixHeaderTest(recvBuf);
	
				
	//byte 1
	if(recvFixHeaderTest.Version() != sendFixHeaderTest.Version())
		SetTestStepResult(EFail);
	if(recvFixHeaderTest.IsPadding() != sendFixHeaderTest.IsPadding())
		SetTestStepResult(EFail);
	if(recvFixHeaderTest.IsExtension() != sendFixHeaderTest.IsExtension())
		SetTestStepResult(EFail);
	if(recvFixHeaderTest.CC() != sendFixHeaderTest.CC())
		SetTestStepResult(EFail);
	
	// byte 2
	if(recvFixHeaderTest.IsMarker() != sendFixHeaderTest.IsMarker())
		SetTestStepResult(EFail);
	if(recvFixHeaderTest.PayloadType() != sendFixHeaderTest.PayloadType())
		SetTestStepResult(EFail);
	// byte 3/4
	if(recvFixHeaderTest.SequenceNumber() != sendFixHeaderTest.SequenceNumber())
		SetTestStepResult(EFail);
	// TIMESTAMP
	// byte 5/6/7/8
	if(recvFixHeaderTest.TimeStamp() != sendFixHeaderTest.TimeStamp())
		SetTestStepResult(EFail);
	
	// SSRC
	// byte 9/10/11/12 MSB from SSRC
	if(recvFixHeaderTest.SSRC() != sendFixHeaderTest.SSRC())
		SetTestStepResult(EFail);

	
	iServer->iRtpSocket.Close();

	return TestStepResult();
}

TVerdict CRRFixedHeader::doRtpTestStepL()
	{
	__RTP_LOG(_L("CRRFixedHeader::doRtpTestStepL()"));
	SetTestStepResult(EPass);

	TBuf8<1024> sendBuf;
	sendBuf.SetLength(50);
	TInt i;
	for(i=0;i<50;i++)
		sendBuf[i]=0;
	TBuf8<1024> recvBuf; // To put the packet in
	recvBuf.SetLength(50);
	for(i=0;i<50;i++)
		recvBuf[i]=55;
	
	
	TInt port;
	if (!GetIntFromConfig(ConfigSection(), _L("SendPort"), port))
		port = 9001;
	TInetAddr remoteAddress(KInetAddrLoop, port);
	User::LeaveIfError(iServer->iRtpSocket.Open(iServer->iSocketServer, KAfInet, 
									   KSockDatagram, KProtocolInetUdp));
	CleanupClosePushL(iServer->iRtpSocket);
	User::LeaveIfError(iServer->iRtpSocket.Bind(remoteAddress));

	TSockAddr addr;
	CWaiter* waiter = CWaiter::NewLC(iServer);
	iServer->iRtpSocket.RecvFrom(recvBuf,addr, 0,waiter->iStatus);
	
	CRtpHandlerBase* rtcpHandler= iServer->iController->RtcpHandler();
	if(NULL == rtcpHandler)
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	rtcpHandler->SetRemoteAddress(remoteAddress);
	
	CWaiter* sender = CWaiter::NewLC(iServer);
	
	// RR for one source only - 32 bytes
	sendBuf.SetLength(32);
	TRtcpFixedHdrMemLayout sendHeader(sendBuf);
	sendHeader.SetVersion(1);
	sendHeader.SetCount(1);
	sendHeader.SetPacketType(ERtcpRR);
	// The length of this RTCP packet in 32-bit words minus one
	sendHeader.SetLength(7);

	rtcpHandler->Send(sendBuf,sender->iStatus);
	TInt length = sendBuf.Size();
	length = recvBuf.Size();
	User::WaitForRequest(sender->iStatus);
	// lets get the send packet into our class
	CRtcpRRPacketTest sendRRTest(sendBuf);
	// wait for receive
	User::WaitForRequest(waiter->iStatus);
	CleanupStack::PopAndDestroy(3); // socket, waiter, sender

	
	
	CRtcpRRPacketTest recvRRTest(recvBuf);
	if(recvRRTest != sendRRTest)
		SetTestStepResult(EFail);
	
	
	iServer->iRtpSocket.Close();

	return TestStepResult();
}

TVerdict CReceive::doTestStepPreambleL()
	{
	__RTP_LOG(_L("\n"));
	__RTP_LOG(_L("\n"));
	__RTP_LOG(_L("\n"));
	__RTP_LOG(_L("CReceive::doTestStepPreambleL()"));
	SetTestStepResult(EPass);
	
	TInt addrFamily; // either KAfInet or BT
	if(!GetIntFromConfig(ConfigSection(),_L("SendAddressFamily"),addrFamily))
		addrFamily = KAfInet;

	TSockAddr localAddr;
	localAddr.SetFamily(addrFamily);
	
	localAddr.SetPort(9010);
	
	TInt address;
	if(!GetIntFromConfig(ConfigSection(),_L("SendLocalAddress"),address))
		{
		address = 2130706433; // KInetAddrLoop
		}
	switch(addrFamily)
		{
		case KAfInet:
			{
			TInetAddr bearerAddr(localAddr);
			bearerAddr.SetAddress(address);
			localAddr = bearerAddr;
			break;
			}
		case KBTAddrFamily:
			{
			TBTSockAddr bearerAddr(localAddr);
			TBTDevAddr btDevAddr(address);
			bearerAddr.SetBTAddr(btDevAddr);
			localAddr = bearerAddr;
			break;
			}
		default:
			SetTestStepResult(ETestSuiteError);
			return TestStepResult();
		}
	
	// we need RTCP as well from now on...
	TUint rtcpSendPort = localAddr.Port()+1;
	iServer->iSendController = CRtpControllerBase::NewL(iServer->iSocketServer,localAddr,rtcpSendPort,KNullDesC8);
	// set some arbitrary RTP 'time'
	TUint now = 100;
	TUint conversion = 100;
	iServer->iSendController->SetRTPTimeConversion(now, conversion);
	iServer->iSendController->ActivateL(ERtpCL);
	return TestStepResult();
	}

TVerdict CReceive::doTestStepPostambleL()
	{
	// get rid of the ssrc from the receiver's cache
	iServer->iController->DeleteEntryL(iServer->iSendController->GetLocalSSRC());
	
	delete iServer->iSendController;
	iServer->iSendController = 0;
	return TestStepResult();
	}

TVerdict CBasicReceive::doRtpTestStepL()
	{
	if(0 == CActiveScheduler::Current())
		{
		SetTestStepResult(ETestSuiteError);	
		return TestStepResult();
		}
	if(0 == iServer->iController)
		{
		SetTestStepResult(ETestSuiteError);	
		return TestStepResult();
		}

	if(0 == iServer->iSendController)
		{
		SetTestStepResult(ETestSuiteError);	
		return TestStepResult();
		}

	SetTestStepResult(EPass);
	//
	TBuf8<1024> sendBuf;
	sendBuf.SetLength(50);
	TInt i;
	for(i=0;i<50;i++)
		sendBuf[i]=0;
		
	TBuf8<1024> recvBuf; // To put the packet in
	recvBuf.SetLength(50);
	for(i=0;i<50;i++)
		recvBuf[i]=55;
	
	//
	TInt stopCount;
	if (!GetIntFromConfig(ConfigSection(), _L("StopCount"), stopCount))
		stopCount = 2;
	
	CWaiter* waiter = CWaiter::NewLC(iServer);
	CRtpHandlerBase* recvRtpHandler= iServer->iController->RtpHandler();
	TSockAddr remoteAddress = recvRtpHandler->LocalAddress();
	recvRtpHandler->SetDataObserver(waiter);
	recvRtpHandler->Read(recvBuf);
	CWaiter* sender = CWaiter::NewLC(iServer);	 
	sendBuf.SetLength(12);
	//
	TRtpFixedHdrMemLayout sendHeader(sendBuf);
	struct timeval tm;
	gettimeofday(&tm,NULL);
	TUint32 t1=tm.tv_sec;
	sendHeader.SetTimeStamp(t1);
		
	//
	CRtpHandlerBase* sendRtpHandler= iServer->iSendController->RtpHandler();
	sendRtpHandler->SetRemoteAddress(remoteAddress);
	
	sendRtpHandler->Send(sendBuf,sender->iStatus);
	User::WaitForRequest(sender->iStatus);
		
	CActiveScheduler::Start();

	if (stopCount == 1)
		{
		// The waiter probably hasn't finished. Cancel the read
		iServer->iRtpSocket.CancelRecv();
		iServer->iStopCount = 1;
		iServer->iStopping = 0;
		CActiveScheduler::Start();
		}
	CleanupStack::PopAndDestroy(2); // waiter, sender

	if(!CTRtpCoreServer::CompareRtpHeadersFromBuf(sendBuf,recvBuf))
		SetTestStepResult(EFail);
	TRtpFixedHdrMemLayout header(sendBuf);	
	if(!CTRtpCoreServer::CompareRtpHeaderWithBuf(header,recvBuf))
		SetTestStepResult(EFail);
	
	return TestStepResult();
}

LOCAL_C TVerdict SendRtpPacketsL(CTRtpCoreServer* iServer, CRtpControllerBase* iController, CRtpControllerBase* iSendController);

TVerdict SendRtpPacketsL(CTRtpCoreServer* iServer, CRtpControllerBase* iController, CRtpControllerBase* iSendController)
	{
	__RTP_LOG(_L("Sending RTP packets ..."));
	
	if(0 == CActiveScheduler::Current())
		{
		return ETestSuiteError;
		}
	if(0 == iController)
		{
		return ETestSuiteError;
		}

	if(0 == iSendController)
		{
		return ETestSuiteError;
		}

	//
	const TInt numberOfSendPackets = 3;
	//
	
	TBuf8<1024> sendBuf;
	TInt i;
	for (i=0;i<numberOfSendPackets;i++)
		{
		sendBuf.SetLength(50);
		for(int ii=0;ii<50;ii++)
			sendBuf[ii]=0;
		}
	
	TVerdict verdict(EPass);
	HBufC8* sendBufs[numberOfSendPackets];
	
	for (i=0;i<numberOfSendPackets;i++)
		{
		sendBufs[i] = HBufC8::NewMaxLC(50);
		}
	TBuf8<1024> recvBuf; // To put the packet in
	recvBuf.SetLength(50);
	for(i=0;i<50;i++)
		recvBuf[i]=55;
	
	//
	CWaiter* sender[numberOfSendPackets]; 
	
	for (i=0;i<numberOfSendPackets;i++)
		{ 
		sender[i] = CWaiter::NewLC(iServer);
		//
		TPtr8 sendBuf = sendBufs[i]->Des();
		sendBuf.SetLength(12);
		//
		TRtpFixedHdrMemLayout sendHeader(sendBuf);
		struct timeval tm;
		gettimeofday(&tm,NULL);
		TUint32 t1=tm.tv_sec;
		sendHeader.SetTimeStamp(t1);
		// needs to be set to zero otherwise panics
		sendHeader.SetCSRCCount(0);
		sendHeader.SetPadding(EFalse);
		sendHeader.SetPayloadType(96);
		sendHeader.SetVersion(2);
		}
	//

	CWaiter* waiter = CWaiter::NewLC(iServer);
	CRtpHandlerBase* recvRtpHandler= iController->RtpHandler();
	TSockAddr remoteAddress = recvRtpHandler->LocalAddress();
	recvRtpHandler->SetDataObserver(waiter);
	recvRtpHandler->Read(recvBuf);
	
	CRtpHandlerBase* sendRtpHandler= iSendController->RtpHandler();
	sendRtpHandler->SetRemoteAddress(remoteAddress);
	
	for (i=0;i<numberOfSendPackets;i++)
		{
		TPtr8 sendBuf = sendBufs[i]->Des();
		sendRtpHandler->Send(sendBuf,sender[i]->iStatus);
		User::WaitForRequest(sender[i]->iStatus);
		}
	
	CActiveScheduler::Start();
	
	if(!CTRtpCoreServer::CompareRtpHeadersFromBuf(*sendBufs[numberOfSendPackets-1],recvBuf))
		verdict = EFail;
	//
	TRtpFixedHdrMemLayout header(*sendBufs[numberOfSendPackets-1]);	
	if(!CTRtpCoreServer::CompareRtpHeaderWithBuf(header,recvBuf))
		verdict = EFail;
	// lets check time
	struct timeval recv_tm;
	gettimeofday(&recv_tm,NULL);
	TUint32 t2=recv_tm.tv_sec;
	// lets get error margin ... 2s?
	if(!(t2 >= header.TimeStamp())&&(t2 < header.TimeStamp()+2))
		verdict = EFail;
	
	if(header.CSRCs())
		verdict = EFail;
	
	//
	CleanupStack::PopAndDestroy(1+2*numberOfSendPackets);
	
	return verdict;
}

TVerdict CInitialReceive::doRtpTestStepL()
	{
	__RTP_LOG(_L("CInitialReceive::doRtpTestStepL()"));
	SetTestStepResult(EPass);

	//
	TInt stopCount;
	if (!GetIntFromConfig(ConfigSection(), _L("StopCount"), stopCount))
		stopCount = 1;
	
	iServer->iStopping = EFalse;
	iServer->iStopCount = stopCount;
			
	return SendRtpPacketsL(iServer, iServer->iController, iServer->iSendController);
	}

TVerdict CReceive::doRtcpBeginL(TDes8& aRecvBuf, MRtpDataObserver *aDataObs, CRtpControllerBase* iController, CRtpControllerBase* iSendController)
	{
	if(0 == CActiveScheduler::Current())
		{
		SetTestStepResult(ETestSuiteError);
		return TestStepResult();
		}
	if(0 == iController)
		{
		SetTestStepResult(ETestSuiteError);	
		return TestStepResult();
		}

	if(0 == iSendController)
		{
		SetTestStepResult(ETestSuiteError);	
		return TestStepResult();
		}

	SetTestStepResult(EPass);
	//
	CRtpHandlerBase* recvRtcpHandler= iSendController->RtcpHandler();
	if(NULL == recvRtcpHandler)
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
		
	CRtpHandlerBase* sendRtcpHandler= iController->RtcpHandler();
	if(NULL == sendRtcpHandler)
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}	
	//
	TSockAddr remoteAddress = recvRtcpHandler->LocalAddress();
	sendRtcpHandler->SetRemoteAddress(remoteAddress);

	__RTP_LOG1(_L("Reading RTCP packet at address 0x%x"), aRecvBuf.Ptr());
    recvRtcpHandler->SetDataObserver(aDataObs);
	recvRtcpHandler->Read(aRecvBuf);

	// The controller should not send packets automatically
	iController->SetAutoSendOption(EFalse);
	iSendController->SetAutoSendOption(EFalse);
	
	iServer->iStopping = EFalse;
	iServer->iStopCount = 1;
			
	SendRtpPacketsL(iServer, iController, iSendController);

	iServer->iStopping = EFalse;
	iServer->iStopCount = 1;
			
	__RTP_LOG2(_L("sending packet from %u to %u"), iController->GetLocalSSRC(), iSendController->GetLocalSSRC());
	return TestStepResult();
	}

const TInt KCompareSize = 4;
TVerdict CBYEReceive::doRtpTestStepL()
	{
	__RTP_LOG(_L("CBYEReceive::doRtpTestStepL()"));
	
	CWaiter* waiter = CWaiter::NewLC(iServer);
	TBuf8<1024> recvBuf; // To put the packet in
	SetTestStepResult(doRtcpBeginL(recvBuf, waiter, iServer->iController, iServer->iSendController));
	
	CRtpHandlerBase* sendRtcpHandler= iServer->iController->RtcpHandler();
	TInetAddr remoteAddress(KInetAddrLoop,iServer->iSendController->RtcpHandler()->LocalAddress().Port());
	sendRtcpHandler->SetDefaultRemoteAddress(remoteAddress);
	sendRtcpHandler->SetRemoteAddress(remoteAddress);
	iServer->iController->SendBYEPacketL(iServer->iController->GetLocalSSRC());
	
	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(waiter); // waiter

	//
	TBuf8<1024> sendBuf;
	sendBuf.SetLength(50);
	TInt i;
	for(i=0;i<50;i++)
		sendBuf[i]=0;
	
	sendBuf.SetLength(8);
	TRtcpFixedHdrMemLayout sendHeader(sendBuf);
	sendHeader.SetVersion(2);
	sendHeader.SetPadding(EFalse);
	sendHeader.SetCount(1);
	sendHeader.SetPacketType(ERtcpBYE);
	sendHeader.SetSSRC(iServer->iController->GetLocalSSRC());	
	sendHeader.SetLength(1);//size in words

	//The recvBuf will actually contain a compound packet with BYE packet as the
	//last RTCP packet within that compound packet.
	//Before comparing the sendBuf and recvBuf, it is important
	//that the recvBuf offset is incremented...

	//Class CRtcpByePacketTest is responsible for correctly mapping
	//the offset in recvBuf to BYE packet
	CRtcpByePacketTest packet2(recvBuf);
	
	TInt recvBufOffset = (packet2.iOffset);
	TBuf8<KCompareSize> cmpRecvBuf(KCompareSize);

	for(TInt ii=0;ii<KCompareSize;ii++,recvBufOffset++)
		cmpRecvBuf[ii]=recvBuf[recvBufOffset];

	if(!CTRtpCoreServer::CompareRtcpHeadersFromBuf(sendBuf,cmpRecvBuf))
		SetTestStepResult(EFail);
	
	TRtcpFixedHdrMemLayout header(sendBuf);	
	
	if(!CTRtpCoreServer::CompareRtcpHeaderWithBuf(header,cmpRecvBuf))
		SetTestStepResult(EFail);

	//
	CRtcpByePacketTest packet1(sendBuf, EFalse);
	if(packet1 != packet2)
		SetTestStepResult(EFail);

	return TestStepResult();
}


TVerdict CIntervalTest::doRtpTestStepL()
{
	__RTP_LOG(_L("CIntervalTest::doRtpTestStepL()"));
	if(0 == CActiveScheduler::Current())
		{
		SetTestStepResult(ETestSuiteError);	
		return TestStepResult();
		}

	TInt members = 200;
	TInt senders = 5;
		
	//const TUint KRtcpDefaultAverageSize = 50; 
	//const TUint KRtpDefaultSessionBandwidth = 10000;
	TUint avgRtcpSize = KRtcpDefaultAverageSize;
	TUint rtcpBw = KRtpDefaultSessionBandwidth*KRtcpBandwidthFraction/100;
	
	TBuf8<8> random(8);
	TRandom::RandomL(random);
	TInt64 randSeed = *(reinterpret_cast<const TInt64*>(random.Ptr()));
	TInt i;
	
	for(i=0;i<20;i++)
	{
		TTimeIntervalMicroSeconds32 interval1 = 
		RtpUtils::RtcpIntervalMicroSeconds32(members,senders,
										rtcpBw,
										EFalse,
										avgRtcpSize,
										ETrue, randSeed);
		
	}
	
	members = 200;
	senders = 55;
	
	for(i=0;i<10;i++)
	{
		TTimeIntervalMicroSeconds32 interval2 = 
		
		RtpUtils::RtcpIntervalMicroSeconds32(members,senders,
										rtcpBw,
										ETrue,
										avgRtcpSize,
										EFalse, randSeed);
		
	}
	
	/*
	if (interval1 >= interval2)
	{
		SetTestStepResult(EFail);
	}
	*/
									

	members = 200;
	senders = 150;

	for(i=0;i<10;i++)
	{
		TTimeIntervalMicroSeconds32 interval1 = RtpUtils::RtcpIntervalMicroSeconds32(members,senders,
														rtcpBw,
														ETrue,
														avgRtcpSize,
														EFalse, randSeed);
		
		
	}
	
	/*
	if(interval1 == interval2)
	{
		SetTestStepResult(EFail);
	}
	*/
	return TestStepResult();
}


//consideration test does not send any packets
//the test only calculates intervals based on member/sender count
//can be improved by simulating the actual sending of rtp packets
//from multiple sources

TVerdict CConsiderationTest::doRtpTestStepL()
{
	//	
	__RTP_LOG(_L("CConsiderationTest::doRtpTestStepL()"));
	if(0 == CActiveScheduler::Current())
		{
		SetTestStepResult(ETestSuiteError);	
		return TestStepResult();
		}

	TInt members = 2;
	TInt senders = 1;
		
	//call 
	//const TUint KRtcpDefaultAverageSize = 50; 
	//const TUint KRtpDefaultSessionBandwidth = 10000;
	TUint avgRtcpSize = KRtcpDefaultAverageSize;
	TUint rtcpBw = KRtpDefaultSessionBandwidth*KRtcpBandwidthFraction/100;
	
	TBuf8<8> random(8	);
	TRandom::RandomL(random);
	TInt64 randSeed = *(reinterpret_cast<const TInt64*>(random.Ptr()));
	
	
										
	//assume this member has already sent a RTCP packet
	TTimeIntervalMicroSeconds32 interval1 = 
	RtpUtils::RtcpIntervalMicroSeconds32(members,senders,
									rtcpBw,
									ETrue,
									avgRtcpSize,
									EFalse, randSeed);
	
	//100 members joined at the same time
	// senders <= (members*KRtcpSenderBwFraction/100)
	members+=100;
	senders+=100;
	// senders > (members*KRtcpSenderBwFraction/100)
	TTimeIntervalMicroSeconds32 interval2 = 
	RtpUtils::RtcpIntervalMicroSeconds32(members,senders,
													rtcpBw,
													ETrue,
													avgRtcpSize,
													EFalse, randSeed);
	
	if(interval1 >= interval2)
	{
		SetTestStepResult(EFail);
	}
	
	return TestStepResult();
}
