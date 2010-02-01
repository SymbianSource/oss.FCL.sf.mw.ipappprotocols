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
// Name          : NormalClientTa.cpp
// Part of       : Transaction
// Version       : SIP/5.0
//



#include "SipAssert.h"
#include "Lwtimer.h"
#include "sipresponse.h"

#include "NormalClientTa.h"
#include "TransactionState.h"
#include "TransactionTimer.h"
#include "TimerValues.h"
#include "RestoreTransactionState.h"

// -----------------------------------------------------------------------------
// CNormalClientTransaction::NewL
// -----------------------------------------------------------------------------
//
CNormalClientTransaction* CNormalClientTransaction::NewL(
										CUserAgentBase& aUserAgent,
										CTransmitter& aTransmitter,
										MTimerManager& aTimers,
										CTransactionState& aInitialState,
										TTimerValues& aTimerValues,
										MTransactionStore& aTransactionStore)
	{
	return new (ELeave) CNormalClientTransaction(aUserAgent,					
								                 aTransmitter,
								                 aTimers,
								                 aInitialState,
								                 aTimerValues,
								                 aTransactionStore);
	}

// -----------------------------------------------------------------------------
// CNormalClientTransaction::CNormalClientTransaction
// -----------------------------------------------------------------------------
//
CNormalClientTransaction::CNormalClientTransaction(CUserAgentBase& aUserAgent,
										CTransmitter& aTransmitter,
										MTimerManager& aTimers,
										CTransactionState& aInitialState,
										TTimerValues& aTimerValues,
										MTransactionStore& aTransactionStore) :
	CClientTransaction(aUserAgent,
					   aTransmitter,
					   aTimers,
					   aInitialState,
					   aTimerValues,
					   aTransactionStore)
	{
	}

// -----------------------------------------------------------------------------
// CNormalClientTransaction::~CNormalClientTransaction
// -----------------------------------------------------------------------------
//
CNormalClientTransaction::~CNormalClientTransaction()
	{
    //Scope operator to silence lint warning
    CNormalClientTransaction::CancelAllTimers();
	}

// -----------------------------------------------------------------------------
// CNormalClientTransaction::IncreaseTimerEtoT2
// -----------------------------------------------------------------------------
//
void CNormalClientTransaction::IncreaseTimerEtoT2()
	{
	__TEST_INVARIANT;
	__ASSERT_DEBUG(!iTimerE,
		User::Panic(_L("CNormCTa:IncreaseTimerEtoT2"), KErrAlreadyExists));

	iTimerEDuration = iTimerValues.T2();

	__TEST_INVARIANT;	
	}

// -----------------------------------------------------------------------------
// CNormalClientTransaction::StartTimerEL
// -----------------------------------------------------------------------------
//
void CNormalClientTransaction::StartTimerEL()
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(!iTimerE, KErrAlreadyExists);

	iTimerValues.DoubleUptoT2(iTimerEDuration);
	iTimerE = CTimerRetransmit::NewL(iTimers, this, iTimerEDuration);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CNormalClientTransaction::StopTimerE
// -----------------------------------------------------------------------------
//
void CNormalClientTransaction::StopTimerE()
	{
	__TEST_INVARIANT;

	delete iTimerE;
	iTimerE = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CNormalClientTransaction::StartTimerFUnlessExistsL
// -----------------------------------------------------------------------------
//
void CNormalClientTransaction::StartTimerFUnlessExistsL()
	{
	__TEST_INVARIANT;

	if (!iTimerF)
		{
		iTimerF = CTimerTerminateTa::NewL(iTimers,
										  this,
										  iTimerValues.Duration64xT1(),
										  KErrTimedOut);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CNormalClientTransaction::StopTimerF
// -----------------------------------------------------------------------------
//
void CNormalClientTransaction::StopTimerF()
	{
	__TEST_INVARIANT;

	delete iTimerF;
	iTimerF = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CNormalClientTransaction::StartTimerKL
// Duration is 0s for reliable and T4 for unreliable transports.
// -----------------------------------------------------------------------------
//
void CNormalClientTransaction::StartTimerKL()
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(!iTimerK, KErrAlreadyExists);

	TUint32 duration = 0;	
	if (IsUnreliableTransportUsed())
		{
		duration = iTimerValues.T4();
		}

	iTimerK = CTimerTerminateTa::NewL(iTimers, this, duration);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CNormalClientTransaction::StopTimerK
// -----------------------------------------------------------------------------
//
void CNormalClientTransaction::StopTimerK()
	{
	__TEST_INVARIANT;

	delete iTimerK;
	iTimerK = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CNormalClientTransaction::DeleteTimer
// -----------------------------------------------------------------------------
//
void CNormalClientTransaction::DeleteTimer(const CTransactionTimer& aTimer)
	{
    __TEST_INVARIANT;

	if (&aTimer == iTimerE)
		{
		StopTimerE();
		}

	if (&aTimer == iTimerF)
		{
		StopTimerF();
		}

	if (&aTimer == iTimerK)
		{
		StopTimerK();		
		}

    __TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CNormalClientTransaction::CancelAllTimers
// -----------------------------------------------------------------------------
//
void CNormalClientTransaction::CancelAllTimers()
	{
	__TEST_INVARIANT;

	StopTimerE();
	StopTimerF();
	StopTimerK();
	
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CNormalClientTransaction::Ptr
// -----------------------------------------------------------------------------
//
CNormalClientTransaction&
CNormalClientTransaction::Ptr(CTransaction& aTransaction)
	{	
	return static_cast<CNormalClientTransaction&>(aTransaction);
	}

// -----------------------------------------------------------------------------
// CNormalClientTransaction::ReceiveResponseL
// Stop timers when transaction can no longer return to previous state.
// -----------------------------------------------------------------------------
//
void CNormalClientTransaction::ReceiveResponseL(CSIPResponse* aResponse,
                                    const CTransactionState& aCurrentState,
                                    const CTransactionState& aProceeding,
                                    const CTransactionState& aCompleted)
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aResponse, KErrArgument);

    if (ShouldTaHandleResponse(aResponse))
        {
		TUint respCode = aResponse->ResponseCode();
        if (respCode >= 200)
            {
            StartTimerKL();
            ChangeState(aCompleted);
			}
		else
			{
			ChangeState(aProceeding);
			}

        TRestoreTransactionState restoreState(*this, aCurrentState, iTimerK);
        CleanupStack::PushL(restoreState.CleanupItem());

		PassResponseToUserAgentL(aResponse);

        CleanupStack::Pop(); //cleanupItem

        if (respCode >= 200)
            {
            StopTimerE();
	        StopTimerF();
            }
		}

	__TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CNormalClientTransaction::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CNormalClientTransaction::__DbgTestInvariant() const
	{
	if (iTimerEDuration > iTimerValues.T2())
		{
		User::Invariant();
		}
	}
