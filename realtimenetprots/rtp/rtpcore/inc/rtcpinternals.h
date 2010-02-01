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
// Header for class specific to SDES handling
// 
//

/**
 @file
 @internalComponent
*/

#if !defined(__RTCPINTERNALS_H__)
#define __RTCPINTERNALS_H__

#include <e32base.h>
#include "rtcppacket.h"

/*
 *	Classes and definitions specific to the Sdes types
 */
NONSHARABLE_CLASS( CSdesItem) : public CBase
	{
public:
	static CSdesItem* NewL(TUint32 aSSRCOrCSRC, const TUint8 aItemType, const TDesC8& aItemValue);
	static CSdesItem* NewL(TUint32 aSSRCOrCSRC, const TDesC8& aPrefixString, const TDesC8& aValueString);
	virtual ~CSdesItem();

	static TInt CompareSsrcAndType(const CSdesItem& aFirst, const CSdesItem& aSecond);

	TUint32 iSSRCOrCSRC;
	TUint8 iItemType;
	HBufC8* iItemValue;
	
private:
	CSdesItem(TUint32 aSSRCOrCSRC, TUint8 aItemType);
	void ConstructL(const TDesC8& aPrefixString, const TDesC8& aValueString);
	};

typedef RPointerArray<CSdesItem> TSdesItemArray;

class TSdesItemOrder : public TLinearOrder<CSdesItem>
	{
public:
	inline TSdesItemOrder() :TLinearOrder<CSdesItem>(CSdesItem::CompareSsrcAndType) {}
	};

// The RTCP packet types
class TRtcpPacket
	{
public:
	//static inline const TRtcpHeader* Header() const;

	static void WriteReportBlock(TPtr8& aRRDataPtr, CRtpSourceEntry& aEntry);
	static TUint NumberOfPacketsExpected(CRtpSourceEntry& aEntry);
	static TInt NumberOfPacketsLost(CRtpSourceEntry& aEntry);
	static TUint8 FractionOfPacketsLostInterval(CRtpSourceEntry& aEntry);

	TPtr8& iData;
protected:
	inline TRtcpPacket(TPtr8& aPtr);
	};

TRtcpPacket::TRtcpPacket(TPtr8& aPtr) :iData(aPtr) {}

/**
RR: Receiver Report RTCP Packet

        0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
header |V=2|P|    RC   |   PT=RR=201   |             length            |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                     SSRC of packet sender                     |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_1 (SSRC of first source)                 |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  1    | fraction lost |       cumulative number of packets lost       |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |           extended highest sequence number received           |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                      interarrival jitter                      |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         last SR (LSR)                         |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                   delay since last SR (DLSR)                  |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_2 (SSRC of second source)                |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  2    :                               ...                             :
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
       |                  profile-specific extensions                  |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
class TRtcpRRPacket : public TRtcpPacket
	{
public:
	TRtcpRRPacket(TPtr8& aPtr);
	void WriteToPacket(TUint32 aLocalSsrc, TUint8 aEntryCount);
	};

class TRtcpSRPacket : public TRtcpPacket
	{
public:
	TRtcpSRPacket(TPtr8& aPtr);
	void WriteToPacket(CRtpSourceEntry& aEntry, TUint32 aRtpTime, TUint32 aLocalSsrc, TUint8 aEntryCount);
	};

class TRtcpByePacket : public TRtcpPacket
	{
public:
	TRtcpByePacket(TPtr8& aPtr);

	void WriteHdrToPacket(TUint32 aSSRC);
	TInt WriteReasonToPacket(TDesC8& aDes);
	};

/*

			0                   1                   2                   3
			0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	header |V=2|P|    SC   |  PT=SDES=202  |             length            |
		   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	chunk  |                          SSRC/CSRC_1                          |
	  1    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |                           SDES items                          |
		   |                              ...                              |
		   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	chunk  |                          SSRC/CSRC_2                          |
	  2    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |                           SDES items                          |
		   |                              ...                              |
		   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*/
class TRtcpSDesPacket : public TRtcpPacket
	{
public:
	TRtcpSDesPacket(TPtr8& aPtr);
	void WriteToPacket(TSdesItemArray& aArray);
	void SetSDESFlags(TUint8 aFlags);

private:
	TInt WriteToSdesChunk(const CSdesItem& aItem, TDes8& aChunk);
	TUint8 iSDESFlags;
	};

/*
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P| subtype |   PT=APP=204  |             length            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                           SSRC/CSRC                           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                          name (ASCII)                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                   application-dependent data                ...
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
class TRtcpAppPacket : public TRtcpPacket
	{
public:
	TRtcpAppPacket(TPtr8& aPtr);
	TInt WriteToPacket(TUint32 aSSRC, const TDesC8& aName, const TDesC8& aAppData, TUint8 aSubType=0);	
	};

#endif //__RTCPINTERNALS_H__

