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


#ifndef CMSRPFROMPATHHEADER_H
#define CMSRPFROMPATHHEADER_H

// INTERNAL INCLUDES
#include "CMsrpFromToHeaderBase.h"

/*
* @publishedAll
*
* Class provides functions for setting and getting MSRP "From-Path" header field
*
* @lib msrpclient.lib
*/
class CMSRPFromPathHeader : public CMSRPFromToHeaderBase
	{
	public:	// Constructors and destructor

		/**
		* Constructs a CMSRPFromPathHeader from textual representation
		* of the header's value part.
		* @param aUri a value part of a "From-Path"-header
		* @return a new instance of CMSRPFromPathHeader
		*/
		IMPORT_C static CMSRPFromPathHeader* DecodeL( const TDesC8& aUri );

		/**
		* Creates a new instance of CMSRFromPathHeader
		* @param aUri a value part of a "From-Path"-header
		* @return a new instance of CMSRPFromPathHeader
		*/
		IMPORT_C static CMSRPFromPathHeader* NewL( CUri8* aUri );

		/**
		* Creates a new instance of CMSRPFromPathHeader and puts it to CleanupStack
		* @param aUri a value part of a "From-Path"-header
		* @return a new instance of CMSRPFromPathHeader
		*/
		IMPORT_C static CMSRPFromPathHeader* NewLC( CUri8* aUri );

		/**
		* Creates a new instance of CMSRFromPathHeader
		* @return a new instance of CMSRPFromPathHeader
		*/
		static CMSRPFromPathHeader* NewL( );

		/**
		* Destructor, deletes the resources of CMSRPFromPathHeader.
		*/
		IMPORT_C ~CMSRPFromPathHeader();


	public: // New functions

		/**
		* Constructs an instance of a CMSRPFromPathHeader from a RReadStream
		* @param aReadStream a stream containing the value of the
		*        externalized object (header name not included).
		* @return an instance of a CMSRPFromPathHeader
		*/
		IMPORT_C static CMSRPFromPathHeader*
			InternalizeValueL( RReadStream& aReadStream );


	protected: // From CMSRPHeaderBase

		/**
		* From CMSRPHeaderBase Name
		*/
		RStringF Name() const;

	protected: // constructors

        /**
        * Constructor
		* @param aUri a value part of a "From-Path"-header
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
		CMSRPFromPathHeader();

	};

#endif // CMSRPFROMPATHHEADER_H

// End of File
