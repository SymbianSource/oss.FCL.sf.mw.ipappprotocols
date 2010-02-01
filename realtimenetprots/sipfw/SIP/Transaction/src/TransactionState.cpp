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
// Name          : TransactionState.cpp
// Part of       : Transaction
// Version       : SIP/5.0
//



#include "SipAssert.h"
#include "siprequest.h"

#include "NormalClientTa.h"
#include "InviteClientTa.h"
#include "NormalServerTa.h"
#include "InviteServerTa.h"

#include "TransactionState.h"
#include "TransactionTimer.h"


// -----------------------------------------------------------------------------
// CTransactionState::CTransactionState
// -----------------------------------------------------------------------------
//
CTransactionState::CTransactionState()
	{
	}

// -----------------------------------------------------------------------------
// CTransactionState::~CTransactionState
// -----------------------------------------------------------------------------
//
CTransactionState::~CTransactionState()
	{
	}

// -----------------------------------------------------------------------------
// CTransactionState::SendRequestL
// -----------------------------------------------------------------------------
//
void CTransactionState::SendRequestL(CTransaction& /*aTransaction*/,
							     CSIPRequest& /*aReq*/,
							     const TInetAddr& /*aAddress*/,								     
							     RStringF /*aProtocol*/,
							     TBool /*aForceUDP*/,
							     const TSIPTransportParams& /*aParams*/,
							     CUri8* /*aOutboundProxy*/) const
	{
    __SIP_ASSERT_LEAVE(EFalse, KErrGeneral);
	}

// -----------------------------------------------------------------------------
// CTransactionState::SendResponseL
// -----------------------------------------------------------------------------
//
void CTransactionState::SendResponseL(CTransaction& /*aTransaction*/,
						      CSIPResponse* aResp,								      
						      RStringF /*aProtocol*/,
						      const TSIPTransportParams& /*aParams*/) const
	{
    __SIP_ASSERT_LEAVE(aResp, KErrArgument);
	__SIP_ASSERT_LEAVE(EFalse, KErrGeneral);
	}

// -----------------------------------------------------------------------------
// CTransactionState::SendCompleteL
// -----------------------------------------------------------------------------
//
void CTransactionState::SendCompleteL(CTransaction& /*aTransaction*/) const
	{
    __SIP_ASSERT_LEAVE(EFalse, KErrGeneral);
	}

// -----------------------------------------------------------------------------
// CTransactionState::SendFailedL
// -----------------------------------------------------------------------------
//
void CTransactionState::SendFailedL(CTransaction& /*aTransaction*/,
									TInt /*aError*/) const
	{    
    __SIP_ASSERT_LEAVE(EFalse, KErrGeneral);
	}

// -----------------------------------------------------------------------------
// CTransactionState::ReceiveL
// -----------------------------------------------------------------------------
//
void CTransactionState::ReceiveL(CTransaction& /*aTransaction*/,
                                 CSIPRequest* aRequest) const
    {
    __SIP_ASSERT_LEAVE(aRequest, KErrArgument);    
    __SIP_ASSERT_LEAVE(EFalse, KErrGeneral);
    }

// -----------------------------------------------------------------------------
// CTransactionState::ReceiveL
// -----------------------------------------------------------------------------
//	
void CTransactionState::ReceiveL(CTransaction& /*aTransaction*/,
                                 CSIPResponse* aResponse) const
    {
    __SIP_ASSERT_LEAVE(aResponse, KErrArgument);
    __SIP_ASSERT_LEAVE(EFalse, KErrGeneral);
    }

// -----------------------------------------------------------------------------
// CTransactionState::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CTransactionState::TimerExpiredL(CTransaction& /*aTransaction*/,
								      TTimerId /*aTimerId*/,
								      TAny* aTimerParam) const
	{
    __SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);
    __SIP_ASSERT_LEAVE(EFalse, KErrGeneral);
	}

// -----------------------------------------------------------------------------
// CTransactionState::HandleTimerExpirationL
// -----------------------------------------------------------------------------
//
void CTransactionState::HandleTimerExpirationL(CTransaction& aTransaction,
											   TTimerId aTimerId,
											   TAny* aTimerParam) const
	{
	__SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);	

	CTransactionTimer* timer =
        reinterpret_cast<CTransactionTimer*>(aTimerParam);
	timer->ExpiredL(aTransaction, aTimerId);

	//Even if CTransactionTimer::ExpiredL() used DeleteMgr to free the expired
	//timer (timer) and started a new instance of the same timer, the following
	//call is ok. It just doesn't do anything since timer points to the old
	//timer, and aTransaction now has the a pointer to the new timer.
	aTransaction.DeleteTimer(*timer);
	}

// -----------------------------------------------------------------------------
// CTransactionState::CanContinueWithoutOutgoingMsg
// -----------------------------------------------------------------------------
//
TBool CTransactionState::CanContinueWithoutOutgoingMsg() const
	{
	//By default, CTransaction can't continue without iOutgoingMsg
	return EFalse;
	}

#if defined(USE_SIP_LOGS)
// -----------------------------------------------------------------------------
// CTransactionState::Log
// -----------------------------------------------------------------------------
//
const TDesC8& CTransactionState::Log() const
	{
	return KNullDesC8;
	}

// -----------------------------------------------------------------------------
// CTransactionState::EnteringThisStateIsntLogged
// -----------------------------------------------------------------------------
//
const TDesC8& CTransactionState::EnteringThisStateIsntLogged()
    {
    //This constant is needed to avoid writing CInviteServerTransaction's
    //transition into state Sending2xx to log, but to get the transition from
    //state Sending2xx to Terminated written into log. An empty descriptor
    //can't be used here.
    _LIT8(KEnteringThisStateIsntLogged, " ");

    return KEnteringThisStateIsntLogged;
    }
#endif
