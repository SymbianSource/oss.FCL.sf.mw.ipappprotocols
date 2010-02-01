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
* Name        : sippaccessnetworkinfoheader.h
* Part of     : SIP Codec
* Interface   : SDK API, SIP Codec API
* Version     : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPPACCESSNETWORKINFOHEADER_H
#define CSIPPACCESSNETWORKINFOHEADER_H

//  INCLUDES
#include "sipparameterheaderbase.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class CSIPAddress;
class CSIPPAccessNetworkInfoHeaderParams;

// CLASS DECLARATION
/**
* Class provides functions for setting and getting parameters in SIP 
* "P-Access-Network-Info" header.
*
*  @lib sipcodec.lib
*/
class CSIPPAccessNetworkInfoHeader : public CSIPParameterHeaderBase
	{
	public:	// Constructors and destructor

		/**
		* Constructs a CSIPPAccessNetworkInfoHeader from textual representation 
		* of the header's value part.
		* @param aValue a value part of a "P-Access-Network-Info"-header 
		*        (e.g. "IEEE-802.11a;cgi-3gpp=\"qString\"")
		* @return a new instance of CSIPPAccessNetworkInfoHeader   
		*/
		IMPORT_C static CSIPPAccessNetworkInfoHeader* 
			DecodeL(const TDesC8& aValue);

		/**
		* Creates a new instance of CSIPPAccessNetworkInfoHeader
		* @pre aSIPAddress != 0
		* @param aAccessType an access type.
		* @return a new instance of CSIPPAccessNetworkInfoHeader
		*/
		IMPORT_C static CSIPPAccessNetworkInfoHeader* 
            NewL(RStringF aAccessType);

		/**
		* Creates a new instance of CSIPPAccessNetworkInfoHeader and 
        * puts it to CleanupStack
		* @param aAccessType an access type.
		* @return a new instance of CSIPPAccessNetworkInfoHeader
		*/
		IMPORT_C static CSIPPAccessNetworkInfoHeader* 
            NewLC(RStringF aAccessType);

		/**
		* Destructor, deletes the resources of CSIPPAccessNetworkInfoHeader.
		*/
		IMPORT_C virtual ~CSIPPAccessNetworkInfoHeader();


	public: // New functions

		/**
		* Sets the access-type
		* @param aAccessType an access type.
		*/
		IMPORT_C void SetAccessTypeL(RStringF aAccessType);

		/**
		* Gets the access-type as const
		* @return access-type
		*/
		IMPORT_C RStringF AccessType() const;


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


	public: // From CSIPHeaderBase, for internal use

		TPreferredPlace PreferredPlaceInMessage() const;

	public: // New functions, for internal use

		static RPointerArray<CSIPHeaderBase> BaseDecodeL(const TDesC8& aValue);

	private: // From CSIPParameterHeaderBase

		HBufC8* ToTextMandatoryPartLC() const;
		void ParseMandatoryPartL(const TDesC8& aMandatoryPart);
		const CSIPParamContainerBase& Params() const;
		CSIPParamContainerBase& Params();

	private: // Constructors

		CSIPPAccessNetworkInfoHeader();
		void ConstructL();		
		void ConstructL(RStringF aAccessType);
		void ConstructL(const CSIPPAccessNetworkInfoHeader& aHeader);

	private: // Data

		RStringF iAccessType;
		CSIPPAccessNetworkInfoHeaderParams* iParams;

	private: // For testing purposes
	
		UNIT_TEST(CSIPPAccessNetworkInfoHeaderTest)
	};

#endif // end of CSIPPACCESSNETWORKINFOHEADER_H

// End of File
