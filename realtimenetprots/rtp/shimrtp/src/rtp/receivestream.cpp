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
// class for handling incoming rtp stream
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
	__RTP_LOG(_L("RRtpReceiveSource::Packet() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSourceIsClosed));
	RRtpReceivePacket p;
	p.iPtr = iPtr->iNewPacket;
	iPtr->iNewPacket = NULL;
	return p;
	}




/**
@internalComponent
*/

EXPORT_C void RRtpReceiveSource::PrivRegisterEventCallbackL(TUint aType, 
											  TRtpCallbackFunction aCallback, 
											  TAny* aPtr, TInt aParameter)
	{
	__RTP_LOG(_L("RRtpReceiveSource::PrivRegisterEventCallbackL() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSourceIsClosed));
	iPtr->iEventModel.RegisterEventCallbackL(aType,aCallback,aPtr,aParameter);
	}




/**
@internalComponent
*/
EXPORT_C void RRtpReceiveSource::PrivRegisterEventCallbackL(TUint aType, 
											  TRtpCallbackFunction aCallback, 
											  TAny* aPtr)
	{
	__RTP_LOG(_L("RRtpReceiveSource::PrivRegisterEventCallbackL() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSourceIsClosed));
	iPtr->iEventModel.RegisterEventCallbackL(aType, aCallback, aPtr,
											 KRtpNoParameter);
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
	__RTP_LOG(_L("RRtpReceiveSource::Close() ... "));
	if (iPtr)
		iPtr->Close();
	iPtr = 0;
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
	__RTP_LOG(_L("RRtpReceiveSource::SSRC() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSourceIsClosed));
	__RTP_LOG1(_L("\t==> %u"), iPtr->iSSRC);
	return iPtr->iSSRC;
	}

EXPORT_C void RRtpReceiveSource::GetLastApp(TPtrC8& aName, TPtrC8& aAppData, TUint& aSubType)
	{
	__RTP_LOG(_L("RRtpReceiveSource::GetLastApp() ... "));
	iPtr->GetLastApp(aName, aAppData, aSubType);
	}

EXPORT_C TDesC8& RRtpReceiveSource::GetByeReason()
	{
	__RTP_LOG(_L("RRtpReceiveSource::GetByeReason() ... "));
	return *iPtr->iLastByeReason;	
	}


EXPORT_C TInt RRtpReceiveSource::GetSDES(const TInt aType, TDes8& aValue)
	{	
	__RTP_LOG(_L("RRtpReceiveSource::GetSDES() ... "));
	return GetSDESPart(aType, aValue, iPtr->iLastSdesPart);
	}

EXPORT_C RRtcpSRPart RRtpReceiveSource::GetSR()
	{
	__RTP_LOG(_L("RRtpReceiveSource::GetSR() ... "));
	return RRtcpSRPart(iPtr->iLastSRPart);	
	}

EXPORT_C TUint RRtpReceiveSource::PayLoadTypeL()
	{
	if(iPtr->iNewPacket)
		{
		return iPtr->iNewPacket->Header()->iPayloadType;
		}
	User::Leave(KErrNotFound);
	return 0;
	}


/*************/

void CRtpReceiveSource::Close()
	{
	__RTP_LOG(_L("CRtpReceiveSource::Close() ... "));
	if (iEventModel.CanCloseNow())
		delete this;
	}

CRtpReceiveSource::~CRtpReceiveSource()
	{
	delete iNewPacket;
	delete iLastByeReason;
	iAppPart.Reset();
	iLastSdesPart.Reset();
	iEventModel.Close();
	if(iSession)
		{
		iSession->RemoveRXSource(this);
		}
	}

void CRtpReceiveSource::NewPacketL(TRtpReceivePacket* aPacket, 
								   MEventSource*& aErrorCallback)
	{	
	__RTP_LOG(_L("CRtpReceiveSource::NewPacketL() ..."));
	aErrorCallback = this;
	delete iNewPacket;
	iNewPacket = aPacket;
	// upload the iHeader in this packet so we can use it from now on via Header()
	iNewPacket->ReadIntoHeader();
	NotifyPacketReceivedL();
	aErrorCallback = NULL;
	}


void CRtpReceiveSource::ActivateL(MEventSource*& aErrorCallback)
	{	
	__RTP_LOG(_L("CRtpReceiveSource::ActivateL() ..."));
	iPrevPayloadType = iNewPacket->Header()->iPayloadType;
	aErrorCallback = this;
	NotifyPacketReceivedL();
	aErrorCallback = NULL;
	}


void CRtpReceiveSource::NotifyRtcpReceivedL(TRtpEventType aEvent, TInt aError)
	{	
	__RTP_LOG(_L("CRtpReceiveSource::NotifyRtcpReceivedL() ... "));
	iEventModel.HandleEventL(aEvent, aError, this);
	}

void CRtpReceiveSource::NotifyPacketReceivedL()
	{
	__RTP_LOG(_L("CRtpReceiveSource::NotifyPacketReceivedL() ... "));
	if (iNewPacket->Header()->iMarker)
		{
		iEventModel.HandleEventL(ERtpMarker, 0, this);
		}
	if (iNewPacket->Header()->iPayloadType != iPrevPayloadType)
		{
		__RTP_LOG2(_L("Payload changed from %d to %d"), iPrevPayloadType, iNewPacket->Header()->iPayloadType);
		iPrevPayloadType = iNewPacket->Header()->iPayloadType;
		iEventModel.HandleEventL(ERtpCodecChange, iPrevPayloadType, this);
		}
	iEventModel.HandleEventL(ERtpPacketReceived, 0, this);	
	}

void CRtpReceiveSource::HandleErrorL(TInt aError)
	{	
	__RTP_LOG(_L("CRtpReceiveSource::HandleErrorL() ... "));
	__RTP_LOG1(_L("\t[Error] == %d"), aError);
	iEventModel.HandleEventL(ERtpSourceFail, aError, this);
	}

void CRtpReceiveSource::CloseNow()
	{
	__RTP_LOG(_L("CRtpReceiveSource::CloseNow() ... "));
	delete this;
	}

void CRtpReceiveSource::CloseSession()
	{
	__RTP_LOG(_L("CRtpReceiveSource::CloseSession() ... "));
	iEventModel.CloseSession();
	}
TInt CRtpReceiveSource::WhatShouldClose()
	{
	__RTP_LOG(_L("CRtpReceiveSource::WhatShouldClose() ... "));
	return iEventModel.WhatShouldClose();
	}

void CRtpReceiveSource::GetLastApp(TPtrC8& aName, TPtrC8& aAppData, TUint& aSubType)
	{	
	__RTP_LOG(_L("CRtpReceiveSource::GetLastApp() ... "));
	// Parse the data
	aSubType = iAppPart.iSubType;
	aName.Set(iAppPart.iName);
	aAppData.Set(*iAppPart.iAppData);
	}

void CRtpReceiveSource::SetLastAppL(TDesC8& aName, TDesC8& aAppData, TUint& aSubType)
	{	
	__RTP_LOG(_L("CRtpReceiveSource::SetLastAppL() ... "));
	iAppPart.iSubType = aSubType;
	iAppPart.iName = aName;
	delete iAppPart.iAppData;
	iAppPart.iAppData = 0;
	iAppPart.iAppData = aAppData.AllocL();
	}

void CRtpReceiveSource::SetLastByeL(TDesC8& aBye)
	{	
	__RTP_LOG(_L("CRtpReceiveSource::SetLastByeL() ... "));
	delete iLastByeReason;
	iLastByeReason = 0;
	iLastByeReason = aBye.AllocL();
	}

/**
This function copies the packet data to SR report block

@param aReport     The SR report block
@param aSsrc    The Source Synchronisation identifier
@param aSr    The SR block
*/
void CRtpReceiveSource::SetLastSR(TDesC8& aReport, const TUint aSsrc)
	{	
	__RTP_LOG(_L("CRtpReceiveSource::SetLastSR() ... "));
	TRtcpSRSenderInfoMemLayout layout(aReport, 0);
	iLastSRPart.ssrc = aSsrc;
	iLastSRPart.ntp_sec = layout.NtpMsb(); //-- Check these
	iLastSRPart.ntp_frac = layout.NtpLsb();
	iLastSRPart.rtp_ts = layout.RtpTimeStamp();
	iLastSRPart.psent = layout.PacketCount();
	iLastSRPart.osent = layout.OctetCount();
	}


TInt GetSDESPart(const TInt aType, TDes8& aValue, TRtcpSdesPart& aLastSdesPart)
	{	
	__RTP_LOG(_L("GetSDESPart() ... "));
	const TInt numItems = aLastSdesPart.Count();
	TInt result = KErrNotFound;
	for (TInt count = 0; count < numItems; count++)
		{
		TRtcpSdesItem& item = aLastSdesPart[count];
		if ((TUint8)item.iType == aType)
			{
			// if Length is valid
			if (aValue.MaxLength() < item.iData.Length())
				{
				result = KErrArgument;
				break;
				}
			aValue.Copy(item.iData);
			result = KErrNone;
			break;
			}
		}
	return result;
	}

