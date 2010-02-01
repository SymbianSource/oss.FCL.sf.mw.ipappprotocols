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
// class for handling rtcp packets
// 
//

/**
 @file
 @internalComponent
*/

#include <e32base.h>

#include "rtcppacket.h"
#include "rtpsourceentry.h"
#include "rtcpinternals.h"


const static TInt KPrefixLength = 1;

void TRtcpPacket::WriteReportBlock(TPtr8& aRRDataPtr, CRtpSourceEntry& aEntry)
	{
	/*
        0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
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

	*/
	// we have the 'right' descriptor here thus offset zero
	TRtcpReportBlockMemLayout rrBlock(aRRDataPtr,0);
	rrBlock.SetSSRC(aEntry.SRC());
	rrBlock.SetFraction(FractionOfPacketsLostInterval(aEntry));
	rrBlock.SetCumulativeLost(NumberOfPacketsLost(aEntry));
	// extended highest sequence number received: 32 bits
    // The low 16 bits contain the highest sequence number received in an
    // RTP data packet from source SSRC_n, and the most significant 16
    // bits extend that sequence number with the corresponding count of
    // sequence number cycles.
	// we need just 16 bits from cycles but in fact it's stored (acording to RFC)
	// as 32 bits... hmmm
	rrBlock.SetSequenceHigh(aEntry.MaxSequenceNo(),TUint16(aEntry.ShiftedCycles()>>16));
	rrBlock.SetJitter(aEntry.Jitter() >> 4);
	rrBlock.SetLastSR(aEntry.LastSRTimestamp());
	TNTPTime currentTime(0,0);
	TInt lastDelay = 0;
	// if no SR received yet then set to zero
	if(0 != aEntry.WhenLastSRReceived())
		{
		if(KErrNone == RtpUtils::GetNTPTime(currentTime))
			lastDelay = currentTime.GetCompactTime() - aEntry.WhenLastSRReceived();
		}
	rrBlock.SetLastDelay(lastDelay);
	}

TUint TRtcpPacket::NumberOfPacketsExpected(CRtpSourceEntry& aEntry)
	{	
	//
	//A.3 Determining Number of Packets Expected and Lost
	//
	/* Both the received packet count and the count of cycles
	are maintained in appropriate states in the state machine...
	as defined in Appendix A.1 of the RFC. */
	TUint extendedMax = aEntry.ShiftedCycles() + aEntry.MaxSequenceNo();
	// why + 1 here? It's recommended in Appendix A.3 of the RFC
	// as result we always expect at least 1 packet... seems fine but results ALWAYS
	// in at least one lost packet...may be no big deal with high numbers 
	// certainly looks 'weird' a the beginning of reports...
    return extendedMax - aEntry.BaseSequenceNo() + 1;
	}

TInt TRtcpPacket::NumberOfPacketsLost(CRtpSourceEntry& aEntry)
	{	
	//
	//A.3 Determining Number of Packets Expected and Lost
	//
	/* Both the received packet count and the count of cycles
	are maintained in appropriate states in the state machine...
	as defined in Appendix A.1 of the RFC. */
	/* from RFC 3550: The number of packets lost is defined to be the number of packets
	expected less the number of packets actually received */

    TInt lost =  NumberOfPacketsExpected(aEntry) - aEntry.PacketsReceived();
	
	/* from RFC 3550: Since this signed number is carried in 24 bits, it should be clamped
	at 0x7fffff for positive loss or 0x800000 for negative loss rather than wrapping around. */
	if(0 > lost)
		{
		lost &= 0x800000;
		}	
	else
		{
		lost &= 0x7fffff;
		}
	return lost;
	}

/** The fraction of packets lost during the last reporting interval
	(since the previous SR or RR packet was sent)
NOTE: This should be called only when actually sending RTCP packet as it updates ALL
		interval related internal fields in the CRtpSSRCEntry
@see CRtpSSRCEntry
@internalComponent
@param aEntry The data related to a particular source as identified by it's SSRC
@return The fraction lost as 8-bit fixed point number with the binary point at the left edge.
*/
TUint8 TRtcpPacket::FractionOfPacketsLostInterval(CRtpSourceEntry& aEntry)
	{	
	//
	//A.3 Determining Number of Packets Expected and Lost
	//
	/* Both the received packet count and the count of cycles
	are maintained in appropriate states in the state machine...
	as defined in Appendix A.1 of the RFC. */
	/* from RFC3550: The fraction of packets lost during the last reporting interval
	(since the previous SR or RR packet was sent) is calculated from differences in the expected 
	and received packet counts across the interval, where expected_prior and received_prior are the values
	saved when the previous reception report was generated */
	TUint expected = NumberOfPacketsExpected(aEntry);
	TInt expectedInterval =  expected - aEntry.ExpectedPrior();
	aEntry.SetExpectedPrior(expected);

    TUint receivedInterval = aEntry.PacketsReceived() - aEntry.ReceivedPrior();
	aEntry.SetReceivedPrior(aEntry.PacketsReceived());
  
    TInt lostInterval = expectedInterval - receivedInterval;

	TUint8 fraction = 0;
	if (!(0 == expectedInterval || 0 >= lostInterval))  
		{
		fraction = static_cast<TUint8>((lostInterval << 8) / expectedInterval);
		}
	//The resulting fraction is an 8-bit fixed point number with the binary point at the left edge.
	return fraction;
	}

/**
extended highest sequence number received: 32 bits
      The low 16 bits contain the highest sequence number received in an
      RTP data packet from source SSRC_n, and the most significant 16
      bits extend that sequence number with the corresponding count of
      sequence number cycles, which may be maintained according to the
      algorithm in Appendix A.1.  Note that different receivers within
      the same session will generate different extensions to the
      sequence number if their start times differ significantly.

*/
//TRtcpPacket::ExtendedSequenceNumberReceived(CRtpSSRCEntry& aEntry)
//	{
//	/* from RFC3550:
//	*/
//	}

//>>>>>>>>>>>>>>>>>>
TRtcpRRPacket::TRtcpRRPacket(TPtr8& aPtr) : TRtcpPacket(aPtr)
	{
	__ASSERT_ALWAYS(aPtr.MaxLength()>KRtcpRRHeaderSize + KRtcpRRBlockSize,Panic(ERtcpBufferToSmall));
	iData.SetLength(KRtcpRRHeaderSize);
	}

void TRtcpRRPacket::WriteToPacket(TUint32 aLocalSsrc, TUint8 aEntryCount)
	{	
	/* RR
			0                   1                   2                   3
			0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	header |V=2|P|    RC   |   PT=RR=201   |             length            |
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |                     SSRC of packet sender                     |
		   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	*/
	// lets write data to the packet itself
	TRtcpFixedHdrMemLayout headerInPacket(iData);
	headerInPacket.SetVersion2();
	headerInPacket.SetPadding(EFalse);
	headerInPacket.SetCount((TUint8)(aEntryCount));
	headerInPacket.SetPacketType(ERtcpRR);
	// length: 16 bits
    // The length of this RTCP packet in 32-bit words minus one,
    // including the header and any padding. 
	headerInPacket.SetLength(static_cast<TUint16>(KRtcpRRHeaderSize/4+(KRtcpRRBlockSize*aEntryCount)/4-1));
	headerInPacket.SetSSRC(aLocalSsrc);
	}

//>>>>>>>>>>>>>>>>>>
TRtcpSRPacket::TRtcpSRPacket(TPtr8& aPtr) : TRtcpPacket(aPtr)
	{
	__ASSERT_ALWAYS(aPtr.MaxLength()>KRtcpSRHeaderSize + 
				KRtcpSRSenderInfoSize +
				KRtcpRRBlockSize,Panic(ERtcpBufferToSmall));
	
	iData.SetLength(KRtcpSRHeaderSize + KRtcpSRSenderInfoSize);
	}

void TRtcpSRPacket::WriteToPacket(CRtpSourceEntry& aEntry, TUint32 aRtpTime, TUint32 aLocalSsrc, TUint8 aEntryCount)
	{	
	/* SR
			0                   1                   2                   3
			0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	header |V=2|P|    RC   |   PT=SR=200   |             length            |
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |                         SSRC of sender                        |
		   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	*/
	// lets write data to the packet itself
	TRtcpFixedHdrMemLayout headerInPacket(iData);
	headerInPacket.SetVersion2();
	headerInPacket.SetPadding(EFalse);
	// sending just one RR at the time
	headerInPacket.SetCount((TUint8)(aEntryCount));
	headerInPacket.SetPacketType(ERtcpSR);
	// length: 16 bits
    // The length of this RTCP packet in 32-bit words minus one,
    // including the header and any padding. 
	headerInPacket.SetLength (static_cast<TUint16>(KRtcpSRHeaderSize/4
							+KRtcpSRSenderInfoSize/4
							+(KRtcpRRBlockSize*aEntryCount)/4-1));
	headerInPacket.SetSSRC(aLocalSsrc);
	// SR block
	/*
			0                   1                   2                   3
			0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	sender |              NTP timestamp, most significant word             |
	info   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |             NTP timestamp, least significant word             |
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |                         RTP timestamp                         |
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |                     sender's packet count                     |
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |                      sender's octet count                     |
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
	*/
	TRtcpSRSenderInfoMemLayout senderInfo(iData, KRtcpSRHeaderSize);
	TUint32 seconds;
	TUint32 useconds;
	// we don't have to check for return error as the NTP is set to zero in case of any error
	RtpUtils::GetNTPTime(seconds,useconds);
	senderInfo.SetNtpMsb(seconds);
	senderInfo.SetNtpLsb(useconds);
	//RTP time
	senderInfo.SetRtpTimeStamp(aRtpTime);
	senderInfo.SetPacketCount(aEntry.PacketsSent());
	senderInfo.SetOctetCount(aEntry.OctetsSent());
	}

//>>>>>>>>>>>>>>>>>>
TRtcpByePacket::TRtcpByePacket(TPtr8& aPtr) : TRtcpPacket(aPtr)
	{
	iData.SetLength(KRtcpByeFixedSize);
	}
	
void TRtcpByePacket::WriteHdrToPacket(TUint32 aLocalSsrc)
	{	
	/*
			 0                   1                   2                   3
		   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		  |V=2|P|    SC   |   PT=BYE=203  |             length            |
		  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		  |                           SSRC/CSRC                           |
		  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		  :                              ...                              :
		  +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	(opt) |     length    |               reason for leaving            ...
		  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	
	SDES/BYE	source count (SC): 5 bits
      The number of SSRC/CSRC chunks contained in this SDES packet.  A
      value of zero is valid but useless.

	
	*/
	// lets write data to the packet itself
	__ASSERT_ALWAYS(iData.MaxLength()>= KRtcpByeFixedSize,Panic(ERtcpBufferToSmall));
	TRtcpFixedHdrMemLayout headerInPacket(iData);
	headerInPacket.SetVersion2();
	headerInPacket.SetPadding(EFalse);
	// sending BYE just for this SSRCEntry
	headerInPacket.SetCount(1);
	headerInPacket.SetPacketType(ERtcpBYE);
	headerInPacket.SetSSRC(aLocalSsrc);
	// length: 16 bits
    // The length of this RTCP packet in 32-bit words minus one,
    // we expect only 1 SSRC and no reason text...might be updated later though! 
	headerInPacket.SetLength(KRtcpByeFixedSize/4 + KRtcpRRBlockSize/4 -1);
	}

TInt TRtcpByePacket::WriteReasonToPacket(TDesC8& aReason)
	{	
	/*
			 0                   1                   2                   3
		   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		  +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	(opt) |     length    |               reason for leaving            ...
		  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	*/
	/* Reasons for RTCP BYE packets (must end on 32-bit boundary)
	we could do zero-padding here but seems easier to extend with spaces 
	From RFC "If the string fills the packet
	to the next 32-bit boundary, the string is not null terminated.  If
	not, the BYE packet MUST be padded with null octets to the next 32-
	bit boundary. "
	*/

	if(aReason.Length() == 0)
		{
		TRtcpFixedHdrMemLayout headerInPacket(iData);
		//if there is no aReason, the length of bye packet is ONE (except in the case
		//of MIXER or TRANSLATRO that we are any way not supporting here)
		headerInPacket.SetLength(static_cast<TUint16>(1));
		return KErrNone;
		}

	__ASSERT_ALWAYS(aReason.Length()<256,Panic(ERtcpByePacketNotPadded));
	TUint8 length = static_cast<TUint8>(aReason.Length());
	iData.Append(&length,1);

	// we add 1 for length itself
	length += 1;
	
	//we check length of text (+ 1 byte for length) for 32-bit boundary
	TUint8 reminder = static_cast<TUint8>(length % 4);
	if (0 != reminder)
		{
		length = static_cast<TUint8>(length + 4 - reminder);
		}
	
	iData.Append(aReason);
	if(reminder)
		{
		iData.AppendFill(0,4-reminder);
		}
	// we need to update length  of the packet itself!
	TRtcpFixedHdrMemLayout headerInPacket(iData);
	// see  WriteHdrToPacket for more info
	headerInPacket.SetLength(static_cast<TUint16>(iData.Length()/4-1));
	return KErrNone;
	}

//>>>>>>>>>>>>>>>>>>
TRtcpSDesPacket::TRtcpSDesPacket(TPtr8& aPtr) : TRtcpPacket(aPtr)
	{
	iData.SetLength(KRtcpSDesFixedSize);
	iSDESFlags =0;
	}

void TRtcpSDesPacket::SetSDESFlags(TUint8 aFlags)
	{
	iSDESFlags = aFlags | 1;
	}
/*
 *	We get a list of sdes items sorted by SSRC
 */
void TRtcpSDesPacket::WriteToPacket(TSdesItemArray& aArray)
	{	
	/* SDES
				0                   1                   2                   3
			0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	header |V=2|P|    SC   |  PT=SDES=202  |             length            |
		   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	*/
	CSdesItem* item = NULL;
	TUint32 oldSsrc = 0;
	TInt len, chunkCount = 0;
	for(TInt count = 0; count < aArray.Count(); count++)
		{
		// Extract the first item in the array
		item = aArray[count];
		oldSsrc = item->iSSRCOrCSRC;
		// write this ssrc to the packet
		TUint32 Swapped = TRtcpMemLayout::Swap32(oldSsrc);
		iData.Append(reinterpret_cast<TUint8*>(&Swapped), 4);
		chunkCount++;
		// While the ssrc of the next one is same as the last one
		len = 0;
		for(;count < aArray.Count(); count++)
			{
			item = aArray[count];
			if (oldSsrc == item->iSSRCOrCSRC)
				{
				if (iSDESFlags != 0)
					{
					if(item->iItemType && (0x1<<(item->iItemType-1) & iSDESFlags))
						{
						len += WriteToSdesChunk(*item, iData);
						}
					if(!item->iItemType)
						{
						len += WriteToSdesChunk(*item, iData);
						}
					}
					else
					{
						// write this item to the chunk
						len += WriteToSdesChunk(*item, iData);
					}
				}
			else
				{
				count--;
				break;
				}
			}
		if((len % 4))
		    {
		    // Append the null charecters
		    iData.AppendFill('\0', 4 - (len % 4));
		    }
		}
	TRtcpFixedHdrMemLayout headerInPacket(iData);
	headerInPacket.SetVersion2();
	headerInPacket.SetPadding(EFalse);
	headerInPacket.SetCount(static_cast<TUint8>(chunkCount));
	headerInPacket.SetPacketType(ERtcpSDES); //202
	// length: 16 bits
	// The length of this RTCP packet in 32-bit words minus one,
	// including the header and any padding. 
	headerInPacket.SetLength( static_cast<TUint16>(	iData.Length()/4 - 1 ));
	}

TInt TRtcpSDesPacket::WriteToSdesChunk(const CSdesItem& aItem, TDes8& aChunk)
	{	
	const TInt oldLen = aChunk.Length();
	TRtcpSdesItemMemLayout item(aChunk, oldLen);
	item.SetAttribute(aItem.iItemType, *aItem.iItemValue);
	TInt len;
	if(aItem.iItemType)
	    {
	    len = aItem.iItemValue->Length() + KRtcpSDesItemFixedSize; // For the Type & length
	    }
	else
	    {
	    len = 1;
	    }
	aChunk.SetLength(oldLen + len);
	return len;
	}

//>>>>>>>>>>>>>>>>>>
TRtcpAppPacket::TRtcpAppPacket(TPtr8& aPtr) : TRtcpPacket(aPtr)
	{
	iData.SetLength(KRtcpAppFixedSize);
	}

TInt TRtcpAppPacket::WriteToPacket(TUint32 aSSRC, const TDesC8& aName, const TDesC8& aAppData, TUint8 aSubType)
	{	
	__ASSERT_DEBUG(aName.Length()==4,Panic(ERtcpInvalidAppName));
	__ASSERT_ALWAYS(aAppData.Length()<KRtcpDataDefaultSize-12,Panic(ERtcpBufferToSmall));

	__ASSERT_DEBUG(iData.MaxLength()>= KRtcpAppFixedSize,Panic(ERtcpBufferToSmall));
	iData.Append(aName);
	//add aAppData now
	iData.Append(aAppData);
	TInt remainder = aAppData.Length() % 4;
	iData.AppendFill(0, 4 - remainder);

	TRtcpFixedHdrMemLayout headerInPacket(iData);
	headerInPacket.SetVersion2();
	/*
	subtype: 5 bits
      May be used as a subtype to allow a set of APP packets to be
      defined under one unique name, or for any application-dependent
      data.
	*/
	headerInPacket.SetCount(aSubType);
	headerInPacket.SetPacketType(ERtcpAPP);
	headerInPacket.SetSSRC(aSSRC);

	//Update the length of the APP packet now
	headerInPacket.SetLength(static_cast<TUint16>(iData.Length()/4-1));
	headerInPacket.SetPadding(EFalse);
	return KErrNone;	
	}

//>>>>>>>>>>>>>>>>>>
CSdesItem* CSdesItem::NewL(TUint32 aSSRCOrCSRC, const TUint8 aItemType, const TDesC8& aItemValue)
	{
    /* For adding PRIV data use the other overloaded NewL() */
	if(aItemType > EsdesNOTE)
		{
		User::Leave(KErrArgument);
		}

	CSdesItem* self = new(ELeave) CSdesItem(aSSRCOrCSRC, aItemType);
	CleanupStack::PushL(self);
	self->iItemValue = aItemValue.AllocL();
	CleanupStack::Pop();
	return self;
	}

//PRIV support
CSdesItem* CSdesItem::NewL(TUint32 aSSRCOrCSRC, const TDesC8& aPrefixString, const TDesC8& aValueString)
	{	
	CSdesItem* self = new(ELeave) CSdesItem(aSSRCOrCSRC, EsdesPRIV);
	CleanupStack::PushL(self);
	self->ConstructL(aPrefixString, aValueString);
	CleanupStack::Pop();
	return self;
	}

void CSdesItem::ConstructL(const TDesC8& aPrefixString, const TDesC8& aValueString)
	{	
	iItemValue = HBufC8::NewL(KPrefixLength+aPrefixString.Length()+aValueString.Length());
	TPtr8 t = iItemValue->Des();
	TChar ch(aPrefixString.Length());
	t.Append(ch); //NOW...after fix
	t.Append(aPrefixString);
	t.Append(aValueString); 
	}

CSdesItem::CSdesItem(TUint32 aSSRCOrCSRC, TUint8 aItemType)
	:iSSRCOrCSRC(aSSRCOrCSRC), iItemType(aItemType), iItemValue(NULL)
	{}

CSdesItem::~CSdesItem()
	{
	delete iItemValue;
	}

TInt CSdesItem::CompareSsrcAndType(const CSdesItem& aFirst, const CSdesItem& aSecond)
	{
	/* aFirst is the Item being Inserted. aSecond is the Element in array
	 * against which aFirst is being compared */	
	if (aFirst.iSSRCOrCSRC < aSecond.iSSRCOrCSRC)
		{
		return -1;
		}
	else if(aFirst.iSSRCOrCSRC > aSecond.iSSRCOrCSRC)
		{
		return -1;
		}
	// If ssrc is equal, compare itemTypes
	else if (aSecond.iItemType == aFirst.iItemType)
		{
		// They are the same
		return 0;
		}
	else if (aFirst.iItemType == EsdesNONE)
		{
		// We want None to be at the end. So keep on returning
		// 1 till we reach the End of Array. 
		return 1;
		}
	else if(aFirst.iItemType == EsdesCNAME) // Opposite, as we need CNAME at the beginning
		{
		// CNAME at the beginning, and rest in the middle
		return -1;
		}
	else if(aSecond.iItemType == EsdesNONE)
		{
		// We reached the end of the Array(Because we hit NONE). So insert before
		return -1;
		}
    /* Do we need to Check for the Validity of Types Here?. The Validity is
       already verified when an SDES is created. So let go .. */
	return (aFirst.iItemType - aSecond.iItemType);
	}
