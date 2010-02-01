/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name          : CSIPNotifyXmlBodyParser.h
* Part of       : ProfileAgent
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef __CSIPNOTIFYXMLBODYPARSER_H__
#define __CSIPNOTIFYXMLBODYPARSER_H__

// INCLUDES
#include <e32base.h>
#include <xml/parser.h>
#include <xml/contenthandler.h>

// FORWARD DECLARATIONS
class CSIPRegInfoElement;
class CSIPRegistrationElement;
class CSIPContactElement;

// ENUMERATIONS
/// Internal states for CSIPNotifyXmlBodyParser
enum TNotifyParserState
	{
	EParsingUnknown,
	EInit,
	EParsingXmlDocument,
	EParsingRegInfo,
	EParsingRegistration,
	EParsingContact,
	EParsingUri
	};

// CLASS DEFINITIONS
/**
 * CSIPNotifyXmlBodyParser implements an XML parser for CSIPClientData.
 * The XML DTD style is shown below.
 *
 * @code
 * <?xml version="1.0" encoding="UTF-8"?>
 *  <reginfo xmlns="urn:ietf:params:xml:ns:reginfo" version="1" state="full">
 *   <registration aor="sip:user1_public1@home1.net" id="a7" state="active">
 *    <contact id="76" state="active" event="registered">
 *     <uri>sip:[5555::aaa:bbb:ccc:ddd]</uri>
 *    </contact>
 *   </registration>
 *   <registration aor="sip:user1_public2@home1.net" id="a8" state="active">
 *    <contact id="77" state="active" event="created">
 *     <uri>sip:[5555::aaa:bbb:ccc:ddd]</uri>
 *    </contact>
 *   </registration>
 *   <registration aor="tel:+358504821437" id="a9" state="active">
 *    <contact id="78" state="active" event="created">
 *     <uri>sip:[5555::aaa:bbb:ccc:ddd]</uri>
 *    </contact>
 *   </registration>
 * </reginfo>
 * @endcode
 *
 */
class CSIPNotifyXmlBodyParser
	: public CBase,
	  public Xml::MContentHandler
	{
	public: // Constructors and destructor

		/**
		 * Static constructor.
		 *
		 * @return An initialized instance of this class.
		 */
		static CSIPNotifyXmlBodyParser* NewL();

		/// Destructor.
		~CSIPNotifyXmlBodyParser();

	public: // New methods

		/**
		 * Gets a reference to the string pool used.
		 *
		 * @return a reference to string pool.
		 */
        RStringPool& StringPool();

		/**
		 * Parse an XML document extracting information into
		 * a CSIPClientData object.
		 *
		 * @param aClientData parsed data will be added to this object
		 *        The ownership is not transferred.
		 * @param aXmlDocument
		 */
		void ParseL( CSIPRegInfoElement* aNotifyData,
			         const TDesC8& aXmlDocument );

	public: // From Xml::MContentHandler

		void OnStartDocumentL( const Xml::RDocumentParameters& aDocParam,
							   TInt aErrorCode );
		void OnEndDocumentL( TInt /*aErrorCode*/ ) {};
		void OnStartElementL( const Xml::RTagInfo& aElement,
							  const Xml::RAttributeArray& aAttributes,
							  TInt aErrorCode );
		void OnEndElementL( const Xml::RTagInfo& aElement, TInt aErrorCode );
		void OnContentL( const TDesC8& aBytes, TInt aErrorCode );
		void OnStartPrefixMappingL( const RString& /*aPrefix*/,
									const RString& /*aUri*/,
									TInt /*aErrorCode*/ ) {};
		void OnEndPrefixMappingL( const RString& /*aPrefix*/,
								  TInt /*aErrorCode*/ ) {};
		void OnIgnorableWhiteSpaceL( const TDesC8& /*aBytes*/,
									 TInt /*aErrorCode*/ ) {};
		void OnSkippedEntityL( const RString& /*aName*/,
							   TInt /*aErrorCode*/ ) {};
		void OnProcessingInstructionL( const TDesC8& /*aTarget*/,
									   const TDesC8& /*aData*/,
									   TInt /*aErrorCode*/ ) {};
		void OnError( TInt aErrorCode );
		TAny* GetExtendedInterface( const TInt32 /*aUid*/ ) { return NULL; };

	private: // New methods

		void ChangeState( const RString& aElementName, TBool aEnter );
		void HandleRegInfoL( const Xml::RTagInfo& aElement,
							 const Xml::RAttributeArray& aAttributes );
		void HandleRegistrationL( const Xml::RAttributeArray& aAttributes );
		void HandleContactL( const Xml::RAttributeArray& aAttributes );
		void HandleUriL( const TDesC8& aBytes );
		TInt FindAttributeIndex( const TDesC8& aName,
		                         const Xml::RAttributeArray& aAttributes) const;

	private:  // Constructors

		/// Default constructor
		CSIPNotifyXmlBodyParser();

		/// 2nd phase constructor.
		void ConstructL();
		
		TUint ConvertToTUintL( const TDesC8& aValue );

	private: // private data

		/// XML Parser object. Owned.
		Xml::CParser* iParser;

		/// Temporary pointer to Client data object. Not owned.
		CSIPRegInfoElement* iNotifyData;
		CSIPRegistrationElement* iRegElement;
		CSIPContactElement* iContact;
		/// Parsing state, for XML validation.
		TNotifyParserState iState;
        TNotifyParserState iPreviousState;
        TInt iDepthOfUnknownElements;
	};

#endif // __CSIPNOTIFYXMLBODYPARSER_H__

