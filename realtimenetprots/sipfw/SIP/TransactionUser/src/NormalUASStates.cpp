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
// Name          : NormalUASStates.cpp
// Part of       : TransactionUser
// Version       : SIP/5.0
//



#include "siperr.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "SipAssert.h"

#include "NormalUAS.h"
#include "NormalUASStates.h"
#include "SIPMessageUtility.h"


// -----------------------------------------------------------------------------
// CNormalUAS_Start::CNormalUAS_Start
// -----------------------------------------------------------------------------
//
CNormalUAS_Start::CNormalUAS_Start()
	{
	}

// -----------------------------------------------------------------------------
// CNormalUAS_Start::~CNormalUAS_Start
// -----------------------------------------------------------------------------
//
CNormalUAS_Start::~CNormalUAS_Start()
	{
	}

// -----------------------------------------------------------------------------
// CNormalUAS_Start::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CNormalUAS_Start::SetNeighbourStates(CUserAgentState& aGetTxOwner,
										  CUserAgentState& aWaitRespFromApp,
										  CUserAgentState& aFinalRespSent)
	{
	iGetTxOwner = &aGetTxOwner;
	iWaitRespFromApp = &aWaitRespFromApp;
	iFinalRespSent = &aFinalRespSent;
	}

// -----------------------------------------------------------------------------
// CNormalUAS_Start::TransactionEndsL
// Happens if MSIPRequestRouter leaves.
// -----------------------------------------------------------------------------
//
void
CNormalUAS_Start::TransactionEndsL(CUserAgent& aUserAgent, TInt aReason) const
	{
	aUserAgent.Stop(aReason);
	}

// -----------------------------------------------------------------------------
// CNormalUAS_Start::ReceiveL
// -----------------------------------------------------------------------------
//
void
CNormalUAS_Start::ReceiveL(CUserAgent& aUserAgent, CSIPRequest* aRequest) const
	{
    __SIP_ASSERT_LEAVE(aRequest, KErrArgument);    

	CNormalUAS::Ptr(aUserAgent).InitialRequestReceivedL(aRequest,
													    *iGetTxOwner,
													    *iWaitRespFromApp,
													    *iFinalRespSent);
	}

// -----------------------------------------------------------------------------
// CNormalUAS_Start::SendResponseL
// Application shouldn't know about this transaction yet.
// -----------------------------------------------------------------------------
//
void
CNormalUAS_Start::SendResponseL(CUserAgent& /*aUserAgent*/,
								CSIPResponse* aResp,
								const TSIPTransportParams& /*aParams*/) const
	{
    __SIP_ASSERT_LEAVE(aResp, KErrArgument);

	User::Leave(KErrSIPInvalidTransactionState);
	}

// -----------------------------------------------------------------------------
// CNormalUAS_GetTxOwner::CNormalUAS_GetTxOwner
// -----------------------------------------------------------------------------
//
CNormalUAS_GetTxOwner::CNormalUAS_GetTxOwner()
	{
	}

// -----------------------------------------------------------------------------
// CNormalUAS_GetTxOwner::~CNormalUAS_GetTxOwner
// -----------------------------------------------------------------------------
//
CNormalUAS_GetTxOwner::~CNormalUAS_GetTxOwner()
	{
	}

// -----------------------------------------------------------------------------
// CNormalUAS_GetTxOwner::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void
CNormalUAS_GetTxOwner::SetNeighbourStates(CUserAgentState& aWaitRespFromApp,
							   			  CUserAgentState& aFinalRespSent)
	{
	iWaitRespFromApp = &aWaitRespFromApp;
	iFinalRespSent = &aFinalRespSent;
	}

// -----------------------------------------------------------------------------
// CNormalUAS_GetTxOwner::OwnerFoundL
// -----------------------------------------------------------------------------
//
void CNormalUAS_GetTxOwner::OwnerFoundL(CUserAgent& aUserAgent,
                                        MTransactionOwner* aOwner) const
	{
    __SIP_ASSERT_LEAVE(aOwner, KErrArgument);

	CNormalUAS::Ptr(aUserAgent).HandleOwnerFoundL(aOwner, *iWaitRespFromApp);
	}

// -----------------------------------------------------------------------------
// CNormalUAS_GetTxOwner::OwnerNotFoundL
// No application found, MSIPRequestRouter gave a SIP response to send.
// -----------------------------------------------------------------------------
//
void CNormalUAS_GetTxOwner::OwnerNotFoundL(CUserAgent& aUserAgent,
										   CSIPResponse* aResp) const
	{
    __SIP_ASSERT_LEAVE(aResp, KErrArgument);

	CNormalUAS::Ptr(aUserAgent).HandleOwnerNotFoundL(aResp, *iFinalRespSent);
	}

// -----------------------------------------------------------------------------
// CNormalUAS_GetTxOwner::RequestRouterErrorL
// -----------------------------------------------------------------------------
//
void CNormalUAS_GetTxOwner::RequestRouterErrorL(CUserAgent& aUserAgent) const
	{
	static_cast<CUserAgentServer*>(&aUserAgent)->
        HandleRequestRouterErrorL(*iFinalRespSent);
	}

// -----------------------------------------------------------------------------
// CNormalUAS_GetTxOwner::SendResponseL
// Application shouldn't know about this transaction yet.
// -----------------------------------------------------------------------------
//
void CNormalUAS_GetTxOwner::SendResponseL(CUserAgent& /*aUserAgent*/,
	CSIPResponse* aResp,
	const TSIPTransportParams& /*aParams*/) const
	{
    __SIP_ASSERT_LEAVE(aResp, KErrArgument);

	User::Leave(KErrSIPInvalidTransactionState);
	}

// -----------------------------------------------------------------------------
// CNormalUAS_GetTxOwner::TransactionEndsL
// -----------------------------------------------------------------------------
//
void CNormalUAS_GetTxOwner::TransactionEndsL(CUserAgent& aUserAgent,
											 TInt aReason) const
	{
	aUserAgent.Stop(aReason);
	}

// -----------------------------------------------------------------------------
// CNormalUAS_GetTxOwner::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CNormalUAS_GetTxOwner::TimerExpiredL(CUserAgent& aUserAgent,
									      TTimerId aTimerId,
										  TAny* aTimerParam) const
	{
    __SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);    

	HandleTimerExpirationL(aUserAgent, aTimerId, aTimerParam);
	}

// -----------------------------------------------------------------------------
// CNormalUAS_WaitRespFromApp::CNormalUAS_WaitRespFromApp
// -----------------------------------------------------------------------------
//
CNormalUAS_WaitRespFromApp::CNormalUAS_WaitRespFromApp()
	{
	}

// -----------------------------------------------------------------------------
// CNormalUAS_WaitRespFromApp::~CNormalUAS_WaitRespFromApp
// -----------------------------------------------------------------------------
//
CNormalUAS_WaitRespFromApp::~CNormalUAS_WaitRespFromApp()
	{
	}

// -----------------------------------------------------------------------------
// CNormalUAS_WaitRespFromApp::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void
CNormalUAS_WaitRespFromApp::SetNeighbourStates(CUserAgentState& aFinalRespSent)
	{
	iFinalRespSent = &aFinalRespSent;
	}

// -----------------------------------------------------------------------------
// CNormalUAS_WaitRespFromApp::SendResponseL
// -----------------------------------------------------------------------------
//
void CNormalUAS_WaitRespFromApp::SendResponseL(CUserAgent& aUserAgent,
								   CSIPResponse* aResp,
								   const TSIPTransportParams& aParams) const
	{
    __SIP_ASSERT_LEAVE(aResp, KErrArgument);

	CNormalUAS& ua = CNormalUAS::Ptr(aUserAgent);
	ua.HandleSendResponseL(aResp, aParams);

	//Enter next state before sending response, in case transaction terminates
	//in SendResponseToTransactionL, UAS is already in the correct state.
    if (CSIPMessageUtility::IsFinalResponse(*aResp))
		{
		aUserAgent.ChangeState(*iFinalRespSent);
		}

	ua.SendResponseToTransactionL(aResp);
	}

// -----------------------------------------------------------------------------
// CNormalUAS_WaitRespFromApp::TransactionEndsL
// -----------------------------------------------------------------------------
//
void CNormalUAS_WaitRespFromApp::TransactionEndsL(CUserAgent& aUserAgent,
											      TInt aReason) const
	{
	aUserAgent.Stop(aReason);
	}

// -----------------------------------------------------------------------------
// CNormalUAS_FinalRespSent::CNormalUAS_FinalRespSent
// -----------------------------------------------------------------------------
//
CNormalUAS_FinalRespSent::CNormalUAS_FinalRespSent()
	{
	}

// -----------------------------------------------------------------------------
// CNormalUAS_FinalRespSent::~CNormalUAS_FinalRespSent
// -----------------------------------------------------------------------------
//
CNormalUAS_FinalRespSent::~CNormalUAS_FinalRespSent()
	{
	}

// -----------------------------------------------------------------------------
// CNormalUAS_FinalRespSent::TransactionEndsL
// -----------------------------------------------------------------------------
//
void CNormalUAS_FinalRespSent::TransactionEndsL(CUserAgent& aUserAgent,
											    TInt aReason) const
	{    
	aUserAgent.Stop(aReason);
	}

// -----------------------------------------------------------------------------
// CNormalUAS_FinalRespSent::SendResponseL
// Application tries to send another final response, after already sending one.
// -----------------------------------------------------------------------------
//
void CNormalUAS_FinalRespSent::SendResponseL(CUserAgent& /*aUserAgent*/,
								 CSIPResponse* aResp,
								 const TSIPTransportParams& /*aParams*/) const
	{
    __SIP_ASSERT_LEAVE(aResp, KErrArgument);

	User::Leave(KErrSIPInvalidTransactionState);
	}
