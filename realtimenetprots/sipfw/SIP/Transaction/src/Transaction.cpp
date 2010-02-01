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
// Name          : Transaction.cpp
// Part of       : Transaction
// Version       : SIP/5.0
//



#include "SipAssert.h"
#include "siperr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "Lwtimer.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "UserAgentBase.h"
#include "Transmitter.h"
#include "SIPMessageUtility.h"

#include "CTransaction.h"
#include "TransactionState.h"
#include "SipLogs.h"
// -----------------------------------------------------------------------------
// CTransaction::CTransaction
// -----------------------------------------------------------------------------
//
CTransaction::CTransaction(CUserAgentBase& aUserAgent,
						   CTransmitter& aTransmitter,
						   MTimerManager& aTimers,
						   CTransactionState& aInitialState,
						   TTimerValues& aTimerValues) :
	iUserAgent(&aUserAgent),
	iTransmitter(&aTransmitter),
	iTimers(aTimers),
	iTransportProtocol(SIPStrings::StringF(SipStrConsts::EUDP)),
	iTransportParams(aUserAgent.TransportParams()),
	iTimerValues(aTimerValues),
	iTerminated(EFalse),
	iState(&aInitialState)
	{
#if defined(USE_SIP_LOGS)
	WriteStateToLog(*iState);
#endif
	}

// -----------------------------------------------------------------------------
// CTransaction::~CTransaction
// -----------------------------------------------------------------------------
//
CTransaction::~CTransaction()
	{
	}

// -----------------------------------------------------------------------------
// CTransaction::TransactionId
// This function is only used by UAS. CClientTransaction overloads this.
// -----------------------------------------------------------------------------
//
TTransactionId CTransaction::TransactionId() const
	{
	__TEST_INVARIANT;
	__ASSERT_DEBUG(iUserAgent,
				   User::Panic(_L("CTransaction:TaId"), KErrGeneral));
	return iUserAgent->TransactionId();
	}

#if defined(USE_SIP_LOGS)
// -----------------------------------------------------------------------------
// CTransaction::WriteStateToLog
// Only INVITE transactions write logs.
// -----------------------------------------------------------------------------
//
void CTransaction::WriteStateToLog(const CTransactionState& aState) const
	{
	const TDesC8& log = aState.Log();

	if (log.Length() > 0)
		{
		//If INVITE-UAC stops transaction as a 2xx was received, UAC writes the
		//transaction state to log as transaction won't have the TransactionId.
		TTransactionId id = TransactionId();
		if (id != KEmptyTransactionId)
			{
			TBuf8<40> state;
			if (iTerminated)
				{
				state.Copy(_L8("Terminated"));
				}
			else
				{				
                if (log.Compare(
                    CTransactionState::EnteringThisStateIsntLogged()) == 0)
					{
					return;
					}
				state.Copy(log);
				}

			__SIP_INT_LOG1( "TU transaction, taID", id )
			__SIP_DES8_LOG( "TU transaction, state", state )
			}
		}
	}
#endif

// -----------------------------------------------------------------------------
// CTransaction::SendRequestL
// -----------------------------------------------------------------------------
//
void CTransaction::SendRequestL(CSIPRequest& aReq,
								const TInetAddr& aRemoteAddr,
								RStringF aProtocol,
								TBool aForceUDP,
								const TSIPTransportParams& aParams,
								CUri8* aOutboundProxy)
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(CSIPMessageUtility::CheckTransport(aProtocol),
                       KErrArgument);
	if (iTerminated)
		{
		User::Leave(KErrSIPInvalidTransactionState);
		}

	iState->SendRequestL(*this,
						 aReq,
						 aRemoteAddr,
						 aProtocol,
						 aForceUDP,
						 aParams,
						 aOutboundProxy);
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransaction::SendResponseL
// -----------------------------------------------------------------------------
//
void CTransaction::SendResponseL(CSIPResponse* aResp,
								 RStringF aProtocol,
								 const TSIPTransportParams& aParams)
	{
	__TEST_INVARIANT;
	
	CSIPInternalStates::TState state;
    this->GetState(state);
    __SIP_INT_LOG1("CTransaction::SendResponseL Transaciton state is: ", state)

	    
	TTransactionId Id= this->TransactionId();
    __SIP_INT_LOG1("CTransaction::SendResponseL Transaciton ID is: ", Id)

	__SIP_ASSERT_LEAVE(aResp, KErrArgument);
	__SIP_ASSERT_LEAVE(CSIPMessageUtility::CheckTransport(aProtocol),
                       KErrArgument);
	if (iTerminated)
		{
		User::Leave(KErrSIPInvalidTransactionState);
		}

    iState->SendResponseL(*this, aResp, aProtocol, aParams);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransaction::ReceiveL
// -----------------------------------------------------------------------------
//
void CTransaction::ReceiveL(CSIPRequest* aRequest)
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aRequest, KErrArgument);
	__SIP_MESSAGE_LOG("TransactionUser", *aRequest)
	
	if (iTerminated)
		{
		delete aRequest;
		}
	else
		{
		iState->ReceiveL(*this, aRequest);
		}

	__TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CTransaction::ReceiveL
// -----------------------------------------------------------------------------
//
void CTransaction::ReceiveL(CSIPResponse* aResponse)
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aResponse, KErrArgument);
	__SIP_MESSAGE_LOG("TransactionUser", *aResponse)
	
	if (iTerminated)
		{
		delete aResponse;
		}
	else
		{
		iState->ReceiveL(*this, aResponse);
		}

	__TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CTransaction::UpdateTransportProtocol
// -----------------------------------------------------------------------------
//
TBool CTransaction::UpdateTransportProtocol(CSIPMessage& aMsg)
	{
	__TEST_INVARIANT;
	return CSIPMessageUtility::TransportProtocol(aMsg, iTransportProtocol);
	}

// -----------------------------------------------------------------------------
// CTransaction::LeaveOccurred
// Use overloaded TerminatedL (not Terminated), to free detached transaction.
// -----------------------------------------------------------------------------
//
void CTransaction::LeaveOccurred(TInt aReason)
	{
	__TEST_INVARIANT;
	__ASSERT_DEBUG(aReason != KErrNone,
				   User::Panic(_L("CTransaction:LeaveOccurred"), KErrArgument));

    if (aReason != KErrSIPInvalidDialogResponse)
    	{
	    TRAP_IGNORE(TerminatedL(aReason))

		if (iUserAgent)
			{
			iUserAgent->Stop(aReason);
			}
    	}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransaction::TransportParams
// -----------------------------------------------------------------------------
//
const TSIPTransportParams& CTransaction::TransportParams() const
	{
	__TEST_INVARIANT;
	return iTransportParams;	
	}

// -----------------------------------------------------------------------------
// CTransaction::TimerExpiredL
// Ignore timer if terminated. Occurs e.g. if ClientTransaction has timers E and
// F, timer F expires so transaction terminates, but timer E isn't stopped until
// Transaction is deleted.
// -----------------------------------------------------------------------------
//
void CTransaction::TimerExpiredL(TTimerId aTimerId, TAny* aTimerParam)
	{
	__TEST_INVARIANT;
	
    // Check if it is a Invite transaction
	if (IsInviteTransaction())
	{
		__SIP_LOG("It is invite transaction CTransaction::TimerExpiredL")
	}
	else
	{
		__SIP_LOG("It is non-invite transaction CTransaction::TimerExpiredL")
	}
	
	// Get the transacton state
	CSIPInternalStates::TState state;
	this->GetState(state);
	__SIP_INT_LOG1("CTransaction::TimerExpiredL Transaciton state is: ", state)
	
	TTransactionId Id= this->TransactionId();
    __SIP_INT_LOG1("CTransaction::TimerExpiredL Transaciton ID is: ", Id)

	if (!iTerminated)
		{
		iState->TimerExpiredL(*this, aTimerId, aTimerParam);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransaction::SendCompleteL
// -----------------------------------------------------------------------------
//
void CTransaction::SendCompleteL()
	{
	__TEST_INVARIANT;

	if (!iTerminated)
		{
		iState->SendCompleteL(*this);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransaction::SendFailedL
// -----------------------------------------------------------------------------
//
void CTransaction::SendFailedL(TInt aError)
	{
	__TEST_INVARIANT;

	if (!iTerminated)
		{
		iState->SendFailedL(*this, aError);
		}
	
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransaction::LeaveFromTransmitter
// -----------------------------------------------------------------------------
//
void CTransaction::LeaveFromTransmitter(TInt aReason)
	{
	__TEST_INVARIANT;
	__ASSERT_DEBUG(aReason != KErrNone,
				   User::Panic(_L("CTa:LeaveFromTransmitter"), KErrArgument));

	LeaveOccurred(aReason);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransaction::IsUnreliableTransportUsed
// -----------------------------------------------------------------------------
//
TBool CTransaction::IsUnreliableTransportUsed() const
	{
	__TEST_INVARIANT;
	return iTransportProtocol == SIPStrings::StringF(SipStrConsts::EUDP);
	}

// -----------------------------------------------------------------------------
// CTransaction::IsTransmitterSending
// -----------------------------------------------------------------------------
//
TBool CTransaction::IsTransmitterSending() const
	{
	__TEST_INVARIANT;
	return iTransmitter && iTransmitter->IsActive();
	}

// -----------------------------------------------------------------------------
// CTransaction::IsInviteTransaction
// -----------------------------------------------------------------------------
//
TBool CTransaction::IsInviteTransaction() const
	{
	__TEST_INVARIANT;
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CTransaction::State
// -----------------------------------------------------------------------------
//
const CTransactionState* CTransaction::State() const
	{
	__TEST_INVARIANT;
	return iState;
	}

// -----------------------------------------------------------------------------
// CTransaction::ChangeState
// -----------------------------------------------------------------------------
//
void CTransaction::ChangeState(const CTransactionState& aNewState)
	{
	__TEST_INVARIANT;
    
	iState = &aNewState;

#if defined(USE_SIP_LOGS)	
	WriteStateToLog(aNewState);
#endif

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransaction::ClearSendBuffer
// If buffer has ACK, CInviteClientTransaction created and owns it.
// -----------------------------------------------------------------------------
//
TBool CTransaction::ClearSendBuffer()
	{
	__TEST_INVARIANT;

	if (iOutgoingMsg && CSIPMessageUtility::IsAck(*iOutgoingMsg))
		{
		return ETrue;
		}

	//Can't use iOutgoingMsg anymore. Cancel a possible sending so ConnectionMgr
	//won't use it either.
	iOutgoingMsg = NULL;
	if (iTransmitter)
    	{
		iTransmitter->Cancel();
    	}

	//A terminated transaction can continue without iOutgoingMsg
	return iTerminated || iState->CanContinueWithoutOutgoingMsg();
	}

// -----------------------------------------------------------------------------
// CTransaction::GetState
// -----------------------------------------------------------------------------
//
void CTransaction::GetState(CSIPInternalStates::TState& aState)
    {
    __TEST_INVARIANT;

    if (iTerminated)
        {
        aState = CSIPInternalStates::ETransactionTerminated;        
        }
    else
        {        
        iState->GetState(aState);
        }
    }

// -----------------------------------------------------------------------------
// CTransaction::Terminated
// -----------------------------------------------------------------------------
//
void CTransaction::Terminated()
	{
	__TEST_INVARIANT;

	if (!iTerminated)
		{
		iTerminated = ETrue;

#if defined(USE_SIP_LOGS)
		WriteStateToLog(*iState);
#endif
        if (iTransmitter)
        	{
			iTransmitter->DetachCallback(this);
        	}
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransaction::TerminatedL
// -----------------------------------------------------------------------------
//
void CTransaction::TerminatedL(TInt aReason)
	{
	__TEST_INVARIANT;
#if defined(USE_SIP_LOGS)
        WriteStateToLog(*iState);
#endif

	if (!iTerminated)
		{
		Terminated();

		if (iUserAgent)
			{
			iUserAgent->TransactionEndsL(aReason);
            }
		}

	__TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CTransaction::HasTerminated
// -----------------------------------------------------------------------------
//
TBool CTransaction::HasTerminated() const
	{
    __TEST_INVARIANT;
	return iTerminated;
	}

// -----------------------------------------------------------------------------
// CTransaction::DetachFromUserAgent
// -----------------------------------------------------------------------------
//
void CTransaction::DetachFromUserAgent()
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_RETURN(iUserAgent, KErrNotFound);

	iUserAgent = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransaction::TransportProtocol
// -----------------------------------------------------------------------------
//
RStringF CTransaction::TransportProtocol() const
	{
	__TEST_INVARIANT;

	return iTransportProtocol;
	}

// -----------------------------------------------------------------------------
// CTransaction::__DbgTestInvariant
// iUserAgent can be NULL. Detached transaction sets iTransmitter to NULL.
// -----------------------------------------------------------------------------
//

void CTransaction::__DbgTestInvariant() const
	{
	if (!iState)
		{
		User::Invariant();
		}
	}

