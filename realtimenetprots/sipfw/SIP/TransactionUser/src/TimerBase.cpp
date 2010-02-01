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
// Name          : TimerBase.cpp
// Part of       : TransactionUser
// Version       : SIP/6.0
//



#include "TimerBase.h"

// -----------------------------------------------------------------------------
// CTimerBase::CTimerBase
// -----------------------------------------------------------------------------
//
CTimerBase::CTimerBase(MTimerManager& aTimerMgr,
					   MExpirationHandler* aObserver,
					   TUint32 aDuration) :
	iTimerMgr(aTimerMgr),
	iObserver(aObserver),
	iDuration(aDuration)
	{
	__ASSERT_DEBUG(aObserver, User::Panic(_L("CTimerBase"), KErrArgument));
	}

// -----------------------------------------------------------------------------
// CTimerBase::ConstructL
// -----------------------------------------------------------------------------
//
void CTimerBase::ConstructL()						   
	{
	iId = iTimerMgr.StartL(iObserver, iDuration, this);
	}

// -----------------------------------------------------------------------------
// CTimerBase::~CTimerBase
// -----------------------------------------------------------------------------
//
CTimerBase::~CTimerBase()
	{
	Stop();
	}

// -----------------------------------------------------------------------------
// CTimerBase::Stop
// -----------------------------------------------------------------------------
//
void CTimerBase::Stop()
	{
	iTimerMgr.Stop(iId);
	iId = MTimerManager::KNoSuchTimer;
	}
