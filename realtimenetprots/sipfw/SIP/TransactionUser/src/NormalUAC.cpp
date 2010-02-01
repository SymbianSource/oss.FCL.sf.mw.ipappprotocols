// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : NormalUAC.cpp
// Part of       : TransactionUser
// Version       : SIP/4.0 
//



#include "SipAssert.h"
#include "Transmitter.h"
#include "TransactionMgr.h"
#include "ClientTransaction.h"

#include "NormalUAC.h"


// -----------------------------------------------------------------------------
// CNormalUAC::NewL
// -----------------------------------------------------------------------------
//
CNormalUAC* CNormalUAC::NewL(CUserAgentCreateParams& aParams,							 
                             MSipConnectionMgr& aConnectionMgr,
                             MSipUriResolver& aResolver,							 
							 CSIPSec& aSIPSec,
							 TUint32 aCSeqNumber)
	{
	CNormalUAC* self =
		new (ELeave) CNormalUAC(aParams, aResolver, aSIPSec, aCSeqNumber);
	CleanupStack::PushL(self);	
	self->CUserAgent::ConstructL(aConnectionMgr);
	self->CUserAgentClient::ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CNormalUAC::CNormalUAC
// -----------------------------------------------------------------------------
//
CNormalUAC::CNormalUAC(CUserAgentCreateParams& aParams,
					   MSipUriResolver& aResolver,
				       CSIPSec& aSIPSec,
				       TUint32 aCSeqNumber) :
	CUserAgentClient(aParams, aResolver, aSIPSec, aCSeqNumber)
	{
	}

// -----------------------------------------------------------------------------
// CNormalUAC::CreateTransactionL
// Creates a transaction object used by NormalUAC. The possible previous
// transaction should have been given to CDeleteMgr for deletion before
// UserAgent can create another transaction.
// -----------------------------------------------------------------------------
//
void CNormalUAC::CreateTransactionL()
	{
    __SIP_ASSERT_LEAVE(!iTransaction, KErrAlreadyExists);

	iTransaction =
        iTransactionMgr.CreateTransactionL(CTransactionBase::KClientTransaction,
										   *this,
										   *iTransmitter,
										   iTimerValues);
	}

// -----------------------------------------------------------------------------
// CNormalUAC::ReceiveResponseL
// -----------------------------------------------------------------------------
//
void CNormalUAC::ReceiveResponseL(CSIPResponse* aResp,
						  const CUserAgentState& aResolve,
						  const CUserAgentState& aWaitResponse,								  
						  const CUserAgentState& /*aWaitAckFromApp*/,
						  const CUserAgentState& /*aWaitTransactionToEnd*/)
	{
	__SIP_ASSERT_LEAVE(aResp, KErrArgument);

	if (!ShouldUaTryAgainL(aResp, aResolve, aWaitResponse))
		{
		//Ownership of the response is passed to the upper layer
		PassRespToTransactionOwnerL(aResp);
		}
	}
		
// -----------------------------------------------------------------------------
// CNormalUAC::PrepareTxForNewRequestL
// -----------------------------------------------------------------------------
//
void CNormalUAC::PrepareTxForNewRequestL(TTransactionId& aNewTransactionId)
	{
    __SIP_ASSERT_LEAVE(iTransmitter, KErrNotFound);

	iTransmitter->Cancel();
    static_cast<CClientTransaction*>(iTransaction)->
        RunIndependently(aNewTransactionId, &iDeleteMgr, ETrue);
	}

// -----------------------------------------------------------------------------
// CNormalUAC::Ptr
// -----------------------------------------------------------------------------
//
CNormalUAC& CNormalUAC::Ptr(CUserAgent& aUserAgent)
	{
	return static_cast<CNormalUAC&>(aUserAgent);
	}
