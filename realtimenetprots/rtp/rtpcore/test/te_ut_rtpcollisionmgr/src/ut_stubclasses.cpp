// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// @internalAll
// 
//

#include "ut_stubclasses.h"
#include <random.h>
#include <in_sock.h>

//#include <test/testexecutelogger.h>
//

CRtpHandlerBase* CRtpHandlerBase::NewL()
{
	CRtpHandlerBase *self = new(ELeave) CRtpHandlerBase();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CRtpHandlerBase::CRtpHandlerBase()
{
}


CRtpHandlerBase::~CRtpHandlerBase()
{
}
 
void CRtpHandlerBase::ConstructL()
{
}
 
TBool CRtpHandlerBase::CompareAddresses(const TSockAddr& aAddr1,const TSockAddr& aAddr2)
{
	if(aAddr1.Family() != aAddr2.Family())
		{
		return EFalse;
		}
		
	TInetAddr inetAddr1(aAddr1);
	TInetAddr inetAddr2(aAddr2);
	//Tests the protocol family, IP address, and port values
	TBool flag = inetAddr1.CmpAddr(inetAddr2);
	return flag;
}
 
//
/* Implementation of CRtpController Stub      */
//
CRtpController* CRtpController::NewL()
{
	CRtpController *self = new(ELeave) CRtpController();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CRtpController::CRtpController()
{
}


CRtpController::~CRtpController()
{
	delete iRtp;
	delete iRtcp;
}

void CRtpController::SetLocalSSRC(TUint32 aSSRC)
{
	iSSRC = aSSRC;
}

void CRtpController::ConstructL()
{
	iRtp = CRtpHandlerBase::NewL();
	iRtcp = CRtpHandlerBase::NewL();
	return;
}

void PanicL(TRtpCorePanicCode aPanicCode)
   {
	/* To Catch the panics from Rtp Collision manager we Trap them here */
	//User::Panic(_L("TestCodePanc"), aPanicCode);
   // INFO_PRINTF1(_L("TEST CODE PANIC!!!"));
   User::Leave(aPanicCode);
   }
	
void CRtpController::HandleCollision()
{
	/* Nothing to do here */
	return;
}

void CRtpController::SendBYEPacketL(TUint32 , const TDesC8& )
	{
	return;
	}
	
CRtpHandlerBase* CRtpController::RtpHandler()
{
	return iRtp;
}

CRtpHandlerBase* CRtpController::RtcpHandler()
{
	return iRtcp;
}


//
// RTPUtils Helper Class                          //
// //
TNTPTime RtpUtils::GetNTPTimeL()
	{
	return TNTPTime(0,0XDEAD);
	}

TUint32 RtpUtils::GenerateRandomNumber32L()
	{
	TBuf8<4> random(4);
	TRandom::Random(random);
	TUint32 number = *(reinterpret_cast<const TUint32*>(random.Ptr()));
	return number;
	}
//


//
//* Implementation of CRtpSourceEntry Stub.      */
//
CRtpSourceEntry* CRtpSourceEntry::NewLC(TUint32 aSSRC, const TDesC8& aCNAME,
								   CRtpController* aController)
	{	

	CRtpSourceEntry* self = new(ELeave) CRtpSourceEntry(aSSRC);
	CleanupStack::PushL(self);
	self->iController = aController;
	self->ConstructL(aCNAME);
	return self;
	}

CRtpSourceEntry::~CRtpSourceEntry()
	{
	delete iCNAME;
	}

void CRtpSourceEntry::ConstructL(const TDesC8& aCNAME)
	{	
	if(0 != aCNAME.Compare(KNullDesC8))
		{
		iCNAME = aCNAME.AllocL();
		}
	}

CRtpSourceEntry::CRtpSourceEntry(TUint32 aSRC) : iSRC(aSRC)
	{
	}

TInt CRtpSourceEntry::CompareSSRC(const CRtpSourceEntry& aFirst,const CRtpSourceEntry& aSecond)
	{	
	TInt result =0;
	if (aFirst.iSRC < aSecond.iSRC)
		result = -1;
	else if (aFirst.iSRC > aSecond.iSRC)
		result = 1;
	
	return result;
	}

void CRtpSourceEntry::SetRtpSourceAddr(TSockAddr& aRtpAddr)
	{
	iRtpSourceAddr = aRtpAddr;
	}
void CRtpSourceEntry::SetRtcpSourceAddr(TSockAddr& aRtcpAddr)
	{
	iRtcpSourceAddr = aRtcpAddr;
	}

TUint CRtpSourceEntry::SRC() const
	{
	return iSRC;
	}

void  CRtpSourceEntry::SetState(TRtpSourceState aState)
{
	iState = aState;
	return;
}

void CRtpSourceEntry::SetCnameL(const TDesC8& aCNAME)
	{	
	if(iCNAME)
		{
		User::Leave(KErrAlreadyExists);
		}
	else
		{
		iCNAME = aCNAME.AllocL();
		}
	}
	
const TDesC8& CRtpSourceEntry::Cname()
	{
	if(iCNAME)
		return *iCNAME;
	else
		return KNullDesC8;
	}
	
TRtpSourceState CRtpSourceEntry::State()
	{
	return iState;
	}
	
void CRtpSourceEntry::SetSRC(TUint32 aSRC)
	{	
	iSRC = aSRC;
	}


const TSockAddr& CRtpSourceEntry::RtpSourceAddr() const
{
	return iRtpSourceAddr;
}
	
	
const TSockAddr& CRtpSourceEntry::RtcpSourceAddr() const
{
	return iRtcpSourceAddr;
}

