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
//

/**
 @file
*/

#include <e32base.h>
#include "rtp.h"
#include "events.h"

/**
Constructor.

@param aType   The event type.
@param aStatus The event status.
@param aData   Additional data that is dependent on the event type.
*/
EXPORT_C TRtpEvent::TRtpEvent(TRtpEventType aType, TInt aStatus, TAny* aData)
		: iType(aType), iStatus(aStatus), iData(aData)
	{
	}

/**
Gets a handle to the session associated with this event.

This function should only be called for events that are associated with
an RTP session, otherwise it raises a panic.
	   
@panic RTP 8 if the event is not associated with an RTP session.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function on in inappropriate event is
             likely to raise panics such as KERN-EXEC 3.	   
             
@see TRtpEvent::IsSessionEvent()             
*/
EXPORT_C RRtpSession TRtpEvent::Session() const
	{
	ASSERT(0);
	RRtpSession session;
	return session;
	}




/**
Gets a handle to the receive stream associated with this event.

This function should only be called for events that are associated with
a receive stream, otherwise it raises a panic.
	   
@panic RTP 8 if the event is not associated with a receive stream.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function on in inappropriate event is
             likely to raise panics such as KERN-EXEC 3.	   
             
@see TRtpEvent::IsReceiveSourceEvent()             
*/
EXPORT_C RRtpReceiveSource TRtpEvent::ReceiveSource() const
	{
	ASSERT(0);
	RRtpReceiveSource stream;
	return stream;
	}




/**
Gets a handle to the send stream associated with this event.

This function should only be called for events that are associated with
a send stream, otherwise it raises a panic.
	   
@panic RTP 8 if the event is not associated with a send stream.
             Note that this panic is raised in debug mode only; in release
             mode, calling this function on in inappropriate event is
             likely to raise panics such as KERN-EXEC 3.
             
@see TRtpEvent::IsSendSourceEvent()   	   
*/
EXPORT_C RRtpSendSource TRtpEvent::SendSource() const
	{
	ASSERT(0);
	RRtpSendSource stream;
	return stream;	
	}

/**
Tests whether this event is associated with an RTP session.

@return True, if this event is associated with an RTP session,
        false otherwise.
*/
EXPORT_C TBool TRtpEvent::IsSessionEvent() const
	{
	ASSERT(0);
	return EFalse;
	}

/**
Tests whether this event is associated with a send stream.

@return True, if this event is associated with a send stream,
        false otherwise.
*/
EXPORT_C TBool TRtpEvent::IsSendSourceEvent() const
	{
	ASSERT(0);
	return EFalse;
	}




/**
Tests whether this event is associated with a receive stream.

@return True, if this event is associated with a receive stream,
        false otherwise.
*/
EXPORT_C TBool TRtpEvent::IsReceiveSourceEvent() const
	{
	ASSERT(0);
	return EFalse;
	}
