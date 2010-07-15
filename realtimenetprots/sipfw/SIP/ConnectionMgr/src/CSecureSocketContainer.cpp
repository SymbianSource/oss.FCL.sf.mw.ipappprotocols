// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : CSecureSocketContainer.cpp
// Part of     : ConnectionMgr
// Version     : SIP/5.0
//



#include <securesocket.h>

#include "CSecureSocketContainer.h"
 
// -----------------------------------------------------------------------------
// CSecureSocketContainer::NewLC
// -----------------------------------------------------------------------------
//
CSecureSocketContainer* CSecureSocketContainer::NewL( RSocket& aSocket )
    {
    CSecureSocketContainer* self = NewLC( aSocket );
    CleanupStack::Pop( self );
    return self;
    }
      
// -----------------------------------------------------------------------------
// CSecureSocketContainer::NewLC
// -----------------------------------------------------------------------------
//
CSecureSocketContainer* CSecureSocketContainer::NewLC( RSocket& aSocket )
    {
    CSecureSocketContainer* self = 
        new ( ELeave ) CSecureSocketContainer( aSocket );
    CleanupStack::PushL( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CSecureSocketContainer::~CSecureSocketContainer
// -----------------------------------------------------------------------------
//
CSecureSocketContainer::~CSecureSocketContainer()
    {
    }

// -----------------------------------------------------------------------------
// CSecureSocketContainer::Send
// -----------------------------------------------------------------------------
// 
void CSecureSocketContainer::Send( 
    const TDesC8& aDesc, 
    TUint aFlags, 
    TRequestStatus& aStatus )
    {
    if ( HasSecureSocket() )
        {
        iSecureSocket->Send( aDesc, aStatus );
        }
    else
        {
        CSocketContainer::Send( aDesc, aFlags, aStatus );
        }
    }

// -----------------------------------------------------------------------------
// CSecureSocketContainer::CancelSend
// -----------------------------------------------------------------------------
//         
void CSecureSocketContainer::CancelSend()
    {
    if ( HasSecureSocket() )
        {
        iSecureSocket->CancelSend();
        }
    else
        {
        CSocketContainer::CancelSend();
        }
    }

// -----------------------------------------------------------------------------
// CSecureSocketContainer::RecvOneOrMore
// -----------------------------------------------------------------------------
//         
void CSecureSocketContainer::RecvOneOrMore( 
    TDes8& aDes, 
    TUint aFlags, 
    TRequestStatus& aStatus, 
    TSockXfrLength& aLen )
    {
    if ( HasSecureSocket() )
        {
        iSecureSocket->RecvOneOrMore( aDes, aStatus, aLen );
        }
    else
        {
        CSocketContainer::RecvOneOrMore( aDes, aFlags, aStatus, aLen );
        }
    }

// -----------------------------------------------------------------------------
// CSecureSocketContainer::CancelRecv
// -----------------------------------------------------------------------------
//                             
void CSecureSocketContainer::CancelRecv()
    {
    if ( HasSecureSocket() )
        {
        iSecureSocket->CancelRecv();
        }
    else
        {
        CSocketContainer::CancelRecv();
        }
    }

// -----------------------------------------------------------------------------
// CSecureSocketContainer::Connect
// -----------------------------------------------------------------------------
//         
void CSecureSocketContainer::Connect( 
    TSockAddr& aAddr, 
    TRequestStatus& aStatus )
    {
    if ( HasSecureSocket() )
        {
        // Since secure socket is opened on top of connected tcp, 
        // connect will initiate handshake.
        iSecureSocket->StartClientHandshake( aStatus );
        }
    else
        {
        CSocketContainer::Connect( aAddr, aStatus );
        }
    }

// -----------------------------------------------------------------------------
// CSecureSocketContainer::CancelConnect
// -----------------------------------------------------------------------------
//         
void CSecureSocketContainer::CancelConnect()
    {
    if ( HasSecureSocket() )
        {
        // Connect method call initiated handshake if CSecureSocket existed.
        iSecureSocket->CancelHandshake();
        }
    else
        {
        CSocketContainer::CancelConnect();
        }
    }

// -----------------------------------------------------------------------------
// CSecureSocketContainer::RemoteName
// -----------------------------------------------------------------------------
//         
void CSecureSocketContainer::RemoteName( TSockAddr& aAddr )
    {
    CSocketContainer::RemoteName( aAddr );
    }

// -----------------------------------------------------------------------------
// CSecureSocketContainer::SetOpt
// -----------------------------------------------------------------------------
//         
TInt CSecureSocketContainer::SetOpt( 
    TUint aOptionName, 
    TUint aOptionLevel, 
    const TDesC8& aOption )
    {
    if ( HasSecureSocket() )
        {
        return iSecureSocket->SetOpt( aOptionName, aOptionLevel, aOption );
        }
    return CSocketContainer::SetOpt( aOptionName, aOptionLevel, aOption );
    }

// -----------------------------------------------------------------------------
// CSecureSocketContainer::SetOpt
// -----------------------------------------------------------------------------
//         
TInt CSecureSocketContainer::SetOpt( 
    TUint aOptionName, 
    TUint aOptionLevel, 
    TInt aOption )
    {
    if ( HasSecureSocket() )
        {
        return iSecureSocket->SetOpt( aOptionName, aOptionLevel, aOption );
        }
    return CSocketContainer::SetOpt( aOptionName, aOptionLevel, aOption );
    }
    
// -----------------------------------------------------------------------------
// CSecureSocketContainer::HasSecureSocket
// -----------------------------------------------------------------------------
//  
TBool CSecureSocketContainer::HasSecureSocket() const
    {
    return ( iSecureSocket != 0 );
    }

// -----------------------------------------------------------------------------
// CSecureSocketContainer::SecureSocket
// -----------------------------------------------------------------------------
//          
CSecureSocket* CSecureSocketContainer::SecureSocket()
    {
    return iSecureSocket;
    }
        
// -----------------------------------------------------------------------------
// CSecureSocketContainer::SetSecureSocket
// -----------------------------------------------------------------------------
//    
void CSecureSocketContainer::SetSecureSocket( CSecureSocket* aSecureSocket )
    {
    iSecureSocket = aSecureSocket;
    }

// -----------------------------------------------------------------------------
// CSecureSocketContainer::CSecureSocketContainer
// -----------------------------------------------------------------------------
//    
CSecureSocketContainer::CSecureSocketContainer( RSocket& aSocket ) : 
    CSocketContainer( aSocket )
    {
    }

// -----------------------------------------------------------------------------
// CSecureSocketContainer::CancelAll
// -----------------------------------------------------------------------------
// 
void CSecureSocketContainer::CancelAll()
    {
    if ( HasSecureSocket() )
        {        
        iSecureSocket->CancelAll();
        }
    else
        {
        CSocketContainer::CancelAll();
        }
    }

// End of file
