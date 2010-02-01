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
// Name          : CSipAcceptContactStrategy2.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSipAcceptContactStrategy2.h"
#include "MSipClients.h"
#include "siprequest.h"
#include "sipacceptcontactheader.h"
#include "CSIPClientResolver2.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "SIPCRLogs.h"

// ----------------------------------------------------------------------------
// CSipAcceptContactStrategy2::NewL
// ----------------------------------------------------------------------------
//
CSipAcceptContactStrategy2* CSipAcceptContactStrategy2::NewL( 
    								   MSipClients& aSipClients,
    								   CSipHeaderStrategyBase* aNextStrategy,
    								   CSipHeaderStrategyBase* aNextStrategy2 )
	{
	CSipAcceptContactStrategy2* self = 
		CSipAcceptContactStrategy2::NewLC( aSipClients,
										   aNextStrategy,
										   aNextStrategy2 );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSipAcceptContactStrategy2::NewLC
// ----------------------------------------------------------------------------
//
CSipAcceptContactStrategy2* CSipAcceptContactStrategy2::NewLC( 
    								   MSipClients& aSipClients,
    								   CSipHeaderStrategyBase* aNextStrategy,
    								   CSipHeaderStrategyBase* aNextStrategy2 )
	{
	CSipAcceptContactStrategy2* self = 
		new( ELeave )CSipAcceptContactStrategy2( aSipClients,aNextStrategy,
											aNextStrategy2 );
	CleanupStack::PushL( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSipAcceptContactStrategy2::CSipAcceptContactStrategy2
// ----------------------------------------------------------------------------
//
CSipAcceptContactStrategy2::CSipAcceptContactStrategy2(
    MSipClients& aSipClients,
    CSipHeaderStrategyBase* aNextStrategy,
    CSipHeaderStrategyBase* aNextStrategy2) 
 : CSipHeaderStrategyBase( aSipClients,aNextStrategy,aNextStrategy2 )
	{
	iAcceptContactHeaderName = 
	    SIPStrings::StringF( SipStrConsts::EAcceptContactHeader );
	}

// ----------------------------------------------------------------------------
// CSipAcceptContactStrategy2::~CSipAcceptContactStrategy2
// ----------------------------------------------------------------------------
//
CSipAcceptContactStrategy2::~CSipAcceptContactStrategy2 ()
	{
	}

// ----------------------------------------------------------------------------
// CSipAcceptContactStrategy2::ApplyL
// ----------------------------------------------------------------------------
//
CSIPResponse* 
	CSipAcceptContactStrategy2::ApplyL( CSIPRequest& aRequest, 
    									RArray<TUid>& aUids,
    									TBool& aContinueSearch,
    									CSIPClientResolver2& aClientResolver2 )
	{
	SIP_CR_LOG("CSipAcceptContactStrategy2::ApplyL")
	CSIPResponse* response = NULL;
	// The strategy is applied only for requests that contain Accept-Contact
	if ( aRequest.HasHeader( iAcceptContactHeaderName ) )
        {
        SIP_CR_LOG("CSipAcceptContactStrategy2::ApplyL Accept-Contact")
        response = MatchClients2L( aRequest,aUids,aContinueSearch,
        												aClientResolver2 );
        }
    else
        {
        SIP_CR_LOG("CSipAcceptContactStrategy2::ApplyL No Accept-Contact")
         response =	NextStrategyL( aRequest,aUids,aContinueSearch,
         												aClientResolver2 );
        }
    return response;
	}

// ----------------------------------------------------------------------------
// CSipAcceptContactStrategy2::MatchResolvedClientsL
// ----------------------------------------------------------------------------
//
TBool CSipAcceptContactStrategy2::MatchResolvedClientL( 
									CSIPResolvedClient2& aClient,
									CSIPRequest& aRequest,
    								TUid& aClientUid,
    								const CUri8& aUri8 )
	{
	SIP_CR_LOG("CSipAcceptContactStrategy2::MatchResolvedClientL")
	return aClient.MatchAcceptContactsL( aRequest.Method(),
										 aUri8,
          								 aRequest.AllHeadersL(),
          								 aRequest.Content(), 
          								 ContentType(aRequest), 
          								 aClientUid );
	}



