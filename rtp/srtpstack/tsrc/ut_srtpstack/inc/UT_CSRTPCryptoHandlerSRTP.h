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




#ifndef __UT_CSRTPCRYPTOHANDLERSRTP_H__
#define __UT_CSRTPCRYPTOHANDLERSRTP_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include "srtpmastersalt.h"
#include "srtpmasterkey.h"
#include "srtpcryptocontext.h"
#include <in_sock.h>
#include "msrtprekeyingobserver.h"
#include "srtpcryptohandlersrtp.h"


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
class UT_CSRTPCryptoHandlerSRTP
     : public CEUnitTestSuiteClass,
     public MSRTPReKeyingObserver
    {
    
    class TPayloadDetails
        {
        public:    
            TPayloadDetails();
    
        public:
            TUint16 seqNr;
//          TUint8* ssrc;
            TUint8  padding;
            TUint8  numCSRC;
            TBool   fHeaderExtension;
            TUint8  payloadLen;
            const TUint8* payloadData;
            TUint8  MKILen;
            const TUint8* MKIData;
            TUint8  authTagLen;
            const TUint8* authTagData;    
        };
    
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CSRTPCryptoHandlerSRTP* NewL();
        static UT_CSRTPCryptoHandlerSRTP* NewLC();
        /**
         * Destructor
         */
        ~UT_CSRTPCryptoHandlerSRTP();
        
        void SRTPMasterKeyStaleEvent(const CSRTPStream& aStream);  
        void SRTPMasterKeyStaleEvent(const CSRTPSession& aSession); 

    private:    // Constructors and destructors

        UT_CSRTPCryptoHandlerSRTP();
        void ConstructL();

	public: 	// From observer interface
		
		

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CSRTPCryptoHandlerSRTP_NewLL();
                
         void UT_CSRTPCryptoHandlerSRTP_NewLCL();        
                
        
         HBufC8* BuildPacketL(TBool aSrtp, TPayloadDetails& details);
                  
                  
         void UT_DeriveSessionKeysL_OneL();
         

         void UT_UpdateROCL();         

         void Hex(HBufC8& aString);
          
         void CountSenderPacketIndexL(  );   
         
         void ReplayProtectionL1L( );   
         void ReplayProtectionL2L( );   
         void ReplayProtectionL3L( );   
         void ReplayProtectionL4L( );  
         void TestPacketHeaderAndPayloadL( );      
         void TestAuthenticationL1L(  );    
         void TestUpdate_s_l_and_RocLL();   
         void TestCountReceiverPacketIndexL();
         void TestCheckMasterKeyIdentifierL();
         void TestCountEncryptedPacketSizeL();
         void TestCountEncryptedPacketSize2L();
         void UT_InitializeEncryptedPacketLL();
                          
    private:    // Data
      
   
        CSRTPMasterSalt* iMasterSalt; 
        CSRTPMasterKey*  iMasterKey;         
        
        CSRTPCryptoContext* iContext;
        
        HBufC8* iTestPayload160Bits;
        HBufC8* iTestMKI128Bits;
        HBufC8* iTestAuthTag80Bits;
        
        CSRTPSession* iSRTPSession;
        CSRTPStreamIn* iStreamIn;
        CSRTPCryptoHandlerSRTP *iCryptoHandlerSRTP;
        TInetAddr iDestination;
        
	    HBufC8* iRFC3711_SessionEncrKey128bits;
	    HBufC8* iRFC3711_SessionSaltKey128bits;
	    HBufC8* iRFC3711_SessionAuthKey128bits;
        HBufC8* iDecryptedPayload; 
        TBool iReKeyCalled;                
		EUNIT_DECLARE_TEST_TABLE; 
		
    };

#endif      //  __UT_CSRTPCRYPTOHANDLERSRTP_H__

// End of file
