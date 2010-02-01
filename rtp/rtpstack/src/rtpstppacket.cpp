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




// INCLUDES

#include <e32base.h>
#include <e32std.h>

#include "rtpstppacket.h"

//
// RTPLite header
//
// ****************************************
// Extract from "PoC Core Network RTP RP01" 
// ****************************************
//                       1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |F M R C S|  PT | Sequence nbr  |      Checksum (optional)      |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Full header indication (F)  1 bit
//                              If this bit is set to zero then header format
//                              is reduced (otherwise full)
//
//  Marker (M)                  1 bit
//                              Indication of the start of speech item (value 1).
//                              It is set on the Nth first RTP packets including 
//                              speech frames within the RTP payload. N is expected
//                              to have a constant value in the order of 3 to 5. 
//                              Note: M-bit is zero for all embedded control packets.
//
//  Reserved bit (R)            1 bit
//                              This bit is reserved for future use. Default value is 1. 
//
//  Checksum indication (C)     1 bit
//                              C-bit is an indication if header includes at the end an
//                              optional checksum field. Value 1 indicates that checksum
//                              field exist on the header.
//
//  Speech Item toggle indication (S)   1 bit
//                              S-bit is used as an indication (toggled value) that speech
//                              item has been changed (toggled between two speech items 
//                              from same source; toggled between two speech items from 
//                              different users; toggled between two speech items from 
//                              different groups 1-to-1 call). This bit is on the same
//                              position for the respective leader, speech and trailing
//                              packets.
//
//  Payload Type (PT)           3 bits
//                              PT-field indicates what kind of information
//                              is carried on the paylaod area. Only two codes
//                              (000 and 001) are reserved at the first phase, 
//                              other values are reserved for future use.000 is
//                              used to indicate that embedded control signal
//                              information is in payload area and 001 indicates
//                              that payload contains speech packets coded with
//                              IETF AMR speech coding 8. In case of embedded 
//                              control signal coding then the first byte on the payload
//                              area has special coding rules, which are defined on section 5.3.4.
//                              Note: GSM HR (HalfRate) will be used for 1st integration testing 
//                              and related PT is 010.
//                              Note: Standard RTP header uses 7 bits for the same indication.
//
// Sequence Nbr (SN)            8 bits
//                              The sequence nbr increments by one for each
//                              RTP data packet sent, and may be used by the
//                              receiver to detect packet loss and restore
//                              packet sequence. The initial value (when S 
//                              bit is toggled) of the sequence number is zero. 
//                              Note one exception: during talkspurt 
//                              (for 1-to-1 call or group call) if one/few 
//                              RTP packets are not sent (on N*20ms interval)
//                              due to DTX usage (silence periods not sent) 
//                              the sequency number at the originator side 
//                              is anyhow incremented by one with the N*20ms 
//                              frequency. The value for N is 3 by default. 
//                              Very probably RTP packets are always sent in
//                              GPRS terminals during talkspurt to avoid TBF
//                              drop. RTP packet payload contains during 
//                              silence periods three NO_DATA -indication
//                              bytes to AMR decoder. In the future extented
//                              TFB timer feature may change this behaviour, 
//                              i.e. the sending of NO_DATA -indication bytes
//                              might be dropped (over GPRS).Within standard
//                              version of RTP header the length of SN is 16
//                              bits , but within PoC application it is enough
//                              to have 8 bits; it provides 7.62s (127*3*20ms)
//                              jitter buffering without any special number wrap
//                              control tricks (shown values are valid with N=3). 
//                              Note: On the uplink direction on each speech item,
//                              the Sequency Nbr is zero for all leading packets
//                              (the first leading packet is replicated few times,
//                              but the replicated messages are identical 
//                              (even concerning the SN field)) and the first RTP
//                              packet containing speech has SN value one.
//                              Note: for all embedded control signals the SN
//                              is set to zero.
//
//  CheckSum (CS)               16 bits (optional)
//                              Checksum field is optional and the existence
//                              of this field is indicated on the first octet
//                              by C-bit.  Checksum will be needed with RP01
//                              interface if RLC/LLC/UDP checksums are not
//                              available/used. UDP checksum is optional 
//                              with IPv4. Further more by using RTP level
//                              CRC in combination of UDP-lite it would be 
//                              possible to implement more efficient unequal
//                              error protection on AMR bits than UDP-lite 
//                              alone would offer.If checksum error is detected
//                              then whole RTP packet is discarded.
//                              Note: Checksum information is not available 
//                              within standard RTP header.
//                              Note: Taking the other aspects (header compression
//                              with Degermark) into account it currently means 
//                              that if checksum is included on RTPLite header
//                              the whole packet does fit to 8 kbit/s channel
//                              on the AIR interface only with AMR 4.75 coding.

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
// 
CRtpStpPacket::CRtpStpPacket() :
    iSize( 0 ),
    iData( 0 ),
    iDataPtr( 0 ),
    iSessionId( 0 )
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CRtpStpPacket::ConstructL( TUint aPacketSize, TRtpId aSessionId )
    {
    iBuf = HBufC8::NewL( aPacketSize );
    iData = ( TUint8* ) iBuf->Des().Ptr();
    iSize = 0;
    iDataPtr = iData;
    iSessionId = aSessionId;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
// 
CRtpStpPacket::~CRtpStpPacket()
    {
    delete iBuf;
    }

// ---------------------------------------------------------------------------
// CRtpStpPacket::Close()
// works as destructor
// ---------------------------------------------------------------------------
//
void CRtpStpPacket::Close()
    {
    delete this;
    }

// ---------------------------------------------------------------------------
// CRtpStpPacket::RtpPacketReset()
// 
// ---------------------------------------------------------------------------
//
void CRtpStpPacket::RtpPacketReset()
    {
    iSize = 0;
    iDataPtr = iData;
    }

// ---------------------------------------------------------------------------
// CRtpStpPacket::RtpPacketResetPtr()
// 
// ---------------------------------------------------------------------------
//
void CRtpStpPacket::RtpPacketResetPtr()
    {
    iDataPtr = iData;
    }

// ---------------------------------------------------------------------------
// TInt CRtpStpPacket::RtpPacketBuild()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpStpPacket::RtpPacketBuild( const TRtpSendHeader& aHeader, const TDesC8& aPayloadData )
    {
    TUint8* dataP;

    dataP = iData;

    // build the packet content
    Mem::FillZ( dataP, 4 );

    // Full header indication (F)
    dataP[0] |= ( 0 << 7 );

    // Marker (M) 
    dataP[0] |= static_cast<TUint8>( aHeader.iMarker << 6 );

    // Padding bit (P) = 1
    dataP[0] |= static_cast<TUint8>( aHeader.iPadding << 5 );

    // Checksum indecation (C)  if checkSumInd == 1, need checksum in the header
    dataP[0] |= static_cast<TUint8>( aHeader.iChecksumInd << 4 );

    // Speech Item toggle indication (S)
    dataP[0] |= static_cast<TUint8>( aHeader.iSpeechItemToggle << 3 );

    // payload type
    dataP[0] |= ( aHeader.iPayloadType & 0x07 );

    // sequence number
    dataP[1] = aHeader.iSeqNum;
    dataP += 2;

    // Optinal checksum
    if ( aHeader.iChecksumInd == 1 )
        {
        write16( dataP, aHeader.iChecksum );
        dataP += 2;
        }

    // copy payload data 
    Mem::Copy( dataP, aPayloadData.Ptr(), aPayloadData.Length() );
    iSize = aPayloadData.Length() + ( dataP - iData );

    return 0;
    }

// ---------------------------------------------------------------------------
// CRtpStpPacket::RtpPacketProcess()
// Return Value:
//    KErrNone if STP packet has been processed OK, or
//    KErrCorrupt if packet is invalid or some other error has occured
// ---------------------------------------------------------------------------
//
TInt CRtpStpPacket::RtpPacketProcess( TUint8* aBuf, TInt* aLength )
    {
    TRtpRecvHeader header;
    TUint8* dataP;

    dataP = iData;

    // Full header indication (F) 
    if ( ( dataP[0] & 0x80 ) != 0 )
        return KErrCorrupt;

    // Marker (M)
    header.iMarker = ( TUint8 ) ( ( dataP[0] & 0x40 ) >> 6 );

    // Padding bit (P)
    header.iPadding = ( TUint8 ) ( ( dataP[0] & 0x20 ) >> 5 );

    // Checksum indication (C) 
    header.iChecksumInd = ( TUint8 ) ( ( dataP[0] & 0x10 ) >> 4 );

    // Speech item toggle indication (S) 
    header.iSpeechItemToggle = ( TUint8 ) ( ( dataP[0] & 0x08 ) >> 3 );

    // Payload type (PT)
    header.iPayloadType = ( TUint8 ) ( ( dataP[0] & 0x07 ) );

    // Sequence number
    header.iSeqNum = ( TUint8 ) ( dataP[1] );
    dataP += 2;

    // Optinal checksum
    if ( header.iChecksumInd == 1 )
        {
        header.iChecksum = ( TUint16 ) read16( dataP ); 
        dataP += 2;
        }

    *aLength = iSize - ( dataP - iData );
    aBuf = ( TUint8 * ) dataP;

    TPtrC8 buf( aBuf, *aLength );

    if ( iObserver )
        iObserver->RtpPacketReceived( iSessionId /* streamId */, header, buf );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Write a 16-bit value as 2 consecutive bytes in MSB order
// Memory (at least 2 bytes) must have been allocated to pointer
// before the function is called.
// ---------------------------------------------------------------------------
//
void CRtpStpPacket::write16( TUint8* const aPointer, TUint32 aValue )
    {
    // check value range (16 bits)
    aPointer[0] = ( TUint8 ) ( ( aValue & 0xFF00 ) >> 8 );
    aPointer[1] = ( TUint8 ) ( aValue & 0x00FF );
    }

// ---------------------------------------------------------------------------
// Read a 16-bit value given as 2 consecutive bytes in MSB order
// Memory (at least 2 bytes) must have been allocated to pointer
// before the function is called.
// ---------------------------------------------------------------------------
//
TUint32 CRtpStpPacket::read16( const TUint8* const aPointer )
    {
    return( aPointer[1] + ( ( TUint32 ) aPointer[0] << 8 ) );
    }


#if !defined ( EKA2 ) && !defined ( RTP_UNIT_TEST )
// The E32Dll() entry point function
GLDEF_C TInt E32Dll( TDllReason /*aReason*/ )
    {
    return( KErrNone );
    }
#endif // EKA2

// ---------------------------------------------------------------------------
// Function to construct a CRtpPacketExt object. Note that this function
// is exported at ordinal 1 and is not a member of any class.
// ---------------------------------------------------------------------------
//
EXPORT_C MRtpPacketExt* NewL()
    {
    return new ( ELeave ) CRtpStpPacket;
    }


// End of File
