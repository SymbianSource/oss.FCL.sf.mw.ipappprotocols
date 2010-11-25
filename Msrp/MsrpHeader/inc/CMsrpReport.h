/*
* ==============================================================================
*  Name        : CMSRPReport.h
*  Part of     : MSRP
*  Description : MSRP report header
*  Version     : %version: 1 % << Don't touch! Updated by Synergy at check-out.
*
*  Copyright © 2009 Nokia. All rights reserved.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation. All rights are reserved. Copying,
*  including reproducing, storing, adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia Corporation. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia Corporation.
* ==============================================================================
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
