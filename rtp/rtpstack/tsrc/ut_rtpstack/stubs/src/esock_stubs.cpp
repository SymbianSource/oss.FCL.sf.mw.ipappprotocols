/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    
*
*/




#include <es_sock.h>
#include <in_sock.h>
#include <e32base.h>
const TUint KMaxRtpPacketSize = 8192;
// Local helper functions:
LOCAL_C TInt SimulateAllocFailure(TInt aError=KErrNone);

LOCAL_C void CompleteAsyncRequest(TRequestStatus& aStatus, TInt aError=KErrNone);

LOCAL_C void SimulateAllocFailureL()
    {
    HBufC* tmp = HBufC::NewL(10);
    delete tmp;
    }    
    
LOCAL_C TInt SimulateAllocFailure(TInt aError)
    {
    TInt err = aError;
    if (err == KErrNone)
        {
        TRAP(err, SimulateAllocFailureL());
        }
    return err;
    }

LOCAL_C void CompleteAsyncRequest(TRequestStatus& aStatus, TInt aError)
    {
    TInt err = SimulateAllocFailure(aError);
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err); 
    }


// TSockAddr

// -----------------------------------------------------------------------------
// TSockAddr::TSockAddr
// -----------------------------------------------------------------------------
//
TSockAddr::TSockAddr()
 : TBuf8<KMaxSockAddrSize>()
	{
	SetLength(KMaxSockAddrSize);
	SetFamily(KAFUnspec);   
    }

// -----------------------------------------------------------------------------
// TSockAddr::TSockAddr
// -----------------------------------------------------------------------------
//
TSockAddr::TSockAddr(TUint aFamily)
	: TBuf8<KMaxSockAddrSize>()
	{
	SetLength(KMaxSockAddrSize);
	SetFamily(aFamily);
    }

// -----------------------------------------------------------------------------
// TSockAddr::Family
// -----------------------------------------------------------------------------
//
TUint TSockAddr::Family() const
    {
    return BasePtr()->iFamily;
    }

// -----------------------------------------------------------------------------
// TSockAddr::SetFamily
// -----------------------------------------------------------------------------
//
void TSockAddr::SetFamily(TUint aFamily)
    {
	BasePtr()->iFamily = aFamily;
    }

// -----------------------------------------------------------------------------
// TSockAddr::Port
// -----------------------------------------------------------------------------
//
TUint TSockAddr::Port() const
    {
    return BasePtr()->iPort;
    }

// -----------------------------------------------------------------------------
// TSockAddr::SetPort
// -----------------------------------------------------------------------------
//
void TSockAddr::SetPort(TUint aPort)
    {
    BasePtr()->iPort = aPort;
    }

// -----------------------------------------------------------------------------
// TSockAddr::CmpPort
// -----------------------------------------------------------------------------
//
TBool TSockAddr::CmpPort(const TSockAddr& anAddr) const
    {
	return (BasePtr()->iFamily==anAddr.BasePtr()->iFamily) 
		&& (BasePtr()->iPort==anAddr.BasePtr()->iPort);
    }

// -----------------------------------------------------------------------------
// TSockAddr::SetUserLen
// -----------------------------------------------------------------------------
//
void TSockAddr::SetUserLen(TInt aLen)
	{
	SetLength(aLen+Align4(sizeof(SSockAddr)));
	}

// -----------------------------------------------------------------------------
// TSockAddr::GetUserLen
// -----------------------------------------------------------------------------
//
TInt TSockAddr::GetUserLen()
	{
	return Length()-Align4(sizeof(SSockAddr));
	}



// RSocketServ:

// -----------------------------------------------------------------------------
// RSocketServ::RSocketServ
// -----------------------------------------------------------------------------
//
RSocketServ::RSocketServ()
: iNextError( KErrNone ) 
    {
    }
    
// -----------------------------------------------------------------------------
// RSocketServ::Connect
// -----------------------------------------------------------------------------
//    
TInt RSocketServ::Connect(TUint /*aMessageSlots*/)
    {
    if ( iNextError == KErrNotReady )
    	{
    	return KErrNotReady;
    	}
    return SimulateAllocFailure();
    }



// RConnection:

// -----------------------------------------------------------------------------
// RConnection::RConnection
// -----------------------------------------------------------------------------
//
RConnection::RConnection() 
 : iNewISPId(0), iNextError( KErrNone )
	{
	}
	
// -----------------------------------------------------------------------------
// RConnection::~RConnection
// -----------------------------------------------------------------------------
//
RConnection::~RConnection()
	{
	}	
	
// -----------------------------------------------------------------------------
// RConnection::Open
// -----------------------------------------------------------------------------
//
TInt RConnection::Open(
    RSocketServ& /*aSocketServer*/, 
    TUint /*aConnectionType*/) 
	{
	if ( iNextError == KErrNotReady )
		{
		return KErrNotReady;
		}
	return SimulateAllocFailure();
	}	

// -----------------------------------------------------------------------------
// RConnection::Close
// -----------------------------------------------------------------------------
//
void RConnection::Close()
    {
    }

// -----------------------------------------------------------------------------
// RConnection::Start
// -----------------------------------------------------------------------------
//
void RConnection::Start(TConnPref& /*aPref*/, TRequestStatus& aStatus)
    {
    CompleteAsyncRequest(aStatus);
    }
    
// -----------------------------------------------------------------------------
// RConnection::Start
// -----------------------------------------------------------------------------
//
TInt RConnection::Start(TConnPref& /*aPref*/)
    {
    return SimulateAllocFailure();    
    }

// -----------------------------------------------------------------------------
// RConnection::ProgressNotification
// -----------------------------------------------------------------------------
//
void RConnection::ProgressNotification(
    TNifProgressBuf& aProgress,
    TRequestStatus& aStatus,
    TUint /*aSelectedProgress*/)
    {
    aProgress().iStage = KErrNone;
    CompleteAsyncRequest(aStatus);
    }

// -----------------------------------------------------------------------------
// RConnection::CancelProgressNotification
// -----------------------------------------------------------------------------
//
void RConnection::CancelProgressNotification()
    {
    }

// -----------------------------------------------------------------------------
// RConnection::Stop
// -----------------------------------------------------------------------------
//
TInt RConnection::Stop()
	{
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// RConnection::EmueratConnection
// -----------------------------------------------------------------------------
//
TInt RConnection::EnumerateConnections(TUint& /*aCount*/ )
	{
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// RConnection::GetConnectionInfo
// -----------------------------------------------------------------------------
//
TInt RConnection::GetConnectionInfo(TUint /*aIndex*/, TDes8& /*aConnectionInfo*/ )
	{
	return KErrNone;
	}
// RSocket:

// -----------------------------------------------------------------------------
// RSocket::RSocket
// -----------------------------------------------------------------------------
//
RSocket::RSocket()
 : iNextError( KErrNone ),
   iLocalAddrResolved(EFalse),
   iLocalPort(0)
	{
	}

// -----------------------------------------------------------------------------
// RSocket::Open
// -----------------------------------------------------------------------------
//	
TInt RSocket::Open(
    RSocketServ& /*aServer*/,
    TUint /*addrFamily*/,
    TUint /*sockType*/,
    TUint /*protocol*/)
    {
    return SimulateAllocFailure();
    }

// -----------------------------------------------------------------------------
// RSocket::Open
// -----------------------------------------------------------------------------
//	
TInt RSocket::Open(
    RSocketServ& /*aServer*/,
    TUint /*addrFamily*/,
    TUint /*sockType*/,
    TUint /*protocol*/,
    RConnection& /*aConnection*/)
    {
    return SimulateAllocFailure();
    }

// -----------------------------------------------------------------------------
// RSocket::Open
// -----------------------------------------------------------------------------
//
TInt RSocket::Open(RSocketServ& /*aServer*/)
    {
    return SimulateAllocFailure();
    }

// -----------------------------------------------------------------------------
// RSocket::Close
// -----------------------------------------------------------------------------
//
void RSocket::Close()
    {
    }

// -----------------------------------------------------------------------------
// RSocket::Send
// -----------------------------------------------------------------------------
//
void RSocket::Send(
    const TDesC8& /*aDesc*/,
    TUint /*someFlags*/,
    TRequestStatus& aStatus)
    {
    CompleteAsyncRequest(aStatus);   
    }

// -----------------------------------------------------------------------------
// RSocket::CancelSend
// -----------------------------------------------------------------------------
//
void RSocket::CancelSend()
    {
    }

// -----------------------------------------------------------------------------
// RSocket::CancelRead
// -----------------------------------------------------------------------------
//
void RSocket::CancelRead()
	{
	}

// -----------------------------------------------------------------------------
// RSocket::Recv
// -----------------------------------------------------------------------------
//
void RSocket::Recv(
    TDes8& /*aDesc*/,
    TUint /*flags*/,
    TRequestStatus& aStatus)
    {
    CompleteAsyncRequest(aStatus);
    }

// -----------------------------------------------------------------------------
// RSocket::RecvOneOrMore
// -----------------------------------------------------------------------------
//
void RSocket::RecvOneOrMore(
    TDes8& /*aDesc*/,
    TUint /*flags*/,
    TRequestStatus& aStatus,
    TSockXfrLength& /*aLen*/)
    {
    CompleteAsyncRequest(aStatus);
    }

// -----------------------------------------------------------------------------
// RSocket::CancelRead
// -----------------------------------------------------------------------------
//
void RSocket::RecvFrom(TDes8& /*aDesc*/,
						TSockAddr& /*anAddr*/,
						TUint /*flags*/,
						TRequestStatus& aStatus)
	{
	CompleteAsyncRequest(aStatus);
	}
// -----------------------------------------------------------------------------
// RSocket::CancelRecv
// -----------------------------------------------------------------------------
//
void RSocket::CancelRecv()
    {
    }

// -----------------------------------------------------------------------------
// RSocket::SendTo
// -----------------------------------------------------------------------------
//
void RSocket::SendTo(
    const TDesC8& aDesc,
    TSockAddr& /*anAddr*/,
    TUint /*flags*/,
    TRequestStatus& aStatus)
    {
    if ( aDesc.Length() && aDesc.Length() >= KMaxRtpPacketSize + 1 )
    	{
    	TRequestStatus* status = &aStatus;
		User::RequestComplete( status, KErrTooBig ); 
    	}
    else
    	{
    	CompleteAsyncRequest(aStatus);
    	}
    }

// -----------------------------------------------------------------------------
// RSocket::Connect
// -----------------------------------------------------------------------------
//
void RSocket::Connect(
    TSockAddr& /*anAddr*/,
    TRequestStatus& aStatus)
    {
    CompleteAsyncRequest(aStatus);
    }

// -----------------------------------------------------------------------------
// RSocket::CancelConnect
// -----------------------------------------------------------------------------
//
void RSocket::CancelConnect()
    {
    }

// -----------------------------------------------------------------------------
// RSocket::Bind
// -----------------------------------------------------------------------------
//
TInt RSocket::Bind(TSockAddr& anAddr)
    {
    iLocalPort = anAddr.Port();
    return SimulateAllocFailure();
    }

// -----------------------------------------------------------------------------
// RSocket::SetOpt
// -----------------------------------------------------------------------------
//
TInt RSocket::SetOpt(
    TUint /*anOptionName*/,
    TUint /*anOptionLevel*/,
    const TDesC8& /*anOption*/)
    {
    return SimulateAllocFailure();
    }

// -----------------------------------------------------------------------------
// RSocket::Accept
// -----------------------------------------------------------------------------
//
void RSocket::Accept(
    RSocket& /*aBlankSocket*/,
    TRequestStatus& aStatus)
    {
    CompleteAsyncRequest(aStatus);
    }

// -----------------------------------------------------------------------------
// RSocket::CancelAccept
// -----------------------------------------------------------------------------
//
void RSocket::CancelAccept()
    {
    }

// -----------------------------------------------------------------------------
// RSocket::Listen
// -----------------------------------------------------------------------------
//
TInt RSocket::Listen(TUint /*qSize*/)
    {
    return SimulateAllocFailure();
    }

// -----------------------------------------------------------------------------
// RSocket::SetOpt
// -----------------------------------------------------------------------------
//
TInt RSocket::SetOpt(
    TUint /*anOptionName*/,
    TUint /*anOptionLevel*/,
    TInt /*anOption*/)
    {
    return SimulateAllocFailure();
    }

// -----------------------------------------------------------------------------
// RSocket::GetOpt
// -----------------------------------------------------------------------------
//
TInt RSocket::GetOpt(
    TUint anOptionName,
    TUint anOptionLevel,
    TDes8& anOption)
    {
    TInt err = SimulateAllocFailure();
    if (err == KErrNone)
        {
        if (iLocalAddrResolved)
            {
            // See CLocalAddrResolver
            iLocalAddrResolved = EFalse;
            return KErrNotFound;
            }

	    if (anOptionName == KSoInetNextInterface && 
	        anOptionLevel == KSolInetIfCtrl)
	        {
	        TPckgBuf<TSoInetInterfaceInfo>& infoBuf =
	            reinterpret_cast<TPckgBuf<TSoInetInterfaceInfo>&>(anOption);    
	        infoBuf().iAddress.SetAddress(INET_ADDR(1,2,3,4));
	        }

        if (anOptionName == KSoInetIfQueryByName && 
            anOptionLevel == KSolInetIfQuery)
			{
			// Local address being resolved
			TPckgBuf<TSoInetIfQuery>& queryBuf = 
			    reinterpret_cast<TPckgBuf<TSoInetIfQuery>&>(anOption);
			queryBuf().iZone[1] = 1; // IAP id
			iLocalAddrResolved = ETrue;
			}
        }
    return err;
    }

// -----------------------------------------------------------------------------
// RSocket::SetLocalPort
// -----------------------------------------------------------------------------
//
TInt RSocket::SetLocalPort(TInt aPort)
    {
    TInt err = SimulateAllocFailure();
    if (err == KErrNone)
        {
        if (aPort <= 0)
            {
            return KErrArgument;
            }
        iLocalPort = aPort;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// RSocket::LocalPort
// -----------------------------------------------------------------------------
//
TUint RSocket::LocalPort()
    {
    return iLocalPort;
    }

// -----------------------------------------------------------------------------
// RSocket::RemoteName
// -----------------------------------------------------------------------------
//
void RSocket::RemoteName(TSockAddr& anAddr)
    {
    TInetAddr addr;
    addr.SetAddress(INET_ADDR(1,2,3,4));
    anAddr = addr;
    }

// -----------------------------------------------------------------------------
// RSocket::CancelAll
// -----------------------------------------------------------------------------
// 
void RSocket::CancelAll()
    {
    }

// -----------------------------------------------------------------------------
// RSocket::Info
// -----------------------------------------------------------------------------
// 
TInt RSocket::Info(TProtocolDesc& aProtocol)
	{
	aProtocol.iAddrFamily = KAFUnspec;
    aProtocol.iByteOrder = EBigEndian;
    aProtocol.iMessageSize = KSocketMessageSizeIsStream;
    //aProtocol.iName = NULL ;
   	aProtocol.iNamingServices = KNSNameResolution;
    aProtocol.iProtocol = KProtocolInetUdp;
    aProtocol.iSecurity = KSocketNoSecurity;
    aProtocol.iServiceInfo = KSockDatagram;
    aProtocol.iSockType = KSockDatagram; 
    return KErrNone;     
	}

// ByteOrder:

// -----------------------------------------------------------------------------
// ByteOrder::Swap32
// -----------------------------------------------------------------------------
//
TUint32 ByteOrder::Swap32(TUint32 aVal)
	{
	return ((aVal>>24)&0xff) | ((aVal>>8)&0xff00) | 
	        ((aVal<<8)&0xff0000) | ((aVal<<24)&0xff000000);
	}

// -----------------------------------------------------------------------------
// ByteOrder::Swap16
// -----------------------------------------------------------------------------
//
TUint16 ByteOrder::Swap16(TUint16 aVal)
	{
	return ((aVal>>8) & 0xff) | ((aVal<<8) & 0xff00);
	}



// BigEndian:

// -----------------------------------------------------------------------------
// BigEndian::Get32
// -----------------------------------------------------------------------------
//
TUint32 BigEndian::Get32(const TUint8 *aPtr)
	{
	return (aPtr[0]<<24) | (aPtr[1]<<16) | (aPtr[2]<<8) | aPtr[3];
	}

// -----------------------------------------------------------------------------
// BigEndian::Put32
// -----------------------------------------------------------------------------
//
void BigEndian::Put32(TUint8 *aPtr, TUint32 aVal)
	{
	aPtr[0] = aVal >> 24;
	aPtr[1] = (aVal >> 16) & 0xff;
	aPtr[2] = (aVal >> 8) & 0xff;
	aPtr[3] = aVal & 0xff;
	}

// -----------------------------------------------------------------------------
// BigEndian::Get16
// -----------------------------------------------------------------------------
//
TUint16 BigEndian::Get16(const TUint8 *aPtr)
	{
	return (aPtr[0]<<8) | aPtr[1];
	}

// -----------------------------------------------------------------------------
// BigEndian::Put16
// -----------------------------------------------------------------------------
//
void BigEndian::Put16(TUint8 *aPtr, TUint16 aVal)
	{
	aPtr[0] = aVal >> 8;
	aPtr[1] = aVal & 0xff;
	}  
