/*
* =============================================================================
*  Name        : CMsrpStatusHeader.h
*  Part of     : MSRP Client
*  Interface   : SDK API, MSRP API
*  Description :
*  Version     : %version: 1 % << Don't touch! Updated by Synergy at check-out.
*
*  Copyright © 2007 Nokia. All rights reserved.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation. All rights are reserved. Copying,
*  including reproducing, storing, adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia Corporation. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia Corporation.
* =============================================================================
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
