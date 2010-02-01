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
// Name          : UserAgentTimer.cpp
// Part of       : TransactionUser
// Version       : SIP/4.0
//



#include "SipAssert.h"
#include "UserAgentTimer.h"
#include "InviteUAS.h"
#include "InviteUAC.h"

// -----------------------------------------------------------------------------
// CUserAgentTimer::CUserAgentTimer
// -----------------------------------------------------------------------------
//
CUserAgentTimer::CUserAgentTimer(MTimerManager& aTimerMgr,
								 MExpirationHandler* aObserver,
								 TUint32 aDuration) :
	CTimerBase(aTimerMgr, aObserver, aDuration)
	{	
	}

// -----------------------------------------------------------------------------
// CTimerStopUA::NewL
// -----------------------------------------------------------------------------
//
CTimerStopUA* CTimerStopUA::NewL(MTimerManager& aTimerMgr,
							     MExpirationHandler* aObserver,
								 TUint32 aDuration,
								 TInt aReason)
	{    
    __SIP_ASSERT_LEAVE(aObserver, KErrArgument);

	CTimerStopUA* self = new (ELeave) CTimerStopUA(aTimerMgr,
												   aObserver,
												   aDuration,
												   aReason);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// -----------------------------------------------------------------------------
// CTimerStopUA::CTimerStopUA
// -----------------------------------------------------------------------------
//
CTimerStopUA::CTimerStopUA(MTimerManager& aTimerMgr,
						   MExpirationHandler* aObserver,
						   TUint32 aDuration,
						   TInt aReason) : 
	CUserAgentTimer(aTimerMgr, aObserver, aDuration),
	iReason(aReason)
	{
	}

// -----------------------------------------------------------------------------
// CTimerStopUA::ExpiredL
// CUserAgent::DeleteTimer() is not called, as it will delete this object and
// the execution is currently inside it.
// -----------------------------------------------------------------------------
//
void CTimerStopUA::ExpiredL(CUserAgent* aUserAgent, TTimerId aTimerId)
	{
    __SIP_ASSERT_LEAVE(aUserAgent, KErrArgument);
    __SIP_ASSERT_LEAVE(aTimerId == iId, KErrArgument);

	aUserAgent->Stop(iReason);
	}

// -----------------------------------------------------------------------------
// CTimerRetransmit2xx::NewL
// -----------------------------------------------------------------------------
//
CTimerRetransmit2xx* CTimerRetransmit2xx::NewL(MTimerManager& aTimerMgr,
											   MExpirationHandler* aObserver,
											   TUint32 aDuration)
	{
	CTimerRetransmit2xx* self = new (ELeave) CTimerRetransmit2xx(aTimerMgr,
																 aObserver,
																 aDuration);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CTimerRetransmit2xx::CTimerRetransmit2xx
// -----------------------------------------------------------------------------
//
CTimerRetransmit2xx::CTimerRetransmit2xx(MTimerManager& aTimerMgr,
										 MExpirationHandler* aObserver,
										 TUint32 aDuration) :
	CUserAgentTimer(aTimerMgr, aObserver, aDuration)
	{
	}

// -----------------------------------------------------------------------------
// CTimerRetransmit2xx::ExpiredL
// -----------------------------------------------------------------------------
//
void CTimerRetransmit2xx::ExpiredL(CUserAgent* aUserAgent, TTimerId aTimerId)
	{
    __SIP_ASSERT_LEAVE(aUserAgent, KErrArgument);
    __SIP_ASSERT_LEAVE(aTimerId == iId, KErrArgument);
	
    CInviteUAS::Ptr(*aUserAgent).Retransmit2xxL();
	}

// -----------------------------------------------------------------------------
// CTimerOwnerResolver::NewL
// -----------------------------------------------------------------------------
//
CTimerOwnerResolver* CTimerOwnerResolver::NewL(MTimerManager& aTimerMgr,
											   MExpirationHandler* aObserver,
											   TUint32 aDuration)
	{
	CTimerOwnerResolver* self = new (ELeave) CTimerOwnerResolver(aTimerMgr,
															     aObserver,
															     aDuration);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CTimerOwnerResolver::CTimerOwnerResolver
// -----------------------------------------------------------------------------
//
CTimerOwnerResolver::CTimerOwnerResolver(MTimerManager& aTimerMgr,
									     MExpirationHandler* aObserver,
									     TUint32 aDuration) :
	CUserAgentTimer(aTimerMgr, aObserver, aDuration)
	{
	}

// -----------------------------------------------------------------------------
// CTimerOwnerResolver::ExpiredL
// -----------------------------------------------------------------------------
//
void CTimerOwnerResolver::ExpiredL(CUserAgent* aUserAgent, TTimerId aTimerId)
	{
    __SIP_ASSERT_LEAVE(aUserAgent, KErrArgument);
    __SIP_ASSERT_LEAVE(aTimerId == iId, KErrArgument);

    static_cast<CUserAgentServer*>(aUserAgent)->RequestRouterErrorL();
	}
	
// -----------------------------------------------------------------------------
// CTimerAsyncSIPSec::NewL
// -----------------------------------------------------------------------------
//
CTimerAsyncSIPSec* CTimerAsyncSIPSec::NewL(MTimerManager& aTimerMgr,
				 					       MExpirationHandler* aObserver)
	{
	CTimerAsyncSIPSec* self = new (ELeave) CTimerAsyncSIPSec(aTimerMgr,
															 aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CTimerAsyncSIPSec::CTimerAsyncSIPSec
// -----------------------------------------------------------------------------
//
CTimerAsyncSIPSec::CTimerAsyncSIPSec(MTimerManager& aTimerMgr,
									 MExpirationHandler* aObserver) :
	CUserAgentTimer(aTimerMgr, aObserver, 0)
	{
	}

// -----------------------------------------------------------------------------
// CTimerAsyncSIPSec::ExpiredL
// -----------------------------------------------------------------------------
//
void CTimerAsyncSIPSec::ExpiredL(CUserAgent* aUserAgent, TTimerId aTimerId)
	{
    __SIP_ASSERT_LEAVE(aUserAgent, KErrArgument);
    __SIP_ASSERT_LEAVE(aTimerId == iId, KErrArgument);

	CInviteUAC::Ptr(*aUserAgent).SIPSecTimerExpiredL();
	}
