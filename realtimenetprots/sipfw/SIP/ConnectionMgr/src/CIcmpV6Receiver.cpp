// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CIcmpV6Receiver.cpp
// Part of       : ConnectionMgr
// Version       : SIP/4.0
// Destination Unreachable Message:
// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// |     Type      |     Code      |          Checksum             |
// |                             Unused                            |
// |                    As much of invoking packet                 |
// +                as will fit without the ICMPv6 packet          +
// |                exceeding the minimum IPv6 MTU [IPv6]          |
// IPv6 Header Format:
// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// |Version| Traffic Class |           Flow Label                  |
// |         Payload Length        |  Next Header  |   Hop Limit   |
// +                         Source Address                        +
// +                      Destination Address                      +
// Extension Header Format:
// |  Next Header  |  Hdr Ext Len  |                               |
// .             type-specific data (variable length)              .
// User datagram:
// 0      7 8     15 16    23 24    31
// |     Source      |   Destination   |
// |      Port       |      Port       |
// |     Length      |    Checksum     |
// |          data octets ...
// In the code the data is located in 8-bit pieces.
//




#include <ip6_hdr.h>
#include <icmp6_hdr.h>
#include <udp_hdr.h>
#include <ext_hdr.h>
#include <in_sock.h>
#include "CIcmpV6Receiver.h"
#include "MIcmpReceiver.h"
#include "CSipConnection.h"
#include "CommonConsts.h"
#include "SipLogs.h"


// -----------------------------------------------------------------------------
// CIcmpV6Receiver::NewL
// -----------------------------------------------------------------------------
//
CIcmpV6Receiver* CIcmpV6Receiver::NewL(MIcmpErrorObserver& aObserver,
									   RSocketServ& aServer)
	{
	CIcmpV6Receiver* self = new (ELeave) CIcmpV6Receiver();
	CleanupStack::PushL(self);
	self->ConstructL(aObserver, aServer);
	CleanupStack::Pop();
	return self;
	}

// -----------------------------------------------------------------------------
// CIcmpV6Receiver::CIcmpV6Receiver
// -----------------------------------------------------------------------------
//
CIcmpV6Receiver::CIcmpV6Receiver() : CActive( CActive::EPriorityStandard )
	{
	}

// -----------------------------------------------------------------------------
// CIcmpV6Receiver::ConstructL
// -----------------------------------------------------------------------------
//
void CIcmpV6Receiver::ConstructL(MIcmpErrorObserver& aObserver,
								 RSocketServ& aServer)
	{
	CActiveScheduler::Add(this);
	iObserver = &aObserver;
	iServer = &aServer;

	// Start
	User::LeaveIfError(iSocket.Open(*iServer, KAfInet, KSockDatagram,
									KProtocolInet6Icmp));
	Receive();
	}

// -----------------------------------------------------------------------------
// CIcmpV6Receiver::~CIcmpV6Receiver
// -----------------------------------------------------------------------------
//
CIcmpV6Receiver::~CIcmpV6Receiver()
	{
	Cancel();
	iSocket.Close();
	}

// -----------------------------------------------------------------------------
// CIcmpV6Receiver::RunL
// -----------------------------------------------------------------------------
//
void CIcmpV6Receiver::RunL()
	{
	if(iStatus == KErrNone)
		{
		CheckError();
		Receive();
		}
	}

// Disabled PC-Lint warning for "Suspicious pointer-to-pointer conversion
// (area too small))". It was caused by TUin8* to TInet6xxx typecasts and could
// not be avoided otherwise.
/*lint -e826 */

// -----------------------------------------------------------------------------
// CIcmpV6Receiver::CheckError
// -----------------------------------------------------------------------------
//
void CIcmpV6Receiver::CheckError()
	{
	const TUint KDestinationUnreachable = 1;
	
	// Pointer to ICMP header
	TInet6HeaderICMP* icmpPtr = ( TInet6HeaderICMP* ) iData.Ptr();

	// Pointer to IPv6 header
	TInet6HeaderIP* ipv6Ptr = ( TInet6HeaderIP* )( iData.Ptr() + 8 );
	// Get the whole length of the header in bytes
	TUint32 payloadLength = ipv6Ptr->PayloadLength();
	TUint32 offset = 0;

	// Pointer to IPv6 extension header
	TInet6HeaderExtension* extPtr =
		( TInet6HeaderExtension* )( iData.Ptr() + 14 );
	// Get the extension header length in bytes
	TUint32 extHdrLength = extPtr->HeaderLength();

	// Pointer to UDP data
	TInet6HeaderUDP* udpPtr = ( TInet6HeaderUDP* )( ipv6Ptr->EndPtr() );
				
	// Error message type
	TUint32 error = icmpPtr->Type();
	if( error == KDestinationUnreachable )
		{
		TUint32 nextHeader = 0;
		do  // Search the correct header, if there are extension headers
			{
			nextHeader = extPtr->NextHeader();
			if( nextHeader == KProtocolInetUdp )
				{
				// Get the destination port number
				iAddress.SetPort( udpPtr->DstPort() );

				// Set the destination address
				iAddress.SetAddress( ipv6Ptr->DstAddr() );

			    __SIP_INT_LOG1( "ICMP error", error )
                __SIP_ADDR_LOG( "ICMP error address", iAddress )

				iObserver->IcmpError( iAddress );
				}
			else
				{
				// Step to next header
				if( offset < payloadLength )
					{
					extPtr = extPtr + extHdrLength;
					offset = offset + extHdrLength;
					}
				}
			// Loop until UDP datagram is found. If it is not found,
			// get out of here and do nothing.
			} while ((nextHeader != KProtocolInetUdp) && (offset < payloadLength));
		}
	}

// -----------------------------------------------------------------------------
// CIcmpV6Receiver::DoCancel
// -----------------------------------------------------------------------------
//
void CIcmpV6Receiver::DoCancel()
	{
	iSocket.CancelRecv();
	}

// -----------------------------------------------------------------------------
// CIcmpV6Receiver::Receive
// -----------------------------------------------------------------------------
//
void CIcmpV6Receiver::Receive()
	{
	if(!IsActive())
		{
		iSocket.Recv( iData, 0, iStatus );
		SetActive();
		}
	}
