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




#ifndef __UT_CRTPSESSION_H__
#define __UT_CRTPSESSION_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES
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
class CRtpSession;

#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif
const TInt KRtpNumPayloadTypes = 128;

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
NONSHARABLE_CLASS( UT_CRtpSession )
     : public CEUnitTestSuiteClass,
      public MRtpErrNotify,
      public MRtpAsignUniqueID,
      public MRtcpObserver, MRtpObserver,
      public MNonRTPDataObserver,MTimeOutNotify
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CRtpSession* NewL();
        static UT_CRtpSession* NewLC();
        /**
         * Destructor
         */
        ~UT_CRtpSession();

    private:    // Constructors and destructors

        UT_CRtpSession();
        void ConstructL();

    public:     // From observer interface
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
         
         void Setup2L();
        
         void Teardown();
        
         void UT_CRtpSession_NewLL();
        
        
         void UT_CRtpSession_SSRCL();
        
        
         void UT_CRtpSession_StartSessionL();
        
        
         void UT_CRtpSession_SetRemoteAddressL();
        
        
         void UT_CRtpSession_SetRemoteRtcpAddressL();
        
        
         void UT_CRtpSession_CreateReceiveStreamLL();
        
        
         void UT_CRtpSession_CreateTransmitStreamLL();
        
        
         void UT_CRtpSession_CreateTransmitStreamExtLL();
        
        
         void UT_CRtpSession_CloseStreamL();
        
        
         void UT_CRtpSession_ResetTxStreamStatsL();
        
        
         void UT_CRtpSession_ResetRxStreamStatsL();
        
        
         void UT_CRtpSession_RegisterRtpObserverL();
        
        
         void UT_CRtpSession_UnregisterRtpObserverL();
        
        
         void UT_CRtpSession_SetNonRTPDataObserverL();
        
        
         void UT_CRtpSession_SendRtpPacketL();
        
        
         void UT_CRtpSession_SendRtpPacket_OneL();
        
        
         void UT_CRtpSession_SendRtpPacket_TwoL();
        
        
         void UT_CRtpSession_SendRtpPacket_3L();
        
        
         void UT_CRtpSession_SendRtpPacket_4L();
        
        
         void UT_CRtpSession_SendDataL();
        
        
         void UT_CRtpSession_CancelSendL();
        
        
         void UT_CRtpSession_RegisterRtcpObserverL();
        
        
         void UT_CRtpSession_UnregisterRtcpObserverL();
        
        
         void UT_CRtpSession_SendRtcpByePacketLL();
        
        
         void UT_CRtpSession_SendRtcpAppPacketL();
        
        
         void UT_CRtpSession_SendRtcpSrPacket1L();
         void UT_CRtpSession_SendRtcpSrPacket2L();
         void UT_CRtpSession_RemoveSdesFromArrayL();
        
         void UT_CRtpSession_SendRtcpRrPacketL();
        
         void UT_CRtpSession_DoBuildSendRTCPReportIfNotSentL();
         void UT_CRtpSession_DoBuildSendRTCPReportL();
         void UT_CRtpSession_BuildSendRTCPReport1L();
         void UT_CRtpSession_BuildSendRTCPReport2L();
         void UT_CRtpSession_SendRTCPReportL(  );
         
         void UT_CRtpSession_CreateSDESL( ) ;
         void UT_CRtpSession_ProcessOneRTCPReportSectionL();
         
         void UT_CRtpSession_ProcessRTCPReportL();
         
         void UT_CRtpSession_ProcessSDESDataL();
         
         void UT_CRtpSession_ProcessSDESData2L();
         
         void UT_CRtpSession_ProcessRRSectionL();
         
         void UT_CRtpSession_ProcessSRSectionL();
         
         void UT_CRtpSession_ProcessBYESectionL();
         void UT_CRtpSession_GetRtpSocketL();
        
        
         void UT_CRtpSession_GetRtcpSocketL();
        
        
         void UT_CRtpSession_GetStreamStatisticsL();
        
        
         void UT_CRtpSession_SetRtcpParametersL();
        
        
         void UT_CRtpSession_GetSessionIdL();
        
        
         void UT_CRtpSession_StopRtcpSendingL();
        
        
         void UT_CRtpSession_ResumeRtcpSendingL();
        
        
         void UT_CRtpSession_IsRtcpSendingSuspendedL();
        
        
         void UT_CRtpSession_CheckRemoteSsrcL();
        
        
         void UT_CRtpSession_OnExpiredL();
        
         void UT_CRtpSession_ScheduleRtcpSendL();
        
         void UT_CRtpSession_OnRtpReceivedLL();
        
         void UT_CRtpSession_OnRtpReceivedL2L(  );	
        
         void UT_CRtpSession_OnRtcpReceivedLL();
        
        
         void UT_CRtpSession_OnReceptionErrorL();
        
        
         void UT_CRtpSession_OnPacketRejectedL();
         
         void UT_CRtpSession_BuildRTCPReportWithoutStreams();
         
         void UT_CRtpSession_BuildRTCPReportWithoutPackets();

      	void UT_CRtpSession_OnRtpReceivedL3L(  );
         void UT_CRtpSession_SendSRTCPReport();
      	void UT_CRtpSession_DeallocateMemoryForSdesL( );   
         void UT_CRtpSession_NextRTCPIntervalL();
         void UT_CRtpSession_MatchSSRCToStreamL();
      	void UT_CRtpSession_MatchSSRCToStream2L( );
      	void UT_CRtpSession_RcvStreamReceivedRtpPacketL();
      	
      	void UT_CRtpSession_GetRcvStreamByIndexL( );
      	void UT_CRtpSession_CreateNewRecvStreamLL();
      	void UT_CRtpSession_SendSrtcpReportLL();
      	//Test Utility function	  
		TBool PeerStatsEqual( TRtpPeerStat aStat1, TRtpPeerStat aStat2 );

		void Hex(HBufC8& aString);
    private:    // Data
		CRtpSession* iSession;
		CRtpSession* iSession2;
        RSocketServ iSocketServ;
        RConnection iConn;
        CRtpSDES* iSdes;
        TUint32 iProfileRTPTimeRates[KRtpNumPayloadTypes];
        RLibrary iLibrary;
        TRtpId iNumOfObjects;   // count of session and participants IDs assigned
        TRtpSSRC iTxStreamSSRC; // the local SSRC of the Tx stream we work with
        TRtpSSRC iRxStreamSSRC; // the remote SSRC of the Rx stream we work with
        TInt iPacketsExpected;
        TRtpId iTranstreamId;
		TRtpId iRecvstreamId;
		
		TBool iRollBackRtcp;
		TBool iRollBackRtcpValue;
		
        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __UT_CRTPSESSION_H__

// End of file
