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
// Name          : CSipHeaderStrategyBase.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSipHeaderStrategyBase.h"
#include "MSipClient.h"
#include "MSipClients.h"
#include "siprequest.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcontenttypeheader.h"
#include "CSIPClientResolver2.h"
#include "uricontainer.h"
#include "SIPCRLogs.h"
#include "TSipClient.h"

// ----------------------------------------------------------------------------
// CSipHeaderStrategyBase::~CSipHeaderStrategyBase
// ----------------------------------------------------------------------------
//
CSipHeaderStrategyBase::~CSipHeaderStrategyBase()
	{	
	}

// ----------------------------------------------------------------------------
// CSipHeaderStrategyBase::CSipHeaderStrategyBase
// ----------------------------------------------------------------------------
//
CSipHeaderStrategyBase::CSipHeaderStrategyBase( 
    								MSipClients& aSipClients,
    								CSipHeaderStrategyBase* aNextStrategy,
    								CSipHeaderStrategyBase* aNextStrategy2 )
: iSipClients( aSipClients ),
  iNextStrategy( aNextStrategy ),
  iNextStrategy2( aNextStrategy2 )
	{
	}

// ----------------------------------------------------------------------------
// CSipHeaderStrategyBase::MatchClientsL
// ----------------------------------------------------------------------------
//
CSIPResponse* CSipHeaderStrategyBase::MatchClientsL( 
    								CSIPRequest& aRequest,
    								RArray<TUid>& aUids )
    {
    SIP_CR_LOG("CSipHeaderStrategyBase::MatchClientsL")
    EliminateClientsL( aRequest,aUids );
	if ( aUids.Count() == 0 )
		{
		return CreateResponseL();
		}
	return NULL;
    }

// ----------------------------------------------------------------------------
// CSipHeaderStrategyBase::MatchClients2L
// ----------------------------------------------------------------------------
//
CSIPResponse* CSipHeaderStrategyBase::MatchClients2L(
    									CSIPRequest& aRequest,
    									RArray<TUid>& aUids,
    									TBool& aContinueSearch,
    									CSIPClientResolver2& aClientResolver2 )
    {
    SIP_CR_LOG("CSipHeaderStrategyBase::MatchClients2L")
    CSIPResponse* response = NULL;
  	aClientResolver2.FetchMatchingUidsL( aRequest, *this );
    response = NoMatchL( aRequest,aUids,aContinueSearch,aClientResolver2 );
    if ( !response )
    	{
    	response = MoreThanOneMatchL( aRequest,aUids,aContinueSearch,
    														aClientResolver2 );
    	}
    return response;
    }

// ----------------------------------------------------------------------------
// CSipHeaderStrategyBase::MatchL
// ----------------------------------------------------------------------------
//
TBool CSipHeaderStrategyBase::MatchL( CSIPResolvedClient2& aClient,
									  CSIPRequest& aRequest, 
    								  TUid& aUid )
	{
	CUri8* uri8 = ConvertRequestUriL( aRequest );
	CleanupStack::PushL( uri8 );
	TBool match( EFalse );
	match = MatchResolvedClientL( aClient,aRequest,aUid,*uri8 );
	CleanupStack::PopAndDestroy( uri8 );
	return match;
	}

// ----------------------------------------------------------------------------
// CSipHeaderStrategyBase::ConvertRequestUriL
// ----------------------------------------------------------------------------
//
CUri8* CSipHeaderStrategyBase::ConvertRequestUriL( CSIPRequest& aRequest )
    {
    CUri8* uri8 = NULL;
    __ASSERT_ALWAYS(aRequest.RequestURI(), User::Leave(KErrCorrupt));
    HBufC8* temp = aRequest.RequestURI()->ToTextL();
    CleanupStack::PushL( temp );
    TUriParser8 parser;
    User::LeaveIfError( parser.Parse( *temp) );
    uri8 = CUri8::NewL( parser );
    CleanupStack::PopAndDestroy( temp );
    return uri8;	
    }

// ----------------------------------------------------------------------------
// CSipHeaderStrategyBase::NextStrategyL
// ----------------------------------------------------------------------------
//
CSIPResponse* CSipHeaderStrategyBase::NextStrategyL( CSIPRequest& aRequest, 
    									RArray<TUid>& aUids,
    									TBool& aContinueSearch,
    									CSIPClientResolver2& aClientResolver2 )
	{
	SIP_CR_LOG("CSipHeaderStrategyBase::NextStrategyL")
	CSIPResponse* response = NULL;
	if ( iNextStrategy )
    	{
	    response = iNextStrategy->ApplyL( aRequest,aUids,aContinueSearch,
	    												aClientResolver2 );
	    }
	return response;
	}

// ----------------------------------------------------------------------------
// CSipHeaderStrategyBase::NextStrategy2L
// ----------------------------------------------------------------------------
//
CSIPResponse* CSipHeaderStrategyBase::NextStrategy2L( 
										CSIPRequest& aRequest, 
    									RArray<TUid>& aUids,
    									TBool& aContinueSearch,
    									CSIPClientResolver2& aClientResolver2 )
	{
	SIP_CR_LOG("CSipHeaderStrategyBase::NextStrategy2L")
	CSIPResponse* response = NULL;
	if ( iNextStrategy2 )
        {
		response = iNextStrategy2->ApplyL( aRequest,aUids,aContinueSearch,
															aClientResolver2 );
	    }
	return response;
	}

// ----------------------------------------------------------------------------
// CSipHeaderStrategyBase::NoMatchL
// ----------------------------------------------------------------------------
//
CSIPResponse* CSipHeaderStrategyBase::NoMatchL( 
										CSIPRequest& aRequest, 
    									RArray<TUid>& aUids,
    									TBool& aContinueSearch,
    									CSIPClientResolver2& aClientResolver2 )
	{
	SIP_CR_LOG("CSipHeaderStrategyBase::NoMatchL")
	CSIPResponse* response = NULL;
	if ( !aClientResolver2.ClientsData().Count() )
        {
	    response = NextStrategy2L( aRequest,aUids,aContinueSearch,
	    													aClientResolver2 );
        }
	return response;
	}

// ----------------------------------------------------------------------------
// CSipHeaderStrategyBase::MoreThanOneMatchL
// ----------------------------------------------------------------------------
//
CSIPResponse* CSipHeaderStrategyBase::MoreThanOneMatchL( CSIPRequest& aRequest, 
    									RArray<TUid>& aUids,
    									TBool& aContinueSearch,
    									CSIPClientResolver2& aClientResolver2 )
	{
	SIP_CR_LOG("CSipHeaderStrategyBase::MoreThanOneMatchL")
	CSIPResponse* response = NULL;
	if ( iNextStrategy && aClientResolver2.ClientsData().Count() > 1 )
        {
		response = NextStrategyL( aRequest,aUids,aContinueSearch,
															aClientResolver2 );
        } 
	return response;
	}

// ----------------------------------------------------------------------------
// CSipHeaderStrategyBase::MatchResolvedClientsL
// ----------------------------------------------------------------------------
//
TBool CSipHeaderStrategyBase::MatchResolvedClientL( 
									CSIPResolvedClient2& /*aClient*/,
									CSIPRequest& /*aRequest*/, 
    								TUid& /*aUids*/,
        							const CUri8& /*aUri8*/ )
	{
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CSipHeaderStrategyBase::EliminateClientsL
// ----------------------------------------------------------------------------
//
void CSipHeaderStrategyBase::EliminateClientsL(
    CSIPRequest& aRequest,
    RArray<TUid>& aUids )
    {
	for ( TInt i=0; i < aUids.Count();i++ )
		{
		MSipClient* client = iSipClients.GetByUID( aUids[i] );
		if ( client && !CompareHeaders( aRequest,*client) )
			{
			aUids.Remove( i );
			i--;
			}	
		}
    }
    
// ----------------------------------------------------------------------------
// CSipHeaderStrategyBase::CreateResponseL
// ----------------------------------------------------------------------------
//
CSIPResponse* CSipHeaderStrategyBase::CreateResponseL()
    {
    return NULL;
    }

// ----------------------------------------------------------------------------
// CSipHeaderStrategyBase::CompareHeaders
// ----------------------------------------------------------------------------
//
TBool CSipHeaderStrategyBase::CompareHeaders(
    CSIPRequest& /*aRequest*/,
    MSipClient& /*aClient*/)
	{
	// Not implemented for this sub-class
	return EFalse;
	}    
    
// ----------------------------------------------------------------------------
// CSipHeaderStrategyBase::ContentType
// ----------------------------------------------------------------------------
//
CSIPContentTypeHeader* CSipHeaderStrategyBase::ContentType(
													CSIPRequest& aRequest )
    {
	CSIPContentTypeHeader* contentType = NULL;	
	CSIPHeaderBase* header = aRequest.Header(
	    SIPStrings::StringF( SipStrConsts::EContentTypeHeader ),0 );	
	if ( header )
		{
        contentType = static_cast<CSIPContentTypeHeader*>( header );
		}	
	return contentType;
    }
