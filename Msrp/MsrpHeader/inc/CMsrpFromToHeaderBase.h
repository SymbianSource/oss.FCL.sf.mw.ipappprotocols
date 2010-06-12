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


#ifndef CMSRPFROMTOHEADERBASE_H
#define CMSRPFROMTOHEADERBASE_H

// EXTERNAL INCLUDES
#include <uri8.h>

// INTERNAL INCLUDES
#include "CMsrpHeaderBase.h"

// CLASS DECLARATION
/**
* @publishedAll
*
* Class provides functions for setting and getting parameters in MSRP "From-Path"
* and "To-Path" header.
*
* @lib msrpclient.lib
*/
class CMSRPFromToHeaderBase : public CMSRPHeaderBase
	{
	public: // Constructors and destructor

		/**
		* Destructor, deletes the resources of CMSRPFromToHeaderBase.
		*/
		IMPORT_C virtual ~CMSRPFromToHeaderBase();


	public: // New functions

		/**
		* Compares this instance to another "From-Path" or "To-Path" header object
		* @param aHeader a header to compare to
		* @return ETrue, if the objects are equal otherwise EFalse
		*/
		IMPORT_C TBool operator==( const CMSRPFromToHeaderBase& aHeader ) const;

	public: // From CMSRPHeaderBase

		IMPORT_C void ExternalizeValueL( RWriteStream& aWriteStream ) const;

	protected: // From CMSRPHeaderBase

		HBufC8* ToTextValueL() const;

	protected:

        /**
        * Constructor
        */
		CMSRPFromToHeaderBase();

	protected: // New functions

        /**
        * Parce URI string
        * @param aReadStream URI string in one piece
        */
		void DoInternalizeValueL( RReadStream& aReadStream );

	protected: // data

		// MSRP url (from or to header field)
		CUri8* iUri;
	};


#endif // CMSRPFROMTOHEADERBASE_H

// End of File
