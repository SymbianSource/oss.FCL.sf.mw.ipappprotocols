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




#ifndef __UT_CSRTPSTREAMIN_H__
#define __UT_CSRTPSTREAMIN_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES
#include "srtpmastersalt.h"
#include "srtpmasterkey.h"
#include "srtpcryptocontext.h"
#include <in_sock.h>
#include "msrtprekeyingobserver.h"


//  FORWARD DECLARATIONS
class CSRTPCryptoHandlerSRTP;
class CSRTPCryptoHandlerSRTCP;
class CSRTPSession;
class CSRTPStreamIn;


//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
class UT_CSRTPStreamIn
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
        static UT_CSRTPStreamIn* NewL();
        static UT_CSRTPStreamIn* NewLC();
        /**
         * Destructor
         */
        ~UT_CSRTPStreamIn();
        
        
        

    private:    // Constructors and destructors

        UT_CSRTPStreamIn();
        void ConstructL();

	public: 	// From observer interface
		
		void SRTPMasterKeyStaleEvent(const CSRTPStream& aStream); 
		void SRTPMasterKeyStaleEvent(const CSRTPSession& aSession); 

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CSRTPStreamIn_NewL_OneL();
         void UT_CSRTPStreamIn_NewL_TwoL();
         void UT_CSRTPStreamIn_NewL_ThreeL();
         void UT_CSRTPStreamIn_NewL_FourL();
        
        
         void UT_CSRTPStreamIn_UnprotectSrtpLL();
        
         void UT_CSRTPStreamIn_UnprotectSrtpL2L(  );
         void UT_CSRTPStreamIn_UnprotectSrtpL3L(  );
         void UT_CSRTPStreamIn_UnprotectSrtcpLL();
         void UT_CSRTPStreamIn_UnprotectSrtcpL2L();
         void UT_CSRTPStreamIn_StateTestsL();
	 	 void UT_CSRTPStreamIn_StateTests2L();
         HBufC8* BuildPacketL(TBool aSrtp, TPayloadDetails& details);    
         void UT_CSRTPStreamIn_NewL_FiveL(  );
         void UT_CSRTPStreamIn_SetCryptoInL(  );

         void Hex(HBufC8& aString);             

    private:    // Data
    
        CSRTPMasterSalt* iMasterSalt; 
        CSRTPMasterKey*  iMasterKey;         
        
        CSRTPCryptoContext* iContext;
        
        HBufC8* iTestPayload160Bits;
        HBufC8* iTestMKI128Bits;
        HBufC8* iTestAuthTag80Bits;
        
        CSRTPSession* iSRTPSession;
        CSRTPStreamIn* iStreamIn;
        CSRTPStreamIn* iStreamInLateBD;
        CSRTPCryptoHandlerSRTP *iCryptoHandlerSRTP;
        CSRTPCryptoHandlerSRTCP *iCryptoHandlerSRTCP;
        CSRTPCryptoHandlerSRTP *iCryptoHandlerSRTP2;
        CSRTPCryptoHandlerSRTCP *iCryptoHandlerSRTCP2;
        TInetAddr iDestination;
        

	    HBufC8* iRFC3711_SessionEncrKey128bits;
	    HBufC8* iRFC3711_SessionSaltKey128bits;
	    HBufC8* iRFC3711_SessionAuthKey128bits;
	    HBufC8* iRFC3711_RtcpEncrKey128bits;
	    HBufC8* iRFC3711_RtcpSaltKey128bits;
	    HBufC8* iRFC3711_RtcpAuthKey128bits;
	    
        HBufC8* iDecryptedPayload; 
		TBool iMasterKeyStaleRefresh;
		EUNIT_DECLARE_TEST_TABLE; 
		
    };

#endif      //  __UT_CSRTPSTREAMIN_H__

// End of file
