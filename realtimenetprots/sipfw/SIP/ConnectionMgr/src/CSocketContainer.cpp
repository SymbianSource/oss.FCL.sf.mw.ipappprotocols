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
// Name        : CSocketContainer.cpp
// Part of     : ConnectionMgr
// Version     : SIP/5.0
//



#include "CSocketContainer.h"
 
// -----------------------------------------------------------------------------
// CSocketContainer::NewLC
// -----------------------------------------------------------------------------
//
CSocketContainer* CSocketContainer::NewL( RSocket& aSocket )
    {
    CSocketContainer* self = NewLC( aSocket );
    CleanupStack::Pop( self );
    return self;
    }
      
// -----------------------------------------------------------------------------
// CSocketContainer::NewLC
// -----------------------------------------------------------------------------
//
CSocketContainer* CSocketContainer::NewLC( RSocket& aSocket )
    {
    CSocketContainer* self = 
        new ( ELeave ) CSocketContainer( aSocket );
    CleanupStack::PushL( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CSocketContainer::~CSocketContainer
// -----------------------------------------------------------------------------
//
CSocketContainer::~CSocketContainer()
    {
    }

// -----------------------------------------------------------------------------
// CSocketContainer::Send
// -----------------------------------------------------------------------------
// 
void CSocketContainer::Send( 
    const TDesC8& aDesc, 
    TUint aFlags, 
    TRequestStatus& aStatus )
    {
    iSocket.Send( aDesc, aFlags, aStatus );
    }

// -----------------------------------------------------------------------------
// CSocketContainer::SendTo
// -----------------------------------------------------------------------------
//                    
void CSocketContainer::SendTo( 
    const TDesC8& aDesc,
    TSockAddr& aAddr,
    TUint aFlags,
    TRequestStatus& aStatus )
    {
    iSocket.SendTo( aDesc, aAddr, aFlags, aStatus );
    }

// -----------------------------------------------------------------------------
// CSocketContainer::CancelSend
// -----------------------------------------------------------------------------
//         
void CSocketContainer::CancelSend()
    {
    iSocket.CancelSend();
    }

// -----------------------------------------------------------------------------
// CSocketContainer::RecvOneOrMore
// -----------------------------------------------------------------------------
//         
void CSocketContainer::RecvOneOrMore( 
    TDes8& aDes, 
    TUint aFlags, 
    TRequestStatus& aStatus, 
    TSockXfrLength& aLen )
    {
    iSocket.RecvOneOrMore( aDes, aFlags, aStatus, aLen );
    }

// -----------------------------------------------------------------------------
// CSocketContainer::CancelRecv
// -----------------------------------------------------------------------------
//                             
void CSocketContainer::CancelRecv()
    {
    iSocket.CancelRecv();
    }

// -----------------------------------------------------------------------------
// CSocketContainer::Connect
// -----------------------------------------------------------------------------
//         
void CSocketContainer::Connect( TSockAddr& aAddr, TRequestStatus& aStatus )
    {
    iSocket.Connect( aAddr, aStatus );
    }

// -----------------------------------------------------------------------------
// CSocketContainer::CancelConnect
// -----------------------------------------------------------------------------
//         
void CSocketContainer::CancelConnect()
    {
    iSocket.CancelConnect();
    }

// -----------------------------------------------------------------------------
// CSocketContainer::RemoteName
// -----------------------------------------------------------------------------
//         
void CSocketContainer::RemoteName( TSockAddr& aAddr )
    {
    iSocket.RemoteName( aAddr );
    }

// -----------------------------------------------------------------------------
// CSocketContainer::SetOpt
// -----------------------------------------------------------------------------
//         
TInt CSocketContainer::SetOpt( 
    TUint aOptionName, 
    TUint aOptionLevel, 
    const TDesC8& aOption )
    {
    return iSocket.SetOpt( aOptionName, aOptionLevel, aOption );
    }

// -----------------------------------------------------------------------------
// CSocketContainer::SetOpt
// -----------------------------------------------------------------------------
//         
TInt CSocketContainer::SetOpt( 
    TUint aOptionName, 
    TUint aOptionLevel, 
    TInt aOption )
    {
    return iSocket.SetOpt( aOptionName, aOptionLevel, aOption );
    }

// -----------------------------------------------------------------------------
// CSocketContainer::HasSecureSocket
// -----------------------------------------------------------------------------
//  
TBool CSocketContainer::HasSecureSocket() const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSocketContainer::SecureSocket
// -----------------------------------------------------------------------------
//          
CSecureSocket* CSocketContainer::SecureSocket()
    {
    return 0;
    }
        
// -----------------------------------------------------------------------------
// CSocketContainer::Socket
// -----------------------------------------------------------------------------
//
RSocket& CSocketContainer::Socket()
    {
    return iSocket;
    }
    
// -----------------------------------------------------------------------------
// CSocketContainer::CSocketContainer
// -----------------------------------------------------------------------------
//    
CSocketContainer::CSocketContainer( RSocket& aSocket ) : iSocket( aSocket )
    {
    }

// -----------------------------------------------------------------------------
// CSocketContainer::CancelAll
// -----------------------------------------------------------------------------
//                             
void CSocketContainer::CancelAll()
    {
    iSocket.CancelAll();
    }

// End of file
