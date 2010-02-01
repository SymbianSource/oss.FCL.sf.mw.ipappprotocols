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
* Name          : sipminexpiresheader.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPMINEXPIRESHEADER_H
#define CSIPMINEXPIRESHEADER_H

//  INCLUDES
#include "sipunsignedintheaderbase.h"
#include "_sipcodecdefs.h"

// CLASS DECLARATION
/**
* Class for a SIP Min-Expires header.
*
* @lib sipcodec.lib
*/
class CSIPMinExpiresHeader : public CSIPUnsignedIntHeaderBase
	{
	public:	// Constructors and destructor

		/**
		* Constructs a CSIPMinExpiresHeader from textual representation 
		* of the header's value part.
		* @param aValue a value part of a "Min-Expires"-header (e.g. "3600")
		* @returns a new instance of CSIPMinExpiresHeader
		*/
		IMPORT_C static CSIPMinExpiresHeader* DecodeL(const TDesC8& aValue);
	
		/**
		* Constructor
		* @param aValue the value to set
		*/
		IMPORT_C CSIPMinExpiresHeader (TUint aValue);
	
		/**
		* Destructor
		*/
		IMPORT_C ~CSIPMinExpiresHeader();
	

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

	private: // For testing purposes
	
		UNIT_TEST(CSIPMinExpiresHeaderTest)
	};

#endif // end of CSIPMINEXPIRESHEADER_H

// End of File
