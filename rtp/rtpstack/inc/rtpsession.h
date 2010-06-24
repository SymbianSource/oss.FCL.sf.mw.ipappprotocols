/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __RTPSESSION_H
#define __RTPSESSION_H

// INCLUDES
#include <e32base.h>
#include <rtpheader.h>
#include "rtputil.h"
#include "rtptimer.h"
#include "rtpsdes.h"
#include "rtpsessiondef.h"

//Packet
#include "rtppacket.h"
#include "rtppacketext.h"
#include "rtppacketparam.h"

//Stream
#include "rtpstreamdef.h"
#include "rtptranstream.h"
#include "rtprecvstream.h"
#include "rtpasignuniqueid.h"

//Comm
#include "rtpcomm.h"
#include "rtpcommdef.h"


/**
*  class for rtp session. 
*
*  @lib RtpService.dll
*/
class CRtpSession :
    public CBase,
    public MReceivedNotify,
    public MTimeOutNotify,
    public MSsrcCheckCallback
    {
    
    public:

        enum TStreamType
            {
            ETxStream       = 0,
            ERxStream
            };

        enum
            {
            ECallBack,
            ETimeOut
            };
       
    public: // new functions
        static CRtpSession* NewL( const TCreateSessionParams& aParams,
                                  TUint& aPort,
                                  TBool aEnableRtcp,
                                  const TRtcpParams* aRtcpParams,
                                  RSocketServ& aSocketServ,
                                  RConnection& aRConn,
                                  const TRtpId aSessionId,
                                  const CRtpSDES* aSdes,
                                  const TUint32* aProfileRTPTimeRates,
                                  const TBool aStandardRtp,
                                  const RLibrary& aLibrary,
                                  MRtpErrNotify& aErrNotify,
                                  MRtpAsignUniqueID& aAssignUniqueID );
                                  
        ~CRtpSession();

        TRtpSSRC SSRC() const;

        TInt StartSession();

        TInt SetRemoteAddress( TInetAddr& aRemoteAddr );
        TInt SetRemoteRtcpAddress( TInetAddr& aRemoteAddr );

        TInt CreateReceiveStreamL( TRtpId aStreamId, const TRtpPayloadType aPayloadType );

        TInt CreateTransmitStreamL( TRtpId aStreamId, const TRtpPayloadType aPayloadType, TUint32& aSSRC );

        TInt CreateTransmitStreamExtL( TRtpId aStreamId, const TRtpPayloadType aPayloadType, const TUint32 aSSRC );

        void CloseStream( TRtpId aStreamId );

        void ResetTxStreamStats();
        void ResetRxStreamStats();

        TInt RegisterRtpObserver( MRtpObserver& aObserver );
        void UnregisterRtpObserver();
        
        TInt RegisterRtpPostProcessingObserver( MRtpPostProcessingObserver& aObserver );
        void UnregisterRtpPostProcessingObserver();

        TInt SetNonRTPDataObserver( MNonRTPDataObserver* aNonRTPDataObserver );

        TInt SendRtpPacket( TRtpId aTranStreamId, const TRtpSendHeader& aHeaderInfo, 
                            const TDesC8& aPayloadData, const TArray<TRtpCSRC> *aCsrcList = NULL );

        TInt SendRtpPacket( TRtpId aTranStreamId,
                            const TRtpSendHeader& aHeaderInfo,
                            const TDesC8& aPayloadData,
                            TRequestStatus& aStatus,
                            const TArray<TRtpCSRC> *aCsrcList = NULL );

        TInt SendRtpPacket( const TRtpSendHeader& aHeaderInfo, const TDesC8& aPayloadData );

        TInt SendRtpPacket( const TRtpSendHeader& aHeaderInfo, const TDesC8& aPayloadData, 
                            TRequestStatus& aStatus );

        TInt SendRtpPacket( TRtpId aTranStreamId,
                            TRtpSequence aSequenceNum,
                            const TRtpSendHeader& aHeaderInfo,
                            const TDesC8& aPayloadData,
                            TRequestStatus& aStatus,
                            const TArray<TRtpCSRC> *aCsrcList = NULL );

        void SendData( TBool aUseRTPSocket,
                            const TDesC8& aData, 
                            TRequestStatus& aStatus );

        void CancelSend();

        TInt RegisterRtcpObserver( MRtcpObserver& aObserver );
        void UnregisterRtcpObserver();
        TInt SendRtcpByePacketL( TRtpId aTranStreamId, const TDesC8& aReason );
        TInt SendRtcpAppPacketL( TRtpId aTranStreamId, const TRtcpApp& aApp );
        TInt SendRtcpSrPacketL( TRtpId aTranStreamId );
        TInt SendRtcpRrPacketL( TRtpId aRecvStreamId );

        RSocket* GetRtpSocket();
        RSocket* GetRtcpSocket();

        TInt GetStreamStatistics( TRtpId aStreamId, TRtpPeerStat& aStat );
        TInt SetRtcpParameters( const TRtcpParams& aRtcpParams );

        TRtpId GetSessionId();

        TInt StopRtcpSending();
        TInt ResumeRtcpSending();

		/**
        * Gets the status of automatic RTCP sending.
        * @param TBool& aAutoSending - ETrue: RTCP atuo sending is on
        *                             EFalse: RTCP auto sending is off
 		* @return TInt - KErrNone if successful; KErrNotSupported if RTCP
        * disabled; system wide error code otherwise
        */
        TInt IsRtcpSendingSuspended( TBool& aAutoSending );
        
        void ReadyToSendRtpPacket(TRtpId aTranStreamId);

    public: // from MSsrcCheckCallback
        /**
         * Checks if a newly received SSRC collides with the SSRC's of the
         * existing Tx streams in this session, and reacts appropriately.
         * This is SSRC collision handling.
         * @param aSSRC a newly received SSRC
         */
        TInt CheckRemoteSsrcL( TRtpSSRC& aSSRC );
      	TBool CheckRemoteAddr();
        
    public: // from MTimeOutNotify
        void OnExpiredL( TInt aStatus );

    public: // from MReceivedNotify
        void OnRtpReceivedL();
        void OnRtcpReceivedL();
        void OnReceptionError( TPortType aPort, TInt aError );
        void OnPacketRejected(TPortType aPort);
        TBool IsSrtp();
    protected: // construction
        CRtpSession( const TRtpId aSessionId,
                     const TUint32* aProfileRTPTimeRates,
                     const TBool aStandardRtp,
                     MRtpErrNotify& aErrNotify,
                     const CRtpSDES* aSdesInfo,
                     MRtpAsignUniqueID& aAssignUniqueID );

        void ConstructL( const TCreateSessionParams& aParams,
                         TUint& aPort,
                         TBool aEnableRtcp,
                         const TRtcpParams* aRtcpParams,
                         RSocketServ& aSocketServ,
                         RConnection& aRConn,
                         const RLibrary& aLibrary,
                         MRtpErrNotify& aErrNotify );
        void FindRtpRxStreamSSRC(TRtpSSRC& aSSRC);                 
		TInt FindStream( const TRtpId aMagicKey, TUint& aStreamAddress );
		TInt IssueRtpRecv();
		virtual void SendSRTCPReportL(TRtpSSRC aSSRC);
		void AverageRtcpSize( TUint aRtcpReportSize );
       TInt IssueRtcpRecv();
       		
    private:
        TInt InitialiseRtcp( const TRtcpParams* aParams );

        
 
        void ScheduleRtcpSendL();
        void StartReceiveRtcpPacket();
        void StopReceiveRtcpPacket();

        TBool DoBuildSendRTCPReportIfNotSentL();
        void DoBuildSendRTCPReportL();
        TBool ShouldBuildEmptyRR( CRtpTranStream* aTempTranStream );

        /**
        * Determines the source count and length before an RTCP report is sent
        * @param aSourceCount reference to the source count variable to update
        * @param aLength reference to the packet length variable to update
        * @param aRxActive a list of active/not active streams
        * @return None
        */
        void DetermineSourceCountAndLength( TInt& aSourceCount,
                                            TInt& aLength,
                                            TBool aRxActive[] );

        /**
        * Determines parameters before an RTCP report with SR (RR) and SDES
        * sections is sent.
        * @param aTranStream The stream that will send the RTCP report
        * @param aStreamParam pointer to the stream parameters
        * @param aInitParam pointer to the initialisation parameters
        * @param aRxActive a list of active/not active streams
        * @return None
        */
        void SetParamsForFullRTCPReport( CRtpTranStream* aTranStream,
                       TRtpPacketStreamParam* aStreamParam,
                       TRtpPacketIOParam* aInitParam,
                       TBool aRxActive[] );
        void BuildSendRTCPReportL();
        TInt BuildRTCPReport( CRtpTranStream* tempStream, TInt aLastReport );
        void BuildSrSection( CRtpTranStream* aTempTranStream );
        void BuildRrSection( TInt aNumRcvStreams,
                             TBool aRxActive[KMaxNumActiveRcvStreams],
                             TInt aLastReport );
        void BuildEmptyRr( CRtpTranStream* aTempTranStream );
        void BuildSdesSection( CRtpTranStream* aTempTranStream );
        void SendRTCPReport();
        void CreateSDES( TInt aFCNameOnly, TRtpSSRC aSSRC );


        /**
        * Launches the processing of individual sections of RTCP reports.
        * @param extractParam pointer to the stream parameters
        * @param aSourceSSRC The synchronisation source value of the sender
        * @return Symbian OS error code
        */
        TRtpRtcpEnum ProcessOneRTCPReportSectionL(
                              const TRtpPacketIOParam& aExtractParam,
                              TRtpSSRC aSourceSSRC );
        /**
        * Processes an RTCP report.
        * @return An RTP-specific error code
        */
        TRtpRtcpEnum ProcessRTCPReportL(); 

        /**
        * Processes the SR section of an RTCP report.
        * @return An RTP-specific error code
        */
        TRtpRtcpEnum ProcessSRSectionL( TInt aSourceCount );

        /**
        * Processes the RR section of an RTCP report.
        * @return An RTP-specific error code
        */
        TRtpRtcpEnum ProcessRRSectionL( TInt aSourceCount );

        /**
        * Processes the SDES section of an RTCP report.
        * @return An RTP-specific error code
        */
        TRtpRtcpEnum ProcessSDESSectionL( TRtpSSRC aSourceSSRC );

        /**
        * Processes the BYE section of an RTCP report.
        * @return An RTP-specific error code
        */
        TRtpRtcpEnum ProcessBYESectionL();

        /**
        * Processes the APP section of an RTCP report.
        * @return An RTP-specific error code
        */
        TRtpRtcpEnum ProcessAPPSectionL( TInt aTotalPacketLen, TInt aSubType );

        /**
        * Processes the data contained in an SDES section.
        * @return An RTP-specific error code
        */
        TRtpRtcpEnum ProcessSDESDataL( TRtpPacketStreamParam& aStreamParam,
                                      TRtpPacketIOParam& aExtractParam,
                                      TRtpSSRC aSourceSSRC );

        /**
        * Gets the SDES values from the data contained in an SDES section.
        * Either aRecvStream or aSourceSdes is always NULL.
        * @param aTargetSdes the SDES in which the data will be stored
        * @param aRecvStream the stream that contains the SDES
        * @param aSourceSdes the SDES in which the data resides
        * @return KErrNone if successful, a system-wide error code otherwise
        */
        TInt GetSDESFromSDESData( TRtpSdesParams* aTargetSdes,
                                  const CRtpRecvStream* aRecvStream,
                                  CRtpSDES* aSourceSdes );

        /**
        * Logs the SDES data from the data contained in an SDES section.
        * @param aStreamParam stream parameters
        * @param aRecvStream the receiving stream
        * @param aSdes the new SDES
        * @param aSdesSession indicates whether the SDES was assigned to the
        * session
        */
#ifdef _DEBUG         
        void LogSDESData( const TRtpPacketStreamParam& aStreamParam,
                          const CRtpRecvStream* aRecvStream,
                          const CRtpSDES* aSdes,
                          const TBool aSdesSession );
#endif
        TRtpSSRC GetUniqueSSRC();
        TRtpSSRC GenerateSSRC();
        TInt AddStream( const TStream aArrayID, TStreamType aStreamType );
        
        TInt FindStream( const TRtpId aMagicKey, TUint& aStreamAddress, 
                         TStreamType& aStreamType );
        TInt RemoveStream( const TRtpId aMagicKey, TStreamType& aStreamType );
        void RemoveAllStreams();
        TInt MatchSSRCToStream( TUint& aRcvStreamAddress, TRtpSSRC aSSRC, 
                                TStreamType& aStreamType );
        TInt FindRtpRxStreamL();
        TInt AssignRtpRxStreamL();
        
        TInt StreamSendBYEPacket();
        TInt StreamSendAPPPacket();
        TBool FindStreamForSSRC( TRtpSSRC aSSRC );
        /**
         * Finds if a send stream is already using the given SSRC
         * @param aSSRC the synchronisation source value
         * @param a pointer to a stream pointer. If a Tx stream is found for
         * the specified SSRC, the stream pointer contains the address of the
         * Tx stream when this function returns.
         */
        TBool FindTxStreamForSSRC( TRtpSSRC aSSRC,
                                   CRtpTranStream** streamPointerPointer );

        TBool RcvStreamActive( TInt aIndex );
        TBool AnyRcvStreamReceivedRtpPacket();
        TBool RcvStreamReceivedRtpPacket( TInt aIndex );
        CRtpRecvStream* GetRcvStreamByIndex( TInt aIndex );
        
        void RedistributeBandwidth();
        
        CRtpSDES* DoAddSdesToArrayL( TRtpPacketIOParam* aExtractParam );
        TBool AddSdesToArrayL( TUint& aSdesAddress, TRtpPacketIOParam* extractParam );

        /**
         * Compares the specified SDES values with the ones in the parameters
         * and updates the SDES where the values differ
         * @param aTempSdes the target SDES
         * @param aExtractParam the parameters to compare with
         * @param aUpdateCName indicates whether to include the CName
         * @return ETrue if the SDES was updated
         */
        TBool CompareAndUpdateSdes( CRtpSDES* aTempSdes,
                                    TRtpPacketIOParam* aExtractParam,
                                    TBool aUpdateCName );
        TInt RemoveSdesFromArray( TRtpSSRC aSSRC );

        void NextRtcpInterval();
        
        void DeallocateMemoryForSdes( TRtpPacketIOParam* aParam );

        // Methods for getting random numbers
        TInt Random();
        TReal Random64();
        void SendRTCPByewithBanedSSRCL();
        TBool CheckifBanded();
        void CreateNewRecvStreamL();


    private:  
        TBool iStandardRtp;
        TRtpId iSessionId;   // session ID value
        
        
        TInetAddr iRemoteAddr[KMaxPorts]; // store here remote addresses
        TBool iRemoteAddrSet;

        // default values for this session when no transmit stream has been created.
        TRtpSSRC iDefaultSSRC;      // default SSRC value

        const CRtpSDES* iLocalSdes; // iManager->GetSdes();
        TRtpSdesParams* iNewSdes;   // Sdes to send to observer
        TInt iSendSdesCounter;      // keeps track of SDES packets sent

	 protected:
		CRtpPacket* iPktSnd;
        CRtpPacket* iPktRcv;
        TPtr8 iRtpRecvBuf;  // receive buf of iPktRtpRcv;
        TPtr8 iRtcpRecvBuf; // receive buf of iPktRtcpRcv;
        CRtpComm* iCommNet;
        MRtpPacketExt* iPktExtSnd;
        TBool iIsSrtp;
        TBool iRtcpEnabled;
        CRtpTimer* iSndRtcpTimer;     // timer for period sending RTCP packets

        // packets allocated for reporting, sending, receiving
        CRtpPacket* iPktRtcpSnd;
        CRtpPacket* iPktRtcpRcv;
        

  

        // packets allocated for extended rtp packet
        
        MRtpPacketExt* iPktExtRcv;

        // Flags
        TBool iFirstRTCPSent;        // flag that indicates if the first RTCP Packet has been sent 
        TBool iFPortsInit;           // indicates if the local port has been initialized 

        TUint16 iNumOfTxStreams;    // total number of send streams
        TUint16 iNumOfRxStreams;    // total number of receive streams
        CArrayFixFlat<TStream>* iStreamTxArray; //array to keep Transmit stream objects
        CArrayFixFlat<TStream>* iStreamRxArray; //array to keep Receive stream objects
        CArrayPtrFlat<CRtpSDES>* iSdesArray;     //array to keep remote participants SDES object
        RArray<TRtpSSRC> iRxSSRCArray; //array to keep ReceiveStreamSSRC Array

        const TUint32* iProfileRTPTimeRates;
        TReal iAverageRtcpSize;         // average RTCP report size updated everytime a RTCP report is send or/and received
        TUint iBandWidth;               // session bandwidth
        TReal iFraction;                // fraction of the session bandwidth to be used for RTCP report
        TUint iRtcpTimeInterval;        // RTCP report interval (milliseconds)
        TUint iTotalParticipantsSession;// keeps track of total participants in session

        MRtpObserver* iRtpObserver;
        MRtcpObserver* iRtcpObserver;
        MNonRTPDataObserver* iNonRTPDataObserver;
        MRtpErrNotify& iErrNotify;
        MRtpAsignUniqueID& iAssignUniqueID;
        MRtpPostProcessingObserver* iRtpPacketObserver;
        
        
        TBool iSessionStarted;
        TInt iSSRCJumps;
        TInt iRtcpErrors;
        
        TBool iRtcpSendingSuspended;
        TInt64 iSeed;
        
    private:    
	    #ifdef EUNIT_TESTING
		  friend class UT_CRtpAPI;
		  friend class UT_CRtpSession;
		  friend class UT_CRtpSessionSrtp;
		#endif   
		#if defined( _DEBUG ) && defined( RTP_UNIT_TEST )
        friend class CRtpUtRtpSession;
    	#endif   	    
    };

#endif   // __RTPSESSION_H
 
// End of File
