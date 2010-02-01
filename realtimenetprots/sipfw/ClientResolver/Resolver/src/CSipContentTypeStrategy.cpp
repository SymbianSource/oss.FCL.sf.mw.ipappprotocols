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
// Name          : CSipContentTypeStrategy.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSipContentTypeStrategy.h"
#include "MSipClients.h"
#include "MSipClient.h"
#include "sipresponse.h"
#include "siprequest.h"
#include "sipcontenttypeheader.h"
#include "sipacceptheader.h"
#include "sipacceptencodingheader.h"
#include "sipacceptlanguageheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "SIPCRLogs.h"

const TInt KResponse415 = 415;


// ----------------------------------------------------------------------------
// CSipContentTypeStrategy::NewL
// ----------------------------------------------------------------------------
//
CSipContentTypeStrategy* CSipContentTypeStrategy::NewL(
    MSipClients& aSipClients,
    CSipHeaderStrategyBase* aNextStrategy)
	{
	CSipContentTypeStrategy* self = 
	    CSipContentTypeStrategy::NewLC(aSipClients,aNextStrategy);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSipContentTypeStrategy::NewLC
// ----------------------------------------------------------------------------
//
CSipContentTypeStrategy* CSipContentTypeStrategy::NewLC(
    MSipClients& aSipClients,
    CSipHeaderStrategyBase* aNextStrategy)
	{
	CSipContentTypeStrategy* self = 
		new(ELeave)CSipContentTypeStrategy(aSipClients,aNextStrategy);
	CleanupStack::PushL(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSipContentTypeStrategy::CSipContentTypeStrategy
// ----------------------------------------------------------------------------
//
CSipContentTypeStrategy::CSipContentTypeStrategy(
    MSipClients& aSipClients,
    CSipHeaderStrategyBase* aNextStrategy) 
 : CSipHeaderStrategyBase(aSipClients,aNextStrategy,NULL),
   iType(KNullDesC8),
   iSubType(KNullDesC8)
	{
	}

// ----------------------------------------------------------------------------
// CSipContentTypeStrategy::~CSipContentTypeStrategy
// ----------------------------------------------------------------------------
//
CSipContentTypeStrategy::~CSipContentTypeStrategy ()
	{
	iSipAcceptHeaders.Reset();
	}

// ----------------------------------------------------------------------------
// CSipContentTypeStrategy::ApplyL
// From CSipHeaderStrategyBase
// ----------------------------------------------------------------------------
//
CSIPResponse* CSipContentTypeStrategy::ApplyL(
    CSIPRequest& aRequest, 
    RArray<TUid>& aUids,
    TBool& aContinueSearch,
    CSIPClientResolver2& aClientResolver2 )
	{
	SIP_CR_LOG("CSipContentTypeStrategy::ApplyL")
	CSIPResponse* response = NULL;
	aContinueSearch = ETrue; // Always continue to SDP strategies
    SaveContentType(aRequest);
    response = MatchClientsL(aRequest,aUids);
    if (iNextStrategy && !response)
        {
        response = iNextStrategy->ApplyL(aRequest,aUids,aContinueSearch,
        													aClientResolver2 ); 
        }
    return response;
	}

// ----------------------------------------------------------------------------
// CSipContentTypeStrategy::CreateResponseL
// From CSipHeaderStrategyBase
// ----------------------------------------------------------------------------
//
CSIPResponse* CSipContentTypeStrategy::CreateResponseL()
	{
	CSIPResponse* response = 
	    CSIPResponse::NewLC(KResponse415,
	        SIPStrings::StringF(SipStrConsts::EPhraseUnsupportedMediaType));
			 
	RemoveDuplicateAcceptHeaders();
	
	CSIPAcceptHeader* acceptHeader;
	
	for (TInt i=0;i<iSipAcceptHeaders.Count();i++)
		{
		acceptHeader = static_cast<CSIPAcceptHeader*>(iSipAcceptHeaders[i]);
		if(acceptHeader->IsEmpty())
			{
			iSipAcceptHeaders.Remove(i);
			}
		else
			{
			CSIPHeaderBase* header = iSipAcceptHeaders[i]->CloneL();
			CleanupStack::PushL(header);
			response->AddHeaderL(header);
			CleanupStack::Pop(header);
			}
		}
		
	iSipAcceptHeaders.Reset();

	CleanupStack::Pop(response);
	return response;
	}

// ----------------------------------------------------------------------------
// CSipContentTypeStrategy::RemoveDuplicateAcceptHeaders
// ----------------------------------------------------------------------------
//
void CSipContentTypeStrategy::RemoveDuplicateAcceptHeaders()
	{
	for (TInt i=0;i<iSipAcceptHeaders.Count();i++)
		{
		CSIPAcceptHeader* acceptHeader =
		    static_cast<CSIPAcceptHeader*>(iSipAcceptHeaders[i]);

		for (TInt j=iSipAcceptHeaders.Count() - 1;j>i;j--)
			{
			CSIPAcceptHeader* acceptHeaderT =
				static_cast<CSIPAcceptHeader*> (iSipAcceptHeaders[j]);

			if(((acceptHeader->MediaType().Compare(
										acceptHeaderT->MediaType()) == 0) &&
				(acceptHeader->MediaSubtype().Compare(
										acceptHeaderT->MediaSubtype()) == 0)))
				{
				iSipAcceptHeaders.Remove(j);
				}
			}
		}
	}

// ----------------------------------------------------------------------------
// CSipContentTypeStrategy::EliminateClientsL
// ----------------------------------------------------------------------------
//
void CSipContentTypeStrategy::EliminateClientsL(
    CSIPRequest& aRequest,
    RArray<TUid>& aUids)
    {	
    TBool clientFound = EFalse;
	iSipAcceptHeaders.Reset();
	for (TInt i=aUids.Count()-1; i>=0; i--)
		{
		MSipClient* client = iSipClients.GetByUID(aUids[i]);
		if (client && !CompareHeaders(aRequest,*client))
			{
			aUids.Remove(i);
			if (!clientFound)
				{
				CollectHeadersL(client);
				}
			}
	    else
	        {
	        clientFound = (client != NULL);
	        }
		}
    }

// ----------------------------------------------------------------------------
// CSipContentTypeStrategy::SaveContentType
// ----------------------------------------------------------------------------
//
void CSipContentTypeStrategy::SaveContentType(CSIPRequest& aRequest)
	{	
	CSIPHeaderBase* header = aRequest.Header(
	    SIPStrings::StringF(SipStrConsts::EContentTypeHeader),0);	
	
	if(header)
		{
        CSIPContentTypeHeader* contentType =
		    static_cast<CSIPContentTypeHeader*>(header);
		iType.Set(contentType->MediaType());
		iSubType.Set(contentType->MediaSubtype());
		iIsEmpty = EFalse;
		}	
    else
        {
 		iType.Set(KNullDesC8);
		iSubType.Set(KNullDesC8);
		iIsEmpty = ETrue;       
        }
	}

// ----------------------------------------------------------------------------
// CSipContentTypeStrategy::CompareHeaders
// ----------------------------------------------------------------------------
//
TBool CSipContentTypeStrategy::CompareHeaders(
    CSIPRequest& /*aRequest*/,
    MSipClient& aClient)
	{
	RPointerArray<CSIPHeaderBase>& headers = aClient.SIPHeaders();
	
	for (TInt i=0; i < headers.Count();i++)
		{
		if (headers[i]->Name() == 
		    SIPStrings::StringF(SipStrConsts::EAcceptHeader))
			{
			CSIPAcceptHeader* acceptHeader =
				static_cast<CSIPAcceptHeader*> (headers[i]);
			
			if (iIsEmpty && acceptHeader->IsEmpty())
				{
				return ETrue;
				}
			else
				{
				if (!iIsEmpty && !acceptHeader->IsEmpty() && 
					((acceptHeader->MediaType()).CompareF(iType) == 0) &&
					((acceptHeader->MediaSubtype()).CompareF(iSubType) == 0))
					{
					return ETrue;
					}
				}
			}
		}
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CSipContentTypeStrategy::CollectHeadersL
// ----------------------------------------------------------------------------
//
void CSipContentTypeStrategy::CollectHeadersL(MSipClient* aSipClient)
	{
	RPointerArray<CSIPHeaderBase>& headers = aSipClient->SIPHeaders();
	
	for(TInt i=0; i < headers.Count();i++)
		{
		if (headers[i]->Name() == 
		    SIPStrings::StringF(SipStrConsts::EAcceptHeader))
			{
			User::LeaveIfError(iSipAcceptHeaders.Append(headers[i]));
			}
		}
	}
