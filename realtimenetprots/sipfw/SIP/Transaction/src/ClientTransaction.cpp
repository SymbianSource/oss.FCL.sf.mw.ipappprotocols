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
// Name          : ClientTransaction.cpp
// Part of       : Transaction
// Version       : SIP/5.0
//



#include "uri8.h"
#include "SipAssert.h"
#include "siperr.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipfromheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "DeleteMgr.h"
#include "UserAgentBase.h"
#include "SIPMessageUtility.h"
#include "MTransactionStore.h"

#include "Transmitter.h"
#include "ClientTransaction.h"


// -----------------------------------------------------------------------------
// CClientTransaction::CClientTransaction
// -----------------------------------------------------------------------------
//
CClientTransaction::CClientTransaction(CUserAgentBase& aUserAgent,
									   CTransmitter& aTransmitter,
									   MTimerManager& aTimers,
									   CTransactionState& aInitialState,
									   TTimerValues& aTimerValues,
									   MTransactionStore& aTransactionStore) :
	CTransaction(aUserAgent,
				 aTransmitter,
				 aTimers,
				 aInitialState,
				 aTimerValues),
	iTransactionStore(aTransactionStore)
	{	
	}

// -----------------------------------------------------------------------------
// CClientTransaction::~CClientTransaction
// -----------------------------------------------------------------------------
//
CClientTransaction::~CClientTransaction()
	{
	delete iOutboundProxy;
	}

// -----------------------------------------------------------------------------
// CClientTransaction::TransactionId
// If transaction is detached from UAC, transaction knows the TransactionId.
// -----------------------------------------------------------------------------
//
TTransactionId CClientTransaction::TransactionId() const
	{
	__TEST_INVARIANT;

	if (iUserAgent)
		{
		return iUserAgent->TransactionId();
		}

	return iTransactionId;
	}

// -----------------------------------------------------------------------------
// CClientTransaction::StoreRequest
// -----------------------------------------------------------------------------
//
void CClientTransaction::StoreRequest(CSIPRequest& aReq,
									  const TInetAddr& aAddress,
									  const TSIPTransportParams& aParams,
									  RStringF aTransportProtocol,
									  TBool aForceUDP,
									  CUri8* aOutboundProxy)
	{
	__TEST_INVARIANT;
	__ASSERT_ALWAYS(!iOutgoingMsg && !iOutboundProxy,
					User::Panic(_L("ClientTa:StoreRq"), KErrAlreadyExists));

	iOutgoingMsg = &aReq;
	iRemoteAddr = aAddress;
	iTransportParams = aParams;
	iTransportProtocol = aTransportProtocol;
	iForceUDP = aForceUDP;
	iOutboundProxy = aOutboundProxy;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CClientTransaction::ClearProxy
// -----------------------------------------------------------------------------
//
void CClientTransaction::ClearProxy()
	{
	__TEST_INVARIANT;

	iOutboundProxy = NULL;
	
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CClientTransaction::SendToTransmitterL
// -----------------------------------------------------------------------------
//
void CClientTransaction::SendToTransmitterL()
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(iOutgoingMsg && iTransmitter, KErrNotFound);    

	iTransmitter->SendRequestL(static_cast<CSIPRequest&>(*iOutgoingMsg),							   
							   iRemoteAddr,
							   iForceUDP,
							   iTransportParams,
							   iOutboundProxy,
							   this);
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CClientTransaction::ShouldTaHandleResponse
// Drop invalid response, but don't terminate transaction just because a broken
// SIP response came from the network.
//
// A Non-INVITE transaction passes each response to upper layer only once and
// filters out retransmissions.
//
// INVITE transaction passes every 1xx to upper layer, as INVITE may've forked.
// -----------------------------------------------------------------------------
//
TBool CClientTransaction::ShouldTaHandleResponse(CSIPResponse* aResp) const
	{
	__TEST_INVARIANT;
	__ASSERT_DEBUG(aResp,
		User::Panic(_L("ClientTa:ShouldTaHandleResp !aResp"), KErrArgument));

    //After a final response, ShouldTaHandleResponse must not be used
	__ASSERT_DEBUG(iReceivedResponseCode < 200,
	   User::Panic(_L("ClientTa:ShouldTaHandleResp iRecvRespCode>=200"),
       KErrSIPInvalidTransactionState));

	if ( !CheckReceivedResponse(*aResp) ||
		 (!IsInviteTransaction() &&
	      (iReceivedResponseCode == aResp->ResponseCode())) )
	    {
	    delete aResp;
	    return EFalse;
	    }

	return ETrue;
	}

// -----------------------------------------------------------------------------
// CClientTransaction::PassResponseToUserAgentL
// -----------------------------------------------------------------------------
//
void CClientTransaction::PassResponseToUserAgentL(CSIPResponse* aResp)
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(iUserAgent, KErrNotFound);
    __SIP_ASSERT_LEAVE(aResp, KErrArgument);

	TUint respCode = aResp->ResponseCode();
	iUserAgent->ReceiveL(aResp);
	iReceivedResponseCode = respCode;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CClientTransaction::CheckReceivedResponse
// From-tag must match the request's tag. Response must have exactly one Via.
// -----------------------------------------------------------------------------
//
TBool CClientTransaction::CheckReceivedResponse(CSIPResponse& aResp) const
	{
	__TEST_INVARIANT;

	RStringF from = SIPStrings::StringF(SipStrConsts::EFromHeader);	
	if (!aResp.HasHeader(from))
		{
		return EFalse;
		}

	__ASSERT_DEBUG(iOutgoingMsg,
		User::Panic(_L("ClientTa:ChkRecvResp !iOutMsg"), KErrNotFound));

	TSglQueIter<CSIPHeaderBase> iterReq = iOutgoingMsg->Headers(from);
	TSglQueIter<CSIPHeaderBase> iterResp = aResp.Headers(from);

    return CSIPMessageUtility::CompareTags(
		static_cast<CSIPFromHeader&>(*iterReq),
		static_cast<CSIPFromHeader&>(*iterResp)) &&
		(aResp.HeaderCount(SIPStrings::StringF(SipStrConsts::EViaHeader)) == 1);
	}

// -----------------------------------------------------------------------------
// CClientTransaction::RunIndependently
// -----------------------------------------------------------------------------
//
void CClientTransaction::RunIndependently(TTransactionId aTransactionId,
										  CDeleteMgr* aDeleteMgr,
										  TBool aClearTransmitter)
	{
	__TEST_INVARIANT;
	__ASSERT_DEBUG(!iDeleteMgr,
		User::Panic(_L("ClientTa:RunIndependently iDM!=0"), KErrAlreadyExists));
    __SIP_ASSERT_RETURN(aDeleteMgr, KErrArgument);

	iTransactionId = aTransactionId;
	iDeleteMgr = aDeleteMgr;

	if (aClearTransmitter)
		{
		iTransmitter->DetachCallback(this);
		iTransmitter = NULL;
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CClientTransaction::CheckForTransportChange
// -----------------------------------------------------------------------------
//
void CClientTransaction::CheckForTransportChange()
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_RETURN(iOutgoingMsg, KErrNotFound);

	UpdateTransportProtocol(*iOutgoingMsg);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CClientTransaction::IcmpErrorL
// Terminate with KErrSIPICMPFailure if transaction sends using UDP to the failed address
// KErrSIPICMPFailure will be passed to observers like Profile Agent
// -----------------------------------------------------------------------------
//
void CClientTransaction::IcmpErrorL(const TInetAddr& aAddress,
								    CSipConnectionMgr::TICMPError /*aError*/)
	{
	__TEST_INVARIANT;

	if (IsUnreliableTransportUsed() &&
		CSIPMessageUtility::CompareTInetAddr(iRemoteAddr,
											 TransportProtocol(),
											 aAddress))
		{
        TerminatedL(KErrSIPICMPFailure);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CClientTransaction::TerminatedL
// -----------------------------------------------------------------------------
//
void CClientTransaction::TerminatedL(TInt aReason)
	{
	__TEST_INVARIANT;
	__SIP_INT_LOG1( "CClientTransaction::TerminatedL Error",
					aReason )
	if (!iTerminated)
		{		
		CTransaction::TerminatedL(aReason);

		if (iDeleteMgr)
			{
			//Transaction is detached from UAC, and must remove itself from
			//TransactionStore and use DeleteMgr to free itself.

			if (iDeleteMgr->AddDeleteRequest(this) == KErrNone)
				{
				iTransactionStore.Remove(iTransactionId);
				}
			else
				{
				//Mark transaction to be deleted later on by MTransactionStore
				iTransactionStore.UpdateTransactionId(iTransactionId,
													  KEmptyTransactionId);
				}
			}
		}

	__TEST_INVARIANT;
	}	

// -----------------------------------------------------------------------------
// CClientTransaction::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CClientTransaction::__DbgTestInvariant() const
	{
	if (iOutgoingMsg && !iOutgoingMsg->IsRequest())
		{
		User::Invariant();
		}
	}
