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
// Name          : CSIPRequestHandler.cpp
// Part of       : SIP Request Handler
// Version       : SIP/4.0 
//



#include "CSIPRequestHandler.h"
#include "SipRequestHandlerInitParams.h"
#include "SipRequestHandlerObserver.h"
#include "sipclientdiscovery.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipheaderbase.h"
#include "sipcontenttypeheader.h"
#include "uricontainer.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

// -----------------------------------------------------------------------------
// CSIPRequestHandler::NewL
// -----------------------------------------------------------------------------
//
CSIPRequestHandler* CSIPRequestHandler::NewL (TAny* aInitParams)
	{
	__ASSERT_ALWAYS (aInitParams != NULL, User::Leave(KErrArgument));
	TSIPRequestHandlerInitParams* params =
		static_cast<TSIPRequestHandlerInitParams*>(aInitParams);
	MSIPRequestHandlerObserver* observer = params->iObserver;
	__ASSERT_ALWAYS (observer != NULL, User::Leave(KErrArgument));
    CSIPRequestHandler* self = new(ELeave)CSIPRequestHandler(*observer);
    CleanupStack::PushL(self);
    self->ConstructL(params->iUid);
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPRequestHandler::CSIPRequestHandler
// -----------------------------------------------------------------------------
//
CSIPRequestHandler::CSIPRequestHandler (MSIPRequestHandlerObserver& aObserver)
 : CSIPRequestHandlerBase(aObserver)
	{
	iContentTypeHeaderName = 
		SIPStrings::StringF(SipStrConsts::EContentTypeHeader);
	}

// -----------------------------------------------------------------------------
// CSIPRequestHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPRequestHandler::ConstructL (TUid aUid)
    {
	iSIPClientDiscovery = CSIPClientDiscovery::NewL(*this,aUid);
    }

// -----------------------------------------------------------------------------
// CSIPRequestHandler::~CSIPRequestHandler
// -----------------------------------------------------------------------------
//
CSIPRequestHandler::~CSIPRequestHandler()
	{
	delete iSIPClientDiscovery;
	}

// -----------------------------------------------------------------------------
// CSIPRequestHandler::RegisterL
// From CSIPRequestHandlerBase
// -----------------------------------------------------------------------------
//
void CSIPRequestHandler::RegisterL(TUid aChannel)
	{
	iSIPClientDiscovery->RegisterL(aChannel);
	}

// -----------------------------------------------------------------------------
// CSIPRequestHandler::Deregister
// From CSIPRequestHandlerBase
// -----------------------------------------------------------------------------
//
TInt CSIPRequestHandler::Deregister(TUid aChannel)
	{
	return iSIPClientDiscovery->Deregister(aChannel);
	}

// -----------------------------------------------------------------------------
// CSIPRequestHandler::ChannelL
// From CSIPRequestHandlerBase
// -----------------------------------------------------------------------------
//
TUint32 CSIPRequestHandler::ChannelL(CSIPRequest& aSIPRequest)
	{
	CURIContainer* requestURI = aSIPRequest.RequestURI();
	__ASSERT_ALWAYS(requestURI != NULL, User::Leave(KErrArgument));
	HBufC8* requestURIText = requestURI->ToTextL();
	CleanupStack::PushL(requestURIText);
	
	RPointerArray<CSIPHeaderBase> headers = aSIPRequest.AllHeadersL();
	const CSIPContentTypeHeader* contentTypeHeader = NULL;
	
	for (TInt i=headers.Count()-1; i >= 0; i--)
		{
		CSIPHeaderBase* header = headers[i];
		if (header->Name() == iContentTypeHeaderName)
			{
			contentTypeHeader = static_cast<CSIPContentTypeHeader*>(header);
			headers.Remove(i);
			}
		}
		
	TUint32 requestId = iSIPClientDiscovery->ChannelL(aSIPRequest.Method(),
	                                                  *requestURIText,
	                                                  headers,
	                                                  aSIPRequest.Content(),
	                                                  contentTypeHeader);
	
	CleanupStack::PopAndDestroy(requestURIText);

	return requestId;
	}

// -----------------------------------------------------------------------------
// CSIPRequestHandler::Cancel
// From CSIPRequestHandlerBase
// -----------------------------------------------------------------------------
//		
void CSIPRequestHandler::Cancel(TUint32 aRequestId)
	{
	iSIPClientDiscovery->Cancel(aRequestId);
	}
	
// -----------------------------------------------------------------------------
// CSIPRequestHandler::ChannelResolvedL
// From MSIPClientDiscoveryObserver
// -----------------------------------------------------------------------------
//
void CSIPRequestHandler::ChannelResolvedL(TUid aChannel, TUint32 aRequestId)
	{
	iObserver.ClientFoundL(aRequestId,aChannel);
	}

// -----------------------------------------------------------------------------
// CSIPRequestHandler::ClientNotFoundL
// From MSIPClientDiscoveryObserver
// -----------------------------------------------------------------------------
//
void CSIPRequestHandler::ClientNotFoundL(TUint32 aRequestId,
                                         TUint aStatusCode, 
                                         RStringF aReasonPhrase, 
                                         RPointerArray<CSIPHeaderBase> aHeaders,
                                         HBufC8* aContent)
	{
	CSIPResponse* response = CSIPResponse::NewLC(aStatusCode,aReasonPhrase);
    if (aContent)
        {
        response->SetContent(aContent->AllocL()); 
        }
	for (TInt i=aHeaders.Count()-1; i >= 0; i--)
		{
		CSIPHeaderBase* header = aHeaders[i];
		response->AddHeaderL(header);
		aHeaders.Remove(i);
		}
	iObserver.ClientNotFoundL(aRequestId,response);
	delete aContent;
	aHeaders.ResetAndDestroy();
	CleanupStack::Pop(response);
	}

// -----------------------------------------------------------------------------
// CSIPRequestHandler::ErrorOccurred
// From MSIPClientDiscoveryObserver
// -----------------------------------------------------------------------------
//
void CSIPRequestHandler::ErrorOccurred(TInt aError, TUint32 aRequestId)
	{
	iObserver.ErrorOccurred(aRequestId,aError);
	}	


