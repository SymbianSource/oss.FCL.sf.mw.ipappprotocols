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




#ifndef __UT_CRTPAPI_H__
#define __UT_CRTPAPI_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES
//#include "SRTPCryptoHandler.h"
#include <srtpmastersalt.h>
#include <srtpmasterkey.h>
#include <srtpcryptocontext.h>
#include "msrtprekeyingobserver.h"


#include <rtpapi.h>
#include <srtpstreamin.h>
#include "rtpmanager.h"





#include "srtpsession.h"

//  FORWARD DECLARATIONS
class CRtpAPI;


//class MSRTPCryptoHandlerContext;

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
NONSHARABLE_CLASS( UT_CRtpAPI )
     : public CEUnitTestSuiteClass,
     public MRtpErrNotify,
     public MRtpObserver,
     public MNonRTPDataObserver,
     public MRtcpObserver,
     public MSRTPReKeyingObserver,
     public MRtpPostProcessingObserver
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CRtpAPI* NewL();
        static UT_CRtpAPI* NewLC();
        /**
         * Destructor
         */
        ~UT_CRtpAPI();

    private:    // Constructors and destructors

        UT_CRtpAPI();
        void ConstructL();

    public:     // From observer interface

        void ErrorNotify( TInt aErrCode ) ;
        //from MRtpObs
        void RtpPacketReceived( TRtpId aStreamId, 
                                        const TRtpRecvHeader& aHeaderInfo, 
                                        const TDesC8& aPayloadData ) ;
        //from NonRTPDataObs
        void NonRTPDataReceived( TUint aPort, TBool aRTPport, const TDesC8& aNonRTPData );     
        //from MRtcpObs
        void SdesReceived( TRtpSSRC aSSRC, const TRtpSdesParams& aParams ) ;
		void ByeReceived( TRtpId aStreamId, TRtpSSRC aSSRC, const TDesC8& aReason ) ;
        void AppReceived( TRtpId aStreamId, TRtpSSRC aSSRC, const TRtcpApp& aApp ) ;
        void SrReceived( TRtpId aStreamId, TRtpSSRC aSSRC, const TTimeStamps& aTimeStamps );
        void RrReceived( TRtpId aStreamId, TRtpSSRC aSSRC ) ;                           

		void SRTPMasterKeyStaleEvent(const CSRTPSession&  aSession);
		void SRTPMasterKeyStaleEvent(const CSRTPStream&  aStream);
		
        void ReadyToSendRtpPacket( TRtpId aTranStreamId, TPtr8 &aPacket );
		
    private:    // New methods

         void SetupL();
         void Hex(HBufC8& aString);
        
         void Teardown();
        
         void UT_CRtpAPI_NewLL();
        
        
         void UT_CRtpAPI_OpenLL();
        
        
         void UT_CRtpAPI_StartConnectionL();
        
        
         void UT_CRtpAPI_StartConnection_OneL();
        
        
         void UT_CRtpAPI_CancelStartL();
        
        
         void UT_CRtpAPI_CloseL();
        
        
         void UT_CRtpAPI_SetLocalSdesL();
        
        
         void UT_CRtpAPI_GetLocalIPAddressLL();
        
        
         void UT_CRtpAPI_CreateSessionLL();
        
         void UT_CRtpAPI_CreateSessionL2L(  );	
        
         void UT_CRtpAPI_StartSessionL();
        
         void UT_CRtpAPI_TestStartSession2L(  );
        
         void UT_CRtpAPI_CloseSessionL();
        
        
         void UT_CRtpAPI_SetRemoteAddressL();
        
        
         void UT_CRtpAPI_SetRemoteRtcpAddressL();
        
        
         void UT_CRtpAPI_CreateReceiveStreamLL();
        
        
         void UT_CRtpAPI_CreateTransmitStreamLL();
        
        
         void UT_CRtpAPI_CreateTransmitStreamExtLL();
        
        
         void UT_CRtpAPI_CloseStreamL();
        
        
         void UT_CRtpAPI_RegisterRtpObserverL();
        
        
         void UT_CRtpAPI_UnregisterRtpObserverL();
        
        
         void UT_CRtpAPI_SetNonRTPDataObserverL();
        
        
         void UT_CRtpAPI_SendRtpPacketL();
        
        
         void UT_CRtpAPI_SendRtpPacket_1L();
        
        
         void UT_CRtpAPI_SendRtpPacket_2L();
        
         void UT_CRtpAPI_SendRtpPacket_ThreeL();	
        
         void UT_CRtpAPI_SendDataLL();
        
        
         void UT_CRtpAPI_CancelSendL();
        
        
         void UT_CRtpAPI_RegisterRtcpObserverL();
        
        
         void UT_CRtpAPI_UnregisterRtcpObserverL();
        
        
         void UT_CRtpAPI_SendRtcpByePacketL();
        
        
         void UT_CRtpAPI_SendRtcpAppPacketL();
        
        
         void UT_CRtpAPI_SendRtcpRrPacketL();
        
        
         void UT_CRtpAPI_SendRtcpSrPacketL();
        
        
         void UT_CRtpAPI_SuspendRtcpSendingL();
        
        
         void UT_CRtpAPI_IsRtcpSendingSuspendedL();
        
        
         void UT_CRtpAPI_GetSessionIdL();
        
        
         void UT_CRtpAPI_GetRtpSocketL();
        
        
         void UT_CRtpAPI_GetRtcpSocketL();
        
        
         void UT_CRtpAPI_GetStreamStatisticsL();
        
        
         void UT_CRtpAPI_GetSamplingRateL();
        
        
         void UT_CRtpAPI_SetSamplingRateL();
        
        
         void UT_CRtpAPI_SetRtcpParametersL();
        
        
         void UT_CRtpAPI_CustomCommandSyncL();
        
        
         void UT_CRtpAPI_CustomCommandAsyncL();
        
        
         void UT_CRtpAPI_VersionL();
         
         void UT_CRtpAPI_RegisterRtpPostProcessingObserverL();
         
         void UT_CRtpAPI_SendRtpPacketWithCSRCL();
         
         void UT_CRtpAPI_UnregisterRtpPostProcessingObserverL();

    private:    // Data

        EUNIT_DECLARE_TEST_TABLE; 

	private:    // Data
        CRtpAPI* iRtpAPI;
        CRtpAPI* iStpAPI;
        CActiveScheduler* iScheduler;
        TInt iErr;
        //SRTPSession Data
        TInetAddr iDestination;
		TBuf8<32>       iMKI;
		TBuf8<32> iRFC3711_TestMasterKey128bits;	     
	    TBuf8<32> iRFC3711_TestMasterSalt112bits;
	    TBuf8<32>       iSalt;
        TBuf8<32>       iKey;
        TBool iIsCallBackReceived;
    };

#endif      //  __UT_CRTPAPI_H__

// End of file
