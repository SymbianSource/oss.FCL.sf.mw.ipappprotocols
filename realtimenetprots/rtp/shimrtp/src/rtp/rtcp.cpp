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
// class for handling rtcp data
// 
//

/**
 @file
*/
 
#include <e32base.h>

#include "rtcp.h"
#include "rtcppacket.h"


EXPORT_C RRtcpRRItem::RRtcpRRItem() :iPtr(NULL)
	{}

EXPORT_C TUint RRtcpRRItem::PacketsLost() const
	{
	__RTP_LOG(_L("RRtcpRRItem::PacketLost() ... "));
	return iPtr->lost;
	}

EXPORT_C TUint RRtcpRRItem::AboutSSRC() const
	{
	__RTP_LOG(_L("RRtcpSRItem::AboutSSRC() ... "));
	return iPtr->ssrc;
	}

EXPORT_C TUint RRtcpRRItem::FractionLost() const
	{
	__RTP_LOG(_L("RRtcpRRItem::FractionLost() ... "));
	return iPtr->fraction;
	}

EXPORT_C TUint RRtcpRRItem::ExtendedHighestSequenceNumber() const
	{
	__RTP_LOG(_L("RRtcpSRItem::ExtendedHighestSequenceNumber() ... "));
	return iPtr->last_seq;
	}

EXPORT_C TUint RRtcpRRItem::InterarrivalJitter() const
	{
	__RTP_LOG(_L("RRtcpSRItem::InterarrivalJitter() ... "));
	return iPtr->jitter;
	}

EXPORT_C TUint RRtcpRRItem::LastSRTimestamp() const
	{
	__RTP_LOG(_L("RRtcpSRItem::LastSRTimeStamp() ... "));
	return iPtr->lsr;
	}

EXPORT_C TUint RRtcpRRItem::DelaySinceLastSR() const
	{
	__RTP_LOG(_L("RRtcpRRItem::DelaySinceLastSR() ... "));
	return iPtr->dlsr;
	}

EXPORT_C TUint RRtcpRRItem::SenderSSRC() const
	{
	__RTP_LOG(_L("RRtcpSRItem::SenderSSRC() ... "));
	return AboutSSRC();
	}
	

/******/
EXPORT_C RRtcpRRPart::RRtcpRRPart(TRtcpRRPart& aRR) :iRRPart(aRR)
	{}

EXPORT_C TInt RRtcpRRPart::Count() const
	{
	__RTP_LOG(_L("RRtcpRRPart::Count() ... "));
	return iRRPart.iRRItems.Count();
	}

EXPORT_C TUint RRtcpRRPart::SenderSSRC() const
	{
	__RTP_LOG(_L("RRtcpRRPart::SenderSSRC() ... "));
	return iRRPart.ssrc;
	}

EXPORT_C RRtcpRRItem RRtcpRRPart::operator[](TInt aIndex) const
	{	
	__RTP_LOG(_L("RRtcpRRItem::Operator[]() ... "));
	RRtcpRRItem item;
	item.iPtr = &iRRPart.iRRItems[aIndex];
	return item;	
	}

/******/

EXPORT_C RRtcpSRPart::RRtcpSRPart(TRtcpSRPart& aSr) :iSrPtr(aSr)
	{}

EXPORT_C void RRtcpSRPart::NTPTimestamp(TTime& aTime) const
	{
	__RTP_LOG(_L("RRtcpSRPart::NtpTimeStamp() ... "));
	//_LIT(KNTPBaseTime,"190000101:000000");
	TDateTime dateTime(1900,EJanuary,01,0,0,0,0);
	TTime ntpBaseTime(dateTime); 
	

	//TTime ntpBaseTime(KNTPBaseTime);
	aTime = (TUint64)(iSrPtr.ntp_sec * 1000000) + (TUint64) iSrPtr.ntp_frac;
	aTime = aTime.Int64() + ntpBaseTime.Int64();
	}
	
 // A New api added , for NtpTimeStamp
EXPORT_C void RRtcpSRPart::NTPTimestamp(TUint32& aSec,TUint32& aFrac) const
	{
	__RTP_LOG(_L("RRtcpSRPart::NtpTimeStamp() ... "));
	aSec = iSrPtr.ntp_sec;
 	aFrac = iSrPtr.ntp_frac;
 	}
  
EXPORT_C TUint RRtcpSRPart::RTPTimestamp() const
	{
	__RTP_LOG(_L("RRtcpSRPart::RtpTimeStamp() ... "));
	return iSrPtr.rtp_ts;
	}

EXPORT_C TUint RRtcpSRPart::PacketCount() const
	{
	__RTP_LOG(_L("RRtcpSRPart::PacketCount() ... "));
	return iSrPtr.psent;
	}

EXPORT_C TUint RRtcpSRPart::ByteCount() const
	{
	__RTP_LOG(_L("RRtcpSRPart::ByeCount() ... "));
	return iSrPtr.osent;
	}



