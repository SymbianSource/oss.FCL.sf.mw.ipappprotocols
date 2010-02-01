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
// Name          : InviteUACStates.cpp
// Part of       : TransactionUser
// Version       : SIP/5.0
//



#include "siperr.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "SipAssert.h"

#include "InviteUAC.h"
#include "InviteUACStates.h"


// -----------------------------------------------------------------------------
// CInviteUAC_Start::CInviteUAC_Start
// -----------------------------------------------------------------------------
//
CInviteUAC_Start::CInviteUAC_Start()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAC_Start::~CInviteUAC_Start
// -----------------------------------------------------------------------------
//
CInviteUAC_Start::~CInviteUAC_Start()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAC_Start::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CInviteUAC_Start::SetNeighbourStates(CUserAgentState& aResolveAddress)
	{    
	iResolveAddress = &aResolveAddress;
	}

// -----------------------------------------------------------------------------
// CInviteUAC_Start::SendRequestL
// -----------------------------------------------------------------------------
//
void CInviteUAC_Start::SendRequestL(CUserAgent& aUserAgent,
									CSIPRequest* aReq,
									TRegistrationId aRegisterId,
									const CURIContainer& aRemoteTarget) const
    {
    __SIP_ASSERT_LEAVE(aReq, KErrArgument);    

	CInviteUAC::Ptr(aUserAgent).HandleSendRequestL(aReq,
												   aRegisterId,
												   aRemoteTarget,
                                                   *iResolveAddress);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_Start::SendAckL
// Can't send ACK before a 2xx has been received
// -----------------------------------------------------------------------------
//
void CInviteUAC_Start::SendAckL(CUserAgent& /*aUserAgent*/,						
							    CSIPRequest* aAck,
							    MTransactionOwner& /*aObserver*/,
							    const TSIPTransportParams& /*aParams*/,
							    TRegistrationId /*aRegisterId*/,
							    const CURIContainer& /*aRemoteTarget*/,
							    TBool /*aDeleteRequest*/,
							    RStringF /*aBranch*/) const
	{
    __SIP_ASSERT_LEAVE(aAck, KErrArgument);

	User::Leave(KErrSIPInvalidTransactionState);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_ResolveAddress::CInviteUAC_ResolveAddress
// -----------------------------------------------------------------------------
//
CInviteUAC_ResolveAddress::CInviteUAC_ResolveAddress()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAC_ResolveAddress::~CInviteUAC_ResolveAddress
// -----------------------------------------------------------------------------
//
CInviteUAC_ResolveAddress::~CInviteUAC_ResolveAddress()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAC_ResolveAddress::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void
CInviteUAC_ResolveAddress::SetNeighbourStates(CUserAgentState& aWaitResponse)
	{
	iWaitResponse = &aWaitResponse;
	}

// -----------------------------------------------------------------------------
// CInviteUAC_ResolveAddress::IsResolving
// -----------------------------------------------------------------------------
//
TBool CInviteUAC_ResolveAddress::IsResolving() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CInviteUAC_ResolveAddress::AddressResolvedL
// -----------------------------------------------------------------------------
//
void CInviteUAC_ResolveAddress::AddressResolvedL(CUserAgent& aUserAgent) const
	{
	CInviteUAC::Ptr(aUserAgent).RemoteAddressResolvedL(*iWaitResponse);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_ResolveAddress::ResolvingFailed
// -----------------------------------------------------------------------------
//
void CInviteUAC_ResolveAddress::ResolvingFailed(CUserAgent& aUserAgent) const
	{
	CInviteUAC::Ptr(aUserAgent).HandleResolvingFailure(*this, *iWaitResponse);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_ResolveAddress::SendAckL
// Can't send ACK before a 2xx has been received
// -----------------------------------------------------------------------------
//
void CInviteUAC_ResolveAddress::SendAckL(CUserAgent& /*aUserAgent*/,
										 CSIPRequest* aAck,										 
										 MTransactionOwner& /*aObserver*/,
										 const TSIPTransportParams& /*aParams*/,
										 TRegistrationId /*aRegisterId*/,
										 const CURIContainer& /*aRemoteTarget*/,
										 TBool /*aDeleteRequest*/,
										 RStringF /*aBranch*/) const
	{
    __SIP_ASSERT_LEAVE(aAck, KErrArgument);
	
	User::Leave(KErrSIPInvalidTransactionState);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitResponse::CInviteUAC_WaitResponse
// -----------------------------------------------------------------------------
//
CInviteUAC_WaitResponse::CInviteUAC_WaitResponse()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitResponse::~CInviteUAC_WaitResponse
// -----------------------------------------------------------------------------
//
CInviteUAC_WaitResponse::~CInviteUAC_WaitResponse()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitResponse::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CInviteUAC_WaitResponse::SetNeighbourStates(
							 CUserAgentState& aWaitAckFromApp,
							 CUserAgentState& aWaitTransactionToEnd,
							 CUserAgentState& aResolveAddress,
							 CUserAgentState& aWaitAuthentication)
	{    	
	iWaitAckFromApp = &aWaitAckFromApp;
	iWaitTransactionToEnd = &aWaitTransactionToEnd;
	iResolveAddress = &aResolveAddress;
	iWaitAuthentication = &aWaitAuthentication;
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitResponse::ReceiveL
// -----------------------------------------------------------------------------
//
void CInviteUAC_WaitResponse::ReceiveL(CUserAgent& aUserAgent,
	                                   CSIPResponse* aResponse) const
	{
    __SIP_ASSERT_LEAVE(aResponse, KErrArgument);

	CInviteUAC& ua = CInviteUAC::Ptr(aUserAgent);

	//If a final response has been passed to TransactionOwner, UA should be in
	//CInviteUAC_WaitAckFromApp state
    __SIP_ASSERT_LEAVE(!ua.FinalRespPassed(), KErrSIPInvalidTransactionState);
	
    TBool wait(EFalse);
    TRAPD(err,wait = ua.PassResponseToSIPSecL(*aResponse))
    
    if(err )
        {
        if(aResponse->IsErrorResponse())
            
            ua.SetSipSecError(err);
        else
            User::Leave(err); 
        }
    
	if (wait)
		{
		//Wait for SIPSec to handle response
		ua.StoreResponse(aResponse);
		aUserAgent.ChangeState(*iWaitAuthentication);
		}
	else
		{
		if (aResponse->ResponseCode() >= 300)
			{			
			ua.StartSIPSecTimerL();
			ua.StoreResponse(aResponse);
			aUserAgent.ChangeState(*iWaitAuthentication);
			}
		else
			{
			ua.ReceiveResponseL(aResponse,
								*iResolveAddress,
								*this,
								*iWaitAckFromApp,
								*iWaitTransactionToEnd);
			}
		}
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitResponse::TransactionEndsL
// -----------------------------------------------------------------------------
//
void CInviteUAC_WaitResponse::TransactionEndsL(CUserAgent& aUserAgent,
											   TInt aReason) const
	{
	CInviteUAC::Ptr(aUserAgent).TransactionEndsWithoutFinalResponseL(aReason,
											    *iResolveAddress,
											    *this);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitResponse::SendAckL
// Can't send ACK before a 2xx has been received
// -----------------------------------------------------------------------------
//
void CInviteUAC_WaitResponse::SendAckL(CUserAgent& /*aUserAgent*/,
									   CSIPRequest* aAck,
									   MTransactionOwner& /*aObserver*/,
									   const TSIPTransportParams& /*aParams*/,
									   TRegistrationId /*aRegisterId*/,
									   const CURIContainer& /*aRemoteTarget*/,
									   TBool /*aDeleteRequest*/,
									   RStringF /*aBranch*/) const
	{
    __SIP_ASSERT_LEAVE(aAck, KErrArgument);

	User::Leave(KErrSIPInvalidTransactionState);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitAckFromApp::CInviteUAC_WaitAckFromApp
// -----------------------------------------------------------------------------
//
CInviteUAC_WaitAckFromApp::CInviteUAC_WaitAckFromApp()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitAckFromApp::~CInviteUAC_WaitAckFromApp
// -----------------------------------------------------------------------------
//
CInviteUAC_WaitAckFromApp::~CInviteUAC_WaitAckFromApp()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitAckFromApp::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CInviteUAC_WaitAckFromApp::SetNeighbourStates(
	CUserAgentState& aResolveAckAddress)
	{    
	iResolveAckAddress = &aResolveAckAddress;
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitAckFromApp::SendAckL
// ACK's headers are not updated to CTransactionStore, as an ACK to a 2xx has a
// different branch than the original INVITE, and further 2xx responses couldn't
// be routed to this UAC anymore if the new branch is put to CTransactionStore.
// -----------------------------------------------------------------------------
//
void CInviteUAC_WaitAckFromApp::SendAckL(CUserAgent& aUserAgent,
										 CSIPRequest* aAck,
										 MTransactionOwner& aObserver,
										 const TSIPTransportParams& aParams,
										 TRegistrationId aRegisterId,
										 const CURIContainer& aRemoteTarget,
										 TBool aDeleteRequest,
										 RStringF aBranch) const
	{
    __SIP_ASSERT_LEAVE(aAck, KErrArgument);    

	CInviteUAC::Ptr(aUserAgent).SendFirstAckL(aAck,
											  aObserver,
											  aParams,
											  aRegisterId,
											  aRemoteTarget,
											  aDeleteRequest,
											  aBranch);	
	aUserAgent.ChangeState(*iResolveAckAddress);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitAckFromApp::ReceiveL
// As incoming responses are routed using Via's branch parameter, the 2xx
// responses will be received by the same UAC regardless of the To-tag value.
// -----------------------------------------------------------------------------
//
void CInviteUAC_WaitAckFromApp::ReceiveL(CUserAgent& aUserAgent,
	                                     CSIPResponse* aResponse) const
	{
    __SIP_ASSERT_LEAVE(aResponse, KErrArgument);    
	
	CInviteUAC::Ptr(aUserAgent).HandleAdditional2xxL(aResponse);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitAckFromApp::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CInviteUAC_WaitAckFromApp::TimerExpiredL(CUserAgent& aUserAgent,
										      TTimerId aTimerId,
										      TAny* aTimerParam) const
	{
    __SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);    

	HandleTimerExpirationL(aUserAgent, aTimerId, aTimerParam);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_ResolveAckAddress::CInviteUAC_ResolveAckAddress
// -----------------------------------------------------------------------------
//
CInviteUAC_ResolveAckAddress::CInviteUAC_ResolveAckAddress()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAC_ResolveAckAddress::~CInviteUAC_ResolveAckAddress
// -----------------------------------------------------------------------------
//
CInviteUAC_ResolveAckAddress::~CInviteUAC_ResolveAckAddress()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAC_ResolveAckAddress::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void
CInviteUAC_ResolveAckAddress::SetNeighbourStates(CUserAgentState& aSendingAck)
	{
	iSendingAck = &aSendingAck;
	}

// -----------------------------------------------------------------------------
// CInviteUAC_ResolveAckAddress::IsResolving
// -----------------------------------------------------------------------------
//
TBool CInviteUAC_ResolveAckAddress::IsResolving() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CInviteUAC_ResolveAckAddress::AddressResolvedL
// Remote address for ACK resolved, send the ACK from send buffer
// -----------------------------------------------------------------------------
//
void
CInviteUAC_ResolveAckAddress::AddressResolvedL(CUserAgent& aUserAgent) const
	{
	CInviteUAC::Ptr(aUserAgent).AckAddressResolvedL();
	aUserAgent.ChangeState(*iSendingAck);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_ResolveAckAddress::ResolvingFailed
// -----------------------------------------------------------------------------
//
void CInviteUAC_ResolveAckAddress::ResolvingFailed(CUserAgent& aUserAgent) const
	{
	CInviteUAC::Ptr(aUserAgent).HandleResolvingFailure(*this, *iSendingAck);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_ResolveAckAddress::SendAckL
// -----------------------------------------------------------------------------
//
void CInviteUAC_ResolveAckAddress::SendAckL(CUserAgent& aUserAgent,
										    CSIPRequest* aAck,
										    MTransactionOwner& aObserver,
										    const TSIPTransportParams& aParams,
										    TRegistrationId aRegisterId,
										    const CURIContainer& aRemoteTarget,
										    TBool aDeleteRequest,
										    RStringF /*aBranch*/) const
	{
    __SIP_ASSERT_LEAVE(aAck, KErrArgument);    

	CInviteUAC::Ptr(aUserAgent).SendAdditionalAckL(aAck,
												   aObserver,
												   aParams,
												   aRegisterId,
												   aRemoteTarget,
												   aDeleteRequest);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_ResolveAckAddress::ReceiveL
// Incoming responses are routed by Via branch, so 2xx responses are received
// by the same UAC regardless of the To-tag value.
// -----------------------------------------------------------------------------
//
void CInviteUAC_ResolveAckAddress::ReceiveL(CUserAgent& aUserAgent,
	                                        CSIPResponse* aResponse) const
	{
    __SIP_ASSERT_LEAVE(aResponse, KErrArgument);

	CInviteUAC::Ptr(aUserAgent).HandleAdditional2xxL(aResponse);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_ResolveAckAddress::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CInviteUAC_ResolveAckAddress::TimerExpiredL(CUserAgent& aUserAgent,
											     TTimerId aTimerId,
											     TAny* aTimerParam) const
	{
    __SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);    

	HandleTimerExpirationL(aUserAgent, aTimerId, aTimerParam);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_SendingAck::CInviteUAC_SendingAck
// -----------------------------------------------------------------------------
//
CInviteUAC_SendingAck::CInviteUAC_SendingAck()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAC_SendingAck::~CInviteUAC_SendingAck
// -----------------------------------------------------------------------------
//
CInviteUAC_SendingAck::~CInviteUAC_SendingAck()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAC_SendingAck::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void
CInviteUAC_SendingAck::SetNeighbourStates(CUserAgentState& aResolveAckAddress)
	{
	iResolveAckAddress = &aResolveAckAddress;
	}

// -----------------------------------------------------------------------------
// CInviteUAC_SendingAck::SendCompleteL
// No action, UAC ends when iTimerWait2xxRetransmissions expires
// -----------------------------------------------------------------------------
//
void CInviteUAC_SendingAck::SendCompleteL(CUserAgent& /*aUserAgent*/) const
	{    
	}

// -----------------------------------------------------------------------------
// CInviteUAC_SendingAck::SendFailedL
// -----------------------------------------------------------------------------
//
void CInviteUAC_SendingAck::SendFailedL(CUserAgent& aUserAgent,
										TInt aError) const
	{
	if (!CInviteUAC::Ptr(aUserAgent).TryNextAddressL(aError,
													 *iResolveAckAddress,
													 *this))
		{
		aUserAgent.Stop(aError);
		}
	}

// -----------------------------------------------------------------------------
// CInviteUAC_SendingAck::SendAckL
// -----------------------------------------------------------------------------
//
void CInviteUAC_SendingAck::SendAckL(CUserAgent& aUserAgent,
								     CSIPRequest* aAck,
								     MTransactionOwner& aObserver,
								     const TSIPTransportParams& aParams,
								     TRegistrationId aRegisterId,
								     const CURIContainer& aRemoteTarget,
								     TBool aDeleteRequest,
								     RStringF /*aBranch*/) const
	{
    __SIP_ASSERT_LEAVE(aAck, KErrArgument);

	CInviteUAC::Ptr(aUserAgent).SendAdditionalAckL(aAck,
												   aObserver,
												   aParams,
												   aRegisterId,
												   aRemoteTarget,
												   aDeleteRequest);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_SendingAck::ReceiveL
// -----------------------------------------------------------------------------
//
void CInviteUAC_SendingAck::ReceiveL(CUserAgent& aUserAgent,
	                                 CSIPResponse* aResponse) const
	{
    __SIP_ASSERT_LEAVE(aResponse, KErrArgument);    

	CInviteUAC::Ptr(aUserAgent).HandleAdditional2xxL(aResponse);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_SendingAck::IcmpErrorL
// InviteUAC handles IcmpErrorL only in states where it has sent a request
// -----------------------------------------------------------------------------
//
void CInviteUAC_SendingAck::IcmpErrorL(CUserAgent& aUserAgent,
							   const TInetAddr& aAddress,
							   CSipConnectionMgr::TICMPError /*aError*/) const
	{    
	CInviteUAC::Ptr(aUserAgent).HandleIcmpErrorL(aAddress,
										         *iResolveAckAddress,
										         *this);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_SendingAck::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CInviteUAC_SendingAck::TimerExpiredL(CUserAgent& aUserAgent,
									      TTimerId aTimerId,
									      TAny* aTimerParam) const
	{
    __SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);    

	HandleTimerExpirationL(aUserAgent, aTimerId, aTimerParam);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitTransactionToEnd::CInviteUAC_WaitTransactionToEnd
// -----------------------------------------------------------------------------
//
CInviteUAC_WaitTransactionToEnd::CInviteUAC_WaitTransactionToEnd()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitTransactionToEnd::~CInviteUAC_WaitTransactionToEnd
// -----------------------------------------------------------------------------
//
CInviteUAC_WaitTransactionToEnd::~CInviteUAC_WaitTransactionToEnd()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitTransactionToEnd::CanContinueWithoutOutgoingMsg
// -----------------------------------------------------------------------------
//
TBool CInviteUAC_WaitTransactionToEnd::CanContinueWithoutOutgoingMsg(
											TBool /*aFinalRespPassed*/) const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitTransactionToEnd::TransactionEndsL
// -----------------------------------------------------------------------------
//
void CInviteUAC_WaitTransactionToEnd::TransactionEndsL(CUserAgent& aUserAgent,
													   TInt aReason) const
	{
	aUserAgent.Stop(aReason);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitTransactionToEnd::SendAckL
// Upper layer can't send ACK, as a 3xx-6xx response was received.
// -----------------------------------------------------------------------------
//
void CInviteUAC_WaitTransactionToEnd::SendAckL(CUserAgent& /*aUserAgent*/,
									CSIPRequest* aAck,
									MTransactionOwner& /*aObserver*/,
									const TSIPTransportParams& /*aParams*/,
									TRegistrationId /*aRegisterId*/,
									const CURIContainer& /*aRemoteTarget*/,
									TBool /*aDeleteRequest*/,
									RStringF /*aBranch*/) const
	{
    __SIP_ASSERT_LEAVE(aAck, KErrArgument);

	User::Leave(KErrSIPInvalidTransactionState);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitAuthentication::CInviteUAC_WaitAuthentication
// -----------------------------------------------------------------------------
//
CInviteUAC_WaitAuthentication::CInviteUAC_WaitAuthentication()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitAuthentication::~CInviteUAC_WaitAuthentication
// -----------------------------------------------------------------------------
//
CInviteUAC_WaitAuthentication::~CInviteUAC_WaitAuthentication()
	{
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitAuthentication::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CInviteUAC_WaitAuthentication::SetNeighbourStates(
							CUserAgentState& aResolveAddress,
							CUserAgentState& aWaitResponse,
							CUserAgentState& aWaitAckFromApp,
							CUserAgentState& aWaitTransactionToEnd)
	{    
	iResolveAddress = &aResolveAddress;
	iWaitResponse = &aWaitResponse;
	iWaitAckFromApp = &aWaitAckFromApp;
	iWaitTransactionToEnd = &aWaitTransactionToEnd;
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitAuthentication::SIPSecCacheUpdatedL
// -----------------------------------------------------------------------------
//
void
CInviteUAC_WaitAuthentication::SIPSecCacheUpdatedL(CUserAgent& aUserAgent,
												   TBool aSuccess) const
	{
	CInviteUAC::Ptr(aUserAgent).SIPSecCacheUpdatedL(aSuccess,                          
			                                        *iResolveAddress,
			                                        *iWaitResponse,
									 				*iWaitAckFromApp,
									 				*iWaitTransactionToEnd);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitAuthentication::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CInviteUAC_WaitAuthentication::TimerExpiredL(CUserAgent& aUserAgent,
											      TTimerId aTimerId,
											      TAny* aTimerParam) const
	{
    __SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);    

	HandleTimerExpirationL(aUserAgent, aTimerId, aTimerParam);
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitAuthentication::TransactionEndsL
// -----------------------------------------------------------------------------
//
void CInviteUAC_WaitAuthentication::TransactionEndsL(CUserAgent& aUserAgent,
													 TInt /*aReason*/) const
	{
    aUserAgent.RequestDeletionOfTransactionL();
    CInviteUAC::Ptr(aUserAgent).IgnoreResponseRetransmissionsL();
	}

// -----------------------------------------------------------------------------
// CInviteUAC_WaitAuthentication::SendAckL
// 2xx hasn't been received, so application can't send ACK.
// -----------------------------------------------------------------------------
//
void CInviteUAC_WaitAuthentication::SendAckL(CUserAgent& /*aUserAgent*/,
									CSIPRequest* aAck,
									MTransactionOwner& /*aObserver*/,
									const TSIPTransportParams& /*aParams*/,
									TRegistrationId /*aRegisterId*/,
									const CURIContainer& /*aRemoteTarget*/,
									TBool /*aDeleteRequest*/,
									RStringF /*aBranch*/) const
	{
    __SIP_ASSERT_LEAVE(aAck, KErrArgument);

	User::Leave(KErrSIPInvalidTransactionState);
	}
