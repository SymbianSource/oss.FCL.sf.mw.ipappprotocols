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
// Name          : NormalUAS.cpp
// Part of       : TransactionUser
// Version       : SIP/4.0
//



#include "SipAssert.h"
#include "TransactionMgr.h"

#include "NormalUAS.h"
#include "UserAgentTimer.h"


// -----------------------------------------------------------------------------
// CNormalUAS::NewL
// -----------------------------------------------------------------------------
//
CNormalUAS* CNormalUAS::NewL(CUserAgentCreateParams& aParams,							
							 MSipConnectionMgr& aConnectionMgr,					
							 MSIPRequestRouter& aRouter,
                             MSipDialogs& aDialogs)
	{
	CNormalUAS* self = new (ELeave) CNormalUAS(aParams, aDialogs, aRouter);
	CleanupStack::PushL(self);	
	self->ConstructL(aConnectionMgr);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CNormalUAS::CNormalUAS
// -----------------------------------------------------------------------------
//
CNormalUAS::CNormalUAS(CUserAgentCreateParams& aParams,
					   MSipDialogs& aDialogs,
					   MSIPRequestRouter& aRouter) :
	CUserAgentServer(aParams, aDialogs, aRouter)
	{
	}

// -----------------------------------------------------------------------------
// CNormalUAS::ConstructL
// -----------------------------------------------------------------------------
//
void CNormalUAS::ConstructL(MSipConnectionMgr& aConnectionMgr)
	{
	CUserAgent::ConstructL(aConnectionMgr);
	CreateTransactionL();
	}

// -----------------------------------------------------------------------------
// CNormalUAS::~CNormalUAS
// -----------------------------------------------------------------------------
//
CNormalUAS::~CNormalUAS()
	{
	CancelAllTimers();
    }

// -----------------------------------------------------------------------------
// CNormalUAS::CreateTransactionL
// Creates the kind of CTransaction used by NormalUAS
// -----------------------------------------------------------------------------
//
void CNormalUAS::CreateTransactionL()
	{
    __SIP_ASSERT_LEAVE(!iTransaction, KErrAlreadyExists);
    __SIP_ASSERT_LEAVE(iTransmitter, KErrNotFound);

	iTransaction = iTransactionMgr.CreateTransactionL(
										CTransactionBase::KServerTransaction,
										*this,
										*iTransmitter,
										iTimerValues);
	}

// -----------------------------------------------------------------------------
// CNormalUAS::DeleteTimer
// -----------------------------------------------------------------------------
//
void CNormalUAS::DeleteTimer(const CUserAgentTimer& aTimer)
	{	
    if (&aTimer == iTimerOwnerResolver)
        {
        StopTimerOwnerResolver();
        }	
	}

// -----------------------------------------------------------------------------
// CNormalUAS::CancelAllTimers
// -----------------------------------------------------------------------------
//
void CNormalUAS::CancelAllTimers()
	{	
	StopTimerOwnerResolver();
	}

// -----------------------------------------------------------------------------
// CNormalUAS::Ptr
// -----------------------------------------------------------------------------
//
CNormalUAS& CNormalUAS::Ptr(CUserAgent& aUserAgent)
	{
	return static_cast<CNormalUAS&>(aUserAgent);
	}
