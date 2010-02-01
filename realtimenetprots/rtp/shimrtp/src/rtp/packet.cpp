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
// class for handling rtp packets
// 
//

/**
 @file
*/
 
#include <e32base.h>
#include "rtp.h"
#include "rtputils.h"
#include "packet.h"
#include "sendstream.h"

/** Swaps the bytes of a 16 bit number (i.e. convert from big-endian to
	little endian or vica versa
@internalComponent
*/
TUint16 TRtpPacket::Swap16(TUint16 aVal)
	{
	__RTP_LOG(_L("TRtpPacket::Swap16() ... "));
	return static_cast<TUint16>(aVal >> 8 | (aVal & 0xff) << 8);
	}

/** Swaps the bytes of a 32 bit number (i.e. convert from big-endian to
	little endian or vica versa
@internalComponent
*/
TUint32 TRtpPacket::Swap32(TUint32 aVal)
	{	
	__RTP_LOG(_L("TRtpPacket::Swap32() ... "));
	return (Swap16(static_cast<TUint16>(aVal >> 16)) | 
			Swap16(static_cast<TUint16>(aVal & 0xffff)) << 16);

	}




/**
Gets the packet's sequence number.

@return The packet's sequence number.

@panic RTP 1 if the packet has not been opened.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function when the packet has not been opened is
             likely to raise panics such as KERN-EXEC 3.
*/
EXPORT_C TUint RRtpPacket::SequenceNumber() const
	{
	__RTP_LOG(_L("RRtpPacket::SequenceNumber() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpPacketIsClosed));
	__RTP_LOG1(_L("\t==> %u"), iPtr->Header()->iSeq);
	return iPtr->Header()->iSeq;
	}




/**
Gets the packet's timestamp.

This is a monotonic linear time representation, but the resolution
and zero point are profile dependent.

@return The packet's timestamp.

@panic RTP 1 if the packet has not been opened.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function when the packet has not been opened is
             likely to raise panics such as KERN-EXEC 3.
*/
EXPORT_C TUint RRtpPacket::Timestamp() const
	{
	__RTP_LOG(_L("RRtpPacket::Timestamp() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpPacketIsClosed));
	__RTP_LOG1(_L("\t==> %u"), iPtr->Header()->iTimestamp);
	return iPtr->Header()->iTimestamp;
	}


/**
Gets the SSRC, the 32-bit numeric synchronisation source identifier.

@return The 32-bit SSRC identifier.

@panic RTP 1 if the packet has not been opened.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function when the packet has not been opened is
             likely to raise panics such as KERN-EXEC 3.
*/
EXPORT_C TUint RRtpPacket::SSRC() const
	{
	__RTP_LOG(_L("RRtpPacket::SSRC() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpPacketIsClosed));
	__RTP_LOG1(_L("\t==> %u"), iPtr->Header()->iSSRC);
	return iPtr->Header()->iSSRC;
	}




/**
Gets the type of payload carried by the packet.

@return The 7-bit payload type.

@panic RTP 1 if the packet has not been opened.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function when the packet has not been opened is
             likely to raise panics such as KERN-EXEC 3.
*/
EXPORT_C TUint RRtpPacket::PayloadType() const
	{
	__RTP_LOG(_L("RRtpPacket::PayloadType() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpPacketIsClosed));
	__RTP_LOG1(_L("\t==> %u"), iPtr->Header()->iPayloadType);
	return iPtr->Header()->iPayloadType;
	}




/**
Tests whether the marker bit is set in the packet header.

@return True, if the marker bit is set, false otherwise.

@panic RTP 1 if the packet has not been opened.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function when the packet has not been opened is
             likely to raise panics such as KERN-EXEC 3.
*/
EXPORT_C TBool RRtpPacket::Marker() const
	{
	__RTP_LOG(_L("RRtpPacket::Marker() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpPacketIsClosed));
	__RTP_LOG1(_L("\t==> %d"), iPtr->Header()->iMarker == 1);
	return iPtr->Header()->iMarker == 1;
	}




/**
Tests whether the packet has a header extension.

@return True, if a header extension is present, false otherwise.

@panic RTP 1 if the packet has not been opened.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function when the packet has not been opened is
             likely to raise panics such as KERN-EXEC 3.
*/
EXPORT_C TBool RRtpPacket::ExtensionPresent() const
	{
	__RTP_LOG(_L("RRtpPacket::ExtensionPresent() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpPacketIsClosed));
	__RTP_LOG1(_L("\t==> %d"), iPtr->Header()->iExtension == 1);
	return iPtr->Header()->iExtension == 1;
	}

	
	

/**
Gets the packet's header extension. 

@return The header extension.

@panic RTP 1 if the packet has not been opened.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function when the packet has not been opened is
             likely to raise panics such as KERN-EXEC 3.

@panic RTP 5 if there is no header extension.

@see RRtpPacket::ExtensionPresent()
*/
EXPORT_C RRtpHeaderExtension RRtpPacket::Extension() const
	{
	__RTP_LOG(_L("RRtpPacket::Extension() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpPacketIsClosed));
	__ASSERT_DEBUG(ExtensionPresent(), Panic(ERtpNoExtension));
	RRtpHeaderExtension h;
	h.iPtr = iPtr;
	return h;	
	}


const TUint KRtpPaddingHdrBits = 1; // 1 bit in first octet
const TUint KRtpExtensionHdrBits = 1; // 1 bit in first octet
const TUint KRtpCCHdrBits = 4; // 4 bits in first octet
const TUint KRtpMarkerHdrBits = 1; // 1 bit in second octet
const TUint KRtpPayloadTypeHdrBits = 7; // 7 bits in second octet
const TUint KRtpSequenceNoHdrBits = 16; // 16 bits of third and fourth octet

/**
Gets the first 32 bits of the packet header.

This may be useful for profiles that do things such as using a different
number of marker bits.

Flags returned in this format.... V is most significant bit in flags
	0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |V=2|P|X|  CC   |M|     PT      |       sequence number         |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


@return The first 32 bits of the header as an integer.

@panic RTP 1 if the packet has not been opened.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function when the packet has not been opened is
             likely to raise panics such as KERN-EXEC 3.
*/
EXPORT_C TUint RRtpPacket::Flags() const
	{
	__RTP_LOG(_L("RRtpPacket::Flags() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpPacketIsClosed));
	// Returns the first 4 bytes of the packet as an int, for
	// applications that want to interpret it themselves. 
	TUint flags = 0;
	flags = iPtr->Header()->iVersion; // 2bits
	flags <<= KRtpPaddingHdrBits;
	flags |= iPtr->Header()->iPadding;
	flags <<= KRtpExtensionHdrBits;
	flags |= iPtr->Header()->iExtension;
	flags <<= KRtpCCHdrBits;
	flags |= iPtr->Header()->iCsrcCount;
	flags <<= KRtpMarkerHdrBits;
	flags |= iPtr->Header()->iMarker;
	flags <<= KRtpPayloadTypeHdrBits;
	flags |= iPtr->Header()->iPayloadType;
	flags <<= KRtpSequenceNoHdrBits;
	flags |= iPtr->Header()->iSeq;
	return flags;
	}




/**
Gets a handle to a list of CSRCs (contributing sources).

If there are no CSRCs, then the list is empty.

@return A handle encapsulating the list of CSRCs. Callers can access
        the list through this returned handle.

@panic RTP 1 if the packet has not been opened.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function when the packet has not been opened is
             likely to raise panics such as KERN-EXEC 3.
*/
EXPORT_C RRtpCSRCs RRtpPacket::CSRCs() const
	{
	__RTP_LOG(_L("RRtpPacket::CSRCs() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpPacketIsClosed));
	// The RRtpCSRCs object is an identical pointer to the
	// RRtpPacket. The difference is just to structure the API more
	// neatly.
	RRtpCSRCs c;
	c.iPtr = iPtr;
	return c;	
	}




/**
Gets a pointer descriptor representing the packet's
payload. 

@return The pointer descriptor representing the payload.

@panic RTP 1 if the packet has not been opened.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function when the packet has not been opened is
             likely to raise panics such as KERN-EXEC 3.
*/
EXPORT_C TPtrC8 RRtpPacket::Payload() const
	{
	__RTP_LOG(_L("RRtpPacket::Payload() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpPacketIsClosed));
	const TRtpHeader* ptrH = iPtr->Header();
	// Get the length of the useful data in the packet
	TInt len = iPtr->Data().Length();

	if (len == 0)
		{
		// If this is a send packet, the subclass has a separate
		// descriptor that accurately points to the payload, and the
		// length of the base class descriptor is zero. So if the
		// length is 0, we upcast to a SendPacket. (Note that it's
		// impossible for a receive packet to have a zero length as we
		// ignore any packet under 12 bytes long and no-one's going to
		// attempt to get the payload from a packet that hasn't
		// arrived yet.) It has to work this way because the client
		// doesn't tell the stack when it changes the size of the
		// payload descriptor.
		TRtpSendPacket* sendPacket = static_cast<TRtpSendPacket*>(iPtr);
		return sendPacket->Payload();
		}

	// Remove the padding if present
	if (ptrH->iPadding)
		{
		// we've established that this is receive packet (see above)
		// thus last octet from iData should get us number of padding bytes!
		len -= (*iPtr)[len - 1];
		}
	// lets get a HeaderMap in order to access CSRCs and beyond...
	const TRtpSimpleHeaderMap* ptrHMap = 
		reinterpret_cast<const TRtpSimpleHeaderMap*>(iPtr->Data().Ptr());
	const TUint8* ptr = 
		reinterpret_cast<const TUint8*>(&ptrHMap->iCSRCs[ptrH->iCsrcCount]);

	if (ptrH->iExtension)
		{
		// Skip over the first part of the extension to get to its length
		ptr += 2;
		// Then skip over the rest of the extension (including the length)
		ptr += TRtpPacket::Swap16(*reinterpret_cast<const TUint16*>(ptr)) + 2;
		}

	return TPtrC8(ptr, iPtr->Data().Ptr() + len - ptr);
	}




/**
Closes the RTP packet.
*/
EXPORT_C void RRtpPacket::Close()
	{
	// As the TRtpPacket is at the beginning of the alloc cell
	// then this will work, despite the use of placement new.
	__RTP_LOG(_L("RRtpPacket::Close() ... "));
	delete iPtr;
	iPtr = 0;
	}



/**
Closes the RTP packet.
*/
EXPORT_C void RRtpSendPacket::Close()
    {
    __RTP_LOG(_L("RRtpSendPacket::Close() ... "));
    TRtpSendPacket *sendPkt = reinterpret_cast<TRtpSendPacket*>(iPtr);
    if(iPtr && sendPkt->iSource->ShouldPacketBeClosed(sendPkt, sendPkt->iLength))
        {
        /* Default to base class implementation */
        RRtpPacket::Close();
        }
    iPtr = 0;
    }

/**
Sets the RTP packet's payload type.

@param aPayloadType The 7-bit payload type.

@panic RTP 1 if the packet has not been opened.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function when the packet has not been opened is
             likely to raise panics such as KERN-EXEC 3.
             
@panic RTP 2 if the specified payload type value is greater than
             or equal to 128.        
*/
EXPORT_C void RRtpSendPacket::SetPayloadType(TUint aPayloadType)
	{
	__RTP_LOG(_L("RRtpSendPacket::SetPayloadType() ... "));
	__RTP_LOG1(_L("\t[PayloadType] == %u"), aPayloadType);
	__ASSERT_DEBUG(iPtr, Panic(ERtpPacketIsClosed));
	__ASSERT_ALWAYS(aPayloadType < 128, Panic(ERtpPayloadTypeOutOfRange));
	reinterpret_cast<TRtpSendPacket*>(iPtr)->Header()->iPayloadType = aPayloadType;
	}




/**
Sets or unsets the RTP packet's marker bit.

@param aMark ETrue, if the market bit is to be set, EFalse, if the marker bit
             is to be unset.

@panic RTP 1 if the packet has not been opened.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function when the packet has not been opened is
             likely to raise panics such as KERN-EXEC 3.
             
@see RRtpPacket::Marker()
*/
EXPORT_C void RRtpSendPacket::SetMarker(TBool aMark)
	{
	__RTP_LOG(_L("RRtpSendPacket::SetMarker() ... "));
	__RTP_LOG1(_L("\t[Mark] == %d"), aMark);
	__ASSERT_DEBUG(iPtr, Panic(ERtpPacketIsClosed));
	reinterpret_cast<TRtpSendPacket*>(iPtr)->Header()->iMarker = aMark ? 1 : 0;
	}




/**
Sets the RTP packet's timestamp

@param aTimestamp The RTP packet timestamp

@panic RTP 1 if the packet has not been opened.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function when the packet has not been opened is
             likely to raise panics such as KERN-EXEC 3.

@see  RRtpPacket::Timestamp()
*/
EXPORT_C void RRtpSendPacket::SetTimestamp(TUint aTimestamp)
	{
	__RTP_LOG(_L("RRtpSendPacket::SetTimestamp() ... "));
	__RTP_LOG1(_L("\t[Timestamp] == %u"), aTimestamp);
	__ASSERT_DEBUG(iPtr, Panic(ERtpPacketIsClosed));
	reinterpret_cast<TRtpSendPacket*>(iPtr)->Header()->iTimestamp = aTimestamp;
	}




/**
Sets the first 32 bits of the packet header.

This function may be useful	for profiles that do unusual things such as
combining the payload type and marker bits, but should be used with extreme
caution as no validity checking is done on the values passed
in.

Note that the number passed in is in opposite endianess to that required by
the packet. The function does not correct the endianess.

@param aFlags The 32 bits.
aFlags expected in this format.... V is most significant bit in aFlags
	0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |V=2|P|X|  CC   |M|     PT      |       sequence number         |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

@panic RTP 1 if the packet has not been opened.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function when the packet has not been opened is
             likely to raise panics such as KERN-EXEC 3.

@see RRtpPacket::Flags()
*/

EXPORT_C void RRtpSendPacket::SetFlags(TUint aFlags)
	{
	__RTP_LOG(_L("RRtpSendPacket::SetFlags() ... "));
	__RTP_LOG1(_L("\t[Flags] == %u"), aFlags);
	__ASSERT_DEBUG(iPtr, Panic(ERtpPacketIsClosed));
	
	// TRtpFixedHdrMemLayout expect data as from packet... thus swap
	TUint32 bitFlags = TRtpPacket::Swap32(aFlags);
	TPtr8 flags(reinterpret_cast<TUint8*>(&bitFlags),4);
	TRtpFixedHdrMemLayout header(flags);
	// lets update internal iHeader (appropriate bits only) here
	// and it will get written into packet before send.
	iPtr->Header()->iVersion = header.Version();
	iPtr->Header()->iPadding = header.HasPadding() ? 1 : 0;
	iPtr->Header()->iExtension = header.HasExtension() ? 1 : 0;
	iPtr->Header()->iCsrcCount = header.CC();
	iPtr->Header()->iMarker = header.HasMarker() ? 1 : 0;
	iPtr->Header()->iPayloadType = header.PayloadType();
	iPtr->Header()->iSeq = header.SequenceNumber();
	}




/**
Sends the RTP packet.

The packet must be correctly initialised, apart from the sequence number and
the SSRC (the 32-bit numeric synchronisation source identifier).

The packet must continue to exist until it has been sent, i.e. until
an ERtpSendSucceeded or an ERtpSendFailed event has been received on
the RRtpSendSource.

@panic RTP 1 if the packet has not been opened.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function when the packet has not been opened is
             likely to raise panics such as KERN-EXEC 3.
*/
EXPORT_C void RRtpSendPacket::Send()
	{
	__ASSERT_DEBUG(iPtr, Panic(ERtpPacketIsClosed));
	__RTP_LOG(_L("RRtpSendPacket::Send() ... "));
	TRtpSendPacket* ptr = static_cast<TRtpSendPacket*>(iPtr);
	InitRtpPacket(ptr);
	ptr->iSource->Send(ptr);
	}


/**
Sends the RTP packet.

The packet must be correctly initialised, apart from the sequence number and
the SSRC (the 32-bit numeric synchronisation source identifier).

Sends the RTP data Synchronously.

@panic RTP 1 if the packet has not been opened.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function when the packet has not been opened is
             likely to raise panics such as KERN-EXEC 3.
*/
EXPORT_C TInt RRtpSendPacket::SendSync( )
	{
	__ASSERT_DEBUG(iPtr, Panic(ERtpPacketIsClosed));
	__RTP_LOG(_L("RRtpSendPacket::Send() ... "));
	TRtpSendPacket* ptr = static_cast<TRtpSendPacket*>(iPtr);
	InitRtpPacket(ptr);
	TRequestStatus stat;
	ptr->iSource->Send(ptr, stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

void RRtpSendPacket::InitRtpPacket(TRtpSendPacket* ptr)
	{
	__RTP_LOG(_L("RRtpSendPacket::InitRtpPacket() ... "));
	// we need to write the current header data into the packet itself first
	ptr->WriteHeaderToPacket();
	// Set the end of the iData descriptor correctly; it should be at
	// the same place as the end of the iPayload descriptor
	const TUint8* packetStart = ptr->iData.Ptr();
	const TUint8* payloadStart = ptr->iPayload.Ptr();
	ptr->iData.Set(const_cast<TUint8*>(packetStart), 
				   payloadStart - packetStart + ptr->iPayload.Length(), 
				   ptr->iData.MaxLength());	
	}




/**
Gets a modifiable descriptor into which the client can write
the packet's payload.

@return A reference to the modifiable descriptor.

@panic RTP 1 if the packet has not been opened.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function when the packet has not been opened is
             likely to raise panics such as KERN-EXEC 3.
*/
EXPORT_C TDes8& RRtpSendPacket::WritePayload()
	{
	__RTP_LOG(_L("RRtpSendPacket::WritePayload() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpPacketIsClosed));
	TRtpSendPacket* ptr = static_cast<TRtpSendPacket*>(iPtr);
	return ptr->iPayload;	
	}

// This doesn't leave as the only place that calls it
// (CRtpSession::WaitFOrPacketL) has to do some processing on a OOM
// condition before leaving, so this is more efficient.
TRtpReceivePacket* TRtpReceivePacket::New(TInt aLength)
	{
	// Allocate enough space for the header object and the data buffer
	__RTP_LOG(_L("TRtpReceivePacket::New() ... "));
	__RTP_LOG1(_L("\t[Length] == %d"), aLength);
	TAny* ptr = User::Alloc(aLength + sizeof(TRtpReceivePacket));
	if (!ptr)
		return NULL;
	// And initialise the header object with placement new
	TRtpReceivePacket* packet = new (ptr) TRtpReceivePacket(aLength);

	return packet;
	}

// Checks if the descriptor is shorter than the packet header. This is
// so that we don't overrun the descriptor trying to decode the
// header. Note that this function needs to move down the packet, both
// checking how long the descriptor is and finding out how long the
// header should be. For instance, it can't check the lenght of the
// extension until it's sure that the bytes at the start of the
// extension are present.
TBool TRtpReceivePacket::IsTooShort()
	{
	__RTP_LOG(_L("TRtpReceivePacket::IsTooShort() ... "));
	if (iData.Length() < KRtpHeaderSize)
		{
		__RTP_LOG(_L("\t==> ETrue"));
		return ETrue;
		}
	// we do not want to assume that data in Header() has been alredy updated
	// thus we access data in the packet directly....
	TRtpFixedHdrMemLayout header(iData);
	TInt headerLength = KRtpHeaderSize;
	// length of CSRC block is number of CSRC * 4 octets per CSRC
	TInt csrcCountInOctets = header.CC() * 4;
	headerLength += csrcCountInOctets;
	// Don't try to read the length from the extension if the entire
	// extension isn't there!
	if (header.HasExtension() && iData.Length() < 
		KRtpHeaderSize + csrcCountInOctets + KRtpHeaderExtensionSize)
		{
		__RTP_LOG(_L("\t==> ETrue"));
		return ETrue;
		}
	if (header.HasExtension())
		{
		RRtpReceivePacket p;
		p.iPtr = this;
		headerLength += p.Extension().Data().Length() + KRtpHeaderExtensionSize;
		}
	if (iData.Length() < headerLength)
		{
		__RTP_LOG(_L("\t==> ETrue"));
		return ETrue;
		}

	// Check that the padding size is not bigger then the data length itself...
	if (header.HasPadding())
		{
		if (iData.Length() < headerLength + iData[iData.Length() - 1])
			{
			__RTP_LOG(_L("\t==> ETrue"));
			return ETrue;
			}
		}
	__RTP_LOG(_L("\t==> EFalse"));
	return EFalse;
	}

// Updates data in the iHeader from packet (iData)
void TRtpReceivePacket::ReadIntoHeader()
	{
	TRtpFixedHdrMemLayout header(iData);
	
	__RTP_LOG(_L("TRtpReceivePacket::ReadIntoHeader() ... "));
	iHeader.iVersion = header.Version();
	iHeader.iPadding = header.HasPadding() ? 1 : 0;
	iHeader.iExtension = header.HasExtension() ? 1: 0;
	iHeader.iCsrcCount = header.CC();
	iHeader.iMarker = header.HasMarker() ? 1 : 0;
	iHeader.iPayloadType = header.PayloadType();
	iHeader.iSeq = header.SequenceNumber();
	iHeader.iTimestamp = header.TimeStamp();
	iHeader.iSSRC = header.SSRC();
	}

TRtpSendPacket* TRtpSendPacket::NewL(CRtpSendSource* aStream, 
									 TInt aPayloadLength,
									 TInt aExtensionLength)
	{
	__RTP_LOG(_L("TRtpSendPacket::NewL() ... "));
	// Note that it's assumed there are no CSRCs in any sent packet as
	// it doesn't make much sense to run a mixer on a phone.
	TInt length = aPayloadLength + aExtensionLength + KRtpHeaderSize;
	// Allocate enough space for the header object and the data buffer
	TAny* ptr = User::AllocL(length + sizeof(TRtpSendPacket));
	// And initialise the header object with placement new
	TRtpSendPacket* packet = new (ptr) TRtpSendPacket(aStream, aPayloadLength,
													  aExtensionLength + 
													  KRtpHeaderSize);
	packet->iLength = aPayloadLength + aExtensionLength;

	__RTP_LOG1(_L("\t==> packet(%d)"), packet);
	return packet;
	}

TRtpSendPacket* TRtpSendPacket::NewL(CRtpSendSource* aStream, 
                                     TInt aPayloadLength,
                                     TInt aExtensionLength,
                                     TAny *aPtr)
    {
    __RTP_LOG(_L("TRtpSendPacket::NewL() ... "));
    // Note that it's assumed there are no CSRCs in any sent packet as
    // it doesn't make much sense to run a mixer on a phone.
        
    // And initialise the header object with placement new
    TRtpSendPacket* packet = new (aPtr) TRtpSendPacket(aStream, aPayloadLength,
                                                      aExtensionLength + 
                                                      KRtpHeaderSize);
    packet->iLength = aPayloadLength + aExtensionLength;
	__RTP_LOG1(_L("\t==> packet(%d)"), packet);
	return packet;
	}

TRtpSendPacket::TRtpSendPacket(CRtpSendSource* aSource, 
									  TInt aPayloadLength,
									  TInt aHeaderLength)
		: TRtpPacket(GetPointerToPacketData(), aPayloadLength + aHeaderLength),
		iSource(aSource), iPayload(reinterpret_cast<TUint8*>(GetPointerToPacketData()) + 
								   aHeaderLength, aPayloadLength)
									  
	{
	__RTP_LOG(_L("TRtpSendPacket::TRtpSendPacket() ... "));
	__RTP_LOG1(_L("\t[Source] == %d"), aSource);
	__RTP_LOG1(_L("\t[PayloadLength] == %d"), aPayloadLength);
	__RTP_LOG1(_L("\t[HeaderLength] == %d"), aHeaderLength);
	if (aHeaderLength > KRtpHeaderSize)
		{
		// There is a header extension. Set up its length word.
		TUint16 extensionLength = static_cast<TUint16>(
			(aHeaderLength - KRtpHeaderSize - KRtpHeaderExtensionSize)/4);
		// lets get a HeaderMap in order to access CSRCs and beyond...
		TRtpSimpleHeaderMap* headerMap = 
			const_cast<TRtpSimpleHeaderMap*>(reinterpret_cast<const TRtpSimpleHeaderMap*>(iData.Ptr()));
		// lets get pointer to the 16 bits after any CSRCs (should be any)
		// and extension lenght is just the 16 bits after that... so pointer first 
		TUint16* lengthSlot = 
			reinterpret_cast<TUint16*>(&headerMap->iCSRCs[KRtpMaxNumberOfCSRC]) + 1;

		// and the length itself at last
		*lengthSlot = TRtpPacket::Swap16(extensionLength);
		}
	
	};

// Writes data from the iHeader to the packet (iData)
void TRtpSendPacket::WriteHeaderToPacket()
	{
	__RTP_LOG(_L("TRtpSendPacket::WriteHeaderToPacket() ... "));
	// lets write data from iHeader to the packet itself	
	TRtpFixedHdrMemLayout headerInPacket(iData);
	//Version is set in the RRtpSocket
	headerInPacket.SetPadding(Header()->iPadding);
	headerInPacket.SetExtension(Header()->iExtension);
	headerInPacket.SetCSRCCount(Header()->iCsrcCount);
	headerInPacket.SetMarker(Header()->iMarker);
	headerInPacket.SetPayloadType(Header()->iPayloadType);
	// we do not have to worry about seq no and SSRC... done in RRtpSocket
	// and we assume no CSRCs... but would have been set by client anyway...
	headerInPacket.SetTimeStamp(Header()->iTimestamp);
	}


/**
Gets the number of CSRCs in the RTP packet.

@return The number of CSRCs.
*/
EXPORT_C TInt RRtpCSRCs::Count() const
	{
	__RTP_LOG(_L("RRtpCSRCs::Count() ... "));
	__RTP_LOG1(_L("\t==> %d"), iPtr->Header()->iCsrcCount);
	return iPtr->Header()->iCsrcCount;
	}




/**
Gets a specific CSRC as identified by the specified index.

@param aIndex The index value. This is relative to zero, i.e. a zero value
              refers to the first CSRC.
              
@return The CSRC identifier.
*/
EXPORT_C TUint RRtpCSRCs::operator [](TUint aIndex) const
	{	
	__ASSERT_ALWAYS(iPtr->Header()->iCsrcCount > static_cast<TUint>(aIndex),Panic(ERtpWrongParameter)); 
	// lets get a HeaderMap in order to access CSRCs and beyond...
	const TRtpSimpleHeaderMap* headerMap = 
			reinterpret_cast<const TRtpSimpleHeaderMap*>(iPtr->Data().Ptr());
	return TRtpPacket::Swap32(headerMap->iCSRCs[aIndex]);
	}




/**
Gets the RTP packet's header extension type.

@return The header extension type, which is the first 2 bytes of the extension.
*/
EXPORT_C TUint16 RRtpHeaderExtension::Type() const
	{
	__RTP_LOG(_L("RRtpHeaderExtension::Type() ... "));
	// lets get a HeaderMap in order to access CSRCs and beyond...
	const TRtpSimpleHeaderMap* headerMap = 
			reinterpret_cast<const TRtpSimpleHeaderMap*>(iPtr->Data().Ptr());
	// The type is the first 2 bytes of the extension, which is after
	// the CSRCs.
	__RTP_LOG1(_L("\t==> %u"),TRtpPacket::Swap16(*reinterpret_cast<const TUint16*>(
								  &headerMap->iCSRCs[iPtr->Header()->iCsrcCount])));

	return TRtpPacket::Swap16(*reinterpret_cast<const TUint16*>(
								  &headerMap->iCSRCs[iPtr->Header()->iCsrcCount]));
	}
	
	
	
	
/**
Sets the RTP packet's header extension type.

@param aType The header extension type, which is the first 2 bytes of
             the extension.
*/	
EXPORT_C void RRtpHeaderExtension::SetType(TUint16 aType)
	{
	__RTP_LOG(_L("RRtpHeaderExtension::SetType() ... "));
	__RTP_LOG1(_L("\t[Type] == 0x%x"), aType);
	// lets get a HeaderMap in order to access CSRCs and beyond...
	TRtpSimpleHeaderMap* headerMap = 
		const_cast<TRtpSimpleHeaderMap*>(reinterpret_cast<const TRtpSimpleHeaderMap*>(iPtr->Data().Ptr()));
	// The type is the first 2 bytes of the extension, which is after
	// the CSRCs.
	*reinterpret_cast<TUint16*>(&headerMap->iCSRCs[iPtr->Header()->iCsrcCount]) = 
		TRtpPacket::Swap16(aType);
	}




/**
Gets a pointer descriptor representing the data in the RTP packet's
header extension.

@return The pointer descriptor representing the extension data.
*/
EXPORT_C TPtrC8 RRtpHeaderExtension::Data()
	{
	__RTP_LOG(_L("RRtpHeaderExtension::Data() ... "));
	// This returns the rest of the extension, whose length is in the
	// 2 bytes after the type (which in turn is after the CSRcs.)
	// And the length is specified in 4-byte words.
	TInt csrcCount = iPtr->Header()->iCsrcCount;
	const TRtpSimpleHeaderMap* headerMap = 
			reinterpret_cast<const TRtpSimpleHeaderMap*>(iPtr->Data().Ptr());
	// Lets get the header extension which contains a 16-bit length field 
	// that counts the number of 32-bit words in the extension
	TInt len = TRtpPacket::Swap16((TUint16)(headerMap->iCSRCs[csrcCount++]>>16));
	// and get the length in octets
	len *= 4;
	const TUint8* ptr = 
		reinterpret_cast<const TUint8*>(&headerMap->iCSRCs[csrcCount]);
	return TPtrC8(ptr, len);
	}

TRtpHeader::TRtpHeader(TUint8* aPtr,TInt aLength)
	{	
	__RTP_LOG(_L("TRtpHeader::TRtpHeader() ... "));
	__RTP_LOG1(_L("\t[Ptr] == %d"), aPtr);
	__RTP_LOG1(_L("\t[Length] = %d"), aLength);
	TPtr8 ptr8(aPtr,aLength);
	TRtpFixedHdrMemLayout header(ptr8);
	
	iVersion = header.Version();
	iPadding = header.HasPadding() ? 1 : 0;
	iExtension = header.HasExtension() ? 1 : 0;
	iCsrcCount = header.CC();
	iMarker = header.HasMarker() ? 1 : 0;
	iPayloadType = header.PayloadType();
	iSeq = header.SequenceNumber();
	iTimestamp = header.TimeStamp();
	iSSRC = header.SSRC();
	}

