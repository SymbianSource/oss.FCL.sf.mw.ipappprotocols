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
 
#include "trtpcorestep.h"
#include "trtpcoreserver.h"

#include "rtpcollisionmng.h"
#include "rtpstatemachine.h"

#include "rtcpsessiondata.h"

#include "rtpcontroller.h"
#include <test/testexecutelog.h>
#include "in_sock.h"

#include <sys/time.h>
#include <badesca.h>
#include <random.h>


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
			iServer->iStopping = EFalse;
			iServer->iStopCount = 1;
			iIgnoreIncrease = EFalse;
			iError = KErrNone;
			TRAPD(error, verdict = doRtpTestStepL());
			TInt countAfter = User::CountAllocCells();
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

// 
// SDES PACKET
//
TVerdict CSDESSetAttribute::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
		
TVerdict CSDESSetAttribute::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates getting octet count from RTCP SR packet
 */
	{
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KSDESSetAttribute);
	
	RBuf8 buffer;
	
	TRtcpSdesPack rtcpsdespacket(24);
	rtcpsdespacket.AssignL(buffer);
	
	HBufC8* buf;
	buf = HBufC8::NewL(15);
	TPtr8 bufPtr= buf->Des();
	bufPtr = _L8("SDES Packet");
	
	rtcpsdespacket.SetAttribute(7, bufPtr);
		
		
		// SDES Packet......
	
		
	HBufC8* attrPreString;
	attrPreString = HBufC8::NewL(15);
	TPtr8 sdesStringPtr= attrPreString->Des();
	sdesStringPtr = _L8("NameField------");
	
	
	HBufC8* attrValue;
	attrValue = HBufC8::NewL(15);
	TPtr8 sdesPtr= attrValue->Des();
	sdesPtr = _L8("Symbian");
	
	
	RBuf8 sdes;
	TRtcpSdesPack sdesPacket(sdesPtr.MaxLength()+ sdesStringPtr.MaxLength());
	sdesPacket.AssignL(sdes);
	sdesPacket.SetAttribute(sdesStringPtr,sdesPtr);
		

	// Parsing...
	TRtcpSdesPack recvSdes(sdes);
	INFO_PRINTF2(_L("SDes Type - %d\n"),sdesPacket.Type());
	TPtrC8 sdesvalue = recvSdes.Attribute();
	TPtrC8 sdesPreString = recvSdes.PreFixString();
	
	buffer.Close();
	delete buf;
	delete attrPreString;
	delete attrValue;
	sdes.Close();
	return TestStepResult();
	}
	
TVerdict CSDESGetLength::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
		
TVerdict CSDESGetLength::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates getting timestamp from RTCP SR packet
 */
	{
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KSDESGetLength);
	
	RBuf8 buffer;
	
	TRtcpSdesPack rtcpsdespacket(24);
	rtcpsdespacket.AssignL(buffer);
	
	HBufC8* buf;
	buf = HBufC8::NewL(15);
	TPtr8 bufPtr= buf->Des();
	bufPtr = _L8("SDES Packet");
	
	rtcpsdespacket.SetAttribute(7, bufPtr);
	
	TUint32 value = rtcpsdespacket.Length();
	
	if(11 == value)
	{
		SetTestStepResult(EPass);
	}
	else
	{
		SetTestStepResult(EFail);
	}
	
	buffer.Close();
	delete buf;
	return TestStepResult();
	}	
	
TVerdict CSDESGetAttribute::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
		
TVerdict CSDESGetAttribute::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates getting timestamp from RTCP SR packet
 */
	{
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KSDESGetAttribute);
	
	RBuf8 buffer;
	
	TRtcpSdesPack rtcpsdespacket(24);
	rtcpsdespacket.AssignL(buffer);
	
	HBufC8* buf;
	buf = HBufC8::NewL(15);
	TPtr8 bufPtr= buf->Des();
	bufPtr = _L8("SDES Packet");
	
	rtcpsdespacket.SetAttribute(7, bufPtr);
	
	TPtrC8 value = rtcpsdespacket.Attribute();
	
	if(bufPtr == value)
	{
		SetTestStepResult(EPass);
	}
	else
	{
		SetTestStepResult(EFail);
	}
	
	buffer.Close();
	delete buf;
	return TestStepResult();
	}
	
TVerdict CSDESGetType::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
		
TVerdict CSDESGetType::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates getting timestamp from RTCP SR packet
 */
	{
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KSDESGetType);
	
	RBuf8 buffer;
	
	TRtcpSdesPack rtcpsdespacket(24);
	rtcpsdespacket.AssignL(buffer);
	
	HBufC8* buf;
	buf = HBufC8::NewL(15);
	TPtr8 bufPtr= buf->Des();
	bufPtr = _L8("SDES Packet");
	
	rtcpsdespacket.SetAttribute(7, bufPtr);
	
	TUint8 value = rtcpsdespacket.Type();
	
	if(7 == value)
	{
		SetTestStepResult(EPass);
	}
	else
	{
		SetTestStepResult(EFail);
	}
	
	buffer.Close();
	delete buf;
	return TestStepResult();
	}
	

// 
// APP PACKET
//					

TVerdict CAPPSetSubType::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
		
TVerdict CAPPSetSubType::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates getting octet count from RTCP SR packet
 */
	{
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KAPPSetSubType);
	
	
	HBufC8* appbuf;
	appbuf = HBufC8::NewL(15);
	TPtr8 appbufPtr= appbuf->Des();
	appbufPtr = _L8("--SymbianIndia-");
	 
	HBufC8* appNamebuf;
	appNamebuf = HBufC8::NewL(4);
	TPtr8 appNamePtr= appNamebuf->Des();
	appNamePtr = _L8("FOUR");

	RBuf8 app;
	TRtcpAPPPack appPacket(appbufPtr.Length() + appNamePtr.Length());
	appPacket.AssignL(app);
	appPacket.SetSubType(34);
	appPacket.SetAppData(appbufPtr);
	appPacket.SetAppName(appNamePtr);
	
	// Parsing....
	TRtcpAPPPack recvApp(app);
	
	INFO_PRINTF2(_L("Type %d\n"),recvApp.Type());
	INFO_PRINTF2(_L("APP Type - %d\n"),appPacket.SubType());

	TPtrC8 appName = recvApp.AppName();
	TPtrC8 appData = recvApp.AppData();

	delete appbuf;
	delete appNamebuf;
	app.Close();
	return TestStepResult();
	}

TVerdict CAPPGetSubType::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
		
TVerdict CAPPGetSubType::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates getting timestamp from RTCP SR packet
 */
	{
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KAPPGetSubType);
	
	RBuf8 buffer;
	
	TRtcpAPPPack rtcpapppacket(24);
	rtcpapppacket.AssignL(buffer);
	
	rtcpapppacket.SetSubType(7);
	
	TUint8 value = rtcpapppacket.SubType();
	
	if(7 == value)
	{
		SetTestStepResult(EPass);
	}
	else
	{
		SetTestStepResult(EFail);
	}
	
	buffer.Close();
	return TestStepResult();
	}

TVerdict CAPPGetAppName::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
		
TVerdict CAPPGetAppName::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates getting timestamp from RTCP SR packet
 */
	{
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KAPPGetAppName);
	
	RBuf8 buffer;
	
	TRtcpAPPPack rtcpapppacket(24);
	rtcpapppacket.AssignL(buffer);
	
	HBufC8* buf;
	buf = HBufC8::NewL(15);
	TPtr8 bufPtr= buf->Des();
	bufPtr = _L8("APPN");
	rtcpapppacket.SetAppName(bufPtr);
	
	TPtrC8 value = rtcpapppacket.AppName();
	
	if(bufPtr == value)
	{
		SetTestStepResult(EPass);
	}
	else
	{
		SetTestStepResult(EFail);
	}
	
	buffer.Close();
	delete buf;
	return TestStepResult();
	}
	
// 
// BYE PACKET
//	

TVerdict CBYESetSSRC::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
		
TVerdict CBYESetSSRC::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates getting octet count from RTCP SR packet
 */
	{
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KBYESetSSRC);
	
	RBuf8 buffer;
	
	TRtcpBYEPack rtcpbyepacket(24);
	rtcpbyepacket.AssignL(buffer);
	
	rtcpbyepacket.SetSSRC(1234567);
	
	buffer.Close();
	return TestStepResult();
	}
	
TVerdict CBYESetReason::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
		
TVerdict CBYESetReason::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates getting octet count from RTCP SR packet
 */
	{
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KBYESetReason);
	
	RBuf8 buffer;
	
	TRtcpBYEPack rtcpbyepacket(24);
	rtcpbyepacket.AssignL(buffer);
	
	HBufC8* buf;
	buf = HBufC8::NewL(15);
	TPtr8 bufPtr= buf->Des();
	bufPtr = _L8("BYE");

	rtcpbyepacket.SetReason(bufPtr);
		
	buffer.Close();
	delete buf;
	return TestStepResult();
	}
	
TVerdict CBYEGetSSRC::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
		
TVerdict CBYEGetSSRC::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates getting octet count from RTCP SR packet
 */
	{
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KBYEGetSSRC);
	
	RBuf8 buffer;
	
	TRtcpBYEPack rtcpbyepacket(24);
	rtcpbyepacket.AssignL(buffer);
	
	rtcpbyepacket.SetSSRC(1234567);
	
	TUint32 value = rtcpbyepacket.SSRC();
	
	if(1234567 == value)
	{
		SetTestStepResult(EPass);
	}
	else
	{
		SetTestStepResult(EFail);
	}
	
	buffer.Close();
	return TestStepResult();
	}
	
TVerdict CBYEGetReason::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
		
TVerdict CBYEGetReason::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates getting octet count from RTCP SR packet
 */
	{
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KBYEGetReason);
	

	HBufC8* byebuf;
	byebuf = HBufC8::NewL(15);
	TPtr8 byebufPtr= byebuf->Des();
	byebufPtr = _L8("RealTimeStream");
	
	
	
	RBuf8 bye;
	TRtcpBYEPack byePacket(byebufPtr.Length());
	byePacket.AssignL(bye);
	byePacket.SetSSRC(3456);
	byePacket.SetReason(byebufPtr);
	
	
	// Parsing.....
	TRtcpBYEPack recvBye(bye);
	INFO_PRINTF2(_L("\nSSRC - %d\n"),recvBye.SSRC());
	
	TPtrC8 byereason = recvBye.Reason();
	SetTestStepResult(EPass);
	
	delete byebuf;
	bye.Close();
	return TestStepResult();
	}
	
	
// 
// STATE MACHINE
//

TVerdict CSMTestBase::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	iServer->iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iServer->iScheduler);
	return TestStepResult();
	}

TVerdict CSMTestBase::doTestStepPostambleL()
	{
	SetTestStepResult(EPass);
	CActiveScheduler::Install(0);
	delete iServer->iScheduler;
	iServer->iScheduler = 0;
	return TestStepResult();
	}

TVerdict CSMNewL::doTestStepPreambleL()
	{
	CSMTestBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}
		
TVerdict CSMNewL::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates getting timestamp from RTCP SR packet
 */
	{
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KSMNewL);
		
	CRtpStateMachine *stateMachine = NULL;
	
	stateMachine = CRtpStateMachine::NewL(1);
	
	if(stateMachine)
	{
		SetTestStepResult(EPass);
	}
	else
	{
		SetTestStepResult(EFail);
	}
	
	delete stateMachine;
	
	return TestStepResult();
	}	
	
TVerdict CSMSetRtpStreamParameters::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	CSMTestBase::doTestStepPreambleL();
	return TestStepResult();
	}
		
TVerdict CSMSetRtpStreamParameters::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates getting timestamp from RTCP SR packet
 */
	{
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KSMSetRtpStreamParameters);
	
	RBuf8 buffer;
	
	CRtpStateMachine *stateMachine = NULL;
	
	stateMachine = CRtpStateMachine::NewL(1);
	
	stateMachine->SetRtpStreamParameters(3000, 100, 3);
	
	SetTestStepResult(EPass);
	
	buffer.Close();
	delete stateMachine;
	
	return TestStepResult();
	}	
	
TVerdict CSMMaxDropout::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	CSMTestBase::doTestStepPreambleL();
	return TestStepResult();
	}
		
TVerdict CSMMaxDropout::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates getting timestamp from RTCP SR packet
 */
	{
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KSMMaxDropout);
	
	RBuf8 buffer;
	
	CRtpStateMachine *stateMachine = NULL;
	
	stateMachine = CRtpStateMachine::NewL(1);
	
	stateMachine->SetRtpStreamParameters(3000, 100, 3);
	
	TUint dropOut = stateMachine->MaxDropout();
	
	if(3000 == dropOut)
	{
		SetTestStepResult(EPass);
	}
	else
	{
		SetTestStepResult(EFail);
	}
	
	buffer.Close();
	delete stateMachine;
	
	return TestStepResult();
	}
	
TVerdict CSMMaxMisorder::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	CSMTestBase::doTestStepPreambleL();
	return TestStepResult();
	}
		
TVerdict CSMMaxMisorder::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates getting timestamp from RTCP SR packet
 */
	{
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KSMMaxMisorder);
	
	RBuf8 buffer;
	
	CRtpStateMachine *stateMachine = NULL;
	
	stateMachine = CRtpStateMachine::NewL(1);
	
	stateMachine->SetRtpStreamParameters(3000, 100, 3);
	
	TUint misOrder = stateMachine->MaxMisorder();
	
	if(100 == misOrder)
	{
		SetTestStepResult(EPass);
	}
	else
	{
		SetTestStepResult(EFail);
	}
	
	buffer.Close();
	delete stateMachine;
	
	return TestStepResult();
	}	
	
TVerdict CSMMinSequential::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	CSMTestBase::doTestStepPreambleL();
	return TestStepResult();
	}
		
TVerdict CSMMinSequential::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates getting timestamp from RTCP SR packet
 */
	{
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KSMMaxMisorder);
	
	RBuf8 buffer;
	
	CRtpStateMachine *stateMachine = NULL;
	
	stateMachine = CRtpStateMachine::NewL(1);
	
	stateMachine->SetRtpStreamParameters(3000, 100, 3);
	
	TUint minSequential = stateMachine->MinSequential();
	
	if(3 == minSequential)
	{
		SetTestStepResult(EPass);
	}
	else
	{
		SetTestStepResult(EFail);
	}
	
	buffer.Close();
	delete stateMachine;
	
	return TestStepResult();
	}	
	
TVerdict CSMByeTimeout::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	CSMTestBase::doTestStepPreambleL();
	return TestStepResult();
	}
		
TVerdict CSMByeTimeout::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates getting timestamp from RTCP SR packet
 */
	{
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KSMByeTimeout);
	
	RBuf8 buffer;
	
	CRtpStateMachine *stateMachine = NULL;
	
	stateMachine = CRtpStateMachine::NewL(1);
	
	stateMachine->SetRtpStreamParameters(3000, 100, 3);
	
	TTimeIntervalMicroSeconds32 byeTimeout = stateMachine->ByeTimeout();
	
	if(byeTimeout.Int() == (KRtcpMinTimeMicroSeconds32 * KRtcpByeTimeoutMultiplier))
	{		
		SetTestStepResult(EPass);
	}
	else
	{
		SetTestStepResult(EFail);
	}
	
	buffer.Close();
	delete stateMachine;
	
	return TestStepResult();
	}	
	
TVerdict CSMShortTimeout::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	CSMTestBase::doTestStepPreambleL();
	return TestStepResult();
	}
		
TVerdict CSMShortTimeout::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates getting timestamp from RTCP SR packet
 */
	{
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KSMShortTimeout);
	
	RBuf8 buffer;
	
	CRtpStateMachine *stateMachine = NULL;
	
	stateMachine = CRtpStateMachine::NewL(1);
	
	stateMachine->SetRtpStreamParameters(3000, 100, 3);
	
	TTimeIntervalMicroSeconds32 shortTimeout = stateMachine->ShortTimeout();
	
	if(shortTimeout.Int() == (KRtcpMinTimeMicroSeconds32 * KRtcpShortTimeoutMultiplier))
	{		
		SetTestStepResult(EPass);
	}
	else
	{
		SetTestStepResult(EFail);
	}
	
	buffer.Close();
	delete stateMachine;
	
	return TestStepResult();
	}	
	
TVerdict CSMLongTimeout::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	CSMTestBase::doTestStepPreambleL();
	return TestStepResult();
	}
		
TVerdict CSMLongTimeout::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates getting timestamp from RTCP SR packet
 */
	{
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KSMLongTimeout);
	
	RBuf8 buffer;
	
	CRtpStateMachine *stateMachine = NULL;
	
	stateMachine = CRtpStateMachine::NewL(1);
	
	stateMachine->SetRtpStreamParameters(3000, 100, 3);
	
	TTimeIntervalMicroSeconds32 longTimeout = stateMachine->LongTimeout();
	
	if(longTimeout.Int() == KRtpLongTimeout)
	{		
		SetTestStepResult(EPass);
	}
	else
	{
		SetTestStepResult(EFail);
	}
	
	buffer.Close();
	delete stateMachine;
	
	return TestStepResult();
	}	
	
TVerdict CSMNext::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	CSMTestBase::doTestStepPreambleL();
	return TestStepResult();
	}
		
TVerdict CSMNext::doRtpTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates getting timestamp from RTCP SR packet
 */
	{
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(KSMNext);
	
	RBuf8 buffer;
	
	CRtpStateMachine *stateMachine = NULL;
	stateMachine = CRtpStateMachine::NewL(1);
	stateMachine->StartL();
	stateMachine->SetRtpStreamParameters(3000, 100, 3);
	
	
	CRtpSourceEntry *sourceEntry = NULL;
	HBufC8* buf;
	buf = HBufC8::NewL(15);
	TPtr8 cname1= buf->Des();
	cname1 = _L8("CNAME1");
	
	RSocketServ serv;
	User::LeaveIfError(serv.Connect());
    TUint32 ipAddr3 = INET_ADDR(10,192,192,153);
    TUint port1 = 12101;
    TSockAddr addr(ipAddr3);
    addr.SetPort(port1);
    
	
	CRtpController *rtpController = NULL;
    rtpController = CRtpController::NewL(serv,addr);

	sourceEntry = CRtpSourceEntry::NewLC(1234567, cname1, rtpController);
	
	sourceEntry->IncShiftedCycles(123);
	sourceEntry->ProbationCount();
	sourceEntry->RelTransitTime();
	sourceEntry->IsTimerUsed();
	

	
	int seq =0;
	TRtpSourceState currentState;

		for(int j =0;j<7;j++)
			{
			for(int k=0;k<7;k++)
				{
					for(int i=1; i<=10; i++)
						{	
						sourceEntry->SetState((TRtpSourceState)j);
						currentState = stateMachine->NextL(*sourceEntry,(TRtpSourceEvent)k, seq);
						INFO_PRINTF4(_L("Current State %d  -> Next State %d :On Event %d\n"),j,currentState,k);
						seq = seq + 3001;
					}
				}
			}
	
	CleanupStack::PopAndDestroy(sourceEntry);
	if(rtpController) 
		delete rtpController;
	serv.Close();
	if(stateMachine) 
		delete stateMachine;
    
	buffer.Close();
	delete buf;
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CRtpApiTest::doRtpTestStepL()
{

	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(_L("Rtp Params Test"));
	
	
	RBuf8 param;
	TRtpParam paramPack(0);
	paramPack.AssignL(param);
	paramPack.SetRtpParam(EBandwidth,3000);
	
	
	TRtpParam getPack((TDes8 &)param);
	INFO_PRINTF2(_L("PacketType %d"),getPack.Type());
	INFO_PRINTF2(_L("RtpParamType %d"),getPack.GetParamType());
	INFO_PRINTF2(_L("RtpParamValue %d"),getPack.GetParam());
	
	
	RBuf8 param2;
	TRtpParam paramPack2(0);
	paramPack2.AssignL(param2);
	HBufC8* parambuf;
	parambuf = HBufC8::NewL(4);
	TPtr8 paramPtr= parambuf->Des();
	paramPtr = _L8("FOUR");
	
	paramPack2.SetRtpParam(ERtcpCompound,paramPtr);
	TPtrC8 data = paramPack2.GetData();
	
	param.Close();
	param2.Close();
	delete parambuf;
	
	SetTestStepResult(EPass);
	return TestStepResult();
}


TVerdict CRtpUtilsTest::doRtpTestStepL()
	{
	
	INFO_PRINTF1(_L("In Step:"));
	INFO_PRINTF1(_L("Rtp Params Test"));
	
	TNTPTime time1(5600,0);
	TNTPTime time2(2000,0);
	
	time1 += time2;
	time1 -=time2;
	time1 = time1 + time2;
	time1 = time1 - time2;
	time2 = time2 * 2; 
	
	TUint32 interval = 5670;
	
	time2 = interval;
	TTimeIntervalMicroSeconds32 mseconds(45566);
	time2 = mseconds;
	
	
	TRTPTime rtime;
	rtime.SetTimeConversion(2000,0);
	rtime.SetTimeStamp(300);
	
	
	//Panic((TRtpCorePanicCode)0);
	SetTestStepResult(EPass);
	return TestStepResult();
	
	}
