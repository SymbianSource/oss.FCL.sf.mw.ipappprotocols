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
// Name          : CIcmpV4Receiver.cpp
// Part of       : ConnectionMgr
// Version       : SIP/4.0
// Destination Unreachable Message:
// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// |     Type      |     Code      |          Checksum             |
// |                             unused                            |
// |      Internet Header + 64 bits of Original Data Datagram      |
// Internet header:
// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// |Version|  IHL  |Type of Service|          Total Length         |
// |         Identification        |Flags|      Fragment Offset    |
// |  Time to Live |    Protocol   |         Header Checksum       |
// |                       Source Address                          |
// |                    Destination Address                        |
// |                    Options                    |    Padding    |
// User datagram:
// 0      7 8     15 16    23 24    31
// |     Source      |   Destination   |
// |      Port       |      Port       |
// |     Length      |    Checksum     |
// |          data octets ...
// In the code the data is located in bytes.
//




#include <ip4_hdr.h>
#include <icmp6_hdr.h> // This is identical to ICMPv4 header
#include <udp_hdr.h>
#include <in_sock.h>
#include "CIcmpV4Receiver.h"
#include "MIcmpReceiver.h"
#include "CommonConsts.h"
#include "SipLogs.h"


// -----------------------------------------------------------------------------
// CIcmpV4Receiver::NewL
// -----------------------------------------------------------------------------
//
CIcmpV4Receiver* CIcmpV4Receiver::NewL(MIcmpErrorObserver& aObserver,
									   RSocketServ& aServer)
	{
	CIcmpV4Receiver* self = new (ELeave) CIcmpV4Receiver();
	CleanupStack::PushL(self);
	self->ConstructL(aObserver, aServer);
	CleanupStack::Pop();
	return self;
	}

// -----------------------------------------------------------------------------
// CIcmpV4Receiver::CIcmpV4Receiver
// -----------------------------------------------------------------------------
//
CIcmpV4Receiver::CIcmpV4Receiver() : CActive( CActive::EPriorityStandard )
	{
	}

// -----------------------------------------------------------------------------
// CIcmpV4Receiver::ConstructL
// -----------------------------------------------------------------------------
//
void CIcmpV4Receiver::ConstructL(MIcmpErrorObserver& aObserver,
								 RSocketServ& aServer)
	{
	CActiveScheduler::Add(this);
	iObserver = &aObserver;
	iServer = &aServer;

	// Start
	User::LeaveIfError(iSocket.Open(*iServer, KAfInet, KSockDatagram,
									KProtocolInetIcmp));
	Receive();
	}

// -----------------------------------------------------------------------------
// CIcmpV4Receiver::~CIcmpV4Receiver
// -----------------------------------------------------------------------------
//
CIcmpV4Receiver::~CIcmpV4Receiver()
	{
	Cancel();
	iSocket.Close();
	}

// -----------------------------------------------------------------------------
// CIcmpV4Receiver::RunL
// -----------------------------------------------------------------------------
//
void CIcmpV4Receiver::RunL()
	{
	if (iStatus == KErrNone)
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
// CIcmpV4Receiver::CheckError
// -----------------------------------------------------------------------------
//
void CIcmpV4Receiver::CheckError()
	{
	const TUint KDestinationUnreachable = 3;
	
	// Pointer to ICMP header
	TInet6HeaderICMP* icmpPtr = ( TInet6HeaderICMP* ) iData.Ptr();

	// Pointer to IPv4 header
	TInet6HeaderIP4* ipv4Ptr = ( TInet6HeaderIP4* )( iData.Ptr() + 8 );

	// Error message type
	TUint32 error = icmpPtr->Type();
	if ( error == KDestinationUnreachable )
		{
		// Check that protocol is UDP
		TUint32 protocol = ipv4Ptr->Protocol();
		if ( protocol == KProtocolInetUdp )
			{
			// Pointer to UDP data
			TInet6HeaderUDP* udpPtr = ( TInet6HeaderUDP* )( ipv4Ptr->EndPtr() );

			// Get the destination port number
			iAddress.SetPort( udpPtr->DstPort() );

			// Set the destination address
			iAddress.SetAddress( ipv4Ptr->DstAddr() );

			__SIP_INT_LOG1( "ICMP error", error )
            __SIP_ADDR_LOG( "ICMP error address", iAddress )
			
	        // Error message to observer
			iObserver->IcmpError( iAddress );
			}
		}
	}

// -----------------------------------------------------------------------------
// CIcmpV4Receiver::DoCancel
// -----------------------------------------------------------------------------
//
void CIcmpV4Receiver::DoCancel()
	{
	iSocket.CancelRecv();
	}

// -----------------------------------------------------------------------------
// CIcmpV4Receiver::Receive
// -----------------------------------------------------------------------------
//
void CIcmpV4Receiver::Receive()
	{
	if(!IsActive())
		{
		iSocket.Recv( iData, 0, iStatus );
		SetActive();
		}
	}
