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
// Name          : UserAgentState.cpp
// Part of       : TransactionUser
// Version       : SIP/5.0
//



#include "SipAssert.h"

#include "CUserAgent.h"
#include "UserAgentState.h"
#include "UserAgentTimer.h"

// -----------------------------------------------------------------------------
// CUserAgentState::CUserAgentState
// -----------------------------------------------------------------------------
//
CUserAgentState::CUserAgentState()
	{
	}

// -----------------------------------------------------------------------------
// CUserAgentState::~CUserAgentState
// -----------------------------------------------------------------------------
//
CUserAgentState::~CUserAgentState()
	{
	}

// -----------------------------------------------------------------------------
// CUserAgentState::IsResolving
// -----------------------------------------------------------------------------
//
TBool CUserAgentState::IsResolving() const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CUserAgentState::CanContinueWithoutOutgoingMsg
// By default, UA can't continue without iOutgoingMsg.
// -----------------------------------------------------------------------------
//
TBool CUserAgentState::CanContinueWithoutOutgoingMsg(
	TBool /*aFinalRespPassed*/) const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CUserAgentState::ShouldUASStop
// -----------------------------------------------------------------------------
//
TBool CUserAgentState::ShouldUASStop() const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CUserAgentState::CancelL
// Only certain UAs can be canceled, and they have to be in a certain state.
// -----------------------------------------------------------------------------
//
TBool CUserAgentState::CancelL(CUserAgent& /*aUserAgent*/) const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CUserAgentState::SendRequestL
// -----------------------------------------------------------------------------
//
void CUserAgentState::SendRequestL(CUserAgent& /*aUserAgent*/,
								   CSIPRequest* aReq,
								   TRegistrationId /*aRegisterId*/,
								   const CURIContainer& /*aRemoteTarget*/) const
	{
    __SIP_ASSERT_LEAVE(aReq, KErrArgument);
    __SIP_ASSERT_LEAVE(EFalse, KErrGeneral);	
	}

// -----------------------------------------------------------------------------
// CUserAgentState::SendResponseL
// -----------------------------------------------------------------------------
//
void
CUserAgentState::SendResponseL(CUserAgent& /*aUserAgent*/,
							   CSIPResponse* aResp,
							   const TSIPTransportParams& /*aParams*/) const
	{
    __SIP_ASSERT_LEAVE(aResp, KErrArgument);
    __SIP_ASSERT_LEAVE(EFalse, KErrGeneral);
	}

// -----------------------------------------------------------------------------
// CUserAgentState::SendCancelL
// -----------------------------------------------------------------------------
//
void CUserAgentState::SendCancelL(CUserAgent& /*aUserAgent*/,
								  TTransactionId /*aInviteTaId*/) const
	{    
    __SIP_ASSERT_LEAVE(EFalse, KErrGeneral);
	}

// -----------------------------------------------------------------------------
// CUserAgentState::SendAckL
// -----------------------------------------------------------------------------
//
void CUserAgentState::SendAckL(CUserAgent& /*aUserAgent*/,
							   CSIPRequest* aAck,
							   MTransactionOwner& /*aObserver*/,
							   const TSIPTransportParams& /*aParams*/,
							   TRegistrationId /*aRegisterId*/,
							   const CURIContainer& /*aRemoteTarget*/,
							   TBool /*aDeleteRequest*/,
							   RStringF /*aBranch*/) const
	{
    __SIP_ASSERT_LEAVE(aAck, KErrArgument);
    __SIP_ASSERT_LEAVE(EFalse, KErrGeneral);
	}

// -----------------------------------------------------------------------------
// CUserAgentState::AddressResolvedL
// -----------------------------------------------------------------------------
//
void CUserAgentState::AddressResolvedL(CUserAgent& /*aUserAgent*/) const
	{    
    __SIP_ASSERT_LEAVE(EFalse, KErrGeneral);
	}

// -----------------------------------------------------------------------------
// CUserAgentState::ResolvingFailed
// -----------------------------------------------------------------------------
//
void CUserAgentState::ResolvingFailed(CUserAgent& /*aUserAgent*/) const
	{    
    __SIP_ASSERT_RETURN(EFalse, KErrGeneral);
	}

// -----------------------------------------------------------------------------
// CUserAgentState::TransactionEndsL
// -----------------------------------------------------------------------------
//
void CUserAgentState::TransactionEndsL(CUserAgent& /*aUserAgent*/,
									   TInt /*aReason*/) const
	{    
    __SIP_ASSERT_LEAVE(EFalse, KErrGeneral);
	}

// -----------------------------------------------------------------------------
// CUserAgentState::ReceiveL
// -----------------------------------------------------------------------------
//
void CUserAgentState::ReceiveL(CUserAgent& /*aUserAgent*/,							  
							   CSIPRequest* aRequest) const
	{
    __SIP_ASSERT_LEAVE(aRequest, KErrArgument);
    __SIP_ASSERT_LEAVE(EFalse, KErrGeneral);
	}

// -----------------------------------------------------------------------------
// CUserAgentState::ReceiveL
// -----------------------------------------------------------------------------
//
void CUserAgentState::ReceiveL(CUserAgent& /*aUserAgent*/,
							   CSIPResponse* aResponse) const
	{
    __SIP_ASSERT_LEAVE(aResponse, KErrArgument);
    __SIP_ASSERT_LEAVE(EFalse, KErrGeneral);
	}

// -----------------------------------------------------------------------------
// CUserAgentState::OwnerFoundL
// -----------------------------------------------------------------------------
//
void CUserAgentState::OwnerFoundL(CUserAgent& /*aUserAgent*/,
								  MTransactionOwner* aOwner) const
	{
    __SIP_ASSERT_LEAVE(aOwner, KErrArgument);
    __SIP_ASSERT_LEAVE(EFalse, KErrGeneral);
	}

// -----------------------------------------------------------------------------
// CUserAgentState::OwnerNotFoundL
// -----------------------------------------------------------------------------
//
void CUserAgentState::OwnerNotFoundL(CUserAgent& /*aUserAgent*/,
                                     CSIPResponse* aResp) const
	{
    __SIP_ASSERT_LEAVE(aResp, KErrArgument);
    __SIP_ASSERT_LEAVE(EFalse, KErrGeneral);
	}

// -----------------------------------------------------------------------------
// CUserAgentState::SendCompleteL
// -----------------------------------------------------------------------------
//
void CUserAgentState::SendCompleteL(CUserAgent& /*aUserAgent*/) const
	{    
    __SIP_ASSERT_LEAVE(EFalse, KErrGeneral);
	}

// -----------------------------------------------------------------------------
// CUserAgentState::SendFailedL
// -----------------------------------------------------------------------------
//
void CUserAgentState::SendFailedL(CUserAgent& /*aUserAgent*/,
                                  TInt /*aErrcode*/) const
	{    
    __SIP_ASSERT_LEAVE(EFalse, KErrGeneral);
	}

// -----------------------------------------------------------------------------
// CUserAgentState::SIPSecCacheUpdatedL
// SIPSec uses this callback also when UAC has canceled the pending operations.
// So the default implementation just ignores the event.
// -----------------------------------------------------------------------------
//
void CUserAgentState::SIPSecCacheUpdatedL(CUserAgent& /*aUserAgent*/,
										  TBool /*aSuccess*/) const
	{    
	}

// -----------------------------------------------------------------------------
// CUserAgentState::IcmpErrorL
// By default, UA ignores ICMP errors.
// -----------------------------------------------------------------------------
//
void
CUserAgentState::IcmpErrorL(CUserAgent& /*aUserAgent*/,
							const TInetAddr& /*aAddress*/,
							CSipConnectionMgr::TICMPError /*aError*/) const
	{    
	}

// -----------------------------------------------------------------------------
// CUserAgentState::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CUserAgentState::TimerExpiredL(CUserAgent& /*aUserAgent*/,
								    TTimerId /*aTimerId*/,
								    TAny* aTimerParam) const
	{
    __SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);
    __SIP_ASSERT_LEAVE(EFalse, KErrGeneral);
	}

// -----------------------------------------------------------------------------
// CUserAgentState::RequestRouterErrorL
// -----------------------------------------------------------------------------
//
void CUserAgentState::RequestRouterErrorL(CUserAgent& /*aUserAgent*/) const
    {    
    __SIP_ASSERT_LEAVE(EFalse, KErrGeneral);
    }

// -----------------------------------------------------------------------------
// CUserAgentState::HandleTimerExpirationL
// -----------------------------------------------------------------------------
//
void CUserAgentState::HandleTimerExpirationL(CUserAgent& aUserAgent,
											 TTimerId aTimerId,
											 TAny* aTimerParam) const
	{
    __SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);    

	CUserAgentTimer* timer = reinterpret_cast<CUserAgentTimer*>(aTimerParam);
	timer->ExpiredL(&aUserAgent, aTimerId);
	aUserAgent.DeleteTimer(*timer);
	}
