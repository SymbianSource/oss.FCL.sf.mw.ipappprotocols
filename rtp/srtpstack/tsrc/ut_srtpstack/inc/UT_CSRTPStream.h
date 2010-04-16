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




#ifndef __UT_CSRTPSTREAM_H__
#define __UT_CSRTPSTREAM_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES
#include "srtpmastersalt.h"
#include "srtpmasterkey.h"
#include "srtpcryptocontext.h"
#include <in_sock.h>
#include "msrtprekeyingobserver.h"
//  FORWARD DECLARATIONS
class CSRTPStream;
class CSRTPCryptoHandlerSRTP;
class CSRTPSession;
//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
class UT_CSRTPStream
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
        static UT_CSRTPStream* NewL();
        static UT_CSRTPStream* NewLC();
        /**
         * Destructor
         */
        ~UT_CSRTPStream();

    private:    // Constructors and destructors

        UT_CSRTPStream();
        void ConstructL();

	public: 	// From observer interface
		
		void SRTPMasterKeyStaleEvent(const CSRTPStream& aStream);
    
		void SRTPMasterKeyStaleEvent(const CSRTPSession& aSession);
    
    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CSRTPStream_SetROCL();
        
        
         void UT_CSRTPStream_ROCL();
        
        
         void UT_CSRTPStream_GetCryptoContextL();
         void UT_CSRTPStream_SetSSRCL(  );
         void Hex(HBufC8& aString);    
         void UT_CSRTPStream_ReKeyNeeded_OneL(  );
         void UT_CSRTPStream_ReKeyNeeded_TwoL(  );
         void UT_CSRTPStream_TestNewL(  );
         void UT_CSRTPStream_IsContextSetL();

    private:    // Data
		    
       
        
        CSRTPMasterSalt* iMasterSalt; 
        CSRTPMasterKey*  iMasterKey;         
        
        CSRTPCryptoContext* iContext;
        
        HBufC8* iTestPayload160Bits;
        HBufC8* iTestMKI128Bits;
        HBufC8* iTestAuthTag80Bits;
        
        CSRTPSession* iSRTPSession;
        CSRTPStream* iStreamIn;
        CSRTPCryptoHandlerSRTP *iCryptoHandlerSRTP;
        TInetAddr iDestination;
        
//	    HBufC8* iRFC3711_TestMasterKey128bits;	     
//	    HBufC8* iRFC3711_TestMasterSalt112bits;
	    
	    HBufC8* iRFC3711_SessionEncrKey128bits;
	    HBufC8* iRFC3711_SessionSaltKey128bits;
	    HBufC8* iRFC3711_SessionAuthKey128bits;
        HBufC8* iDecryptedPayload; 
		TBool iMasterKeyStaleRefresh;
		EUNIT_DECLARE_TEST_TABLE; 
		TBool iReKeyCalled;
		
    };

#endif      //  __UT_CSRTPSTREAM_H__

// End of file
