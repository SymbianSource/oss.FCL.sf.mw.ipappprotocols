// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPNotifyXmlBodyParser.cpp
// Part of       : ProfileAgent
// Version       : SIP/4.0
//



#include <uri8.h>
#include "CSIPNotifyXmlBodyParser.h"
#include "sipreginfoelement.h"
#include "sipregistrationelement.h"
#include "sipcontactelement.h"
#include <xml/documentparameters.h>

_LIT8( KMimeTypeXml, "text/xml" );

//Elements
_LIT8(KTagRegInfo, "reginfo");
_LIT8(KTagRegistration, "registration");
_LIT8(KTagContact, "contact");
_LIT8(KTagUri, "uri");
//Attributes
_LIT8(KTagVersion, "version");
_LIT8(KTagId, "id");
_LIT8(KTagState, "state");
_LIT8(KTagAor, "aor");
_LIT8(KTagEvent, "event");
_LIT8(KTagExpires, "expires");
//attribute values
_LIT8(KTagFull, "full");
_LIT8(KTagPartial, "partial");
_LIT8(KTagActive, "active");
_LIT8(KTagTerminated, "terminated");
_LIT8(KTagRegistered, "registered");
_LIT8(KTagCreated, "created");
_LIT8(KTagRefreshed, "refreshed");
_LIT8(KTagShortened, "shortened");
_LIT8(KTagDeactivated, "deactivated");
_LIT8(KTagProbation, "probation");
_LIT8(KTagUnregistered, "unregistered");
_LIT8(KTagRejected, "rejected");
_LIT8(KTagExpired, "expired");

const TInt KValueTwo = 2;
const TInt KValueThree = 3;

// current state, tag, enter, next state
struct TNotifyStateTransition
	{
	public: // Data

		const TDesC8* iTag;
		TUint16 iState;
		TUint8 iEnter;
		TUint8 iNextState;
	};
	
// -----------------------------------------------------------------------------
// CSIPNotifyXmlBodyParser::NewL
// -----------------------------------------------------------------------------
//
CSIPNotifyXmlBodyParser* CSIPNotifyXmlBodyParser::NewL()
	{
	CSIPNotifyXmlBodyParser* self = new( ELeave ) CSIPNotifyXmlBodyParser;
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPNotifyXmlBodyParser::CSIPNotifyXmlBodyParser
// -----------------------------------------------------------------------------
//
CSIPNotifyXmlBodyParser::CSIPNotifyXmlBodyParser()
	{
	}

// -----------------------------------------------------------------------------
// CSIPNotifyXmlBodyParser::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPNotifyXmlBodyParser::ConstructL()
	{
	iParser = Xml::CParser::NewL( KMimeTypeXml, *this );
	}

// -----------------------------------------------------------------------------
// CSIPNotifyXmlBodyParser::~CSIPNotifyXmlBodyParser
// -----------------------------------------------------------------------------
//
CSIPNotifyXmlBodyParser::~CSIPNotifyXmlBodyParser()
	{
	delete iRegElement;
	delete iContact;
	delete iParser;
	}

// -----------------------------------------------------------------------------
// CSIPNotifyXmlBodyParser::StringPool
// -----------------------------------------------------------------------------
//
RStringPool& CSIPNotifyXmlBodyParser::StringPool()
    {
    return iParser->StringPool();
    }

// -----------------------------------------------------------------------------
// CSIPNotifyXmlBodyParser::ParseL
// -----------------------------------------------------------------------------
//
void CSIPNotifyXmlBodyParser::ParseL( CSIPRegInfoElement* aNotifyData,
								      const TDesC8& aXmlDocument )
	{
	__ASSERT_ALWAYS( aXmlDocument.Length(), User::Leave( KErrCorrupt ) );
	//aNotifyData ownership is NOT transfered!
	iNotifyData = aNotifyData;
	iState = EInit;
	iPreviousState = EInit;
	iDepthOfUnknownElements = 0;
	
	// Parse the specified document into iNotifyData
	Xml::ParseL( *iParser, aXmlDocument );
	
	__ASSERT_ALWAYS( iNotifyData->HasVersion(), User::Leave( KErrCorrupt ) );
    __ASSERT_ALWAYS( iNotifyData->State().DesC().Length(), 
                     User::Leave( KErrCorrupt ) );

	iContact = NULL;	
	iRegElement = NULL;
	iNotifyData = NULL;
	}

// -----------------------------------------------------------------------------
// CSIPNotifyXmlBodyParser::OnStartElementL
// -----------------------------------------------------------------------------
//
void CSIPNotifyXmlBodyParser::OnStartElementL(
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    TInt aErrorCode )
	{
	__ASSERT_ALWAYS( !aErrorCode, User::Leave( KErrCorrupt ) );
	// Get element name (e.g. "reginfo") and change state (Enter)
	ChangeState( aElement.LocalName(), ETrue );

	switch( iState )
		{
		case EParsingRegInfo:
			{
			HandleRegInfoL( aElement, aAttributes );
			break;
			}
		case EParsingRegistration:
			{
			HandleRegistrationL( aAttributes );
			break;
			}
		case EParsingContact:
			{
			HandleContactL( aAttributes );
			break;
			}
		default: // do nothing
			break;
		}
	}

// -----------------------------------------------------------------------------
// CSIPNotifyXmlBodyParser::OnEndElementL
// -----------------------------------------------------------------------------
//
void CSIPNotifyXmlBodyParser::OnEndElementL( const Xml::RTagInfo& aElement,
									         TInt aErrorCode )
	{
	__ASSERT_ALWAYS( !aErrorCode, User::Leave( KErrCorrupt ) );
	
	// Get element name (e.g. "reginfo") and change state (Exit)
	if ( iState == EParsingRegistration && iRegElement )
		{
		CSIPRegistrationElement* regElement = iRegElement;
		iRegElement = NULL;
		CleanupStack::PushL( regElement );		
		iNotifyData->RegistrationElements().AppendL( regElement );
		CleanupStack::Pop( regElement );
		}
		
	if ( iState == EParsingContact && iRegElement && iContact )
		{
		CSIPContactElement* contact = iContact;
		iContact = NULL;
		CleanupStack::PushL( contact );
		iRegElement->ContactElements().AppendL( contact );
		CleanupStack::Pop( contact );
		}

	if ( iState == EParsingUri && iContact )
		{
		//Check if there was a value
		if ( iContact->Uri8().Uri().UriDes().Length() == 0 )
			{
			User::Leave( KErrCorrupt );
			}
		}

	ChangeState( aElement.LocalName(), EFalse );
	}

// -----------------------------------------------------------------------------
// CSIPNotifyXmlBodyParser::OnStartDocumentL
// -----------------------------------------------------------------------------
//
void CSIPNotifyXmlBodyParser::OnStartDocumentL( 
    const Xml::RDocumentParameters& /*aDocParam*/,
    TInt aErrorCode )
	{
	__ASSERT_ALWAYS( !aErrorCode, User::Leave( KErrCorrupt ) );

	iState = EParsingXmlDocument;
	}

// -----------------------------------------------------------------------------
// CSIPNotifyXmlBodyParser::OnError
// -----------------------------------------------------------------------------
//
void CSIPNotifyXmlBodyParser::OnError( TInt /*aErrorCode*/ )
	{
	}
	
// -----------------------------------------------------------------------------
// CSIPNotifyXmlBodyParser::OnContentL
// -----------------------------------------------------------------------------
//
void CSIPNotifyXmlBodyParser::OnContentL( const TDesC8& aBytes,
                                          TInt aErrorCode )
	{
	__ASSERT_ALWAYS( !aErrorCode, User::Leave( KErrCorrupt ) );
	if ( iState == EParsingUri )
		{
		HandleUriL( aBytes );
		}
	}

// -----------------------------------------------------------------------------
// CSIPNotifyXmlBodyParser::ChangeState
// -----------------------------------------------------------------------------
//
void CSIPNotifyXmlBodyParser::ChangeState( const RString& aElementName,
										   TBool aEnter )
	{
	// Table based XML validation transitions definition
	const TNotifyStateTransition KNotifyTransitions[] =
	    {
	    { &KTagRegInfo, EParsingXmlDocument, ETrue, EParsingRegInfo },
	    { &KTagRegistration, EParsingRegInfo, ETrue, EParsingRegistration },
	    { &KTagContact, EParsingRegistration, ETrue, EParsingContact },
	    { &KTagUri, EParsingContact, ETrue, EParsingUri },
	    { &KTagUri, EParsingUri, EFalse, EParsingContact },	
	    { &KTagContact, EParsingContact, EFalse, EParsingRegistration },
	    { &KTagRegistration, EParsingRegistration, EFalse, EParsingRegInfo },
	    { &KTagRegInfo, EParsingRegInfo, EFalse, EInit }
	    };
	
	// Try to find a matching entry from the transitions table
	TInt count = sizeof( KNotifyTransitions ) / sizeof( TNotifyStateTransition );
	for( TInt i = 0; i < count; i++ )
		{
		const TNotifyStateTransition& transition = KNotifyTransitions[ i ];
		if( ( iState == transition.iState ) &&
			( aElementName.DesC() == *transition.iTag ) &&
			( aEnter == transition.iEnter ) )
			{
			// Match found, do transition
			iState = (TNotifyParserState)transition.iNextState;
			return;
			}
		}
		
	if ( aEnter )
	    {
	    if ( iDepthOfUnknownElements++ == 0 )
	        {
	        iPreviousState = iState;
	        iState = EParsingUnknown;     
	        }
	    }
	else
	    {
	    if ( --iDepthOfUnknownElements == 0 )
	        {
	        iState = iPreviousState;
	        }
	    }
	}
	
// -----------------------------------------------------------------------------
// CSIPNotifyXmlBodyParser::HandleRegInfoL
// -----------------------------------------------------------------------------
//
void CSIPNotifyXmlBodyParser::HandleRegInfoL( 
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes )
	{
	
	//URI/ URN
	_LIT8(KUriNamespace, "urn:ietf:params:xml:ns:reginfo");
	if( aElement.Uri().DesC() != KUriNamespace )
		{
		User::Leave( KErrCorrupt );
		}
	// Verify that we have correct number of attributes
	if( aAttributes.Count() < KValueTwo )
		{
		User::Leave( KErrCorrupt );
		}
		
	// Verify that we have the correct attribute(s)
	TInt versionIndex = FindAttributeIndex( KTagVersion, aAttributes );
	if ( versionIndex < 0 )
		{
		User::Leave( KErrCorrupt );
		}	
			
	iNotifyData->SetVersion( 
	    ConvertToTUintL( aAttributes[ versionIndex ].Value().DesC() ) );	
	
	TInt stateIndex = FindAttributeIndex( KTagState, aAttributes );
	if ( stateIndex < 0 )
		{
		User::Leave( KErrCorrupt );
		}
	
	RString stateValue = aAttributes[ stateIndex ].Value();
	if ( stateValue.DesC() != KTagFull && stateValue.DesC() != KTagPartial )
		{
		User::Leave( KErrCorrupt );
		}
		
	iNotifyData->SetState( stateValue );
	}

// -----------------------------------------------------------------------------
// CSIPNotifyXmlBodyParser::HandleRegistrationL
// -----------------------------------------------------------------------------
//
void CSIPNotifyXmlBodyParser::HandleRegistrationL( 
    const Xml::RAttributeArray& aAttributes )
	{
	// Verify that we have correct number of attributes
	if( aAttributes.Count() < KValueThree )
		{
		User::Leave( KErrCorrupt );
		}		
		
	// Verify that we have the correct attribute(s)
	TInt aorIndex = FindAttributeIndex( KTagAor, aAttributes );
	if ( aorIndex < 0 )
		{
		User::Leave( KErrCorrupt );
		}
	
	TInt idIndex = FindAttributeIndex( KTagId, aAttributes );
	if ( idIndex < 0 )
		{
		User::Leave( KErrCorrupt );
		}		
	
	TInt stateIndex = FindAttributeIndex( KTagState, aAttributes );
	if ( stateIndex < 0 )
		{
		User::Leave( KErrCorrupt );
		}	
		
	if ( aAttributes[ stateIndex ].Value().DesC() != KTagActive && 
	     aAttributes[ stateIndex ].Value().DesC() != KTagTerminated)
		{
		User::Leave( KErrCorrupt );
		}	
		
	delete iRegElement;
	iRegElement = NULL;
	iRegElement = CSIPRegistrationElement::NewL();
	
	TUriParser8 parser;
	if ( aAttributes[ aorIndex ].Value().DesC().Length() == 0 )
		{
		User::Leave( KErrCorrupt );
		}
    User::LeaveIfError( 
        parser.Parse( aAttributes[ aorIndex ].Value().DesC() ) );
    CUri8* uri8 = CUri8::NewLC( parser );
	iRegElement->SetAORL( uri8 );
	CleanupStack::Pop( uri8 );
		
	iRegElement->SetState( aAttributes[ stateIndex ].Value() );		
	}


// -----------------------------------------------------------------------------
// CSIPNotifyXmlBodyParser::HandleContactL
// -----------------------------------------------------------------------------
//
void CSIPNotifyXmlBodyParser::HandleContactL(
    const Xml::RAttributeArray& aAttributes )
	{
	__ASSERT_ALWAYS( iRegElement ,User::Leave( KErrCorrupt ));
	// Verify that we have correct number of attributes
	if ( aAttributes.Count() < KValueThree )
		{
		User::Leave( KErrCorrupt );
		}
				
	// Verify that we have the correct attribute(s)
	// id
	TInt idIndex = FindAttributeIndex( KTagId, aAttributes );
	if ( idIndex < 0 )
		{
		User::Leave( KErrCorrupt );
		}

    // State
	TInt stateIndex = FindAttributeIndex( KTagState, aAttributes );
	if ( stateIndex < 0 )
		{
		User::Leave( KErrCorrupt );
		}		
	if ( aAttributes[ stateIndex ].Value().DesC() != KTagActive && 
	     aAttributes[ stateIndex ].Value().DesC() != KTagTerminated)
		{
		User::Leave( KErrCorrupt );
		}
	delete iContact;
	iContact = NULL;
	iContact = CSIPContactElement::NewL();		
	iContact->SetState( aAttributes[ stateIndex ].Value() );
	
	// event
	TInt eventIndex = FindAttributeIndex( KTagEvent, aAttributes );
	if ( eventIndex < 0 )
		{
		User::Leave( KErrCorrupt );
		}
	RString event = aAttributes[ eventIndex ].Value();
	if ( event.DesC() != KTagRegistered && 
	     event.DesC() != KTagCreated &&
	     event.DesC() != KTagRefreshed &&
	     event.DesC() != KTagShortened && 
	     event.DesC() != KTagDeactivated &&
	     event.DesC() != KTagProbation &&
	     event.DesC() != KTagUnregistered &&
	     event.DesC() != KTagRejected &&
	     event.DesC() != KTagExpired )
		{
		User::Leave( KErrCorrupt );
		}	
	iContact->SetEvent( event );	

	//Optional attributes
	TInt expiresIndex = FindAttributeIndex( KTagExpires, aAttributes );
	if ( expiresIndex >= 0 )
		{
        iContact->SetExpires( 
            ConvertToTUintL( aAttributes[ expiresIndex ].Value().DesC() ) );		
		}
	}

// -----------------------------------------------------------------------------
// CSIPNotifyXmlBodyParser::HandleUriL
// -----------------------------------------------------------------------------
//
void CSIPNotifyXmlBodyParser::HandleUriL( const TDesC8& aBytes )
	{
	__ASSERT_ALWAYS( iContact, User::Leave( KErrCorrupt ) );
	
	HBufC8* temp = aBytes.AllocLC();
	temp->Des().Trim();
	
	if ( temp->Length() == 0 )
		{
		User::Leave( KErrCorrupt );
		}
		
	TUriParser8 parser;
    User::LeaveIfError( parser.Parse( *temp ) );
    CUri8* uri8 = CUri8::NewLC( parser );
	iContact->SetUri8L( uri8 );
	CleanupStack::Pop( uri8 );
	CleanupStack::PopAndDestroy( temp );
	}
	
// -----------------------------------------------------------------------------
// CSIPNotifyXmlBodyParser::ConvertToTUintL
// -----------------------------------------------------------------------------
//
TUint CSIPNotifyXmlBodyParser::ConvertToTUintL( const TDesC8& aValue )
	{
	__ASSERT_ALWAYS( aValue.Length(), User::Leave( KErrCorrupt ) );
	TLex8 lex( aValue );
	TUint value = 0;
	if ( lex.Val( value ) != KErrNone)
		{
		User::Leave( KErrCorrupt );
		}
	return value;
	}
	
// -----------------------------------------------------------------------------
// CSIPNotifyXmlBodyParser::FindAttributeIndex
// -----------------------------------------------------------------------------
//	
TInt CSIPNotifyXmlBodyParser::FindAttributeIndex(
    const TDesC8& aName,
    const Xml::RAttributeArray& aAttributes) const
    {
    for (TInt i=0; i < aAttributes.Count(); i++)
        {
        if ( aAttributes[ i ].Attribute().LocalName().DesC() == aName )
            {
            return i;
            }
        }
    return KErrNotFound;
    }
