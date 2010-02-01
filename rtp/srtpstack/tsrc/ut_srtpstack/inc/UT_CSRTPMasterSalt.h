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




#ifndef __UT_CSRTPMasterSalt_H__
#define __UT_CSRTPMasterSalt_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES
#include "srtpmastersalt.h"


//  FORWARD DECLARATIONS
class CSRTPMasterSalt;

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
class UT_CSRTPMasterSalt
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CSRTPMasterSalt* NewL();
        static UT_CSRTPMasterSalt* NewLC();
        /**
         * Destructor
         */
        ~UT_CSRTPMasterSalt();

    private:    // Constructors and destructors

        UT_CSRTPMasterSalt();
        void ConstructL();

	public: 	// From observer interface
		
		

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CSRTPMasterSalt_TSRTPMasterSaltL();
               
         void UT_CSRTPMasterSalt_MasterSaltL();        
        
         void UT_CSRTPMasterSalt_SaltLengthL();

         void UT_CSRTPMasterSalt_ValidL();
         void TestEqualL(  );
		 void Hex(HBufC8& aString);
		 void UT_CSRTPMasterSalt_ConstructLL(  );
    private:    // Data

        TBuf8<32>       iSaltKey;
        TBuf8<32>       iSaltKey2;
        CSRTPMasterSalt* iMasterSalt; 
        CSRTPMasterSalt* iMasterSalt2; 
        CSRTPMasterSalt* iMasterSalt3; 
        CSRTPMasterSalt* iMasterSalt4; 
        TBuf8<32>       iRFCTestSalt112bits;
        TBuf8<32>       iRFCTestSalt112bits_dehexed;        
		EUNIT_DECLARE_TEST_TABLE; 
		
    };

#endif      //  __UT_CSRTPMasterSalt_H__

// End of file
