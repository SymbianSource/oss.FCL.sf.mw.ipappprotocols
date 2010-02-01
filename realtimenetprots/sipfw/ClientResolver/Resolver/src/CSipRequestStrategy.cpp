// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSipRequestStrategy.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSipRequestStrategy.h"
#include "MSipClients.h"
#include "siprequest.h"
#include "CSIPClientResolver2.h"
#include "SIPCRLogs.h"

// ----------------------------------------------------------------------------
// CSipRequestStrategy::NewL
// ----------------------------------------------------------------------------
//
CSipRequestStrategy* CSipRequestStrategy::NewL(
    MSipClients& aSipClients,
    CSipHeaderStrategyBase* aNextStrategy,
    CSipHeaderStrategyBase* aNextStrategy2)
	{
	CSipRequestStrategy* self = 
		CSipRequestStrategy::NewLC(aSipClients,aNextStrategy,
    								aNextStrategy2);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSipRequestStrategy::NewLC
// ----------------------------------------------------------------------------
//
CSipRequestStrategy* CSipRequestStrategy::NewLC(
    MSipClients& aSipClients,
    CSipHeaderStrategyBase* aNextStrategy,
    CSipHeaderStrategyBase* aNextStrategy2)
	{
	CSipRequestStrategy* self = 
		new(ELeave)CSipRequestStrategy(aSipClients,aNextStrategy,
    									aNextStrategy2);
	CleanupStack::PushL(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSipRequestStrategy::CSipRequestStrategy
// ----------------------------------------------------------------------------
//
CSipRequestStrategy::CSipRequestStrategy(
    MSipClients& aSipClients,
    CSipHeaderStrategyBase* aNextStrategy,
    CSipHeaderStrategyBase* aNextStrategy2) 
 : CSipHeaderStrategyBase(aSipClients,aNextStrategy,
    						aNextStrategy2)
	{
	}

// ----------------------------------------------------------------------------
// CSipRequestStrategy::~CSipRequestStrategy
// ----------------------------------------------------------------------------
//
CSipRequestStrategy::~CSipRequestStrategy()
	{
	}

// ----------------------------------------------------------------------------
// CSipRequestStrategy::ApplyL
// ----------------------------------------------------------------------------
//
CSIPResponse* 
	CSipRequestStrategy::ApplyL( CSIPRequest& aRequest, 
    							 RArray<TUid>& aUids,
    							 TBool& aContinueSearch,
    							 CSIPClientResolver2& aClientResolver2 )
	{
	SIP_CR_LOG("CSipRequestStrategy::ApplyL")
    return MatchClients2L( aRequest,aUids,aContinueSearch,aClientResolver2 );
	}
	
// ----------------------------------------------------------------------------
// CSipRequestStrategy::MoreThanOneMatchL
// ----------------------------------------------------------------------------
//
CSIPResponse* CSipRequestStrategy::MoreThanOneMatchL( CSIPRequest& /*aRequest*/, 
    								RArray<TUid>& /*aUids*/,
    								TBool& /*aContinueSearch*/,
    								CSIPClientResolver2& /*aClientResolver2*/ )
	{
	return NULL;
	}	

// ----------------------------------------------------------------------------
// CSipRequestStrategy::MatchResolvedClientsL
// ----------------------------------------------------------------------------
//
TBool CSipRequestStrategy::MatchResolvedClientL( CSIPResolvedClient2& aClient,
										CSIPRequest& aRequest,
    									TUid& aClientUid,
    									const CUri8& aUri8 )
	{
	SIP_CR_LOG("CSipRequestStrategy::MatchResolvedClientL")
	return aClient.MatchRequestL( aRequest.Method(),
          						  aUri8,
          						  aRequest.AllHeadersL(),
          						  aRequest.Content(), 
          						  ContentType(aRequest), 
          						  aClientUid );
	}

