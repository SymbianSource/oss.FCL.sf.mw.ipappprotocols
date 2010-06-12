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


#ifndef CMSRPCONTENTTYPEHEADER_H
#define CMSRPCONTENTTYPEHEADER_H

// EXTERNAL INCLUDES
#include <e32base.h>
#include <apmstd.h>

// INTERNAL INCLUDES
#include "CMsrpHeaderBase.h"

// CLASS DECLARATION
/**
* @publishedAll
*
* Class provides functions for setting and getting MSRP "Content-Type" header field
*
* @lib msrpclient.lib
*/
class CMSRPContentTypeHeader : public CMSRPHeaderBase
	{
	public:	// Constructors and destructor

		/**
		* Creates a new instance of CMSRContentTypeHeader
		* @param aMediaType Media type, including subtype in the form
		         type/subtype
		* @return a new instance of CMSRPContentTypeHeader
		*/
		IMPORT_C static CMSRPContentTypeHeader* NewL( const TDesC8& aMediaType );

		/**
		* Creates a new instance of CMSRPContentTypeHeader and puts it to CleanupStack
		* @param aMediaType Media type, including subtype in the form
		         type/subtype
		* @return a new instance of CMSRPContentTypeHeader
		*/
		IMPORT_C static CMSRPContentTypeHeader* NewLC( const TDesC8& aMediaType );

		/**
		* Creates a new instance of CMSRContentTypeHeader
		* @return a new instance of CMSRPContentTypeHeader
		*/
		static CMSRPContentTypeHeader* NewL( );

		/**
		* Destructor, deletes the resources of CMSRPContentTypeHeader.
		*/
		IMPORT_C ~CMSRPContentTypeHeader();


	public: // New functions

		/**
		* Constructs an instance of a CMSRPContentTypeHeader from a RReadStream
		* @param aReadStream a stream containing the value of the
		*        externalized object (header name not included).
		* @return an instance of a CMSRPContentTypeHeader
		*/
		IMPORT_C static CMSRPContentTypeHeader*
			InternalizeValueL( RReadStream& aReadStream );


	public: // From CMSRPHeaderBase

		IMPORT_C void ExternalizeValueL( RWriteStream& aWriteStream ) const;

	protected: // From CMSRPHeaderBase

		RStringF Name() const;
		HBufC8* ToTextValueL() const;

	private: // Constructors

        /**
        * Contstructor
        */
		CMSRPContentTypeHeader( );

        /**
        * Second-phase contstructor
		* @param aMediaType Media type, including subtype in the form
		         type/subtype
        */
        void ConstructL( const TDesC8& aMediaType );

        /**
        * Second-phase constructor
        */
        void ConstructL( );

	private: // new functions
		void DoInternalizeValueL( RReadStream& aReadStream );

	private: // variables

		// Data type
		TDataType iDataType;

	};

#endif // CMSRPCONTENTTYPEHEADER_H

// End of File
