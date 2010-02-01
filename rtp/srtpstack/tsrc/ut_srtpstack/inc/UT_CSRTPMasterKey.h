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




#ifndef __UT_CSRTPMasterKey_H__
#define __UT_CSRTPMasterKey_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES
#include "srtpmasterkey.h"


//  FORWARD DECLARATIONS
class TSRTPMasterKey;

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
class UT_CSRTPMasterKey
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CSRTPMasterKey* NewL();
        static UT_CSRTPMasterKey* NewLC();
        /**
         * Destructor
         */
        ~UT_CSRTPMasterKey();

    private:    // Constructors and destructors

        UT_CSRTPMasterKey();
        void ConstructL();

	public: 	// From observer interface
		
		

    private:    // New methods

		void Hex(HBufC8& aString);
         void SetupL();
        
         void Teardown();
        
         void UT_CSRTPMasterKey_SRTPMasterKeyL();
        
        
         void UT_CSRTPMasterKey_MasterKeyL();
        
        
         void UT_CSRTPMasterKey_MKIL();
        
        
         void UT_CSRTPMasterKey_EncrKeyLengthL();
        
        
         void UT_CSRTPMasterKey_AuthKeyLengthL();
        
        
        void UT_CSRTPMasterKey_ValidL();
         
         void TestEqualL(  );
        void UT_CSRTPMasterKey_ConstructLL(  );

    private:    // Data

        //TBuf8<32>       iKey;
        //TBuf8<32>       iMKI;
        CSRTPMasterKey*  iMasterKey;         
        CSRTPMasterKey*  iMasterKey2;         
        CSRTPMasterKey*  iMasterKey3;         
        CSRTPMasterKey*  iMasterKey4;         
        CSRTPMasterKey*  iMasterKey5;         
        TBuf8<32>       iTestKey128bits;
        TBuf8<32>       iTestKey128bits_dehexed;        

		EUNIT_DECLARE_TEST_TABLE; 
		
    };

#endif      //  __UT_CSRTPMasterKey_H__

// End of file
