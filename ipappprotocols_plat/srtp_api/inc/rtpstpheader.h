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
* Description:    STP (Streaming Transport Protocol) header of RTP packet
*              : to send/receive.
*
*/




#ifndef __RTPSTPHEADER_H
#define __RTPSTPHEADER_H

//  INCLUDES
#include <rtpdef.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
*  STP (Streaming Transport Protocol) common parts of RTP packet headers.
*
*  @lib RtpStpPacket.dll
*/
class TRtpHeader
    {
    public: // Header fields
        TRtpHeader();
        TUint8 iMarker;              // Marker (M) (1 bit)
        TUint8 iPadding;             // Padding (P) (1 bit)
        TUint8 iChecksumInd;         // Checksum indication (C) (1 bit)
        TUint8 iSpeechItemToggle;    // Speech item toggle indication (S) (1 bit)
        TUint8 iPayloadType;         // Payload type (PT) (3 bits)
        TUint8 iSeqNum;              // Sequence number (8 bits)
        TUint16 iChecksum;           // Checksum (16 bits)
    };

inline TRtpHeader::TRtpHeader() 
    : iMarker( 0 ),
      iPadding( 0 ),
      iChecksumInd( 0 ),
      iSpeechItemToggle( 0 ),
      iPayloadType( 0 ),
      iSeqNum( 0 ),
      iChecksum( 0 )
    {
    };

/**
*  STP (Streaming Transport Protocol) header of RTP packet to send.
*
*  @lib RtpStpPacket.dll
*/
class TRtpSendHeader : public TRtpHeader
    {
    };

/**
*  STP (Streaming Transport Protocol) header of RTP packet to receive.
*
*  @lib RtpStpPacket.dll
*/
class TRtpRecvHeader : public TRtpHeader
    {
    };

#endif      // __RTPSTPHEADER_H

// End of File
