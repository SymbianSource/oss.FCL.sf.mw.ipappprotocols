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




#ifndef __UT_CSRTPCIPHERAESCM128_H__
#define __UT_CSRTPCIPHERAESCM128_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CSRTPCipherAESCM128;

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
class UT_CSRTPCipherAESCM128
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CSRTPCipherAESCM128* NewL();
        static UT_CSRTPCipherAESCM128* NewLC();
        /**
         * Destructor
         */
        ~UT_CSRTPCipherAESCM128();

    private:    // Constructors and destructors

        UT_CSRTPCipherAESCM128();
        void ConstructL();

	public: 	// From observer interface				

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_EncryptL_EncryptDataL();

         void UT_EncryptL_EncryptData2L();

         void UT_EncryptL_EncryptData3L();

         void UT_EncryptL_EncryptData4L();
         
         void UT_EncryptL_EncryptDataKeyChangesL();

         void UT_EncryptL_DecryptDataL();
         
         void UT_EncryptL_DecryptData2L();

         void UT_EncryptL_DecryptData3L();
         
         void UT_EncryptL_DecryptData4L();
         
         void UT_EncryptL_DecryptDataKeyChangesL();

         void UT_EncryptL_ErrorTest1L();
                  
         void UT_EncryptL_ErrorTest2L();
         
         void UT_IncreaseIVL(  );

        void Hex(HBufC8& aString);                 

    private:    // Data

		 CSRTPCipherAESCM128* iEncryptor;

	     HBufC8* iRFC3686_TestKey128bits;
	     HBufC8* iRFC3686_TestKey_2_128bits;
	     HBufC8* iRFC3686_TestIV128bits;
	     HBufC8* iRFC3686_TestIV_2_128bits;
	     HBufC8* iRFC3686_TestPlainT104bits;
	     HBufC8* iRFC3686_TestCipherT104bits;
	     HBufC8* iRFC3686_TestPlainT128bits;
	     HBufC8* iRFC3686_TestCipherT128bits;
	     HBufC8* iRFC3686_TestPlainT256bits;
	     HBufC8* iRFC3686_TestCipherT256bits;
	     HBufC8* iRFC3686_TestPlainT232bits;
	     HBufC8* iRFC3686_TestCipherT232bits;
	     
		EUNIT_DECLARE_TEST_TABLE; 
		
    };

#endif      //  __UT_CSRTPCIPHERAESCM128_H__

// End of file
