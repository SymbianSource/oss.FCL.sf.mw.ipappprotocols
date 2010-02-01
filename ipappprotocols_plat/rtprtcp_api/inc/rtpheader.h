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
* Description:    Standard fixed headers of RTP packets to send and receive.
*
*/




#ifndef __RTPHEADER_H
#define __RTPHEADER_H

//  INCLUDES
#include <rtpdef.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

class  TRtpHeaderExtension
    {
    public:
        TRtpHeaderExtension();
        TInt iType;      // Defined by profile
        TInt iLength;    // Length of extension data
        TUint8* iData;   // Extended header data
    } ;
inline TRtpHeaderExtension::TRtpHeaderExtension() 
    : iType( 0 ),
      iLength( 0 ),
      iData( 0 )
    {
    };

/**
*  Standard fixed header of RTP packet to send.
*
*  @lib RtpService.dll
*/
class TRtpSendHeader
    {
    public: // Header fields
        TRtpSendHeader();

        TUint8 iPadding;                 // Padding (P) (1 bit)
        TUint8 iMarker;                  // Marker (M) (1 bit)
        TRtpPayloadType iPayloadType;    // Payload type (PT) (7 bits)
        TRtpTimeStamp iTimestamp;        // Timestamp (32 bits)
        TRtpHeaderExtension* iHeaderExtension;  // Pointer to header extension struct
    };

inline TRtpSendHeader::TRtpSendHeader() : iPadding( 0 ),
                                          iMarker( 0 ),
                                          iPayloadType( 0 ),
                                          iTimestamp( 0 ),
                                          iHeaderExtension( NULL )
    {
    };

/**
*  Standard fixed header of received RTP packet.
*
*  @lib RtpService.dll
*/
class TRtpRecvHeader
    {
    public: // Header fields
        TRtpRecvHeader();

        TUint8 iPadding;                 // Padding (P) (1 bit)
        TUint8 iExtension;               // Extension (X) (1 bit)
        TUint8 iCsrcCount;               // CSRC count (CC) (4 bits)
        TUint8 iMarker;                  // Marker (M) (1 bit)
        TRtpPayloadType iPayloadType;    // Payload type (PT) (7 bits)
        TRtpSequence iSeqNum;            // Sequence number (16 bits)
        TRtpTimeStamp iTimestamp;        // Timestamp (32 bits)
        TRtpHeaderExtension* iHeaderExtension; // Pointer to header extension struct
        TUint32* iCsrcList;      // Pointer to CSRC list
    };

inline TRtpRecvHeader::TRtpRecvHeader() : iPadding( 0 ), 
                                          iExtension( 0 ), 
                                          iCsrcCount( 0 ),
                                          iMarker( 0 ),
                                          iPayloadType( 0 ),
                                          iSeqNum( 0 ),
                                          iTimestamp( 0 ),
                                          iHeaderExtension( NULL ),
                                          iCsrcList( NULL )
    {
    };

#endif      // __RTPHEADER_H

// End of File
