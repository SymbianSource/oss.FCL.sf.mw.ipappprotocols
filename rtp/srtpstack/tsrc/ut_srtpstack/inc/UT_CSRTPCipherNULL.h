/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/




#ifndef __UT_CSRTPCIPHERNULL_H__
#define __UT_CSRTPCIPHERNULL_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CSRTPCipherNULL;

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
class UT_CSRTPCipherNULL
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CSRTPCipherNULL* NewL();
        static UT_CSRTPCipherNULL* NewLC();
        /**
         * Destructor
         */
        ~UT_CSRTPCipherNULL();

    private:    // Constructors and destructors

        UT_CSRTPCipherNULL();
        void ConstructL();

	public: 	// From observer interface				

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_TransformL();

        void Hex(HBufC8& aString);                 

    private:    // Data

		 CSRTPCipherNULL* iEncryptor;

	     HBufC8* iRFC3686_TestKey128bits;
	     HBufC8* iRFC3686_TestIV128bits;
	     HBufC8* iRFC3686_TestPlainT128bits;
	     
		EUNIT_DECLARE_TEST_TABLE; 
		
    };

#endif      //  __UT_CSRTPCIPHERNULL_H__

// End of file
