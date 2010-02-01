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
// receivestream.cpp
// 
//

/**
 @file
*/

#include <e32base.h>
#include "rtp.h"
#include "events.h"
#include "packet.h"
#include "receivestream.h"
#include "rtputils.h"
#include "session.h"

#include "rtcppacket.h"


/**
Gets the packet that has just arrived, or a closed packet
if there isn't one.

Ownership of the packet is transferred to the caller of this function.

Note that if this function is called twice in quick succession, then it will
return a closed packet the second time. 

@return A received RTP packet.
*/
EXPORT_C RRtpReceivePacket RRtpReceiveSource::Packet()
	{
	ASSERT(0);
	RRtpReceivePacket p;
	return p;
	}


/**
@internalComponent
*/
EXPORT_C void RRtpReceiveSource::PrivRegisterEventCallbackL(TUint /*aType*/, 
											  TRtpCallbackFunction /*aCallback*/, 
											  TAny* /*aPtr*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
@internalComponent
*/
EXPORT_C void RRtpReceiveSource::PrivRegisterEventCallbackL(TUint , 
											  TRtpCallbackFunction , 
											  TAny*, TInt )
	{
	User::Leave(KErrNotSupported);
	}

/**
Closes the receive stream releases all associated resources.

This includes closing all RRtpReceivePacket objects that have not already
been closed.

The receive stream object cannot be closed if an event is currently being
handled, i.e. a call back into client code is currently in progress.
*/
EXPORT_C void RRtpReceiveSource::Close()
	{
	ASSERT(0);
	}

/**
Gets the SSRC, the 32-bit numeric synchronisation source identifier.

@return The 32-bit SSRC identifier.

@panic RTP 4 if the receive stream has not been opened.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function when the packet has not been opened is
             likely to raise panics such as KERN-EXEC 3.
*/
EXPORT_C TUint RRtpReceiveSource::SSRC() const
	{
	ASSERT(0);
	return 0;
	}

EXPORT_C void RRtpReceiveSource::GetLastApp(TPtrC8& , TPtrC8& , TUint& )
	{
	ASSERT(0);
	}

EXPORT_C TDesC8& RRtpReceiveSource::GetByeReason()
	{
	ASSERT(0);
	_LIT8(KErrWhatAreWeDoingHere,"WhatAreWeDoingHere!!");
	return (TDesC8&)KErrWhatAreWeDoingHere();	
	}

EXPORT_C TInt RRtpReceiveSource::GetSDES(const TInt /*aType*/, TDes8& /*aValue*/)
	{
	return KErrNotSupported;
	}

EXPORT_C RRtcpSRPart RRtpReceiveSource::GetSR()
	{
	ASSERT(0);
	TRtcpSRPart tsrPart;
	tsrPart.ssrc = 0xDEAD;
	RRtcpSRPart srp(tsrPart);
	return srp;
	}

EXPORT_C TUint RRtpReceiveSource::PayLoadTypeL()
	{
	ASSERT(0);
	return 0;
	}
