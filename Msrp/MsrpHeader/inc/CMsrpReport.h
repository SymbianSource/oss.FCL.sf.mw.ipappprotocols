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

#ifndef CMSRPREPORT_H
#define CMSRPREPORT_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>

#include "CMSRPMessageBase.h"

// CLASS DECLARATION

/**
*  @publishedAll
*
*  Class provides functions for creation and manipulation of MSRP headers
*/
class CMSRPReport : public CMSRPMessageBase
    {
    public:  // Constructors and destructor

        /**
        * Constructor.
        */
	    IMPORT_C CMSRPReport();

        /**
        * Destructor.
        */
		IMPORT_C ~CMSRPReport();

    public: // New functions

		/**
		* Check if the given externalised buffer contains MSRP Report
 		* @param aBuffer a Buffer containing the value of the
		*        externalized object
		* @return true if Buffer contains MSRP report
		*/
		IMPORT_C static TBool IsReport( const TDesC8& aBuffer );

		/**
		* Internalizes the class from given stream
 		* @param aReadStream a stream containing the value of the
		*        externalized object
		* @return Internalized CMSRPReport object, ownership is transferred
		*/
		IMPORT_C static CMSRPReport* InternalizeL( RReadStream& aReadStream );

	public: // from base classes

		// from CMSRPMessageBase
		IMPORT_C void ExternalizeL( RWriteStream& aWriteStream );

    private:

        /**
        * Internalizes this message
 		* @param aReadStream a stream containing the value of the
		*        externalized object (header name not included).
        */
	    void DoInternalizeL( RReadStream& aReadStream  );
	};

#endif // CMSRPREPORT_H

// End of File
