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
* Name          : sipmaxforwardsheader.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPMAXFORWARDSHEADER_H
#define CSIPMAXFORWARDSHEADER_H

//  INCLUDES
#include "sipunsignedintheaderbase.h"
#include "_sipcodecdefs.h"

// CLASS DECLARATION
/**
* Class for a SIP Max-Forwards header.
*
* @lib sipcodec.lib
*/
class CSIPMaxForwardsHeader : public CSIPUnsignedIntHeaderBase
	{
	public:	// Constructors and destructor

		/**
		* Constructs a CSIPMaxForwardsHeader from textual representation 
		* of the header's value part.
		* @param aValue a value part of a "Max-Forwards"-header (e.g. "70")
		* @returns a new instance of CSIPMaxForwardsHeader
		*/
		IMPORT_C static CSIPMaxForwardsHeader* DecodeL(const TDesC8& aValue);
	
		/**
		* Constructor
		* @param aValue the value to set
		*/
		IMPORT_C CSIPMaxForwardsHeader(TUint aValue);

		/**
		* Destructor
		*/
		IMPORT_C ~CSIPMaxForwardsHeader();


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
	};

#endif // end of CSIPMAXFORWARDSHEADER_H

// End of File
