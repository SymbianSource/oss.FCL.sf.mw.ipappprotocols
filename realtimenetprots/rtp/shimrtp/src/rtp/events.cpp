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
// class for handling rtp callbacks
// 
//

/**
 @file
*/

#include <e32base.h>
#include "rtp.h"
#include "rtputils.h"
#include "events.h"

const TInt KTypeMask = 0xf00;


TRtpCallback::TRtpCallback(TRtpCallbackFunction aFn, TAny* aPtr)
		: iFn(aFn), iPtr(aPtr)
	{
	}




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
	__ASSERT_DEBUG(IsSessionEvent(), Panic(ERtpWrongEventType));
	__RTP_LOG(_L("TRtpEvent::Session() ... "));
	RRtpSession session;
	session.iPtr = reinterpret_cast<CRtpSession*>(iData);
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
	__ASSERT_DEBUG(IsReceiveSourceEvent(), Panic(ERtpWrongEventType));
	__RTP_LOG(_L("TRtpEvent::ReceiveSource() ... "));
	RRtpReceiveSource stream;
	stream.iPtr = reinterpret_cast<CRtpReceiveSource*>(iData);
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
	__ASSERT_DEBUG(IsSendSourceEvent(), Panic(ERtpWrongEventType));
	__RTP_LOG(_L("TRtpEvent::SendSource() ... "));
	RRtpSendSource stream;
	stream.iPtr = reinterpret_cast<CRtpSendSource*>(iData);
	return stream;	
	}




/**
Tests whether this event is associated with an RTP session.

@return True, if this event is associated with an RTP session,
        false otherwise.
*/
EXPORT_C TBool TRtpEvent::IsSessionEvent() const
	{
//	return ((iType & KTypeMask) == ERtpSessionEventBase);
	__RTP_LOG(_L("TRtpEvent::IsSessionEvent() ... "));

	return ETrue;
	}




/**
Tests whether this event is associated with a send stream.

@return True, if this event is associated with a send stream,
        false otherwise.
*/
EXPORT_C TBool TRtpEvent::IsSendSourceEvent() const
	{
	__RTP_LOG(_L("TRtpEvent::IsSendSourceEvent() ... "));
	return ((iType & KTypeMask) == ERtpSendEventBase);
	}




/**
Tests whether this event is associated with a receive stream.

@return True, if this event is associated with a receive stream,
        false otherwise.
*/
EXPORT_C TBool TRtpEvent::IsReceiveSourceEvent() const
	{
	__RTP_LOG(_L("TRtpEvent::IsReceiveSourceEvent() ... "));
	return ((iType & KTypeMask) == ERtpSourceEventBase);
	}

TCallbackRegistration::TCallbackRegistration(TUint aType, 
											 TRtpCallback aCallback, 
											 TUint32 aParameter)
		: iType(aType), iCallback(aCallback),
		  iParameter(aParameter), iState(aParameter)
	{
	}

TBool TCallbackRegistration::ShouldEventFire(TRtpEvent& aEvent)
	{	
	const TUint KAnyFail = 1;
	const TUint KFailMask = 0xff;

	__RTP_LOG(_L("TCallbackRegistration::ShouldEventFire() ... "));
	if (aEvent.Type() == Type() || 
		(aEvent.Type() & KFailMask) != KAnyFail && Type() == ERtpAnyEvent)
		{
		// The event basically matches. Check if the registration
		// parameter matches. There are 2 possible meanings of
		// registration parameters; either it's the value that the
		// status should be or it's a count of the number of events to
		// ignore. Which depends on the registration event
		// type. Unless of course the parameter is KRtpNoParameter, in
		// which case it is ignored.
		if (static_cast<TInt>(iParameter) == KRtpNoParameter)
			{
			__RTP_LOG(_L("\t==> ETrue"));
			return ETrue;
			}

		if ((iType & KFailMask) == KAnyFail)
			{
			if (aEvent.Status() != (TInt)iParameter)
				{
				__RTP_LOG(_L("\t==> EFalse")); 
				return EFalse;
				}
			}
		else
			{
			if (--iState)
				{
				__RTP_LOG(_L("\t==> EFalse"));
				return EFalse;
				}
			else
				iState = iParameter;
			}
		__RTP_LOG(_L("\t==> ETrue"));
		return ETrue;
		}
	__RTP_LOG(_L("\t==> EFalse"));
	return EFalse;
	}

void REventModel::HandleEventL(TRtpEventType aEvent, TInt aStatus, TAny* aPtr)
	{	
	__RTP_LOG(_L("REventModel::HandleEventL() ... "));
	__RTP_LOG1(_L("\t[Type] == 0x%x"), aEvent);
	__RTP_LOG1(_L("\t[Status] == %d"), aStatus);
	iCloseStatus = ECallingBack;
	TRtpEvent event(aEvent, aStatus, aPtr);

	TInt count = Count();
	for (TInt i = 0; i < count; i++)
		{
		TCallbackRegistration& r = (*this)[i];
		if ((*this)[i].ShouldEventFire(event))
			{
			// A one-shot callback that leaves must still be removed,
			// hence we remove it before calling it. So we need to
			// make a copy that we actually call.
			TCallbackRegistration r1 = r;
			if (r1.OneShot())
				{
				Remove(i);
				// And skip back a step, as the array indeces are now all out.
				i--;
				count--;
				}

			// And actually call the callback
			r1.iCallback.iFn(r1.iCallback.iPtr, event);
			if (iCloseStatus == EClosing)
				{
				__RTP_LOG(_L("\tiCloseStatus == EClosing"));
				User::Leave(KErrCloseCurrent);
				}
			if (iCloseStatus == EClosingSession)
				{
				__RTP_LOG(_L("\tiCloseStatus == EClosingSession"));
				User::Leave(KErrCloseSession);
				}
			}
		}
	__RTP_LOG(_L("\tiCloseStatus == ENormal"));
	iCloseStatus = ENormal;
	}

void REventModel::RegisterEventCallbackL(TUint aType, 
										 TRtpCallbackFunction aCallback, 
										 TAny* aPtr,
										 TInt aParameter)
	{	
	__RTP_LOG1(_L("\t[Type] == 0x%x"), aType);
	__RTP_LOG1(_L("\t[Parameter] == %d"), aParameter);
	TCallbackRegistration callback(aType, TRtpCallback(aCallback, aPtr),
								   aParameter);
	User::LeaveIfError(Append(callback));
	}

TBool REventModel::CanCloseNow()
	{
	__RTP_LOG(_L("REventModel::CanCloseNow() ... "));
	if (iCloseStatus == ENormal)
		{
		__RTP_LOG(_L("\t==> ETrue"));
		return ETrue;
		}
	else
		{
		__RTP_LOG(_L("\t==> EFalse"));
		iCloseStatus = EClosing;
		return EFalse;
		}
	}

void REventModel::CloseSession()
	{
	__RTP_LOG(_L("REventModel::CloseSession() ... "));
	iCloseStatus = EClosingSession;
	}

TInt REventModel::WhatShouldClose()
	{
	__RTP_LOG(_L("REventModel::WhatShouldClose() ..."));
	if (iCloseStatus == EClosingSession)
		{
		__RTP_LOG(_L("\t==> KErrCloseSession"));
		return KErrCloseSession;
		}
	else if (iCloseStatus == EClosing)
		{
		__RTP_LOG(_L("\t==> KErrCloseCurrent"));
		return KErrCloseCurrent;
		}
	else
		{
		__RTP_LOG(_L("\t==> KErrNone"));
		return KErrNone;
		}
	}
	
TInt REventModel::RemoveCallback(TRtpEventType aType)
	{
	__RTP_LOG(_L("REventModel::RemoveCallback() ... "));
	TInt count = Count();
	TInt err = KErrNotFound;
	for (TInt i = 0; i < count; i++)
		{
		TCallbackRegistration& r = (*this)[i];
		if(r.iType == aType)
			{
			Remove(i);
			err = KErrNone;
			break;
			}
		}
	return err;
	}
