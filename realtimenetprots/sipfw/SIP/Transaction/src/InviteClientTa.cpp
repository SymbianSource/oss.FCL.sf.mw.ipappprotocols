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
// Name          : InviteClientTa.cpp
// Part of       : Transaction
// Version       : SIP/5.0
//



#include "SipAssert.h"
#include "siperr.h"
#include "Lwtimer.h"
#include "uricontainer.h"
#include "sipcseqheader.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "TimerValues.h"
#include "SIPMessageUtility.h"
#include "SIPRequestUtility.h"

#include "InviteClientTa.h"
#include "Transmitter.h"
#include "TransactionState.h"
#include "TransactionTimer.h"
#include "RestoreTransactionState.h"


// -----------------------------------------------------------------------------
// CInviteClientTransaction::NewL
// -----------------------------------------------------------------------------
//
CInviteClientTransaction* CInviteClientTransaction::NewL(
							CUserAgentBase& aUserAgent,
							CTransmitter& aTransmitter,
							MTimerManager& aTimers,
							CTransactionState& aInitialState,
							TTimerValues& aTimerValues,
							MTransactionStore& aTransactionStore,
							TBool aRetransmitInvite)
	{
	CInviteClientTransaction* self =
        new (ELeave) CInviteClientTransaction(aUserAgent,							
									          aTransmitter,
									          aTimers,
									          aInitialState,
									          aTimerValues,
									          aTransactionStore,
									          aRetransmitInvite);
	return self;
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::CInviteClientTransaction
// -----------------------------------------------------------------------------
//
CInviteClientTransaction::CInviteClientTransaction(
						CUserAgentBase& aUserAgent,
						CTransmitter& aTransmitter,
						MTimerManager& aTimers,
						CTransactionState& aInitialState,
						TTimerValues& aTimerValues,
						MTransactionStore& aTransactionStore,
						TBool aRetransmitInvite) :
	CClientTransaction(aUserAgent,
					   aTransmitter,
					   aTimers,
					   aInitialState,
					   aTimerValues,
					   aTransactionStore),
	iRetransmitInvite(aRetransmitInvite),iDTimerErrCode(KErrNone)
	{
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::~CInviteClientTransaction
// -----------------------------------------------------------------------------
//
CInviteClientTransaction::~CInviteClientTransaction()
	{
    //Scope operator to silence lint warning
    CInviteClientTransaction::CancelAllTimers();

	if (iTaOwnsRequest)
		{
		delete iOutgoingMsg;
		}

	//CInviteClientTransaction can own iTransmitter
	if (iDeleteMgr)
		{
		delete iTransmitter;
		}
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::IsInviteTransaction
// -----------------------------------------------------------------------------
//
TBool CInviteClientTransaction::IsInviteTransaction() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::ReceiveResponseL
// -----------------------------------------------------------------------------
//
void CInviteClientTransaction::ReceiveResponseL(CSIPResponse* aResponse,
									const CTransactionState& aCurrent,
                                    const CTransactionState& aCompleted,
                                    CTransactionTimer* aTimer,
                          			CTransactionTimer* aTimer2)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aResponse, KErrArgument);

	CSIPRequest* ack = NULL;
	TBool isErrResponse = aResponse->IsErrorResponse();
	if ( isErrResponse )
	    {
	    ack = MakeAckLC(*aResponse);
        ChangeState(aCompleted);
	    }

	TRestoreTransactionState restoreState(*this, aCurrent, aTimer, aTimer2);
    CleanupStack::PushL(restoreState.CleanupItem());
    
	TRAPD(err,PassResponseToUserAgentL(aResponse));
	if( err != KErrNone )
		{
		if( err == KErrNoMemory || !isErrResponse )
			{
			User::Leave(err);
			}
		else
			{
			iDTimerErrCode = err;
			}
		}
		
    CleanupStack::Pop(); //cleanupItem  

    if (ack)
    	{
    	CleanupStack::Pop(ack);
		SendAck(ack);
    	}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::SendAck
// Can't leave as response's ownership has been passed. Cancel CTransmitter if
// needed so ACK can be sent. Transaction doesn't own INVITE, so ACK is put to
// iOutgoingMsg, in place of INVITE.
// -----------------------------------------------------------------------------
//
void CInviteClientTransaction::SendAck(CSIPRequest* aAck)
	{
	__TEST_INVARIANT;

	if (IsTransmitterSending())
		{
		iTransmitter->Cancel();
		}

	iOutgoingMsg = aAck;
	iTaOwnsRequest = ETrue;

	TRAPD(err, SendToTransmitterL());
	if (err != KErrNone)
		{
		TRAP_IGNORE(TerminatedL(err))
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::MakeAckLC
// Create ACK just once. Copy To-header from the response.
// -----------------------------------------------------------------------------
//
CSIPRequest* CInviteClientTransaction::MakeAckLC(CSIPResponse& aResp) const
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(!iTaOwnsRequest, KErrAlreadyExists);	

	CSIPRequest* ack =
		CSIPRequest::NewLC(SIPStrings::StringF(SipStrConsts::EAck));
	CopyHeadersFromInviteToAckL(*ack);

	RStringF to = SIPStrings::StringF(SipStrConsts::EToHeader);
	__SIP_ASSERT_LEAVE(aResp.HasHeader(to), KErrArgument);
	CSIPMessageUtility::CopyHeadersL(aResp, *ack, to);

	SIPRequestUtility::FillNewMaxForwardsL(*ack);
	CSIPMessageUtility::CopyAuthorizationHeadersL(iOutgoingMsg, *ack);
	SIPRequestUtility::FillSupportedSecAgreeL(*ack);

    __TEST_INVARIANT;
    return ack;
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::CopyHeadersFromInviteToAckL
// Copy INVITE's Via header to ACK. Both have the same CSeq sequence number.
// -----------------------------------------------------------------------------
//
void
CInviteClientTransaction::CopyHeadersFromInviteToAckL(CSIPRequest& aAck) const
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(iOutgoingMsg, KErrNotFound);

    RStringF from   = SIPStrings::StringF(SipStrConsts::EFromHeader);
    RStringF callId = SIPStrings::StringF(SipStrConsts::ECallIDHeader);
    RStringF via    = SIPStrings::StringF(SipStrConsts::EViaHeader);
    RStringF cseq   = SIPStrings::StringF(SipStrConsts::ECSeqHeader);

    __SIP_ASSERT_LEAVE(iOutgoingMsg->HasHeader(from) &&
    				   iOutgoingMsg->HasHeader(callId) &&
    				   iOutgoingMsg->HasHeader(via) &&
    				   iOutgoingMsg->HasHeader(cseq), KErrSIPMalformedMessage);

	const CURIContainer* origReqUri =
        static_cast<CSIPRequest*>(iOutgoingMsg)->RequestURI();
    __SIP_ASSERT_LEAVE(origReqUri != NULL, KErrSIPMalformedMessage);

	CURIContainer* reqUri = CURIContainer::NewLC(*origReqUri);
	aAck.SetRequestURIL(reqUri);
	CleanupStack::Pop(reqUri);

	CSIPMessageUtility::CopyHeadersL(*iOutgoingMsg, aAck, from);
	CSIPMessageUtility::CopyHeadersL(*iOutgoingMsg, aAck, callId);
	CSIPMessageUtility::CopyHeadersL(*iOutgoingMsg, aAck, via);

	TSglQueIter<CSIPHeaderBase> iter = iOutgoingMsg->Headers(cseq);
	CSIPMessageUtility::FillCSeqL(aAck,
								  static_cast<CSIPCSeqHeader&>(*iter).Seq(),
								  SIPStrings::StringF(SipStrConsts::EAck));
	CSIPMessageUtility::CopyHeadersL(*iOutgoingMsg,
							 aAck,
							 SIPStrings::StringF(SipStrConsts::ERouteHeader));
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::StartTimerAL
// -----------------------------------------------------------------------------
//
void CInviteClientTransaction::StartTimerAL()
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(!iTimerA, KErrAlreadyExists);

	if (iTimerADuration == 0)
		{
		iTimerADuration = iTimerValues.T1();
		}
	else
		{
		iTimerADuration = iTimerADuration << 1;
		}

	iTimerA = CTimerRetransmit::NewL(iTimers, this, iTimerADuration);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::StartTimerBUnlessExistsL
// -----------------------------------------------------------------------------
//
void CInviteClientTransaction::StartTimerBUnlessExistsL()
	{
	__TEST_INVARIANT;

	if (!iTimerB)
		{
		iTimerB = CTimerTerminateTa::NewL(iTimers,
										  this,
										  iTimerValues.Duration64xT1(),
										  KErrTimedOut);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::StartTimerDUnlessExistsL
// Duration is 0s for reliable and 65*T1 for unreliable transports.
// -----------------------------------------------------------------------------
//
void CInviteClientTransaction::StartTimerDUnlessExistsL()
	{
	__TEST_INVARIANT;

    if (!iTimerD)
    	{
		TUint32 duration = 0;
		if (IsUnreliableTransportUsed())
			{
			duration = iTimerValues.Duration64xT1() + iTimerValues.T1();
			}
	    iTimerD = CTimerTerminateTa::NewL(iTimers, this, duration,iDTimerErrCode);
    	}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::StartTimerProceedingL
// -----------------------------------------------------------------------------
//
CTransactionTimer* CInviteClientTransaction::StartTimerProceedingL()
	{		
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(!iTimerProceeding, KErrAlreadyExists);

	iTimerProceeding =
        CTimerTerminateTa::NewL(iTimers,
								this,
								CTimerTerminateTa::KProceedingTimerDuration,
								KErrTimedOut);
	__TEST_INVARIANT;
	return iTimerProceeding;
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::StartTimerNATL
// If 1xx comes before SendCompleteL, timer exists when handling SendCompleteL.
// -----------------------------------------------------------------------------
//
CTransactionTimer*  CInviteClientTransaction::StartTimerNATL()
	{
	__TEST_INVARIANT;

	if (!iTimerNAT && iRetransmitInvite && IsUnreliableTransportUsed())
		{
		iTimerNAT = CTimerRetransmit::NewL(iTimers,
									   this,
									   CTimerRetransmit::KNATBindingInterval);
		}

	__TEST_INVARIANT;
	return iTimerNAT;
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::StopTimerA
// -----------------------------------------------------------------------------
//
void CInviteClientTransaction::StopTimerA()
	{
	__TEST_INVARIANT;

	delete iTimerA;
	iTimerA = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::StopTimerB
// -----------------------------------------------------------------------------
//
void CInviteClientTransaction::StopTimerB()
	{
	__TEST_INVARIANT;

	delete iTimerB;
	iTimerB = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::StopTimerD
// -----------------------------------------------------------------------------
//
void CInviteClientTransaction::StopTimerD()
	{
	__TEST_INVARIANT;

	delete iTimerD;
	iTimerD = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::StopTimerProceeding
// -----------------------------------------------------------------------------
//
void CInviteClientTransaction::StopTimerProceeding()
	{
	__TEST_INVARIANT;

	delete iTimerProceeding;
	iTimerProceeding = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::StopTimerNAT
// -----------------------------------------------------------------------------
//
void CInviteClientTransaction::StopTimerNAT()
	{
	__TEST_INVARIANT;

	delete iTimerNAT;
	iTimerNAT = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::DeleteTimer
// -----------------------------------------------------------------------------
//
void CInviteClientTransaction::DeleteTimer(const CTransactionTimer& aTimer)
	{
	__TEST_INVARIANT;    

	if (&aTimer == iTimerA)
		{
		StopTimerA();
		}

	if (&aTimer == iTimerB)
		{
		StopTimerB();		
		}

	if (&aTimer == iTimerD)
		{
		StopTimerD();		
		}

	if (&aTimer == iTimerProceeding)
		{
		StopTimerProceeding();
		}

	if (&aTimer == iTimerNAT)
		{		
		StopTimerNAT();
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::CancelAllTimers
// -----------------------------------------------------------------------------
//
void CInviteClientTransaction::CancelAllTimers()
	{
	__TEST_INVARIANT;

	StopTimerA();
	StopTimerB();
	StopTimerD();
	StopTimerProceeding();
	StopTimerNAT();

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::Ptr
// -----------------------------------------------------------------------------
//
CInviteClientTransaction&
CInviteClientTransaction::Ptr(CTransaction& aTransaction)
	{
	return static_cast<CInviteClientTransaction&>(aTransaction);
	}

// -----------------------------------------------------------------------------
// CInviteClientTransaction::__DbgTestInvariant
// iTaOwnsRequest can be ETrue only if iOutgoingMsg is ACK.
// -----------------------------------------------------------------------------
//

void CInviteClientTransaction::__DbgTestInvariant() const
	{
	if (iOutgoingMsg &&
		iTaOwnsRequest &&
		!CSIPMessageUtility::IsAck(*iOutgoingMsg) &&
		(iRetransmitInvite && !iTimerNAT ||
		 !iRetransmitInvite && iTimerNAT))
		{
		User::Invariant();
		}
	}
