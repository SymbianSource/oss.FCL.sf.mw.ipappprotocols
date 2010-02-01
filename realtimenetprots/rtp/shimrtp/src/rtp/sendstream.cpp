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
// Active object class for handling rtp send
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
	__RTP_LOG(_L("RRtpSendSource::Close() ... "));
	if (iPtr)
		iPtr->Close();
		iPtr = 0;
	}



/**
Cancels the outstanding send on the send Stream

The send object must be cancelled if a send request is outstanding
*/
EXPORT_C void RRtpSendSource::Cancel()
	{
	__RTP_LOG(_L("RRtpSendSource::Cancel() ... "));
	if (iPtr)
		iPtr->Cancel();
	}




/**
@internalComponent
*/
EXPORT_C void
RRtpSendSource::PrivRegisterEventCallbackL(TUint aType, 
										   TRtpCallbackFunction aCallback, 
										   TAny* aPtr, 
										   TInt aParameter)
	{
	__RTP_LOG(_L("RRtpSendSource::PrivRegisterEventCallbackL() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSourceIsClosed));

	iPtr->iEventModel.RegisterEventCallbackL(aType, aCallback, aPtr,
											 aParameter);
	}




/**
@internalComponent
*/

EXPORT_C void
RRtpSendSource::PrivRegisterEventCallbackL(TUint aType, 
										   TRtpCallbackFunction aCallback, 
										   TAny* aPtr)
	{
	__RTP_LOG(_L("RRtpSendSource::PrivRegisterEventCallbackL() ..."));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSourceIsClosed));
	iPtr->iEventModel.RegisterEventCallbackL(aType, aCallback, aPtr,
											 KRtpNoParameter);
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
EXPORT_C void RRtpSendSource::SetPayloadType(TUint aPayloadType)
	{
	__RTP_LOG(_L("RRtpSendSource::SetPayloadType() ... "));
	__RTP_LOG1(_L("\t[PayloadType] == %u"), aPayloadType);
	__ASSERT_DEBUG(iPtr, Panic(ERtpSourceIsClosed));
	iPtr->iPayloadType = aPayloadType;
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
EXPORT_C void RRtpSendSource::SetDefaultPayloadSize(TInt aPayloadSize)
	{
	__RTP_LOG(_L("RRtpSendSource::SetDefaultPayloadSize() ... "));
	__RTP_LOG1(_L("\t[PayloadSize] == %d"), aPayloadSize);
	__ASSERT_DEBUG(iPtr, Panic(ERtpSourceIsClosed));
	iPtr->iPayloadSize = aPayloadSize;
	}




/**
Sets the alignment required for padding.

After padding, all blocks will be a multiple of this length.

@param aAlignment The padding value. This must be in the range:
                  1-255. A value of 1 means don't pad and is 
                  the default value.
                  
@see RRtpSendSource::Alignment()         
*/
EXPORT_C void RRtpSendSource::SetAlignment(TInt aAlignment)
	{
	__RTP_LOG(_L("RRtpSendSource::SetAlignment() ... "));
	__RTP_LOG1(_L("\t[Alignment] == %d"), aAlignment);	
	__ASSERT_DEBUG(iPtr, Panic(ERtpSourceIsClosed));
	iPtr->iAlignment = aAlignment;
	}




/**
Gets the alignment value.

@return The alignment value.

@see RRtpSendSource::SetAlignment()         
*/
EXPORT_C TInt RRtpSendSource::Alignment() const
	{
	__RTP_LOG(_L("RRtpSendSource::Alignment() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSourceIsClosed));
	__RTP_LOG1(_L("\t==> %d"), iPtr->iAlignment);
	return iPtr->iAlignment;
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
		RRtpSendSource::NewSendPacketL(TInt aPayloadSize, 
									   TInt aHeaderExtensionSize)
	{
	__RTP_LOG(_L("RRtpSendSource::NewSendPacketL() ... "));
	RRtpSendPacket p = NewSendPacketLC(aPayloadSize,aHeaderExtensionSize);
	CleanupStack::Pop(p.iPtr);
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
	RRtpSendSource::NewSendPacketLC(TInt aPayloadSize, TInt aHeaderExtensionSize)
	{
	__RTP_LOG(_L("RRtpSendSource::NewSendPacketLC() ... "));
	__RTP_LOG1(_L("\t[PayloadSize] == %d"), aPayloadSize);
	__RTP_LOG1(_L("\t[HeaderExtensionSize] == %d"), aHeaderExtensionSize);
	__ASSERT_DEBUG(iPtr, Panic(ERtpSourceIsClosed));
	// Use the default payload size if required.
	if (aPayloadSize == 0)
		aPayloadSize = iPtr->iPayloadSize;
	// The extension size is the number of 4-byte words in the
	// extension, excluding the header of the extension. Thus 0 is a
	// permissible value that has a different meaning from
	// KRtpNoExtension
	TInt extensionSize = 0;
	if (aHeaderExtensionSize != KRtpNoExtension)
		{
		extensionSize = (aHeaderExtensionSize + 1) * 4;
		}
	TRtpSendPacket* packet;
	/* See if we can reuse the buffered Packet */
	if(iPtr->iBufferedPkt && iPtr->iBuffPktLen >= aPayloadSize + aHeaderExtensionSize)
	    {
	    packet = TRtpSendPacket::NewL(iPtr, aPayloadSize,
	                                                   extensionSize, iPtr->iBufferedPkt);
	    iPtr->iBufferedPkt = NULL;
	    iPtr->iBuffPktLen = 0;
	    }
	else
	    {
	    packet = TRtpSendPacket::NewL(iPtr, aPayloadSize, extensionSize);
	    }
	
	CleanupStack::PushL(packet);
	RRtpSendPacket p;
	p.iPtr = packet;
    
	// Set up the default values for RTP flags (32-bits - version MSB)
	// The first 2 bits should be 10. The X bit should be set
	// if there's an extension. Everything else should be 0 for now.
	TUint flags;
	if (extensionSize)
		flags = 0x90000000;
	else
		flags = 0x80000000;
		
	p.SetFlags(flags);
	p.SetPayloadType(iPtr->iPayloadType);
	return p;
	}

/**
Tests whether the send stream is currently sending an RTP packet.

@return ETrue, if a send operation is in progress, EFalse, otherwise.
*/
EXPORT_C TBool RRtpSendSource::IsSending()
	{
	__RTP_LOG(_L("RRtpSendSource::IsSending()"));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSourceIsClosed));
	return iPtr->IsActive();
	}

EXPORT_C void RRtpSendSource::ByeL(TDesC8& aReason)
	{
	__RTP_LOG(_L("RRtpSendSource::ByeL() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSourceIsClosed));
	iPtr->iSession->iRtpSocketController->SendBYEPacketL(GetLocalSSRC(), aReason);
	}

EXPORT_C TUint32 RRtpSendSource::GetLocalSSRC()
	{
	__RTP_LOG(_L("RRtpSendSource::GetLocalSSRC() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSourceIsClosed));
	return iPtr->iSession->iRtpSocketController->GetLocalSSRC();
	}

void CRtpSendSource::Close()
	{
	__RTP_LOG1(_L("CRtpSendSource::Close() ... 0x%x"), this);
	if (iEventModel.CanCloseNow())	
		delete this;
	}

CRtpSendSource::~CRtpSendSource()
	{
	__RTP_LOG1(_L("CRtpSendSource::~CRtpSendSource() 0x%x"), this);
	Cancel();
	iEventModel.Close();
    delete iBufferedPkt;
	if(iSession)
		{
		iSession->SendSourceClosing();
		}
	}
	
void CRtpSendSource::HandleErrorL(TInt aError)
	{	
	__RTP_LOG(_L("CRtpSendSource::HandleErrorL() ... "));
	__RTP_LOG1(_L("\t[Error] == %d"), aError);
	iEventModel.HandleEventL(ERtpSendFail, aError, this);
	}

void CRtpSendSource::CloseNow()
	{
	__RTP_LOG(_L("CRtpSendSource::CloseNow() ... "));
	delete this;
	}

void CRtpSendSource::CloseSession()
	{
	__RTP_LOG(_L("CRtpSendSource::CloseSession() ... "));
	iEventModel.CloseSession();
	}

TInt CRtpSendSource::WhatShouldClose()
	{
	__RTP_LOG(_L("CRtpSendSource::WhatShouldClose() ... "));
	return iEventModel.WhatShouldClose();
	}

void CRtpSendSource::RunL()
	{
	__RTP_LOG(_L("CRtpSendSource::RunL() ... "));
	TInt error = iStatus.Int();
	TRtpEventType type;
	if (error)
		{
		type = ERtpSendFail;
		}
	else
		{
		type = ERtpSendSucceeded;
		}
	__RTP_LOG1(_L("\t[type] = %u"), type);
	Deque();
	CActiveScheduler::Add(this);
	// This function must not leave when this isn't the session's
	// error handler.
	iSession->SetErrorHandler(this);
	iEventModel.HandleEventL(type, error, this);
	iSession->SetErrorHandler(iSession);
	}

TInt CRtpSendSource::RunError(TInt aError)
	{	
	// The session will call us back to handle the error via the
	// MEventSource mechanism
	__RTP_LOG(_L("CRtpSendSource::RunError() ... "));
	__RTP_LOG1(_L("[Error] == %d"), aError);
	iSession->HandleError(aError);
	return KErrNone;
	}

void CRtpSendSource::DoCancel()
	{
	__RTP_LOG(_L("CRtpSendSource::DoCancel() ... "));
	iSession->CancelSend();
	}


void CRtpSendSource::Send(TRtpSendPacket* aPacket)
	{	
	__RTP_LOG(_L("CRtpSendSource::Send() ... "));
	__RTP_LOG1(_L("\t[TRtpSendPacket] == %d"), aPacket);
	Send(aPacket, iStatus);
	SetActive();
	}

void CRtpSendSource::Send(TRtpSendPacket* aPacket, TRequestStatus &aStatus)
	{	
	__RTP_LOG(_L("CRtpSendSource::Send() ...aStatus "));
	__RTP_LOG1(_L("\t[TRtpSendPacket] == %d"), aPacket);
	__ASSERT_DEBUG(!IsActive(), Panic(ERtpCantSend2PacketsAtOnce));
	iLastSentPacket = aPacket;
	if (iAlignment > 1 && aPacket->Data().Length() % iAlignment)
		{
		// We need to pad
		TInt padding = iAlignment - aPacket->Data().Length() % iAlignment;
		TInt length = aPacket->Data().Length();
		if (aPacket->Data().MaxLength() < length + padding)
			{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, KErrGeneral);
			__RTP_LOG(_L("\tiData.MaxLength() < length + padding\t==> User::RequestComplete(status, KErrGeneral)"));
			return;
			}
		// we need to pad with zeros
		TChar zeroFill(0);
		aPacket->Data().AppendFill(zeroFill, padding-1);
		// and set number of padded octets
		aPacket->Data().AppendFill(padding, padding);
		aPacket->Header()->iPadding = 1;
		// we need to write to the packet directly as well since other header fields
		// were already copied in RRtpSendPacket::Send()
		TRtpFixedHdrMemLayout header(aPacket->Data());
		header.SetPadding(ETrue);
		}
	iSession->Send(aPacket, aStatus);
	}

TBool CRtpSendSource::ShouldPacketBeClosed(TAny *aPtr, TUint aLength)
    {
    if(!iBufferedPkt || iBuffPktLen < aLength)
        {
        delete iBufferedPkt;
        iBufferedPkt = aPtr;
        iBuffPktLen = aLength;
        /* We have taken the owner ship. So the packet shud not be closed */
        return EFalse;
        }
    return ETrue;
    }
