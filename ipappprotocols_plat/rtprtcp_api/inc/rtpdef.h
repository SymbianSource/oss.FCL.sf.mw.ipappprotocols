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
* Description:    Header file for definitions related to RTP
*              : (Real-time Transport Protocol) library
*
*/




#ifndef __RTPDEF_H
#define __RTPDEF_H

//  INCLUDES

#include <e32std.h>
#include <in_sock.h>
#include <es_sock.h>

// CONSTANTS
const TUint KMaxRtcpAppData = 512;
const TInt KMaxRtcpReason = 255; // max of RTCP APP packet in bytes
const TUint KMaxSdesItemSize = 255;
const TUint KSocketBufSize = 4096;
const TInt KMinRtpHeaderSize = 12; // 96 bits in header, 96/8 = 12
const TInt KMaxCsrcIdentifiers = 15;

// DATA TYPES

typedef TUint32 TRtpId;
const TRtpId KNullId = 0xffffffff;

typedef TUint32 TRtpSSRC;       //
typedef TUint32 TRtpCSRC;       //
typedef TUint8 TRtpPayloadType; // payload type of the RTP packet
typedef TUint16 TRtpSequence;   // sequence number of the RTP packet
typedef TUint32 TRtpTimeStamp;  // timestamp of the RTP packet

//
// TRtcpAppName, TRtcpAppSubType, TRtcpApp - RTCP APP packet
// related data types. See RFC1889.txt for detailed description
//
typedef TUint8 TRtcpAppName[4]; // APP packet name
typedef TInt TRtcpAppSubType;   // subtype ID under the same name

// FORWARD DECLARATIONS
class TRtpSendHeader;
class TRtpRecvHeader;
class TRtpSendPktParams;

// CLASS DECLARATION

/**
*  RTCP APP class. 
*
*  @lib RtpService.dll
*/
class TRtcpApp
    {
    public:
        inline TRtcpApp() 
            : iSubType( 0 ), 
              iAppDataLen( 0 )
            {};
        TRtcpAppSubType iSubType;
        TRtcpAppName iName;
        TUint8 iAppData[KMaxRtcpAppData];
        TUint iAppDataLen;
    };

/**
*  Parameters for creating session. 
*
*  @lib RtpService.dll
*/
class TCreateSessionParams
    {
    public:
        inline TCreateSessionParams()
            : iPriority( EPriorityStandard ), iSocketBufSize( KSocketBufSize ) {};

        enum TPriority
            {
            EPriorityIdle                   = -100,
            EPriorityLow                    = -20,
            EPriorityStandard               = 0,
            EPriorityUserInput              = 10,
            EPriorityHigh                   = 20
            };
        TPriority iPriority;
        TInt iSocketBufSize;                // For RTP sending only;
    };

/**
*  RTCP SDES class. 
*
*  @lib RtpService.dll
*/
class TRtpSdesParams
    {
    public:
        inline TRtpSdesParams& operator = ( const TRtpSdesParams& aParams );
        TPtrC8 iCName; 
        TPtrC8 iUserName;
        TPtrC8 iEmail;
        TPtrC8 iPhoneNumber;
        TPtrC8 iLocation;
        TPtrC8 iSwToolName;
        TPtrC8 iNoticeStatus;
        TPtrC8 iPrivate;
    };

TRtpSdesParams& TRtpSdesParams::operator = ( const TRtpSdesParams& aParams )
    {
    iCName.Set( aParams.iCName );
    iUserName.Set( aParams.iUserName );
    iEmail.Set( aParams.iEmail );
    iPhoneNumber.Set( aParams.iPhoneNumber );
    iLocation.Set( aParams.iLocation );
    iSwToolName.Set( aParams.iSwToolName );
    iNoticeStatus.Set( aParams.iNoticeStatus );
    iPrivate.Set( aParams.iPrivate );

    return *this;
    }

/**
*  Parameters for creating receiving stream. 
*
*  @lib RtpService.dll
*/
class TRcvStreamParams
    {
    public:
        TRtpPayloadType iPayloadType;
    };

/**
*  Parameters for creating transmit stream. 
*
*  @lib RtpService.dll
*/
class TTranStreamParams
    {
    public:
        TRtpPayloadType iPayloadType;
    };

/**
*  Statistics for each stream. 
*
*  @lib RtpService.dll
*/
class TRtpPeerStat
    {
    public:
        // sender related stat
        TInt iNumPacketsSent;            // cumulative number of packets sent
        TInt iCumNumOctetsSent;          // cumulative number of octets sent
        TUint iRoundTripDelay;           // Round trip delay
        TUint iTxBandwidth;              // transmitter bandwidth (bps)

        // receiver related stat
        TInt iCumNumPacketsLost;         // packets lost
        TUint8 iFractionLost;            // fraction of packets lost between last two reports
        TInt iArrivalJitter;             // arrival jitter in tenth of millsecond.
        TUint iRxBandwidth;              // receiver bandwidth (bps) calculated by the transmitter
        TUint iChannelBufferSize;        // an estimate of the channel buffer size
        TUint32 iNTPTimeStampSec;        // NTP seconds
        TUint32 iNTPTimeStampFrac;       // NTP fraction
        TRtpTimeStamp iTimeStamp;        // RTP Time stamp of last packet sent by TX stream
    };

/**
*  RTCP parameters for RTCP interval calculation and timeout. 
*
*  @lib RtpService.dll
*/
class TRtcpParams
    {
    public:
        TUint iSessionBWidth; // Session bandwidth in bps
        TReal iRtcpFraction;  // Fraction of session bandwidth to be allocated to RTCP
        TTimeIntervalMicroSeconds32 iRtcpTimeOut; // Time out for RTCP asynchronous sending 
    };

/**
*  Time stamps from RTCP report for A/V sync. 
*
*  @lib RtpService.dll
*/
class TTimeStamps
    {
    public:
        TUint32 iNTPTimeStampSec;         // NTP seconds
        TUint32 iNTPTimeStampFrac;        // NTP fraction of second
        TRtpTimeStamp iTimeStamp;         // RTP Time stamp
    };


/**
*  Header class for sending RTP Packets
*
*  @lib RtpService.dll
*/
class TRtpSendPktParams
    {
public:
    
    TRtpSendPktParams(TRtpSendHeader &aHeaderInfo);

    TRtpId iTranStreamId;           // Transtream Id
    TRtpSendHeader  &iHeaderInfo;   // Standard fixed header of RTP packet to send.
    TPtrC8 iPayloadData;            // Payload
    TRequestStatus *iStatus;
    TRtpSequence *iSequenceNum;     // Sequence Number
    };

inline TRtpSendPktParams::TRtpSendPktParams(TRtpSendHeader &aHeaderInfo) : iTranStreamId( 0 ), 
                                                  iHeaderInfo( aHeaderInfo ),
                                                  iStatus( 0 ),
                                                  iSequenceNum( 0 )
    {
    };

/**
 * An interface to the callback functions for asynchronous event
 * notifications from RTP module.
 *
 * The class is a mixin and is intended to be inherited by the client
 * class which is observing RTP module error. A reference to
 * this object is passed as a parameter when constructing an RTP object.
 */
class MRtpErrNotify
{
public:
    
    /**
    * Error call back 
    * @param aErrCode - System wide error code
    */
    virtual void ErrorNotify( TInt aErrCode ) = 0;
};


/**
*  callback function for received rtp packet. 
*
*  @lib RtpService.dll
*/
class MRtpObserver
    {
    public:
        /**
        * Callback function to receive an RTP packet.
        * @param TRtpId aStreamId - RTP receive-stream ID
        * @param TRtpRecvHeader& aHeaderInfo - Header info of RTP packet received
        * @param TDesC8& aPayloadData - Payload data of RTP packet received
        * @return None
        */
        virtual void RtpPacketReceived( TRtpId aStreamId, 
                                        const TRtpRecvHeader& aHeaderInfo, 
                                        const TDesC8& aPayloadData ) = 0;
    };

/**
*  callback functions for received rtcp packet. 
*
*  @lib RtpService.dll
*/
class MRtcpObserver
    {
    public:
        /**
        * Callback function to receive an RTCP SDES (Source Description) packet.
        * @param TRtpSSRC aSSRC - SSRC value
        * @param TRtpSdesParams& aParams - SDES parameters received
        * @return None
        */
        virtual void SdesReceived( TRtpSSRC aSSRC, const TRtpSdesParams& aParams ) = 0;

        /**
        * Callback function to receive an RTCP BYE packet.
        * @param TRtpId aStreamId - RTP stream ID
        * @param TRtpSSRC aSSRC - SSRC value
        * @param TDesC8& aReason - Reason for leaving
        * @return None
        */
        virtual void ByeReceived( TRtpId aStreamId, TRtpSSRC aSSRC, const TDesC8& aReason ) = 0;
        
        /**
        * Callback function to receive an RTCP APP packet.
        * @param TRtpId aStreamId - RTP stream ID
        * @param TRtpSSRC aSSRC - SSRC value
        * @param TRtcpApp& aApp - APP contain
        * @return None
        */
        virtual void AppReceived( TRtpId aStreamId, TRtpSSRC aSSRC, const TRtcpApp& aApp ) = 0;
        
        /**
        * Callback function to receive an RTCP SR packet.
        * @param TRtpId aStreamId - RTP stream ID
        * @param TRtpSSRC aSSRC - SSRC value
        * @param TTimeStamps& aTimeStamps - RTP and NTP timestamps for A/V synch.
        * @return None
        */
        virtual void SrReceived( TRtpId aStreamId, TRtpSSRC aSSRC, const TTimeStamps& aTimeStamps ) = 0;
        
        /**
        * Callback function to receive an RTCP RR packet.
        * @param TRtpId aStreamId - RTP stream ID or Session ID if empty RR received.
        * @param TRtpSSRC aSSRC - SSRC value
        * @return None
        */
        virtual void RrReceived( TRtpId aStreamId, TRtpSSRC aSSRC ) = 0;
    };

/**
*  callback functions for any received non-RTP (control) packets. 
*
*  @lib RtpService.dll
*/
class MNonRTPDataObserver
    {
    public:
        /**
        * Callback function to receive a non-RTP packet.
        * @param TUint aPort - port ID
        * @param TBool aRTPport - RTP port true, RTCP port false        
        * @param TDesC8& aNonRTPData - non-RTP packet received
        * @return None
        */
        virtual void NonRTPDataReceived( TUint aPort, TBool aRTPport, const TDesC8& aNonRTPData ) = 0;

    };


/**
*  callback functions for Send RTP packets. 
*
*  @lib RtpService.dll
*/
class MRtpPostProcessingObserver
    {
    public:
        /**
        * Callback function to receive a handle to RTP packet which is ready to send.
        * @param TRtpId aTranStreamId - Transimission stream id
        * @param TPtr8 &aPacket -  RTP packet Ready to send
        * @return None
        */
        virtual void ReadyToSendRtpPacket( TRtpId aTranStreamId, TPtr8 &aPacket ) = 0;

    };

#endif // __RTPDEF_H
