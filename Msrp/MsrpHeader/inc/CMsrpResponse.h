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

#ifndef CMSRPRESPONSE_H
#define CMSRPRESPONSE_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <stringpool.h>

#include "CMSRPMessageBase.h"

// CLASS DECLARATION


/**
*  @publishedAll
*
*  Class provides services for creating and manipulating MSRP response
*/
class CMSRPResponse : public CMSRPMessageBase
	{
    public:

		/** Error codes */
		enum TMSRPErrorCodes
			{
			EUnknownCode = 0,
			EAllOk = 200,
			EUnintelligibleRequest = 400,
			EActionNotAllowed = 403,
			ETimeout = 408,
			EStopSending = 413,
			EMimeNotUnderstood = 415,
			EParameterOutOfBounds = 423,
			ESessionDoesNotExist = 481,
			EUnknownRequestMethod = 501,
			ESessionAlreadyBound = 506
			};

    public:  // Constructors and destructor

        /**
        * Constructor.
	    * @param aStatusCode a known MSRP response status code.
	    * Cannot be 100.
        */
		IMPORT_C CMSRPResponse( TUint aStatusCode, RStringF aReasonPhrase );

        /**
        * Constructor
        */
		IMPORT_C CMSRPResponse( );

        /**
        * Destructor.
        */
		IMPORT_C virtual ~CMSRPResponse();

    public: // New functions

		/**
		* Returns the MSRP Response status code
		* @return MSRP Response status code
		*/
    	IMPORT_C TUint StatusCode() const;

		/**
		* Gets a MSRP Response Reason Phrase
		* @return a MSRP response reason phrase or an empty string if
        *   the reason phrase is not defined.
		*/
    	IMPORT_C RStringF ReasonPhrase() const;

		/**
		* Sets a MSRP Response extension status code. It is not possible to set
        * value 100.
		* @param aStatusCode extension status code
		* @leave KErrArgument if aStatusCode < 100 or aStatusCode >= 700
		*/
    	IMPORT_C void SetStatusCodeL( TUint aStatusCode );

		/**
		* Sets a MSRP Response Reason Phrase
		* @param aReasonPhrase a MSRP response reason phrase.
		*/
    	IMPORT_C void SetReasonPhrase( RStringF aReasonPhrase );

		/**
		* Internalizes object from a given stream
		* @param aReadStream stream to read from
		* @return CMSRPResponse if internalization successfull
		*/
    	IMPORT_C static CMSRPResponse* InternalizeL( RReadStream& aReadStream );

		/**
		* Check if the given externalised buffer contains MSRP Response
 		* @param aBuffer a Buffer containing the value of the
		*        externalized object
		* @return true if Buffer contains MSRP response
		*/
		IMPORT_C static TBool IsResponse( const TDesC8& aBuffer );

	public: // from base classes

		// from CMSRPMessageBase
		IMPORT_C void ExternalizeL( RWriteStream& aWriteStream );
		
    private:

        /**
        * Internalizes the class from a given stream
		* @param aReadStream stream to read from
        */
	    void DoInternalizeL( RReadStream& aReadStream );

    private: // Data

		// status code
	    TUint iStatusCode;

		// reason phrase text
	    RStringF iReasonPhrase;
	};

#endif // CMSRPRESPONSE_H

// End of File
