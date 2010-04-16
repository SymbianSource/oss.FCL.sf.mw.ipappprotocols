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
// Name          : CSIPOptionsHandler.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include <e32math.h>
#include <uri8.h>
#include "CSIPOptionsHandler.h"
#include "sdpcodecstringconstants.h"
#include "sipresponse.h"
#include "siprequest.h"
#include "uricontainer.h"
#include "sipuri.h"
#include "siphostport.h"
#include "sipcontenttypeheader.h"
#include "sipacceptheader.h"
#include "sdpdocument.h"
#include "sdporiginfield.h"
#include "sdpconnectionfield.h"
#include "sdpmediafield.h"
#include "sdpfmtattributefield.h"
#include "siperr.h"
#include "sdpcodecstringpool.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

_LIT8(KSdp, "sdp");
_LIT8(KApplication, "application");
_LIT8(KSessionName, "-");
_LIT8(KOriginFieldUser, "-");

const TUint KResponse200 = 200;
const TInt KSdpExternalizeBufferExpandSize = 100;


// ----------------------------------------------------------------------------
// CSIPOptionsHandler::NewL
// ----------------------------------------------------------------------------
//
CSIPOptionsHandler* CSIPOptionsHandler::NewLC(CSIPRequest& aRequest)
	{
	CSIPOptionsHandler* self = new(ELeave)CSIPOptionsHandler;
	CleanupStack::PushL(self);
	self->ConstructL(aRequest);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPOptionsHandler::CSIPOptionsHandler
// ----------------------------------------------------------------------------
//
CSIPOptionsHandler::CSIPOptionsHandler()
	{	
	}

// ----------------------------------------------------------------------------
// CSIPOptionsHandler::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPOptionsHandler::ConstructL(CSIPRequest& aRequest)
	{
	RStringPool strPool = SdpCodecStringPool::StringPoolL();
    
    iSdpNetType = strPool.StringF(SdpCodecStringConstants::ENetType,
                                     SdpCodecStringPool::StringTableL());
    
    iSdpIPv4Type = strPool.StringF(SdpCodecStringConstants::EAddressTypeIP4, 
                                      SdpCodecStringPool::StringTableL());
    
    iSdpIPv6Type = strPool.StringF(SdpCodecStringConstants::EAddressType, 
                                      SdpCodecStringPool::StringTableL());
                                     
	iRtpAvp = strPool.StringF(SdpCodecStringConstants::EProtocolRtpAvp, 
                              SdpCodecStringPool::StringTableL());
	        
	iRtpmap = strPool.StringF(SdpCodecStringConstants::EAttributeRtpmap, 
                              SdpCodecStringPool::StringTableL());
                                     
	iApplication = strPool.StringF(SdpCodecStringConstants::EMediaApplication,
                                   SdpCodecStringPool::StringTableL());
                          
    iSdpAccepted = SDPAccepted(aRequest);         
    // Create SDP document
    iSdpDocument = CSdpDocument::NewL();
    if (iSdpAccepted)
        {
        iSdpDocument->SetSessionNameL(KSessionName);   
        // Origin field 
        TPtrC8 localHost;
        RStringF hostType;
        LocalHostL(aRequest,localHost,hostType);
        CSdpOriginField* originField = CreateOriginFieldL(localHost,hostType);
        iSdpDocument->SetOriginField(originField);
        // Connection field
        CSdpConnectionField* connectionField = 
            CSdpConnectionField::NewL(iSdpNetType,hostType,localHost);
        iSdpDocument->SetConnectionField(connectionField);         
        }
	}

// ----------------------------------------------------------------------------
// CSIPOptionsHandler::~CSIPOptionsHandler
// ----------------------------------------------------------------------------
//
CSIPOptionsHandler::~CSIPOptionsHandler ()
	{
    delete iSdpDocument;
    iSipAcceptHeaders.ResetAndDestroy();
	}
	
// ----------------------------------------------------------------------------
// CSIPOptionsHandler::AddClientDataL
// ----------------------------------------------------------------------------
//	
void CSIPOptionsHandler::AddClientDataL(CSdpDocument& aSdpDocument)
    {
    RPointerArray<CSdpMediaField>& mediaFields = aSdpDocument.MediaFields();
    for (TInt i=0; i < mediaFields.Count(); i++)
        {
        CSdpMediaField& newMediaField = *mediaFields[i];
        CSdpMediaField* existingField = FindMatchingField(newMediaField);
        if (!existingField)
            {
            CSdpMediaField* clone = newMediaField.CloneL();
            CleanupStack::PushL( clone );
            AppendMediaFieldL( clone ); 
            CleanupStack::Pop( clone );       
            }
        else
            {
            AddAttributesL(newMediaField,*existingField);
            }
        }
    }

// ----------------------------------------------------------------------------
// CSIPOptionsHandler::AddClientDataL
// ----------------------------------------------------------------------------
//	
void CSIPOptionsHandler::AddClientDataL( RPointerArray<CSdpMediaField>* 
																aMediaFields)
    {
    if ( aMediaFields->Count() > 0 )
    	{
    	RPointerArray<CSdpMediaField>& mediaFields = *aMediaFields;
    	for (TInt i=0; i < mediaFields.Count(); i++)
        	{
        	CSdpMediaField* existingField = FindMatchingField( *mediaFields[i] );
        	if ( !existingField )
            	{
            	AppendMediaFieldL( mediaFields[i] );
            	aMediaFields->Remove( i );             
            	}
        	else
            	{
            	AddAttributesL( mediaFields[i],*existingField );
            	}
        	}
    	}
    }

// -----------------------------------------------------------------------------
// CSIPOptionsHandler::AppendMediaFieldL
// -----------------------------------------------------------------------------
//
void CSIPOptionsHandler::AppendMediaFieldL( CSdpMediaField* aMediafield )
	{
    aMediafield->SetPortL(0);
    UpdateFormatListL(*aMediafield);
    iSdpDocument->MediaFields().AppendL(aMediafield);
	}

// ----------------------------------------------------------------------------
// CSIPOptionsHandler::CreateResponseL
// ----------------------------------------------------------------------------
//	
CSIPResponse* CSIPOptionsHandler::CreateResponseL( RArray<TUid>& aUids,
									MSipClients& aSipClients,
    								CSIPClientResolver2& aClientResolver2 )
    {
    CSIPResponse* response = 
        CSIPResponse::NewLC(KResponse200,
            SIPStrings::StringF(SipStrConsts::EPhraseOk));
    if (iSdpAccepted)
        {
        // Add Content-Type-header
 	    CSIPContentTypeHeader* contentType = 
	        CSIPContentTypeHeader::NewLC(KApplication,KSdp);
	    response->AddHeaderL(contentType);
	    CleanupStack::Pop(contentType);
        // Encode and add content
        CBufFlat* sdpBuf = CBufFlat::NewL(KSdpExternalizeBufferExpandSize);
	    CleanupStack::PushL(sdpBuf);
	    RBufWriteStream writeStream(*sdpBuf,0);
        writeStream.PushL();
        iSdpDocument->EncodeL(writeStream);
 	    writeStream.Pop();
 	    writeStream.Close();
        TPtr8 encodedSdp = sdpBuf->Ptr(0);       
        response->SetContent(encodedSdp.AllocL());
        CleanupStack::PopAndDestroy(sdpBuf);
        }
    AddAcceptToResponseL( *response, aUids, aSipClients, aClientResolver2 );
	
	
	//Add ClientSpecificHeaders for OPTIONS here
	for (TInt i=0; i < aClientResolver2.Clients().Count(); i++)
        {
        CSIPResolvedClient2* client = aClientResolver2.Clients()[i];
        if ( client )
            {
			RPointerArray<CSIPHeaderBase> headers ;
			CSIPHeaderBase::PushLC(&headers);
            client->AddClientSpecificHeadersForOptionsResponseL(headers);
			for (TInt i=0; i<headers.Count(); i++)
				{
				response->AddHeaderL(headers[i]);
				}
			CleanupStack::Pop(1); //headers
            }
        }   	
        
    CleanupStack::Pop(response);
    return response;
    }
  
// ----------------------------------------------------------------------------
// CSIPOptionsHandler::AddAcceptToResponseL
// ----------------------------------------------------------------------------
//	
void CSIPOptionsHandler::AddAcceptToResponseL( CSIPResponse& aResponse,
								 	  RArray<TUid>& aUids,
									  MSipClients& aSipClients,
    								  CSIPClientResolver2& aClientResolver2 )
    {
    CollectHeadersL( aUids,aSipClients,aClientResolver2 );
    RemoveDuplicateAcceptHeaders();
	CSIPAcceptHeader* acceptHeader;
	
	for ( TInt i=iSipAcceptHeaders.Count()-1;i >= 0;i-- )
		{
		acceptHeader = static_cast<CSIPAcceptHeader*>( iSipAcceptHeaders[ i ] );
		if ( acceptHeader->IsEmpty() )
			{
			delete acceptHeader;
			acceptHeader = NULL;
			iSipAcceptHeaders.Remove( i );
			}
		else
			{
			CSIPHeaderBase* header = iSipAcceptHeaders[i];
			aResponse.AddHeaderL( header );
			iSipAcceptHeaders.Remove( i );
			}
		}
	if ( iSipAcceptHeaders.Count() > 0 )
		{
		iSipAcceptHeaders.ResetAndDestroy();
		}
    }
  
// ----------------------------------------------------------------------------
// CSIPOptionsHandler::CollectHeadersL
// ----------------------------------------------------------------------------
//	
void CSIPOptionsHandler::CollectHeadersL( 
									  RArray<TUid>& aUids,
									  MSipClients& aSipClients,
    								  CSIPClientResolver2& aClientResolver2 )
    {
    for ( TInt i=0; i < aUids.Count(); i++ )
        {
        MSipClient* client = aSipClients.GetByUID( aUids[i] );
        if ( client )
            {
            CollectHeadersL( client );
            }
        }
    for ( TInt i=0; i < aClientResolver2.Clients().Count(); i++ )
        {
        CSIPResolvedClient2* client2 = aClientResolver2.Clients()[i];
        if ( client2 )
            {
            CollectHeadersL( client2 );
            }
        }  
    }
  
// ----------------------------------------------------------------------------
// CSIPOptionsHandler::RemoveDuplicateAcceptHeaders
// ----------------------------------------------------------------------------
//
void CSIPOptionsHandler::RemoveDuplicateAcceptHeaders()
	{
	for (TInt i=0;i<iSipAcceptHeaders.Count();i++)
		{
		CSIPAcceptHeader* acceptHeader =
		    static_cast<CSIPAcceptHeader*>( iSipAcceptHeaders[ i ] );

		for ( TInt j=iSipAcceptHeaders.Count() - 1;j>i;j-- )
			{
			CSIPAcceptHeader* acceptHeaderT =
				static_cast<CSIPAcceptHeader*> ( iSipAcceptHeaders[ j ] );

			if ( ( ( acceptHeader->MediaType().Compare(
										acceptHeaderT->MediaType()) == 0 ) &&
				( acceptHeader->MediaSubtype().Compare(
								   acceptHeaderT->MediaSubtype() ) == 0 ) ) )
				{
				delete acceptHeaderT;
				acceptHeaderT = NULL;
				iSipAcceptHeaders.Remove( j );
				}
			}
		}
	}  
  
// ----------------------------------------------------------------------------
// CSIPOptionsHandler::CollectHeadersL
// ----------------------------------------------------------------------------
//
void CSIPOptionsHandler::CollectHeadersL( MSipClient* aSipClient )
	{
	RPointerArray<CSIPHeaderBase>& headers = aSipClient->SIPHeaders();
	
	for ( TInt i=0; i < headers.Count();i++ )
		{
		if ( headers[i]->Name() == 
		    SIPStrings::StringF( SipStrConsts::EAcceptHeader ) )
			{
			CSIPHeaderBase* accept = headers[i]->CloneL();
			CleanupStack::PushL( accept );
			iSipAcceptHeaders.AppendL( accept );
			CleanupStack::Pop( accept );
			}
		}
	} 
  
// ----------------------------------------------------------------------------
// CSIPOptionsHandler::CollectHeadersL
// ----------------------------------------------------------------------------
//
void CSIPOptionsHandler::CollectHeadersL( CSIPResolvedClient2* aClient )
	{
	RPointerArray<CSIPContentTypeHeader> contentTypes; 
	TRAPD(err, contentTypes = aClient->SupportedContentTypesL());
	if ( err == KErrNoMemory )
        {
        User::Leave( err );
        }
	if ( !err )
		{
		CSIPOptionsHandler::PushLC(&contentTypes);
		for ( TInt i=0; i < contentTypes.Count();i++ )
			{
			HBufC8* text = contentTypes[i]->ToTextValueL();
			CleanupStack::PushL( text );
			RPointerArray<CSIPAcceptHeader> acceptHeaders;
			acceptHeaders = CSIPAcceptHeader::DecodeL( *text );
			CleanupStack::PopAndDestroy( text );
			text = NULL;
			CSIPOptionsHandler::PushLC(&acceptHeaders);
			CSIPAcceptHeader* accept = NULL;
			for (TInt j=0;j < acceptHeaders.Count();j++)
				{
				accept = acceptHeaders[j];
				iSipAcceptHeaders.AppendL( accept );
				acceptHeaders[j] = NULL;
				}
			CleanupStack::Pop();//acceptHeaders
			acceptHeaders.Reset();
			}
		CleanupStack::Pop();//contentTypes
    	contentTypes.ResetAndDestroy();	
		}
	}  
  
// -----------------------------------------------------------------------------
// CSIPOptionsHandler::PushLC
// -----------------------------------------------------------------------------
//
void CSIPOptionsHandler::PushLC(RPointerArray<CSIPContentTypeHeader>* aArray)
    {
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArray,aArray));
    }
  
// -----------------------------------------------------------------------------
// CSIPOptionsHandler::ResetAndDestroyArray
// -----------------------------------------------------------------------------
//
void CSIPOptionsHandler::ResetAndDestroyArray(TAny* anArray)
	{
    RPointerArray<CSIPContentTypeHeader>* array =
        reinterpret_cast<RPointerArray<CSIPContentTypeHeader>*>(anArray);
    if (array)
        {
        array->ResetAndDestroy();
        }
	}      
  
// -----------------------------------------------------------------------------
// CSIPOptionsHandler::PushLC
// -----------------------------------------------------------------------------
//
void CSIPOptionsHandler::PushLC(RPointerArray<CSIPAcceptHeader>* aArray)
    {
	CleanupStack::PushL(TCleanupItem(ResetAndDestroy,aArray));
    }
  
// -----------------------------------------------------------------------------
// CSIPOptionsHandler::ResetAndDestroyArray
// -----------------------------------------------------------------------------
//
void CSIPOptionsHandler::ResetAndDestroy(TAny* anArray)
	{
    RPointerArray<CSIPAcceptHeader>* array =
        reinterpret_cast<RPointerArray<CSIPAcceptHeader>*>(anArray);
    if (array)
        {
        array->ResetAndDestroy();
        }
	}      
  
// ----------------------------------------------------------------------------
// CSIPOptionsHandler::SDPAccepted
// ----------------------------------------------------------------------------
//  
TBool CSIPOptionsHandler::SDPAccepted(CSIPRequest& aRequest)
    {
	TSglQueIter<CSIPHeaderBase> iter = 
        aRequest.Headers(SIPStrings::StringF(SipStrConsts::EAcceptHeader));

	while (iter)
		{
		CSIPHeaderBase* header = iter++;
		CSIPAcceptHeader* accept = static_cast<CSIPAcceptHeader*>(header);
		if (accept->MediaType().CompareF(KApplication) == 0 &&
		    accept->MediaSubtype().CompareF(KSdp) == 0)
		    {
		    return ETrue;
		    }
		}
		
	return EFalse;
    }

// ----------------------------------------------------------------------------
// CSIPOptionsHandler::FindMatchingField
// ----------------------------------------------------------------------------
//    
CSdpMediaField* CSIPOptionsHandler::FindMatchingField(
    const CSdpMediaField& aMediaField)
    {
    for (TInt i=0; i < iSdpDocument->MediaFields().Count(); i++)
        {
        CSdpMediaField* mField = iSdpDocument->MediaFields()[i];
        if (mField &&
            aMediaField.Media() == mField->Media() &&
            aMediaField.Protocol() == mField->Protocol())
            {
            if (aMediaField.Media() != iApplication)
                {
                // FormatList is compared only for "application" media type
                return mField;
                }
            if (aMediaField.FormatList().CompareF(mField->FormatList()) == 0)
                {
                return mField;
                }
            }
        }
    return NULL;    
    }
    
// ----------------------------------------------------------------------------
// CSIPOptionsHandler::AddAttributesL
// ----------------------------------------------------------------------------
//
void CSIPOptionsHandler::AddAttributesL(
    CSdpMediaField& aSource,
    CSdpMediaField& aDestination)
    {
    RPointerArray<CSdpFmtAttributeField> attributes = 
        aSource.FormatAttributeFields();
        
    if (aSource.Protocol() == iRtpAvp && attributes.Count() > 0)
        {
        TBool updateFormatList = EFalse;
        for (TInt i=0; i < attributes.Count(); i++)
            {
            CSdpFmtAttributeField* attribute = attributes[i];
            if (attribute->Attribute() == iRtpmap &&
                !HasMatchingFmtAttritbute(aDestination,*attribute))
                {
                CSdpFmtAttributeField* clone = attribute->CloneL();
                CleanupStack::PushL(clone);
                updateFormatList =
                		AppendFormatAttributeFieldsL( clone,
                									  aDestination );
                CleanupStack::Pop(clone);
                }
            }
        if (updateFormatList)
            {
            UpdateFormatListL(aDestination);
            }
        }
    }
    
// ----------------------------------------------------------------------------
// CSIPOptionsHandler::AddAttributesL
// ----------------------------------------------------------------------------
//
void CSIPOptionsHandler::AddAttributesL(
    CSdpMediaField* aSource,
    CSdpMediaField& aDestination)
    {
    RPointerArray<CSdpFmtAttributeField> attributes = 
        aSource->FormatAttributeFields();    
    if (aSource->Protocol() == iRtpAvp && attributes.Count() > 0)
        {
        TBool updateFormatList = EFalse;
        for (TInt i=0; i < attributes.Count(); i++)
            {
            if (attributes[i]->Attribute() == iRtpmap &&
                !HasMatchingFmtAttritbute(aDestination,*attributes[i]))
                {
                updateFormatList =AppendFormatAttributeFieldsL( attributes[i], 
                												aDestination );
                attributes[i] = NULL;
                }
            }
        if (updateFormatList)
            {
            UpdateFormatListL(aDestination);
            }
        }
    }
    
// ----------------------------------------------------------------------------
// CSIPOptionsHandler::AppendFormatAttributeFieldsL
// ----------------------------------------------------------------------------
//
TBool CSIPOptionsHandler::AppendFormatAttributeFieldsL(
									CSdpFmtAttributeField* aAttribute,
									CSdpMediaField& aDestination )
    {
    aDestination.FormatAttributeFields().AppendL(aAttribute);
    return ETrue;  
    }
    
// ----------------------------------------------------------------------------
// CSIPOptionsHandler::CreateOriginFieldL
// ----------------------------------------------------------------------------
//    
CSdpOriginField* CSIPOptionsHandler::CreateOriginFieldL(
    const TDesC8& aLocalHost,
    const RStringF& aHostType) const
    {
    TTime now;
    now.UniversalTime();
    TInt64 rand = now.Int64();
    TInt64 sessionID = Math::Rand(rand);
    return CSdpOriginField::NewL(KOriginFieldUser,
                                 sessionID,
                                 sessionID,
                                 iSdpNetType,
                                 aHostType,
                                 aLocalHost);    
    }
  
// ----------------------------------------------------------------------------
// CSIPOptionsHandler::LocalHostL
// ----------------------------------------------------------------------------
//
void CSIPOptionsHandler::LocalHostL(
    CSIPRequest& aRequest,
    TPtrC8& aLocalHost,
    RStringF& aHostType) const
    {
    CURIContainer* requestURI = aRequest.RequestURI();
    if (!requestURI || !requestURI->IsSIPURI())
        {
        User::Leave(KErrSIPInvalidURIType);
        }
    const CSIPHostPort& hostPort = requestURI->SIPURI()->HostPort();
    aLocalHost.Set(hostPort.Host());
    if (hostPort.HostType() == CSIPHostPort::ESIPIpv6)
        {
        aHostType = iSdpIPv6Type;
        }
    else
        {
        aHostType = iSdpIPv4Type;
        }
    }
   
// ----------------------------------------------------------------------------
// CSIPOptionsHandler::HasMatchingFmtAttritbute
// ----------------------------------------------------------------------------
//	
TBool CSIPOptionsHandler::HasMatchingFmtAttritbute(
    CSdpMediaField& aMedia,
    const CSdpFmtAttributeField& aAttribute)
    {        
    RPointerArray<CSdpFmtAttributeField>& attributes = 
        aMedia.FormatAttributeFields();
        
    for (TInt i=0; i < attributes.Count(); i++)
        {
        if (aAttribute == *attributes[i])
            {
            return ETrue;
            }
        }
    
    return EFalse;
    }
  
// ----------------------------------------------------------------------------
// CSIPOptionsHandler::UpdateFormatListL
// ----------------------------------------------------------------------------
//  
void CSIPOptionsHandler::UpdateFormatListL(CSdpMediaField& aMedia)
    {
    RPointerArray<CSdpFmtAttributeField>& attributes = 
        aMedia.FormatAttributeFields();
        
    if (aMedia.Protocol() == iRtpAvp && attributes.Count() > 0)
        {
        // Calculate length for new format list
        TInt newFormatListLength = 0;
        for (TInt i=0; i < attributes.Count(); i++)
            {
            newFormatListLength += attributes[i]->Format().Length();
            newFormatListLength++; // space
            }
        // Create new format list
        HBufC8* newFormatListBuf = HBufC8::NewLC(newFormatListLength);
        TPtr8 newFormatListPtr = newFormatListBuf->Des();
        for (TInt i=0; i < attributes.Count(); i++)
            {
            newFormatListPtr.Append(attributes[i]->Format());
            if (i < attributes.Count()-1)
                {
                newFormatListPtr.Append(' '); // space
                }
            }
        aMedia.SetFormatListL(newFormatListPtr);
        CleanupStack::PopAndDestroy(newFormatListBuf);
        }
    }
