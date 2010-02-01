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
// Name          : NormalServerTa.cpp
// Part of       : Transaction
// Version       : SIP/5.0
//



#include "SipAssert.h"
#include "Lwtimer.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "UserAgentBase.h"
#include "TimerValues.h"

#include "NormalServerTa.h"
#include "TransactionTimer.h"

// -----------------------------------------------------------------------------
// CNormalServerTransaction::NewL
// -----------------------------------------------------------------------------
//
CNormalServerTransaction*
CNormalServerTransaction::NewL(CUserAgentBase& aUserAgent,
							   CTransmitter& aTransmitter,
							   MTimerManager& aTimers,
							   CTransactionState& aInitialState,
							   TTimerValues& aTimerValues)
	{
	CNormalServerTransaction* self =
        new (ELeave) CNormalServerTransaction(aUserAgent,
											  aTransmitter,
											  aTimers,
											  aInitialState,
											  aTimerValues);
	CleanupStack::PushL(self);
	self->ConstructServerTaL();
	CleanupStack::Pop(self);
	return self;	
	}

// -----------------------------------------------------------------------------
// CNormalServerTransaction::CNormalServerTransaction
// -----------------------------------------------------------------------------
//
CNormalServerTransaction::CNormalServerTransaction(CUserAgentBase& aUserAgent,
										CTransmitter& aTransmitter,
										MTimerManager& aTimers,
										CTransactionState& aInitialState,
										TTimerValues& aTimerValues) :
	CServerTransaction(aUserAgent,
					   aTransmitter,
					   aTimers,
					   aInitialState,
					   aTimerValues)
	{
	}

// -----------------------------------------------------------------------------
// CNormalServerTransaction::~CNormalServerTransaction
// -----------------------------------------------------------------------------
//
CNormalServerTransaction::~CNormalServerTransaction()
	{
    //Scope operator to silence lint warning
    CNormalServerTransaction::CancelAllTimers();
	}

// -----------------------------------------------------------------------------
// CNormalServerTransaction::DeleteTimer
// -----------------------------------------------------------------------------
//
void CNormalServerTransaction::DeleteTimer(const CTransactionTimer& aTimer)
	{
	__TEST_INVARIANT;

	if (&aTimer == iTimerF2)
		{
		StopTimerF2();		
		}

	if (&aTimer == iTimerJ)
		{
		StopTimerJ();		
		}

	if (&aTimer == iTimerSend100)
		{
		StopTimerSend100();
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CNormalServerTransaction::StartTimerF2UnlessExistsL
// Same duration as for timer F (64*T1).
// -----------------------------------------------------------------------------
//
void CNormalServerTransaction::StartTimerF2UnlessExistsL()
	{
	__TEST_INVARIANT;

	if (!iTimerF2)
		{
		iTimerF2 = CTimerTerminateTa::NewL(iTimers,
										   this,
										   iTimerValues.Duration64xT1(),
										   KErrTimedOut);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CNormalServerTransaction::StopTimerF2
// -----------------------------------------------------------------------------
//
void CNormalServerTransaction::StopTimerF2()
	{
	__TEST_INVARIANT;

	delete iTimerF2;
	iTimerF2 = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CNormalServerTransaction::StartTimerJUnlessExistsL
// Duration is 0s for reliable and 64*T1 for unreliable transports.
// -----------------------------------------------------------------------------
//
void CNormalServerTransaction::StartTimerJUnlessExistsL()
	{
	__TEST_INVARIANT;

	if (!iTimerJ)
		{
		TUint32 duration = 0;
		if (IsUnreliableTransportUsed())
			{
			duration = iTimerValues.Duration64xT1();
			}

		iTimerJ = CTimerTerminateTa::NewL(iTimers, this, duration);
		}

	__TEST_INVARIANT;	
	}

// -----------------------------------------------------------------------------
// CNormalServerTransaction::StopTimerJ
// -----------------------------------------------------------------------------
//
void CNormalServerTransaction::StopTimerJ()
	{
	__TEST_INVARIANT;

	delete iTimerJ;
	iTimerJ = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CNormalServerTransaction::IsTimerSend100Running
// -----------------------------------------------------------------------------
//
TBool CNormalServerTransaction::IsTimerSend100Running() const
	{
	__TEST_INVARIANT;
	return iTimerSend100 != NULL;
	}

// -----------------------------------------------------------------------------
// CNormalServerTransaction::StartTimerSend100L
// Same duration can be used for both reliable and unreliable transports.
// -----------------------------------------------------------------------------
//
void CNormalServerTransaction::StartTimerSend100L(CSIPRequest& aRequest)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(!iTimerSend100, KErrAlreadyExists);

	TUint32 duration = 0;
	TUint step = 0;
	iTimerValues.DoubleUptoT2(step);

	while (step < iTimerValues.T2())
		{
		duration += step;
		iTimerValues.DoubleUptoT2(step);
		}

	Create100L(aRequest);
	iTimerSend100 = CTimerSend100::NewL(iTimers,
									    this,
									    duration,
									    static_cast<CSIPResponse*>(iOutgoingMsg),
									    iTransportProtocol,
									    iTransportParams);
	//Timer took ownership of the SIP response
	iOutgoingMsg = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CNormalServerTransaction::StopTimerSend100
// -----------------------------------------------------------------------------
//
void CNormalServerTransaction::StopTimerSend100()
	{
	__TEST_INVARIANT;

	delete iTimerSend100;
	iTimerSend100 = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CNormalServerTransaction::ReceiveRetransmissionL
// -----------------------------------------------------------------------------
//
void CNormalServerTransaction::ReceiveRetransmissionL(CSIPRequest* aRequest)
	{
	__SIP_ASSERT_LEAVE(aRequest, KErrArgument);

	if (!IsTransmitterSending())
		{
		SendToTransmitterL();
		}

    delete aRequest;
	}

// -----------------------------------------------------------------------------
// CNormalServerTransaction::CancelAllTimers
// -----------------------------------------------------------------------------
//
void CNormalServerTransaction::CancelAllTimers()
	{
	__TEST_INVARIANT;

	StopTimerJ();
	StopTimerF2();
	StopTimerSend100();

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CNormalServerTransaction::Ptr
// -----------------------------------------------------------------------------
//
CNormalServerTransaction&
CNormalServerTransaction::Ptr(CTransaction& aTransaction)
	{
	return static_cast<CNormalServerTransaction&>(aTransaction);
	}

// -----------------------------------------------------------------------------
// CNormalServerTransaction::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CNormalServerTransaction::__DbgTestInvariant() const
	{
	//Timers J and F2 must not run at the same time
	if (iTimerJ && iTimerF2)
		{	
		User::Invariant();
		}
	}

