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
// RTP Packet Helper functions.
// 
//

/**
 @file
 @internalComponent
*/


/**	Memory layout of of the fixed part of a RTP header 
	0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|X|  CC   |M|     PT      |       sequence number         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                           timestamp                           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |           synchronization source (SSRC) identifier            |
   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
   |            contributing source (CSRC) identifiers             |
   |                             ....                              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
TRtpFixedHdrMemLayout::TRtpFixedHdrMemLayout(TDesC8& aPacket)
	{
	iFirstOctet = const_cast<unsigned char*>(aPacket.Ptr());
	}

inline void TRtpFixedHdrMemLayout::SetVersion(TUint aVersion)
	{
	/*
	0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|V=2|P|X|  CC   |M|     PT      |       sequence number         |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/
	*iFirstOctet &= 0x3f;
	*iFirstOctet |= ((aVersion<<6)&0xc0);
	}

inline void TRtpFixedHdrMemLayout::SetVersion2()
	{
	/*
	0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|V=2|P|X|  CC   |M|     PT      |       sequence number         |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/
	*iFirstOctet &= 0x3f;
	*iFirstOctet |= 0x80;
	}

inline void TRtpFixedHdrMemLayout::SetCSRCCount(TUint aCC)
	{
	/*
	0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|X|  CC   |M|     PT      |       sequence number         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/
	*iFirstOctet &= 0xf0;
	*iFirstOctet |= (aCC&0x0f);
	}

inline void TRtpFixedHdrMemLayout::SetSequenceNumber(TUint16 aNo)
	{
	/*
	0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|X|  CC   |M|     PT      |       sequence number         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/
	*(iFirstOctet+KRtpSequenceNoOffset) = static_cast<TUint8>(aNo>>8);
	*(iFirstOctet+KRtpSequenceNoOffset+1) = static_cast<TUint8>(aNo);
	}

inline void TRtpFixedHdrMemLayout::SetSSRC(TUint32 aSSRC)
	{
	*(iFirstOctet+KRtpSSRCOffset) = static_cast<TUint8>(aSSRC>>24);
	*(iFirstOctet+KRtpSSRCOffset+1) = static_cast<TUint8>(aSSRC>>16);
	*(iFirstOctet+KRtpSSRCOffset+2) = static_cast<TUint8>(aSSRC>>8);
	*(iFirstOctet+KRtpSSRCOffset+3) = static_cast<TUint8>(aSSRC);
	}

inline void TRtpFixedHdrMemLayout::SetTimeStamp(TUint32 aTime)
	{
	*(iFirstOctet+KRtpTimeStampOffset) = static_cast<TUint8>(aTime>>24);
	*(iFirstOctet+KRtpTimeStampOffset+1) = static_cast<TUint8>(aTime>>16);
	*(iFirstOctet+KRtpTimeStampOffset+2) = static_cast<TUint8>(aTime>>8);
	*(iFirstOctet+KRtpTimeStampOffset+3) = static_cast<TUint8>(aTime);
	}

inline void TRtpFixedHdrMemLayout::SetMarker(TBool aMarker)
	{
	*(iFirstOctet+KRtpMarkerOffset) = static_cast<TUint8>(aMarker ? (*(iFirstOctet+1))|0x80 : (*(iFirstOctet+1)) 
										& 0x7f);
	}

inline void TRtpFixedHdrMemLayout::SetPadding(TBool aPadding)
	{
	*iFirstOctet = static_cast<TUint8>(aPadding ? (*iFirstOctet)|0x20 : (*iFirstOctet) 
										& 0xdf);
	}

inline void TRtpFixedHdrMemLayout::SetExtension(TBool aExtension)
	{
	*iFirstOctet = static_cast<TUint8>(aExtension ? (*iFirstOctet)|0x10 : (*iFirstOctet) 
										& 0xef);
	}

inline void TRtpFixedHdrMemLayout::SetPayloadType(TUint aPayloadType)
	{
	//__ASSERT_DEBUG(aPayloadType <= 128, Panic(ERtpPayloadTypeOutOfRange));
	*(iFirstOctet+KRtpPayloadTypeOffset) &= 0x80;
	*(iFirstOctet+KRtpPayloadTypeOffset) |= (aPayloadType&0x7f);
	}

TUint32 TRtpFixedHdrMemLayout::SSRC()
	{
	return (*(iFirstOctet+KRtpSSRCOffset+3))+
			(*(iFirstOctet+KRtpSSRCOffset+2)<<8)+
			(*(iFirstOctet+KRtpSSRCOffset+1)<<16)+
			(*(iFirstOctet+KRtpSSRCOffset)<<24);
	}

inline TUint TRtpFixedHdrMemLayout::CC()
	{
	return (*iFirstOctet)&0x0f;
	}

inline TUint TRtpFixedHdrMemLayout::PayloadType()
	{
	return (*(iFirstOctet+KRtpPayloadTypeOffset))&0x7f;
	}

TBool TRtpFixedHdrMemLayout::HasMarker()
	{
	return (*(iFirstOctet+KRtpMarkerOffset))&0x80;
	}

inline TBool TRtpFixedHdrMemLayout::HasPadding()
	{
	return (*iFirstOctet)&0x20;
	}

TBool TRtpFixedHdrMemLayout::HasExtension()
	{
	return (*iFirstOctet)&0x10;
	}

inline TUint TRtpFixedHdrMemLayout::Version()
	{
	return (*iFirstOctet>>6)&0x03;
	}

inline TUint16 TRtpFixedHdrMemLayout::SequenceNumber()
	{
	return static_cast<TUint16>((*(iFirstOctet+KRtpSequenceNoOffset+1))+
								(*(iFirstOctet+KRtpSequenceNoOffset)<<8));
	}

inline TUint32 TRtpFixedHdrMemLayout::TimeStamp()
	{
	return (*(iFirstOctet+KRtpTimeStampOffset+3))+
			(*(iFirstOctet+KRtpTimeStampOffset+2)<<8)+
			(*(iFirstOctet+KRtpTimeStampOffset+1)<<16)+
			(*(iFirstOctet+KRtpTimeStampOffset)<<24);
	}

inline TUint8* TRtpFixedHdrMemLayout::CSRCs()
	{
	return CC() ? (iFirstOctet+KRtpCSRCOffset) : NULL;
	}

