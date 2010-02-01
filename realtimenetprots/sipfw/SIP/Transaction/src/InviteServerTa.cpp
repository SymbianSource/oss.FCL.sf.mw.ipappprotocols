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
// Name          : InviteServerTa.cpp
// Part of       : Transaction
// Version       : SIP/5.0
//



#include "SipAssert.h"
#include "siperr.h"
#include "Lwtimer.h"
#include "sipmessage.h"
#include "UserAgentBase.h"
#include "TimerValues.h"

#include "InviteServerTa.h"
#include "TransactionTimer.h"
#include "SIPMessageUtility.h"


// -----------------------------------------------------------------------------
// CInviteServerTransaction::NewL
// -----------------------------------------------------------------------------
//
CInviteServerTransaction*
CInviteServerTransaction::NewL(CUserAgentBase& aUserAgent,
							   CTransmitter& aTransmitter,
						       MTimerManager& aTimers,
							   CTransactionState& aInitialState,
							   TTimerValues& aTimerValues)
	{
	CInviteServerTransaction* self =
		new (ELeave) CInviteServerTransaction(aUserAgent,
										  	  aTransmitter,
										 	  aTimers,
											  aInitialState,
											  aTimerValues);
	CleanupStack::PushL(self);
	self->ConstructServerTaL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CInviteServerTransaction::CInviteServerTransaction
// -----------------------------------------------------------------------------
//
CInviteServerTransaction::CInviteServerTransaction(
				CUserAgentBase& aUserAgent,
				CTransmitter& aTransmitter,
				MTimerManager& aTimers,
				CTransactionState& aInitialState,
				TTimerValues& aTimerValues) :
	CServerTransaction(aUserAgent,
					   aTransmitter,
					   aTimers,
					   aInitialState,
					   aTimerValues)
	{
	}

// -----------------------------------------------------------------------------
// CInviteServerTransaction::~CInviteServerTransaction
// -----------------------------------------------------------------------------
//
CInviteServerTransaction::~CInviteServerTransaction()
	{
    //Scope operator to silence lint warning
    CInviteServerTransaction::CancelAllTimers();
	}

// -----------------------------------------------------------------------------
// CInviteServerTransaction::IsInviteTransaction
// -----------------------------------------------------------------------------
//
TBool CInviteServerTransaction::IsInviteTransaction() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CInviteServerTransaction::ResponseSent
// -----------------------------------------------------------------------------
//
void CInviteServerTransaction::ResponseSent(TUint aRespCode,
											CTransactionState& aSending2xx,
											CTransactionState& aCompleted)
	{
	__TEST_INVARIANT;

	if (aRespCode >= 200)
		{
		if (aRespCode < 300)
			{
			ChangeState(aSending2xx);
			}
		else
			{
			ChangeState(aCompleted);
			}
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteServerTransaction::EnterConfirmedStateL
// -----------------------------------------------------------------------------
//
void
CInviteServerTransaction::EnterConfirmedStateL(CTransactionState& aConfirmed)
	{
	__TEST_INVARIANT;    

	StopTimerG();
	StopTimerH();
	StartTimerIL();
	ChangeState(aConfirmed);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteServerTransaction::StartTimerGUnlessExistsL
// -----------------------------------------------------------------------------
//
void CInviteServerTransaction::StartTimerGUnlessExistsL()
	{
	__TEST_INVARIANT;
    
    if (!iTimerG)
    	{    	
		iTimerValues.DoubleUptoT2(iTimerGDuration);
		iTimerG = CTimerRetransmit::NewL(iTimers, this, iTimerGDuration);
    	}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteServerTransaction::StopTimerG
// -----------------------------------------------------------------------------
//
void CInviteServerTransaction::StopTimerG()
	{
	__TEST_INVARIANT;

	delete iTimerG;
	iTimerG = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteServerTransaction::StartTimerHL
// -----------------------------------------------------------------------------
//
void CInviteServerTransaction::StartTimerHL()
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(!iTimerH, KErrAlreadyExists);
	
	iTimerH = CTimerTerminateTa::NewL(iTimers,
									  this,
									  iTimerValues.Duration64xT1(),									  
									  KErrTimedOut);
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteServerTransaction::StopTimerH
// -----------------------------------------------------------------------------
//
void CInviteServerTransaction::StopTimerH()
	{
	__TEST_INVARIANT;

	delete iTimerH;
	iTimerH = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteServerTransaction::StartTimerIL
// Duration is 0s for reliable and T4 for unreliable transports.
// -----------------------------------------------------------------------------
//
void CInviteServerTransaction::StartTimerIL()
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(!iTimerI, KErrAlreadyExists);

	TUint32 duration = 0;
	if (IsUnreliableTransportUsed())
		{
		duration = iTimerValues.T4();
		}

	iTimerI = CTimerTerminateTa::NewL(iTimers, this, duration);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteServerTransaction::StopTimerI
// -----------------------------------------------------------------------------
//
void CInviteServerTransaction::StopTimerI()
	{
	__TEST_INVARIANT;

	delete iTimerI;
	iTimerI = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteServerTransaction::DeleteTimer
// -----------------------------------------------------------------------------
//
void CInviteServerTransaction::DeleteTimer(const CTransactionTimer& aTimer)
	{
	__TEST_INVARIANT;

	if (&aTimer == iTimerG)
		{
		StopTimerG();
		}

	if (&aTimer == iTimerH)
		{
		StopTimerH();		
		}

	if (&aTimer == iTimerI)
		{
		StopTimerI();		
		}	

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteServerTransaction::CancelAllTimers
// -----------------------------------------------------------------------------
//
void CInviteServerTransaction::CancelAllTimers()
	{
	__TEST_INVARIANT;

	StopTimerG();
	StopTimerH();
	StopTimerI();

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteServerTransaction::TerminatedL
// -----------------------------------------------------------------------------
//
void CInviteServerTransaction::TerminatedL(TInt aReason)
	{
	__TEST_INVARIANT;

	if (!iTerminated)
		{
		//CInviteServerTransaction specific stuff
		Leave2xxForUAS();

		//Common stuff
		CServerTransaction::TerminatedL(aReason);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteServerTransaction::Leave2xxForUAS
// InviteUAS owns 2xx. If iOutgoingMsg is 2xx, clear it so ~CServerTransaction
// won't delete it. 2xx may be in iSendQueue, but the queue never owns 2xx to
// INVITE. 2xx can't be in iSentResponses as only one final response is sent to
// ServerTransaction and the most recent response is in iOutgoingMsg.
// -----------------------------------------------------------------------------
//
void CInviteServerTransaction::Leave2xxForUAS()
	{
	__TEST_INVARIANT;

	if (iOutgoingMsg)
		{
        __SIP_ASSERT_RETURN(!iOutgoingMsg->IsRequest(),
        					KErrSIPMalformedMessage);
		if (CSIPMessageUtility::Is2xxResponse(*iOutgoingMsg))
			{
			iOutgoingMsg = NULL;
			}
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteServerTransaction::Ptr
// -----------------------------------------------------------------------------
//
CInviteServerTransaction&
CInviteServerTransaction::Ptr(CTransactionBase& aTransaction)
	{
	return static_cast<CInviteServerTransaction&>(aTransaction);
	}

// -----------------------------------------------------------------------------
// CInviteServerTransaction::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CInviteServerTransaction::__DbgTestInvariant() const
	{
	if (iTimerGDuration > iTimerValues.T2())
		{	
		User::Invariant();
		}		
	}
