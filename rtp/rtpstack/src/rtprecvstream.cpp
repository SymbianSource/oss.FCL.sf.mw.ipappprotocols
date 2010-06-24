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




#include "rtprecvstream.h"
 


// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CRtpRecvStream::CRtpRecvStream( MSsrcCheckCallback* aCallback,
                                const TRtpPayloadType aPayloadType,
                                const TRtpId aSessionId,
                                const TRtpId aReceiveStreamId,
                                MRtpObserver** aRtpObserver,
                                MRtcpObserver* aRtcpObserver,
                                const TUint32* aProfileRTPTimeRates )
    :
    CRtpStream( aReceiveStreamId,
             aSessionId,
             aProfileRTPTimeRates,
             aRtcpObserver,
             aPayloadType ),
    iNumWrapAround( 0 ),
    iRtpObserver( aRtpObserver ),
    iCallback( aCallback )
    {
    // set syncInfo values to zero 
    RtpStreamJitterInit();
    RtpStreamSyncInfoInit();
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CRtpRecvStream::ConstructL()
    {
    iRemoteSDES = CRtpSDES::NewL();
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CRtpRecvStream* CRtpRecvStream::NewL( const TRtpPayloadType aPayloadType,
                                      const TRtpId aSessionId,
                                      const TRtpId aReceiveStreamId,
                                      MRtpObserver** aRtpObserver,
                                      MRtcpObserver* aRtcpObserver,
                                      const TUint32* aProfileRTPTimeRates,
                                      MSsrcCheckCallback* aCallback )
    {
    CRtpRecvStream* self =
        new ( ELeave ) CRtpRecvStream( aCallback,
                                       aPayloadType,
                                       aSessionId,
                                       aReceiveStreamId,
                                       aRtpObserver,
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
CRtpRecvStream::~CRtpRecvStream()
    {
	delete iRemoteSDES;
    }


// ---------------------------------------------------------------------------
// RegisterRtpObserver
// ---------------------------------------------------------------------------
//
void CRtpRecvStream::RegisterRtpObserver(MRtpObserver* aRtpObserver)
    {
    *iRtpObserver = aRtpObserver; 
    }


// ---------------------------------------------------------------------------
// UnRegisterRtpObserver
// ---------------------------------------------------------------------------
//
void CRtpRecvStream::UnRegisterRtpObserver()
    {
    *iRtpObserver = NULL;
    }

// ---------------------------------------------------------------------------
// TInt CRtpRecvStream::ResetStreamStat()
// ---------------------------------------------------------------------------
//
TInt CRtpRecvStream::ResetStreamStat()
    {
    TRtcpStats rtcpStat;

    RtcpStats( rtcpStat );
    
    rtcpStat.iRtcpReceiverStats.iFractionLost = 0;
    rtcpStat.iRtcpReceiverStats.iCumNumPacketsLost = 0;
    rtcpStat.iRtcpReceiverStats.iSeqNumReceived = 0; 
    rtcpStat.iRtcpReceiverStats.iArrivalJitter = 0;
    rtcpStat.iRtcpReceiverStats.iRoundTripDelay = 0;
    rtcpStat.iRtcpReceiverStats.iChannelBufferSize = 0;
   
    // The sender stats are updated when receiving SR packets, so reset 
    rtcpStat.iRtcpSenderStats.iSSRC = 0;
    rtcpStat.iRtcpSenderStats.iCumNumOctetsSent = 0;
    rtcpStat.iRtcpSenderStats.iNumPacketsSent = 0;
    rtcpStat.iRtcpSenderStats.iNTPTimeStampSec = 0;
    rtcpStat.iRtcpSenderStats.iNTPTimeStampFrac = 0;
    rtcpStat.iRtcpSenderStats.iTimeStamp = 0;

    iRtcpStats = rtcpStat;
    
    iNumReceivedPackets = 0;
    iNumWrapAround = 0;   
    
    // set syncInfo values to zero 
    RtpStreamJitterInit();
    RtpStreamSyncInfoInit();
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TInt CRtpTranStream::GetStreamStat()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpRecvStream::GetStreamStat( TRtpPeerStat& aStat )
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
    aStat.iNTPTimeStampSec = 0;
    aStat.iNTPTimeStampFrac = 0;
    aStat.iTimeStamp = 0;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TInt CRtpRecvStream::RtpStreamProcessRtpPacketL()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpRecvStream::RtpStreamProcessRtpPacketL( CRtpPacket* aPktRcv, TBool aAssignStream )
    {
    RTP_DEBUG_DETAIL( "CRtpRecvStream::RtpStreamProcessRtpPacketL Entry" );
    aPktRcv->RtpPacketResetPtr();

    // Check if this packet belongs to this stream by looking at the source SSRC
    if ( !aAssignStream )
        {
        TRtpSSRC aRemoteSSRC( NULL );
        aPktRcv->SetType( ERTP );
        aRemoteSSRC = aPktRcv->RtpPacketGetSSRC();
        if ( iRemoteSSRC != aRemoteSSRC )
            {
            RTP_DEBUG_DETAIL( "RtpRecvStream: Wrong SSRC" );
            return KErrNotFound;
            }
        }
    else
        {
        //assigned RTP packet SSRC if this stream's remote SSRC is NULL
        // if it is not null then return KErrNotFound
        if ( iRemoteSSRC )
            {
            RTP_DEBUG_DETAIL( "RtpRecvStream: Did not expect own SSRC" );
            return KErrNotFound;
            }
        }

    TRtpPacketStreamParam streamParam;
    TRtpPacketIOParam extractParam;

    // process RTP packet
    aPktRcv->SetType( ERTP );

    extractParam.TRTP.extension.data = NULL;

    if ( aPktRcv->RtpPacketProcessL( &streamParam, &extractParam ) < 0 )
        {
        RTP_DEBUG_DETAIL( "Invalid Rtp packet is received" );
        return KErrCorrupt;
        }
	if (!iCallback->CheckRemoteAddr())
		{
		RTP_DEBUG_DETAIL( "Packet comes from wrong remote address" );
		return KErrNotFound;
		}
    // update and check stream parameters
    if ( RtpStreamUpdateParamL( aPktRcv->Type(), &streamParam ) < 0 )
        {
        
        RTP_DEBUG_PACKET("RtpRecvStream: Could not update params" );
        
        return KErrNotFound;
        }

    iHdr.iPadding = extractParam.TRTP.padding;
    iHdr.iExtension = extractParam.TRTP.fHeaderExtension;
    iHdr.iCsrcCount = extractParam.TRTP.numCSRC;
    
    if ( iHdr.iCsrcCount )
        {
        iHdr.iCsrcList = extractParam.TRTP.CSRCarray;
        }

    iHdr.iMarker = extractParam.TRTP.marker;
    iHdr.iTimestamp = streamParam.TRTP.timeStamp;
    iHdr.iPayloadType = streamParam.TRTP.payload; // iPayload; pass payload type to app,even if it changed

    if ( iHdr.iExtension )
        {
		iHdr.iHeaderExtension = &iHeaderExtension;
        iHdr.iHeaderExtension->iType = extractParam.TRTP.extension.type;
        iHdr.iHeaderExtension->iLength = extractParam.TRTP.extension.length;
        iHdr.iHeaderExtension->iData = extractParam.TRTP.extension.data;
        }

    iFlagReceivedRTPPackets = ETrue;

    // calculate jitter, maybe it eats a big portion of CPU time?
    RtpStreamUpdateJitter( iHdr.iTimestamp );

    iHdr.iSeqNum = streamParam.TRTP.seqNum;
    
    RTP_DEBUG_DETAIL_DVALUE( "The received RTP packet size = ", extractParam.TRTP.payloadDataLen );
    RTP_DEBUG_DETAIL_DVALUE( "The received RTP packet Seq Num = ", streamParam.TRTP.seqNum );
    
    TPtrC8 payloadData( extractParam.TRTP.payloadData, extractParam.TRTP.payloadDataLen );

    if ( *iRtpObserver )
        {
        RTP_DEBUG_PACKET("RTP packet received and pass to upper application" );
        
        (*iRtpObserver)->RtpPacketReceived( iStreamId, iHdr, payloadData );
        }
    else
        {
        RTP_DEBUG_PACKET( "RtpRecvStream: No observer registered" );
        }
   	if(aPktRcv->iExdataAlloc )
      	{
       	User::Free( extractParam.TRTP.extension.data );
       	}
   	if (aPktRcv->iCsrcAlloc)
   		{
   		User::Free( extractParam.TRTP.CSRCarray);
   		}    
   	RTP_DEBUG_DETAIL( "CRtpRecvStream::RtpStreamProcessRtpPacketL Exit" );
    	
    return KErrNone;
    }



// ---------------------------------------------------------------------------
// CRtpRecvStream::RtpStreamSyncInfoInit()
// For receiving stream, iSyncInfo is initialized to zero
// and will be updated by SR received for the future RR creating
// iSyncInfo.timeStamp & iSyncInfo.lastUpdateLocalTime are
// not useful actually.
// ---------------------------------------------------------------------------
// 
void CRtpRecvStream::RtpStreamSyncInfoInit()
    {
    iSyncInfo.iNTPTimeStampFrac = 0;
    iSyncInfo.iNTPTimeStampSec = 0;
    iSyncInfo.iTimeStamp = 0;
    iSyncInfo.iLastUpdateLocalTime = 0;
    }

// ---------------------------------------------------------------------------
// CRtpRecvStream::RtpStreamJitterInit()
// 
// ---------------------------------------------------------------------------
//
void CRtpRecvStream::RtpStreamJitterInit()
    {
    iSyncJitter.iLastPacketS = 0;
    iSyncJitter.iLastPacketR = 0;
    iSyncJitter.iJitterTime = 0;

    iSyncJitter.iWaitTime = 0;
    }

// ---------------------------------------------------------------------------
// CRtpRecvStream::RtpStreamUpdateJitter()
// 
// ---------------------------------------------------------------------------
//
void CRtpRecvStream::RtpStreamUpdateJitter( TRtpTimeStamp aCurrentTimeStamp )
    {
    TUint32 gtTime = TRtpUtil::GtGetTime();
    TRtpTimeStamp packetRTP( aCurrentTimeStamp );
    TInt jitterTime = 0;

    if ( packetRTP == iSyncJitter.iLastPacketS )
        {
        return;
        }

    jitterTime = ( ( ( gtTime - iSyncJitter.iLastPacketR ) * iSyncJitter.iTimeStampResolution ) 
                 / KTenthOfmsPerSecond ) - ( packetRTP - iSyncJitter.iLastPacketS );

    iSyncJitter.iJitterTime = iSyncJitter.iJitterTime + 
        ( ( ( TReal64 ) Abs( jitterTime ) - iSyncJitter.iJitterTime ) / 16.0 );

    // if jitter > 100 seconds, reset it. 
    if ( iSyncJitter.iJitterTime > 1000000 ) // 100 * KTenthOfmsPerSecond 
        {
        iSyncJitter.iJitterTime = 0;
        }
    iSyncJitter.iLastPacketR = gtTime;
    iSyncJitter.iLastPacketS = packetRTP;
    }

// ---------------------------------------------------------------------------
// TInt CRtpRecvStream::RtpStreamUpdateParamL()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpRecvStream::RtpStreamUpdateParamL( TRtpPacketType aPType, 
                                           TRtpPacketStreamParam* aStreamParam )
    {
    TUint32 timerate;
    TInt error( KErrNone );

    if ( aPType == ERTP )
        {
        // Check if there is an SSRC collision
        if ( iCallback )
            {
            error = iCallback->CheckRemoteSsrcL( aStreamParam->TRTP.SSRC );
            }
        if ( error < KErrNone )
            {
            return KErrCorrupt;
            }
        if ( error > KErrNone )
            {
            // There was an SSRC collision, and TRTP.SSRC now contains the
            // updated SSRC value.
            iRemoteSSRC = aStreamParam->TRTP.SSRC;
            }

        if ( iFlagFirstPkg )
            {
            iRemoteSSRC = aStreamParam->TRTP.SSRC;
            iPayload = aStreamParam->TRTP.payload;
            iBaseSeqNum = aStreamParam->TRTP.seqNum;

            timerate = *( iProfileRTPTimeRates + iPayload );
            if ( timerate != 0 )
                {
                iSyncJitter.iTimeStampResolution = KMicrosecondPerSecond / timerate;
                }
            else
                {
                iSyncJitter.iTimeStampResolution = 0;
                }
            }
        else if ( iRemoteSSRC != aStreamParam->TRTP.SSRC )
            {
            // SSRC and seqNum change. start monitoring if this change is intentional, 
            // if several packets received sequentionally, and then re-initialize the stream 
            
            RTP_DEBUG_DETAIL( "SSRC changed" );
            
            
            return KErrNotFound;
            }
        else
            {
            if ( iPayload != aStreamParam->TRTP.payload )
                {
                // payload type changed, inform application somehow 
                RTP_DEBUG_DETAIL( "payload type changed" );
                
                }
            }

        if ( aStreamParam->TRTP.seqNum == TRtpUtil::Min16( aStreamParam->TRTP.seqNum, iSeqNum ) && !iFlagFirstPkg )
            {
            // if there is no buffering, out of order and duplicate packets
            // have to be discarded
            return KErrNone; // give this packet to the application as there is no buffer in RTP
            }
        else
            {
            //update the highest received SN and TS
            if ( TRtpUtil::Wrap16( iSeqNum, aStreamParam->TRTP.seqNum ) && !iFlagFirstPkg )
                {
                iSeqNumCycles++;
                }
            iSeqNum = aStreamParam->TRTP.seqNum;
            iTimeStamp = aStreamParam->TRTP.timeStamp;
            iFlagFirstPkg = 0;
            iNumReceivedPackets++;
            }
        }
    else
        {
        return KErrNotFound;
        }

    return KErrNone;
    } 


// ---------------------------------------------------------------------------
// CRtpRecvStream::RtpStreamCreateRtcpReportSection()
// For receiving stream, it only creates RR packet
// ---------------------------------------------------------------------------
// 
void CRtpRecvStream::RtpStreamCreateRtcpReportSection( CRtpPacket* aPkt )
    {
    TRtpPacketStreamParam streamParam;
    TRtpPacketIOParam inParam;

    TUint32 numPacketsExpected;
    TUint32 numPacketsIntervalLost;
    TUint32 numPacketsIntervalExpected;

    streamParam.TRTCP_RR.SSRC = iRemoteSSRC;

    // iSeqNum is 16 bit, but streamParam.TRTCP_RR.seqNumReceived is 32 bits. 
    // Check RFC1889 for RTCP part 

    streamParam.TRTCP_RR.seqNumReceived = iSeqNum + ( iSeqNumCycles << 16 );
    numPacketsExpected = streamParam.TRTCP_RR.seqNumReceived - iBaseSeqNum + 1;

    if ( numPacketsExpected < iNumReceivedPackets )
        {
        streamParam.TRTCP_RR.cumNumPacketsLost = 0;
        }
    else
        {
        if ( ( numPacketsExpected + ( 1 << 16 ) * iNumWrapAround ) < iLastRR_numExpectedPackets )
            {
            iNumWrapAround++;
            numPacketsExpected += ( 1 << 16 ) * iNumWrapAround;
            }
        else
            {
            numPacketsExpected += ( 1 << 16 ) * iNumWrapAround;
            }
        streamParam.TRTCP_RR.cumNumPacketsLost = numPacketsExpected - iNumReceivedPackets;
        }

    numPacketsIntervalExpected = numPacketsExpected - iLastRR_numExpectedPackets;
    numPacketsIntervalLost = numPacketsIntervalExpected - ( iNumReceivedPackets - iLastRR_numReceivedPackets );

    //
    // From RFC 1889
    // fraction lost: 8 bits
    // The fraction of RTP data packets from source SSRC_n lost since the previous SR or RR packet was sent, 
    // expressed as a fixed point number with the binary point at the left edge of the field. (That is 
    // equivalent to taking the integer part after multiplying the loss fraction by 256.) This fraction is
    // defined to be the number of packets lost divided by the number of packets expected.
    //

    if ( ( numPacketsIntervalExpected == 0 ) || ( numPacketsIntervalLost == 0 ) )
        {
        streamParam.TRTCP_RR.fractionLost = 0;
        }
    else
        {
        streamParam.TRTCP_RR.fractionLost = static_cast<TUint8>(
            ( numPacketsIntervalLost << 8 ) / numPacketsIntervalExpected );
        }

    iLastRR_numReceivedPackets = iNumReceivedPackets;
    iLastRR_numExpectedPackets = numPacketsExpected;

    streamParam.TRTCP_RR.arrivalJitter = static_cast<TUint32>( iSyncJitter.iJitterTime );

    // add receiver statistics to sender of SR packet 
    iRtcpStats.iRtcpReceiverStats.iRoundTripDelay = 0;
    iRtcpStats.iRtcpReceiverStats.iFractionLost = streamParam.TRTCP_RR.fractionLost;
    iRtcpStats.iRtcpReceiverStats.iCumNumPacketsLost = streamParam.TRTCP_RR.cumNumPacketsLost;
    iRtcpStats.iRtcpReceiverStats.iSeqNumReceived = streamParam.TRTCP_RR.seqNumReceived;
    iRtcpStats.iRtcpReceiverStats.iBandwidth = 0;
    iRtcpStats.iRtcpReceiverStats.iChannelBufferSize = 0;

    if ( iSyncJitter.iTimeStampResolution > 0 )
        {
        iRtcpStats.iRtcpReceiverStats.iArrivalJitter = ( streamParam.TRTCP_RR.arrivalJitter * KTenthOfmsPerSecond ) /
                                                      iSyncJitter.iTimeStampResolution;
        }
    else
        {
        iRtcpStats.iRtcpReceiverStats.iArrivalJitter = 0;
        }

    iRtcpStats.iRtcpReceiverStats.iSSRC = streamParam.TRTCP_RR.SSRC; // SSRC sender (coming source)
    
    
    RTP_DEBUG_STAT( "----- RX: Create RTCP Report Statistics -----" );
    RTP_DEBUG_STAT_DVALUE( "RX: roundTripDelay = ", iRtcpStats.iRtcpReceiverStats.iRoundTripDelay );
    RTP_DEBUG_STAT_DVALUE( "RX: fractionLost = ", (TInt)iRtcpStats.iRtcpReceiverStats.iFractionLost );
    RTP_DEBUG_STAT_DVALUE( "RX: cumNumPacketsLost = ", iRtcpStats.iRtcpReceiverStats.iCumNumPacketsLost );
    RTP_DEBUG_STAT_DVALUE( "RX: seqNumReceived = ", iRtcpStats.iRtcpReceiverStats.iSeqNumReceived );
    RTP_DEBUG_STAT_DVALUE( "RX: bandwidth = ", iRtcpStats.iRtcpReceiverStats.iBandwidth );
    RTP_DEBUG_STAT_DVALUE( "RX: channelBufferSize = ", iRtcpStats.iRtcpReceiverStats.iChannelBufferSize );
    RTP_DEBUG_STAT_DVALUE( "RX: arrivalJitter = ", iRtcpStats.iRtcpReceiverStats.iArrivalJitter );
    RTP_DEBUG_STAT_DVALUE( "RX: SSRC (Tx) = ", iRtcpStats.iRtcpReceiverStats.iSSRC );

    TUint32 gtTime = TRtpUtil::GtGetTime();
    // lastSRTimeStamp: use the middle 32 bits of the 64-bit NTP timestamp
    // This results in value where the 16-MSB is the amount of seconds, and
    // the 16-LSB is the fraction of seconds (in tenths of milliseconds).
    // See RFC3550 page 34 for details.
    inParam.TRTCP_RR.lastSRTimeStamp = ( iSyncInfo.iNTPTimeStampSec << 16 ) + ( iSyncInfo.iNTPTimeStampFrac >> 16 );
    inParam.TRTCP_RR.delaySinceLSR = 
        ( static_cast<TUint32>( ( gtTime - iSyncInfo.iLastUpdateLocalTime ) / KTenthOfmsPerSecond ) << 16 );
    inParam.TRTCP_RR.delaySinceLSR += 
    ( ( ( ( TUint32 ) ( ( gtTime - iSyncInfo.iLastUpdateLocalTime ) % KTenthOfmsPerSecond ) ) << 16 ) / KTenthOfmsPerSecond );

    // For receiving stream, it only generates RR packet. 
    aPkt->SetType( ERTCP_RR );
    aPkt->RtpPacketBuild( &streamParam, &inParam );
    }


// ---------------------------------------------------------------------------
// TRtpRtcpEnum CRtpRecvStream::RtpStreamProcessRtcpReportSectionL()
// For receiving stream, it only process SR packet
// ---------------------------------------------------------------------------
//
TRtpRtcpEnum CRtpRecvStream::RtpStreamProcessRtcpReportSectionL( CRtpPacket* aPkt )
    {
    TRtpPacketStreamParam streamParam;
    TRtpPacketIOParam extractParam;
    TRtpRtcpEnum parseResult = ERTCP_NO_ERROR;
    TUint32 gtTime = TRtpUtil::GtGetTime();

    parseResult = aPkt->RtpPacketProcessL( &streamParam, &extractParam );
    if ( parseResult < 0 )
        {
        return ERTCP_PACKET_ERROR; 
        }

    // For receiving stream, it only processes SR packet.
    if ( aPkt->Type() == ERTCP_SR )
        {
        // give this information to receiver on RTCP callback
        iSyncInfo.iLastUpdateLocalTime = gtTime;
        iSyncInfo.iNTPTimeStampSec = extractParam.TRTCP_SR.NTPTimeStampSec;
        iSyncInfo.iNTPTimeStampFrac = extractParam.TRTCP_SR.NTPTimeStampFrac;
        iSyncInfo.iTimeStamp = extractParam.TRTCP_SR.timeStamp;
        
        // do something with numPacketsSent and cumNumOctetsSent 
        iRtcpStats.iRtcpSenderStats.iSSRC = streamParam.TRTCP_SR.SSRC;
        iRtcpStats.iRtcpSenderStats.iCumNumOctetsSent = streamParam.TRTCP_SR.cumNumOctetsSent;
        iRtcpStats.iRtcpSenderStats.iNumPacketsSent = streamParam.TRTCP_SR.numPacketsSent;
        iRtcpStats.iRtcpSenderStats.iNTPTimeStampSec = iSyncInfo.iNTPTimeStampSec;
        iRtcpStats.iRtcpSenderStats.iNTPTimeStampFrac = iSyncInfo.iNTPTimeStampFrac;
        iRtcpStats.iRtcpSenderStats.iTimeStamp = iSyncInfo.iTimeStamp;
        
        
        RTP_DEBUG_STAT( "----- RX: Process RTCP Report Statistics -----" );
        RTP_DEBUG_STAT_DVALUE( "RX: cumNumOctetsSent = ", iRtcpStats.iRtcpSenderStats.iCumNumOctetsSent );
        RTP_DEBUG_STAT_DVALUE( "RX: numPacketsSent = ", iRtcpStats.iRtcpSenderStats.iNumPacketsSent );
        RTP_DEBUG_STAT_DVALUE( "RX: SSRC (Tx) = ", iRtcpStats.iRtcpSenderStats.iSSRC );
        RTP_DEBUG_STAT_DVALUE( "RX: NTPTimeStampSec = ", iRtcpStats.iRtcpSenderStats.iNTPTimeStampSec );
        RTP_DEBUG_STAT_DVALUE( "RX: NTPTimeStampFrac = ", iRtcpStats.iRtcpSenderStats.iNTPTimeStampFrac );
        RTP_DEBUG_STAT_DVALUE( "RX: timeStamp = ", iRtcpStats.iRtcpSenderStats.iTimeStamp );
   
        
        TTimeStamps timeStamps;
        timeStamps.iNTPTimeStampFrac = iRtcpStats.iRtcpSenderStats.iNTPTimeStampFrac;
        timeStamps.iNTPTimeStampSec = iRtcpStats.iRtcpSenderStats.iNTPTimeStampSec;
        timeStamps.iTimeStamp = iRtcpStats.iRtcpSenderStats.iTimeStamp;

        if ( iRtcpObserver )
            {
            iRtcpObserver->SrReceived( iStreamId, iRtcpStats.iRtcpSenderStats.iSSRC, timeStamps );
            }
        }
    else
        {
        // the received report can not be associated with the stream 
        return ERTCP_PACKET_ERROR;
        }

    return parseResult;
    }

// ---------------------------------------------------------------------------
// TInt CRtpRecvStream::GetRemoteStreamInfo()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpRecvStream::GetRemoteStreamInfo( TRtpSdesParams& aSdes )
    {
    iRemoteSDES->GetSDES( aSdes );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TInt CRtpRecvStream::SetReceivedRTPPackets()
// 
// ---------------------------------------------------------------------------
//
void CRtpRecvStream::SetReceivedRTPPackets( TBool aFlag )
    {
    iFlagReceivedRTPPackets = aFlag;
    };


// ---------------------------------------------------------------------------
// TInt CRtpRecvStream::ReceivedRTPPackets()
// 
// ---------------------------------------------------------------------------
//
TBool CRtpRecvStream::ReceivedRTPPackets() const
    {
    return iFlagReceivedRTPPackets;
    };

// ---------------------------------------------------------------------------
// TInt CRtpRecvStream::GetRemoteSSRC()
// 
// ---------------------------------------------------------------------------
//
TRtpSSRC CRtpRecvStream::GetRemoteSSRC()
    {
    return iRemoteSSRC;
    }


