// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Header for class handling RTP packets
// 
//

/**
 @file
 @internalComponent
*/


#ifndef PACKET_H
#define PACKET_H

#include "rtppacket.h"

// WINS: 'this' : used in base member initializer list
// TO cope with the '&this[1]' expressions
//#pragma warning(disable : 4355) 

// Size of the fixed part of a RTP header in bytes
const TInt KRtpHeaderSize = 12;
// Size in 32 bit words 
const TInt KRtpHeaderSizeInWords = KRtpHeaderSize / sizeof(TUint32);
// Size (in octets) of the 'header' of RTP header extension
const TInt KRtpHeaderExtensionSize = 4;
// Maximum number of CSRCs allowed in send packets
// NOTE: currently NONE!
const TInt KRtpMaxNumberOfCSRC = 0;

/** A RTP header. Note that RTP headers are in big-endian format and
	we're running on a little-endian machine, so the data in this header
	has been converted before copying from the packet.
@internalComponent
*/
struct TRtpHeader 
	{
	TUint   iVersion:2;
	TUint   iPadding:1;
	TUint   iExtension:1;
	TUint   iCsrcCount:4;
	TUint   iMarker:1;
	TUint   iPayloadType:7;
	TUint   iSeq:16;
	TUint32 iTimestamp;
	TUint32 iSSRC;
	// no CSRC representation here
	
	TRtpHeader();
	TRtpHeader(TUint8* aPtr, TInt aLength);
	};

/** A RTP header map. Note that RTP headers are in big-endian format and
	we're running on a little-endian machine, so all the numbers are
	backwards!
	However it'll be fine as long as it's used to get correct offsets only...
@internalComponent
*/
struct TRtpSimpleHeaderMap 
	{
	TUint32 iFlags;
	TUint32 iTimestamp;
	TUint32 iSSRC;
	TUint32 iCSRCs[1];
	};


class TRtpPacket
	{
 public:
	static TUint16 Swap16(TUint16 aVal);
	static TUint32 Swap32(TUint32 aVal);

	inline TRtpPacket(TAny* aPtr, TInt aLength);

	inline const TRtpHeader* Header() const;
	inline TRtpHeader* Header();
	inline TPtr8& Data();
	inline TUint8 operator[](TInt aIndex);
 protected:
	// iHeader increases RAM consumption by 12 bytes per RTP packet!
	// it is RAM against big/little-endian conversion 
	// each time a header field is accessed
	TRtpHeader iHeader;
	TPtr8 iData;
	};

NONSHARABLE_CLASS(TRtpReceivePacket) : public TRtpPacket
	{
 public:
	static TRtpReceivePacket* New(TInt aLength);

	// Returns ETrue if the packet is shorter than its header should be.
	TBool IsTooShort();
	void ReadIntoHeader();
	inline TUint SSRC();
 private:
	inline TRtpReceivePacket(TInt aLength);
	inline TAny* GetPointerToPacketData();
	};

// The data in a send packet actually resides in the same alloc cell
// as the packet, after the packet object.
class TRtpSendPacket : public TRtpPacket
	{
	friend class RRtpSendPacket;
 public:
	static TRtpSendPacket* NewL(CRtpSendSource* aSource, TInt aPayloadLength,
								TInt aExtensionLength);
    
	static TRtpSendPacket* NewL(CRtpSendSource* aSource, TInt aPayloadLength,
                                TInt aExtensionLength, TAny *aBuffer);

	TRtpSendPacket(CRtpSendSource* aSource, TInt aPayloadLength, 
						  TInt aHeaderLength);
	inline TRtpHeader* Header();
	inline TPtrC8 Payload();
 private:
	void WriteHeaderToPacket();
	inline TAny* GetPointerToPacketData();
	CRtpSendSource* iSource;
	TUint iLength;
	TPtr8 iPayload;
	};

//
// Inline definitions
//
inline TRtpPacket::TRtpPacket(TAny* aPtr, TInt aLength)
		: 	iHeader(reinterpret_cast<TUint8*>(aPtr),aLength),
			iData(reinterpret_cast<TUint8*>(aPtr), aLength)
	{
	};

inline TUint TRtpReceivePacket::SSRC()
	{
	// SSRC is most often used thus big/little-endian conversion provided
	const TUint8* iFirstOctet = iData.Ptr();
	return (*(iFirstOctet+KRtpSSRCOffset+3))+
			(*(iFirstOctet+KRtpSSRCOffset+2)<<8)+
			(*(iFirstOctet+KRtpSSRCOffset+1)<<16)+
			(*(iFirstOctet+KRtpSSRCOffset)<<24);
	}

inline TRtpReceivePacket::TRtpReceivePacket(TInt aLength)
		: TRtpPacket(GetPointerToPacketData(), aLength)
	{
	};

inline TRtpHeader* TRtpSendPacket::Header()
	{
	return &iHeader;
	}

inline TPtrC8 TRtpSendPacket::Payload()
	{
	return iPayload;
	}

inline TAny* TRtpSendPacket::GetPointerToPacketData()
	{
	return &this[1];
	}
	
inline TAny* TRtpReceivePacket::GetPointerToPacketData()
	{
	return &this[1];
	}

inline TPtr8& TRtpPacket::Data()
	{
	return iData;
	}
		
inline const TRtpHeader* TRtpPacket::Header() const
	{
	return &iHeader;
	}
	
inline TRtpHeader* TRtpPacket::Header()
	{
	return &iHeader;
	}

inline TUint8 TRtpPacket::operator[](TInt aIndex)
	{
	return iData.Ptr()[aIndex];
	}

#endif // PACKET_H

