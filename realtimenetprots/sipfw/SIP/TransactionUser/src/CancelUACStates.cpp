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
// Name          : CancelUACStates.cpp
// Part of       : TransactionUser
// Version       : SIP/5.0
//



#include "SipAssert.h"

#include "CancelUAC.h"
#include "CancelUACStates.h"


// -----------------------------------------------------------------------------
// CCancelUAC_Start::CCancelUAC_Start
// -----------------------------------------------------------------------------
//
CCancelUAC_Start::CCancelUAC_Start()
	{
	}

// -----------------------------------------------------------------------------
// CCancelUAC_Start::~CCancelUAC_Start
// -----------------------------------------------------------------------------
//
CCancelUAC_Start::~CCancelUAC_Start()
	{
	}

// -----------------------------------------------------------------------------
// CCancelUAC_Start::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CCancelUAC_Start::SetNeighbourStates(CUserAgentState& aWaitResponse)
	{
	iWaitResponse = &aWaitResponse;
	}

// -----------------------------------------------------------------------------
// CCancelUAC_Start::SendCancelL
// -----------------------------------------------------------------------------
//
void CCancelUAC_Start::SendCancelL(CUserAgent& aUserAgent,
								   TTransactionId aInviteTaId) const
	{
	CCancelUAC::Ptr(aUserAgent).CreateAndSendCancelL(aInviteTaId);
	aUserAgent.ChangeState(*iWaitResponse);
	}
