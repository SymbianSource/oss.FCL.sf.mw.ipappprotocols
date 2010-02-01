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
// Name          : InviteClientTaStates.cpp
// Part of       : Transaction
// Version       : SIP/5.0
//



#include "siprequest.h"
#include "sipresponse.h"
#include "SipAssert.h"
#include "SIPMessageUtility.h"

#include "InviteClientTa.h"
#include "InviteClientTaStates.h"
#include "ClearProxy.h"


// -----------------------------------------------------------------------------
// CInviteClientTa_Calling::CInviteClientTa_Calling
// -----------------------------------------------------------------------------
//
CInviteClientTa_Calling::CInviteClientTa_Calling()
	{
	}

// -----------------------------------------------------------------------------
// CInviteClientTa_Calling::~CInviteClientTa_Calling
// -----------------------------------------------------------------------------
//
CInviteClientTa_Calling::~CInviteClientTa_Calling()
	{
	}

// -----------------------------------------------------------------------------
// CInviteClientTa_Calling::GetState
// -----------------------------------------------------------------------------
//
void CInviteClientTa_Calling::GetState(CSIPInternalStates::TState& aState) const
    {
    aState = CSIPInternalStates::ETransactionCalling;
    }

#if defined(USE_SIP_LOGS)
// -----------------------------------------------------------------------------
// CInviteClientTa_Calling::Log
// -----------------------------------------------------------------------------
//
const TDesC8& CInviteClientTa_Calling::Log() const
	{
	_LIT8(KState, "Calling");
	return KState;
	}
#endif

// -----------------------------------------------------------------------------
// CInviteClientTa_Calling::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void
CInviteClientTa_Calling::SetNeighbourStates(CTransactionState& aProceeding,
                                 			CTransactionState& aCompleted)
	{
	iProceeding = &aProceeding;
	iCompleted = &aCompleted;
	}

// -----------------------------------------------------------------------------
// CInviteClientTa_Calling::SendRequestL
// CleanupItem handles ownership of aOutboundProxy in case of leave.
// -----------------------------------------------------------------------------
//
void CInviteClientTa_Calling::SendRequestL(CTransaction& aTransaction,
										   CSIPRequest& aInvite,
										   const TInetAddr& aAddress,										   
										   RStringF aProtocol,
										   TBool aForceUDP,
										   const TSIPTransportParams& aParams,
										   CUri8* aOutboundProxy) const
	{
    __SIP_ASSERT_LEAVE(CSIPMessageUtility::CheckTransport(aProtocol),
                       KErrArgument);

	CInviteClientTransaction& ta = CInviteClientTransaction::Ptr(aTransaction);

	TClearProxy clearProxy(ta);
    CleanupStack::PushL(clearProxy.CleanupItem());

	ta.StoreRequest(aInvite,
					aAddress,
					aParams,
					aProtocol,
					aForceUDP,
					aOutboundProxy);
	ta.SendToTransmitterL();	

	CleanupStack::Pop(); //cleanupItem
	}

// -----------------------------------------------------------------------------
// CInviteClientTa_Calling::SendCompleteL
// -----------------------------------------------------------------------------
//
void CInviteClientTa_Calling::SendCompleteL(CTransaction& aTransaction) const
	{
	CInviteClientTransaction& ta = CInviteClientTransaction::Ptr(aTransaction);
	ta.CheckForTransportChange();
	ta.StartTimerBUnlessExistsL();

	if (aTransaction.IsUnreliableTransportUsed())
		{
		ta.StartTimerAL();
		}
	}

// -----------------------------------------------------------------------------
// CInviteClientTa_Calling::SendFailedL
// -----------------------------------------------------------------------------
//
void CInviteClientTa_Calling::SendFailedL(CTransaction& aTransaction,
										  TInt aError) const
	{    
	aTransaction.TerminatedL(aError);
	}

// -----------------------------------------------------------------------------
// CInviteClientTa_Calling::ReceiveL
// If 2xx is received, UAC terminates the transaction. Stop timers when
// transaction can't return to previous state (ReceiveResponseL has returned).
// -----------------------------------------------------------------------------
//
void CInviteClientTa_Calling::ReceiveL(CTransaction& aTransaction,
	                                   CSIPResponse* aResponse) const
	{
    __SIP_ASSERT_LEAVE(aResponse, KErrArgument);

	CInviteClientTransaction& ta = CInviteClientTransaction::Ptr(aTransaction);

	if (ta.ShouldTaHandleResponse(aResponse))
		{
		CTransactionTimer* proceedingTimer = NULL;
		CTransactionTimer* natTimer = NULL;
		if (!CSIPMessageUtility::IsFinalResponse(*aResponse))
			{
			proceedingTimer = ta.StartTimerProceedingL();
			natTimer = ta.StartTimerNATL();
            aTransaction.ChangeState(*iProceeding);
			}

		ta.ReceiveResponseL(aResponse,
							*this,
							*iCompleted,
							proceedingTimer,
							natTimer);
        //Stop retransmission timer A. Timer B has only effect in Calling state
		ta.StopTimerA();
		ta.StopTimerB();
		}
	}

// -----------------------------------------------------------------------------
// CInviteClientTa_Calling::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CInviteClientTa_Calling::TimerExpiredL(CTransaction& aTransaction,
										    TTimerId aTimerId,
										    TAny* aTimerParam) const
	{    
    __SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);

	HandleTimerExpirationL(aTransaction, aTimerId, aTimerParam);
	}

// -----------------------------------------------------------------------------
// CInviteClientTa_Proceeding::CInviteClientTa_Proceeding
// -----------------------------------------------------------------------------
//
CInviteClientTa_Proceeding::CInviteClientTa_Proceeding()
	{
	}

// -----------------------------------------------------------------------------
// CInviteClientTa_Proceeding::~CInviteClientTa_Proceeding
// -----------------------------------------------------------------------------
//
CInviteClientTa_Proceeding::~CInviteClientTa_Proceeding()
	{
	}

// -----------------------------------------------------------------------------
// CInviteClientTa_Proceeding::GetState
// -----------------------------------------------------------------------------
//
void
CInviteClientTa_Proceeding::GetState(CSIPInternalStates::TState& aState) const
    {
    aState = CSIPInternalStates::ETransactionProceeding;
    }

#if defined(USE_SIP_LOGS)
// -----------------------------------------------------------------------------
// CInviteClientTa_Proceeding::Log
// -----------------------------------------------------------------------------
//
const TDesC8& CInviteClientTa_Proceeding::Log() const
	{
	_LIT8(KState, "Proceeding");
	return KState;
	}
#endif

// -----------------------------------------------------------------------------
// CInviteClientTa_Proceeding::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void
CInviteClientTa_Proceeding::SetNeighbourStates(CTransactionState& aCompleted)
	{
	iCompleted = &aCompleted;
	}

// -----------------------------------------------------------------------------
// CInviteClientTa_Proceeding::SendCompleteL
// Occurs if 1xx comes before CTransmitter sent SendCompleteL or if iTimerNAT
// retransmitted INVITE.
// -----------------------------------------------------------------------------
//
void CInviteClientTa_Proceeding::SendCompleteL(CTransaction& aTransaction) const
	{
	CInviteClientTransaction& ta = CInviteClientTransaction::Ptr(aTransaction);
	ta.CheckForTransportChange();
	ta.StartTimerNATL();
	}

// -----------------------------------------------------------------------------
// CInviteClientTa_Proceeding::SendFailedL
// -----------------------------------------------------------------------------
//
void CInviteClientTa_Proceeding::SendFailedL(CTransaction& aTransaction,
											 TInt aError) const
	{
	aTransaction.TerminatedL(aError);
	}

// -----------------------------------------------------------------------------
// CInviteClientTa_Proceeding::ReceiveL
// -----------------------------------------------------------------------------
//
void CInviteClientTa_Proceeding::ReceiveL(CTransaction& aTransaction,
	                                      CSIPResponse* aResponse) const
	{    
    __SIP_ASSERT_LEAVE(aResponse, KErrArgument);
	
	CInviteClientTransaction& ta = CInviteClientTransaction::Ptr(aTransaction);

	if (ta.ShouldTaHandleResponse(aResponse))
		{		
		TBool isErrorResponse = aResponse->IsErrorResponse();
		ta.ReceiveResponseL(aResponse, *this, *iCompleted);
		if (isErrorResponse)
            {
            ta.StopTimerProceeding();
            ta.StopTimerNAT();
            }
		}
	}

// -----------------------------------------------------------------------------
// CInviteClientTa_Proceeding::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CInviteClientTa_Proceeding::TimerExpiredL(CTransaction& aTransaction,
											   TTimerId aTimerId,
											   TAny* aTimerParam) const
	{
    __SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);

	HandleTimerExpirationL(aTransaction, aTimerId, aTimerParam);
	}

// -----------------------------------------------------------------------------
// CInviteClientTa_Completed::CInviteClientTa_Completed
// -----------------------------------------------------------------------------
//
CInviteClientTa_Completed::CInviteClientTa_Completed()
	{
	}

// -----------------------------------------------------------------------------
// CInviteClientTa_Completed::~CInviteClientTa_Completed
// -----------------------------------------------------------------------------
//
CInviteClientTa_Completed::~CInviteClientTa_Completed()
	{
	}

// -----------------------------------------------------------------------------
// CInviteClientTa_Completed::GetState
// -----------------------------------------------------------------------------
//
void
CInviteClientTa_Completed::GetState(CSIPInternalStates::TState& aState) const
    {
    aState = CSIPInternalStates::ETransactionCompleted;
    }

#if defined(USE_SIP_LOGS)
// -----------------------------------------------------------------------------
// CInviteClientTa_Completed::Log
// -----------------------------------------------------------------------------
//
const TDesC8& CInviteClientTa_Completed::Log() const
	{
	_LIT8(KState, "Completed");
	return KState;
	}
#endif

// -----------------------------------------------------------------------------
// CInviteClientTa_Completed::SendCompleteL
// -----------------------------------------------------------------------------
//
void CInviteClientTa_Completed::SendCompleteL(CTransaction& aTransaction) const
	{
	CInviteClientTransaction::Ptr(aTransaction).StartTimerDUnlessExistsL();
	}

// -----------------------------------------------------------------------------
// CInviteClientTa_Completed::SendFailedL
// -----------------------------------------------------------------------------
//
void CInviteClientTa_Completed::SendFailedL(CTransaction& aTransaction,
											TInt aError) const
	{
	aTransaction.TerminatedL(aError);
	}

// -----------------------------------------------------------------------------
// CInviteClientTa_Completed::ReceiveL
// Retransmit ACK unless currently being sent.
// -----------------------------------------------------------------------------
//
void CInviteClientTa_Completed::ReceiveL(CTransaction& aTransaction,
	                                     CSIPResponse* aResponse) const
	{
    __SIP_ASSERT_LEAVE(aResponse, KErrArgument);

	if (!aTransaction.IsTransmitterSending() && aResponse->IsErrorResponse())
        {
	    CInviteClientTransaction::Ptr(aTransaction).SendToTransmitterL();
        }

    delete aResponse;
	}

// -----------------------------------------------------------------------------
// CInviteClientTa_Completed::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CInviteClientTa_Completed::TimerExpiredL(CTransaction& aTransaction,
											  TTimerId aTimerId,
											  TAny* aTimerParam) const
	{    
    __SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);	

	HandleTimerExpirationL(aTransaction, aTimerId, aTimerParam);
	}
