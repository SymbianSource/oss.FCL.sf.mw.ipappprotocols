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




#ifndef __UT_SRTPUTILS_H__
#define __UT_SRTPUTILS_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class SRTPUtils;

// CONSTANTS
const TUint64 KSRTP_56bit_encr_key_id  = 0x00000000000000;
const TUint64 KSRTP_56bit_auth_key_id  = 0x01000000000000;
const TUint64 KSRTP_56bit_salt_key_id  = 0x02000000000000;
const TUint64 KSRTCP_39bit_encr_key_id = 0x0300000000;
const TUint64 KSRTCP_39bit_auth_key_id = 0x0400000000;
const TUint64 KSRTCP_39bit_salt_key_id = 0x0500000000;

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
class UT_SRTPUtils
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_SRTPUtils* NewL();
        static UT_SRTPUtils* NewLC();
        /**
         * Destructor
         */
        ~UT_SRTPUtils();

    private:    // Constructors and destructors

        UT_SRTPUtils();
        void ConstructL();

	public: 	// From observer interface
		
		

    private:    // New methods

         void SetupL();
        
         void Teardown();
                
         void UT_SRTPUtils_Cnt_rL();        
        
         void UT_SRTPUtils_Cnt_key_idL();        
        
         void UT_SRTPUtils_Cnt_xL();        
        
         void UT_SRTPUtils_SrtpDIVL();        
        
         void UT_SRTPUtils_DeHexL();        
        
         void UT_SRTPUtils_HexValL();

         void UT_GetIVFirstTermL();
         void UT_GetIVSecondTermL();
         void UT_GetIVThirdTermL();
         void UT_CountIVL();        

    private:    // Data
        TBuf8<32> iResult;        
        TBuf8<32> iRFCTestSalt112bits;
        TBuf8<32> iRFCTestSalt112bits_dehexed;
        TBuf8<32> iRFC_TestSalt_80bits;        
        
        TBuf8<32> iRFC3711_PaddedSaltX128bits;
        TBuf8<32> iRFC3711_PaddedSaltX128bits_dehexed;
        TBuf8<32> iTest128BitIV;
        TBuf8<32> iTest128BitIV_dehexed;
                
        // SRTP test data
        TBuf8<32> iSRTP_RFC112bitSessionEncrKey;
        TBuf8<32> iSRTP_RFC112bitSessionEncrKey_dehexed;
        TBuf8<32> iSRTP_RFC112bitSessionAuthKey;
        TBuf8<32> iSRTP_RFC112bitSessionAuthKey_dehexed;
        TBuf8<32> iSRTP_RFC112bitSessionSaltKey;
        TBuf8<32> iSRTP_RFC112bitSessionSaltKey_dehexed;

        // SRTCP test data
        TBuf8<32> iSRTCP_RFC112bitSessionEncrKey;
        TBuf8<32> iSRTCP_RFC112bitSessionEncrKey_dehexed;
        TBuf8<32> iSRTCP_RFC112bitSessionAuthKey;
        TBuf8<32> iSRTCP_RFC112bitSessionAuthKey_dehexed;
        TBuf8<32> iSRTCP_RFC112bitSessionSaltKey;
        TBuf8<32> iSRTCP_RFC112bitSessionSaltKey_dehexed;

//        TBuf8<32> iDemo31BitSRTCPIndex:
		EUNIT_DECLARE_TEST_TABLE; 		
    };

#endif      //  __UT_SRTPUTILS_H__

// End of file
