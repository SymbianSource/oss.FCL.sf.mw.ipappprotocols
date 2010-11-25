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

#ifndef CMSRPMESSAGEBASE_H
#define CMSRPMESSAGEBASE_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>

// FORWARD DECLARATIONS
class CMSRPFromPathHeader;
class CMSRPToPathHeader;
class CMSRPMessageIdHeader;
class CMSRPByteRangeHeader;
class CMSRPContentTypeHeader;
class CMSRPFailureReportHeader;
class CMSRPSuccessReportHeader;
class CMSRPStatusHeader;

// CONSTANTS
const TInt KMaxLengthOfShortMessage = 4096;
const TInt KLengthOfMSRPSmallBuffer = 2048;

// CLASS DECLARATION

/**
*  @publishedAll
*
*  Class provides functions for creation and manipulation of MSRP headers
*
*  @lib msrpclient.lib
*/
class CMSRPMessageBase : public CBase
    {
			
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
		IMPORT_C virtual ~CMSRPMessageBase();

    public: // New functions

		/**
		* Sets/resets the recipient's To-Path header
		* @param aTo a To-Path header to be set, the ownership is transferred
		*/
		IMPORT_C void SetToPathHeader( CMSRPToPathHeader* aToPath );

		/**
		* Gets the recipient's To-Path header
		* @return NULL if not present. Ownership is not
        *         transferred.
		*/
		IMPORT_C const CMSRPToPathHeader* ToPathHeader() const;

		/**
		* Sets/resets the From-Path header
 		* @param aFromPath From-Path header to be set, the ownership is transferred
		*/
		IMPORT_C void SetFromPathHeader( CMSRPFromPathHeader* aFromPath );

		/**
		* Returns From-Path header
		* @return NULL if not present. Ownership is not
        *         transferred.
		*/
		IMPORT_C const CMSRPFromPathHeader* FromPathHeader() const;

		/**
		* Sets/resets the Content-Type header
 		* @param aContentType ContentType header to be set, the ownership is transferred
		*/
		IMPORT_C void SetContentTypeHeader( CMSRPContentTypeHeader* aContentType );

		/**
		* Returns Content-Type header
		* @return NULL if not present. Ownership is not
        *         transferred.
		*/
		IMPORT_C const CMSRPContentTypeHeader* ContentTypeHeader() const;

		/**
		* Sets/resets the Failure-Report header
 		* @param aFailureReport FailureReport header to be set, the ownership is transferred
		*/
		IMPORT_C void SetFailureReportHeader( CMSRPFailureReportHeader* aFailureReport );

		/**
		* Returns Failure-Report header
		* @return NULL if not present. Ownership is not
        *         transferred.
		*/
		IMPORT_C const CMSRPFailureReportHeader* FailureReportHeader() const;

		/**
		* Sets/resets the Success-Report header
 		* @param aSuccessReport SuccessReport header to be set, the ownership is transferred
		*/
		IMPORT_C void SetSuccessReportHeader( CMSRPSuccessReportHeader* aSuccessReport );

		/**
		* Returns Success-Report header
		* @return NULL if not present. Ownership is not
        *         transferred.
		*/
		IMPORT_C const CMSRPSuccessReportHeader* SuccessReportHeader() const;

		/**
		* Sets/resets the Message-ID header
 		* @param aMessageId MessageId header to be set, the ownership is transferred
		*/
		IMPORT_C void SetMessageIdHeader( CMSRPMessageIdHeader* aMessageId );

		/**
		* Returns Message-ID header
		* @return NULL if not present. Ownership is not
        *         transferred.
		*/
		IMPORT_C const CMSRPMessageIdHeader* MessageIdHeader() const;

		/**
		* Sets/resets the Byte-Range header
 		* @param aByteRange ByteRange header to be set, the ownership is transferred
		*/
		IMPORT_C void SetByteRangeHeader( CMSRPByteRangeHeader* aByteRange );

		/**
		* Returns Byte-Range header
		* @return NULL if not present. Ownership is not
        *         transferred.
		*/
		IMPORT_C const CMSRPByteRangeHeader* ByteRangeHeader() const;

        /**
        * Sets/resets the Status header
        * @param aStatus status header to be set, the ownership is transferred
        */
        IMPORT_C void SetStatusHeader( CMSRPStatusHeader* aStatus );

        /**
        * returns Status header
        * @return status header, NULL if not present. Ownership is not
        *         transferred.
        */
        IMPORT_C const CMSRPStatusHeader* StatusHeader( ) const;

		/**
		* Writes the object to a RWriteStream
		* @param aWriteStream a stream where the object is to be externalized
		*/
		virtual void ExternalizeL( RWriteStream& aWriteStream ) = 0;

    protected:

		/**
        * Constructor.
        */
	    CMSRPMessageBase();

    protected: // Data

		// Headers
        // For setting and getting MSRP "From-Path" header field
        CMSRPFromPathHeader* iFromPath;
        // For setting and getting MSRP "To-Path" header field
        CMSRPToPathHeader* iToPath;
        // For setting and getting MSRP "Message-ID" header field
        CMSRPMessageIdHeader* iMessageId;
        // For setting and getting MSRP "Byte-Range" header field
        CMSRPByteRangeHeader* iByteRange;
        // For setting and getting MSRP "Content-Type" header field
        CMSRPContentTypeHeader* iContentType;
        // For setting and getting MSRP "Failure-Report" header field
        CMSRPFailureReportHeader* iFailureReport;
        // For setting and getting MSRP "Success-Report" header field
        CMSRPSuccessReportHeader* iSuccessReport;
        // For setting and getting MSRP "Status" header field
        CMSRPStatusHeader* iStatusHeader;        
	};

#endif // CMSRPMESSAGEBASE_H

// End of File
