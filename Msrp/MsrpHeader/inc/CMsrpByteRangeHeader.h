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


#ifndef CMSRPBYTERANGEHEADER_H
#define CMSRPBYTERANGEHEADER_H

// INTERNAL INCLUDES
#include "CMsrpHeaderBase.h"

// CONSTANTS

const TInt KMaxLengthOfNumbers = 50;
_LIT8( KByteRangeSeparator, "-" );
_LIT8( KByteRangeTotalSeparator, "/" );

// CLASS DECLARATION
/**
* @publishedAll
*
* Class provides functions for setting and getting MSRP "Byte-Range" header field
*
* @lib msrpclient.lib
*/
class CMSRPByteRangeHeader : public CMSRPHeaderBase
	{
	public:	// Constructors and destructor

		/**
		* Creates a new instance of CMSRMessageIdHeader
		* @param aStart start position of the bytes in data
		* @param aEnd end position of bytes in the data
		* @param aTotal Total length of the data
		* @return a new instance of CMSRPByteRangeHeader
		*/
		IMPORT_C static CMSRPByteRangeHeader* NewL(
			TInt aStart, TInt aEnd, TInt aTotal );

		/**
		* Creates a new instance of CMSRPByteRangeHeader and puts it to CleanupStack
		* @param aStart start position of the bytes in data
		* @param aEnd end position of bytes in the data
		* @param aTotal Total length of the data
		* @return a new instance of CMSRPByteRangeHeader
		*/
		IMPORT_C static CMSRPByteRangeHeader* NewLC(
			TInt aStart, TInt aEnd, TInt aTotal );

		/**
		* Creates a new instance of CMSRMessageIdHeader
		* @return a new instance of CMSRPByteRangeHeader
		*/
		static CMSRPByteRangeHeader* NewL( );

		/**
		* Destructor, deletes the resources of CMSRPByteRangeHeader.
		*/
		IMPORT_C ~CMSRPByteRangeHeader();

	public: // New functions

		/**
		* Constructs an instance of a CMSRPByteRangeHeader from a RReadStream
		* @param aReadStream a stream containing the value of the
		*        externalized object (header name not included).
		* @return an instance of a CMSRPByteRangeHeader
		*/
		IMPORT_C static CMSRPByteRangeHeader*
			InternalizeValueL( RReadStream& aReadStream );

		/**
		* Sets the start position of the bytes in data
		* @param aStartPos start position of bytes
		*/
		IMPORT_C void SetStartPosition( TInt aStartPos );

		/**
		* Gets the start position of the bytes in data
		*/
		IMPORT_C TInt StartPosition( ) const;

		/**
		* Sets the end position of the bytes in data
		* @param aEndPos start position of bytes
		*/
		IMPORT_C void SetEndPosition( TInt aEndPos );

		/**
		* Gets the end position of the bytes in data
		*/
		IMPORT_C TInt EndPosition( ) const;

		/**
		* Sets the total length of bytes in data
		* @param aTotal total number of bytes
		*/
		IMPORT_C void SetTotalLength( TInt aTotal );

		/**
		* Gets the total number of the bytes in data
		*/
		IMPORT_C TInt TotalLength( ) const;

	public: // From CMSRPHeaderBase

		IMPORT_C void ExternalizeValueL( RWriteStream& aWriteStream ) const;

	protected: // From CMSRPHeaderBase

		RStringF Name() const;
		HBufC8* ToTextValueL() const;

	private: // Constructors

        /**
        * Contructor.
        */
		CMSRPByteRangeHeader( );
        /**
        * Contructor.
		* @param aStart start position of the bytes in data
		* @param aEnd end position of bytes in the data
		* @param aTotal Total length of the data
        */
		CMSRPByteRangeHeader(
			TInt aStart, TInt aEnd, TInt aTotal );
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

		// byte range values
		TInt iStartPosition;
		TInt iEndPosition;
		TInt iTotalBytes;

	};

#endif // CMSRPBYTERANGEHEADER_H

// End of File
