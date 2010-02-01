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
// Name          : CSipAllowEventsStrategy.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSipAllowEventsStrategy.h"
#include "MSipClients.h"
#include "MSipClient.h"
#include "sipresponse.h"
#include "siprequest.h"
#include "sipalloweventsheader.h"
#include "sipeventheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "SIPCRLogs.h"

const TUint K489ResponseCode = 489;


// ----------------------------------------------------------------------------
// CSipAllowEventsStrategy::NewL
// ----------------------------------------------------------------------------
//
CSipAllowEventsStrategy* CSipAllowEventsStrategy::NewL(
    MSipClients& aSipClients,
    CSipHeaderStrategyBase* aNextStrategy,
    CSipHeaderStrategyBase* aNextStrategy2 )
	{
	CSipAllowEventsStrategy* self = 
		CSipAllowEventsStrategy::NewLC( aSipClients,
										aNextStrategy,
										aNextStrategy2 );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSipAllowEventsStrategy::NewLC
// ----------------------------------------------------------------------------
//
CSipAllowEventsStrategy* CSipAllowEventsStrategy::NewLC(
    MSipClients& aSipClients,
    CSipHeaderStrategyBase* aNextStrategy,
    CSipHeaderStrategyBase* aNextStrategy2 )
	{
	CSipAllowEventsStrategy* self = 
		new(ELeave)CSipAllowEventsStrategy( aSipClients,
											aNextStrategy,
											aNextStrategy2 );
	CleanupStack::PushL( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSipAllowEventsStrategy::CSipAllowEventsStrategy
// ----------------------------------------------------------------------------
//
CSipAllowEventsStrategy::CSipAllowEventsStrategy(
    MSipClients& aSipClients,
    CSipHeaderStrategyBase* aNextStrategy,
    CSipHeaderStrategyBase* aNextStrategy2 ) 
 : CSipHeaderStrategyBase( aSipClients,aNextStrategy,aNextStrategy2 )
	{
    iEventHeaderName = SIPStrings::StringF( SipStrConsts::EEventHeader );
    iAllowEventsHeaderName = 
        SIPStrings::StringF( SipStrConsts::EAllowEventsHeader );
	}

// ----------------------------------------------------------------------------
// CSipAllowEventsStrategy::~CSipAllowEventsStrategy
// ----------------------------------------------------------------------------
//
CSipAllowEventsStrategy::~CSipAllowEventsStrategy ()
	{
	}

// ----------------------------------------------------------------------------
// CSipAllowEventsStrategy::ApplyL
// ----------------------------------------------------------------------------
//
CSIPResponse* CSipAllowEventsStrategy::ApplyL(
    CSIPRequest& aRequest, 
    RArray<TUid>& aUids,
    TBool& aContinueSearch,
    CSIPClientResolver2& aClientResolver2 )
	{
	SIP_CR_LOG("CSipAllowEventsStrategy::ApplyL")
	CSIPResponse* response = NULL;
    // If the request does not contain Event-header skip the strategy.
	if ( aRequest.HasHeader( iEventHeaderName ) )
        {
        response = MatchClientsL( aRequest,aUids );
        if ( ( response && response->ResponseCode() == K489ResponseCode ) &&
        	    iNextStrategy2 )
        	{
        	delete response;
        	response = NULL;
        	response = iNextStrategy2->ApplyL( aRequest,aUids,aContinueSearch,
        									   aClientResolver2 );
        	}
        }
    else
        {
        if ( iNextStrategy )
            {
            response = iNextStrategy->ApplyL( aRequest,aUids,aContinueSearch,
            								  aClientResolver2 );
            }    
        }
    return response;
	}

// ----------------------------------------------------------------------------
// CSipAllowEventsStrategy::CreateResponseL
// ----------------------------------------------------------------------------
//
CSIPResponse* CSipAllowEventsStrategy::CreateResponseL()
    {
    return CSIPResponse::NewL( K489ResponseCode,
        SIPStrings::StringF( SipStrConsts::EPhraseBadEvent ) );
    }

// ----------------------------------------------------------------------------
// CSipAllowEventsStrategy::CompareHeaders
// ----------------------------------------------------------------------------
//
TBool CSipAllowEventsStrategy::CompareHeaders(
    CSIPRequest& aRequest,
    MSipClient& aClient )
	{
	CSIPHeaderBase* header = aRequest.Header( iEventHeaderName,0 );
	if ( header )
	    {
	    CSIPEventHeader* eventHeader = static_cast<CSIPEventHeader*>(header);
	    TPtrC8 eventPackage(eventHeader->EventPackage());
	    RPointerArray<CSIPHeaderBase>& clientHeaders = aClient.SIPHeaders();
	
	    for(TInt i=0; i < clientHeaders.Count(); i++)
		    {
            if (clientHeaders[i]->Name() == iAllowEventsHeaderName)
                {
                CSIPAllowEventsHeader* clientHeader = 
                    static_cast<CSIPAllowEventsHeader*>(clientHeaders[i]);
                if (eventPackage.Compare(clientHeader->EventPackage()) == 0)
                    {
                    return ETrue;
                    }
                }
		    }	    
	    }

	return EFalse;
	}
