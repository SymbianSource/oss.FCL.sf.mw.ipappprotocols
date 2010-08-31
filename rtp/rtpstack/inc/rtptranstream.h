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





#ifndef RtpTranStream_H
#define RtpTranStream_H

#include "rtpstream.h"


/**
*  Derived class for RTP transmit stream. 
*
*  @lib RtpService.dll
*/
class CRtpTranStream : public CRtpStream
    {
    #if defined( _DEBUG ) && defined( RTP_UNIT_TEST )
	    //Friend class used for testing
	    friend class CRtpUtRtpStream; 
    #endif 
    public:
        static CRtpTranStream* NewL( const TRtpPayloadType aPayloadType,
                                     const TRtpId aSessionID,
                                     const TRtpId aTranStreamID,
                                     const TRtpSSRC aSSRC,
                                     MRtcpObserver* aObserver,
                                     const TUint32* aProfileRTPTimeRates );
        ~CRtpTranStream();

        TInt BuildRtpPacket( const TRtpSendHeader& aHeaderInfo,
                             const TDesC8& aPayloadData,
                             TRtpSequence aSeqNum,
                             TBool aSetSeqNum,
                             CRtpPacket* aPktSnd,
                             const TArray<TRtpCSRC> *aCsrcList = NULL);

        TInt BuildRtcpBYEPacket( const TDesC8& aReason,
                                 CRtpPacket* aPktRtcpSnd );

        TInt BuildRtcpAPPPacket( const TRtcpApp& aApp,
                                 CRtpPacket* aPktRtcpSnd );

        inline TBool SentRTPPackets() const { return iFlagSentRTPPackets; };
        inline void SetSentRTPPackets( TBool aFlag )
            {
            iFlagSentRTPPackets = aFlag;
            };

        inline TBool SentRtcpReport() { return iFSentRtcpReport; };
        inline void SetRtcpReportFlag() { iFSentRtcpReport = ETrue; };
        inline void ResetRtcpReportFlag() { iFSentRtcpReport = EFalse; };
        inline void SetBandwidth( TUint32 aBandwidth ) 
            { 
            iRtcpStats.iRtcpReceiverStats.iTxBandwidth = aBandwidth;
            }; 

    public: // from CRtpStream
        TInt ResetStreamStat();
        TInt GetStreamStat( TRtpPeerStat& aStat );
        TRtpRtcpEnum RtpStreamProcessRtcpReportSectionL( CRtpPacket* aPkt );
        void RtpStreamCreateRtcpReportSection( CRtpPacket* aPkt );

    private:
        CRtpTranStream( const TRtpPayloadType aPayloadType,
                        const TRtpId aSessionId,
                        const TRtpId aStreamId,
                        const TRtpSSRC aSSRC,
                        MRtcpObserver* aRtcpObserver,
                        const TUint32* aProfileRTPTimeRates );
        void ConstructL();

        inline void IncCumNumOctetsSent( TUint32 aInc ) 
            {
            iCumNumOctetsSent += aInc;
            iSN_size[iSeqNum % KSNMaxArray] = iCumNumOctetsSent;
            };

        void RtpStreamSyncInit( TRtpTimeStamp initTimeStamp );
        TUint32 RtpStreamSyncGetRoundTripDelay( TUint32 lastSRTimeStamp,
                                                TUint32 delaySinceLSR );
        void RtpStreamSyncCurrent( TRtpTimeSync* aSyncInfoCurrent );

        /**
         * Makes an estimate of the Tx and Rx bandwidths.
         * NOTE: This is just a simple estimate, which assumes the following:
         * - this is an end-to-end session
         * - the sessions and streams are set up before traffic starts
         * - the payload bitrate is constant
         * The result of this estimation should not be used in any other
         * contexts.
         */
        void EstimateBandWidths( TUint32 aCurrentTime );

    private:
        // flag to indicate if a packet was sent since last RTCP packet
        TBool iFlagSentRTPPackets;

        // time of previous RTCP packet sent
        TUint32 iPreviousTime;

        // remote bandwidth
        TUint32 iRemoteBandwidth;

        // previous remote sequence number
        TUint32 iPreviousRemoteSN;

        // time of previous RTCP packet received
        TUint32 iPrevRemoteTime;

        // number of bytes sent
        TUint32 iCumNumOctetsSent;

        // keeps track of how many bytes were received by the receiver
        TUint32 iCumNumOctetsReceived;

        // previous value of cumNumOctetsSent
        TUint32 iCumNumOctetsSent_last;

        // keeps track of the total bytes sent
        TUint32 iSN_size[KSNMaxArray];

        // if ETrue, then packet sequence number is given by application
        TBool iFSetSeqNum;

        // flag that indicates if an RTCP Packet has been sent
        TBool iFSentRtcpReport; 
     private:    
	    #ifdef EUNIT_TESTING
		  	friend class UT_CRtpSession;
			friend class UT_CRtpStream;
			friend class UT_CRtpSessionSrtp;
		#endif     	    
            
    };


#endif	// RtpTranStream_H
