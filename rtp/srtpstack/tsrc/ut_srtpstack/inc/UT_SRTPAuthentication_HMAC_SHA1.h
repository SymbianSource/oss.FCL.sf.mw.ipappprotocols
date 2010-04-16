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




#ifndef __UT_SRTPAUTHENTICATION_HMAC_SHA1_H__
#define __UT_SRTPAUTHENTICATION_HMAC_SHA1_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include "srtpauthentication_hmac_sha1.h"

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class SRTPAuthentication_HMAC_SHA1;

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
class UT_SRTPAuthentication_HMAC_SHA1
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_SRTPAuthentication_HMAC_SHA1* NewL();
        static UT_SRTPAuthentication_HMAC_SHA1* NewLC();
        /**
         * Destructor
         */
        ~UT_SRTPAuthentication_HMAC_SHA1();

    private:    // Constructors and destructors

        UT_SRTPAuthentication_HMAC_SHA1();
        void ConstructL();

	public: 	// From observer interface
		
		

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_AuthenticateL_RFC2202_Test1_EightyL();
         void UT_AuthenticateL_RFC2202_Test1_ThirtyTwoL();
         void UT_AuthenticateL_RFC2202_Test2_EightyL();
         void UT_AuthenticateL_RFC2202_Test2_ThirtyTwoL();
         void UT_AuthenticateL_RFC2202_Test3_EightyL();
         void UT_AuthenticateL_RFC2202_Test3_ThirtyTwoL();
         void UT_AuthenticateL_RFC2202_Test4_EightyL();
         void UT_AuthenticateL_RFC2202_Test4_ThirtyTwoL();
         void UT_AuthenticateL_RFC2202_Test5_EightyL();
         void UT_AuthenticateL_RFC2202_Test5_ThirtyTwoL();
         void UT_AuthenticateL_RFC2202_Test6_EightyL();
         void UT_AuthenticateL_RFC2202_Test6_ThirtyTwoL();
         void UT_AuthenticateL_RFC2202_Test7_EightyL();
         void UT_AuthenticateL_RFC2202_Test7_ThirtyTwoL();
         void UT_AuthenticateL_Test8_ThirtyTwoL(  );
         void UT_AuthenticateL_Test_KeyChangedL();
         
         void Hex(HBufC8& aString);                                  

    private:    // Data


        CSRTPAuthentication_HMAC_SHA1* iAuthenticator;    

        // RFC 2202 test 1 128 bit key
        HBufC8* iRFC2202_Test1_Key_160bits;
        // RFC 2202 test 1 128 bit data
        HBufC8* iRFC2202_Test1_Data_16bits;
        // RFC 2202 test 1 80 bit digest/tag
        HBufC8* iRFC2202_Test1_Tag_80bits;
        // RFC 2202 test 1 32 bit digest/tag
        HBufC8* iRFC2202_Test1_Tag_32bits;
        
        // RFC 2202 test 2 key
        HBufC8* iRFC2202_Test2_Key;
        // RFC 2202 test 2 28 bit data
        HBufC8* iRFC2202_Test2_Data_28bits;
        // RFC 2202 test 2 80 bit digest/tag
        HBufC8* iRFC2202_Test2_Tag_80bits;
        // RFC 2202 test 2 32 bit digest/tag
        HBufC8* iRFC2202_Test2_Tag_32bits;
                
        // RFC 2202 test 3 160 bit key
        HBufC8* iRFC2202_Test3_Key_160bits;
        // RFC 2202 test 3 400 bit data
        HBufC8* iRFC2202_Test3_Data_400bits;
        // RFC 2202 test 3 80 bit digest/tag
        HBufC8* iRFC2202_Test3_Tag_80bits;        
        // RFC 2202 test 3 32 bit digest/tag
        HBufC8* iRFC2202_Test3_Tag_32bits;        

        // RFC 2202 test 4 200 bit key
        HBufC8* iRFC2202_Test4_Key_200bits;
        // RFC 2202 test 4 400 bit data
        HBufC8* iRFC2202_Test4_Data_400bits;
        // RFC 2202 test 4 80 bit digest/tag
        HBufC8* iRFC2202_Test4_Tag_80bits;
        // RFC 2202 test 4 32 bit digest/tag
        HBufC8* iRFC2202_Test4_Tag_32bits;

        // RFC 2202 test 5 200 bit key
        HBufC8* iRFC2202_Test5_Key_160bits;
        // RFC 2202 test 5 data
        HBufC8* iRFC2202_Test5_Data;
        // RFC 2202 test 5 80 bit digest/tag
        HBufC8* iRFC2202_Test5_Tag_80bits;
        // RFC 2202 test 5 32 bit digest/tag
        HBufC8* iRFC2202_Test5_Tag_32bits;
        
        // RFC 2202 test 6 640 bit key
        HBufC8* iRFC2202_Test6_Key_640bits;
        // RFC 2202 test 6 54 bit data
        HBufC8* iRFC2202_Test6_Data_54bits;
        // RFC 2202 test 6 80 bit digest/tag
        HBufC8* iRFC2202_Test6_Tag_80bits;
        // RFC 2202 test 6 32 bit digest/tag
        HBufC8* iRFC2202_Test6_Tag_32bits;

        // RFC 2202 test 7 640 bit key
        HBufC8* iRFC2202_Test7_Key_640bits;
        // RFC 2202 test 7 73 bit data
        HBufC8* iRFC2202_Test7_Data_73bits;
        // RFC 2202 test 7 80 bit digest/tag
        HBufC8* iRFC2202_Test7_Tag_80bits;
        // RFC 2202 test 7 32 bit digest/tag
        HBufC8* iRFC2202_Test7_Tag_32bits;

		// TestCase from SRTP open Src
        HBufC8* iTest8_Key_160bits;
        HBufC8* iTest8_Data_168bits;
        HBufC8*	iTest8_Data2_32bits;
        HBufC8* iTest8_Tag_80bits;
		EUNIT_DECLARE_TEST_TABLE; 		
    };

#endif      //  __UT_SRTPAUTHENTICATION_HMAC_SHA1_H__

// End of file
