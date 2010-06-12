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


#ifndef CMSRPSUCCESSREPORTHEADER_H
#define CMSRPSUCCESSREPORTHEADER_H

//  INCLUDES
#include "CMsrpHeaderBase.h"

// CLASS DECLARATION
/**
* @publishedAll
*
* Class provides functions for setting and getting MSRP "Success-Report" header field
*
* @lib msrpcodec.lib
*/
class CMSRPSuccessReportHeader : public CMSRPHeaderBase
	{
	public:	// Constructors and destructor

		/**
		* Creates a new instance of CMSRSuccessReportHeader
		* @param aType type of the content
		* @return a new instance of CMSRPSuccessReportHeader
		*/
		IMPORT_C static CMSRPSuccessReportHeader* NewL( const RStringF& aType );

		/**
		* Creates a new instance of CMSRPSuccessReportHeader and puts it to CleanupStack
		* @param aType type of the content
		* @return a new instance of CMSRPSuccessReportHeader
		*/
		IMPORT_C static CMSRPSuccessReportHeader* NewLC( const RStringF& aType );

		/**
		* Creates a new instance of CMSRSuccessReportHeader
		* @return a new instance of CMSRPSuccessReportHeader
		*/
		static CMSRPSuccessReportHeader* NewL( );

		/**
		* Destructor, deletes the resources of CMSRPSuccessReportHeader.
		*/
		IMPORT_C ~CMSRPSuccessReportHeader();


	public: // New functions

		/**
		* Constructs an instance of a CMSRPSuccessReportHeader from a RReadStream
		* @param aReadStream a stream containing the value of the
		*        externalized object (header name not included).
		* @return an instance of a CMSRPSuccessReportHeader
		*/
		IMPORT_C static CMSRPSuccessReportHeader*
			InternalizeValueL( RReadStream& aReadStream );


	public: // From CMSRPHeaderBase

		IMPORT_C void ExternalizeValueL( RWriteStream& aWriteStream ) const;

	public: // From CMSRPHeaderBase

		RStringF Name() const;
		HBufC8* ToTextValueL() const;

	private: // Constructors

		/**
		* Creates a new instance of CMSRSuccessReportHeader
		* @param aType type of the content
		*/
        void ConstructL( RStringF aType );

		/**
		* Creates a new instance of CMSRSuccessReportHeader
		*/
        void ConstructL( );

		/**
		* Constructor
		*/
		CMSRPSuccessReportHeader();

	private: // new functions
		void DoInternalizeValueL( RReadStream& aReadStream );

  	private: // variables

		// value as a string
		RStringF iReportType;

	};

#endif // CMSRPSUCCESSREPORTHEADER_H

// End of File
