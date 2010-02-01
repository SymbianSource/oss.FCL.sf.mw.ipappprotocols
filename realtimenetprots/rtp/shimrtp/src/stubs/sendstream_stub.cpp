// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// sendstream.cpp
// 
//

/**
 @file
*/

#include <e32base.h>
#include "rtp.h"
#include "rtputils.h"
#include "sendstream.h"
#include "packet.h"
#include "session.h"




/**
Closes the send stream.

The send stream object cannot be closed if an event is currently being
handled, i.e. a call back into client code is currently in progress.
*/
EXPORT_C void RRtpSendSource::Close()
	{
	ASSERT(0);
	}



/**
Cancels the outstanding send on the send Stream

The send object must be cancelled if a send request is outstanding
*/
EXPORT_C void RRtpSendSource::Cancel()
	{
	ASSERT(0); // should call this function
	}




/**
@internalComponent
*/
EXPORT_C void
RRtpSendSource::PrivRegisterEventCallbackL(TUint /*aType*/, 
										   TRtpCallbackFunction /*aCallback*/, 
										   TAny* /*aPtr*/, 
										   TInt /*aParameter*/)
	{
	User::Leave(KErrNotSupported);	
	}




/**
@internalComponent
*/
EXPORT_C void
RRtpSendSource::PrivRegisterEventCallbackL(TUint /*aType*/, 
										   TRtpCallbackFunction /*aCallback*/, 
										   TAny* /*aPtr*/)
	{
	User::Leave(KErrNotSupported);	
	}




/**
Sets the default payload type for packets that are to be sent.

This is the payload type used when a new RTP send packet is created in
calls to: RRtpSendSource::NewSendPacketL() and
RRtpSendSource::NewSendPacketLC().

@param aPayloadType The payload type.

@see RRtpSendSource::NewSendPacketLC()
@see RRtpSendSource::NewSendPacketL()
*/
EXPORT_C void RRtpSendSource::SetPayloadType(TUint /*aPayloadType*/)
	{
	ASSERT(0);
	}




/**
Sets the default payload size for packets that are to be sent.

This is the default payload size used when a new RTP send packet is created in
calls to: RRtpSendSource::NewSendPacketL() and
RRtpSendSource::NewSendPacketLC().

@param aPayloadSize The payload size.

@see RRtpSendSource::NewSendPacketLC()
@see RRtpSendSource::NewSendPacketL()
*/
EXPORT_C void RRtpSendSource::SetDefaultPayloadSize(TInt /*aPayloadSize*/)
	{
	ASSERT(0);
	}




/**
Sets the alignment required for padding.

After padding, all blocks will be a multiple of this length.

@param aAlignment The padding value. This must be in the range:
                  1-255. A value of 1 means don't pad and is 
                  the default value.
                  
@see RRtpSendSource::Alignment()         
*/
EXPORT_C void RRtpSendSource::SetAlignment(TInt /*aAlignment*/)
	{
	ASSERT(0);
	}




/**
Gets the alignment value.

@return The alignment value.

@see RRtpSendSource::SetAlignment()         
*/
EXPORT_C TInt RRtpSendSource::Alignment() const
	{
	return KErrNotSupported;
	}



/**
Creates a new RTP send packet, and returns the RTP send packet handle. 
	
On return from this call, the packet is open but uninitialised, except for
the payload type which is set to the default value set by a previous
call to SetPayloadType()
	
The returned packet is owned by the caller of this function; it is not
owned by the send stream. It must be explicitly closed by a call to
RRtpSendPacket::Close()
	
@param aPayloadSize         The size of the payload. If not explicitly
                            specified, i.e. the default value of 0 is
                            passed to the function, then the size actually
                            used is the value set by a previous call
                            to SetDefaultPayloadSize().
@param aHeaderExtensionSize The size of the extension specified as
                            the number of 4-byte words. This value must
                            exclude the length of the extension header.
                            Zero is a permissible value.
                            If unspecified, i.e. the default value of
                            KRtpNoExtension is passed to the function,
                            then the packet will have no extension.
	
@return The new RTP send packet handle.
		
@see RRtpSendPacket::SetPayloadType()
@see RRtpSendPacket::SetDefaultPayloadSize()
@see RRtpSendPacket::Close()
@see KRtpNoExtension
*/
EXPORT_C RRtpSendPacket 
		RRtpSendSource::NewSendPacketL(TInt /*aPayloadSize*/, 
									   TInt /*aHeaderExtensionSize*/)
	{
	User::Leave(KErrNotSupported);	
	RRtpSendPacket p;
	return p;	
	}

/**
Creates a new RTP send packet, puts a pointer to the RTP send packet object
onto the cleanup stack, and returns the RTP send packet handle.

On return from this call, the packet is open but uninitialised, except for
the payload type which is set to the default value set by a previous
call to SetPayloadType()
	
The returned packet is owned by the caller of this function; it is not
owned by the send stream. It must be explicitly closed by a call to
RRtpSendPacket::Close().

Note that the function does not put a pointer to the RRtpSendPacket object
onto the cleanup stack as you might normally expect, but puts a pointer to 
the RTP send packet object for which the returned RRtpSendPacket is
the handle.

@param aPayloadSize The size of the payload. If unspecified,
   	                i.e. the default value of 0 is passed to the function,
       	            then the size actually used is the value set by
           	        a previous call to SetDefaultPayloadSize().
@param aHeaderExtensionSize The size of the extension specified as
   	                        the number of 4-byte words. This value must
       	                    exclude the length of the extension header.
           	                Zero is a permissible value.
               	            If unspecified, i.e. the default value of
                   	        KRtpNoExtension is passed to the function,
                       	    then the packet will have no extension.
	
@return The new RTP send packet.
		
@see RRtpSendPacket::SetPayloadType()
@see RRtpSendPacket::SetDefaultPayloadSize()
@see RRtpSendPacket::Close()
@see KRtpNoExtension
*/

EXPORT_C RRtpSendPacket 
	RRtpSendSource::NewSendPacketLC(TInt /*aPayloadSize*/, TInt /*aHeaderExtensionSize*/)
	{
	User::Leave(KErrNotSupported);	
	RRtpSendPacket p;
	return p;
	}

/**
Tests whether the send stream is currently sending an RTP packet.

@return ETrue, if a send operation is in progress, EFalse, otherwise.
*/
EXPORT_C TBool RRtpSendSource::IsSending()
	{
	ASSERT(0);
	return EFalse;
	}

EXPORT_C void RRtpSendSource::ByeL(TDesC8& /*aReason*/)
	{
	User::Leave(KErrNotSupported);	
	}

EXPORT_C TUint32 RRtpSendSource::GetLocalSSRC()
	{
	return 0;	
	}

