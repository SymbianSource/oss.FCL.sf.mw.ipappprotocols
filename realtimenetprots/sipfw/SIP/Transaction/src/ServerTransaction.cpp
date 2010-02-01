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
// Name          : ServerTransaction.cpp
// Part of       : Transaction
// Version       : SIP/5.0
//



#include "SipAssert.h"
#include "siperr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "UserAgentBase.h"
#include "SIPMessageUtility.h"

#include "ServerTransaction.h"
#include "ResponseQueue.h"
#include "ResponseQueueItem.h"
#include "Transmitter.h"


// -----------------------------------------------------------------------------
// CServerTransaction::CServerTransaction
// -----------------------------------------------------------------------------
//
CServerTransaction::CServerTransaction(CUserAgentBase& aUserAgent,
									   CTransmitter& aTransmitter,
									   MTimerManager& aTimers,
									   CTransactionState& aInitialState,
									   TTimerValues& aTimerValues) :
	CTransaction(aUserAgent,
				 aTransmitter,
				 aTimers,
				 aInitialState,
				 aTimerValues)
	{
	}

// -----------------------------------------------------------------------------
// CServerTransaction::ConstructServerTaL
// -----------------------------------------------------------------------------
//
void CServerTransaction::ConstructServerTaL()
	{
	iSendQueue = CResponseQueue::NewL();
	iSentResponses = CResponseQueue::NewL();
	}

// -----------------------------------------------------------------------------
// CServerTransaction::~CServerTransaction
// -----------------------------------------------------------------------------
//
CServerTransaction::~CServerTransaction()
	{	
	delete iOutgoingMsg;
	delete iSendQueue;
	delete iSentResponses;	
	}

// -----------------------------------------------------------------------------
// CServerTransaction::PassRequestToUserAgentL
// -----------------------------------------------------------------------------
//
void CServerTransaction::PassRequestToUserAgentL(CSIPRequest* aReq)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aReq, KErrArgument);
	__SIP_ASSERT_LEAVE(iUserAgent, KErrNotFound);
	__SIP_ASSERT_LEAVE(!iRequestPassedToUA, KErrAlreadyExists);

	iUserAgent->ReceiveL(aReq);
	iRequestPassedToUA = ETrue;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CServerTransaction::RequestPassedToUA
// -----------------------------------------------------------------------------
//
TBool CServerTransaction::RequestPassedToUA() const
	{
	__TEST_INVARIANT;
	return iRequestPassedToUA;	
	}

// -----------------------------------------------------------------------------
// CServerTransaction::Create100L
// If no To-tag in aReq, it is not added to response.
// iOutgoingMsg can theorically already have 100, if leave occurs after
// Create100L and a retransmitted request is received before transaction stops.
// -----------------------------------------------------------------------------
//
void CServerTransaction::Create100L(CSIPRequest& aReq)
	{
	__TEST_INVARIANT;
    if (!iOutgoingMsg)
    	{
		CSIPResponse* resp = CSIPResponse::NewLC(100,
			SIPStrings::StringF(SipStrConsts::EPhraseTrying));

		CSIPMessageUtility::CopyHeadersL(aReq, *resp,
							SIPStrings::StringF(SipStrConsts::EToHeader));
		CSIPMessageUtility::CopyHeadersL(aReq, *resp,
							SIPStrings::StringF(SipStrConsts::EFromHeader));
		CSIPMessageUtility::CopyHeadersL(aReq, *resp,
							SIPStrings::StringF(SipStrConsts::ECallIDHeader));
		CSIPMessageUtility::CopyHeadersL(aReq, *resp,
							SIPStrings::StringF(SipStrConsts::ECSeqHeader));
		CSIPMessageUtility::CopyHeadersL(aReq, *resp,
							SIPStrings::StringF(SipStrConsts::EViaHeader));

		CleanupStack::Pop(resp);
		iOutgoingMsg = resp;
    	}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CServerTransaction::SendToTransmitterL
// -----------------------------------------------------------------------------
//
void CServerTransaction::SendToTransmitterL()
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(iOutgoingMsg, KErrNotFound);
	__SIP_ASSERT_LEAVE(!iOutgoingMsg->IsRequest(), KErrSIPMalformedMessage);

	iTransmitter->SendResponseL(static_cast<CSIPResponse&>(*iOutgoingMsg),
								iTransportParams,
								this,
								TransactionId());
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CServerTransaction::SendAndStoreResponseL
// -----------------------------------------------------------------------------
//
void CServerTransaction::SendAndStoreResponseL(CResponseQueueItem* aRespItem)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aRespItem, KErrArgument);
    __SIP_ASSERT_LEAVE(CSIPMessageUtility::CheckTransport(iTransportProtocol),
                       KErrArgument);	

	SendAndStoreResponseL(aRespItem, iTransportProtocol);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CServerTransaction::SendAndStoreResponseL
// -----------------------------------------------------------------------------
//
void CServerTransaction::SendAndStoreResponseL(CResponseQueueItem* aRespItem,
											   RStringF aTransportProtocol)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aRespItem, KErrArgument);
    __SIP_ASSERT_LEAVE(CSIPMessageUtility::CheckTransport(aTransportProtocol),
                       KErrArgument);

	CResponseQueueItem* previousRespItem = NULL;
	if (iOutgoingMsg)
        {
        //Earlier response must be a 1xx, as UAS sends only one final response.
        __ASSERT_DEBUG(!CSIPMessageUtility::IsFinalResponse(
        	static_cast<CSIPResponse&>(*iOutgoingMsg)),
            User::Panic(_L("ServTa:SendAndStoreRespL"), KErrAlreadyExists));
		previousRespItem =
			CResponseQueueItem::NewLC(static_cast<CSIPResponse*>(iOutgoingMsg),
									  iTransportParams,
									  //aRespItem has same value for "is INVITE"
									  aRespItem->IsRespToInvite());
        }

	iTransmitter->SendResponseL(aRespItem->Response(),
								aRespItem->TransportParams(),
								this,
								TransactionId());
    if (iOutgoingMsg)
        {
		TInt status = iSentResponses->Add(previousRespItem);
        if (status != KErrNone)
            {
            //ConnectionMgr must not use the response
			iTransmitter->CancelSendResponses(TransactionId(), ETrue);
            User::Leave(status);
            }
        CleanupStack::Pop(previousRespItem);
		}

	iTransportProtocol = aTransportProtocol;
	iTransportParams = aRespItem->TransportParams();

	iOutgoingMsg = &aRespItem->Response();
	aRespItem->SetResponseOwnership(EFalse);
	delete aRespItem;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CServerTransaction::AddResponseToSendQueueL
// -----------------------------------------------------------------------------
//
void
CServerTransaction::AddResponseToSendQueueL(CResponseQueueItem* aRespItem) const
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aRespItem, KErrArgument);

    User::LeaveIfError(iSendQueue->Add(aRespItem));    
	}

// -----------------------------------------------------------------------------
// CServerTransaction::GetResponseFromSendQueue
// -----------------------------------------------------------------------------
//
CResponseQueueItem* CServerTransaction::GetResponseFromSendQueue() const
	{
	__TEST_INVARIANT;
	return iSendQueue->GetNext();
	}

// -----------------------------------------------------------------------------
// CServerTransaction::IcmpErrorL
// ServerTransaction ignores ICMP error
// -----------------------------------------------------------------------------
//
void CServerTransaction::IcmpErrorL(const TInetAddr& /*aAddress*/,
								    CSipConnectionMgr::TICMPError /*aError*/)
	{
	}

// -----------------------------------------------------------------------------
// CServerTransaction::TerminatedL
// -----------------------------------------------------------------------------
//
void CServerTransaction::TerminatedL(TInt aReason)
	{
	__TEST_INVARIANT;

	if (!iTerminated)
        {
		if (iTransmitter)
			{
            //Responses will be deleted, ConnectionMgr must not use them.
			iTransmitter->CancelSendResponses(TransactionId(),
											  !IsInviteTransaction());
			}

		CTransaction::TerminatedL(aReason);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CServerTransaction::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CServerTransaction::__DbgTestInvariant() const
	{
	if (!iSendQueue || !iSentResponses)
		{	
		User::Invariant();
		}
	}

