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
* Name          : sipviaheader.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPVIAHEADER_H
#define CSIPVIAHEADER_H

//  INCLUDES
#include "sipparameterheaderbase.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class CSIPViaHeaderParams;
class CSIPHostPort;

// CLASS DECLARATION
/**
* Class for a SIP Via-header.
*
* @lib sipcodec.lib
*/
class CSIPViaHeader : public CSIPParameterHeaderBase
	{
	public:	// Constructors and destructor

		/**
		* Constructs a CSIPViaHeader from textual representation 
		* of the header's value part.
		* @param aValue a value part of a "Via"-header
		* @return an array containing 1..n instances of CSIPViaHeader
		*/
		IMPORT_C static RPointerArray<CSIPViaHeader> DecodeL(const TDesC8& aValue);

		/**
		* Creates a CSIPViaHeader instance with default sent-protocol "SIP/2.0"
		* @param aTransport a transport to set e.g. "UDP"
		* @param aSentByHostPort the ownership is transferred, @see CSIPHostPort
		* @return a new instance of CSIPViaHeader
		*/
		IMPORT_C static CSIPViaHeader* NewL(RStringF aTransport,
		                                    CSIPHostPort* aSentByHostPort);

		/**
		* Creates a CSIPViaHeader instance with default sent-protocol "SIP/2.0"
		* and pushes the created object to CleanupStack.
		* @param aTransport a transport to set e.g. "UDP"
		* @param aSentByHostPort the ownership is transferred, @see CSIPHostPort
		* @return a new instance of CSIPViaHeader
		*/
		IMPORT_C static CSIPViaHeader* NewLC(RStringF aTransport,
		                                     CSIPHostPort* aSentByHostPort);

		/**
		* Destructor
		*/
		IMPORT_C ~CSIPViaHeader();


	public: // New functions

		/**
		* Compares this instance to another "Via" header object
		* @param aSIPViaHeader a header to compare to
		* @return ETrue if "Via" headers are similar
		*/
		IMPORT_C TBool operator==(const CSIPViaHeader& aSIPViaHeader);

		/**
		* Gets the transport parameter from the "Via" header
		* @returns a transport parameter
		*/
		IMPORT_C RStringF Transport() const;

		/**
		* Sets the transport parameter in the "Via" header
		* @param aTransport a transport-parameter
		*/
		IMPORT_C void SetTransportL(RStringF aTransport);

		/**
		* Gets the sent-by hostport from the "Via" header
		* @returns a sent-by hostport @see CSIPHostPort
		*/
		IMPORT_C CSIPHostPort& SentByHostPort();

		/**
		* Gets the protocol-name from the "Via" header e.g. "SIP"
		* @returns protocol-name
		*/
		IMPORT_C RStringF SentProtocolName() const;

		/**
		* Gets the protocol-version from the "Via" header e.g. "2.0"
		* @returns protocol-version
		*/
		IMPORT_C RStringF SentProtocolVersion() const;

		/**
		* Sets the protocol-version in the "Via" header
		* @param aVersion protocol version e.g. "2.0"
		*/
		IMPORT_C void SetSentProtocolVersionL(RStringF aVersion);

		/**
		* Gets the value of the "ttl" parameter
		* @return the value of the "ttl" parameter or 
		*         KErrNotFound if the ttl parameter is not present.
		*/	
		IMPORT_C TInt TtlParam() const;

		/**
		* Sets the value of the "ttl" parameter
		* @pre aValue >= 0
		* @param aValue the value of the "ttl" parameter
		*/	
		IMPORT_C void SetTtlParamL(TInt aValue);


	public: // From CSIPHeaderBase

		/**
		* From CSIPHeaderBase CloneL
		*/
		IMPORT_C CSIPHeaderBase* CloneL() const;
		
		/**
		* From CSIPHeaderBase Name
		*/		
		IMPORT_C RStringF Name() const;

		/**
		* From CSIPHeaderBase ExternalizeSupported
		*/
		IMPORT_C TBool ExternalizeSupported() const;


	public: // New functions, for internal use

		static RPointerArray<CSIPHeaderBase> BaseDecodeL(const TDesC8& aValue);

	public: // From CSIPHeaderBase, for internal use
	
		TBool MoreThanOneAllowed() const;	
		TBool HasCompactName() const;
		RStringF CompactName() const;
		TPreferredPlace PreferredPlaceInMessage() const;

	private: // From CSIPParameterHeaderBase

		HBufC8* ToTextMandatoryPartLC() const;
		void ParseMandatoryPartL(const TDesC8& aMandatoryPart);
		const CSIPParamContainerBase& Params() const;
		CSIPParamContainerBase& Params();

	private: // Constructors

		CSIPViaHeader ();
		void ConstructL();
		void ConstructL(RStringF aTransport, CSIPHostPort* aSentByHostPort);
		void ConstructL(const CSIPViaHeader& aViaHeader);

	private: // New functions 

		void ParseSentProtocolL(const TDesC8& aValue, TInt& aLength);
		void SetTransportL(const TDesC8& aTransport);
		void SetSentProtocolVersionL(const TDesC8& aVersion);
		void SetSentProtocolNameL(const TDesC8& aName);	

	private: // Data

		RStringF iSentProtocolName;
		RStringF iSentProtocolVersion;
		RStringF iTransport;
		CSIPHostPort* iSentByHostPort;
		CSIPViaHeaderParams* iParams;

	private: // For testing purposes
	
		UNIT_TEST(CSIPViaHeaderTest)
	};

#endif // CSIPVIAHEADER_H

// End of File
