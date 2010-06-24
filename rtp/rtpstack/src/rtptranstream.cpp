/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies).
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




#include "rtptranstream.h"

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CRtpTranStream::CRtpTranStream( const TRtpPayloadType aPayloadType,
                                const TRtpId aSessionId,
                                const TRtpId aStreamId,
                                const TRtpSSRC aSSRC,
                                MRtcpObserver* aRtcpObserver,
                                const TUint32* aProfileRTPTimeRates )
    :
    CRtpStream( aStreamId,
                aSessionId,
                aProfileRTPTimeRates,
                aRtcpObserver,
                aPayloadType ),
    iFlagSentRTPPackets( EFalse ),
    iPreviousTime( 0 ),
    iPreviousRemoteSN( 0 ),
    iPrevRemoteTime( 0 ),
    iCumNumOctetsSent( 0 ),
    iCumNumOctetsReceived( 0 ),
    iCumNumOctetsSent_last( 0 ),
    iFSentRtcpReport( EFalse )
    {
    iLocalSSRC = aSSRC;
    iTimeStamp = 0;
    iSeqNumCycles = 0;

    for ( TUint i = 0; i < KSNMaxArray; i++ )
        {
        iSN_size[i] = 0;
        }
    // If the client application does not specify sequence numbers, we
    // randomize the first one.
    TTime tmp_time;
    tmp_time.HomeTime();
    TInt64 seed = tmp_time.Int64();
    iBaseSeqNum = static_cast<TRtpSequence>( TRtpUtil::Random( seed )
                                             & 0xFFFF ); // Use 16-LSB only
    iSeqNum = iBaseSeqNum;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CRtpTranStream::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CRtpTranStream* CRtpTranStream::NewL( const TRtpPayloadType aPayloadType,
                                      const TRtpId aSessionId,
                                      const TRtpId aTranStreamId,
                                      const TRtpSSRC aSSRC,
                                      MRtcpObserver* aRtcpObserver,
                                      const TUint32* aProfileRTPTimeRates )
    {
    CRtpTranStream* self =
        new ( ELeave ) CRtpTranStream( aPayloadType,
                                       aSessionId,
                                       aTranStreamId,
                                       aSSRC,
                                       aRtcpObserver,
                                       aProfileRTPTimeRates );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self 
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CRtpTranStream::~CRtpTranStream()
    {
    }


// ---------------------------------------------------------------------------
// TInt CRtpTranStream::ResetStreamStat()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpTranStream::ResetStreamStat()
    {
    TRtcpStats rtcpStat;

    RtcpStats( rtcpStat );
    
    rtcpStat.iRtcpSenderStats.iNumPacketsSent = 0;
    rtcpStat.iRtcpSenderStats.iCumNumOctetsSent = 0;
    rtcpStat.iRtcpSenderStats.iNTPTimeStampSec = 0;    // NTP seconds
    rtcpStat.iRtcpSenderStats.iNTPTimeStampFrac = 0;   // NTPfraction
    rtcpStat.iRtcpSenderStats.iTimeStamp = 0; 

    // The receiver stats are updated when receiving RR packets, so reset
    rtcpStat.iRtcpReceiverStats.iChannelBufferSize = 0;
    rtcpStat.iRtcpReceiverStats.iRoundTripDelay = 0;
    rtcpStat.iRtcpReceiverStats.iFractionLost = 0; 
    rtcpStat.iRtcpReceiverStats.iCumNumPacketsLost = 0;
    rtcpStat.iRtcpReceiverStats.iSeqNumReceived = 0;
    rtcpStat.iRtcpReceiverStats.iSSRC = 0;

    iRtcpStats = rtcpStat;
    
    iFlagSentRTPPackets = EFalse;
    iFSentRtcpReport = EFalse;

    iCumNumOctetsSent = 0;
    iCumNumOctetsSent_last = 0;
    iPreviousTime = 0;
    iPreviousRemoteSN = 0;
    iSeqNumCycles = 0;

    for ( TUint i = 0; i < KSNMaxArray; i++ )
        {
        iSN_size[i] = 0;
        }
        
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TInt CRtpTranStream::GetStreamStat()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpTranStream::GetStreamStat( TRtpPeerStat& aStat )
    {
    TRtcpStats rtcpStat;

    RtcpStats( rtcpStat );

    aStat.iCumNumOctetsSent = rtcpStat.iRtcpSenderStats.iCumNumOctetsSent;
    aStat.iNumPacketsSent = rtcpStat.iRtcpSenderStats.iNumPacketsSent;

    aStat.iTxBandwidth = rtcpStat.iRtcpReceiverStats.iTxBandwidth;
    aStat.iArrivalJitter = rtcpStat.iRtcpReceiverStats.iArrivalJitter;
    aStat.iCumNumPacketsLost = rtcpStat.iRtcpReceiverStats.iCumNumPacketsLost;
    aStat.iFractionLost = rtcpStat.iRtcpReceiverStats.iFractionLost;
    aStat.iRoundTripDelay = rtcpStat.iRtcpReceiverStats.iRoundTripDelay;
    aStat.iRxBandwidth = rtcpStat.iRtcpReceiverStats.iBandwidth;
    aStat.iChannelBufferSize = rtcpStat.iRtcpReceiverStats.iChannelBufferSize;
    aStat.iNTPTimeStampSec = rtcpStat.iRtcpSenderStats.iNTPTimeStampSec;
    aStat.iNTPTimeStampFrac = rtcpStat.iRtcpSenderStats.iNTPTimeStampFrac;
    aStat.iTimeStamp = rtcpStat.iRtcpSenderStats.iTimeStamp;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TInt CRtpTranStream::BuildRtpPacket()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpTranStream::BuildRtpPacket( const TRtpSendHeader& aHeaderInfo,
                                     const TDesC8& aPayloadData,
                                     TRtpSequence aSeqNum,
                                     TBool aSetSeqNum,
                                     CRtpPacket* aPktSnd,
                                     const TArray<TRtpCSRC> *aCsrcList)
    {
    TRtpPacketStreamParam streamParam;
    TRtpPacketIOParam inParam;

    if ( FirstPkg() )
        {
        RtpStreamSyncInit( aHeaderInfo.iTimestamp );
        SetFirstPkg( EFalse );
        }

    //
    // The interpretation of the marker is defined by a profile.
    // It is used for marking significant events such as frame boundaries or
    // talkspurts in the packet stream.

    // Timestamp
    // -- random initial value
    // -- monotonically increasing 
    // -- increases by one sampling period for fixed-rate audio
    // -- increases for every sample independent of whether the block is
    //    transmitted or dropped as silent.
    // -- used for playout and inter-media synchronization.
    //

    //
    // construct RTP packet
    //

    inParam.TRTP.padding = aHeaderInfo.iPadding;

    if ( aHeaderInfo.iHeaderExtension )
        {
        inParam.TRTP.fHeaderExtension = 1;
        inParam.TRTP.extension.type = aHeaderInfo.iHeaderExtension->iType;
        inParam.TRTP.extension.length = aHeaderInfo.iHeaderExtension->iLength;
        inParam.TRTP.extension.data = aHeaderInfo.iHeaderExtension->iData;
        }
    else
        {
        inParam.TRTP.extension.data = NULL;
        }

    inParam.TRTP.marker = aHeaderInfo.iMarker; 

    inParam.TRTP.payloadData = const_cast<TUint8*>( aPayloadData.Ptr() );
    inParam.TRTP.payloadDataLen = aPayloadData.Length();
    
    if(aCsrcList && aCsrcList->Count())
        {
        // Maxm 15 CSRC identifiers will be supported
        if( aCsrcList->Count() <= KMaxCsrcIdentifiers )
            {
            inParam.TRTP.numCSRC = (TUint8)aCsrcList->Count();
            }
        else
            {
            inParam.TRTP.numCSRC = KMaxCsrcIdentifiers;
            }
        
        inParam.TRTP.iCsrcList = aCsrcList;
        }

    streamParam.TRTP.payload = aHeaderInfo.iPayloadType;

    if ( aSetSeqNum )
        {
        iSeqNum = aSeqNum;
        }
    streamParam.TRTP.seqNum = iSeqNum;

    streamParam.TRTP.SSRC = iLocalSSRC;
    streamParam.TRTP.timeStamp = aHeaderInfo.iTimestamp;
    
    //
    // build and send the packet
    //
    aPktSnd->RtpPacketBuild( &streamParam, &inParam ); 

    
    RTP_DEBUG_DETAIL_DVALUE( "CRtpTranStream::BuildRtpPacket, Stream ID = ",
                  iStreamId );
    RTP_DEBUG_DETAIL_DVALUE( "CRtpTranStream::BuildRtpPacket, SSRC = ",
                  streamParam.TRTP.SSRC );
    RTP_DEBUG_DETAIL_DVALUE( "CRtpTranStream::BuildRtpPacket, Seq Num = ",
                  static_cast<TInt>( streamParam.TRTP.seqNum ) );
    RTP_DEBUG_DETAIL_DVALUE( "CRtpTranStream::BuildRtpPacket, Time Stamp = ",
                  aHeaderInfo.iTimestamp );
    
    
    // update sent octets and remote bandwidth parameters 
    IncCumNumOctetsSent( aPayloadData.Length() );

    // update stream parameters
    iTimeStamp = aHeaderInfo.iTimestamp;
    iSeqNum++;
    if ( iSeqNum == 0 )
        {
        iSeqNumCycles++;
        }

    SetSentRTPPackets( ETrue );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TInt CRtpTranStream::BuildRtcpBYEPacket()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpTranStream::BuildRtcpBYEPacket( const TDesC8& aReason,
                                         CRtpPacket* aPktRtcpSnd )
    {
    TInt paddingSize = 0;
    TRtpPacketStreamParam streamParam;
    TRtpPacketIOParam initParam;
    TInt ret = KErrNone;

    TInt byeSize = aReason.Size();

    // build RTCP BYE packet header
    TInt sourceCount = 1; 
    TInt length = 0;

    initParam.TRTCP_HEADER.pt = ERTCP_BYE;

    if ( ( 1 + byeSize ) % 4 == 0 )
        {
        // multiples of 4 bytes, no padding is needed
        // calculate total number of 32 bit words in SDES packet
        length = ( 1 + byeSize ) / 4 + 1;   
        }
    else
        {
        // not multiples of 4 bytes, padding is needed
        paddingSize = 4 - ( 1 + byeSize ) % 4;
        length = ( 1 + byeSize ) / 4 + 2;
        }

    initParam.TRTCP_HEADER.sourceCount = sourceCount;
    initParam.TRTCP_HEADER.length = length;
    aPktRtcpSnd->SetType( ERTCP_HEADER );

    aPktRtcpSnd->RtpPacketBuild( &streamParam, &initParam );

    streamParam.TRTCP_BYE.SSRC = iLocalSSRC;

    initParam.TRTCP_BYE.reason = ( TUint8 * ) aReason.Ptr();
    initParam.TRTCP_BYE.reasonSize = byeSize;
    initParam.TRTCP_BYE.paddingSize = paddingSize;

    aPktRtcpSnd->SetType( ERTCP_BYE );
    aPktRtcpSnd->RtpPacketBuild( &streamParam, &initParam ); 
    

    RTCP_DEBUG_DETAIL_DVALUE( "SEND: Send BYE Packet TX Stream ID = ",
              iStreamId );
    RTCP_DEBUG_DETAIL_DVALUE( "SEND: Send BYE Packet SSRC = ",
              iLocalSSRC );
    RTCP_DEBUG_DETAIL( "SEND: Sending RTCP BYE message as" );
    RTCP_DEBUG_DETAIL( initParam.TRTCP_BYE.reason );

    return ret;
    }

// ---------------------------------------------------------------------------
// TInt CRtpTranStream::BuildRtcpAPPPacket()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpTranStream::BuildRtcpAPPPacket( const TRtcpApp& aApp,
                                         CRtpPacket* aPktRtcpSnd )
    {
    TRtpPacketStreamParam streamParam;
    TRtpPacketIOParam initParam;

    initParam.TRTCP_HEADER.pt = ERTCP_APP;

    initParam.TRTCP_HEADER.sourceCount = aApp.iSubType;

    initParam.TRTCP_HEADER.length = aApp.iAppDataLen / 4 + 3 - 1;

    aPktRtcpSnd->SetType( ERTCP_HEADER );

    aPktRtcpSnd->RtpPacketBuild( &streamParam, &initParam );

    streamParam.TRTCP_APP.SSRC = iLocalSSRC;

    Mem::Copy( initParam.TRTCP_APP.name, aApp.iName, 4 );
    initParam.TRTCP_APP.appData = const_cast<TUint8*>( aApp.iAppData );
    initParam.TRTCP_APP.appDataLen = aApp.iAppDataLen;

    aPktRtcpSnd->SetType( ERTCP_APP );

    aPktRtcpSnd->RtpPacketBuild( &streamParam, &initParam ) ; 

    RTP_DEBUG_DETAIL_DVALUE( "SEND: Send APP Packet TX Stream ID = ", iStreamId );
    RTP_DEBUG_DETAIL_DVALUE( "SEND: Send APP Packet SSRC = " , iLocalSSRC );
    RTCP_DEBUG_DETAIL( "SEND: Sending RTCP APP packet as " );
    RTCP_DEBUG_DETAIL( "InitParam.TRTCP_APP.appData " );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CRtpTranStream::RtpStreamSyncInit()
// For transmission stream, iSyncInfo is initialized by the
// first packet sent.
// ---------------------------------------------------------------------------
//
void CRtpTranStream::RtpStreamSyncInit( TRtpTimeStamp aInitTimeStamp )
    {
    TUint32 gtTime = TRtpUtil::GtGetTime();

    TTime time1;
    TTime time2;
    TTimeIntervalSeconds interval;
    time1.HomeTime();
    _LIT( KTime, "19700101:000000.000000" );
    time2.Set( KTime ); // microsecond from Jan. 1 1970
    time1.SecondsFrom( time2, interval );

    iSyncInfo.iNTPTimeStampSec = interval.Int() + KGetTimeOfDayToNTPOffset;


    iSyncInfo.iNTPTimeStampFrac = static_cast<TUint32>(
        ( ( ( ( TUint32 ) ( gtTime % KTenthOfmsPerSecond ) ) << 16 )
        / KTenthOfmsPerSecond ) << 16 );

    iSyncInfo.iTimeStamp = aInitTimeStamp;
    iSyncInfo.iLastUpdateLocalTime = gtTime;
    }

// ---------------------------------------------------------------------------
// CRtpTranStream::RtpStreamSyncCurrent()
// iSyncInfo : NTP timestamp of initialization as a reference
//             to update the current NTP timestamp
// aSyncInfoCurrent : Bring back current NTP Time stamp
// ---------------------------------------------------------------------------
//
void CRtpTranStream::RtpStreamSyncCurrent( TRtpTimeSync* aSyncInfoCurrent )
    {
    if ( !aSyncInfoCurrent )
        {
        return;
        }
    TUint32 gtTime = TRtpUtil::GtGetTime();
    TUint32 diffFraction = ( gtTime - iSyncInfo.iLastUpdateLocalTime )
                           % KTenthOfmsPerSecond;

    aSyncInfoCurrent->iNTPTimeStampSec = iSyncInfo.iNTPTimeStampSec +
          static_cast<TUint32>( gtTime - iSyncInfo.iLastUpdateLocalTime )
          / KTenthOfmsPerSecond;
    aSyncInfoCurrent->iNTPTimeStampFrac = ( static_cast<TUint32>(
        ( diffFraction << 16 ) / KTenthOfmsPerSecond ) << 16 );
    }

// ---------------------------------------------------------------------------
// CRtpTranStream::RtpStreamCreateRtcpReportSection()
// For transmission stream, it only generates SR packet.
// ---------------------------------------------------------------------------
//
void CRtpTranStream::RtpStreamCreateRtcpReportSection( CRtpPacket* aPkt )
    {
    TRtpPacketStreamParam streamParam;
    TRtpPacketIOParam inParam;

    TRtpTimeSync syncInfoCurrent;

    streamParam.TRTCP_SR.SSRC = iLocalSSRC;

    streamParam.TRTCP_SR.numPacketsSent = iSeqNum + ( iSeqNumCycles << 16 )
                                          - iBaseSeqNum;
    streamParam.TRTCP_SR.cumNumOctetsSent = iCumNumOctetsSent;

    iRtcpStats.iRtcpSenderStats.iNumPacketsSent =
        streamParam.TRTCP_SR.numPacketsSent;
    iRtcpStats.iRtcpSenderStats.iCumNumOctetsSent =
        streamParam.TRTCP_SR.cumNumOctetsSent;
    iRtcpStats.iRtcpSenderStats.iSSRC = streamParam.TRTCP_SR.SSRC;

    RtpStreamSyncCurrent( &syncInfoCurrent );

    inParam.TRTCP_SR.NTPTimeStampSec = syncInfoCurrent.iNTPTimeStampSec;
    inParam.TRTCP_SR.NTPTimeStampFrac = syncInfoCurrent.iNTPTimeStampFrac;
    inParam.TRTCP_SR.timeStamp = iTimeStamp;

    iRtcpStats.iRtcpSenderStats.iNTPTimeStampSec =
        inParam.TRTCP_SR.NTPTimeStampSec;
    iRtcpStats.iRtcpSenderStats.iNTPTimeStampFrac =
        inParam.TRTCP_SR.NTPTimeStampFrac;
    iRtcpStats.iRtcpSenderStats.iTimeStamp = inParam.TRTCP_SR.timeStamp;
    

    RTP_DEBUG_STAT( "----- TX: Create RTCP Report Statistics -----" );
    RTP_DEBUG_STAT_DVALUE( "TX: numPacketsSent = ",
              iRtcpStats.iRtcpSenderStats.iNumPacketsSent );
    RTP_DEBUG_STAT_DVALUE( "TX: cumNumOctetsSent = ",
              iRtcpStats.iRtcpSenderStats.iCumNumOctetsSent );
    RTP_DEBUG_STAT_DVALUE( "TX: SSRC (Tx) = ",
              iRtcpStats.iRtcpSenderStats.iSSRC );
    RTP_DEBUG_STAT_DVALUE( "TX: NTPTimeStampSec = ",
              iRtcpStats.iRtcpSenderStats.iNTPTimeStampSec );
    RTP_DEBUG_STAT_DVALUE( "TX: NTPTimeStampFrac = ",
              iRtcpStats.iRtcpSenderStats.iNTPTimeStampFrac );
    RTP_DEBUG_STAT_DVALUE( "TX: timeStamp = ",
              iRtcpStats.iRtcpSenderStats.iTimeStamp );


    // For transmission stream, it only generates SR packet.
    aPkt->SetType( ERTCP_SR );
    aPkt->RtpPacketBuild( &streamParam, &inParam );
    }


// ---------------------------------------------------------------------------
// TRtpRtcpEnum CRtpTranStream::RtpStreamProcessRtcpReportSectionL()
// For transmission stream, it only processes RR packet.
// ---------------------------------------------------------------------------
//
TRtpRtcpEnum CRtpTranStream::RtpStreamProcessRtcpReportSectionL(
                                 CRtpPacket* aPkt )
    {
    TRtpPacketStreamParam streamParam;
    TRtpPacketIOParam extractParam;
    TRtpRtcpEnum parseResult = ERTCP_NO_ERROR;
    TUint32 gtTime = TRtpUtil::GtGetTime();

    parseResult = aPkt->RtpPacketProcessL( &streamParam, &extractParam );
    if ( parseResult == ERTCP_PACKET_ERROR )
        {
        return ERTCP_PACKET_ERROR;
        }

    // For transmission stream, it only processes RR packet.
    if ( aPkt->Type() == ERTCP_RR )
        {
        TInt roundTripDelay;

        roundTripDelay = RtpStreamSyncGetRoundTripDelay(
                             extractParam.TRTCP_RR.lastSRTimeStamp,
                             extractParam.TRTCP_RR.delaySinceLSR );

        iCumNumOctetsReceived += iSN_size[streamParam.TRTCP_RR.seqNumReceived
                                     % KSNMaxArray] -
                                 iSN_size[iPreviousRemoteSN % KSNMaxArray];

        if ( iCumNumOctetsSent > iCumNumOctetsReceived )
            {
            iRtcpStats.iRtcpReceiverStats.iChannelBufferSize =
                8 * ( iCumNumOctetsSent - iCumNumOctetsReceived );
            }
        else
            {
            iRtcpStats.iRtcpReceiverStats.iChannelBufferSize = 0;
            }

        iRtcpStats.iRtcpReceiverStats.iRoundTripDelay = roundTripDelay;
        iRtcpStats.iRtcpReceiverStats.iFractionLost =
            streamParam.TRTCP_RR.fractionLost;
        iRtcpStats.iRtcpReceiverStats.iCumNumPacketsLost =
            streamParam.TRTCP_RR.cumNumPacketsLost;
        iRtcpStats.iRtcpReceiverStats.iSeqNumReceived =
            streamParam.TRTCP_RR.seqNumReceived;

        // SSRC coming from the source 
        iRtcpStats.iRtcpReceiverStats.iSSRC = streamParam.TRTCP_RR.SSRC;

        EstimateBandWidths( gtTime );

        // Remember some things for next time we receive an RR
        iPreviousRemoteSN = streamParam.TRTCP_RR.seqNumReceived;
        iPrevRemoteTime = gtTime;
        iPreviousTime = gtTime;
        iCumNumOctetsSent_last = iCumNumOctetsSent;
        
        
        RTP_DEBUG_STAT( "----- TX: Process RTCP Report Statistics -----" );
        RTP_DEBUG_STAT_DVALUE( "TX: bandwidth (Tx) = ",
                  iRtcpStats.iRtcpReceiverStats.iTxBandwidth );
        RTP_DEBUG_STAT_DVALUE( "TX: roundTripDelay = ",
                  iRtcpStats.iRtcpReceiverStats.iRoundTripDelay );
        RTP_DEBUG_STAT_DVALUE( "TX: fractionLost = ",
                  static_cast<TInt>(
                      iRtcpStats.iRtcpReceiverStats.iFractionLost ) );
        RTP_DEBUG_STAT_DVALUE( "TX: cumNumPacketsLost = ",
                  iRtcpStats.iRtcpReceiverStats.iCumNumPacketsLost );
        RTP_DEBUG_STAT_DVALUE( "TX: seqNumReceived = ",
                  iRtcpStats.iRtcpReceiverStats.iSeqNumReceived );
        RTP_DEBUG_STAT_DVALUE( "TX: SSRC (Rx) = ",
                  iRtcpStats.iRtcpReceiverStats.iSSRC );
        
        
        if ( iRtcpObserver )
            {
            iRtcpObserver->RrReceived( iStreamId,
                                       iRtcpStats.iRtcpReceiverStats.iSSRC );
            }
        }

    return parseResult;
    }

// ---------------------------------------------------------------------------
// TUint32 CRtpTranStream::EstimateBandWidths()
// Make an estimate of the Tx and Rx bandwidths.
// ---------------------------------------------------------------------------
// 
void CRtpTranStream::EstimateBandWidths( TUint32 aCurrentTime )
    {
    /* NOTE: This is just a simple estimate, which assumes the following:
     * - this is an end-to-end session
     * - the sessions and streams are set up before traffic starts
     * - the payload bitrate is constant
     * The result of this estimation should not be used in any other contexts.
     */

    // Tx bandwidth
    TReal B_A( 0 );

    // Rx bandwidth
    TReal B_B( 0 );

    // number of received packets between the current and previous RR's
    TInt N_B( 0 );

    TReal avgPacketSize( 0 );
    TUint8 fracLost_i( iRtcpStats.iRtcpReceiverStats.iFractionLost );
    TUint32 lastRecvd_i( iRtcpStats.iRtcpReceiverStats.iSeqNumReceived ); 
    TUint32 lastRecvd_k( iPreviousRemoteSN );
    TUint packetsSent( iRtcpStats.iRtcpSenderStats.iNumPacketsSent ); 

    TReal timeBetweenRRs = static_cast<TReal>( aCurrentTime - iPreviousTime )
                           / KTenthOfmsPerSecond;

    // Continue only if two RR's have been received and numbers are valid
    if ( iCumNumOctetsSent_last == 0 ||
         iPreviousRemoteSN == 0 ||
         iPrevRemoteTime == 0 ||
         iPreviousTime == 0 ||
         timeBetweenRRs <= 0 ||
         packetsSent == 0 ||
         fracLost_i == 255 /* Would result in divide by zero and B_B = 0 */ )
        {
        // If this is the first RR to be received, this returns 0,
        // otherwise this returns the result of the previous calculation
        iRtcpStats.iRtcpReceiverStats.iBandwidth = 0;
        iRtcpStats.iRtcpReceiverStats.iTxBandwidth = 0;
        return;
        }

    B_A = 8 * static_cast<TReal>( ( iCumNumOctetsSent -
              iCumNumOctetsSent_last ) ) / timeBetweenRRs;
    iRtcpStats.iRtcpReceiverStats.iTxBandwidth = static_cast<TUint32>( B_A );

    avgPacketSize = static_cast<TReal>( iCumNumOctetsSent )/ packetsSent;

    /* The fraction lost is the fractional part of a decimal value between 0 
     * and 1 (the fixed point is at the left edge of this value. The decimal
     * equivalent can be computed by dividing by 255. */
    N_B = static_cast<TInt>( 
              ( lastRecvd_i - lastRecvd_k ) * ( 1 - ( fracLost_i / 255.0 ) ) );

    B_B = 8 * ( avgPacketSize * N_B ) / timeBetweenRRs;
    
    iRtcpStats.iRtcpReceiverStats.iBandwidth = static_cast<TUint32>( B_B );
    }

// ---------------------------------------------------------------------------
// TUint32 CRtpTranStream::RtpStreamSyncGetRoundTripDelay()
// It is transmission stream's duty to calculate round trip delay.
// ---------------------------------------------------------------------------
// 
TUint32 CRtpTranStream::RtpStreamSyncGetRoundTripDelay(
                            TUint32 aLastSRTimeStamp, TUint32 aDelaySinceLSR )
    {
    TInt roundTripDelay = 0;
    TRtpTimeStamp currentNTP;
    TUint32 gtTime = TRtpUtil::GtGetTime();
    TUint32 diffFraction = ( gtTime - iSyncInfo.iLastUpdateLocalTime )
                           % KTenthOfmsPerSecond;

    if ( aLastSRTimeStamp != 0 )
        {
        // calculate current NTP in a 16+16 bit precision
        // (same format as lastSRTimeStamp)
        currentNTP = ( ( iSyncInfo.iNTPTimeStampSec +
                       ( gtTime - iSyncInfo.iLastUpdateLocalTime )
                       / KTenthOfmsPerSecond ) << 16 );    
        currentNTP += static_cast<TUint32>( ( diffFraction << 16 )
                      / KTenthOfmsPerSecond );

        roundTripDelay = currentNTP - aLastSRTimeStamp - aDelaySinceLSR;
        if ( roundTripDelay > 0 )
            {
            // 16-MSB of roundTripDelay are seconds, 16-LSB of roundTripDelay
            // is the fraction part. Transform this to a decimal value
            // representing the round trip delay in tenths of millisecons
            // ( 10^4 ). See RFC 3550 page 34 for details.
            roundTripDelay = ( static_cast<TUint32>( roundTripDelay ) >> 16 ) *
                             KTenthOfmsPerSecond +
                             ( ( ( roundTripDelay & 0xFFFF ) *
                                 KTenthOfmsPerSecond ) >> 16 );
            }
        else
            {
            // here we assume the delay is so small that we cannot get
            // an accurate reading
            roundTripDelay = 0;     
            }
        }

    return static_cast<TUint32>( roundTripDelay );
    }

//end of file


