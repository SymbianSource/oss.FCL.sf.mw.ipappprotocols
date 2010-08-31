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




#ifndef __UT_CRtpSessionSrtp_H__
#define __UT_CRtpSessionSrtp_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES

#include "srtpmastersalt.h"
#include "srtpmasterkey.h"
#include "srtpcryptocontext.h"
#include <in_sock.h>
#include "msrtprekeyingobserver.h"
#include "rtpsessionsrtp.h"
#include "srtpsession.h"
#include "srtpstreamin.h"
#include "srtpstreamout.h"

#include "rtpsdes.h"
#include "rtppacket.h"
#include "rtppacketparam.h"
#include "rtpstream.h"
#include "rtpasignuniqueid.h"
#include <rtpdef.h>
#include <commdbconnpref.h>
#include <e32std.h>
#include "rtptimer.h"


//  FORWARD DECLARATIONS
class CRtpSessionSrtp;

//Constant
const TInt KTestRtpNumPayloadTypes = 128;


#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
NONSHARABLE_CLASS( UT_CRtpSessionSrtp )
     : public CEUnitTestSuiteClass,
     public MSRTPReKeyingObserver,
      public MRtpErrNotify,
      public MRtpAsignUniqueID,
      public MRtcpObserver, MRtpObserver,
      public MNonRTPDataObserver,MTimeOutNotify     
    {
    class TPayloadDetails
        {
        public:    
            TPayloadDetails();
    
        public:
            TUint16 seqNr;
	        const TUint8* ssrc;
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
        static UT_CRtpSessionSrtp* NewL();
        static UT_CRtpSessionSrtp* NewLC();
        /**
         * Destructor
         */
        ~UT_CRtpSessionSrtp();

    private:    // Constructors and destructors

        UT_CRtpSessionSrtp();
        void ConstructL();

    public:     // From observer interface

        void SRTPMasterKeyStaleEvent(const CSRTPSession&  aSession); 
        void SRTPMasterKeyStaleEvent(const CSRTPStream&  aStream); 
        
        void ErrorNotify( TInt aErrCode );
		 //from MRtpAsignUniqueID     
        TRtpId AssignUniqueID();
        TInt AddStreamToSession(TRtpId aSessionId, TRtpId aStreamId);

    	// from MRtpObserver
        void RtpPacketReceived( TRtpId aStreamId,
                                const TRtpRecvHeader& aHeaderInfo, 
                                const TDesC8& aPayloadData );
    	// from MRtcpObserver

        void SdesReceived( TRtpSSRC aSSRC,
                           const TRtpSdesParams& aParams );
    
        void ByeReceived( TRtpId aStreamId, TRtpSSRC aSSRC,
                          const TDesC8& aReason );

        void AppReceived( TRtpId aStreamId, TRtpSSRC aSSRC,
                          const TRtcpApp& aApp );
    
        void SrReceived( TRtpId aStreamId, TRtpSSRC aSSRC,
                         const TTimeStamps& aTimeStamps );
    
        void RrReceived( TRtpId aStreamId, TRtpSSRC aSSRC );

		// from MNonRTPDataObserver
        void NonRTPDataReceived( TUint aPort, TBool aRTPport, const TDesC8& aNonRTPData );
        
          // from MTimeOutNotify
        void OnExpiredL( TInt aStatus );


    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CRtpSessionSrtp_NewLL();
        
        
         void UT_CRtpSessionSrtp_SendRtpPacketL();
        
         void UT_CRtpSessionSrtp_SendRtpPacket1L(  );
        
         void UT_CRtpSessionSrtp_SendRtpPacket_OneL();
         void UT_CRtpSessionSrtp_SendRtpPacket_OneAL(  );
        
         void UT_CRtpSessionSrtp_SendRtpPacket_2L();
         void UT_CRtpSessionSrtp_SendRtpPacket_TwoAL();
        
        
         void UT_CRtpSessionSrtp_OnRtpReceivedL();
         
         void UT_CRtpSessionSrtp_OnRtpReceived_OneL(  );
         void UT_CRtpSessionSrtp::UT_CRtpSessionSrtp_SendSRTCPReportL();
	
		void UT_CRtpSessionSrtp_OnRtcpReceivedL();
	
        

		 HBufC8* BuildPacketL(TBool aSrtp, TPayloadDetails& details);    
		 
		 void Write32( TUint8* const aPointer, TUint32 aValue );

         void Hex(HBufC8& aString);       
         void Write16( TUint8* const aPointer, TUint16 aValue );      
    private:    // Data
      	//Data for SRTP part
      	TBuf8<32>       iSalt;
        TBuf8<32>       iKey;
        TBuf8<32>       iMKI;
        CSRTPMasterSalt* iMasterSalt; 
        CSRTPMasterKey*  iMasterKey;         
        
        CSRTPCryptoContext* iContext;
        
        HBufC8* iTestPayload160Bits;
        HBufC8* iTestMKI128Bits;
        HBufC8* iTestAuthTag80Bits;
        
        CSRTPSession* iSRTPSession;
        CSRTPStreamIn* iStreamIn;
        CSRTPStreamOut* iStreamOut;
        CSRTPCryptoHandlerSRTP *iCryptoHandlerSRTP;
        TInetAddr iDestination;
	    TBuf8<32> iRFC3711_TestMasterKey128bits;	     
	    TBuf8<32> iRFC3711_TestMasterSalt112bits;
	    HBufC8* iRFC3711_SessionEncrKey128bits;
	    HBufC8* iRFC3711_SessionSaltKey128bits;
	    HBufC8* iRFC3711_SessionAuthKey128bits;
        HBufC8* iDecryptedPayload; 
		TBool iMasterKeyStaleRefresh;
		//Data form RTPSession
		TRtpId iNumOfObjects;   // count of session and participants IDs 
		CRtpSessionSrtp* iSession;
        RSocketServ iSocketServ;
        RConnection iConn;
        CRtpSDES* iSdes;
        TUint32 iProfileRTPTimeRates[KTestRtpNumPayloadTypes];
        RLibrary iLibrary;
		TRtpId iTranstreamId;
		TRtpId iRecvstreamId;
        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __UT_CRtpSessionSrtp_H__

// End of file
