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
// RTP Packet handling helper functions.
// 
//

/**
 @file
 @internalComponent
*/
 
#if !defined(__RTPPACKET_H__)
#define __RTPPACKET_H__

#include <e32std.h>
#include <e32base.h>

/**
@internalTechnology 
*/
class TRtpFixedHdrMemLayout
	{
public:
	inline TRtpFixedHdrMemLayout(TDesC8& packet);
	inline void SetVersion(TUint aVersion);
	inline void SetVersion2();
	inline void SetCSRCCount(TUint aCC);
	inline void SetSequenceNumber(TUint16 aNo);
	inline void SetSSRC(TUint32 aSSRC);
	inline void SetMarker(TBool aMarker);
	inline void SetPadding(TBool aPadding);
	inline void SetExtension(TBool aExtension);
	inline void SetPayloadType(TUint aPayloadType);
	inline void SetTimeStamp(TUint32 aTime);

	inline TUint32 SSRC();
	inline TUint CC();
	inline TBool HasMarker();
	inline TBool HasPadding();
	inline TBool HasExtension();
	inline TUint PayloadType();
	inline TUint Version();
	inline TUint16 SequenceNumber();
	inline TUint32 TimeStamp();
	inline TUint8* CSRCs();
private:
	TRtpFixedHdrMemLayout();
	TUint8* iFirstOctet;
	};

/** Offsets of header fields in a RTP packet.
Offsets in number of octets, first octet with offset zero.
@internalTechnology
*/
const TUint KRtpMarkerOffset = 1;
const TUint KRtpPayloadTypeOffset = KRtpMarkerOffset;
const TUint KRtpSequenceNoOffset = 2;
const TUint KRtpTimeStampOffset = 4;
const TUint KRtpSSRCOffset = 8;
const TUint KRtpCSRCOffset = 12;	

#include "rtppacket.inl"

#endif	//__RTPPACKET_H__

