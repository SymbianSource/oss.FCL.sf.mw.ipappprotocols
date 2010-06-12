/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

#ifndef CMSRPTOPATHHEADER_H
#define CMSRPTOPATHHEADER_H

// INTERNAL INCLUDES
#include "CMsrpFromToHeaderBase.h"

// CLASS DECLARATION
/**
* @publishedAll
*
* Class provides functions for setting and getting MSRP "To-Path" header field
*
* @lib msrpclient.lib
*/
class CMSRPToPathHeader : public CMSRPFromToHeaderBase
	{
	public:	// Constructors and destructor

		/**
		* Constructs a CMSRPToPathHeader from textual representation
		* of the header's value part.
		* @param aValue a value part of a "To-Path"-header
		* @return a new instance of CMSRPToPathHeader
		*/
		IMPORT_C static CMSRPToPathHeader* DecodeL( const TDesC8& aValue );

		/**
		* Creates a new instance of CMSRToPathHeader
		* @param aUri URI of MSRP message receiver
		* @return a new instance of CMSRPToPathHeader
		*/
		IMPORT_C static CMSRPToPathHeader* NewL( CUri8* aUri );

		/**
		* Creates a new instance of CMSRPToPathHeader and puts it to CleanupStack
		* @param aUri URI of MSRP message receiver
		* @return a new instance of CMSRPTopathHeader
		*/
		IMPORT_C static CMSRPToPathHeader* NewLC( CUri8* aUri );

		/**
		* Creates a new instance of CMSRToPathHeader
		* @return a new instance of CMSRPToPathHeader
		*/
		static CMSRPToPathHeader* NewL( );

		/**
		* Destructor, deletes the resources of CMSRPToPathHeader.
		*/
		IMPORT_C ~CMSRPToPathHeader();


	public: // New functions

		/**
		* Constructs an instance of a CMSRPTopathHeader from a RReadStream
		* @param aReadStream a stream containing the value of the
		*        externalized object (header name not included).
		* @return an instance of a CMSRPTopathHeader
		*/
		IMPORT_C static CMSRPToPathHeader*
			InternalizeValueL( RReadStream& aReadStream );


	protected: // From CMSRPHeaderBase

		/**
		* From CMSRPHeaderBase Name
		*/				 
		RStringF Name() const;

	protected: // constructors

		/**
		* Constructor
		*/
		void ConstructL( CUri8* aUri );

		/**
		* Constructor
		*/
		void ConstructL( );

	private: // Constructors

		/**
		* Constructor
		*/
		CMSRPToPathHeader();

	};

#endif // CMSRPTOPATHHEADER_H

// End of File
