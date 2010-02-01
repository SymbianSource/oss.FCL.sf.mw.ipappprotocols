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
// Header for class specific to RTCP packet handling
// 
//

/**
 @file
 @internalComponent
*/
 
#if !defined(RTCP_PACKET_H__)
#define RTCP_PACKET_H__

#include <e32base.h>
#include "rtputils.h"

const TUint KRtcpDefaultAverageSize = 50; 

const TInt KRtcpFixedHdrSize = 4;

const TInt KRtcpRRBlockSize = 24; // 6x 32bits words -> 24 bytes
const TInt KRtcpRRHeaderSize = 8; // 2x 32bits words -> 8 bytes

const TInt KRtcpSRHeaderSize = 8; // 2x 32bits words -> 8 bytes
const TInt KRtcpSRSenderInfoSize = 20; //5x 32bits -> 20 octets
/** Length of a BYE packet for just one source
	without a reason text. */
const TInt KRtcpByeFixedSize = 8;

const TInt KRtcpSDesFixedSize = 4;
const TInt KRtcpSDesItemFixedSize = 2;

const TInt KRtcpAppFixedSize = 8;

const TInt KRtpSsrcSize = 4;

/** Minimum average time between RTCP packets from this site (in seconds).
This time prevents the reports from `clumping' when sessions are small 
and the law of large numbers isn't helping to smooth out the traffic.
It also keeps the report interval from becoming ridiculously small during transient outages 
like a network partition. */
const TUint32 KRtcpMinTimeMicroSeconds32 = 5*1000000;


/** The different types of RTCP packet types
 */
enum TRtcpPacketType
	{
	ERtcpSR = 200,	//sender report
	ERtcpRR = 201,	//receiver report
	ERtcpSDES = 202,//source description
	ERtcpBYE = 203,	//goodbye
	ERtcpAPP = 204,	//application-defined
	ERtcpParam = 205,
	};


enum TSDESType
	{
	/**
	No Extensions SDES Item 
	*/
	EsdesNONE  = 0,

	/**
	Canonical End-Point Identifier SDES Item.
	*/
	EsdesCNAME = 1, 


	/**
	User Name SDES Item.
	*/
	EsdesNAME  = 2, 
    
    
    /**
    Electronic Mail Address SDES Item.
	*/
    EsdesEMAIL = 3, 


	/**
	Phone Number SDES Item.
	*/
	EsdesPHONE = 4, 


	/**
	Geographic User Location SDES Item.
	*/
	EsdesLOC   = 5,


	/**
	Application or Tool Name SDES Item 
	*/
	EsdesTOOL  = 6,


	/**
	Notice/Status SDES Item 
	*/
	EsdesNOTE  = 7,


	/**
	Private Extensions SDES Item 
	*/
	EsdesPRIV  = 8
	};

/* Reasons for RTCP BYE packets (must end on 32-bit boundary)
	we could do zero-padding here but seems easier to extend with spaces 
	From RFC "If the string fills the packet
   to the next 32-bit boundary, the string is not null terminated.  If
   not, the BYE packet MUST be padded with null octets to the next 32-
   bit boundary. "
*/
_LIT8(KRtcpByeReasonLoop,"RTP loop detected   ");
_LIT8(KRtcpByeReasonCollision,"RTP collision detected  ");



//
// RTCP packet definitions used in User dll
//


const TInt KRtcpSDesItemMaxLen = 255;

/*
 * Reception report block
 */
class TRtcpRRItem  
	{
public:
	TUint32 ssrc;             /* data source being reported */
	TUint fraction;			  /* fraction lost since last SR/RR */
	
	TInt lost;				  /* cumul. no. pkts lost (signed!) */
	TUint32 last_seq;         /* extended last seq. no. received */
	TUint32 jitter;           /* interarrival jitter */
	TUint32 lsr;              /* last SR packet from this source */
	TUint32 dlsr;             /* delay since last SR packet */
	};

/* reception report (RR) */
class TRtcpRRPart
	{
public:
	TUint32 ssrc;					/* receiver generating this report */
	RArray<TRtcpRRItem> iRRItems;	/* variable-length list */
	void Reset() { iRRItems.Reset(); }
	};

/* sender report (SR) */
class TRtcpSRPart
	{
public:
	TUint32 ssrc;     /* sender generating this report */
	TUint32 ntp_sec;  /* NTP timestamp */
	TUint32 ntp_frac;
	TUint32 rtp_ts;   /* RTP timestamp */
	TUint32 psent;    /* packets sent */
	TUint32 osent;    /* octets sent */
	};
		
/*
 * SDES item
 */
class TRtcpSdesItem
	{
public:
    TUint8 iType;              /* type of item (rtcp_sdes_type_t) */
    TBuf8<KRtcpSDesItemMaxLen> iData;             /* text, not null-terminated */
	};

/* sender report (SR) */
typedef RArray<TRtcpSdesItem> TRtcpSdesPart;	/* variable-length list */

class TRtcpAppPart
	{
public:
	TUint iSubType;
	TBuf8<4> iName;
	HBufC8* iAppData;
	void Reset() { delete iAppData; iAppData = NULL; }
	};

//
// End of definitions specific to user dll
//

class CRtpSourceEntry;

class TRtcpMemLayout
	{
public:
	static inline TUint16 Swap16(TUint16 aVal);
	static inline TUint32 Swap32(TUint32 aVal);
	TUint8* iFirstOctet;
	TInt iLength;
	};

class TRtcpFixedHdrMemLayout : public TRtcpMemLayout
	/* 
	RTCP fixed header
			0                   1                   2                   3
			0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	header |V=2|P|    aa   |  PT           |             length            |
		   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	
		aa == SC/RC/subtype
		RR/SR - reception report count (RC): 5 bits
		The number of reception report blocks contained in this packet.  A
		value of zero is valid

		SDES/BYE	source count (SC): 5 bits
		The number of SSRC/CSRC chunks contained in this SDES packet.  A
		value of zero is valid but useless.

		APP	subtype: 5 bits
		May be used as a subtype to allow a set of APP packets to be
		defined under one unique name, or for any application-dependent data.	
	*/
	{
public:
	inline TRtcpFixedHdrMemLayout(TDesC8& packet, TInt aOffset = 0);
	inline void SetVersion(TUint aVersion);
	inline void SetVersion2();
	inline void SetPadding(TBool aPadding);
	inline void SetCount(TUint8 aCount);
	inline void SetPacketType(TRtcpPacketType aPacketType);
	inline void SetLength(TUint16 aLength);
	// only for RR/SR
	inline void SetSSRC(TUint32 aSSRC);

	inline TUint Version();
	inline TBool IsPadding();
	inline TUint Count();
	inline TUint PacketType();
	inline TUint Length();
	// only for RR/SR
	inline TUint SSRC();
private:
	TRtcpFixedHdrMemLayout();
	};

class TRtcpReportBlockMemLayout : public TRtcpMemLayout
/*
        0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_1 (SSRC of first source)                 |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  1    | fraction lost |       cumulative number of packets lost       |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |           extended highest sequence number received           |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                      interarrival jitter                      |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         last SR (LSR)                         |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                   delay since last SR (DLSR)                  |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+

*/
	{
public:
	inline TRtcpReportBlockMemLayout(TDesC8& aPacket,TInt aOffset = KRtcpRRHeaderSize);
	inline void SetSSRC(TUint32 aSSRC);
	inline void SetFraction(TUint8 aFraction);
	inline void SetCumulativeLost(TUint aLost);
	inline void SetSequenceHigh(TUint16 aSeqNo,TUint16 aCycles);
	inline void SetJitter(TUint32 aJitter);
	inline void SetLastSR(TUint32 aLastSR);
	inline void SetLastDelay(TUint32 aLastDelay);

	inline TUint32 SSRC();
	inline TUint Fraction();
	inline TUint CumulativeLost();
	inline TUint SequenceHigh();
	inline TUint Jitter();
	inline TUint LastSR();
	inline TUint LastDelay();
private:
	TRtcpReportBlockMemLayout();
	};

class TRtcpSRSenderInfoMemLayout : public TRtcpMemLayout
/*
        0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
sender |              NTP timestamp, most significant word             |
info   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |             NTP timestamp, least significant word             |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         RTP timestamp                         |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                     sender's packet count                     |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                      sender's octet count                     |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*/
	{
public:
	inline TRtcpSRSenderInfoMemLayout(TDesC8& aPacket,TInt aOffset = KRtcpSRHeaderSize);
	inline void SetNtpMsb(TUint32 aNtpMsb);
	inline void SetNtpLsb(TUint32 aNtpLsb);
	inline void SetRtpTimeStamp(TUint32 aRtpTs);
	inline void SetPacketCount(TUint32 aPacketCount);
	inline void SetOctetCount(TUint32 aOctetCount);

	inline TUint32 NtpMsb();
	inline TUint32 NtpLsb();
	inline TUint32 RtpTimeStamp();
	inline TUint32 PacketCount();
	inline TUint32 OctetCount();
private:
	TRtcpSRSenderInfoMemLayout();
	};

class TRtcpSdesItemMemLayout : public TRtcpMemLayout
/*
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |    Attrib     |     length    | user and domain name        ...
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
	{
public:
	inline TRtcpSdesItemMemLayout(TDesC8& aPacket, TInt aOffset = 0);
	inline void SetAttribute(TUint8 aAttrib, TDesC8& aValue);

	inline TUint8 GetLength();
	inline TPtrC8 GetAttribute();
	inline TUint8 GetType();
private:
	TRtcpSdesItemMemLayout();
	};

//>>>>>>>>>>>>>>
// Swaps the bytes of a 16 bit number (i.e. convert from big-endian to
// little endian or vica versa
TUint16 TRtcpMemLayout::Swap16(TUint16 aVal)
	{
	return static_cast<TUint16>(aVal >> 8 | (aVal & 0xff) << 8);
	}

// Swaps the bytes of a 32 bit number (i.e. convert from big-endian to
// little endian or vica versa
TUint32 TRtcpMemLayout::Swap32(TUint32 aVal)
	{
	return (Swap16(static_cast<TUint16>(aVal >> 16)) | 
			Swap16(static_cast<TUint16>(aVal & 0xffff)) << 16);
	}
//>>>>>>>>>>>>>>>>>>

/**
RTCP header

        0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
header |V=2|P|    RC   |   PT=RR=201   |             length            |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
RR only|                     SSRC of packet sender                     |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*/
inline TRtcpFixedHdrMemLayout::TRtcpFixedHdrMemLayout(TDesC8& aPacket, TInt aOffset)
	{
	iFirstOctet = const_cast<unsigned char*>(aPacket.Ptr()) + aOffset;
	iLength = aPacket.Length();
	}

inline void TRtcpFixedHdrMemLayout::SetVersion(TUint aVersion)
	{
	*iFirstOctet &= 0x3f;
	*iFirstOctet |= ((aVersion<<6)&0xc0);
	}

inline void TRtcpFixedHdrMemLayout::SetVersion2()
	{
	*iFirstOctet &= 0x3f;
	*iFirstOctet |= 0x80;
	}

inline void TRtcpFixedHdrMemLayout::SetPadding(TBool aPadding)
	{
	*iFirstOctet = static_cast<TUint8>(aPadding ? (*iFirstOctet)|0x20 : (*iFirstOctet) 
										& 0xdf);
	}

inline void TRtcpFixedHdrMemLayout::SetCount(TUint8 aCount)
	{
	*iFirstOctet &= 0xe0;
	*iFirstOctet |= (aCount&0x1f);
	}

inline void TRtcpFixedHdrMemLayout::SetPacketType(TRtcpPacketType aPacketType)
	{
	*(iFirstOctet+1) = static_cast<TUint8>(aPacketType);
	}

inline void TRtcpFixedHdrMemLayout::SetLength(TUint16 aLength)
	{
	*(reinterpret_cast<TUint16*>(iFirstOctet+2)) = Swap16(aLength);
	}

inline void TRtcpFixedHdrMemLayout::SetSSRC(TUint32 aSSRC)
	{
	// we need at least 8 octets if we want to write SSRC!
	*(reinterpret_cast<TUint32*>(iFirstOctet+4)) = Swap32(aSSRC);
	}

inline TUint TRtcpFixedHdrMemLayout::Version()
	{
	return (*iFirstOctet>>6)&0x03;
	}

inline TBool TRtcpFixedHdrMemLayout::IsPadding()
	{
	return (*iFirstOctet)&0x20;
	}

inline TUint TRtcpFixedHdrMemLayout::Count()
	{
	return (*iFirstOctet)&0x1f;
	}

inline TUint TRtcpFixedHdrMemLayout::PacketType()
	{
	return *(iFirstOctet+1);
	}

inline TUint TRtcpFixedHdrMemLayout::Length()
	{
	return Swap16(*(reinterpret_cast<TUint16*>(iFirstOctet+2)));
	}
// only for RR/SR
inline TUint TRtcpFixedHdrMemLayout::SSRC()
	{
	return Swap32(*(reinterpret_cast<TUint32*>(iFirstOctet+4)));
	}
//>>>>>>>>>>>>>>>>>>
/**
@internalTechnology
@param aPacket
*/
inline TRtcpSRSenderInfoMemLayout::TRtcpSRSenderInfoMemLayout(TDesC8& aPacket ,TInt aOffset)
	{
	iFirstOctet = const_cast<unsigned char*>(aPacket.Ptr()+aOffset);
	iLength = aPacket.Length();
	}

inline void TRtcpSRSenderInfoMemLayout::SetNtpMsb(TUint32 aNtpMsb)
	{
	*(reinterpret_cast<TUint32*>(iFirstOctet)) = Swap32(aNtpMsb);
	}

inline void TRtcpSRSenderInfoMemLayout::SetNtpLsb(TUint32 aNtpLsb)
	{
	*(reinterpret_cast<TUint32*>(iFirstOctet+4)) = Swap32(aNtpLsb);
	}

inline void TRtcpSRSenderInfoMemLayout::SetRtpTimeStamp(TUint32 aRtpTs)
	{
	*(reinterpret_cast<TUint32*>(iFirstOctet+8)) = Swap32(aRtpTs);
	}

inline void TRtcpSRSenderInfoMemLayout::SetPacketCount(TUint32 aPacketCount)
	{
	*(reinterpret_cast<TUint32*>(iFirstOctet+12)) = Swap32(aPacketCount);
	}

inline void TRtcpSRSenderInfoMemLayout::SetOctetCount(TUint32 aOctetCount)
	{
	*(reinterpret_cast<TUint32*>(iFirstOctet+16)) = Swap32(aOctetCount);
	}

inline TUint32 TRtcpSRSenderInfoMemLayout::NtpMsb()
	{
	return Swap32(*(reinterpret_cast<TUint32*>(iFirstOctet)));
	}

inline TUint32 TRtcpSRSenderInfoMemLayout::NtpLsb()
	{
	return Swap32(*(reinterpret_cast<TUint32*>(iFirstOctet+4)));
	}

inline TUint32 TRtcpSRSenderInfoMemLayout::RtpTimeStamp()
	{
	return Swap32(*(reinterpret_cast<TUint32*>(iFirstOctet+8)));
	}

inline TUint32 TRtcpSRSenderInfoMemLayout::PacketCount()
	{
	return Swap32(*(reinterpret_cast<TUint32*>(iFirstOctet+12)));
	}

inline TUint32 TRtcpSRSenderInfoMemLayout::OctetCount()
	{
	return Swap32(*(reinterpret_cast<TUint32*>(iFirstOctet+16)));
	}
//>>>>>>>>>>>>>>>>>>
inline TRtcpReportBlockMemLayout::TRtcpReportBlockMemLayout(TDesC8& aPacket, TInt aOffset)
	{
	iFirstOctet = const_cast<unsigned char*>(aPacket.Ptr()+aOffset);
	iLength = aPacket.Length();
	}

inline void TRtcpReportBlockMemLayout::SetSSRC(TUint32 aSSRC)
	{
	*(reinterpret_cast<TUint32*>(iFirstOctet)) = Swap32(aSSRC);
	}

inline void TRtcpReportBlockMemLayout::SetFraction(TUint8 aFraction)
	{
	*(iFirstOctet+4) = aFraction;
	}

inline void TRtcpReportBlockMemLayout::SetCumulativeLost(TUint aLost)
	{
	// second word
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//| fraction lost |       cumulative number of packets lost       |
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*(iFirstOctet+5) = static_cast<TUint8>(aLost>>16);
	*(iFirstOctet+6) = static_cast<TUint8>(aLost>>8);
	*(iFirstOctet+7) = static_cast<TUint8>(aLost);
	}

inline void TRtcpReportBlockMemLayout::SetSequenceHigh(TUint16 aSeqNo,TUint16 aCycles)
	{
	*(reinterpret_cast<TUint16*>(iFirstOctet+8)) = Swap16(aCycles);
	*(reinterpret_cast<TUint16*>(iFirstOctet+10)) = Swap16(aSeqNo);
	}

inline void TRtcpReportBlockMemLayout::SetJitter(TUint32 aJitter)
	{
	*(reinterpret_cast<TUint32*>(iFirstOctet+12)) = Swap32(aJitter);
	}

inline void TRtcpReportBlockMemLayout::SetLastSR(TUint32 aLastSR)
	{
	*(reinterpret_cast<TUint32*>(iFirstOctet+16)) = Swap32(aLastSR);
	}

inline void TRtcpReportBlockMemLayout::SetLastDelay(TUint32 aLastDelay)
	{
	*(reinterpret_cast<TUint32*>(iFirstOctet+20)) = Swap32(aLastDelay);
	}

inline TUint32 TRtcpReportBlockMemLayout::SSRC()
	{
	return Swap32(*(reinterpret_cast<TUint32*>(iFirstOctet)));
	}

inline TUint TRtcpReportBlockMemLayout::Fraction()
	{
	return *(iFirstOctet+4);
	}

inline TUint TRtcpReportBlockMemLayout::CumulativeLost()
	{
	return (*(iFirstOctet+7))+(*(iFirstOctet+6)<<8)+(*(iFirstOctet+5)<<16);
	}

inline TUint TRtcpReportBlockMemLayout::SequenceHigh()
	{
	return Swap32(*(reinterpret_cast<TUint32*>(iFirstOctet+8)));
	}
inline TUint TRtcpReportBlockMemLayout::Jitter()
	{
	return Swap32(*(reinterpret_cast<TUint32*>(iFirstOctet+12)));
	}

inline TUint TRtcpReportBlockMemLayout::LastSR()
	{
	return Swap32(*(reinterpret_cast<TUint32*>(iFirstOctet+16)));
	}

inline TUint TRtcpReportBlockMemLayout::LastDelay()
	{
	return Swap32(*(reinterpret_cast<TUint32*>(iFirstOctet+20)));
	}

//>>>>>>>>>>>>>>>>>>
TRtcpSdesItemMemLayout::TRtcpSdesItemMemLayout(TDesC8& aPacket, TInt aOffset)
	{
	iFirstOctet = const_cast<TUint8*>(aPacket.Ptr()) + aOffset;
	iLength = aPacket.Length();
	}

void TRtcpSdesItemMemLayout::SetAttribute(TUint8 aAttrib, TDesC8& aValue)
	{
	*iFirstOctet = aAttrib;
	*(iFirstOctet+1) = static_cast<TUint8>(aValue.Length());
	Mem::Copy(iFirstOctet+2, aValue.Ptr(), aValue.Length());
	}

TUint8 TRtcpSdesItemMemLayout::GetLength()
	{
	return *(iFirstOctet+1);
	}

TPtrC8 TRtcpSdesItemMemLayout::GetAttribute()
	{
	// Assert that the descriptor is of appropriate size
	const TUint8 len = *(iFirstOctet+1);
	return TPtrC8(iFirstOctet+2, len);
	}

TUint8 TRtcpSdesItemMemLayout::GetType()
	{
	return *iFirstOctet;
	}

#endif

