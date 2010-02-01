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
// Name          : CSipAllowEventsStrategy2.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSipAllowEventsStrategy2.h"
#include "MSipClients.h"
#include "siprequest.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "CSIPClientResolver2.h"
#include "uricontainer.h"
#include "SIPCRLogs.h"

// ----------------------------------------------------------------------------
// CSipAllowEventsStrategy2::NewL
// ----------------------------------------------------------------------------
//
CSipAllowEventsStrategy2* CSipAllowEventsStrategy2::NewL( 
    								   MSipClients& aSipClients,
    								   CSipHeaderStrategyBase* aNextStrategy,
    								   CSipHeaderStrategyBase* aNextStrategy2 )
	{
	CSipAllowEventsStrategy2* self = 
		CSipAllowEventsStrategy2::NewLC( aSipClients,
										 aNextStrategy,
									     aNextStrategy2 );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSipAllowEventsStrategy2::NewLC
// ----------------------------------------------------------------------------
//
CSipAllowEventsStrategy2* CSipAllowEventsStrategy2::NewLC(
    								   MSipClients& aSipClients,
    								   CSipHeaderStrategyBase* aNextStrategy,
    								   CSipHeaderStrategyBase* aNextStrategy2 )
	{
	CSipAllowEventsStrategy2* self = 
		new( ELeave )CSipAllowEventsStrategy2( aSipClients,
											   aNextStrategy,
									           aNextStrategy2 );
	CleanupStack::PushL( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSipAllowEventsStrategy2::CSipAllowEventsStrategy2
// ----------------------------------------------------------------------------
//
CSipAllowEventsStrategy2::CSipAllowEventsStrategy2(
    MSipClients& aSipClients,
    CSipHeaderStrategyBase* aNextStrategy,
    CSipHeaderStrategyBase* aNextStrategy2) 
 : CSipHeaderStrategyBase( aSipClients,aNextStrategy,aNextStrategy2 )
	{
    iEventHeaderName = SIPStrings::StringF( SipStrConsts::EEventHeader );
	}

// ----------------------------------------------------------------------------
// CSipAllowEventsStrategy2::~CSipAllowEventsStrategy2
// ----------------------------------------------------------------------------
//
CSipAllowEventsStrategy2::~CSipAllowEventsStrategy2 ()
	{
	}

// ----------------------------------------------------------------------------
// CSipAllowEventsStrategy2::ApplyL
// ----------------------------------------------------------------------------
//
CSIPResponse* 
	CSipAllowEventsStrategy2::ApplyL( CSIPRequest& aRequest, 
    								  RArray<TUid>& aUids,
    								  TBool& aContinueSearch,
    								  CSIPClientResolver2& aClientResolver2 )
	{
	SIP_CR_LOG("CSipAllowEventsStrategy2::ApplyL")
	CSIPResponse* response = NULL;
    // If the request does not contain Event-header skip the strategy.
	if ( aRequest.HasHeader( iEventHeaderName ) )
		{
		SIP_CR_LOG("CSipAllowEventsStrategy2::ApplyL Event-header")
        response = MatchClients2L( aRequest,aUids,aContinueSearch,
        											 aClientResolver2 );
        }
    else
        {
        SIP_CR_LOG("CSipAllowEventsStrategy2::ApplyL No Event-header")
        response =	NextStrategyL( aRequest,aUids,aContinueSearch,
        											 aClientResolver2 );   
        }
    return response;
	}
	
// ----------------------------------------------------------------------------
// CSipAllowEventsStrategy2::MatchResolvedClientsL
// ----------------------------------------------------------------------------
//
TBool CSipAllowEventsStrategy2::MatchResolvedClientL( 
										CSIPResolvedClient2& aClient,
										CSIPRequest& aRequest,
    									TUid& aClientUid,
    									const CUri8& aUri8 )
	{
	SIP_CR_LOG("CSipAllowEventsStrategy2::MatchResolvedClientL")
	return aClient.MatchEventL( aRequest.Method(),
          						aUri8,
          						aRequest.AllHeadersL(),
          						aRequest.Content(), 
          						ContentType(aRequest), 
          						aClientUid );
	}	
	

