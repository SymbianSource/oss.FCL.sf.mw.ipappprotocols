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




#ifndef __UT_CSRTPCRYPTOHANDLER_H__
#define __UT_CSRTPCRYPTOHANDLER_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include "srtpmastersalt.h"
#include "srtpmasterkey.h"
#include "srtpcryptocontext.h"
#include <in_sock.h>
#include "msrtprekeyingobserver.h"

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CSRTPCryptoHandlerSRTP;
class CSRTPSession;
class CSRTPStreamIn;
class CSRTPStreamOut;


//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
class UT_CSRTPCryptoHandler
     : public CEUnitTestSuiteClass,
        public MSRTPReKeyingObserver     
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CSRTPCryptoHandler* NewL();
        static UT_CSRTPCryptoHandler* NewLC();
        /**
         * Destructor
         */
        ~UT_CSRTPCryptoHandler();
        
        void SRTPMasterKeyStaleEvent(const CSRTPStream& aStream);   
		void SRTPMasterKeyStaleEvent(const CSRTPSession& aSession); 
    private:    // Constructors and destructors

        UT_CSRTPCryptoHandler();
        void ConstructL();

	public: 	// From observer interface
		
		

    private:    // New methods

         void SetupL();        
         void Teardown();        
         void UT_CSRTPCryptoHandler_MasterKeyUpdatedL(  );                
         void UT_CSRTPCryptoHandler_AuthenticateLL();         
         void UT_GetAuthenticationTagL();     
         void UT_Count_XL(  );             
         void Hex(HBufC8& aString);                          
		 
    private:    // Data

        //TBuf8<32>       iSalt;
        //TBuf8<32>       iKey;
        //TBuf8<32>       iMKI;
        CSRTPMasterSalt* iMasterSalt; 
        CSRTPMasterKey*  iMasterKey;         
        
        CSRTPCryptoContext* iContext;
        
        CSRTPSession* iSRTPSession;
        CSRTPStreamIn* iStreamIn;
        CSRTPStreamOut* iStreamOut;
        CSRTPCryptoHandlerSRTP *iCryptoHandlerSRTP;
        TInetAddr iDestination;
        
	     HBufC8* iTestAuthTag80Bits;
    
        TBuf8<32> iResult;        
        TBuf8<32> iRFCTestSalt112bits;
        TBuf8<32> iRFCTestSalt112bits_dehexed;
        TBuf8<32> iRFC_TestSalt_80bits;
        
        // SRTP test data KSRTP_RFC112bitEncrXValue
        TBuf8<32> iSRTP_RFC112bitEncrXValue;
        TBuf8<32> iSRTP_RFC112bitEncrXValue_dehexed;
        TBuf8<32> iSRTP_RFC112bitAuthXValue;
        TBuf8<32> iSRTP_RFC112bitAuthXValue_dehexed;
        TBuf8<32> iSRTP_RFC112bitSaltXValue;
        TBuf8<32> iSRTP_RFC112bitSaltXValue_dehexed;

        // SRTCP test data
        TBuf8<32> iSRTCP_RFC112bitEncrXValue;
        TBuf8<32> iSRTCP_RFC112bitEncrXValue_dehexed;
        TBuf8<32> iSRTCP_RFC112bitAuthXValue;
        TBuf8<32> iSRTCP_RFC112bitAuthXValue_dehexed;
        TBuf8<32> iSRTCP_RFC112bitSaltXValue;
        TBuf8<32> iSRTCP_RFC112bitSaltXValue_dehexed;
    
		EUNIT_DECLARE_TEST_TABLE; 
		
    };

#endif      //  __UT_CSRTPCRYPTOHANDLER_H__

// End of file
