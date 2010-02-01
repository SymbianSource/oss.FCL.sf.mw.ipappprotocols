// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// packet.cpp
// 
//

/**
 @file
*/


#include <e32base.h>
#include "rtp.h"
#include "packet.h"
#include "sendstream.h"

/** Swaps the bytes of a 16 bit number (i.e. convert from big-endian to
	little endian or vica versa
@internalComponent
*/
TUint16 TRtpPacket::Swap16(TUint16 /*aVal*/)
	{
	ASSERT(0);
	return 0;
	}

/** Swaps the bytes of a 32 bit number (i.e. convert from big-endian to
	little endian or vica versa
@internalComponent
*/
TUint32 TRtpPacket::Swap32(TUint32 /*aVal*/)
	{
	ASSERT(0);
	return 0;
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
	ASSERT(0);
	return 0;
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
	ASSERT(0);
	return 0;
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
	ASSERT(0);
	return 0;
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
	ASSERT(0);
	return 0;
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
	ASSERT(0);
	return 0;
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
	ASSERT(0);
	return 0;
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
	ASSERT(0);
	RRtpHeaderExtension h;
	h.iPtr = 0;
	h.SetType(0);
	return h;	
	}


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
	ASSERT(0);
	return 0;
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
	ASSERT(0);	
	RRtpCSRCs c;
    c.iPtr = NULL;
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
	ASSERT(0);	
	return NULL;
	}




/**
Closes the RTP packet.
*/
EXPORT_C void RRtpPacket::Close()
	{
	// As the TRtpPacket is at the beginning of the alloc cell
	// then this will work, despite the use of placement new.
	ASSERT(0);
	}




/**
Sets the RTP packet's payload type.

@param aPayloadType The 7-bit payload type.

@panic RTP 1 if the packet has not been opened.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function when the packet has not been opened is
             likely to raise panics such as KERN-EXEC 3.
             
@panic RTP 2 if the specified payload type value is greater than
             or equal to 256.        
*/
EXPORT_C void RRtpSendPacket::SetPayloadType(TUint /*aPayloadType*/)
	{
	ASSERT(0);
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
EXPORT_C void RRtpSendPacket::SetMarker(TBool/* aMark*/)
	{
	ASSERT(0);
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
EXPORT_C void RRtpSendPacket::SetTimestamp(TUint /*aTimestamp*/)
	{
	ASSERT(0);
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

EXPORT_C void RRtpSendPacket::SetFlags(TUint /*aFlags*/)
	{
	ASSERT(0);
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
	ASSERT(0);	
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
	ASSERT(0);
	_LIT8(KErrWhatAreWeDoingHere,"WhatAreWeDoingHere!!");
	return (TDes8&)KErrWhatAreWeDoingHere();	
	}

/**
Gets the number of CSRCs in the RTP packet.

@return The number of CSRCs.
*/
EXPORT_C TInt RRtpCSRCs::Count() const
	{
	ASSERT(0);
	return 0;
	}
/**
Gets a specific CSRC as identified by the specified index.

@param aIndex The index value. This is relative to zero, i.e. a zero value
              refers to the first CSRC.
              
@return The CSRC identifier.
*/
EXPORT_C TUint RRtpCSRCs::operator [](TUint /*aIndex*/) const
	{
	ASSERT(0);
	return 0;
	}

/**
Gets the RTP packet's header extension type.

@return The header extension type, which is the first 2 bytes of the extension.
*/
EXPORT_C TUint16 RRtpHeaderExtension::Type() const
	{
	ASSERT(0);
	return 0;
	}
	
/**
Sets the RTP packet's header extension type.

@param aType The header extension type, which is the first 2 bytes of
             the extension.
*/	
EXPORT_C void RRtpHeaderExtension::SetType(TUint16 /*aType*/)
	{
	ASSERT(0);
	}

/**
Gets a pointer descriptor representing the data in the RTP packet's
header extension.

@return The pointer descriptor representing the extension data.
*/
EXPORT_C TPtrC8 RRtpHeaderExtension::Data()
	{
	ASSERT(0);
	return NULL;
	}

EXPORT_C TInt RRtpSendPacket::SendSync( )
	{
	ASSERT(0);
	return KErrNotSupported;
	}

EXPORT_C void RRtpSendPacket::Close()
    {
    ASSERT(0);
    return ;
    }
