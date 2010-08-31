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




#ifndef __UT_SRTPAUTHENTICATION_NULL_H__
#define __UT_SRTPAUTHENTICATION_NULL_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include "srtpauthentication_null.h"

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class SRTPAuthentication_NULL;

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
class UT_SRTPAuthentication_NULL
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_SRTPAuthentication_NULL* NewL();
        static UT_SRTPAuthentication_NULL* NewLC();
        /**
         * Destructor
         */
        ~UT_SRTPAuthentication_NULL();

    private:    // Constructors and destructors

        UT_SRTPAuthentication_NULL();
        void ConstructL();

	public: 	// From observer interface
		
		

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_AuthenticateL_RFC2202_Test1_EightyL();
         
         void Hex(HBufC8& aString);                                  

    private:    // Data


        CSRTPAuthentication_NULL* iAuthenticator;    

        // RFC 2202 test 1 128 bit key
        HBufC8* iRFC2202_Test1_Key_160bits;
        // RFC 2202 test 1 128 bit data
        HBufC8* iRFC2202_Test1_Data_16bits;
        // RFC 2202 test 1 80 bit digest/tag
        HBufC8* iRFC2202_Test1_Tag_80bits;

		EUNIT_DECLARE_TEST_TABLE; 		
    };

#endif      //  __UT_SRTPAUTHENTICATION_NULL_H__

// End of file
