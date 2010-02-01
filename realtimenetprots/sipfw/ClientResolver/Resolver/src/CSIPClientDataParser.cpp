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
// Name          : CSIPClientDataParser.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSIPClientDataParser.h"
#include "CSIPClientData.h"
#include "SIPHeaderLookup.h"
#include "sipacceptcontactheader.h"
#include "CSIPFeatureSet.h"
#include "sdpdocument.h"
#include "sdpmediafield.h"
#include "sdpfmtattributefield.h"
#include "sdpattributefield.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


// Constants for our XML DTD
_LIT8( KTagSIPClient, "SIP_CLIENT" );
_LIT8( KTagSIPHeaders, "SIP_HEADERS" );
_LIT8( KTagSDPLines, "SDP_LINES" );
_LIT8( KTagLine, "LINE" );
_LIT8( KTagLineM, "m" );
_LIT8( KTagRtpmap, "RTPMAP" );
_LIT8( KTagMediaAttribute, "MEDIA_ATTRIBUTE" );
_LIT8( KTagAccept, "ACCEPT" );
_LIT8( KTagAllowStarting, "ALLOW_STARTING" );
_LIT8( KTagAcceptContact, "ACCEPT_CONTACT" );
_LIT8( KTagAllowEvents, "ALLOW_EVENTS" );
_LIT8( KTagYes, "YES" );

_LIT8( KMimeTypeXml, "text/xml" );
_LIT8( KSdpMediaFieldStart, "m=" );
_LIT8( KSdpAttributeFieldStart, "a=" );
_LIT8( KRtpmapName, "rtpmap" );
_LIT8( KCrLf, "\r\n" );

const TInt KSdpLineAttrCount( 2 );

// -----------------------------------------------------------------------------
// CSIPClientDataParser::NewL
// -----------------------------------------------------------------------------
//
CSIPClientDataParser* CSIPClientDataParser::NewL()
	{
	CSIPClientDataParser* self = new( ELeave ) CSIPClientDataParser;
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPClientDataParser::CSIPClientDataParser
// -----------------------------------------------------------------------------
//
CSIPClientDataParser::CSIPClientDataParser() 
 : iSIPHeaderLookupOpened (EFalse)
	{
	}

// -----------------------------------------------------------------------------
// CSIPClientDataParser::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPClientDataParser::ConstructL()
	{
	iParser = Xml::CParser::NewL( KMimeTypeXml, *this );
	SIPHeaderLookup::OpenL();
	iSIPHeaderLookupOpened = ETrue;
	
    // Initialize XML strings
    RStringPool& stringPool = iParser->StringPool();
    iTagSIPClient = stringPool.OpenStringL(KTagSIPClient);
    iTagSIPHeaders = stringPool.OpenStringL(KTagSIPHeaders);
    iTagSDPLines = stringPool.OpenStringL(KTagSDPLines);
    iTagLine = stringPool.OpenStringL(KTagLine);
    iTagLineM = stringPool.OpenStringL(KTagLineM);
    iTagRtpmap = stringPool.OpenStringL(KTagRtpmap);
    iTagMediaAttribute = stringPool.OpenStringL(KTagMediaAttribute);
    iTagAccept = stringPool.OpenStringL(KTagAccept);
    iTagAllowStarting = stringPool.OpenStringL(KTagAllowStarting);
    iTagAcceptContact = stringPool.OpenStringL(KTagAcceptContact);
    iTagAllowEvents = stringPool.OpenStringL(KTagAllowEvents);
    iTagYes = stringPool.OpenStringL(KTagYes);
    
	// Initialize parser states
	iStateTransitions.AppendL(
	    TStateTransition(iTagSIPClient,EInit,ESIPClient));
	    
	iStateTransitions.AppendL(
	    TStateTransition(iTagSIPClient,ESIPClient,EInit,EFalse));
	
	iStateTransitions.AppendL(
	    TStateTransition(iTagSIPHeaders,ESIPClient,ESIPHeaders));
	
	iStateTransitions.AppendL(
	    TStateTransition(iTagSIPHeaders,ESIPHeaders,ESIPClient,EFalse));	

	iStateTransitions.AppendL(
	    TStateTransition(iTagAccept,ESIPHeaders,EAccept));
	
	iStateTransitions.AppendL(
	    TStateTransition(iTagAccept,EAccept,ESIPHeaders,EFalse));
	
	iStateTransitions.AppendL(
	    TStateTransition(iTagAcceptContact,ESIPHeaders,EAcceptContact));
	
	iStateTransitions.AppendL(
	    TStateTransition(iTagAcceptContact,EAcceptContact,ESIPHeaders,EFalse));
	
	iStateTransitions.AppendL(
	    TStateTransition(iTagAllowEvents,ESIPHeaders,EAllowEvents));
	    
	iStateTransitions.AppendL(
	    TStateTransition(iTagAllowEvents,EAllowEvents,ESIPHeaders,EFalse));
	
	iStateTransitions.AppendL(
	    TStateTransition(iTagSDPLines,ESIPClient,ESDPLines));

	iStateTransitions.AppendL(
	    TStateTransition(iTagSDPLines,ESDPLines,ESIPClient,EFalse));
	
	iStateTransitions.AppendL(
	    TStateTransition(iTagLine,ESDPLines,ELine));
	
	iStateTransitions.AppendL(
	    TStateTransition(iTagLine,ELine,ESDPLines,EFalse));
	    
	iStateTransitions.AppendL(
	    TStateTransition(iTagRtpmap,ELine,ERtpmap));
	    
	iStateTransitions.AppendL(
	    TStateTransition(iTagRtpmap,ERtpmap,ELine,EFalse));
	    
	iStateTransitions.AppendL(
	    TStateTransition(iTagMediaAttribute,ELine,EMediaAttribute));
	    
	iStateTransitions.AppendL(
	    TStateTransition(iTagMediaAttribute,EMediaAttribute,ELine,EFalse));	    	    
	}

// -----------------------------------------------------------------------------
// CSIPClientDataParser::~CSIPClientDataParser
// -----------------------------------------------------------------------------
//
CSIPClientDataParser::~CSIPClientDataParser()
	{
	iStateTransitions.Close();
    iTagYes.Close();
    iTagAllowEvents.Close();
    iTagAcceptContact.Close();
    iTagAllowStarting.Close();
    iTagAccept.Close();
    iTagMediaAttribute.Close();
    iTagLineM.Close();
    iTagRtpmap.Close();
    iTagLine.Close();
    iTagSDPLines.Close();
    iTagSIPHeaders.Close();
    iTagSIPClient.Close();
	if( iSIPHeaderLookupOpened )
		{
		SIPHeaderLookup::Close();
		}		
	delete iParser;
	}

// -----------------------------------------------------------------------------
// CSIPClientDataParser::ParseL
// -----------------------------------------------------------------------------
//
void CSIPClientDataParser::ParseL ( CSIPClientData* aClientData,
								    const TDesC8& aXmlDocument )
	{
    iState = EInit;
	iClientData = aClientData;

	// Parse the specified document into iClientData
	Xml::ParseL( *iParser, aXmlDocument );

	iClientData = NULL;
	}

// -----------------------------------------------------------------------------
// CSIPClientDataParser::OnStartElementL
// -----------------------------------------------------------------------------
//
void CSIPClientDataParser::OnStartElementL(
									const Xml::RTagInfo& aElement,
									const Xml::RAttributeArray& aAttributes,
									TInt /*aErrorCode*/ )
	{
	// Get element name (e.g. "SIP_CLIENT") and change state (Enter)
	ChangeStateL( aElement.LocalName(), ETrue );

	switch( iState )
		{
		case ESIPClient:
			{
			HandleSIPClientL( aAttributes );
			break;
			}
		case EAccept:
			{
			HandleSIPHeaderL( 
                SIPStrings::StringF(SipStrConsts::EAcceptHeader), 
                aAttributes );
			break;
			}
        case EAcceptContact:
            {
            HandleSIPHeaderL(
                SIPStrings::StringF(SipStrConsts::EAcceptContactHeader), 
                aAttributes );
            break;
            }
        case EAllowEvents:
            {
            HandleSIPHeaderL(
                SIPStrings::StringF(SipStrConsts::EAllowEventsHeader), 
                aAttributes );
            break;
            }
		case ELine:
			{
			HandleSdpLineL( aAttributes );
			break;
			}
		case ERtpmap:
			{
			HandleRtpmapL( aAttributes );
			break;
			}
		case EMediaAttribute:
			{
			HandleMediaAttributeL( aAttributes );
			break;
			}
		default:
			{
			// do nothing;
			break;
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPClientDataParser::OnEndElementL
// -----------------------------------------------------------------------------
//
void CSIPClientDataParser::OnEndElementL(
									const Xml::RTagInfo& aElement,
									TInt /*aErrorCode*/ )
	{
	// Get element name (e.g. "SIP_CLIENT") and change state (Exit)
	ChangeStateL( aElement.LocalName(), EFalse );
	}

// -----------------------------------------------------------------------------
// CSIPClientDataParser::ChangeStateL
// -----------------------------------------------------------------------------
//
void CSIPClientDataParser::ChangeStateL( const RString& aElementName,
										 TBool aEnter )
	{
	// Try to find a matching entry from the transitions table
	TInt count = iStateTransitions.Count();
	for( TInt i = 0; i < count; i++ )
		{
		const TStateTransition& transition = iStateTransitions[ i ];
		if( ( iState == transition.iState ) &&
			( aElementName == transition.iTag ) &&
			( aEnter == transition.iEnter ) )
			{
			// Match found, do transition
			iState = transition.iNextState;
			return;
			}
		}

	// No match was found, report error
	User::Leave( KErrCorrupt );
	}

// -----------------------------------------------------------------------------
// CSIPClientDataParser::HandleSIPClientL
// -----------------------------------------------------------------------------
//
void CSIPClientDataParser::HandleSIPClientL(
    const Xml::RAttributeArray& aAttributes )
	{
	// Verify that we have correct number of attributes
	__ASSERT_ALWAYS( aAttributes.Count() == 1, User::Leave( KErrCorrupt ) );
	
	// Verify that we have the correct attributes
	const RString& allowName = aAttributes[ 0 ].Attribute().LocalName();
	const RString& allowValue = aAttributes[ 0 ].Value();
	if( allowName != iTagAllowStarting )
		{
		User::Leave( KErrCorrupt );
		}
	iClientData->SetAllowStarting( ( allowValue == iTagYes ) );
	}

// -----------------------------------------------------------------------------
// CSIPClientDataParser::HandleSIPHeaderL
// -----------------------------------------------------------------------------
//
void CSIPClientDataParser::HandleSIPHeaderL(
    RStringF aHeaderName,
    const Xml::RAttributeArray& aAttributes )
	{
	// Verify that we have correct number of attributes
	if( aAttributes.Count() != 1 )
		{
		User::Leave( KErrCorrupt );
		}

	// Read header value
	const RString& value = aAttributes[ 0 ].Value();

	// Create SIP header object
	RPointerArray< CSIPHeaderBase > headers = 
	    SIPHeaderLookup::CreateHeaderL( aHeaderName, value.DesC() );
	// Add created headers to ClientData
	CSIPHeaderBase::PushLC( &headers );
	while( headers.Count() > 0 )
		{
		CSIPHeaderBase* header = headers[ 0 ];
		if ( header->Name() == 
		     SIPStrings::StringF(SipStrConsts::EAcceptContactHeader) )
		    {
		    CSIPAcceptContactHeader* acceptContact = 
		        static_cast< CSIPAcceptContactHeader* >( header );
		    iClientData->SIPFeatureSet().AddFeaturesL( *acceptContact );
		    delete header;
		    }
		else
		    {
		    iClientData->SIPHeaders().AppendL( header );
		    }
		headers.Remove( 0 );
		}
	CleanupStack::PopAndDestroy(); // headers
	}

// -----------------------------------------------------------------------------
// CSIPClientDataParser::HandleSdpLineL
// -----------------------------------------------------------------------------
//
void CSIPClientDataParser::HandleSdpLineL(
    const Xml::RAttributeArray& aAttributes )
	{
	// Verify that we have correct number of attributes
	if( aAttributes.Count() != KSdpLineAttrCount )
		{
		User::Leave( KErrCorrupt );
		}

	// Verify that we have the correct attribute(s)
	if( aAttributes[ 0 ].Value() != iTagLineM )
		{
		User::Leave( KErrCorrupt );
		}

	// Get attribute and convert to a form compatible with CSdpMediaField
	// (basically just surround the string with "m=" and "\r\n")
	TPtrC8 value( aAttributes[ 1 ].Value().DesC() );	                        
	HBufC8* mediaFieldBuf = CreateSdpFieldBufLC( KSdpMediaFieldStart, value );

	// Read and decode SDP media field to ClientData
	CSdpMediaField* mediaField = CSdpMediaField::DecodeLC( *mediaFieldBuf );
	iClientData->AddMediaFieldL( mediaField );
	CleanupStack::Pop( mediaField );
	
	CleanupStack::PopAndDestroy( mediaFieldBuf );
	}

// -----------------------------------------------------------------------------
// CSIPClientDataParser::HandleRtpmapL
// -----------------------------------------------------------------------------
//	
void CSIPClientDataParser::HandleRtpmapL(
    const Xml::RAttributeArray& aAttributes )
    {
    CSdpDocument& sdp = iClientData->SdpDocument();
    if( aAttributes.Count() != 1 || sdp.MediaFields().Count() == 0 )
        {
        User::Leave( KErrCorrupt );
        }
        
	// Get attribute and convert to a form compatible 
	// with CSdpfmtAttributeField
	// (basically just surround the string with "a=" and "\r\n")
	TPtrC8 value( aAttributes[ 0 ].Value().DesC() );
	HBufC8* fieldBuf = CreateSdpFieldBufLC( KSdpAttributeFieldStart, value );
	       
	// Decode and add rtpmap attribute field
    CSdpFmtAttributeField* attributeField = 
        CSdpFmtAttributeField::DecodeL( *fieldBuf );
    CleanupStack::PopAndDestroy( fieldBuf );
    CleanupStack::PushL( attributeField );    
        
    if( attributeField->Attribute().DesC().Compare( KRtpmapName ) != 0 )
        {
        User::Leave( KErrCorrupt );
        }
    
    // Add the rtpmap-attribute to the last added media field
    const TInt lastIndex = sdp.MediaFields().Count()-1;
    CSdpMediaField* lastMediaField = sdp.MediaFields()[ lastIndex ];
    lastMediaField->FormatAttributeFields().AppendL( attributeField );
    
    CleanupStack::Pop( attributeField );
    }

// -----------------------------------------------------------------------------
// CSIPClientDataParser::HandleMediaAttributeL
// -----------------------------------------------------------------------------
//
void CSIPClientDataParser::HandleMediaAttributeL( 
    const Xml::RAttributeArray& aAttributes )
    {
    CSdpDocument& sdp = iClientData->SdpDocument();
    if( aAttributes.Count() != 1 || sdp.MediaFields().Count() == 0 )
        {
        User::Leave( KErrCorrupt );
        }
        
	// Get attribute and convert to a form compatible with CSdpAttributeField
	// (basically just surround the string with "a=" and "\r\n")
	TPtrC8 value( aAttributes[ 0 ].Value().DesC() );
	HBufC8* fieldBuf = CreateSdpFieldBufLC( KSdpAttributeFieldStart, value );
	       
	// Decode and add attribute field
    CSdpAttributeField* attributeField = 
        CSdpAttributeField::DecodeL( *fieldBuf );
    CleanupStack::PopAndDestroy( fieldBuf );
    CleanupStack::PushL( attributeField );
    
    // Add the attribute to the last added media field
    const TInt lastIndex = sdp.MediaFields().Count()-1;
    CSdpMediaField* lastMediaField = sdp.MediaFields()[ lastIndex ];
    lastMediaField->AttributeFields().AppendL( attributeField );
    
    CleanupStack::Pop( attributeField );    
    }

// -----------------------------------------------------------------------------
// CSIPClientDataParser::CreateSdpFieldBufLC
// -----------------------------------------------------------------------------
//  
HBufC8* CSIPClientDataParser::CreateSdpFieldBufLC(
    const TDesC8& aFieldStart,
    const TDesC8& aValue )
    {
	TInt bufLength = aFieldStart.Length() + aValue.Length() + KCrLf().Length();
	HBufC8* buf = HBufC8::NewLC( bufLength );
	TPtr8 bufPtr = buf->Des();
	bufPtr.Copy( aFieldStart );
	bufPtr.Append( aValue );
	bufPtr.Append( KCrLf );
	return buf; 
    }
