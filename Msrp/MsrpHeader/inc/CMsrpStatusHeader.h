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

#ifndef CMSRPSTATUSHEADER_H
#define CMSRPSTATUSHEADER_H

//  INCLUDES
#include "CMsrpHeaderBase.h"

// CONSTANTS
const TInt KMaxLengthOfStatusCodeValue = 100;

// CLASS DECLARATION
/**
* @publishedAll
*
* Class provides functions for setting and getting MSRP "Status" header field
*
* @lib msrpclient.lib
*/
class CMSRPStatusHeader : public CMSRPHeaderBase
	{
	public:	// Constructors and destructor

		/**
		* Creates a new instance of CMSRMessageIdHeader
		* @param aStatusCode status code
		* @return a new instance of CMSRPStatusHeader
		*/
		IMPORT_C static CMSRPStatusHeader* NewL( TInt aStatusCode );

		/**
		* Creates a new instance of CMSRPStatusHeader and puts it to CleanupStack
		* @param aStatusCode status code
		* @return a new instance of CMSRPStatusHeader
		*/
		IMPORT_C static CMSRPStatusHeader* NewLC( TInt aStatusCode );

		/**
		* Creates a new instance of CMSRMessageIdHeader
		* @return a new instance of CMSRPStatusHeader
		*/
		static CMSRPStatusHeader* NewL( );

		/**
		* Destructor, deletes the resources of CMSRPStatusHeader.
		*/
		IMPORT_C ~CMSRPStatusHeader();

	public: // New functions

		/**
		* Constructs an instance of a CMSRPStatusHeader from a RReadStream
		* @param aReadStream a stream containing the value of the
		*        externalized object (header name not included).
		* @return an instance of a CMSRPStatusHeader
		*/
		IMPORT_C static CMSRPStatusHeader*
			InternalizeValueL( RReadStream& aReadStream );

		/**
		* Sets the status code value
		* @param aStatusCode status code
		*/
		IMPORT_C void SetStatusCode( TInt aStartPos );

		/**
		* Gets the status code value
		*/
		IMPORT_C TInt StatusCode( ) const;

	public: // From CMSRPHeaderBase

		IMPORT_C void ExternalizeValueL( RWriteStream& aWriteStream ) const;

	public: // From CMSRPHeaderBase

		RStringF Name() const;
		HBufC8* ToTextValueL() const;

	private: // Constructors

        /**
        * Contructor.
        */
		CMSRPStatusHeader( );
        /**
        * Contructor.
		* @param aStatusCode status code
        */
		CMSRPStatusHeader( TInt aStatusCode );
        /**
        * two-phased contructor.
        */
        void ConstructL( ) const;

	private: // new functions
	    /**
		* Internalize external object data
		* @param aReadStream a stream containing the value of the
		*        externalized object (header name not included).
		*/
		void DoInternalizeValueL( RReadStream& aReadStream );

  	private: // variables

		// status code value
		TInt iStatusCode;

	__DECLARE_TEST;
	};

#endif // CMSRPSTATUSHEADER_H

// End of File
