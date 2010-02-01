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
// Name          : NormalUACStates.cpp
// Part of       : TransactionUser
// Version       : SIP/5.0
//



#include "siperr.h"
#include "SipAssert.h"
#include "sipresponse.h"
#include "sipaddress.h"
#include "sipuri.h"
#include "siphostport.h"

#include "NormalUAC.h"
#include "NormalUACStates.h"


// -----------------------------------------------------------------------------
// CNormalUAC_Start::CNormalUAC_Start
// -----------------------------------------------------------------------------
//
CNormalUAC_Start::CNormalUAC_Start()
	{
	}

// -----------------------------------------------------------------------------
// CNormalUAC_Start::~CNormalUAC_Start
// -----------------------------------------------------------------------------
//
CNormalUAC_Start::~CNormalUAC_Start()
	{
	}

// -----------------------------------------------------------------------------
// CNormalUAC_Start::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CNormalUAC_Start::SetNeighbourStates(CUserAgentState& aResolveAddress)
	{    
	iResolveAddress = &aResolveAddress;
	}

// -----------------------------------------------------------------------------
// CNormalUAC_Start::SendRequestL
// -----------------------------------------------------------------------------
//
void CNormalUAC_Start::SendRequestL(CUserAgent& aUserAgent,
									CSIPRequest* aReq,
									TRegistrationId aRegisterId,
									const CURIContainer& aRemoteTarget) const
	{
    __SIP_ASSERT_LEAVE(aReq, KErrArgument);

	CNormalUAC::Ptr(aUserAgent).HandleSendRequestL(aReq,
                                                   aRegisterId,
                                                   aRemoteTarget,
                                                   *iResolveAddress);
	}

// -----------------------------------------------------------------------------
// CNormalUAC_ResolveAddress::CNormalUAC_ResolveAddress
// -----------------------------------------------------------------------------
//
CNormalUAC_ResolveAddress::CNormalUAC_ResolveAddress()
	{
	}

// -----------------------------------------------------------------------------
// CNormalUAC_ResolveAddress::~CNormalUAC_ResolveAddress
// -----------------------------------------------------------------------------
//
CNormalUAC_ResolveAddress::~CNormalUAC_ResolveAddress()
	{
	}

// -----------------------------------------------------------------------------
// CNormalUAC_ResolveAddress::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void
CNormalUAC_ResolveAddress::SetNeighbourStates(CUserAgentState& aWaitResponse)
	{    
	iWaitResponse = &aWaitResponse;
	}

// -----------------------------------------------------------------------------
// CNormalUAC_ResolveAddress::IsResolving
// -----------------------------------------------------------------------------
//
TBool CNormalUAC_ResolveAddress::IsResolving() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CNormalUAC_ResolveAddress::AddressResolvedL
// -----------------------------------------------------------------------------
//
void CNormalUAC_ResolveAddress::AddressResolvedL(CUserAgent& aUserAgent) const
	{	
	CNormalUAC::Ptr(aUserAgent).RemoteAddressResolvedL(*iWaitResponse);
	}

// -----------------------------------------------------------------------------
// CNormalUAC_ResolveAddress::ResolvingFailed
// -----------------------------------------------------------------------------
//
void CNormalUAC_ResolveAddress::ResolvingFailed(CUserAgent& aUserAgent) const
	{
	CNormalUAC::Ptr(aUserAgent).HandleResolvingFailure(*this, *iWaitResponse);
	}

// -----------------------------------------------------------------------------
// CNormalUAC_WaitResponse::CNormalUAC_WaitResponse
// -----------------------------------------------------------------------------
//
CNormalUAC_WaitResponse::CNormalUAC_WaitResponse()
	{
	}

// -----------------------------------------------------------------------------
// CNormalUAC_WaitResponse::~CNormalUAC_WaitResponse
// -----------------------------------------------------------------------------
//
CNormalUAC_WaitResponse::~CNormalUAC_WaitResponse()
	{
	}

// -----------------------------------------------------------------------------
// CNormalUAC_WaitResponse::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CNormalUAC_WaitResponse::SetNeighbourStates(
	CUserAgentState& aResolveAddress,
	CUserAgentState& aWaitAuthentication)
	{
	iResolveAddress = &aResolveAddress;
	iWaitAuthentication = &aWaitAuthentication;
	}

// -----------------------------------------------------------------------------
// CNormalUAC_WaitResponse::CanContinueWithoutOutgoingMsg
// -----------------------------------------------------------------------------
//
TBool CNormalUAC_WaitResponse::CanContinueWithoutOutgoingMsg(
	TBool aFinalRespPassed) const
	{
	return aFinalRespPassed;
	}

// -----------------------------------------------------------------------------
// CNormalUAC_WaitResponse::ReceiveL
// -----------------------------------------------------------------------------
//
void CNormalUAC_WaitResponse::ReceiveL(CUserAgent& aUserAgent,
	                                   CSIPResponse* aResponse) const
	{
    __SIP_ASSERT_LEAVE(aResponse, KErrArgument);    

	CNormalUAC& ua = CNormalUAC::Ptr(aUserAgent);
	if (ua.PassResponseToSIPSecL(*aResponse))
		{
		ua.StoreResponse(aResponse);
		aUserAgent.ChangeState(*iWaitAuthentication);
		}
	else
		{
		ua.ReceiveResponseL(aResponse, *iResolveAddress, *this, *this, *this);
		}
	}

// -----------------------------------------------------------------------------
// CNormalUAC_WaitResponse::TransactionEndsL
// -----------------------------------------------------------------------------
//
void CNormalUAC_WaitResponse::TransactionEndsL(CUserAgent& aUserAgent,
											   TInt aReason) const
	{
	CNormalUAC& ua = CNormalUAC::Ptr(aUserAgent);
	if (ua.FinalRespPassed())
		{
		// MTransactionOwner got final response, so it sees the transaction has
		//succeeded, even if aReason is an error now.		
		aUserAgent.Stop(KErrNone);
		}
	else
		{
		ua.TransactionEndsWithoutFinalResponseL(aReason,
											    *iResolveAddress,
											    *this);
		}
	}

// -----------------------------------------------------------------------------
// CNormalUAC_WaitAuthentication::CNormalUAC_WaitAuthentication
// -----------------------------------------------------------------------------
//
CNormalUAC_WaitAuthentication::CNormalUAC_WaitAuthentication()
	{
	}

// -----------------------------------------------------------------------------
// CNormalUAC_WaitAuthentication::~CNormalUAC_WaitAuthentication
// -----------------------------------------------------------------------------
//
CNormalUAC_WaitAuthentication::~CNormalUAC_WaitAuthentication()
	{
	}

// -----------------------------------------------------------------------------
// CNormalUAC_WaitAuthentication::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CNormalUAC_WaitAuthentication::SetNeighbourStates(
	CUserAgentState& aResolveAddress,
	CUserAgentState& aWaitResponse)
	{
	iResolveAddress = &aResolveAddress;
	iWaitResponse = &aWaitResponse;
	}

// -----------------------------------------------------------------------------
// CNormalUAC_WaitAuthentication::SIPSecCacheUpdatedL
// -----------------------------------------------------------------------------
//
void CNormalUAC_WaitAuthentication::SIPSecCacheUpdatedL(CUserAgent& aUserAgent,
												   	    TBool aSuccess) const
	{
	CNormalUAC::Ptr(aUserAgent).SIPSecCacheUpdatedL(aSuccess,
												    *iResolveAddress,
                                                    *iWaitResponse,
                                                    *iWaitResponse,
                                                    *iWaitResponse);
	}

// -----------------------------------------------------------------------------
// CNormalUAC_WaitAuthentication::TransactionEndsL
// -----------------------------------------------------------------------------
//
void CNormalUAC_WaitAuthentication::TransactionEndsL(CUserAgent& aUserAgent,
													 TInt /*aReason*/) const
	{
	aUserAgent.RequestDeletionOfTransactionL();    
    CNormalUAC::Ptr(aUserAgent).IgnoreResponseRetransmissionsL();
	}
