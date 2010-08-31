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




#ifndef __UT_CSRTPSTREAMOUT_H__
#define __UT_CSRTPSTREAMOUT_H__

//  INTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  EXTERNAL INCLUDES
#include "srtpmastersalt.h"
#include "srtpmasterkey.h"
#include "srtpcryptocontext.h"
#include <in_sock.h>
#include "msrtprekeyingobserver.h"


//  FORWARD DECLARATIONS
class CSRTPCryptoHandlerSRTP;
class CSRTPCryptoHandlerSRTCP;
class CSRTPSession;
class CSRTPStreamOut;

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
class UT_CSRTPStreamOut
     : public CEUnitTestSuiteClass,
     public MSRTPReKeyingObserver     
    {

    class TPayloadDetails
        {
        public:    
            TPayloadDetails();
    
        public:
            TUint16 seqNr;
          TUint16 ssrc;
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
        static UT_CSRTPStreamOut* NewL();
        static UT_CSRTPStreamOut* NewLC();
        /**
         * Destructor
         */
        ~UT_CSRTPStreamOut();

    private:    // Constructors and destructors

        UT_CSRTPStreamOut();
        void ConstructL();

	public: 	// From observer interface
		
        void SRTPMasterKeyStaleEvent(const CSRTPStream& aStream); 
        void SRTPMasterKeyStaleEvent(const CSRTPSession& aSession); 	

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CSRTPStreamOut_NewL_OneL();
        
        
         void UT_CSRTPStreamOut_NewL_TwoL();
        
         void UT_CSRTPStreamOut_NewL_ThreeL(  );
         
         void UT_CSRTPStreamOut_SetCryptoOutLL(  );
         
         void UT_CSRTPStreamOut_ProtectRtpLL();
        
        
         void UT_CSRTPStreamOut_ProtectRtcpLL();
         
         void UT_CSRTPStreamOut_StateTestsL();
        
         HBufC8* BuildPacketL(TBool aSrtp, TPayloadDetails& details);    

         void Hex(HBufC8& aString);            
         
         void UT_CSRTPStreamOut_ProtectRtpL2L(); 

    private:    // Data

   
        CSRTPMasterSalt* iMasterSalt; 
        CSRTPMasterKey*  iMasterKey;         
        
        CSRTPCryptoContext* iContext;
        
        HBufC8* iTestPayload160Bits;
        HBufC8* iTestMKI128Bits;
        HBufC8* iTestAuthTag80Bits;
        
        CSRTPSession* iSRTPSession;
        CSRTPStreamOut* iStreamOut;
        CSRTPCryptoHandlerSRTP *iCryptoHandlerSRTP;
        CSRTPCryptoHandlerSRTCP *iCryptoHandlerSRTCP;
        TInetAddr iDestination;

	    HBufC8* iRFC3711_SessionEncrKey128bits;
	    HBufC8* iRFC3711_SessionSaltKey128bits;
	    HBufC8* iRFC3711_SessionAuthKey128bits;
        HBufC8* iDecryptedPayload; 
	    HBufC8* iRFC3711_RtcpEncrKey128bits;
	    HBufC8* iRFC3711_RtcpSaltKey128bits;
	    HBufC8* iRFC3711_RtcpAuthKey128bits;
		EUNIT_DECLARE_TEST_TABLE; 
		
    };

#endif      //  __UT_CSRTPSTREAMOUT_H__

// End of file
