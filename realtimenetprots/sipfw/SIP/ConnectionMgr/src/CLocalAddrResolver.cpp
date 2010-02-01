// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CLocalAddrResolver.cpp
// Part of       : ConnectionMgr
// Version       : SIP/5.0 
//




#include "CLocalAddrResolver.h"
#include "SipLogs.h"

// -----------------------------------------------------------------------------
// CLocalAddrResolver::NewL
// -----------------------------------------------------------------------------
//
CLocalAddrResolver* CLocalAddrResolver::NewL(
    RSocketServ& aServer,
    RConnection& aConnection )
	{
	CLocalAddrResolver* self = NewLC( aServer, aConnection );
	CleanupStack::Pop();
	return self;	
	}

// -----------------------------------------------------------------------------
// CLocalAddrResolver::NewLC
// -----------------------------------------------------------------------------
//
CLocalAddrResolver* CLocalAddrResolver::NewLC(
    RSocketServ& aServer,
    RConnection& aConnection )
	{
	CLocalAddrResolver* self = 
	    new( ELeave ) CLocalAddrResolver( aServer, aConnection );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;	
	}

// -----------------------------------------------------------------------------
// CLocalAddrResolver::CLocalAddrResolver
// -----------------------------------------------------------------------------
//
CLocalAddrResolver::CLocalAddrResolver(
    RSocketServ& aServer,
    RConnection& aConnection )
	: iServer( aServer ),
	  iConnection( aConnection )
	{
	}

// -----------------------------------------------------------------------------
// CLocalAddrResolver::~CLocalAddrResolver
// -----------------------------------------------------------------------------
//
CLocalAddrResolver::~CLocalAddrResolver()
	{
	}

// -----------------------------------------------------------------------------
// CLocalAddrResolver::ConstructL
// -----------------------------------------------------------------------------
//
void CLocalAddrResolver::ConstructL()
	{
	_LIT( KIap, "IAP\\Id" );
	// The following is done to make local IP address lookup work in a case
	// where there are several IAPs used which have different IDs but all
	// point to the same physical AP. For each CSipConnection instance, their
	// iLocalAddrResolver->iIapId will be the correct IAP's ID after the
	// following function call is complete.
	User::LeaveIfError( iConnection.GetIntSetting( KIap, iIapId ) );
	}

// -----------------------------------------------------------------------------
// CLocalAddrResolver::Addr
// -----------------------------------------------------------------------------
//
const TInetAddr& CLocalAddrResolver::IPv6Addr() const
    {
    return iIPv6Addr;
    }

// -----------------------------------------------------------------------------
// CLocalAddrResolver::HasIPv4Addr
// -----------------------------------------------------------------------------
//
TBool CLocalAddrResolver::HasIPv4Addr() const
    {
    return ( !iIPv4Addr.IsUnspecified() );
    }

// -----------------------------------------------------------------------------
// CLocalAddrResolver::IPv4Addr
// -----------------------------------------------------------------------------
//	
const TInetAddr& CLocalAddrResolver::IPv4Addr() const
    {
    return iIPv4Addr;
    }

// -----------------------------------------------------------------------------
// CLocalAddrResolver::IsLocalAddr
// -----------------------------------------------------------------------------
//
TBool CLocalAddrResolver::IsLocalAddr(const TInetAddr& aAddr) const
    {
    return ( aAddr.CmpAddr( iIPv6Addr ) || aAddr.CmpAddr( iIPv4Addr ) );
    }

// -----------------------------------------------------------------------------
// CLocalAddrResolver::RefreshLocalAddressesL
// -----------------------------------------------------------------------------
//
void CLocalAddrResolver::RefreshLocalAddressesL()
	{	
	RSocket socket;
	User::LeaveIfError( socket.Open(
	    iServer, KAfInet, KSockDatagram, KProtocolInetUdp, iConnection ) );

	iIPv6Addr.Init( KAFUnspec );
	iIPv4Addr.Init( KAFUnspec );
	
    TInt err = socket.SetOpt( KSoInetEnumInterfaces, KSolInetIfCtrl );
	if ( err == KErrNone )
		{
		TPckgBuf<TSoInetInterfaceInfo> info;
		err = socket.GetOpt( KSoInetNextInterface, KSolInetIfCtrl, info );
		while ( err == KErrNone )
			{
			TPckgBuf<TSoInetIfQuery> query;
			query().iName = info().iName;
			err = socket.GetOpt( KSoInetIfQueryByName, KSolInetIfQuery, query );
			if ( err == KErrNone )
				{
				if ( Match(info(), query() ) )
				    {
                    SetAddress( info().iAddress );
				    }
				}
		    err = socket.GetOpt( KSoInetNextInterface, KSolInetIfCtrl, info );
			}
		}
	socket.Close();
	if ( err == KErrNoMemory )
	    {
	    User::LeaveNoMemory();
	    }
	}	

// -----------------------------------------------------------------------------
// CLocalAddrResolver::Match
// -----------------------------------------------------------------------------
//
TBool CLocalAddrResolver::Match(
    const TSoInetInterfaceInfo& aInfo,
    const TSoInetIfQuery& aQuery ) const
    {
    TBool match = EFalse;
    if (!aInfo.iAddress.IsUnspecified() && 
        !aInfo.iAddress.IsLoopback() &&
        !aInfo.iAddress.IsLinkLocal() &&
        aQuery.iZone[1] == iIapId )
        {
        match = ETrue;
        }
    return match;
    }

// -----------------------------------------------------------------------------
// CLocalAddrResolver::SetAddress
// -----------------------------------------------------------------------------
//
void CLocalAddrResolver::SetAddress( const TInetAddr& aAddr )
    {    
    if ( aAddr.Address() != 0 )
        {
        iIPv4Addr = aAddr;     
        iIPv4Addr.ConvertToV4();
        }
    else
        {
        iIPv6Addr = aAddr;    
        }         
    }
