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
// class for base rtp controller
// 
//

/**
 @file
 @internalComponent
*/
 
#include "rtputils.h"
#include "rtpcontrollerbase.h"
#include "in_sock.h"
#include "rtpcontroller.h"
#include "rtpcontrollerdummy.h"
#include "rtcppacket.h"
#include "rtpsourceentry.h"
#include "rtppacket.h"

// RTP Constructors....

// Base Class Constructors for both Version1 
EXPORT_C CRtpControllerBase* CRtpControllerBase::NewL(RSocketServ& aServer, TSockAddr& aLocalAddr,
					   const TDesC8& aCNAME, TInt aPriority)
	{
	return  ((CRtpControllerBase *)CRtpController::NewL(aServer,aLocalAddr,aCNAME,aPriority));
	}

// ControllerV2
EXPORT_C CRtpControllerBase* CRtpControllerBase::NewL(RSocketServ& aServer, TSockAddr& aLocalAddr,
					   TInt aPriority)
	{
	return ((CRtpControllerBase *)CRtpControllerV2::NewL(aServer,aLocalAddr,aPriority));
	}
	
EXPORT_C CRtpControllerBase* CRtpControllerBase::NewL(RSocketServ& aServer, TSockAddr& aLocalAddr,
					   TUint aRtcpLocalPort, const TDesC8& aCNAME,TInt aPriority)
	{
	return ((CRtpControllerBase *)CRtpController::NewL(aServer,aLocalAddr,aRtcpLocalPort,aCNAME,aPriority));
	}
	
EXPORT_C CRtpControllerBase* CRtpControllerBase::NewL(RSocket& aRtpSocket,
										TInt aPriority)
	{
	 return ((CRtpControllerBase *)CRtpController::NewL(aRtpSocket,aPriority));
	}
	
EXPORT_C CRtpControllerBase* CRtpControllerBase::NewL(RSocket& aRtpSocket,RSocket& aRtcpSocket,
										const TDesC8& aCNAME, TInt aPriority)
	{	
	return ((CRtpControllerBase *)CRtpController::NewL(aRtpSocket,aRtcpSocket,aCNAME,aPriority));
	}

	
/**
Checks some fields of RTP fixed header for correctness: 
version, padding, payload type

@internalComponent
@return KErrNone, KErrNotSupported, KErrArgument, KErrCorrupt
@param 
@param 
@pre 
@post 
*/
TInt CRtpControllerBase::CheckRtpPacket(TDesC8& aPacket)
	{
	TInt result = KErrNone;
	TRtpFixedHdrMemLayout header(aPacket);

	__RTP_LOG(_L("CRtpController::CheckRtpPacket() ... "));
	if(KRtpVersion2 != header.Version())
		{
		result = KErrNotSupported;
		}
	else
		{
		if(1 < aPacket.Length())
			{
			// we've got at least 2 bytes thus it's save to check payload type 
			if(header.PayloadType() > KRtpMaxPayloadTypes)
				{
				result = KErrArgument;
				}
			else if(header.HasPadding())
				{
				/*
				If the P bit is set, then the last octet of the packet must
				contain a valid octet count, in particular, less than the total
				packet length minus the header size.
				*/
				// header length is fixed size + a number of CSRC (4 byte each)
				/* This rather week check but be can't do much more
				without knowing payload specifics */
				TUint headerLength = KRtpFixedNoCSRCHeaderLength + header.CC()*4;
				TPtrC8 lastOctet(aPacket.Right(1));
				if(*lastOctet.Ptr() > (aPacket.Length() - headerLength))
					result = KErrCorrupt;
				}
			}
		else
			{
			result = KErrUnderflow;
			}
		}
	__RTP_LOG1(_L("\t==> %d"), result);
	return result;
	}

/**
Checks some fields of RTCP fixed header for correctness: 
version, padding, packet type

@internalComponent
@return KErrNone, KErrNotSupported, KErrArgument, KErrCorrupt
@param 
@param 
@pre 
@post 
*/

TInt CRtpControllerBase::CheckRtcpPacket(TDesC8& aPacket, TBool& aIsBye)
	{	
	__RTP_LOG(_L("CheckRtcpPacket() ..."));
	TInt result = KErrNone;
	TRtcpFixedHdrMemLayout header(aPacket);
	if (header.PacketType() == ERtcpBYE)
		{
		aIsBye = ETrue;
		}

	if(KRtpVersion2 != header.Version())
		{
		return KErrNotSupported;
		}
	// do we have a compound RTCP?
	// so lets get size of the RTCP packet in bytes
	// The length of RTCP packet is in 32-bit words minus one
	TInt length = (header.Length()+1)*4;
	if(length < aPacket.Length())
		{
		// compound packet
		if((ERtcpSR != header.PacketType()) && (ERtcpRR != header.PacketType()))
			{
			// The payload type field of the first RTCP packet in a compound
			// packet must be equal to SR or RR.
			result = KErrCorrupt;
			}
		
		//Codenomicon defect fix for 5711-5771-rr-rc
		if (ERtcpSR == header.PacketType())
			{
			if (((header.Length() +1)*4) != (28 + (header.Count() * 6 * 4)) )
			result = KErrCorrupt;
			}
			
		if (ERtcpRR == header.PacketType())
			{
			if (((header.Length() + 1)*4) != (8 + (header.Count() * 6 * 4)) )
			result = KErrCorrupt;
			}
		//Codenomicon defect fix for 5711-5771-rr-rc
	
	
		//Codenomicon defect fix for 5711-5771-rr-rc
		if (ERtcpSR == header.PacketType())
			{
			if (((header.Length() +1)*4) > (28 + (header.Count() * 6 * 4)) )
			result = KErrCorrupt;
			}
			
		if (ERtcpRR == header.PacketType())
			{
			if (((header.Length() + 1)*4) > (8 + (header.Count() * 6 * 4)) )
			result = KErrCorrupt;
			}
		//Codenomicon defect fix for 5711-5771-rr-rc
		else if(header.IsPadding())
			{
			// The padding bit (P) should be zero for the first packet of a
			// compound RTCP packet because padding should only be applied, if it
			// is needed, to the last packet.
			result = KErrCorrupt;
			}
		else
			{
			do 
				{
				// so first packet of compound seems fine
				//TUint8* nextHeaderData = const_cast<TUint8*>(aPacket.Ptr() + length);
				//TPtr8 headerPtr(nextHeaderData, aPacket.Size()-length);
				TPtrC8 headerPtr(aPacket.Mid(length));
				TRtcpFixedHdrMemLayout nextHeader(headerPtr);
				
				/*Fix for  Codenomican 6033-6061 */
				// UnderFlow
				if (nextHeader.Length() > length)
					{
					result = KErrCorrupt;
				/*Fix for  Codenomican 6033-6061 */
					}
					
					
				if(KRtpVersion2 != nextHeader.Version())
					{
					// one of the packets is corrupted
					// lets try to recover at least some data rather the quit...
					result = KErrCorrupt;
					break;
					}

				if (nextHeader.PacketType() == ERtcpBYE)
					{
					aIsBye = ETrue;
					// Make sure that packet size and count match up
					const TInt check = (nextHeader.Length()+1)*4 - KRtcpFixedHdrSize - 4*nextHeader.Count();
					if (check < 0)
						{
						result = KErrCorrupt;
						break;
						}
					}
				
				//lets keep a cumulative lenght
				length += (nextHeader.Length()+1)*4;
				}
			while (length < aPacket.Length());

			if(length != aPacket.Length())
				result = KErrCorrupt;
			}
		}
	else if (length == aPacket.Length())
		{
		if (ERtcpAPP != header.PacketType())
			{
			result  = KErrCorrupt;
			}
		}
	/*Fix for  Codenomican 6033-6061 */
	else if (length > aPacket.Length())
		{
		// underFlow
		result = KErrCorrupt;
			/*Fix for  Codenomican 6033-6061 */
		
		}
	__RTP_LOG1(_L("\t==> %d"), result);
	return result;
	}

void CRtpControllerBase::SetNonRtpData(TBool aValue)
	{
	iEnableNonRtpData = aValue;
	}

void CRtpControllerBase::SetNonRtcpData(TBool aValue)
	{
	iEnableNonRtcpData = aValue;
	}
