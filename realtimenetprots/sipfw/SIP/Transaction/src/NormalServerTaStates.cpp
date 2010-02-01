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
// Name          : NormalServerTaStates.cpp
// Part of       : Transaction
// Version       : SIP/5.0
//



#include "siperr.h"
#include "SipAssert.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "SIPMessageUtility.h"

#include "NormalServerTa.h"
#include "NormalServerTaStates.h"
#include "ResponseQueueItem.h"

// -----------------------------------------------------------------------------
// CNormalServerTa_Trying::CNormalServerTa_Trying
// -----------------------------------------------------------------------------
//
CNormalServerTa_Trying::CNormalServerTa_Trying()
	{
	}

// -----------------------------------------------------------------------------
// CNormalServerTa_Trying::~CNormalServerTa_Trying
// -----------------------------------------------------------------------------
//
CNormalServerTa_Trying::~CNormalServerTa_Trying()
	{
	}

// -----------------------------------------------------------------------------
// CNormalServerTa_Trying::GetState
// -----------------------------------------------------------------------------
//
void CNormalServerTa_Trying::GetState(CSIPInternalStates::TState& aState) const
    {
    aState = CSIPInternalStates::ETransactionTrying;
    }

// -----------------------------------------------------------------------------
// CNormalServerTa_Trying::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CNormalServerTa_Trying::SetNeighbourStates(CTransactionState& aProceeding,
												CTransactionState& aCompleted)
	{
	iProceeding = &aProceeding;
	iCompleted = &aCompleted;
	}

// -----------------------------------------------------------------------------
// CNormalServerTa_Trying::ReceiveL
// Response can be sent when TimerSend100 has expired or been stopped.
// -----------------------------------------------------------------------------
//
void CNormalServerTa_Trying::ReceiveL(CTransaction& aTransaction,
	                                  CSIPRequest* aRequest) const
	{
    __SIP_ASSERT_LEAVE(aRequest, KErrArgument);

	CNormalServerTransaction& ta = CNormalServerTransaction::Ptr(aTransaction);

	if (ta.RequestPassedToUA())
		{
		if (!ta.IsTimerSend100Running() && !ta.IsTransmitterSending())
			{
			ta.SendToTransmitterL();
			}
		delete aRequest;
		}
	else
		{
		__ASSERT_ALWAYS(ta.UpdateTransportProtocol(*aRequest),
					    User::Leave(KErrSIPMalformedMessage));		
		ta.StartTimerF2UnlessExistsL();		
		if (!ta.IsTimerSend100Running())
			{	
			ta.StartTimerSend100L(*aRequest);
			}
		ta.PassRequestToUserAgentL(aRequest);
		}
	}

// -----------------------------------------------------------------------------
// CNormalServerTa_Trying::SendResponseL
// -----------------------------------------------------------------------------
//
void CNormalServerTa_Trying::SendResponseL(CTransaction& aTransaction,
								   CSIPResponse* aResp,
								   RStringF aProtocol,
								   const TSIPTransportParams& aParams) const
	{
	__SIP_ASSERT_LEAVE(aResp, KErrArgument);
    __SIP_ASSERT_LEAVE(CSIPMessageUtility::CheckTransport(aProtocol),
                       KErrArgument);

	TBool isFinalResp = CSIPMessageUtility::IsFinalResponse(*aResp);
	CNormalServerTransaction& ta = CNormalServerTransaction::Ptr(aTransaction);
	CResponseQueueItem* respItem =
		CResponseQueueItem::NewLC(aResp, aParams, EFalse);

	ta.SendAndStoreResponseL(respItem, aProtocol);
	CleanupStack::Pop(respItem);

	ta.StopTimerSend100();
	if (isFinalResp)
		{
		ta.StopTimerF2();
		aTransaction.ChangeState(*iCompleted);
		}
	else
		{
		aTransaction.ChangeState(*iProceeding);
		}
	}

// -----------------------------------------------------------------------------
// CNormalServerTa_Trying::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CNormalServerTa_Trying::TimerExpiredL(CTransaction& aTransaction,
										   TTimerId aTimerId,
										   TAny* aTimerParam) const
	{
	__SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);

	HandleTimerExpirationL(aTransaction, aTimerId, aTimerParam);
	}

// -----------------------------------------------------------------------------
// CNormalServerTa_Proceeding::CNormalServerTa_Proceeding
// -----------------------------------------------------------------------------
//
CNormalServerTa_Proceeding::CNormalServerTa_Proceeding()
	{
	}

// -----------------------------------------------------------------------------
// CNormalServerTa_Proceeding::~CNormalServerTa_Proceeding
// -----------------------------------------------------------------------------
//
CNormalServerTa_Proceeding::~CNormalServerTa_Proceeding()
	{
	}

// -----------------------------------------------------------------------------
// CNormalServerTa_Proceeding::GetState
// -----------------------------------------------------------------------------
//
void
CNormalServerTa_Proceeding::GetState(CSIPInternalStates::TState& aState) const
    {
    aState = CSIPInternalStates::ETransactionProceeding;
    }

// -----------------------------------------------------------------------------
// CNormalServerTa_Proceeding::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void
CNormalServerTa_Proceeding::SetNeighbourStates(CTransactionState& aCompleted)
	{
	iCompleted = &aCompleted;
	}

// -----------------------------------------------------------------------------
// CNormalServerTa_Proceeding::SendCompleteL
// -----------------------------------------------------------------------------
//
void CNormalServerTa_Proceeding::SendCompleteL(CTransaction& aTransaction) const
	{
	CNormalServerTransaction& ta = CNormalServerTransaction::Ptr(aTransaction);
	CResponseQueueItem* respItem = ta.GetResponseFromSendQueue();

	if (respItem)
		{
		TBool isFinalResp =
			CSIPMessageUtility::IsFinalResponse(respItem->Response());

		CleanupStack::PushL(respItem);
		ta.SendAndStoreResponseL(respItem);
		CleanupStack::Pop(respItem);

        if (isFinalResp)
			{
			ta.StopTimerF2();
			aTransaction.ChangeState(*iCompleted);
			}
		}
	}

// -----------------------------------------------------------------------------
// CNormalServerTa_Proceeding::SendFailedL
// -----------------------------------------------------------------------------
//
void CNormalServerTa_Proceeding::SendFailedL(CTransaction& aTransaction,
											 TInt aError) const
	{
	aTransaction.TerminatedL(aError);
	}

// -----------------------------------------------------------------------------
// CNormalServerTa_Proceeding::ReceiveL
// -----------------------------------------------------------------------------
//
void CNormalServerTa_Proceeding::ReceiveL(CTransaction& aTransaction,
	                                      CSIPRequest* aRequest) const
	{
	CNormalServerTransaction::Ptr(aTransaction).ReceiveRetransmissionL(aRequest);
	}

// -----------------------------------------------------------------------------
// CNormalServerTa_Proceeding::SendResponseL
// If sending a previous response, queue the new response.
// -----------------------------------------------------------------------------
//
void CNormalServerTa_Proceeding::SendResponseL(CTransaction& aTransaction,
								   CSIPResponse* aResp,
								   RStringF aProtocol,
								   const TSIPTransportParams& aParams) const
	{
	__SIP_ASSERT_LEAVE(aResp, KErrArgument);
    __SIP_ASSERT_LEAVE(CSIPMessageUtility::CheckTransport(aProtocol),
                       KErrArgument);

	CNormalServerTransaction& ta = CNormalServerTransaction::Ptr(aTransaction);
	CResponseQueueItem* respItem =
		CResponseQueueItem::NewLC(aResp, aParams, EFalse);
	if (ta.IsTransmitterSending())
		{
		ta.AddResponseToSendQueueL(respItem);
		}
	else
		{		
		TBool isFinalResp =
			CSIPMessageUtility::IsFinalResponse(respItem->Response());

		ta.SendAndStoreResponseL(respItem, aProtocol);

        if (isFinalResp)
			{
			ta.StopTimerF2();
			aTransaction.ChangeState(*iCompleted);
			}
		}
	CleanupStack::Pop(respItem);
	}

// -----------------------------------------------------------------------------
// CNormalServerTa_Proceeding::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CNormalServerTa_Proceeding::TimerExpiredL(CTransaction& aTransaction,
											   TTimerId aTimerId,
											   TAny* aTimerParam) const
	{
	__SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);

	HandleTimerExpirationL(aTransaction, aTimerId, aTimerParam);
	}

// -----------------------------------------------------------------------------
// CNormalServerTa_Completed::CNormalServerTa_Completed
// -----------------------------------------------------------------------------
//
CNormalServerTa_Completed::CNormalServerTa_Completed()
	{
	}

// -----------------------------------------------------------------------------
// CNormalServerTa_Completed::~CNormalServerTa_Completed
// -----------------------------------------------------------------------------
//
CNormalServerTa_Completed::~CNormalServerTa_Completed()
	{
	}

// -----------------------------------------------------------------------------
// CNormalServerTa_Completed::GetState
// -----------------------------------------------------------------------------
//
void
CNormalServerTa_Completed::GetState(CSIPInternalStates::TState& aState) const
    {
    aState = CSIPInternalStates::ETransactionCompleted;
    }

// -----------------------------------------------------------------------------
// CNormalServerTa_Completed::SendCompleteL
// -----------------------------------------------------------------------------
//
void CNormalServerTa_Completed::SendCompleteL(CTransaction& aTransaction) const
	{
	CNormalServerTransaction::Ptr(aTransaction).StartTimerJUnlessExistsL();
	}

// -----------------------------------------------------------------------------
// CNormalServerTa_Completed::SendFailedL
// -----------------------------------------------------------------------------
//
void CNormalServerTa_Completed::SendFailedL(CTransaction& aTransaction,
											TInt aError) const
	{
	aTransaction.TerminatedL(aError);
	}

// -----------------------------------------------------------------------------
// CNormalServerTa_Completed::ReceiveL
// -----------------------------------------------------------------------------
//
void CNormalServerTa_Completed::ReceiveL(CTransaction& aTransaction,
	                                     CSIPRequest* aRequest) const
	{
	CNormalServerTransaction::Ptr(aTransaction).ReceiveRetransmissionL(aRequest);    
	}

// -----------------------------------------------------------------------------
// CNormalServerTa_Completed::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CNormalServerTa_Completed::TimerExpiredL(CTransaction& aTransaction,
											  TTimerId aTimerId,
											  TAny* aTimerParam) const
	{	
	__SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);

	HandleTimerExpirationL(aTransaction, aTimerId, aTimerParam);
	}
