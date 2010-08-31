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




#ifndef __UT_CSRTPKEYDERIVATION_AESCM128_H__
#define __UT_CSRTPKEYDERIVATION_AESCM128_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CSRTPKeyDerivation_AESCM128;

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
class UT_CSRTPKeyDerivation_AESCM128
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CSRTPKeyDerivation_AESCM128* NewL();
        static UT_CSRTPKeyDerivation_AESCM128* NewLC();
        /**
         * Destructor
         */
        ~UT_CSRTPKeyDerivation_AESCM128();

    private:    // Constructors and destructors

        UT_CSRTPKeyDerivation_AESCM128();
        void ConstructL();
        

	public: 	// From observer interface
		
		

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_RFC3711_AESCM_TestL();

         void UT_RFC3711_Session_Encr_Key_TestL();

         void UT_RFC3711_Session_Salt_Key_TestL();

         void UT_RFC3711_Session_Auth_Key_TestL();

        void UT_ErronousInput_OneL();
         
        void UT_ErronousInput_2L();
         
         void Hex(HBufC8& aString);                                  

    private:    // Data

		 CSRTPKeyDerivation_AESCM128* iEncryptor;

	     HBufC8* iRFC3711_Test1Key128bits;
	     HBufC8* iRFC3711_Test2Key128bits;
	     HBufC8* iRFC3711_TestIV128bits;
	     HBufC8* iRFC3711_TestKeystream128bits;
	     
	     HBufC8* iRFC3711_SessionEncrIV128bits;
	     HBufC8* iRFC3711_SessionSaltIV128bits;
	     HBufC8* iRFC3711_SessionAuthIV128bits;

	     HBufC8* iRFC3711_SessionEncrKey128bits;
	     HBufC8* iRFC3711_SessionSaltKey128bits;
	     HBufC8* iRFC3711_SessionAuthKey128bits;
		
		EUNIT_DECLARE_TEST_TABLE; 
		
    };

#endif      //  __UT_CSRTPKEYDERIVATION_AESCM128_H__

// End of file
