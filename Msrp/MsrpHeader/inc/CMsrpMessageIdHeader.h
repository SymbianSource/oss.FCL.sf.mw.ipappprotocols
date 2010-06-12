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

#ifndef CMSRPMESSAGEIDHEADER_H
#define CMSRPMESSAGEIDHEADER_H

//  INCLUDES
#include "CMsrpHeaderBase.h"

// CLASS DECLARATION
/**
* @publishedAll
*
* Class provides functions for setting and getting MSRP "Message-ID" header field
*
* @lib msrpclient.lib
*/
class CMSRPMessageIdHeader : public CMSRPHeaderBase
	{
	public:	// Constructors and destructor

		/**
		* Creates a new instance of CMSRMessageIdHeader
		* @param aMessageId unique message identifier
		* @return a new instance of CMSRPMessageIdHeader
		*/
		IMPORT_C static CMSRPMessageIdHeader* NewL( const TDesC8& aMessageId );

		/**
		* Creates a new instance of CMSRPMessageIdHeader and puts it to CleanupStack
		* @param aMessageId unique message identifier
		* @return a new instance of CMSRPMessageIdHeader
		*/
		IMPORT_C static CMSRPMessageIdHeader* NewLC( const TDesC8& aMessageId );

		/**
		* Creates a new instance of CMSRMessageIdHeader
		* @return a new instance of CMSRPMessageIdHeader
		*/
		static CMSRPMessageIdHeader* NewL( );

		/**
		* Destructor, deletes the resources of CMSRPMessageIdHeader.
		*/
		IMPORT_C ~CMSRPMessageIdHeader();

	public: // New functions

		/**
		* Constructs an instance of a CMSRPMessageIdHeader from a RReadStream
		* @param aReadStream a stream containing the value of the
		*        externalized object (header name not included).
		* @return an instance of a CMSRPMessageIdHeader
		*/
		IMPORT_C static CMSRPMessageIdHeader*
			InternalizeValueL( RReadStream& aReadStream );

	public: // From CMSRPHeaderBase

		IMPORT_C void ExternalizeValueL( RWriteStream& aWriteStream ) const;

	protected: // From CMSRPHeaderBase

		RStringF Name() const;
		HBufC8* ToTextValueL() const;

	private: // Constructors

	    /**
	     * Constructor.
	     */
		CMSRPMessageIdHeader();

	    /**
	     * Two-phased constructor.
		 * @param aMessageId unique message identifier
	     */
        void ConstructL( const TDesC8& aMessageId );

	    /**
	     * Two-phased constructor.
	     */
        void ConstructL( );

	private: // new functions

	    /**
		* Internalize external object data
		* @param aReadStream a stream containing the value of the
		*        externalized object (header name not included).
		*/
		void DoInternalizeValueL( RReadStream& aReadStream );

  	private: // variables

		// value as a string
		HBufC8* iIdValue;

	};

#endif // CMSRPMESSAGEIDHEADER_H

// End of File
