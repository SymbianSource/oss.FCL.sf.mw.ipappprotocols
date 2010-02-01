/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSIPClientDataParser.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/
#ifndef CSIPCLIENTDATAPARSER_H
#define CSIPCLIENTDATAPARSER_H

// INCLUDES
#include <e32base.h>
#include <xml/parser.h>
#include <xml/contenthandler.h>

// FORWARD DECLARATIONS
class CSIPClientData;

// ENUMERATIONS
/// Internal states for CSIPClientDataParser
enum TParserState
	{
	EInit,
	ESIPClient,
	ESIPHeaders,
	EAccept,
    EAcceptContact,
    EAllowEvents,
	ESDPLines,
	ELine,
	ERtpmap,
	EMediaAttribute
	};

// CLASS DEFINITIONS

// State class for XML parsing
// current state, tag, enter, next state
class TStateTransition
	{
	public: // Constructors

        TStateTransition(const RString& aTag,
                         TParserState aState,
                         TParserState aNextState,
                         TBool aEnter=ETrue)
            : iTag(aTag),
              iState(aState),
              iNextState(aNextState),
              iEnter(aEnter) {}

    public: // Data

		RString iTag;
		TParserState iState;
		TParserState iNextState;
		TBool iEnter;
	};

/**
 * CSIPClientDataParser implements an XML parser for CSIPClientData.
 * The XML style is shown below.
 *
 * @code
 * <SIP_CLIENT ALLOW_STARTING="YES">
 *   <SIP_HEADERS>
 *     <ACCEPT value="application/sdp"/>
 *     <ACCEPT value=""/>
 *   </SIP_HEADERS>
 *   <SDP_LINES>
 *     <LINE name="m" value="video 53000 RTP/AVP 32">
 *       <RTPMAP value="rtpmap:32 MPV/90000"/>
 *       <MEDIA_ATTRIBUTE value="extension"/>
 *     </LINE>
 *   </SDP_LINES>
 * </SIP_CLIENT>
 * @endcode
 *
 */
NONSHARABLE_CLASS(CSIPClientDataParser)
	: public CBase,
	  public Xml::MContentHandler
	{
	public: // Constructors and destructor

		/**
		 * Static constructor.
		 *
		 * @return An initialized instance of this class.
		 */
		static CSIPClientDataParser* NewL();

		/// Destructor.
		~CSIPClientDataParser();

	public: // New methods

		/**
		 * Parse an XML document extracting information into
		 * a CSIPClientData object.
		 *
		 * @param aClientData parsed data will be added to this object
		 *        The ownership is not transferred.
		 * @param aXmlDocument
		 */
		void ParseL( CSIPClientData* aClientData,
			         const TDesC8& aXmlDocument );

	public: // From Xml::MContentHandler

		void OnStartDocumentL( const Xml::RDocumentParameters& /*aDocParam*/,
							   TInt /*aErrorCode*/ ) {};
		void OnEndDocumentL( TInt /*aErrorCode*/ ) {};
		void OnStartElementL( const Xml::RTagInfo& aElement,
							  const Xml::RAttributeArray& aAttributes,
							  TInt aErrorCode );
		void OnEndElementL( const Xml::RTagInfo& aElement, TInt aErrorCode );
		void OnContentL( const TDesC8& /*aBytes*/, TInt /*aErrorCode*/ ) {};
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
		void OnError( TInt /*aErrorCode*/ ) {};
		TAny* GetExtendedInterface( const TInt32 /*aUid*/ ) { return NULL; };

	private: // New methods

		void ChangeStateL( const RString& aElementName, TBool aEnter );
		void HandleSIPClientL( const Xml::RAttributeArray& aAttributes );
		void HandleSIPHeaderL( RStringF aHeaderName, 
                               const Xml::RAttributeArray& aAttributes );
		void HandleSdpLineL( const Xml::RAttributeArray& aAttributes );
		void HandleRtpmapL( const Xml::RAttributeArray& aAttributes );
		void HandleMediaAttributeL( const Xml::RAttributeArray& aAttributes );
		HBufC8* CreateSdpFieldBufLC( const TDesC8& aFieldStart, 
		                             const TDesC8& aValue );

	private:  // Constructors

		/// Default constructor
		inline CSIPClientDataParser();

		/// 2nd phase constructor.
		inline void ConstructL();

	private: // private data

		/// XML Parser object. Owned.
		Xml::CParser* iParser;

		/// Temporary pointer to Client data object. Not owned.
		CSIPClientData* iClientData;

		/// ETrue when SIP header lookup has been opened
		TBool iSIPHeaderLookupOpened;

        /// RString constants used in XML 
        RString iTagSIPClient;
        RString iTagSIPHeaders;
        RString iTagSDPLines;
        RString iTagLine;
        RString iTagLineM;
        RString iTagRtpmap;
        RString iTagMediaAttribute;
        RString iTagAccept;
        RString iTagAllowStarting;
        RString iTagAcceptContact;
        RString iTagAllowEvents;
        RString iTagYes;

        /// An array of parser states
        RArray<TStateTransition> iStateTransitions;

		/// Parsing state, for XML validation.
		TParserState iState;
	};

#endif // CSIPCLIENTDATAPARSER_H

