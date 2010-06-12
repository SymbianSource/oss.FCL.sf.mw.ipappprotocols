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

#ifndef CMSRPTLSPTRS_H
#define CMSRPTLSPTRS_H

// EXTERNAL INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CMSRPStrings;

/*
* CLASS DEFINITION
*/
class CMSRPTlsPtrs : public CBase
    {
	public:
	    
        /**
        * Constructor.
		* @param aStrings new CMSRPStrings object
        */
	    CMSRPTlsPtrs( CMSRPStrings* aStrings );
        /**
        * Destructor.
        */
		~CMSRPTlsPtrs();

        /**
        * Constructor.
        */
		void DestroyStrings();

	public:
	
        /**
        * Gives MsrpStrings object
        */
		CMSRPStrings* GetMsrpString();
		
	private:

        /**
        * Constructor.
        */
		CMSRPTlsPtrs();    

	private: // data
	
		// CMSRPStrings object
	    CMSRPStrings* iStrings;

    };

#endif // CMSRPTLSPTRS_H

// End of File
