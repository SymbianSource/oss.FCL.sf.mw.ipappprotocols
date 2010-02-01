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
// Name          : InviteServerTaStates.cpp
// Part of       : Transaction
// Version       : SIP/5.0
//



#include "siperr.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "SipAssert.h"
#include "SIPMessageUtility.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

#include "InviteServerTa.h"
#include "InviteServerTaStates.h"
#include "ResponseQueueItem.h"


// -----------------------------------------------------------------------------
// CInviteServerTa_Proceeding::CInviteServerTa_Proceeding
// -----------------------------------------------------------------------------
//
CInviteServerTa_Proceeding::CInviteServerTa_Proceeding()
	{
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Proceeding::~CInviteServerTa_Proceeding
// -----------------------------------------------------------------------------
//
CInviteServerTa_Proceeding::~CInviteServerTa_Proceeding()
	{
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Proceeding::GetState
// -----------------------------------------------------------------------------
//
void
CInviteServerTa_Proceeding::GetState(CSIPInternalStates::TState& aState) const
    {
    aState = CSIPInternalStates::ETransactionProceeding;
    }

#if defined(USE_SIP_LOGS)
// -----------------------------------------------------------------------------
// CInviteServerTa_Proceeding::Log
// -----------------------------------------------------------------------------
//
const TDesC8& CInviteServerTa_Proceeding::Log() const
	{
	_LIT8(KState, "Proceeding");
	return KState;
	}
#endif

// -----------------------------------------------------------------------------
// CInviteServerTa_Proceeding::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void
CInviteServerTa_Proceeding::SetNeighbourStates(CTransactionState& aCompleted,
											   CTransactionState& aSending2xx)
	{
	iCompleted = &aCompleted;
	iSending2xx = &aSending2xx;
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Proceeding::ReceiveL
// -----------------------------------------------------------------------------
//
void CInviteServerTa_Proceeding::ReceiveL(CTransaction& aTransaction,
                                          CSIPRequest* aRequest) const
	{
    __SIP_ASSERT_LEAVE(aRequest, KErrArgument);	

	CInviteServerTransaction& ta = CInviteServerTransaction::Ptr(aTransaction);
	if (!ta.RequestPassedToUA())
		{
		__ASSERT_ALWAYS(ta.UpdateTransportProtocol(*aRequest),
					    User::Leave(KErrSIPMalformedMessage));
		ta.Create100L(*aRequest);
		}

	//Send the most recent response, unless already being sent
	if (!ta.IsTransmitterSending())
		{
		ta.SendToTransmitterL();
		}

	if (ta.RequestPassedToUA())
		{
		//Absorb retransmissions
        delete aRequest;
		}
	else
		{
		//Pass the first INVITE to UAS
		ta.PassRequestToUserAgentL(aRequest);
		}
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Proceeding::SendResponseL
// Queue the new response, if a previous response is being sent.
// -----------------------------------------------------------------------------
//
void CInviteServerTa_Proceeding::SendResponseL(CTransaction& aTransaction,
								  CSIPResponse* aResp,
								  RStringF aProtocol,
								  const TSIPTransportParams& aParams) const
	{
    __SIP_ASSERT_LEAVE(aResp, KErrArgument);
    __SIP_ASSERT_LEAVE(CSIPMessageUtility::CheckTransport(aProtocol),
                       KErrArgument);

	CInviteServerTransaction& ta = CInviteServerTransaction::Ptr(aTransaction);
	TUint respCode = aResp->ResponseCode();
	CResponseQueueItem* respItem =
		CResponseQueueItem::NewLC(aResp, aParams, ETrue);

	if (ta.IsTransmitterSending())
		{
		ta.AddResponseToSendQueueL(respItem);
		}
	else
		{
		/* TimerH is started after the StateChange and sending the Response
		 * This is done to avoid the timer expiry in proceeding state if 
		 * SendAndStoreResponseL() function leaves and state change doesn't happen
		 * In proceeding State, timer expiry is not expected and not handled 
		 */
		ta.ResponseSent(respCode, *iSending2xx, *iCompleted);
		
		ta.SendAndStoreResponseL(respItem, aProtocol);
		
		if (respCode >= 300)
		    {
		    ta.StartTimerHL();
		    }
		}
	CleanupStack::Pop(respItem);
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Proceeding::SendCompleteL
// -----------------------------------------------------------------------------
//
void CInviteServerTa_Proceeding::SendCompleteL(CTransaction& aTransaction) const
	{
	CInviteServerTransaction& ta = CInviteServerTransaction::Ptr(aTransaction);
	CResponseQueueItem* respItem = ta.GetResponseFromSendQueue();
			
     /* TimerH is started after the StateChange and sending the Response
      * This is done to avoid the timer expiry in proceeding state if 
      * SendAndStoreResponseL() function leaves and state change doesn't happen
      * In proceeding State, timer expiry is not expected and not handled 
      */
	if (respItem)
		{
		CleanupStack::PushL(respItem);

		TUint respCode = respItem->Response().ResponseCode();
		
		ta.ResponseSent(respCode, *iSending2xx, *iCompleted);
		        
		ta.SendAndStoreResponseL(respItem);
		        
		if (respCode >= 300)
		    {
            ta.StartTimerHL();
		    }
		CleanupStack::Pop(respItem);
		}
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Proceeding::SendFailedL
// -----------------------------------------------------------------------------
//
void CInviteServerTa_Proceeding::SendFailedL(CTransaction& aTransaction,
											 TInt aError) const
	{
	aTransaction.TerminatedL(aError);
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Completed::CInviteServerTa_Completed
// -----------------------------------------------------------------------------
//
CInviteServerTa_Completed::CInviteServerTa_Completed()
	{
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Completed::~CInviteServerTa_Completed
// -----------------------------------------------------------------------------
//
CInviteServerTa_Completed::~CInviteServerTa_Completed()
	{
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Completed::GetState
// -----------------------------------------------------------------------------
//
void
CInviteServerTa_Completed::GetState(CSIPInternalStates::TState& aState) const
    {
    aState = CSIPInternalStates::ETransactionCompleted;
    }

#if defined(USE_SIP_LOGS)
// -----------------------------------------------------------------------------
// CInviteServerTa_Completed::Log
// -----------------------------------------------------------------------------
//
const TDesC8& CInviteServerTa_Completed::Log() const
	{
	_LIT8(KState, "Completed");
	return KState;
	}
#endif

// -----------------------------------------------------------------------------
// CInviteServerTa_Completed::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void
CInviteServerTa_Completed::SetNeighbourStates(CTransactionState& aConfirmed)
	{
	iConfirmed = &aConfirmed;
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Completed::SendCompleteL
// Only one timer G exists at once. It can already exist if:
// 1. Timer G is started
// 2. INVITE is received and response sent
// 3. SendCompleteL event comes while timer G is running
// -----------------------------------------------------------------------------
//
void CInviteServerTa_Completed::SendCompleteL(CTransaction& aTransaction) const
	{
	if (aTransaction.IsUnreliableTransportUsed())
		{
		CInviteServerTransaction::Ptr(aTransaction).StartTimerGUnlessExistsL();
		}
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Completed::SendFailedL
// -----------------------------------------------------------------------------
//
void CInviteServerTa_Completed::SendFailedL(CTransaction& aTransaction,
											TInt aError) const
	{
	aTransaction.TerminatedL(aError);
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Completed::ReceiveL
// Don't pass ACK to UAS. If retransmitted INVITE, retransmit the most recent
// response unless already being sent.
// -----------------------------------------------------------------------------
//
void CInviteServerTa_Completed::ReceiveL(CTransaction& aTransaction,
                                         CSIPRequest* aRequest) const
	{
    __SIP_ASSERT_LEAVE(aRequest, KErrArgument);

	CInviteServerTransaction& ta = CInviteServerTransaction::Ptr(aTransaction);

	if (CSIPMessageUtility::IsAck(*aRequest))
		{
		ta.EnterConfirmedStateL(*iConfirmed);
		}
	else 
        {
        if ((aRequest->Method() == SIPStrings::StringF(SipStrConsts::EInvite))
            &&
            !ta.IsTransmitterSending())
		    {
		    ta.SendToTransmitterL();
            }
		}

    delete aRequest;
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Completed::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CInviteServerTa_Completed::TimerExpiredL(CTransaction& aTransaction,
											  TTimerId aTimerId,
											  TAny* aTimerParam) const
	{    
    __SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);
	
	HandleTimerExpirationL(aTransaction, aTimerId, aTimerParam);
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Confirmed::CInviteServerTa_Confirmed
// -----------------------------------------------------------------------------
//
CInviteServerTa_Confirmed::CInviteServerTa_Confirmed()
	{
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Confirmed::~CInviteServerTa_Confirmed
// -----------------------------------------------------------------------------
//
CInviteServerTa_Confirmed::~CInviteServerTa_Confirmed()
	{
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Confirmed::GetState
// -----------------------------------------------------------------------------
//
void
CInviteServerTa_Confirmed::GetState(CSIPInternalStates::TState& aState) const
    {
    aState = CSIPInternalStates::ETransactionConfirmed;
    }

#if defined(USE_SIP_LOGS)
// -----------------------------------------------------------------------------
// CInviteServerTa_Confirmed::Log
// -----------------------------------------------------------------------------
//
const TDesC8& CInviteServerTa_Confirmed::Log() const
	{
	_LIT8(KState, "Confirmed");
	return KState;
	}
#endif

// -----------------------------------------------------------------------------
// CInviteServerTa_Confirmed::CanContinueWithoutOutgoingMsg
// This is never called for a server side.
// -----------------------------------------------------------------------------
//
TBool CInviteServerTa_Confirmed::CanContinueWithoutOutgoingMsg() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Confirmed::SendCompleteL
// Happens if entered Confirmed state before CTransmitter finished sending.
// Nothing is sent anymore, so no need to update transport protocol.
// -----------------------------------------------------------------------------
//
void
CInviteServerTa_Confirmed::SendCompleteL(CTransaction& /*aTransaction*/) const
	{
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Confirmed::SendFailedL
// No action, wait for timer I to expire.
// -----------------------------------------------------------------------------
//
void CInviteServerTa_Confirmed::SendFailedL(CTransaction& /*aTransaction*/,
											TInt /*aError*/) const
	{
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Confirmed::ReceiveL
// Absorb any additional ACKs.
// -----------------------------------------------------------------------------
//
void CInviteServerTa_Confirmed::ReceiveL(CTransaction& /*aTransaction*/,
                                         CSIPRequest* aRequest) const
	{
    __SIP_ASSERT_LEAVE(aRequest, KErrArgument);

    delete aRequest;
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Confirmed::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CInviteServerTa_Confirmed::TimerExpiredL(CTransaction& aTransaction,
											  TTimerId aTimerId,
											  TAny* aTimerParam) const
	{    
    __SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);	

	HandleTimerExpirationL(aTransaction, aTimerId, aTimerParam);
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Sending2xx::CInviteServerTa_Sending2xx
// -----------------------------------------------------------------------------
//
CInviteServerTa_Sending2xx::CInviteServerTa_Sending2xx()
	{
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Sending2xx::~CInviteServerTa_Sending2xx
// -----------------------------------------------------------------------------
//
CInviteServerTa_Sending2xx::~CInviteServerTa_Sending2xx()
	{
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Sending2xx::GetState
// 2xx was sent but SendCompleteL not yet received. Treat state as terminated,
// as "Sending2xx" is implementation's internal state.
// -----------------------------------------------------------------------------
//
void
CInviteServerTa_Sending2xx::GetState(CSIPInternalStates::TState& aState) const
    {
    aState = CSIPInternalStates::ETransactionTerminated;
    }

#if defined(USE_SIP_LOGS)
// -----------------------------------------------------------------------------
// CInviteServerTa_Sending2xx::Log
// Don't log transition to this state, as RFC doesn't specify it.
// -----------------------------------------------------------------------------
//
const TDesC8& CInviteServerTa_Sending2xx::Log() const
	{
    return EnteringThisStateIsntLogged();
	}
#endif

// -----------------------------------------------------------------------------
// CInviteServerTa_Sending2xx::CanContinueWithoutOutgoingMsg
// This is never called for a server side.
// -----------------------------------------------------------------------------
//
TBool CInviteServerTa_Sending2xx::CanContinueWithoutOutgoingMsg() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Sending2xx::SendCompleteL
// -----------------------------------------------------------------------------
//
void CInviteServerTa_Sending2xx::SendCompleteL(CTransaction& aTransaction) const
	{    
	aTransaction.TerminatedL(KErrNone);
	}

// -----------------------------------------------------------------------------
// CInviteServerTa_Sending2xx::SendFailedL
// -----------------------------------------------------------------------------
//
void CInviteServerTa_Sending2xx::SendFailedL(CTransaction& aTransaction,
											 TInt aError) const
	{
	aTransaction.TerminatedL(aError);
	}
