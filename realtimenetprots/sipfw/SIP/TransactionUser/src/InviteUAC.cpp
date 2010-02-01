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
// Name          : InviteUAC.cpp
// Part of       : TransactionUser
// Version       : SIP/6.0
//



#include "siperr.h"
#include "sipfromheader.h"
#include "sipviaheader.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "MSipRegistrations.h"
#include "TransactionMgr.h"
#include "ClientTransaction.h"
#include "Transmitter.h"
#include "DeleteMgr.h"
#include "SipAssert.h"

#include "InviteUAC.h"
#include "InviteUACStates.h"
#include "CTransactionStore.h"
#include "SIPMessageUtility.h"
#include "SIPRequestUtility.h"
#include "TimerValues.h"
#include "UserAgentTimer.h"
#include "UserAgentCreateParams.h"
#include "RouteSet.h"
#include "ResolvingResults.h"


// -----------------------------------------------------------------------------
// CInviteUAC::NewL
// -----------------------------------------------------------------------------
//
CInviteUAC* CInviteUAC::NewL(CUserAgentCreateParams& aParams,							 
                             MSipConnectionMgr& aConnectionMgr,
                             MSipUriResolver& aResolver,							 
							 CSIPSec& aSIPSec,
							 TUint32 aCSeqNumber,
							 TBool aIsPrivateAddress,
							 CUserAgentState* aAckSenderState)
	{
	CInviteUAC* self = new (ELeave) CInviteUAC(aParams,
											   aConnectionMgr,
                                               aResolver,											   
											   aSIPSec,
											   aCSeqNumber,
											   aIsPrivateAddress,
											   aAckSenderState);
	CleanupStack::PushL(self);
	self->CUserAgent::ConstructL(aConnectionMgr);
	self->CUserAgentClient::ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::CInviteUAC
// -----------------------------------------------------------------------------
//
CInviteUAC::CInviteUAC(CUserAgentCreateParams& aParams,					   
                       MSipConnectionMgr& aConnectionMgr,
                       MSipUriResolver& aResolver,					   
					   CSIPSec& aSIPSec,
					   TUint32 aCSeqNumber,
					   TBool aIsPrivateAddress,
					   CUserAgentState* aAckSenderState) :
	CUserAgentClient(aParams, aResolver, aSIPSec, aCSeqNumber),
	iConnectionMgr(aConnectionMgr),	
	iIsPrivateAddress(aIsPrivateAddress),
	iAckSenderState(aAckSenderState)
#ifdef CPPUNIT_TEST
    , iAckSenders(1)
#endif
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAC::~CInviteUAC
// -----------------------------------------------------------------------------
//
CInviteUAC::~CInviteUAC()
	{
	CancelAllTimers();
	iAckSenders.ResetAndDestroy();
	delete iDeleteThisTransaction;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::ReceiveResponseL
// Application sends ACK (uses ITC) to 2xx, so stop transaction and enter next
// state after PassRespToTransactionOwnerL, as ACK isn't sent inside it.
// -----------------------------------------------------------------------------
//
void CInviteUAC::ReceiveResponseL(CSIPResponse* aResp,
						  		  const CUserAgentState& aResolve,
						  		  const CUserAgentState& aWaitResponse,
								  const CUserAgentState& aWaitAckFromApp,
								  const CUserAgentState& aWaitTransactionToEnd)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aResp, KErrArgument);
	__SIP_ASSERT_LEAVE(!iTimerWait2xxRetransmissions, KErrAlreadyExists);

	if (!ShouldUaTryAgainL(aResp, aResolve, aWaitResponse))
		{
		CSIPResponse::TType type = aResp->Type();
		if (type == CSIPResponse::E2XX)
			{
			// UAC treats INVITE transaction completed 64*T1 after the first
			// 2xx. Expire with KErrNone as UAC ends successfully.
			iTimerWait2xxRetransmissions = CTimerStopUA::NewL(iTimers,
											   this,
											   iTimerValues.Duration64xT1(),
											   KErrNone);
			}
		PassRespToTransactionOwnerL(aResp,
									// If timer, set UA and timer pointers
									iTimerWait2xxRetransmissions ? this : NULL,
									iTimerWait2xxRetransmissions);
		if (type == CSIPResponse::E2XX)
			{
			StopTransaction();
			ChangeState(aWaitAckFromApp);
			}
	    else if (type == CSIPResponse::E1XX)
			{
			// Pass 1xx to upper layer
			}
	    else
	        {
	        // Transaction sends ACK, UAC waits it to end
			ChangeState(aWaitTransactionToEnd);
	        }
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::PrepareTxForNewRequestL
// -----------------------------------------------------------------------------
//
void CInviteUAC::PrepareTxForNewRequestL(TTransactionId& aNewTransactionId)

	{
	__TEST_INVARIANT;

	CTransmitter* transmitter = CTransmitter::NewL(iConnectionMgr);
    static_cast<CClientTransaction*>(iTransaction)->
        RunIndependently(aNewTransactionId, &iDeleteMgr, EFalse);
    iTransmitter = transmitter;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::CreateTransactionL
// -----------------------------------------------------------------------------
//
void CInviteUAC::CreateTransactionL()
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(!iTransaction, KErrAlreadyExists);

	iTransaction = iTransactionMgr.CreateTransactionL(
		CTransactionBase::KClientInviteTransaction,
		*this,
		*iTransmitter,
		iTimerValues,
		iIsPrivateAddress && !iRegistrations.IsOutboundProxy(NextHopL()));
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::ClearTransactionOwner
// -----------------------------------------------------------------------------
//
void CInviteUAC::ClearTransactionOwner()
	{
	__TEST_INVARIANT;	
	
	for (TInt i = 0; i < iAckSenders.Count(); i++)
	    {
	    iAckSenders[i]->ClearTransactionOwner();
	    }
    CUserAgent::ClearTransactionOwner();

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::DeleteTimer
// -----------------------------------------------------------------------------
//
void CInviteUAC::DeleteTimer(const CUserAgentTimer& aTimer)
	{
	__TEST_INVARIANT;

	if (&aTimer == iTimerWait2xxRetransmissions)
		{
		StopTimerUaTimeout();
		}
	if (&aTimer == iSIPSecTimer)
		{
		StopTimerSIPSec();
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::IsInviteUAC
// -----------------------------------------------------------------------------
//
TBool CInviteUAC::IsInviteUAC() const
	{
	__TEST_INVARIANT;
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::IsSubInviteUAC
// -----------------------------------------------------------------------------
//
TBool CInviteUAC::IsSubInviteUAC() const
	{
	__TEST_INVARIANT;
	return iAckSenderState == NULL;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::IsCanceled
// -----------------------------------------------------------------------------
//
TBool CInviteUAC::IsCanceled() const
	{
	__TEST_INVARIANT;
	return iCanceled;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::StartSIPSecTimerL
// -----------------------------------------------------------------------------
//
void CInviteUAC::StartSIPSecTimerL()
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(!iSIPSecTimer, KErrAlreadyExists);

	iSIPSecTimer = CTimerAsyncSIPSec::NewL(iTimers, this);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::SendAckL
// -----------------------------------------------------------------------------
//
void CInviteUAC::SendAckL(CSIPRequest* aAck,
						  MTransactionOwner& aObserver,
						  const TSIPTransportParams& aParams,
						  TRegistrationId aRegisterId,
						  const CURIContainer& aRemoteTarget,
						  TBool aDeleteRequest,
						  RStringF aBranch)
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aAck, KErrArgument);

	State().SendAckL(*this,
					 aAck,
					 aObserver,
					 aParams,
					 aRegisterId,
					 aRemoteTarget,
					 aDeleteRequest,
					 aBranch);	
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::CancelInvite
// -----------------------------------------------------------------------------
//
void CInviteUAC::CancelInvite()
	{
	__TEST_INVARIANT;

	iCanceled = ETrue;
	
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::CopyRouteHeadersToCancelL
// -----------------------------------------------------------------------------
//
void CInviteUAC::CopyRouteHeadersToCancelL(CSIPRequest& aCancel)
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(iOutgoingMsg, KErrNotFound);

	CSIPMessageUtility::CopyHeadersL(*iOutgoingMsg,
						 aCancel,
						 SIPStrings::StringF(SipStrConsts::ERouteHeader));
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::HandleIcmpErrorL
// Cancel send. If 2 UAs use same address, one gets ICMP error KErrSIPICMPFailure and its send ends
// but the other can be active. Unless UA stops, pass ICMP error to "sub UACs".
// -----------------------------------------------------------------------------
//
void CInviteUAC::HandleIcmpErrorL(const TInetAddr& aAddress,
								  const CUserAgentState& aResolveAckAddress,
								  const CUserAgentState& aWaitResponse)
	{
	__TEST_INVARIANT;

	if (CSIPMessageUtility::CompareTInetAddr(iRemoteAddr,
											 iTransportProtocol,
											 aAddress))
		{
		iTransmitter->Cancel();
		TInt error = KErrSIPICMPFailure;
		if (!TryNextAddressL(error, aResolveAckAddress, aWaitResponse))
			{
            Stop(error);
            return;
			}
		}

	for (TInt i = 0; i < iAckSenders.Count(); ++i)
		{
        // Use any error code. "sub UAC" stops with CSIP::ETransportFailure.
		iAckSenders[i]->IcmpErrorL(aAddress,
								   CSipConnectionMgr::EHostUnreachable);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::CheckAckL
// Application must fill To-header, as many 2xx (each with a unique To-tag) can
// arrive and UAC won't know which tag to put in To-header.
// -----------------------------------------------------------------------------
//
void CInviteUAC::CheckAckL(CSIPRequest& aAck) const
	{
	__TEST_INVARIANT;

    if (!aAck.HasHeader(SIPStrings::StringF(SipStrConsts::EToHeader)) ||
        aAck.HasHeader(SIPStrings::StringF(SipStrConsts::EViaHeader)))
        {
        User::Leave(KErrSIPMalformedMessage);
        }
	}

// -----------------------------------------------------------------------------
// CInviteUAC::FillAckL
// -----------------------------------------------------------------------------
//
void CInviteUAC::FillAckL(CSIPRequest& aAck, RStringF aBranch)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(iRemoteTarget, KErrNotFound);

	SIPRequestUtility::FillRouteAndRequestUriL(aAck,
											   *iRouteSet,
											   *iRemoteTarget);
    // Copy From-header before Contact-headers are handled
	iTransactionStore.CopyHeadersToRequestL(TransactionId(),
											aAck,
											EFalse, // Request-URI already set
											EFalse);// Don't copy Via
	CheckContactHeadersL(aAck, aAck.From());

	if (!aAck.HasHeader(SIPStrings::StringF(SipStrConsts::EMaxForwardsHeader)))
		{
		SIPRequestUtility::FillNewMaxForwardsL(aAck);
		}

    FillNewViaL(aAck, aBranch);
    // "sub UAC" has no iOutgoingMsg, but the authorization headers have already
    // been copied from the first ACK.
	CSIPMessageUtility::CopyAuthorizationHeadersL(iOutgoingMsg, aAck);
	SIPRequestUtility::FillSupportedSecAgreeL(aAck);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::AckAddressResolvedL
// Remote address must exist.
// -----------------------------------------------------------------------------
//
void CInviteUAC::AckAddressResolvedL()
	{
	__TEST_INVARIANT;	
	__SIP_ASSERT_LEAVE(SelectNextRemoteAddressL(), KErrNotFound);

	if (FillSecurityParamsL())
		{
		SendRequestToNetworkL();
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::SendFirstAckL
// Clear INVITE's route set and resolving results before resolving ACK address.
// -----------------------------------------------------------------------------
//
void CInviteUAC::SendFirstAckL(CSIPRequest* aAck,
							   MTransactionOwner& aObserver,
							   const TSIPTransportParams& aParams,
							   TRegistrationId aRegisterId,
							   const CURIContainer& aRemoteTarget,
							   TBool aDeleteRequest,
							   RStringF aBranch)
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aAck, KErrArgument);
    __SIP_ASSERT_LEAVE(!iObserverForFirstAck, KErrAlreadyExists);
    // Main-UAC must always use a new branch
    if (iAckSenderState && aBranch != SIPStrings::StringF(SipStrConsts::EEmpty))
    	{
    	User::Leave(KErrArgument);
    	}

	iObserver = &aObserver;
	iObserverForFirstAck = &aObserver;
	iRegisterId = aRegisterId;
	iTransportParams = aParams;	

    CheckAckL(*aAck);
	StoreRemoteTargetL(aRemoteTarget);

	delete iRouteSet;
    iRouteSet = NULL;
	iRouteSet = CRouteSet::NewL(*aAck, iRegistrations, iRegisterId);

	FillAckL(*aAck, aBranch);
	StoreRequestUriL(*aAck);

	iResolvingResults->ClearAll();
	ResolveNextHopL(ETrue);

	StoreOutgoingMsg(aAck);
	SetRequestOwnership(aDeleteRequest);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::SendAdditionalAckL
// Get branch value before updating observer. Don't update iObserverForFirstAck.
// -----------------------------------------------------------------------------
//
void CInviteUAC::SendAdditionalAckL(CSIPRequest* aAck,
									MTransactionOwner& aObserver,
								    const TSIPTransportParams& aParams,
									TRegistrationId aRegisterId,
									const CURIContainer& aRemoteTarget,
									TBool aDeleteRequest)
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aAck, KErrArgument);

	iObserver = &aObserver;
	iRegisterId = aRegisterId;

	CUserAgentCreateParams* params =
        CUserAgentCreateParams::NewLC(iTransactionMgr,
                                      iTimers,
								      iSIPMsgUtility,
								      iTimerValues,
								      iRegistrations,
                                      iRegistrationContact,
                                      iSigComp,
                                      iTransactionStore,
                                      iDeleteMgr,
								      aDeleteRequest,
								      aParams);
	params->iTransactionId = TransactionId();	
	params->iObserver = iObserver;
	params->iInitialUaState = iAckSenderState;

	CInviteUAC* subUac =
		CInviteUAC::NewL(*params,
					     iConnectionMgr,
					     iResolver,
					     iSIPSec,
					     0, // CSeq value not used for ACK
					     iIsPrivateAddress, // Not used for ACK
					     NULL); // State is NULL for "sub UAC"
	CleanupStack::PopAndDestroy(params);
	CleanupStack::PushL(subUac);
	iAckSenders.AppendL(subUac);
    CleanupStack::Pop(subUac);

	CSIPMessageUtility::CopyAuthorizationHeadersL(iOutgoingMsg, *aAck);
	subUac->SendAckL(aAck,
					 *iObserver,
					 aParams,
					 iRegisterId,
					 aRemoteTarget,
					 aDeleteRequest,
					 SelectBranchToUse(aObserver));
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::HandleAdditional2xxL
// SIPSec must handle 2xx synchronously. PassRespToTransactionOwnerL allows only
// one final response, so use PassMsgToTransactionOwnerL. Drop other responses.
// -----------------------------------------------------------------------------
//
void CInviteUAC::HandleAdditional2xxL(CSIPResponse* aResp)
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aResp, KErrArgument);

    if (aResp->Type() == CSIPResponse::E2XX)
		{
        __SIP_ASSERT_LEAVE(!PassResponseToSIPSecL(*aResp), KErrGeneral);
		PassMsgToTransactionOwnerL(aResp);
		}
	else
		{
		delete aResp;
		}
	}

// -----------------------------------------------------------------------------
// CInviteUAC::Ptr
// -----------------------------------------------------------------------------
//
CInviteUAC& CInviteUAC::Ptr(CUserAgent& aUserAgent)
	{
	return static_cast<CInviteUAC&>(aUserAgent);
	}

// -----------------------------------------------------------------------------
// CInviteUAC::StopTransaction
// Even if AddDeleteRequest fails, transaction is detached and won't receive
// messages. Transaction doesn't know its id, so write log here.
// -----------------------------------------------------------------------------
//
void CInviteUAC::StopTransaction()
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_RETURN(iTransaction, KErrNotFound);
	__SIP_ASSERT_RETURN(!iDeleteThisTransaction, KErrAlreadyExists);

	// Prevent transaction from calling TransactionEndsL
	iTransaction->DetachFromUserAgent();
	iTransaction->Terminated();
	__SIP_INT_LOG1("TU transaction state=Terminated, taID", TransactionId() )

	if (iDeleteMgr.AddDeleteRequest(iTransaction) != KErrNone)
		{
		iDeleteThisTransaction = iTransaction;
		}
	iTransaction = NULL;
	iTransactionStore.ClearTransaction(TransactionId());

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::StopTimerUaTimeout
// -----------------------------------------------------------------------------
//
void CInviteUAC::StopTimerUaTimeout()
	{
	__TEST_INVARIANT;

	delete iTimerWait2xxRetransmissions;
	iTimerWait2xxRetransmissions = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::StopTimerSIPSec
// -----------------------------------------------------------------------------
//
void CInviteUAC::StopTimerSIPSec()
	{
	__TEST_INVARIANT;

	delete iSIPSecTimer;
	iSIPSecTimer = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::CancelAllTimers
// -----------------------------------------------------------------------------
//
void CInviteUAC::CancelAllTimers()
	{
	__TEST_INVARIANT;

	StopTimerUaTimeout();
	StopTimerSIPSec();
	
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::SelectBranchToUse
// -----------------------------------------------------------------------------
//
RStringF CInviteUAC::SelectBranchToUse(MTransactionOwner& aObserver) const
	{
	__TEST_INVARIANT;

	const RStringF KEmpty = SIPStrings::StringF(SipStrConsts::EEmpty);
	RStringF branch = UsedBranch(aObserver);

	for (TInt i = 0; branch == KEmpty && i < iAckSenders.Count(); ++i)
		{
		branch = iAckSenders[i]->UsedBranch(aObserver);
		}
	return branch;
	}

// -----------------------------------------------------------------------------
// CInviteUAC::UsedBranch
// -----------------------------------------------------------------------------
//
RStringF CInviteUAC::UsedBranch(MTransactionOwner& aObserver) const
	{
	__TEST_INVARIANT;

	if ((&aObserver == iObserverForFirstAck) && iOutgoingMsg)
		{
		const RStringF KBranch = SIPStrings::StringF(SipStrConsts::EBranch);
		CSIPViaHeader* via = CSIPMessageUtility::TopVia(*iOutgoingMsg);
    	if (via && via->HasParam(KBranch))
    		{
    		return via->ParamValue(KBranch);
    		}
		}
	return SIPStrings::StringF(SipStrConsts::EEmpty);
	}
	
// -----------------------------------------------------------------------------
// CInviteUAC::SetSipSecError
// -----------------------------------------------------------------------------
//
void CInviteUAC::SetSipSecError(TInt aErr)
    {
    iSipSecError = aErr;
    }

// -----------------------------------------------------------------------------
// CInviteUAC::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CInviteUAC::__DbgTestInvariant() const
	{
	// A "sub UAC" can't have "sub UACs" or timer
    if (!iAckSenderState &&
        (iAckSenders.Count() > 0 ||
         iTimerWait2xxRetransmissions ||
         iSIPSecTimer))
		{
		User::Invariant();
		}
	}
