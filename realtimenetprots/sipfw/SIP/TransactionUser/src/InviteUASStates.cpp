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
// Name          : InviteUASStates.cpp
// Part of       : TransactionUser
// Version       : SIP/6.0
//



#include "siperr.h"
#include "sipresponse.h"
#include "siprequest.h"
#include "SipLogs.h"
#include "SipAssert.h"

#include "InviteUAS.h"
#include "InviteUASStates.h"
#include "SIPMessageUtility.h"


// -----------------------------------------------------------------------------
// CInviteUAS_Start::CInviteUAS_Start
// -----------------------------------------------------------------------------
//
CInviteUAS_Start::CInviteUAS_Start()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAS_Start::~CInviteUAS_Start
// -----------------------------------------------------------------------------
//
CInviteUAS_Start::~CInviteUAS_Start()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAS_Start::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CInviteUAS_Start::SetNeighbourStates(CUserAgentState& aGetTxOwner,
										  CUserAgentState& aWaitRespFromApp,
										  CUserAgentState& aRejected)
	{    
	iGetTxOwner = &aGetTxOwner;
	iWaitRespFromApp = &aWaitRespFromApp;
	iRejected = &aRejected;
	}

// -----------------------------------------------------------------------------
// CInviteUAS_Start::TransactionEndsL
// Can happen if leave occurs when obtaining upper layer callback.
// -----------------------------------------------------------------------------
//
void
CInviteUAS_Start::TransactionEndsL(CUserAgent& aUserAgent, TInt aReason) const
	{
	aUserAgent.Stop(aReason);
	}

// -----------------------------------------------------------------------------
// CInviteUAS_Start::ReceiveL
// -----------------------------------------------------------------------------
//
void
CInviteUAS_Start::ReceiveL(CUserAgent& aUserAgent, CSIPRequest* aRequest) const
	{
    __SIP_ASSERT_LEAVE(aRequest, KErrArgument);    
	
	CInviteUAS::Ptr(aUserAgent).InitialRequestReceivedL(aRequest,
													    *iGetTxOwner,
													    *iWaitRespFromApp,
													    *iRejected);
	}

// -----------------------------------------------------------------------------
// CInviteUAS_Start::SendResponseL
// Application shouldn't know about this transactionId yet
// -----------------------------------------------------------------------------
//
void
CInviteUAS_Start::SendResponseL(CUserAgent& /*aUserAgent*/,
								CSIPResponse* aResp,
								const TSIPTransportParams& /*aParams*/) const
	{
    __SIP_ASSERT_LEAVE(aResp, KErrArgument);

	User::Leave(KErrSIPInvalidTransactionState);
	}

// -----------------------------------------------------------------------------
// CInviteUAS_Start::CancelL
// -----------------------------------------------------------------------------
//
TBool CInviteUAS_Start::CancelL(CUserAgent& aUserAgent) const
	{
	CInviteUAS::Ptr(aUserAgent).HandleCancelInviteL(*iRejected);
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CInviteUAS_GetTxOwner::CInviteUAS_GetTxOwner
// -----------------------------------------------------------------------------
//
CInviteUAS_GetTxOwner::CInviteUAS_GetTxOwner()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAS_GetTxOwner::~CInviteUAS_GetTxOwner
// -----------------------------------------------------------------------------
//
CInviteUAS_GetTxOwner::~CInviteUAS_GetTxOwner()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAS_GetTxOwner::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void
CInviteUAS_GetTxOwner::SetNeighbourStates(CUserAgentState& aWaitRespFromApp,
										  CUserAgentState& aRejected)
	{
	iWaitRespFromApp = &aWaitRespFromApp;
	iRejected = &aRejected;
	}

// -----------------------------------------------------------------------------
// CInviteUAS_GetTxOwner::OwnerFoundL
// -----------------------------------------------------------------------------
//
void CInviteUAS_GetTxOwner::OwnerFoundL(CUserAgent& aUserAgent,
										MTransactionOwner* aOwner) const
	{
    __SIP_ASSERT_LEAVE(aOwner, KErrArgument);

	CInviteUAS::Ptr(aUserAgent).HandleOwnerFoundL(aOwner, *iWaitRespFromApp);
	}

// -----------------------------------------------------------------------------
// CInviteUAS_GetTxOwner::OwnerNotFoundL
// -----------------------------------------------------------------------------
//
void CInviteUAS_GetTxOwner::OwnerNotFoundL(CUserAgent& aUserAgent,
										   CSIPResponse* aResp) const
	{
    __SIP_ASSERT_LEAVE(aResp, KErrArgument);

	CInviteUAS::Ptr(aUserAgent).HandleOwnerNotFoundL(aResp, *iRejected);
	}

// -----------------------------------------------------------------------------
// CInviteUAS_GetTxOwner::SendResponseL
// Application shouldn't know about this transaction yet
// -----------------------------------------------------------------------------
//
void CInviteUAS_GetTxOwner::SendResponseL(CUserAgent& /*aUserAgent*/,
							     CSIPResponse* aResp,
								 const TSIPTransportParams& /*aParams*/) const
	{
    __SIP_ASSERT_LEAVE(aResp, KErrArgument);
	
	User::Leave(KErrSIPInvalidTransactionState);
	}

// -----------------------------------------------------------------------------
// CInviteUAS_GetTxOwner::TransactionEndsL
// -----------------------------------------------------------------------------
//
void CInviteUAS_GetTxOwner::TransactionEndsL(CUserAgent& aUserAgent,
											 TInt aReason) const
	{
	aUserAgent.Stop(aReason);
	}

// -----------------------------------------------------------------------------
// CInviteUAS_GetTxOwner::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CInviteUAS_GetTxOwner::TimerExpiredL(CUserAgent& aUserAgent,
									      TTimerId aTimerId,
										  TAny* aTimerParam) const
	{
    __SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);    

	HandleTimerExpirationL(aUserAgent, aTimerId, aTimerParam);
	}

// -----------------------------------------------------------------------------
// CInviteUAS_GetTxOwner::RequestRouterErrorL
// -----------------------------------------------------------------------------
//
void CInviteUAS_GetTxOwner::RequestRouterErrorL(CUserAgent& aUserAgent) const
	{
	static_cast<CUserAgentServer*>(&aUserAgent)->
        HandleRequestRouterErrorL(*iRejected);
	}

// -----------------------------------------------------------------------------
// CInviteUAS_GetTxOwner::CancelL
// -----------------------------------------------------------------------------
//
TBool CInviteUAS_GetTxOwner::CancelL(CUserAgent& aUserAgent) const
	{    
	CInviteUAS::Ptr(aUserAgent).HandleCancelInviteL(*iRejected);
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CInviteUAS_WaitRespFromApp::CInviteUAS_WaitRespFromApp
// -----------------------------------------------------------------------------
//
CInviteUAS_WaitRespFromApp::CInviteUAS_WaitRespFromApp()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAS_WaitRespFromApp::~CInviteUAS_WaitRespFromApp
// -----------------------------------------------------------------------------
//
CInviteUAS_WaitRespFromApp::~CInviteUAS_WaitRespFromApp()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAS_WaitRespFromApp::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CInviteUAS_WaitRespFromApp::SetNeighbourStates(CUserAgentState& aAccepted,
									  				CUserAgentState& aRejected)
	{
	iAccepted = &aAccepted;
	iRejected = &aRejected;
	}

// -----------------------------------------------------------------------------
// CInviteUAS_WaitRespFromApp::SendResponseL
// -----------------------------------------------------------------------------
//
void CInviteUAS_WaitRespFromApp::SendResponseL(CUserAgent& aUserAgent,
								   CSIPResponse* aResp,
								   const TSIPTransportParams& aParams) const
	{
    __SIP_ASSERT_LEAVE(aResp, KErrArgument);    
	
	CInviteUAS& ua = CInviteUAS::Ptr(aUserAgent);	
	ua.HandleSendResponseL(aResp, aParams);
	
	if (aResp->Type() == CSIPResponse::E2XX)
		{
		aUserAgent.ChangeState(*iAccepted);
		ua.SendFirst2xxL(aResp);
		}
	else
		{
		if (aResp->IsErrorResponse())
			{
			// Enter "rejected" before sending response to transaction. If
			// transaction ends already in SendMessageToTransactionL, UAS is in
			// the correct state.
			aUserAgent.ChangeState(*iRejected);	
			}
		ua.SendResponseToTransactionL(aResp);
		}	
	}

// -----------------------------------------------------------------------------
// CInviteUAS_WaitRespFromApp::TransactionEndsL
// Transaction ends, likely application hasn't sent any final response.
// -----------------------------------------------------------------------------
//
void CInviteUAS_WaitRespFromApp::TransactionEndsL(CUserAgent& aUserAgent,
												  TInt aReason) const
	{
	aUserAgent.Stop(aReason);
	}

// -----------------------------------------------------------------------------
// CInviteUAS_WaitRespFromApp::ShouldUASStop
// -----------------------------------------------------------------------------
//
TBool CInviteUAS_WaitRespFromApp::ShouldUASStop() const
	{
	return ETrue;	
	}

// -----------------------------------------------------------------------------
// CInviteUAS_WaitRespFromApp::CancelL
// -----------------------------------------------------------------------------
//
TBool CInviteUAS_WaitRespFromApp::CancelL(CUserAgent& aUserAgent) const
	{    
	CInviteUAS::Ptr(aUserAgent).HandleCancelInviteL(*iRejected);
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CInviteUAS_Accepted::CInviteUAS_Accepted
// -----------------------------------------------------------------------------
//
CInviteUAS_Accepted::CInviteUAS_Accepted()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAS_Accepted::~CInviteUAS_Accepted
// -----------------------------------------------------------------------------
//
CInviteUAS_Accepted::~CInviteUAS_Accepted()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAS_Accepted::SendResponseL
// Final response has already been sent
// -----------------------------------------------------------------------------
//
void
CInviteUAS_Accepted::SendResponseL(CUserAgent& /*aUserAgent*/,
								   CSIPResponse* aResp,
								   const TSIPTransportParams& /*aParams*/) const
	{
    __SIP_ASSERT_LEAVE(aResp, KErrArgument);

	User::Leave(KErrSIPInvalidTransactionState);
	}

// -----------------------------------------------------------------------------
// CInviteUAS_Accepted::TransactionEndsL
// Free the transaction that sent the first 2xx. Even if send failed, UAS begins
// to retransmit the 2xx, regardless of the used transport protocol.
// -----------------------------------------------------------------------------
//
void CInviteUAS_Accepted::TransactionEndsL(CUserAgent& aUserAgent,
										   TInt /*aReason*/) const
	{
	aUserAgent.RequestDeletionOfTransactionL();	
	CInviteUAS::Ptr(aUserAgent).StartTimerRetransmit2xxL();
	}

// -----------------------------------------------------------------------------
// CInviteUAS_Accepted::ReceiveL
// -----------------------------------------------------------------------------
//
void CInviteUAS_Accepted::ReceiveL(CUserAgent& aUserAgent, 
	                               CSIPRequest* aRequest) const
	{
    __SIP_ASSERT_LEAVE(aRequest, KErrArgument);    
    
	if (CSIPMessageUtility::IsAck(*aRequest))
		{
		// Received ACK to 2xx directly from ConnectionMgr. CUserAgent::ReceiveL
		// wrote ACK to log, unless CUserAgent::iTransaction still runs.
		if (aUserAgent.Transaction())
			{
			__SIP_MESSAGE_LOG("TransactionUser", *aRequest)
			}

        CInviteUAS& ua = CInviteUAS::Ptr(aUserAgent);
		ua.StopTimerRetransmit2xx();

        // Ask MTransactionOwner also for ACK
		if (ua.DoesDialogExistForAckL(*aRequest))
			{
			aUserAgent.PassMsgToTransactionOwnerL(aRequest);
            aUserAgent.Stop(KErrNone);
			}
		else
			{
            // Broken ACK. No need to wait any more.
            delete aRequest;
            aUserAgent.Stop(KErrTimedOut);
			}
		}
	else
		{
		// Drop other messages than ACKs
        delete aRequest;
		}
	}

// -----------------------------------------------------------------------------
// CInviteUAS_Accepted::SendCompleteL
// -----------------------------------------------------------------------------
//
void CInviteUAS_Accepted::SendCompleteL(CUserAgent& aUserAgent) const
	{
	CInviteUAS::Ptr(aUserAgent).StartTimerRetransmit2xxL();
	}

// -----------------------------------------------------------------------------
// CInviteUAS_Accepted::SendFailedL
// Sending 2xx failed. Response can't be sent to any other address, so stop.
// -----------------------------------------------------------------------------
//
void CInviteUAS_Accepted::SendFailedL(CUserAgent& aUserAgent,
									  TInt aError) const
	{    
	aUserAgent.Stop(aError);
	}

// -----------------------------------------------------------------------------
// CInviteUAS_Accepted::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CInviteUAS_Accepted::TimerExpiredL(CUserAgent& aUserAgent,
										TTimerId aTimerId,
										TAny* aTimerParam) const
	{
    __SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);

	HandleTimerExpirationL(aUserAgent, aTimerId, aTimerParam);
	}

// -----------------------------------------------------------------------------
// CInviteUAS_Accepted::CancelL
// -----------------------------------------------------------------------------
//
TBool CInviteUAS_Accepted::CancelL(CUserAgent& aUserAgent) const
	{
	CInviteUAS::Ptr(aUserAgent).InformObserverAboutCancel();
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CInviteUAS_Rejected::CInviteUAS_Rejected
// -----------------------------------------------------------------------------
//
CInviteUAS_Rejected::CInviteUAS_Rejected()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAS_Rejected::~CInviteUAS_Rejected
// -----------------------------------------------------------------------------
//
CInviteUAS_Rejected::~CInviteUAS_Rejected()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAS_Rejected::TransactionEndsL
// -----------------------------------------------------------------------------
//
void CInviteUAS_Rejected::TransactionEndsL(CUserAgent& aUserAgent,
										   TInt aReason) const
	{
	aUserAgent.Stop(aReason);
	}

// -----------------------------------------------------------------------------
// CInviteUAS_Rejected::SendResponseL
// Final response has already been sent
// -----------------------------------------------------------------------------
//
void
CInviteUAS_Rejected::SendResponseL(CUserAgent& /*aUserAgent*/,
								   CSIPResponse* aResp,
								   const TSIPTransportParams& /*aParams*/) const
	{
    __SIP_ASSERT_LEAVE(aResp, KErrArgument); 
	
	User::Leave(KErrSIPInvalidTransactionState);
	}
