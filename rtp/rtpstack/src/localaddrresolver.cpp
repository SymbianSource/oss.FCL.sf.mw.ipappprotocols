/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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





#include "localaddrresolver.h"




// -----------------------------------------------------------------------------
// CLocalAddrResolver::NewL
// -----------------------------------------------------------------------------
//
CLocalAddrResolver* CLocalAddrResolver::NewL( RSocketServ& aServer )
	{
	CLocalAddrResolver* self = NewLC( aServer );
	CleanupStack::Pop();
	return self;	
	}

// -----------------------------------------------------------------------------
// CLocalAddrResolver::NewLC
// -----------------------------------------------------------------------------
//
CLocalAddrResolver* CLocalAddrResolver::NewLC( RSocketServ& aServer )
	{
	CLocalAddrResolver* self = new (ELeave)CLocalAddrResolver(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}

// -----------------------------------------------------------------------------
// CLocalAddrResolver::ConstructL
// -----------------------------------------------------------------------------
//
void CLocalAddrResolver::ConstructL()
	{
	}


// -----------------------------------------------------------------------------
// CLocalAddrResolver::SetAddr
// -----------------------------------------------------------------------------
//
void CLocalAddrResolver::SetAddr( TInetAddr& aTarget, TInetAddr& aSource )
    {
    if ( aSource.Family() == KAfInet6 )
        {
        if(!aSource.IsLinkLocal())
			{
			aTarget = aSource;
			aTarget.SetScope(0);
			}
        }
    else
        {
        aTarget = aSource;
        aTarget.SetScope(0);
        }
    }

// -----------------------------------------------------------------------------
// CLocalAddrResolver::CheckAndSetAddr
// -----------------------------------------------------------------------------
//
void CLocalAddrResolver::CheckAndSetAddr( TInetAddr& aTarget,
                                          TInetAddr& aCandidate,
                                          TUint32 aCandidateIap,
				                          TUint32 aSpecifiedIap )
    {
    if ( !aCandidate.IsUnspecified() && !aCandidate.IsLoopback() )
        {
        if ( aCandidate.IsV4Mapped())
            {
            aCandidate.ConvertToV4();
            }
        if ( aCandidateIap == aSpecifiedIap )
            {
            SetAddr( aTarget, aCandidate );
            }
        }
    }				                          

// -----------------------------------------------------------------------------
// CLocalAddrResolver::GetLocalAddrL
// -----------------------------------------------------------------------------
//
void CLocalAddrResolver::GetLocalAddrL(TInetAddr& aAddr, TUint32 aIap)
	{
	RSocket socket;
	User::LeaveIfError( socket.Open( *iServer, KAfInet, KSockDatagram, 
		                             KProtocolInetUdp ) );

	if (socket.SetOpt( KSoInetEnumInterfaces, KSolInetIfCtrl ) == KErrNone )
		{
		TPckgBuf<TSoInetInterfaceInfo> opt;
		
		while ( socket.GetOpt( KSoInetNextInterface, KSolInetIfCtrl, opt ) == KErrNone)
			{
			TPckgBuf<TSoInetIfQuery> optifquery;
			optifquery().iName = opt().iName;
		
			if(socket.GetOpt( KSoInetIfQueryByName, KSolInetIfQuery, optifquery ) == KErrNone)
				{
				CheckAndSetAddr( aAddr, ( TInetAddr& )opt().iAddress,
				                 optifquery().iZone[1], aIap );
				}
			}
		}
		
	socket.Close();
	}


// -----------------------------------------------------------------------------
// CLocalAddrResolver::~CLocalAddrResolver
// -----------------------------------------------------------------------------
//
CLocalAddrResolver::~CLocalAddrResolver()
	{
	}

// -----------------------------------------------------------------------------
// CLocalAddrResolver::CLocalAddrResolver
// -----------------------------------------------------------------------------
//
CLocalAddrResolver::CLocalAddrResolver(RSocketServ& aServer) : 
	iServer(&aServer)
	{
	}
