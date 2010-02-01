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
// Name          : NormalClientTaStates.cpp
// Part of       : Transaction
// Version       : SIP/5.0
//



#include "SipAssert.h"
#include "sipresponse.h"
#include "SIPMessageUtility.h"

#include "NormalClientTa.h"
#include "NormalClientTaStates.h"
#include "ClearProxy.h"

// -----------------------------------------------------------------------------
// CNormalClientTa_Trying::CNormalClientTa_Trying
// -----------------------------------------------------------------------------
//
CNormalClientTa_Trying::CNormalClientTa_Trying()
	{
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Trying::~CNormalClientTa_Trying
// -----------------------------------------------------------------------------
//
CNormalClientTa_Trying::~CNormalClientTa_Trying()
	{
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Trying::GetState
// -----------------------------------------------------------------------------
//
void CNormalClientTa_Trying::GetState(CSIPInternalStates::TState& aState) const
    {
    aState = CSIPInternalStates::ETransactionTrying;
    }

// -----------------------------------------------------------------------------
// CNormalClientTa_Trying::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CNormalClientTa_Trying::SetNeighbourStates(CTransactionState& aProceeding,
												CTransactionState& aCompleted)
	{
	iProceeding = &aProceeding;
	iCompleted = &aCompleted;
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Trying::SendRequestL
// -----------------------------------------------------------------------------
//
void CNormalClientTa_Trying::SendRequestL(CTransaction& aTransaction,
									  	  CSIPRequest& aReq,
									  	  const TInetAddr& aAddress,
									  	  RStringF aProtocol,
									  	  TBool aForceUDP,
									  	  const TSIPTransportParams& aParams,
									  	  CUri8* aOutboundProxy) const
	{
    __SIP_ASSERT_LEAVE(CSIPMessageUtility::CheckTransport(aProtocol),
                       KErrArgument);

	CNormalClientTransaction& ta = CNormalClientTransaction::Ptr(aTransaction);

	TClearProxy clearProxy(ta);
    CleanupStack::PushL(clearProxy.CleanupItem());

	ta.StoreRequest(aReq,
					aAddress,
					aParams,
					aProtocol,
					aForceUDP,
					aOutboundProxy);
	ta.SendToTransmitterL();	

	CleanupStack::Pop(); //cleanupItem
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Trying::SendCompleteL
// -----------------------------------------------------------------------------
//
void CNormalClientTa_Trying::SendCompleteL(CTransaction& aTransaction) const
	{
	CNormalClientTransaction& ta = CNormalClientTransaction::Ptr(aTransaction);
	ta.CheckForTransportChange();	
	ta.StartTimerFUnlessExistsL();

	if (aTransaction.IsUnreliableTransportUsed())
		{
		ta.StartTimerEL();
		}
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Trying::SendFailedL
// -----------------------------------------------------------------------------
//
void CNormalClientTa_Trying::SendFailedL(CTransaction& aTransaction,
										 TInt aError) const
	{
	aTransaction.TerminatedL(aError);
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Trying::ReceiveL
// -----------------------------------------------------------------------------
//
void CNormalClientTa_Trying::ReceiveL(CTransaction& aTransaction,
	                                  CSIPResponse* aResponse) const
	{
    __SIP_ASSERT_LEAVE(aResponse, KErrArgument);

    CNormalClientTransaction::Ptr(aTransaction).ReceiveResponseL(aResponse,
				                                                 *this,
				                                                 *iProceeding,
				                                                 *iCompleted);
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Trying::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CNormalClientTa_Trying::TimerExpiredL(CTransaction& aTransaction,
										   TTimerId aTimerId,
										   TAny* aTimerParam) const
	{    
    __SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);	

	HandleTimerExpirationL(aTransaction, aTimerId, aTimerParam);
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Proceeding::CNormalClientTa_Proceeding
// -----------------------------------------------------------------------------
//
CNormalClientTa_Proceeding::CNormalClientTa_Proceeding()
	{
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Proceeding::~CNormalClientTa_Proceeding
// -----------------------------------------------------------------------------
//
CNormalClientTa_Proceeding::~CNormalClientTa_Proceeding()
	{
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Proceeding::GetState
// -----------------------------------------------------------------------------
//
void
CNormalClientTa_Proceeding::GetState(CSIPInternalStates::TState& aState) const
    {
    aState = CSIPInternalStates::ETransactionProceeding;
    }

// -----------------------------------------------------------------------------
// CNormalClientTa_Proceeding::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CNormalClientTa_Proceeding::SetNeighbourStates(CTransactionState& aCompleted)
	{
	iCompleted = &aCompleted;
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Proceeding::SendCompleteL
// -----------------------------------------------------------------------------
//
void CNormalClientTa_Proceeding::SendCompleteL(CTransaction& aTransaction) const
	{
	CNormalClientTransaction& ta = CNormalClientTransaction::Ptr(aTransaction);
	ta.CheckForTransportChange();

	if (aTransaction.IsUnreliableTransportUsed())
		{
		//In proceeding state, timer E is set with value T2
		ta.IncreaseTimerEtoT2();
		ta.StartTimerEL();
		}
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Proceeding::SendFailedL
// -----------------------------------------------------------------------------
//
void CNormalClientTa_Proceeding::SendFailedL(CTransaction& aTransaction,
											 TInt aError) const
	{
	aTransaction.TerminatedL(aError);
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Proceeding::ReceiveL
// -----------------------------------------------------------------------------
//
void CNormalClientTa_Proceeding::ReceiveL(CTransaction& aTransaction,
	                                      CSIPResponse* aResponse) const
	{
    __SIP_ASSERT_LEAVE(aResponse, KErrArgument);

    CNormalClientTransaction::Ptr(aTransaction).ReceiveResponseL(aResponse,
				                                                 *this,
				                                                 *this,
				                                                 *iCompleted);
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Proceeding::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CNormalClientTa_Proceeding::TimerExpiredL(CTransaction& aTransaction,
											   TTimerId aTimerId,
											   TAny* aTimerParam) const
	{    
    __SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);
	
	HandleTimerExpirationL(aTransaction, aTimerId, aTimerParam);
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Completed::CNormalClientTa_Completed
// -----------------------------------------------------------------------------
//
CNormalClientTa_Completed::CNormalClientTa_Completed()
	{
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Completed::~CNormalClientTa_Completed
// -----------------------------------------------------------------------------
//
CNormalClientTa_Completed::~CNormalClientTa_Completed()
	{
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Completed::GetState
// -----------------------------------------------------------------------------
//
void
CNormalClientTa_Completed::GetState(CSIPInternalStates::TState& aState) const
    {
    aState = CSIPInternalStates::ETransactionCompleted;
    }

// -----------------------------------------------------------------------------
// CNormalClientTa_Completed::CanContinueWithoutOutgoingMsg
// -----------------------------------------------------------------------------
//
TBool CNormalClientTa_Completed::CanContinueWithoutOutgoingMsg() const
	{	
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Completed::SendCompleteL
// Occurs if Completed state was entered before receiving sending status event.
// No action.
// -----------------------------------------------------------------------------
//
void
CNormalClientTa_Completed::SendCompleteL(CTransaction& /*aTransaction*/) const
	{
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Completed::SendFailedL
// -----------------------------------------------------------------------------
//
void CNormalClientTa_Completed::SendFailedL(CTransaction& aTransaction,
											TInt aError) const
	{
	aTransaction.TerminatedL(aError);
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Completed::ReceiveL
// Final response already received. Ignore further responses.
// -----------------------------------------------------------------------------
//
void CNormalClientTa_Completed::ReceiveL(CTransaction& /*aTransaction*/,
	                                     CSIPResponse* aResponse) const
	{
    __SIP_ASSERT_LEAVE(aResponse, KErrArgument);

    delete aResponse;
	}

// -----------------------------------------------------------------------------
// CNormalClientTa_Completed::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CNormalClientTa_Completed::TimerExpiredL(CTransaction& aTransaction,
											  TTimerId aTimerId,
											  TAny* aTimerParam) const
	{    
    __SIP_ASSERT_LEAVE(aTimerParam, KErrArgument);

	HandleTimerExpirationL(aTransaction, aTimerId, aTimerParam);
	}
