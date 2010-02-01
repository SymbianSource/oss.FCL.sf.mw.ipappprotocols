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
// Name          : CSipLaunchingStrategies.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSipLaunchingStrategies.h"
#include "sdpcodecstringconstants.h"
#include "MSipClients.h"
#include "MSipClient.h"
#include "CSipAcceptContactStrategy2.h"
#include "CSipAcceptContactStrategy.h"
#include "CSipAllowEventsStrategy2.h"
#include "CSipAllowEventsStrategy.h"
#include "CSipRequestStrategy.h"
#include "CSipContentTypeStrategy.h"
#include "CSdpMLineNameStrategy.h"
#include "CSdpMLineMediaStrategy.h"
#include "CSdpMLineTransportStrategy.h"
#include "CSdpRtpmapStrategy.h"
#include "CSdpMediaAttributeStrategy.h"
#include "CSIPOptionsHandler.h" 
#include "sipresponse.h"
#include "siprequest.h"
#include "sipcontenttypeheader.h"
#include "siperr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sdpdocument.h"
#include "SIPCRLogs.h"
#include "CSIPClientResolver2.h"
#include "sdpmediafield.h"

const TUint KResponse400 = 400;
const TUint KResponse501 = 501;

_LIT8(KSdp, "sdp");
_LIT8(KApplication, "application");


// ----------------------------------------------------------------------------
// CSipLaunchingStrategies::NewL
// ----------------------------------------------------------------------------
//
CSipLaunchingStrategies* CSipLaunchingStrategies::NewL(
    MSipClients& aSipClients)
	{
	CSipLaunchingStrategies* self = 
	    new(ELeave)CSipLaunchingStrategies(aSipClients);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSipLaunchingStrategies::CSipLaunchingStrategies
// ----------------------------------------------------------------------------
//
CSipLaunchingStrategies::CSipLaunchingStrategies(
    MSipClients& aSipClients)
    : iSipClients(aSipClients) 
	{	
	}

// ----------------------------------------------------------------------------
// CSipLaunchingStrategies::ConstructL
// ----------------------------------------------------------------------------
//
void CSipLaunchingStrategies::ConstructL()
	{
	// SIP header strategies
	CSipContentTypeStrategy* contentTypeStrategy = 
		CSipContentTypeStrategy::NewLC( iSipClients,NULL );
	iSipStrategies.AppendL( contentTypeStrategy );
	CleanupStack::Pop( contentTypeStrategy );	
	
	CSipRequestStrategy* requestStrategy = 
		CSipRequestStrategy::NewLC( iSipClients,NULL,contentTypeStrategy );
	iSipStrategies.AppendL( requestStrategy );
	CleanupStack::Pop( requestStrategy );	
	
	CSipAllowEventsStrategy* allowEventsStrategy = 
		CSipAllowEventsStrategy::NewLC( iSipClients, 
										requestStrategy, 
										requestStrategy );
	iSipStrategies.InsertL( allowEventsStrategy,0 );
	CleanupStack::Pop( allowEventsStrategy );
	
	CSipAllowEventsStrategy2* allowEventsStrategy2 =
		CSipAllowEventsStrategy2::NewLC( iSipClients,
										 requestStrategy,
										 allowEventsStrategy );
	iSipStrategies.InsertL( allowEventsStrategy2,0 );
	CleanupStack::Pop( allowEventsStrategy2 );
		
	CSipAcceptContactStrategy* acceptContactStrategy = 
		CSipAcceptContactStrategy::NewLC( iSipClients,
										  allowEventsStrategy,
										  allowEventsStrategy2 );	
	iSipStrategies.InsertL( acceptContactStrategy,0 );
	CleanupStack::Pop( acceptContactStrategy );
	
    CSipAcceptContactStrategy2* acceptContactStrategy2 =
    	CSipAcceptContactStrategy2::NewLC( iSipClients,
    									   allowEventsStrategy2,
    									   acceptContactStrategy );
    iSipStrategies.InsertL( acceptContactStrategy2,0 );
	CleanupStack::Pop( acceptContactStrategy2 );
    
    // SDP strategies
	CSdpMLineMediaStrategy* mlineMedia = 
	    CSdpMLineMediaStrategy::NewLC(iSipClients);
	iSdpStrategies.AppendL(mlineMedia);
	CleanupStack::Pop(mlineMedia);

	CSdpMLineTransportStrategy* mlineTransport = 
	    CSdpMLineTransportStrategy::NewLC(iSipClients);
	iSdpStrategies.AppendL(mlineTransport);
	CleanupStack::Pop(mlineTransport);

	CSdpRtpmapStrategy* rtpmap = 
		CSdpRtpmapStrategy::NewLC(iSipClients);
	iSdpStrategies.AppendL(rtpmap);
	CleanupStack::Pop(rtpmap);

    CSdpMediaAttributeStrategy* mediaAttribute = 
		CSdpMediaAttributeStrategy::NewLC(iSipClients);
    iSdpStrategies.AppendL(mediaAttribute);
	CleanupStack::Pop(mediaAttribute);

	CSdpMLineNameStrategy* mlineName = 
		CSdpMLineNameStrategy::NewLC(iSipClients);
    iSdpStrategies.AppendL(mlineName);
	CleanupStack::Pop(mlineName);
	}

// ----------------------------------------------------------------------------
// CSipLaunchingStrategies::~CSipLaunchingStrategies
// ----------------------------------------------------------------------------
//
CSipLaunchingStrategies::~CSipLaunchingStrategies()
	{
	iSipStrategies.ResetAndDestroy();
	iSdpStrategies.ResetAndDestroy();
	}

// ----------------------------------------------------------------------------
// CSipLaunchingStrategies::ApplyL
// ----------------------------------------------------------------------------
//
CSIPResponse* CSipLaunchingStrategies::ApplyL(
    CSIPRequest& aRequest, 
    RArray<TUid>& aUids,
    CSIPClientResolver2& aClientResolver2 )
	{
	SIP_CR_LOG("CSipLaunchingStrategies::ApplyL")	
	if (aRequest.Method() == SIPStrings::StringF(SipStrConsts::EOptions))
	    {
	    SIP_CR_LOG("CSipLaunchingStrategies::ApplyL EOptions")
        if (ClientForOptionsL(aRequest,aUids,aClientResolver2))
            {
            return NULL;
            }
	    }
    CSIPResponse* response = CheckMethodL(aRequest,aUids,aClientResolver2);
    if (response)
        {
        SIP_CR_LOG("CSipLaunchingStrategies::ApplyL wrong method")
        return response;
        }
    
    // Apply SIP header strategies
    TBool continueSearch = EFalse;
    response = iSipStrategies[0]->ApplyL(aRequest,aUids,continueSearch,
    									aClientResolver2 );
    
    // Apply SDP strategies
	if (!response && 
	    continueSearch && 
	    HasSDPContent(const_cast<CSIPRequest&>(aRequest)))
		{
	    CSdpDocument* sdpDocument = NULL;
		TRAPD (err, sdpDocument = CSdpDocument::DecodeL(aRequest.Content()));
		SIP_CR_INT_LOG("CSdpDocument::DecodeL completed with", err)
		if (err != KErrNone)
			{
			if (err == KErrNoMemory)
				{
				User::Leave(err);
				}
			return CSIPResponse::NewL(KResponse400, 
			    SIPStrings::StringF(SipStrConsts::EPhraseBadRequest));
			}
	    CleanupStack::PushL(sdpDocument);			
		if (!sdpDocument->IsValid())
		    {
		    response = CSIPResponse::NewL(KResponse400, 
			    SIPStrings::StringF(SipStrConsts::EPhraseBadRequest));
		    }
		else
		    {
		    for (TInt j=0; j < iSdpStrategies.Count() && !response; j++)
			    {
			    response = iSdpStrategies[j]->ApplyL(*sdpDocument,aUids);
			    }		    
		    }
	    CleanupStack::PopAndDestroy(sdpDocument);
		}

	return response;
	}

// ----------------------------------------------------------------------------
// CSipLaunchingStrategies::CheckMethodL
// ----------------------------------------------------------------------------
//
CSIPResponse* CSipLaunchingStrategies::CheckMethodL(
    CSIPRequest& aRequest,
    RArray<TUid>& aUids,
    CSIPClientResolver2& aClientResolver2)
    {
    CSIPResponse* response = NULL;
    RStringF method = aRequest.Method();
    
	if (method == SIPStrings::StringF(SipStrConsts::EOptions))
	    {
	    response = CreateOptionsResponseL(aRequest,aUids,aClientResolver2);
	    }
	else if (method == SIPStrings::StringF(SipStrConsts::EPrack) ||
	         method == SIPStrings::StringF(SipStrConsts::EBye) ||
	         method == SIPStrings::StringF(SipStrConsts::EUpdate) ||
	         method == SIPStrings::StringF(SipStrConsts::EAck))
	    {
	    response = 
	        CSIPResponse::NewL(KResponse400,
	            SIPStrings::StringF(SipStrConsts::EPhraseBadRequest));
	    }
	else if (method == SIPStrings::StringF(SipStrConsts::ERegister))
	    {
        response = 
	        CSIPResponse::NewL(KResponse501,
	            SIPStrings::StringF(SipStrConsts::EPhraseNotImplemented)); 
	    }
	else
	    {
	    // NOP
	    }
    
    return response;
    }

// ----------------------------------------------------------------------------
// CSipLaunchingStrategies::HasSDPContent
// ----------------------------------------------------------------------------
//
TBool CSipLaunchingStrategies::HasSDPContent(CSIPRequest& aRequest)
	{
	CSIPHeaderBase* header = 
	    aRequest.Header(
	        SIPStrings::StringF(SipStrConsts::EContentTypeHeader),0);
	
	if (header)
	    {
	    CSIPContentTypeHeader* contentType =
		    static_cast<CSIPContentTypeHeader*> (header);
		
	    if((contentType->MediaType()).CompareF(KApplication) != 0)
		    {
		    return EFalse;
		    }
	
	    return (contentType->MediaSubtype().CompareF(KSdp) == 0);	    
	    }

	return EFalse;
	}

// ----------------------------------------------------------------------------
// CSipLaunchingStrategies::CreateOptionsResponseL
// ----------------------------------------------------------------------------
//	
CSIPResponse* CSipLaunchingStrategies::CreateOptionsResponseL(
    CSIPRequest& aRequest,
    RArray<TUid>& aUids,
    CSIPClientResolver2& aClientResolver2)
    {
    CSIPOptionsHandler* optionsHandler = CSIPOptionsHandler::NewLC(aRequest);
    
    for (TInt i=0; i < aUids.Count(); i++)
        {
        MSipClient* client = iSipClients.GetByUID(aUids[i]);
        if (client)
            {
            optionsHandler->AddClientDataL(client->SdpDocument());
            }
        }
        
    for (TInt i=0; i < aClientResolver2.Clients().Count(); i++)
        {
        CSIPResolvedClient2* client = aClientResolver2.Clients()[i];
        if ( client )
            {
            RPointerArray<CSdpMediaField> mediaFields;
            TRAPD( err,mediaFields = client->SupportedSdpMediasL() );
            if ( err == KErrNoMemory )
        		{
        		User::Leave( err );
        		}
            if ( !err )
            	{
            	CSipLaunchingStrategies::PushLC(&mediaFields);
            	optionsHandler->AddClientDataL(&mediaFields);
            	CleanupStack::Pop();//mediaFields
            	mediaFields.ResetAndDestroy();
            	}
            err = 0;
            }
        }   
        
    CSIPResponse* response = 
    				   optionsHandler->CreateResponseL( aUids,
													    iSipClients,
    								  					aClientResolver2);

    CleanupStack::PopAndDestroy(optionsHandler);
    
    return response;
    }

// -----------------------------------------------------------------------------
// CSipLaunchingStrategies::PushLC
// -----------------------------------------------------------------------------
//
void CSipLaunchingStrategies::PushLC(RPointerArray<CSdpMediaField>* aArray)
    {
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArray,aArray));
    }
  
// -----------------------------------------------------------------------------
// CSipLaunchingStrategies::ResetAndDestroyArray
// -----------------------------------------------------------------------------
//
void CSipLaunchingStrategies::ResetAndDestroyArray(TAny* anArray)
	{
    RPointerArray<CSdpMediaField>* array =
        reinterpret_cast<RPointerArray<CSdpMediaField>*>(anArray);
    if (array)
        {
        array->ResetAndDestroy();
        }
	}    

// ----------------------------------------------------------------------------
// CSipLaunchingStrategies::ClientForOptionsL
// ----------------------------------------------------------------------------
//    
TBool CSipLaunchingStrategies::ClientForOptionsL(
    CSIPRequest& aRequest,
    RArray<TUid>& aUids,
    CSIPClientResolver2& aClientResolver2 )
    {
    TBool found = EFalse;
    if (aRequest.HasHeader(
            SIPStrings::StringF(SipStrConsts::EAcceptContactHeader)))
        {
        CSIPResponse* response = NULL;
        RArray<TUid> tmpUids;
	    CleanupClosePushL(tmpUids);
        for (TInt i=0; i<aUids.Count(); i++)
	        {
	        tmpUids.AppendL(aUids[i]);
	        }
        
        TBool secondGenerationClientsFound = 
            (aClientResolver2.Clients().Count() > 0);
            
	    CSipAcceptContactStrategy* acceptContact = 
	        CSipAcceptContactStrategy::NewLC( iSipClients,NULL,NULL );          
        
        TBool dummy = EFalse;
        
        if ( secondGenerationClientsFound )
        	{
    		CSipAcceptContactStrategy2* acceptContact2 =
    			CSipAcceptContactStrategy2::NewLC( iSipClients,
    									   		   NULL,
    									           acceptContact );
	    	response = acceptContact2->ApplyL( aRequest,tmpUids,dummy,
	    													aClientResolver2 );
	    	CleanupStack::PopAndDestroy( acceptContact2 );        	
        	}
        else
        	{
            response = acceptContact->ApplyL( aRequest,tmpUids,dummy,
            												aClientResolver2 );
       		}
        CleanupStack::PopAndDestroy( acceptContact );
       	CleanupStack::PushL(response); 
	    
	    if (!response)
	        {
	        found = ETrue;
	        aUids.Reset();
            // Add only the best match to the results.
            // The best match is the first item in the resulting array.
            if (tmpUids.Count() > 0)
                {
                aUids.AppendL(tmpUids[0]);
                }  
	        }
	    delete response;
		CleanupStack::Pop(); //response
	    CleanupStack::PopAndDestroy(&tmpUids);     
        }
    return found;
    }
