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




#ifndef __UT_CSRTPCRYPTOHANDLERSRTCP_H__
#define __UT_CSRTPCRYPTOHANDLERSRTCP_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES
#include "srtpmastersalt.h"
#include "srtpmasterkey.h"
#include "srtpcryptocontext.h"
#include <in_sock.h>
#include "msrtprekeyingobserver.h"

//  FORWARD DECLARATIONS
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
class UT_CSRTPCryptoHandlerSRTCP
     : public CEUnitTestSuiteClass,
     public MSRTPReKeyingObserver
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CSRTPCryptoHandlerSRTCP* NewL();
        static UT_CSRTPCryptoHandlerSRTCP* NewLC();
        /**
         * Destructor
         */
        ~UT_CSRTPCryptoHandlerSRTCP();

    private:    // Constructors and destructors

        UT_CSRTPCryptoHandlerSRTCP();
        void ConstructL();

	public: 	// From observer interface
		void SRTPMasterKeyStaleEvent(const CSRTPStream& aStream);  
        void SRTPMasterKeyStaleEvent(const CSRTPSession& aSession); 
		

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CSRTPCryptoHandlerSRTCP_NewLL();
        
        
         void UT_CSRTPCryptoHandlerSRTCP_NewLCL();
        
        
         void UT_DeriveSessionKeysL_OneL();
         
         
         void UT_PerformAuthenticationLL();         
         void UT_AuthenticateLL( ) ;
         void UT_AuthenticateL2L( ) ;
         void UT_AddAuthTagLL();
         
         void Hex(HBufC8& aString);
          
         void UT_CountSenderPacketIndexL1L(  );   
         void UT_CountSenderPacketIndexL2L();
         void UT_ReplayProtectionL1L( );   
         void UT_ReplayProtectionL2L( );   
         void UT_ReplayProtectionL3L( );   
         void UT_ReplayProtectionL4L( );  
         void UT_CheckMasterKeyIdentifierLL( );          

    private:    // Data

		EUNIT_DECLARE_TEST_TABLE; 
		        CSRTPMasterSalt* iMasterSalt; 
        CSRTPMasterKey*  iMasterKey;         
        
        CSRTPCryptoContext* iContext;
        
        HBufC8* iTestPayload160Bits;
        HBufC8* iTestMKI128Bits;
        HBufC8* iTestAuthTag80Bits;
        
        CSRTPSession* iSRTPSession;
        CSRTPStreamIn* iStreamIn;
        CSRTPCryptoHandlerSRTCP *iCryptoHandlerSRTCP;
        TInetAddr iDestination;
        
	    HBufC8* iRFC3711_SessionEncrKey128bits;
	    HBufC8* iRFC3711_SessionSaltKey128bits;
	    HBufC8* iRFC3711_SessionAuthKey128bits;
        HBufC8* iDecryptedPayload; 
        TBool iReKeyCalled;                
    };

#endif      //  __UT_CSRTPCRYPTOHANDLERSRTCP_H__

// End of file
