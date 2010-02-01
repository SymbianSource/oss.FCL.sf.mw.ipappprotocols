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
// Name          : InviteUAS.cpp
// Part of       : TransactionUser
// Version       : SIP/6.0
//



#include "siperr.h"
#include "sipinternalerr.h"
#include "SipAssert.h"
#include "sipresponse.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "MSipDialogs.h"
#include "TransactionMgr.h"
#include "Transmitter.h"

#include "MTransactionOwner.h"
#include "InviteUAS.h"
#include "InviteServerTa.h"     // Leave2xxForUAS
#include "SIPMessageUtility.h"
#include "CTransactionStore.h"
#include "UserAgentState.h"
#include "UserAgentTimer.h"
#include "TimerValues.h"


// -----------------------------------------------------------------------------
// CInviteUAS::NewL
// -----------------------------------------------------------------------------
//
CInviteUAS* CInviteUAS::NewL(CUserAgentCreateParams& aParams,							
							 MSipConnectionMgr& aConnectionMgr,
							 MSIPRequestRouter& aRouter,
                             MSipDialogs& aDialogs)
	{
	CInviteUAS* self = new (ELeave) CInviteUAS(aParams, aDialogs, aRouter);
	CleanupStack::PushL(self);
	self->ConstructL(aConnectionMgr);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CInviteUAS::CInviteUAS
// -----------------------------------------------------------------------------
//
CInviteUAS::CInviteUAS(CUserAgentCreateParams& aParams,					   
                       MSipDialogs& aDialogs,
                       MSIPRequestRouter& aRouter) :
	CUserAgentServer(aParams, aDialogs, aRouter)
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAS::ConstructL
// -----------------------------------------------------------------------------
//
void CInviteUAS::ConstructL(MSipConnectionMgr& aConnectionMgr)
	{
	CUserAgent::ConstructL(aConnectionMgr);
	CreateTransactionL();
	}

// -----------------------------------------------------------------------------
// CInviteUAS::~CInviteUAS
// Transaction must clear pointers to the 2xx, as UAS will delete the 2xx.
// -----------------------------------------------------------------------------
//
CInviteUAS::~CInviteUAS()
	{
	CancelAllTimers();

    if (IsSending2xx() && iTransaction && !iTransaction->HasTerminated())
        {
		CInviteServerTransaction::Ptr(*iTransaction).Leave2xxForUAS();	
		}
	}

// -----------------------------------------------------------------------------
// CInviteUAS::CreateTransactionL
// -----------------------------------------------------------------------------
//
void CInviteUAS::CreateTransactionL()
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(!iTransaction && iTransmitter, KErrAlreadyExists);

	iTransaction = iTransactionMgr.CreateTransactionL(
									CTransactionBase::KServerInviteTransaction,
									*this,
									*iTransmitter,
									iTimerValues);
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAS::DoesDialogExistForAckL
// -----------------------------------------------------------------------------
//
TBool CInviteUAS::DoesDialogExistForAckL(CSIPRequest& aAck)
	{
	__TEST_INVARIANT;

	CSIPResponse* resp(NULL);
    MTransactionOwner* taOwner = iDialogs.TransactionOwnerL(aAck, &resp);
	if (taOwner)
		{
        iObserver = taOwner;
		return ETrue;
		}

    // Nothing is sent after ACK
	delete resp;

    __TEST_INVARIANT;
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CInviteUAS::SendFirst2xxL
// Clear transaction from CTransactionStore, so further INVITEs/ACKs are passed
// to UAS, and not to transaction. INVITE UAS owns the 2xx.
// -----------------------------------------------------------------------------
//
void CInviteUAS::SendFirst2xxL(CSIPResponse* aResp)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aResp, KErrArgument);
	__SIP_ASSERT_LEAVE(aResp->Type() == CSIPResponse::E2XX, KErrArgument);

	iTransactionStore.ClearTransaction(TransactionId());
	StartTimerWaitAckL();
	SendResponseToTransactionL(aResp);
	iOutgoingMsg = aResp;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAS::Retransmit2xxL
// -----------------------------------------------------------------------------
//
void CInviteUAS::Retransmit2xxL()
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(iOutgoingMsg && iTransmitter, KErrGeneral);
    __SIP_ASSERT_LEAVE(
        static_cast<CSIPResponse*>(iOutgoingMsg)->Type() == CSIPResponse::E2XX,
        KErrSIPMalformedMessage);

	iTransmitter->SendResponseL(static_cast<CSIPResponse&>(*iOutgoingMsg),
								TransportParams(),
							    this,
								TransactionId());
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAS::CancelInviteL
// -----------------------------------------------------------------------------
//
void CInviteUAS::CancelInviteL()
	{
	__TEST_INVARIANT;

	if (!iCanceled)
		{
		iCanceled = State().CancelL(*this);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAS::HandleCancelInviteL
// -----------------------------------------------------------------------------
//
void CInviteUAS::HandleCancelInviteL(const CUserAgentState& aRejected)
	{
	__TEST_INVARIANT;

	if (iTransaction)
		{
		SendErrorResponseL(487,
	    	SIPStrings::StringF(SipStrConsts::EPhraseRequestTerminated),
	    	aRejected);
		}

	if (!InformObserverAboutCancel())
		{
		// Waiting to get MTransactionOwner. End silently.
		CancelGetOwnerRequest();
        StopTimerOwnerResolver();
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAS::InformObserverAboutCancel
// -----------------------------------------------------------------------------
//
TBool CInviteUAS::InformObserverAboutCancel()
	{
	if (iObserver)
		{
		// INVITE was passed to iObserver, run transaction silently to end
		iObserver->TransactionEnded(iTransportParams.IapId(),
		                            TransactionId(),
									KInviteTransactionCancelled);
		iObserver = NULL;
		return ETrue;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CInviteUAS::IsSending2xx
// -----------------------------------------------------------------------------
//
TBool CInviteUAS::IsSending2xx() const
    {
    __TEST_INVARIANT;
    return iOutgoingMsg && CSIPMessageUtility::Is2xxResponse(*iOutgoingMsg);
    }

// -----------------------------------------------------------------------------
// CInviteUAS::StartTimerRetransmit2xxL
// -----------------------------------------------------------------------------
//
void CInviteUAS::StartTimerRetransmit2xxL()
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(!iTimerRetransmit2xx, KErrAlreadyExists);

	iTimerValues.DoubleUptoT2(iTimerRetransmit2xxDuration);
	iTimerRetransmit2xx =
        CTimerRetransmit2xx::NewL(iTimers,
								  this,				
								  iTimerRetransmit2xxDuration);
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAS::StopTimerRetransmit2xx
// -----------------------------------------------------------------------------
//
void CInviteUAS::StopTimerRetransmit2xx()
	{
	__TEST_INVARIANT;

	delete iTimerRetransmit2xx;
	iTimerRetransmit2xx = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAS::StartTimerWaitAckL
// -----------------------------------------------------------------------------
//
void CInviteUAS::StartTimerWaitAckL()
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(!iTimerWaitAck, KErrAlreadyExists);
	
	iTimerWaitAck = CTimerStopUA::NewL(iTimers,
									   this,
									   iTimerValues.Duration64xT1(),
                                       KErrSIPNoAckReceived);
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAS::StopTimerWaitAck
// -----------------------------------------------------------------------------
//
void CInviteUAS::StopTimerWaitAck()
	{
	__TEST_INVARIANT;

	delete iTimerWaitAck;
	iTimerWaitAck = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAS::DeleteTimer
// -----------------------------------------------------------------------------
//
void CInviteUAS::DeleteTimer(const CUserAgentTimer& aTimer)
	{
	__TEST_INVARIANT;

	if (&aTimer == iTimerRetransmit2xx)
		{
		StopTimerRetransmit2xx();		
		}

	if (&aTimer == iTimerWaitAck)
		{
		StopTimerWaitAck();		
		}

    if (&aTimer == iTimerOwnerResolver)
        {
        StopTimerOwnerResolver();
        }

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAS::CancelAllTimers
// -----------------------------------------------------------------------------
//
void CInviteUAS::CancelAllTimers()
	{
	__TEST_INVARIANT;

	StopTimerRetransmit2xx();
	StopTimerWaitAck();
    StopTimerOwnerResolver();

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAS::Ptr
// -----------------------------------------------------------------------------
//
CInviteUAS& CInviteUAS::Ptr(CUserAgent& aUserAgent)
	{
	return static_cast<CInviteUAS&>(aUserAgent);
	}

// -----------------------------------------------------------------------------
// CInviteUAS::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CInviteUAS::__DbgTestInvariant() const
	{
	if (iTimerRetransmit2xxDuration > iTimerValues.T2())
		{	
		User::Invariant();
		}
	}

