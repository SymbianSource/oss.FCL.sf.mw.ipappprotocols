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
// rtcp.cpp
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
	ASSERT(0);
	return 0;
	}

EXPORT_C TUint RRtcpRRItem::AboutSSRC() const
	{
	ASSERT(0);
	return 0;
	}

EXPORT_C TUint RRtcpRRItem::FractionLost() const
	{
	ASSERT(0);
	return 0;
	}

EXPORT_C TUint RRtcpRRItem::ExtendedHighestSequenceNumber() const
	{
	ASSERT(0);
	return 0;
	}

EXPORT_C TUint RRtcpRRItem::InterarrivalJitter() const
	{
	ASSERT(0);
	return 0;
	}

EXPORT_C TUint RRtcpRRItem::LastSRTimestamp() const
	{
	ASSERT(0);
	return 0;
	}

EXPORT_C TUint RRtcpRRItem::DelaySinceLastSR() const
	{
	ASSERT(0);
	return 0;
	}

EXPORT_C TUint RRtcpRRItem::SenderSSRC() const
	{
	ASSERT(0);
	return 0;
	}

/******/
EXPORT_C RRtcpRRPart::RRtcpRRPart(TRtcpRRPart& aRR) :iRRPart(aRR)
	{}

EXPORT_C TInt RRtcpRRPart::Count() const
	{
	ASSERT(0);
	return 0;
	}

EXPORT_C TUint RRtcpRRPart::SenderSSRC() const
	{
	ASSERT(0);
	return 0;
	}

EXPORT_C RRtcpRRItem RRtcpRRPart::operator[](TInt /*aIndex*/) const
	{
	ASSERT(0);
	RRtcpRRItem item;
	return item;	
	}

/******/

EXPORT_C RRtcpSRPart::RRtcpSRPart(TRtcpSRPart& aSr) :iSrPtr(aSr)
	{}

EXPORT_C void RRtcpSRPart::NTPTimestamp(TTime& /*aTime*/) const
	{
	ASSERT(0);
	}
	
 // A New api added , for NtpTimeStamp
EXPORT_C void RRtcpSRPart::NTPTimestamp(TUint32& ,TUint32&) const
 	{
 	ASSERT(0);
  	}
  
EXPORT_C TUint RRtcpSRPart::RTPTimestamp() const
	{
	ASSERT(0);
	return 0;
	}

EXPORT_C TUint RRtcpSRPart::PacketCount() const
	{
	ASSERT(0);
	return 0;
	}

EXPORT_C TUint RRtcpSRPart::ByteCount() const
	{
	ASSERT(0);
	return 0;
	}



