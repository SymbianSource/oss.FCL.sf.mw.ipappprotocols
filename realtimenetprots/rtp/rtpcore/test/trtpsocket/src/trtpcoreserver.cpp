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
// server class for the rtp core unit test cases
// 
//

/**
 @file
 @internalComponent
*/
 
#include "trtpcoreserver.h"
#include "trtpcorestep.h"

static const TInt KStartOfItemValue = 2;
static const TInt KStartOfFirstItemValue = 10;

// __EDIT_ME__ - Substitute the name of your test server 
_LIT(KServerName,"trtpsocket");
// __EDIT_ME__ - Use your own server class name
CTRtpCoreServer* CTRtpCoreServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTRtpCoreServer* server = new (ELeave) CTRtpCoreServer();
	CleanupStack::PushL(server);
	//CServer call
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

CTRtpCoreServer::CTRtpCoreServer()
	{
	iUsingConnectedSocket = EFalse;
	}

CTRtpCoreServer::~CTRtpCoreServer()
	{
	delete iScheduler;
	//
	delete iController;
	delete iSendController;
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
	CTRtpCoreServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTRtpCoreServer::NewL());
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
	CTRtpCoreServer* server = NULL;
	// Create the CTestServer derived server
	// __EDIT_ME__ Your server name
	TRAPD(err,server = CTRtpCoreServer::NewL());
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

// Only a DLL on emulator for typhoon and earlier
#if (defined __WINS__ && !defined EKA2)
// and E32Dll(enum TDllReason) defined in rtputils.cpp
/*
GLDEF_C TInt E32Dll(enum TDllReason)
	{
	return 0;
	}
*/
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
	TInt err = 0;
	TRAP(err,MainL());
	delete cleanup;
	return err;
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


CTestStep* CTRtpCoreServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTRtpCoreStep* testStep = NULL;
	// __EDIT_ME__ - Create your own test steps here
	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	DEF_TEST(KCreateRtpController,CCreateRtpController);
	DEF_TEST(KActivateRtpController,CActivateRtpController);
	DEF_TEST(KDeleteRtpController,CDeleteRtpController);
	DEF_TEST(KBasicSend,CBasicSend);
	DEF_TEST(KBasicReceive,CBasicReceive);
	DEF_TEST(KInitialReceive,CInitialReceive);
	DEF_TEST(KFixedHeader,CFixedHeader);
	DEF_TEST(KRRFixedHeader,CRRFixedHeader);
	DEF_TEST(KBYEReceive,CBYEReceive);
	
	DEF_TEST(KOom,COom);
	
	DEF_TEST(KIntervalTest, CIntervalTest);
	DEF_TEST(KConsiderationTest,CConsiderationTest);

	if (testStep)
		testStep->SetServer(this);
	

	return testStep;
	}

/*
void CTRtpCoreServer::StopScheduler(CTRtpCoreServer* aPtr, const TRtpEvent& aEvent)
	{
	aPtr->iLastEvent = aEvent;
	if (!aPtr->iStopping && !--aPtr->iStopCount)
		{
		CActiveScheduler::Stop();
		aPtr->iStopping = ETrue;
		}
	}

*/

CWaiter* CWaiter::NewLC(CTRtpCoreServer* aServer)
	{
	CWaiter* that = new (ELeave) CWaiter(aServer);
	CleanupStack::PushL(that);
	return that;
	}

void CWaiter::DataAvailable(TInt)
    {
    if (!iServer->iStopping && !--iServer->iStopCount)
        {
        CActiveScheduler::Stop();
        //iServer->iWaitScheduler.AsyncStop();
        iServer->iStopping = ETrue;
        }
    }

CWaiter::~CWaiter()
	{
	
	}

TBool CTRtpCoreServer::CompareRtpHeadersFromBuf(TDesC8& aBuf1,TDesC8& aBuf2)
	{
	for (TInt i = 0; i < 12/*packet->Length()*/; i++)
		{
		// Don't compare the SSRC or timestamp as it's random.
		//if ((i < 8 || i > 11) && (i != 2 && i != 3))
			if (aBuf1[i] != aBuf2[i])
				return EFalse;
				
		}
	return ETrue;
	}

TBool CTRtpCoreServer::CompareRtcpHeadersFromBuf(TDesC8& aBuf1,TDesC8& aBuf2)
	{
	// Don't compare the SSRC as it's random.
	for (TInt i = 0; i < 4; i++)
		{
		if (aBuf1[i] != aBuf2[i])
			return EFalse;	
		}
	return ETrue;
	}

TBool CTRtpCoreServer::CompareRtpHeaderWithBuf(TRtpFixedHdrMemLayout& aHeader,TDesC8& aBuf)	
	{
	TUint8 byte;
	//byte 1
	byte = static_cast<TUint8>(aHeader.Version()); // 2 bits
	// padding 1 bit
	byte = static_cast<TUint8>(aHeader.HasPadding() ? (byte << 1)|1 : (byte << 1)); 
	// extension 1 bit
	byte = static_cast<TUint8>(aHeader.HasExtension() ? (byte << 1)|1 : (byte << 1)); 
	// cc count 4 bits
	byte = static_cast<TUint8>((byte << 4)|aHeader.CC()); 
	if (byte != aBuf[0])
		return EFalse;
	// byte 2
	// marker 1 bit
	byte = static_cast<TUint8>(aHeader.HasMarker() ? 1 : 0); 
	// payload type 7 bits
	byte = static_cast<TUint8>((byte << 7)|aHeader.PayloadType()); 
	if (byte != aBuf[1])
		return EFalse;
	// byte 3
	byte = static_cast<TUint8>(aHeader.SequenceNumber() >> 8); // upper byte
	if (byte != aBuf[2])
		return EFalse;
	// byte 4
	byte = static_cast<TUint8>(aHeader.SequenceNumber()&0x0000ffff); // lower byte
	if (byte != aBuf[3])
		return EFalse;
	// TIMESTAMP
	// byte 5 MSB from timestamp
	byte = static_cast<TUint8>(aHeader.TimeStamp() >> 24); // most signicifant byte of timestamp
	if (byte != aBuf[4])
		return EFalse;
	// byte 6
	byte = static_cast<TUint8>(aHeader.TimeStamp() >> 16); // timestamp
	if (byte != aBuf[5])
		return EFalse;
	// byte 7
	byte = static_cast<TUint8>(aHeader.TimeStamp() >> 8); // timestamp
	if (byte != aBuf[6])
		return EFalse;
	// byte 8 LSB from timestamp
	// least signicifant byte of timestamp
	byte = static_cast<TUint8>(aHeader.TimeStamp()&0x000000000000ffff); 
	if (byte != aBuf[7])
		return EFalse;
	// SSRC
	// byte 9 MSB from SSRC
	byte = static_cast<TUint8>(aHeader.SSRC() >> 24); // most signicifant byte of SSRC
	if (byte != aBuf[8])
		return EFalse;
	// byte 10
	byte = static_cast<TUint8>(aHeader.SSRC() >> 16); // SSRC
	if (byte != aBuf[9])
		return EFalse;
	// byte 11
	byte = static_cast<TUint8>(aHeader.SSRC() >> 8); // timestamp
	if (byte != aBuf[10])
		return EFalse;
	// byte 12 LSB from SSRC
	// least signicifant byte of SSRC
	byte = static_cast<TUint8>(aHeader.SSRC()&0x000000000000ffff); 
	if (byte != aBuf[11])
		return EFalse;

	return ETrue;
	}

TBool CTRtpCoreServer::CompareRtcpHeaderWithBuf(TRtcpFixedHdrMemLayout& aHeader,TDesC8& aBuf)	
	{
	TUint8 byte;
	//byte 1
	byte = static_cast<TUint8>(aHeader.Version()); // 2 bits
	// padding 1 bit
	byte = static_cast<TUint8>(aHeader.IsPadding() ? (byte << 1)|1 : (byte << 1));  
	// cc count 5 bits
	byte = static_cast<TUint8>((byte << 5)|aHeader.Count()); 
	if (byte != aBuf[0])
		return EFalse;
	// byte 2 
	// packet type 8 bits
	byte = static_cast<TUint8>(aHeader.PacketType()); 
	if (byte != aBuf[1])
		return EFalse;
	// byte 3
	byte = static_cast<TUint8>(aHeader.Length() >> 8); // upper byte
	if (byte != aBuf[2])
		return EFalse;
	// byte 4
	byte = static_cast<TUint8>(aHeader.Length()&0x0000ffff); // lower byte
	if (byte != aBuf[3])
		return EFalse;

	return ETrue;
	}

/*
_LIT(KRTP_CORE, "RTP-CORE");
GLDEF_C void Panic(TRtpCorePanicCode aPanicCode)
	{
	User::Panic(KRTP_CORE, aPanicCode);
	}
	*/

CRtpFixedHeader::CRtpFixedHeader()
  	{
  	
  	}
   
CRtpFixedHeader::CRtpFixedHeader(TDes8& aPacket)
  	{
  	TRtpFixedHdrMemLayout header(aPacket);
  	iHeader.iVersion = header.Version();	// protocol version: 2 bits
  	iHeader.iP = header.HasPadding();		// padding flag: 1 bit
  	iHeader.iX = header.HasExtension();		// header extension flag: 1 bit
  	iHeader.iCC = header.CC();				// CSRC count:  4 bits  	
  	iHeader.iM = header.HasMarker();			// marker bit: 1 bit
  	iHeader.iPT = header.PayloadType();		// payload type: 7 bits
  	iHeader.iSequenceNo = header.SequenceNumber();	// sequence number 16 bits
  	iHeader.iTimeStamp = header.TimeStamp();	// timestamp: 32 bits
  	iHeader.iSSRC = header.SSRC();		// synchronization source: 32 bits
  	
  	/*
  	TUint8* csrc = const_cast<unsigned char*>(aPacket.Ptr()+12);
  	for(TUint i = 0;i < iHeader.cc; i++)
  		{
  		iHeader.iCSRC.Append((*(csrc+3))+(*(csrc+2)<<8)+(*(csrc+1)<<16)+(*(csrc)<<24));
  		csrc += 4;
  		}
	*/
  	}
  
CRtpFixedHeader::~CRtpFixedHeader()
  	{
  	//iHeader.iCSRC.Close();
  	}
  
void CRtpFixedHeader::SetVersion(TUint aVersion)
  	{
  	iHeader.iVersion = aVersion;
  	}
  
void CRtpFixedHeader::SetCSRCCount(TUint aCC)
  	{
  	iHeader.iCC = aCC;
  	}
  
void CRtpFixedHeader::SetSequenceNumber(TUint16 aNo)
  	{
  	iHeader.iSequenceNo = aNo;
  	}
  
void CRtpFixedHeader::SetSSRC(TUint32 aSSRC)
  	{
  	iHeader.iSSRC = aSSRC;
  	}
  
void CRtpFixedHeader::SetTimeStamp(TUint32 aTime)
  	{
  	iHeader.iTimeStamp = aTime;
  	}
  
void CRtpFixedHeader::SetPadding(TBool aPadding)
  	{
  	iHeader.iP = aPadding ? 1 : 0;
  	}
  
void CRtpFixedHeader::SetExtension(TBool aExtension)
  	{
  	iHeader.iX = aExtension ? 1 : 0;
  	}
  
TUint32 CRtpFixedHeader::SSRC()
  	{
  	return iHeader.iSSRC;
  	}
  
TUint8 CRtpFixedHeader::CC()
  	{
  	return iHeader.iCC;
  	}
  
TUint8 CRtpFixedHeader::PayloadType()
  	{
  	return iHeader.iPT;
  	}
  
TBool CRtpFixedHeader::IsMarker()
  	{
  	return iHeader.iM;
  	}
  
TBool CRtpFixedHeader::IsPadding()
  	{
  	return iHeader.iP;
  	}
  
TBool CRtpFixedHeader::IsExtension()
  	{
  	return iHeader.iX;
  	}
  
TUint8 CRtpFixedHeader::Version()
  	{
  	return iHeader.iVersion;
  	}
  
TUint16 CRtpFixedHeader::SequenceNumber()
  	{
  	return iHeader.iSequenceNo;
  	}
  
TUint32 CRtpFixedHeader::TimeStamp()
  	{
  	return iHeader.iTimeStamp;
  	}
//
CRtcpPacketTest::CRtcpPacketTest(TDes8& aPacket)
	{
	TRtcpFixedHdrMemLayout header(aPacket);
	iPacket.common.iVersion = header.Version();
	iPacket.common.iPadding = header.IsPadding() ? 1 : 0;
	iPacket.common.iCount = header.Count();
	iPacket.common.iPacketType = header.PacketType();
	iPacket.common.iLength = static_cast<TUint16>(header.Length());
	}
//CRtcpRRPacket();

void CRtcpPacketTest::Init(TDes8& aPacket)
	{
	TRtcpFixedHdrMemLayout header(aPacket);
	iPacket.common.iVersion = header.Version();
	iPacket.common.iPadding = header.IsPadding() ? 1 : 0;
	iPacket.common.iCount = header.Count();
	iPacket.common.iPacketType = header.PacketType();
	iPacket.common.iLength = static_cast<TUint16>(header.Length());
	}

CRtcpPacketTest::~CRtcpPacketTest()
	{
	}

TBool CRtcpPacketTest::operator==(CRtcpPacketTest& aPacket)
	{
	TBool result = ETrue;
	if(iPacket.common.iVersion != aPacket.iPacket.common.iVersion)
		result = EFalse;
	if(iPacket.common.iPadding != aPacket.iPacket.common.iPadding)
		result = EFalse;
	if(iPacket.common.iCount != aPacket.iPacket.common.iCount)
		result = EFalse;
	if(iPacket.common.iPacketType != aPacket.iPacket.common.iPacketType)
		result = EFalse;
	if(iPacket.common.iLength != aPacket.iPacket.common.iLength)
		result = EFalse;
	return result;
	}

TBool CRtcpPacketTest::operator!=(CRtcpPacketTest& aPacket)
	{
	return !this->operator==(aPacket);
	}

void CRtcpPacketTest::SetVersion(TUint aVersion)
	{
	iPacket.common.iVersion = aVersion;
	}

void CRtcpPacketTest::SetPadding(TBool aPadding)
	{
	iPacket.common.iPadding = aPadding ? 1 : 0;
	}

void CRtcpPacketTest::SetCount(TUint8 aCount)
	{
	iPacket.common.iCount = aCount;
	}

void CRtcpPacketTest::SetPacketType(TRtcpPacketType aPacketType)
	{
	iPacket.common.iPacketType = aPacketType;
	}

void CRtcpPacketTest::SetLength(TUint16 aLength)
	{
	iPacket.common.iLength = aLength;
	}

TUint CRtcpPacketTest::Version()
	{
	return iPacket.common.iVersion;
	}

TBool CRtcpPacketTest::IsPadding()
	{
	return iPacket.common.iPadding;
	}

TUint8 CRtcpPacketTest::Count()
	{
	return iPacket.common.iCount;
	}

TRtcpPacketType CRtcpPacketTest::PacketType()
	{
	return static_cast<TRtcpPacketType>(iPacket.common.iPacketType);
	}

TUint16 CRtcpPacketTest::Length()
	{
	return iPacket.common.iLength;
	}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
CRtcpRRPacketTest::CRtcpRRPacketTest(TDes8& aPacket) : CRtcpPacketTest(aPacket)
	{
	TInt i = 0;
	//
	TRtcpFixedHdrMemLayout header(aPacket);
	iPacket.specific.rr.iSSRC = header.SSRC();
	//
	//TPtr8 data(const_cast<TUint8*>(aPacket.Ptr()+8),aPacket.Length());
	TPtr8 data(const_cast<TUint8*>(aPacket.Ptr()),aPacket.Length(),aPacket.Length());
	TRtcpReportBlockMemLayout rrBlock(data);
	//
	iPacket.specific.rr.iRR[i].iSSRC = rrBlock.SSRC();
	iPacket.specific.rr.iRR[i].iFractionLost =  rrBlock.Fraction();
	iPacket.specific.rr.iRR[i].iCumulLost = rrBlock.CumulativeLost();
	iPacket.specific.rr.iRR[i].iExtendedLastSeq = rrBlock.SequenceHigh();
	iPacket.specific.rr.iRR[i].iJitter = rrBlock.Jitter();
	iPacket.specific.rr.iRR[i].iLastSR = rrBlock.LastSR();
	iPacket.specific.rr.iRR[i].iDSLR = rrBlock.LastDelay();
	
	}

CRtcpRRPacketTest::~CRtcpRRPacketTest()
	{
	}

TBool CRtcpRRPacketTest::operator==(CRtcpRRPacketTest& aPacket)
	{
	TBool result = EFalse;
	if(CRtcpPacketTest::operator==(aPacket))
		{
		result = ETrue;
		if(this->SenderSSRC() != aPacket.SenderSSRC())
			result = EFalse;
		if(this->ReportSSRC() != aPacket.ReportSSRC())
			result = EFalse;
		if(this->Fraction() != aPacket.Fraction())
			result = EFalse;
		if(this->Lost() != aPacket.Lost())
			result = EFalse;
		if(this->HighestSequence() != aPacket.HighestSequence())
			result = EFalse;
		if(this->Jitter() != aPacket.Jitter())
			result = EFalse;
		/* enable this?
		if(this->LastSR() != aPacket.LastSR())
			result = EFalse;
		if(this->DSLR() != aPacket.DSLR())
			result = EFalse;
		*/
		}
	return result;
	}

TBool CRtcpRRPacketTest::operator!=(CRtcpRRPacketTest& aPacket)
	{
	return !this->operator==(aPacket);
	}

void CRtcpRRPacketTest::SetSenderSSRC(TUint32 aSSRC)
	{
	iPacket.specific.rr.iSSRC = aSSRC;
	}

void CRtcpRRPacketTest::SetReportSSRC(TUint32 aSSRC)
	{
	iPacket.specific.rr.iRR[0].iSSRC = aSSRC;
	}

void CRtcpRRPacketTest::SetFraction(TUint8 aFraction)
	{
	iPacket.specific.rr.iRR[0].iFractionLost = aFraction;
	}

void CRtcpRRPacketTest::SetLost(TUint aLost)
	{
	iPacket.specific.rr.iRR[0].iCumulLost = aLost;
	}

void CRtcpRRPacketTest::SetHighestSequence(TUint32 aSeq)
	{
	iPacket.specific.rr.iRR[0].iExtendedLastSeq = aSeq;
	}

void CRtcpRRPacketTest::SetJitter(TUint32 aJitter)
	{
	iPacket.specific.rr.iRR[0].iJitter = aJitter;
	}

void CRtcpRRPacketTest::SetLastSR(TUint32 aLastSR)
	{
	iPacket.specific.rr.iRR[0].iLastSR = aLastSR;
	}

void CRtcpRRPacketTest::SetDSLR(TUint32 aDSLR)
	{
	iPacket.specific.rr.iRR[0].iDSLR = aDSLR;
	}

   
TUint32 CRtcpRRPacketTest::SenderSSRC()
	{
	return iPacket.specific.rr.iSSRC;
	}

TUint32 CRtcpRRPacketTest::ReportSSRC()
	{
	return iPacket.specific.rr.iRR[0].iSSRC;
	}

TUint8 CRtcpRRPacketTest::Fraction()
	{
	return iPacket.specific.rr.iRR[0].iFractionLost;
	}

TUint CRtcpRRPacketTest::Lost()
	{
	return iPacket.specific.rr.iRR[0].iCumulLost;
	}

TUint32 CRtcpRRPacketTest::HighestSequence()
	{
	return iPacket.specific.rr.iRR[0].iExtendedLastSeq;
	}

TUint32 CRtcpRRPacketTest::Jitter()
	{
	return iPacket.specific.rr.iRR[0].iJitter;
	}

TUint32 CRtcpRRPacketTest::LastSR()
	{
	return iPacket.specific.rr.iRR[0].iLastSR;
	}

TUint32 CRtcpRRPacketTest::DSLR()
	{
	return iPacket.specific.rr.iRR[0].iDSLR;
	}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
CRtcpSRPacketTest::CRtcpSRPacketTest(TDes8& aPacket) : CRtcpPacketTest(aPacket)
	{
	TInt i = 0;
	//
	TRtcpFixedHdrMemLayout header(aPacket);
	iPacket.specific.sr.iSSRC = header.SSRC();
	// Sender Info
	TPtr8 data(const_cast<TUint8*>(aPacket.Ptr()),aPacket.Length(),aPacket.Length());
	TRtcpSRSenderInfoMemLayout senderInfo(data,8);
	iPacket.specific.sr.iNtpSeconds = senderInfo.NtpMsb();
	iPacket.specific.sr.iNtpFraction = senderInfo.NtpLsb();
	iPacket.specific.sr.iRtpTimeStamp = senderInfo.RtpTimeStamp();
	iPacket.specific.sr.iPacketsSent = senderInfo.PacketCount();
	iPacket.specific.sr.iOctetsSent = senderInfo.OctetCount();
	//
	// RR data
	data.Set(const_cast<TUint8*>(aPacket.Ptr()),aPacket.Length(),aPacket.Length());
	TRtcpReportBlockMemLayout rrBlock(data,28);
	//
	iPacket.specific.sr.iRR[i].iSSRC = rrBlock.SSRC();
	iPacket.specific.sr.iRR[i].iFractionLost =  rrBlock.Fraction();
	iPacket.specific.sr.iRR[i].iCumulLost = rrBlock.CumulativeLost();
	iPacket.specific.sr.iRR[i].iExtendedLastSeq = rrBlock.SequenceHigh();
	iPacket.specific.sr.iRR[i].iJitter = rrBlock.Jitter();
	iPacket.specific.sr.iRR[i].iLastSR = rrBlock.LastSR();
	iPacket.specific.sr.iRR[i].iDSLR = rrBlock.LastDelay();
	
	}

CRtcpSRPacketTest::~CRtcpSRPacketTest()
	{
	}

TBool CRtcpSRPacketTest::operator==(CRtcpSRPacketTest& aPacket)
	{
	TBool result = EFalse;
	if(CRtcpPacketTest::operator==(aPacket))
		{
		result = ETrue;
		if(this->SenderSSRC() != aPacket.SenderSSRC())
			result = EFalse;
		//
		if(this->NtpMsb() != aPacket.NtpMsb())
			result = EFalse;
		if(this->NtpLsb() != aPacket.NtpLsb())
			result = EFalse;
		if(this->RtpTimeStamp() != aPacket.RtpTimeStamp())
			result = EFalse;
		if(this->PacketCount() != aPacket.PacketCount())
			result = EFalse;
		if(this->OctetCount() != aPacket.OctetCount())
			result = EFalse;
		//
		if(this->ReportSSRC() != aPacket.ReportSSRC())
			result = EFalse;
		if(this->Fraction() != aPacket.Fraction())
			result = EFalse;
		if(this->Lost() != aPacket.Lost())
			result = EFalse;
		if(this->HighestSequence() != aPacket.HighestSequence())
			result = EFalse;
		if(this->Jitter() != aPacket.Jitter())
			result = EFalse;
		/* enable this?!?
		if(this->LastSR() != aPacket.LastSR())
			result = EFalse;
		if(this->DSLR() != aPacket.DSLR())
			result = EFalse;
		*/
		}

	return result;
	}

TBool CRtcpSRPacketTest::operator!=(CRtcpSRPacketTest& aPacket)
	{
	return !this->operator==(aPacket);
	}

void CRtcpSRPacketTest::SetSenderSSRC(TUint32 aSSRC)
	{
	iPacket.specific.sr.iSSRC = aSSRC;
	}

void CRtcpSRPacketTest::SetReportSSRC(TUint32 aSSRC)
	{
	iPacket.specific.sr.iRR[0].iSSRC = aSSRC;
	}

void CRtcpSRPacketTest::SetFraction(TUint8 aFraction)
	{
	iPacket.specific.sr.iRR[0].iFractionLost = aFraction;
	}

void CRtcpSRPacketTest::SetLost(TUint aLost)
	{
	iPacket.specific.sr.iRR[0].iCumulLost = aLost;
	}

void CRtcpSRPacketTest::SetHighestSequence(TUint32 aSeq)
	{
	iPacket.specific.sr.iRR[0].iExtendedLastSeq = aSeq;
	}

void CRtcpSRPacketTest::SetJitter(TUint32 aJitter)
	{
	iPacket.specific.sr.iRR[0].iJitter = aJitter;
	}

void CRtcpSRPacketTest::SetLastSR(TUint32 aLastSR)
	{
	iPacket.specific.sr.iRR[0].iLastSR = aLastSR;
	}

void CRtcpSRPacketTest::SetDSLR(TUint32 aDSLR)
	{
	iPacket.specific.sr.iRR[0].iDSLR = aDSLR;
	}

   
TUint32 CRtcpSRPacketTest::SenderSSRC()
	{
	return iPacket.specific.sr.iSSRC;
	}

TUint32 CRtcpSRPacketTest::ReportSSRC()
	{
	return iPacket.specific.sr.iRR[0].iSSRC;
	}

TUint8 CRtcpSRPacketTest::Fraction()
	{
	return iPacket.specific.sr.iRR[0].iFractionLost;
	}

TUint CRtcpSRPacketTest::Lost()
	{
	return iPacket.specific.sr.iRR[0].iCumulLost;
	}

TUint32 CRtcpSRPacketTest::HighestSequence()
	{
	return iPacket.specific.sr.iRR[0].iExtendedLastSeq;
	}

TUint32 CRtcpSRPacketTest::Jitter()
	{
	return iPacket.specific.sr.iRR[0].iJitter;
	}

TUint32 CRtcpSRPacketTest::LastSR()
	{
	return iPacket.specific.sr.iRR[0].iLastSR;
	}

TUint32 CRtcpSRPacketTest::DSLR()
	{
	return iPacket.specific.sr.iRR[0].iDSLR;
	}

void CRtcpSRPacketTest::SetNtpMsb(TUint32 aNtpMsb)
	{
	iPacket.specific.sr.iNtpSeconds = aNtpMsb;
	}

void CRtcpSRPacketTest::SetNtpLsb(TUint32 aNtpLsb)
	{
	iPacket.specific.sr.iNtpFraction = aNtpLsb;
	}

void CRtcpSRPacketTest::SetRtpTimeStamp(TUint32 aRtpTs)
	{
	iPacket.specific.sr.iRtpTimeStamp = aRtpTs;
	}

void CRtcpSRPacketTest::SetPacketCount(TUint32 aPacketCount)
	{
	iPacket.specific.sr.iPacketsSent = aPacketCount;
	}

void CRtcpSRPacketTest::SetOctetCount(TUint32 aOctetCount)
	{
	iPacket.specific.sr.iOctetsSent = aOctetCount;
	}

TUint32 CRtcpSRPacketTest::NtpMsb()
	{
	return iPacket.specific.sr.iNtpSeconds;
	}

TUint32 CRtcpSRPacketTest::NtpLsb()
	{
	return iPacket.specific.sr.iNtpFraction;
	}

TUint32 CRtcpSRPacketTest::RtpTimeStamp()
	{
	return iPacket.specific.sr.iRtpTimeStamp;
	}

TUint32 CRtcpSRPacketTest::PacketCount()
	{
	return iPacket.specific.sr.iPacketsSent;
	}

TUint32 CRtcpSRPacketTest::OctetCount()
	{
	return iPacket.specific.sr.iOctetsSent;
	}
//

TBool IsSDESPresent( const TUint8* aPacketPtr ) 
	{
/*
0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
header |V=2|P|    SC   |  PT=SDES=202  |             length            |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+

*/
	//SDES is present if 2nd byte contains a value to be 202
	return aPacketPtr[1] == 202;
	}

TInt CRtcpByePacketTest::ReadSROrRRAndSDESAndIncOffset(const TUint8** aPtr)
	{
	//Bypass the RR or SR packet
	TUint16 increaseBy = iPacket.common.iLength;
	increaseBy = static_cast<TUint16>((increaseBy * 4) + 4);
	*aPtr = (*aPtr) + increaseBy;

	//By-pass the SDES, if present...
	//Check if the ensuing packet is SDES or not
	//TBool sdesFollows = ::IsSDESPresent( *aPtr );
	TUint16 sdesIncreaseBy = 0;
	if( ::IsSDESPresent( *aPtr ) )
		{
		//If yes, increase the offset further
		TUint8* tmp = const_cast<TUint8*>( *aPtr );
		TPtr8 t(tmp, 1024+sdesIncreaseBy);//Assuing that max length of sdes is <1024
		TRtcpFixedHdrMemLayout r(t);

		sdesIncreaseBy = static_cast<TUint16>(r.Length());
		sdesIncreaseBy = static_cast<TUint16>((sdesIncreaseBy * 4) + 4);
		*aPtr = (*aPtr) + sdesIncreaseBy;
		}
	
	return increaseBy+sdesIncreaseBy;
	}




CRtcpByePacketTest::CRtcpByePacketTest(TDes8& aPacket, TBool aIncreaseOffset) : CRtcpPacketTest(aPacket), iOffset(0)
	{
	
	const TUint8* ptr = aPacket.Ptr();
	if( aIncreaseOffset )
		{
		iOffset = ReadSROrRRAndSDESAndIncOffset(&ptr);	
		}
	//TPtr8 temp(const_cast<TUint8*>(ptr+iOffset), aPacket.Length() - iOffset + 1);
	TPtr8 temp(const_cast<TUint8*>(ptr), aPacket.Length() - iOffset + 1);

	//Run the init() function after the pointer adjustment is done
	Init(temp);		

	TRtcpFixedHdrMemLayout header(temp);
	iPacket.specific.bye.iSRC[0] = header.SSRC();	
	}

CRtcpByePacketTest::~CRtcpByePacketTest()
	{
	}

TBool CRtcpByePacketTest::operator==(CRtcpByePacketTest& aPacket)
	{
	TBool result = EFalse;
	if(CRtcpPacketTest::operator==(aPacket))
		{
		if(iPacket.specific.bye.iSRC[0] == aPacket.iPacket.specific.bye.iSRC[0])
			result = ETrue;
		}
	return result;
	}

TBool CRtcpByePacketTest::operator!=(CRtcpByePacketTest& aPacket)
	{
	return !this->operator==(aPacket);
	}

void CRtcpByePacketTest::SetSSRC(TUint32 aSSRC)
	{
	iPacket.specific.bye.iSRC[0] = aSSRC;
	}   	

TUint32 CRtcpByePacketTest::SSRC()
	{
	return iPacket.specific.bye.iSRC[0];
	}

//
CRtcpAppPacketTest* CRtcpAppPacketTest::NewL(TDes8& aPacket)
	{
	CRtcpAppPacketTest* t = new(ELeave) CRtcpAppPacketTest(aPacket);
	CleanupStack::PushL(t);
	t->ConstructL(aPacket);
	CleanupStack::Pop(t);
	return t;
	}

CRtcpAppPacketTest::CRtcpAppPacketTest(TDes8& aPacket) : CRtcpPacketTest(aPacket)														
	{		
	}




void CRtcpAppPacketTest::ConstructL(TDes8& aPacket)
	{
	TRtcpFixedHdrMemLayout header(aPacket);
	iPacket.specific.app.iSRC = header.SSRC();
	iPacket.specific.app.iAppName = aPacket.Mid(8, 4).AllocL();
	iPacket.specific.app.iAppData = aPacket.Mid(12).AllocL();
	}

CRtcpAppPacketTest::~CRtcpAppPacketTest()
	{
	}

TBool CRtcpAppPacketTest::operator==(CRtcpAppPacketTest& aPacket)
	{
	TBool result = EFalse;
	if(CRtcpPacketTest::operator==(aPacket))
		{
		if(iPacket.specific.app.iSRC == aPacket.iPacket.specific.app.iSRC)
			result = ETrue;
		}
	return result;
	}

TBool CRtcpAppPacketTest::operator!=(CRtcpAppPacketTest& aPacket)
	{
	return !this->operator==(aPacket);
	}

void CRtcpAppPacketTest::SetSSRC(TUint32 aSSRC)
	{
	iPacket.specific.app.iSRC = aSSRC;
	}   	

TUint32 CRtcpAppPacketTest::SSRC()
	{
	return iPacket.specific.app.iSRC;
	}

void CRtcpAppPacketTest::SetAppData(const TDesC8& aAppData)
	{
	*(iPacket.specific.app.iAppData) = aAppData;
	}

const TDesC8& CRtcpAppPacketTest::AppData()
	{
	return *(iPacket.specific.app.iAppData);
	}

void CRtcpAppPacketTest::SetAppName(const TDesC8& aAppName)
	{
	*(iPacket.specific.app.iAppName) = aAppName;
	}

const TDesC8& CRtcpAppPacketTest::AppName()
	{
	return *(iPacket.specific.app.iAppName);
	}

//
CItem* CItem::NewL( const TDesC8& aItem, TInt aType )
	{
	CItem* t = new(ELeave) CItem( static_cast<TUint8>(aType) );
	CleanupStack::PushL( t );
	t->ConstructL( aItem );
	CleanupStack::Pop( t ); 
	return t;
	}

TInt CItem::FlatCompare(const CItem& aItemOne, const CItem& aItemTwo )
	{
	return aItemOne.DoFlatCompare(aItemTwo);
	}

TInt CItem::DoFlatCompare( const CItem& aItem ) const
	{
	return ((iType == aItem.iType) && (*aItem.iData == *iData));	
	}

void CItem::ConstructL( const TDesC8& aItem )
	{
	iData = aItem.AllocL();
	}

CItem::CItem( TUint8 aType ) : iType( aType )
	{
	}

CItem::~CItem()
	{
	delete iData;
	}

//

CRtcpSDESPacketTest* CRtcpSDESPacketTest::NewL(TDes8& aPacket)
	{
	CRtcpSDESPacketTest* t = new(ELeave) CRtcpSDESPacketTest(aPacket);
	CleanupStack::PushL(t);
	t->ConstructL(aPacket);
	CleanupStack::Pop(t);
	return t;
	}

CRtcpSDESPacketTest::CRtcpSDESPacketTest(TDes8& aPacket) : CRtcpPacketTest(aPacket), 															
															iPktSize( aPacket.Length() ),
															iItemCount(0),
															iStartOfPkt( aPacket.Ptr() )														
	{		
	}


void CRtcpSDESPacketTest::ConstructL(TDes8& aPacket)
	{
	const TUint8* ptr = aPacket.Ptr();
	iOffset = SROrRRLength();	
	ptr += iOffset;

	TInt len = aPacket.Length() - iOffset ;
	TPtrC8 temp(const_cast<TUint8*>(ptr), len );

	TRtcpFixedHdrMemLayout header(temp);
	iPacket.common.iVersion = header.Version();
	iPacket.common.iPadding = header.IsPadding() ? 1 : 0;
	iPacket.common.iCount = header.Count();
	iPacket.common.iPacketType = header.PacketType();
	iPacket.common.iLength = static_cast<TUint16>(header.Length());
	iPacket.specific.sdes.iSRC = header.SSRC();

	_LIT( KSDESTest, "SDES Test" );
	__ASSERT_ALWAYS( iItemCount == 0, User::Panic( KSDESTest, ERtpNotSupported ) );
	TInt i = iItemCount;
	
	TRtcpSDESItem* sdesItemPtr = &iPacket.specific.sdes.iItem[i];

	sdesItemPtr->iType = ptr[8];
	sdesItemPtr->iLength = ptr[9];
	sdesItemPtr->iData = TPtrC8(ptr + KStartOfFirstItemValue, ptr[9]).AllocL();
	
	CItem* cname = CItem::NewL( *(sdesItemPtr->iData), sdesItemPtr->iType );
	iFirstItem = cname;

	//Dont forget about NextL() semantics...
	iItemCount++;
	iCurrentOffset = iOffset + KStartOfFirstItemValue + (sdesItemPtr->iLength);
	}

CRtcpSDESPacketTest::~CRtcpSDESPacketTest()
	{
	}

CItem* CRtcpSDESPacketTest::NextL()
	{
	CItem* t = NULL;
	const TUint8* ptr = iStartOfPkt + iCurrentOffset;	
	//
	//Check if the currently pointed byte is a null byte.
	//If it is a null byte, then this marks the end of the _chunk_
	//Note that the presence of this NULL byte is actually
	//what has been referred in the code as ESdesNone.
	//
	if( ptr[0] == 0 )
		{
		return NULL;
		}

	TRtcpSDESItem* sdesItemPtr = &iPacket.specific.sdes.iItem[iItemCount];
	sdesItemPtr->iType = ptr[0];
	sdesItemPtr->iLength = ptr[1];
	sdesItemPtr->iData = TPtrC8(ptr + KStartOfItemValue, ptr[1]).AllocL();

	t = CItem::NewL( *(sdesItemPtr->iData), sdesItemPtr->iType );

	//"2" because the length-of-sdes item and type-of-sdes-item
	//is stored in one byte each
	iCurrentOffset += 2 + ( sdesItemPtr->iLength );

	//Dont forget the NextL() call semantics...
	iItemCount++;
	return t;
	}

CItem* CRtcpSDESPacketTest::First()
	{
	return iFirstItem;	
	}

void CRtcpSDESPacketTest::SetSSRC(TUint32 aSSRC)
	{
	iPacket.specific.sdes.iSRC = aSSRC;
	}   	

TUint32 CRtcpSDESPacketTest::SSRC()
	{
	return iPacket.specific.sdes.iSRC;
	}

void CRtcpSDESPacketTest::SetData(const TDesC8& aData)
	{	
	*(iPacket.specific.sdes.iItem[0].iData) = aData;
	}

TInt CRtcpSDESPacketTest::SROrRRLength() const
	{
	return static_cast<TUint16>(((iPacket.common.iLength) * 4) + 4);
	}


const TDesC8& CRtcpSDESPacketTest::Data()
	{
	return *(iPacket.specific.sdes.iItem[0].iData);
	}

void CRtcpSDESPacketTest::SetType(TSDESType aType)
	{
	iPacket.specific.sdes.iItem[0].iType = static_cast<TUint8>(aType);
	}

TSDESType CRtcpSDESPacketTest::Type()
	{
	return static_cast<TSDESType>(iPacket.specific.sdes.iItem[0].iType);
	}


RSdesItemManager::RSdesItemManager() : iCurrentItem( 0 ), iSegregatedItemsArrayID( 0 )
	{
	iSdesPkt=NULL;//assigning the pointer 
	}

RSdesItemManager::~RSdesItemManager()
	{
	}


TInt RSdesItemManager::Open()
	{
	//Nothing for now
	return KErrNone;
	}

void RSdesItemManager::Close()
	{
	iArray.Close();
	iSegregatedItemsArray.Close();
	}


TInt RSdesItemManager::CompareSentAndReceivedValues()
	{
	TInt r = KErrNone;

	TInt cnt = iArray.Count() + 1;//+1 cuz CNAME is not added in iArray

	if( cnt != iSegregatedItemsArray.Count() )
		{
		//It means that the number of items that were sent
		//do not match with number of items that were received!!
		return KErrGeneral;
		}

	//NOTE: i=1. Reason: Skip the first entry as its CNAME. 
	for( TInt i=1; i < cnt; i++ )
		{		
		CItem* f = iArray[ i - 1 ];
		CItem* s = iSegregatedItemsArray[i];
		r = CItem::FlatCompare( *f, *s );
		if( !r )
			{
			return KErrGeneral;
			}
		}
	
	return KErrNone;
	}


TInt RSdesItemManager::StoreL( const TDesC8& aItem, TInt aItemType ) //store sent items except CNAME
	{
	CItem* i = CItem::NewL( aItem, aItemType );
	iArray.AppendL( i );
	iCurrentItem++;

	return KErrNone;
	}

TInt RSdesItemManager::SegregateItemsL() //acts on recv-buffer
	{
	//
	//NOTE: Skip the first CNAME item
	//
	CItem* t = iSdesPkt->First();
	if(t)
		{
		iSegregatedItemsArray.AppendL( t );
		iSegregatedItemsArrayID++;
		}

	for(;;)
		{
		t = iSdesPkt->NextL();
		if(!t)
			{
			break;
			}
		iSegregatedItemsArray.AppendL( t );
		iSegregatedItemsArrayID++;
		}

	return KErrNone;
	}




