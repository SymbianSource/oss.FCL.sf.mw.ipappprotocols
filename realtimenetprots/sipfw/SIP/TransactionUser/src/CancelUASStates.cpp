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
// Name          : CancelUASStates.cpp
// Part of       : TransactionUser
// Version       : SIP/5.0
//



#include "siperr.h"
#include "siprequest.h"
#include "SipAssert.h"

#include "CancelUAS.h"
#include "CancelUASStates.h"


// -----------------------------------------------------------------------------
// CCancelUAS_Start::CCancelUAS_Start
// -----------------------------------------------------------------------------
//
CCancelUAS_Start::CCancelUAS_Start()
	{
	}

// -----------------------------------------------------------------------------
// CCancelUAS_Start::~CCancelUAS_Start
// -----------------------------------------------------------------------------
//
CCancelUAS_Start::~CCancelUAS_Start()
	{
	}

// -----------------------------------------------------------------------------
// CCancelUAS_Start::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CCancelUAS_Start::SetNeighbourStates(CUserAgentState& aFinalRespSent)
	{	
	iFinalRespSent = &aFinalRespSent;
	}

// -----------------------------------------------------------------------------
// CCancelUAS_Start::TransactionEndsL
// Happens if leave occurs while in this state. ConnectionMgr TRAPs the leave
// and call LeaveOccurred causing transaction to end.
// -----------------------------------------------------------------------------
//
void
CCancelUAS_Start::TransactionEndsL(CUserAgent& aUserAgent, TInt aReason) const
	{
	aUserAgent.Stop(aReason);
	}

// -----------------------------------------------------------------------------
// CCancelUAS_Start::ReceiveL
// -----------------------------------------------------------------------------
//
void
CCancelUAS_Start::ReceiveL(CUserAgent& aUserAgent, CSIPRequest* aRequest) const
	{
    __SIP_ASSERT_LEAVE(aRequest, KErrArgument);

	CCancelUAS::Ptr(aUserAgent).CancelReceivedL(aRequest, *iFinalRespSent);
	}

// -----------------------------------------------------------------------------
// CCancelUAS_FinalRespSent::CCancelUAS_FinalRespSent
// -----------------------------------------------------------------------------
//
CCancelUAS_FinalRespSent::CCancelUAS_FinalRespSent()
	{
	}

// -----------------------------------------------------------------------------
// CCancelUAS_FinalRespSent::~CCancelUAS_FinalRespSent
// -----------------------------------------------------------------------------
//
CCancelUAS_FinalRespSent::~CCancelUAS_FinalRespSent()
	{
	}

// -----------------------------------------------------------------------------
// CCancelUAS_FinalRespSent::TransactionEndsL
// -----------------------------------------------------------------------------
//
void CCancelUAS_FinalRespSent::TransactionEndsL(CUserAgent& aUserAgent,
											    TInt aReason) const
	{
	aUserAgent.Stop(aReason);
	}

// -----------------------------------------------------------------------------
// CCancelUAS_FinalRespSent::SendResponseL
// Application tries to send a response, but TU always sends the response to
// CANCEL.
// -----------------------------------------------------------------------------
//
void CCancelUAS_FinalRespSent::SendResponseL(CUserAgent& /*aUserAgent*/,
								 CSIPResponse* aResp,
								 const TSIPTransportParams& /*aParams*/) const
	{
	__SIP_ASSERT_LEAVE(aResp, KErrArgument);

	User::Leave(KErrSIPInvalidTransactionState);
	}
