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
// Name        : TSIPRegEventStateSubscribed.cpp
// Part of     : SIP Profile Agent
// implementation
// Version     : 1.0
//



#include "TSIPRegEventStateSubscribed.h"
#include "sipregeventcontext.h"

// ----------------------------------------------------------------------------
// TSIPRegEventStateSubscribed::TSIPRegEventStateSubscribed
// ----------------------------------------------------------------------------
//
TSIPRegEventStateSubscribed::TSIPRegEventStateSubscribed(
    MSIPRegEventContext& aContext)
    : TSIPRegEventSubscriptionStateBase(aContext)
    {
    }
		
// ----------------------------------------------------------------------------
// TSIPRegEventStateSubscribed::RefreshL
// ----------------------------------------------------------------------------
//
void TSIPRegEventStateSubscribed::RefreshL()
    {
    iContext.DoRefreshL();
    }

// ----------------------------------------------------------------------------
// TSIPRegEventStateSubscribed::ResponseReceivedL
// ----------------------------------------------------------------------------
//		
void TSIPRegEventStateSubscribed::ResponseReceivedL(
    TUint aStatusCode,
    TUint aRetryAfter)
    {
    if (aStatusCode >= 300)
        {
        iContext.ChangeState(MSIPRegEventContext::ENotSubscribed);
        TUint retryAfter = 0;
        if (aStatusCode == 503)
            {
            retryAfter = aRetryAfter;
            }
        iContext.ReSubscribeAfterL(retryAfter);
        iContext.ChangeState(MSIPRegEventContext::EReSubscribing);
        }
    }
    
// ----------------------------------------------------------------------------
// TSIPRegEventStateSubscribed::ErrorOccuredL
// ----------------------------------------------------------------------------
//    
void TSIPRegEventStateSubscribed::ErrorOccuredL()
    {
    iContext.ChangeState(MSIPRegEventContext::ENotSubscribed);
    iContext.ReSubscribeAfterL(0);
    iContext.ChangeState(MSIPRegEventContext::EReSubscribing);
    }
