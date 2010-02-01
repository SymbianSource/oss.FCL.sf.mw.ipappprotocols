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




#ifndef RtpStreamDef_H
#define RtpStreamDef_H



// CONSTANTS
const TUint KSNMaxArray = 200;

// Number of seconds between 1-Jan-1900 and 1-jan-1970
const TUint32 KGetTimeOfDayToNTPOffset = 2208988800U;



// CLASS DECLARATIONS
/**
*  Callback class used by CRtpStream and CRtpSession. 
*
*  @lib RtpService.dll
*/
class MSsrcCheckCallback
    {
    public:
        /**
         * Inform this object that a new SSRC is found within a packet
         */
        virtual TInt CheckRemoteSsrcL( TRtpSSRC& aSSRC ) = 0;
        virtual TBool CheckRemoteAddr() =0;
    };


/**
*  stream time synchronization struct for NTP time calculation
*
*  @lib RtpService.dll
*/
class TRtpTimeSync
    {
    public:
        // for RX stream: they record the NTP time from SR and will be used by creating RR
        // for TX stream: they are generated for creating SR.
        TUint32 iNTPTimeStampSec;  // seconds
        TUint32 iNTPTimeStampFrac; // fraction

        // RTP Time stamp of last packet sent by TX stream
        TRtpTimeStamp iTimeStamp;

        // for RX stream: time when last NTP time stamp (SR report) has been received.
        // for TX stream: time of initialization (for NTP synchronization)
        TUint32 iLastUpdateLocalTime;
    };


/**
*  stream time jitter struct, just be used by receiving stream
*
*  @lib RtpService.dll
*/
class TRtpTimeJitter
    {
    public:
        TUint32 iTimeStampResolution;   

        // The time of arrival in tenth of ms. 
        TUint32 iLastPacketR;

        // The RTP timestamp from last packet
        TRtpTimeStamp iLastPacketS;          

        //
        // keeps jitter time for RR report. 
        // It will be tailored to TUint32 when assigned to RR.JitterTime.
        //
        TReal64 iJitterTime;

        TUint32 iWaitTime;
    };

class TRtcpReceiverStats
    {
    public:
        TRtpSSRC iSSRC;              // SSRC receiver number 
        TUint8 iFractionLost;        // fraction lost 
        TUint32 iCumNumPacketsLost;  // packets lost
        TUint32 iSeqNumReceived;     // sequence number
        TUint32 iArrivalJitter;      // arrival jitter 
        TUint32 iRoundTripDelay;     // round trip delay of RTP packet 
        TUint32 iBandwidth;          // receiver bandwidth (bps) calculated by the transmitter 
        TUint32 iTxBandwidth;        // transmitter bandwidth (bps) calculated at transmitter 
        TUint32 iChannelBufferSize;  // an estimate of the channel buffer size 
    };

class TRtcpSenderStats
    {
    public:
        TRtpSSRC iSSRC;              // SSRC transmitter number 
        TUint32 iNumPacketsSent;     // number of packets sent 
        TUint32 iCumNumOctetsSent;   // number of octets sent 
        TUint32 iNTPTimeStampSec;    // NTP seconds
        TUint32 iNTPTimeStampFrac;   // NTPfraction
        TUint32 iTimeStamp;          // RTP Time stamp of last packet sent by TX stream
    };

class TRtcpStats
    {
    public:
        TRtcpSenderStats iRtcpSenderStats;      // Sender's statistic structure 
        TRtcpReceiverStats iRtcpReceiverStats;  // Receiver's statistic structure 
    };


#endif	// RtpStreamDef_H





