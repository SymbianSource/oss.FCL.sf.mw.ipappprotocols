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




#ifndef __UT_CSRTPSESSION_H__
#define __UT_CSRTPSESSION_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES
#include <in_sock.h>

#include "srtpmastersalt.h"
#include "srtpmasterkey.h"
#include "msrtprekeyingobserver.h"
#include "srtpcryptocontext.h"
#include "srtpsession.h"
//#include "SRTPCryptoHandlerSRTP.h"
//#include "SRTPCryptoHandlerSRTCP.h"

#include "srtputils.h"
//  FORWARD DECLARATIONS
class CSRTPCryptoHandlerSRTP;
class CSRTPSession;


//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
class UT_CSRTPSession
     : public CEUnitTestSuiteClass,
     public MSRTPReKeyingObserver
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CSRTPSession* NewL();
        static UT_CSRTPSession* NewLC();
        /**
         * Destructor
         */
        ~UT_CSRTPSession();

    private:    // Constructors and destructors

        UT_CSRTPSession();
        void ConstructL();

	public: 	// From observer interface
		
		void SRTPMasterKeyStaleEvent(const CSRTPStream& aStream);
		void SRTPMasterKeyStaleEvent(const CSRTPSession& aSession);

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CSRTPSession_NewL_OneL();
        
        
         void UT_CSRTPSession_NewL_TwoL();
        
        
         void UT_CSRTPSession_RemoveStreamL();
        
        
         void UT_CSRTPSession_GetCryptoContextL();
         void Hex(HBufC8& aString);
         void UT_CSRTPSession_StreamLL();
         void UT_CSRTPSession_ProtectRTPLL();
         void UT_CSRTPSession_UnprotectRTPLL();	
         void UT_CSRTPSession_UnprotectRTPL2L();
      	 void UT_CSRTPSession_ProtectRTCPLL();
         void UT_CSRTPSession_UnprotectRTCPLL();
         void UT_CSRTPSession_UnprotectRTCPL2L();	         
         void UT_CSRTPSession_FindStreamL();
         void UT_CSRTPSession_ConstructLL(  );
         void FindLateBindingStreamAndUnprotectRTPLL();
         void FindLateBindingStreamAndUnprotectRTCPLL();
         void UT_CSRTPSession_UpdateCryptoContextLL();
         void UT_CSRTPSession_RemoteAddrL();
         void UT_CSRTPSession_RemoveAllStreamL();

    private:    // Data
		TInetAddr iDestination;
		
        CSRTPMasterSalt* iMasterSalt; 
        CSRTPMasterKey*  iMasterKey;         
        
        CSRTPCryptoContext* iContext;
        
        HBufC8* iTestPayload160Bits;
        HBufC8* iTestMKI128Bits;
        HBufC8* iTestAuthTag80Bits;
        
        CSRTPSession* iSRTPSession;
        
        CSRTPCryptoHandlerSRTP* iCryptoHandlerSRTP;
	    
		EUNIT_DECLARE_TEST_TABLE; 
		
    };

#endif      //  __UT_CSRTPSESSION_H__

// End of file
