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


#ifndef CMSRPFAILUREREPORTHEADER_H
#define CMSRPFAILUREREPORTHEADER_H

// INTERNAL INCLUDES
#include "CMsrpHeaderBase.h"

// CLASS DECLARATION
/**
* @publishedAll
*
* Class provides functions for setting and getting MSRP "Failure-Report" header field
*
* @lib msrpclient.lib
*/
class CMSRPFailureReportHeader : public CMSRPHeaderBase
	{
	public:	// Constructors and destructor

		/**
		* Creates a new instance of CMSRFailureReportHeader
		* @param aType type of the content
		* @return a new instance of CMSRPFailureReportHeader
		*/
		IMPORT_C static CMSRPFailureReportHeader* NewL( const RStringF& aType );

		/**
		* Creates a new instance of CMSRPFailureReportHeader and puts it to CleanupStack
		* @param aType type of the content
		* @return a new instance of CMSRPFailureReportHeader
		*/
		IMPORT_C static CMSRPFailureReportHeader* NewLC( const RStringF& aType );

		/**
		* Creates a new instance of CMSRPFailureReportHeader and puts it to CleanupStack
		* @return a new instance of CMSRPFailureReportHeader
		*/
		static CMSRPFailureReportHeader* NewL( );

		/**
		* Destructor, deletes the resources of CMSRPFailureReportHeader.
		*/
		IMPORT_C ~CMSRPFailureReportHeader();


	public: // New functions

		/**
		* Constructs an instance of a CMSRPFailureReportHeader from a RReadStream
		* @param aReadStream a stream containing the value of the
		*        externalized object (header name not included).
		* @return an instance of a CMSRPFailureReportHeader
		*/
		IMPORT_C static CMSRPFailureReportHeader*
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
		CMSRPFailureReportHeader();

        /**
        * Second-phase Contstructor
		* @param aType type of the content
        */
        void ConstructL( RStringF aType );

        /**
        * Second-phase Contstructor
        */
        void ConstructL( );

	private: // new functions
		void DoInternalizeValueL( RReadStream& aReadStream );

  	private: // variables

		// value as a string
		RStringF iReportType;

	};

#endif // CMSRPFAILUREREPORTHEADER_H

// End of File
