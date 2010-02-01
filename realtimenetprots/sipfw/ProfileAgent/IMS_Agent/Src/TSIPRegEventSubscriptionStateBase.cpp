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
// Name        : TSIPRegEventSubscriptionStateBase.cpp
// Part of     : SIP Profile Agent
// implementation
// Version     : 1.0
//



#include "TSIPRegEventSubscriptionStateBase.h"
#include "sipregeventcontext.h"

// ----------------------------------------------------------------------------
// TSIPRegEventSubscriptionStateBase::TSIPRegEventSubscriptionStateBase
// ----------------------------------------------------------------------------
//
TSIPRegEventSubscriptionStateBase::TSIPRegEventSubscriptionStateBase(
    MSIPRegEventContext& aContext)
    : iContext(aContext)
    {
    }
    
// ----------------------------------------------------------------------------
// TSIPRegEventSubscriptionStateBase::~TSIPRegEventSubscriptionStateBase
// ----------------------------------------------------------------------------
//		
TSIPRegEventSubscriptionStateBase::~TSIPRegEventSubscriptionStateBase()
    {
    }

// ----------------------------------------------------------------------------
// TSIPRegEventSubscriptionStateBase::SubscribeL
// ----------------------------------------------------------------------------
//
void TSIPRegEventSubscriptionStateBase::SubscribeL()
    {
    // By default NOP.
    }
		
// ----------------------------------------------------------------------------
// TSIPRegEventSubscriptionStateBase::RefreshL
// ----------------------------------------------------------------------------
//
void TSIPRegEventSubscriptionStateBase::RefreshL()
    {
    // By default NOP.
    }

// ----------------------------------------------------------------------------
// TSIPRegEventSubscriptionStateBase::ResponseReceivedL
// ----------------------------------------------------------------------------
//    
void TSIPRegEventSubscriptionStateBase::ResponseReceivedL(
    TUint /*aStatusCode*/,
    TUint /*aRetryAfter*/)
    {
    // By default NOP.
    }
    
// ----------------------------------------------------------------------------
// TSIPRegEventSubscriptionStateBase::ErrorOccuredL
// ----------------------------------------------------------------------------
//    
void TSIPRegEventSubscriptionStateBase::ErrorOccuredL()
    {
    iContext.SubscriptionFailedL();
    }
