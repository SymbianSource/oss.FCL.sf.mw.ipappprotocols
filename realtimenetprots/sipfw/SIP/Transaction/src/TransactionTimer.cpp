// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : TransactionTimer.cpp
// Part of       : Transaction
// Version       : SIP/5.0
//



#include "SipAssert.h"
#include "sipstrings.h"
#include "sipresponse.h"
#include "TransactionTimer.h"
#include "ServerTransaction.h"

// -----------------------------------------------------------------------------
// CTransactionTimer::CTransactionTimer
// -----------------------------------------------------------------------------
//
CTransactionTimer::CTransactionTimer(MTimerManager& aTimerMgr,
									 MExpirationHandler* aObserver,
									 TUint32 aDuration) :
	CTimerBase(aTimerMgr, aObserver, aDuration)
	{
    __ASSERT_DEBUG(aObserver,
		User::Panic(_L("CTransactionTimer::CTransactionTimer() aObserver = 0"),
		KErrArgument));
	}

// -----------------------------------------------------------------------------
// CTimerRetransmit::NewL
// -----------------------------------------------------------------------------
//
CTimerRetransmit* CTimerRetransmit::NewL(MTimerManager& aTimerMgr,
										 MExpirationHandler* aObserver,
										 TUint32 aDuration)
	{
	CTimerRetransmit* self = new (ELeave) CTimerRetransmit(aTimerMgr,
														   aObserver,
														   aDuration);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CTimerRetransmit::CTimerRetransmit
// -----------------------------------------------------------------------------
//
CTimerRetransmit::CTimerRetransmit(MTimerManager& aTimerMgr,
								   MExpirationHandler* aObserver,
								   TUint32 aDuration) :
	CTransactionTimer(aTimerMgr, aObserver, aDuration)	
	{
	}

// -----------------------------------------------------------------------------
// CTimerRetransmit::ExpiredL
// Retransmit a SIP message, unless CTransmitter is sending. That happen only if
// InviteServerTransaction receives INVITE, sends response and timer G expires
// before CTransmitter has finished sending.
// -----------------------------------------------------------------------------
//
void CTimerRetransmit::ExpiredL(CTransaction& aTransaction, TTimerId aTimerId)
	{
    __SIP_ASSERT_LEAVE(aTimerId == iId, KErrArgument);

	if (!aTransaction.IsTransmitterSending())
		{
		aTransaction.SendToTransmitterL();
		}
	}

// -----------------------------------------------------------------------------
// CTimerTerminateTa::NewL
// -----------------------------------------------------------------------------
//
CTimerTerminateTa* CTimerTerminateTa::NewL(MTimerManager& aTimerMgr,
										   MExpirationHandler* aObserver,
										   TUint32 aDuration,
										   TInt aReason)
	{
	CTimerTerminateTa* self = new (ELeave) CTimerTerminateTa(aTimerMgr,
															 aObserver,
															 aDuration,
															 aReason);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CTimerTerminateTa::CTimerTerminateTa
// -----------------------------------------------------------------------------
//
CTimerTerminateTa::CTimerTerminateTa(MTimerManager& aTimerMgr,
									 MExpirationHandler* aObserver,
									 TUint32 aDuration,
									 TInt aReason) :
	CTransactionTimer(aTimerMgr, aObserver, aDuration),
	iReason(aReason)
	{
	}

// -----------------------------------------------------------------------------
// CTimerTerminateTa::ExpiredL
// -----------------------------------------------------------------------------
//
void CTimerTerminateTa::ExpiredL(CTransaction& aTransaction, TTimerId aTimerId)
	{
    __SIP_ASSERT_LEAVE(aTimerId == iId, KErrArgument);

	aTransaction.TerminatedL(iReason);
	}

// -----------------------------------------------------------------------------
// CTimerSend100::NewL
// -----------------------------------------------------------------------------
//
CTimerSend100* CTimerSend100::NewL(MTimerManager& aTimerMgr,
								   MExpirationHandler* aObserver,
								   TUint32 aDuration,
								   CSIPResponse* aResponse,
								   RStringF aTransportProtocol,
								   TSIPTransportParams& aTransportParams)
	{
	CTimerSend100* self = new (ELeave) CTimerSend100(aTimerMgr,
													 aObserver,
													 aDuration,													 
								   					 aTransportProtocol,
								   					 aTransportParams);
	CleanupStack::PushL(self);
	self->ConstructL();
	self->iResponse = aResponse;
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CTimerSend100::CTimerSend100
// -----------------------------------------------------------------------------
//
CTimerSend100::CTimerSend100(MTimerManager& aTimerMgr,
						     MExpirationHandler* aObserver,
						     TUint32 aDuration,						     
						     RStringF aTransportProtocol,
							 TSIPTransportParams& aTransportParams) :
	CTransactionTimer(aTimerMgr, aObserver, aDuration),	
	iTransportParams(aTransportParams)
	{
	iTransportProtocol = aTransportProtocol.Copy();
	}

// -----------------------------------------------------------------------------
// CTimerSend100::~CTimerSend100
// -----------------------------------------------------------------------------
//
CTimerSend100::~CTimerSend100()
	{
	delete iResponse;
	iTransportProtocol.Close();
	}

// -----------------------------------------------------------------------------
// CTimerSend100::ExpiredL
// -----------------------------------------------------------------------------
//
void CTimerSend100::ExpiredL(CTransaction& aTransaction, TTimerId aTimerId)
	{
    __SIP_ASSERT_LEAVE(aTimerId == iId, KErrArgument);

	CSIPResponse* resp = iResponse;
	iResponse = NULL;
	CleanupStack::PushL(resp);

	//Causes transaction to delete this timer, so don't refer it afterwards.
	aTransaction.SendResponseL(resp, iTransportProtocol, iTransportParams);
	CleanupStack::Pop(resp);
	}
