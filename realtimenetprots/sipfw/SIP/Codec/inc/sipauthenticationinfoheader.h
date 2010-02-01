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
* Name          : sipauthenticationinfoheader.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPAUTHENTICATIONINFOHEADER_H
#define CSIPAUTHENTICATIONINFOHEADER_H

//  INCLUDES
#include "sipparameterheaderbase.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class CSIPAuthenticationInfoHeaderParams;

// CLASS DECLARATION
/**
* Class for a SIP Authentication-Info header.
*
* @lib sipcodec.lib
*/
class CSIPAuthenticationInfoHeader : public CSIPHeaderBase
	{
	public:	// Constructors and destructor

		/**
		* Constructs a CSIPAuthenticationInfoHeader from textual representation
		* of the header's value part.
		* @param aValue a value part of the header
		* @returns a new instance of CSIPAuthenticationInfoHeader
		*/
		IMPORT_C static CSIPAuthenticationInfoHeader* 
			DecodeL(const TDesC8& aValue);

		/**
		* Destructor
		*/
		IMPORT_C ~CSIPAuthenticationInfoHeader();


	public: // New functions

		/**
		* Gets the value of the given parameter
		* @param aParam the name of the parameter
		* @returns the parameter value
		*/
		IMPORT_C RStringF Value(RStringF aParam) const;

		/**
		* Gets the value of the given parameter stored as a descriptor.
		* This is needed for parameters that have case sensitive values.
		* @param aParam the name of the parameter
		* @returns the parameter value
		*/
		IMPORT_C const TDesC8& DesValue(RStringF aParam) const;


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
		* From CSIPHeaderBase ToTextValueL
		*/
		IMPORT_C HBufC8* ToTextValueL() const;

		/**
		* From CSIPHeaderBase ExternalizeSupported
		*/
		IMPORT_C TBool ExternalizeSupported() const;


	public: // From CSIPHeaderBase, for internal use
	
        TBool EncodeMultipleToOneLine() const;
		TPreferredPlace PreferredPlaceInMessage() const;

	public: // New functions, for internal use

		static RPointerArray<CSIPHeaderBase> BaseDecodeL(const TDesC8& aValue);

	private: // Constructors

		CSIPAuthenticationInfoHeader();
		void ConstructL(const TDesC8& aValue);
		void ConstructL(const CSIPAuthenticationInfoHeader& aHeader);

	private: // Data

		CSIPAuthenticationInfoHeaderParams* iParams;

	private: // For testing purposes
	
		UNIT_TEST(CSIPAuthenticationInfoHeaderTest)
	};

#endif // CSIPAUTHENTICATIONINFOHEADER_H

// End of File
