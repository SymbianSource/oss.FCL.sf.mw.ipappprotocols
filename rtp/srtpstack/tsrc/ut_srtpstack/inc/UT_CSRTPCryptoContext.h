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




#ifndef __UT_CSRTPCryptoContext_H__
#define __UT_CSRTPCryptoContext_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES
#include "srtpmastersalt.h"
#include "srtpmasterkey.h"
#include "srtpcryptocontext.h"
#include <in_sock.h>
#include "msrtprekeyingobserver.h"


//  FORWARD DECLARATIONS
class TSRTPCryptoContext;
class CSRTPCryptoHandlerSRTP;
class CSRTPCryptoHandlerSRTCP;
class CSRTPSession;
class CSRTPStreamIn;
class CSRTPStreamOut;

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
class UT_CSRTPCryptoContext
     : public CEUnitTestSuiteClass,
     public MSRTPReKeyingObserver     
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CSRTPCryptoContext* NewL();
        static UT_CSRTPCryptoContext* NewLC();
        
        void SRTPMasterKeyStaleEvent(const CSRTPStream& aStream); 
        void SRTPMasterKeyStaleEvent(const CSRTPSession& aSession); 
        
        /**
         * Destructor
         */
        ~UT_CSRTPCryptoContext();

    private:    // Constructors and destructors

        UT_CSRTPCryptoContext();
        void ConstructL();
        void Hex(HBufC8& aString);
        

	public: 	// From observer interface
		
		

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CSRTPCryptoContext_Test1L();
         void UT_CSRTPCryptoContext_Test2L();
         void UT_CSRTPCryptoContext_Test3L();
         void UT_CSRTPCryptoContext_Test4L();
         void UT_CSRTPCryptoContext_Test5L();
         
         void UT_MasterKey_Test1L();         
         
         void UT_MasterSalt_Test1L();         

         void UT_AddCryptoChangeObserver_Test1L();
         
         void UT_SetMasterKey_Test1L();
         
         void UT_SetMasterSalt_Test1L();   
         
         void UT_ValidL();  
         
         void TestOperatorEqualL();   
         
         void TestPrefixLenL();   
         
         void TestReplayProtectionL(); 
         
         void TestIsEqualL();
         
         void TestSetRCCm3SyncL();
         
         void TestIsValidL();

    private:    // Data
        //TBuf8<32>       iSalt;
        //TBuf8<32>       iKey;
        TBuf8<32>       iMKI;
        CSRTPMasterSalt* iMasterSalt; 
        CSRTPMasterKey*  iMasterKey; 
        
        CSRTPMasterSalt* iMasterSalt1a; 
        CSRTPMasterKey*  iMasterKey1a;                        
        CSRTPCryptoContext* iContext;    

        //TBuf8<32>       iSalt2;
        //TBuf8<32>       iKey2;
        TBuf8<32>       iMKI2;
        CSRTPMasterSalt* iMasterSalt2; 
        CSRTPMasterKey*  iMasterKey2;                 
        CSRTPCryptoContext* iContext2;    

        //TBuf8<32>       iSalt3;
        //TBuf8<32>       iKey3;
        TBuf8<32>       iMKI3;
        CSRTPMasterSalt* iMasterSalt3; 
        CSRTPMasterKey*  iMasterKey3;                 
       
        CSRTPMasterSalt* iMasterSalt4; 
        CSRTPMasterKey*  iMasterKey4;    
        CSRTPMasterSalt* iMasterSalt5; 
        CSRTPMasterKey*  iMasterKey5;    
        CSRTPMasterSalt* iMasterSalt6; 
        CSRTPMasterKey*  iMasterKey6;    
        CSRTPCryptoContext* iContext3;    
        CSRTPCryptoContext* iContext4;    
        CSRTPCryptoContext* iContext5;    
        CSRTPCryptoContext* iContext6;    
        
        // 128 bit master key, test 1
        HBufC8* i128bitMasterKey1;
        // 128 bit master key, test 2
        HBufC8* i128bitMasterKey2;
        // 128 bit master key, test 3
        HBufC8* i128bitMasterKey3;
        // 112 bit master salt, test 1
        HBufC8* i112bitMasterSalt1;
        // 112 bit master salt, test 2
        HBufC8* i112bitMasterSalt2;       
        // 112 bit master salt, test 3
        HBufC8* i112bitMasterSalt3;       
        // 128 bit MKI, test 1
        HBufC8* i128bitMKI1;
        // 128 bit MKI, test 2
        HBufC8* i128bitMKI2;
        // 128 bit MKI, test 3
        HBufC8* i128bitMKI3;

        CSRTPSession* iSRTPSession;
        CSRTPStreamIn* iStreamIn;
        CSRTPStreamOut* iStreamOut;
        //CSRTPCryptoHandlerSRTP *iCryptoHandlerSRTP;
        CSRTPCryptoHandler *iCryptoHandlerSRTCP;
        TInetAddr iDestination;

		EUNIT_DECLARE_TEST_TABLE; 
		
    };

#endif      //  __UT_CSRTPCryptoContext_H__

// End of file
