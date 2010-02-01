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
// Name        : TSIPRegEventStateReSubscribing.cpp
// Part of     : SIP Profile Agent
// implementation
// Version     : 1.0
//



#include "TSIPRegEventStateReSubscribing.h"
#include "sipregeventcontext.h"

// ----------------------------------------------------------------------------
// TSIPRegEventStateReSubscribing::TSIPRegEventStateReSubscribing
// ----------------------------------------------------------------------------
//
TSIPRegEventStateReSubscribing::TSIPRegEventStateReSubscribing(
    MSIPRegEventContext& aContext)
    : TSIPRegEventSubscriptionStateBase(aContext)
    {
    }

// ----------------------------------------------------------------------------
// TSIPRegEventStateReSubscribing::ResponseReceivedL
// ----------------------------------------------------------------------------
//		
void TSIPRegEventStateReSubscribing::ResponseReceivedL(
    TUint aStatusCode,
    TUint /*aRetryAfter*/)
    {
    if (aStatusCode >= 200)
        {
        if (aStatusCode < 300)
            {
            iContext.ChangeState(MSIPRegEventContext::ESubscribed);
            }
        else if (aStatusCode == 405 || 
                 aStatusCode == 482 || 
                 aStatusCode == 489)
            {
            iContext.ChangeState(MSIPRegEventContext::ENotSubscribed);
            iContext.RegEventNotSupportedByNetworkL();
            }
        else
            {
            iContext.ChangeState(MSIPRegEventContext::ENotSubscribed);
            iContext.ReRegister();
            }
        }
    }
