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
// Name        : TOutgoingRequestQueueItem.cpp
// Part of     : ConnectionMgr
// Version     : SIP/5.0
//



#include "TOutgoingRequestQueueItem.h"
#include "TSIPTransportParams.h"
#include <in_sock.h>

const TUint32 KDummyRequestId = 0;

// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::TOutgoingRequestQueueItem
// -----------------------------------------------------------------------------
//
TOutgoingRequestQueueItem::TOutgoingRequestQueueItem( 
    const TSIPTransportParams& aParams,
    CSIPRequest& aRequest,
    TBool aForceUDP,
    const TInetAddr& aAddr,
    TUint aLocalPort,
    const CUri8* aOutboundProxy,
    TUint aOrigTransport,
    TRequestStatus& aStatus,
    RSocket& aSocket,
    TBool aPermissionToUse ) :
    iParams( aParams ),
    iRequest( aRequest ),
    iForceUDP( aForceUDP ),
    iAddr( aAddr ),
    iLocalPort( aLocalPort ),
    iOutboundProxy( aOutboundProxy ),
    iOrigTransport( aOrigTransport ),
    iStatus( aStatus ),
    iSocket( aSocket ),
    iPermissionToUse( aPermissionToUse ),
    iRequestId( KDummyRequestId ),
    iError( KErrNone )
    {
    }

// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::Match
// -----------------------------------------------------------------------------
//   
TBool TOutgoingRequestQueueItem::Match( TUint32 aRequestId ) const
    {
    return ( iRequestId == aRequestId );
    }
    
// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::Match
// -----------------------------------------------------------------------------
//   
TBool TOutgoingRequestQueueItem::Match( RSocket& aSocket ) const
    {
    return ( aSocket.SubSessionHandle() == iSocket.SubSessionHandle() );
    }
    
// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::Match
// -----------------------------------------------------------------------------
//   
TBool TOutgoingRequestQueueItem::Match( TRequestStatus& aStatus ) const
    {
    return ( &iStatus == &aStatus );
    }

// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::IsActiveRequest
// -----------------------------------------------------------------------------
//      
TBool TOutgoingRequestQueueItem::IsActiveRequest() const
    {
    return ( iRequestId != KDummyRequestId );
    }

// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::Params
// -----------------------------------------------------------------------------
//        
const TSIPTransportParams& TOutgoingRequestQueueItem::Params()
    {
    return iParams;
    }

// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::Request
// -----------------------------------------------------------------------------
//        
CSIPRequest& TOutgoingRequestQueueItem::Request()
    {
    return iRequest;
    }

// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::ForceUDP
// -----------------------------------------------------------------------------
//    
TBool TOutgoingRequestQueueItem::ForceUDP()
    {
    return iForceUDP;
    }

// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::Addr
// -----------------------------------------------------------------------------
//        
const TInetAddr& TOutgoingRequestQueueItem::Addr()
    {
    return iAddr;
    }

// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::LocalPort
// -----------------------------------------------------------------------------
//     
TUint TOutgoingRequestQueueItem::LocalPort() const
    {
    return iLocalPort;
    }
 
// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::OutboundProxy
// -----------------------------------------------------------------------------
//   
const CUri8* TOutgoingRequestQueueItem::OutboundProxy()
    {
    return iOutboundProxy;
    }

// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::OrigTransport
// -----------------------------------------------------------------------------
//      
TUint TOutgoingRequestQueueItem::OrigTransport() const
    {
    return iOrigTransport;
    }

// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::Status
// -----------------------------------------------------------------------------
//        
TRequestStatus& TOutgoingRequestQueueItem::Status()
    {
    return iStatus;
    }

// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::Socket
// -----------------------------------------------------------------------------
//       
RSocket& TOutgoingRequestQueueItem::Socket()
    {
    return iSocket;
    }

// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::SetPermissionToUse
// -----------------------------------------------------------------------------
//   
void TOutgoingRequestQueueItem::SetPermissionToUse( TBool aPermissionToUse )
    {
    iPermissionToUse = aPermissionToUse;
    }

// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::PermissionToUse
// -----------------------------------------------------------------------------
//        
TBool TOutgoingRequestQueueItem::PermissionToUse() const
    {
    return iPermissionToUse;
    }

// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::SetRequestId
// -----------------------------------------------------------------------------
//        
void TOutgoingRequestQueueItem::SetRequestId( TUint32 aRequestId )
    {
    iRequestId = aRequestId;
    }

// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::RequestId
// -----------------------------------------------------------------------------
//        
TUint32 TOutgoingRequestQueueItem::RequestId() const
    {
    return iRequestId;
    }

// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::SetFailed
// -----------------------------------------------------------------------------
//
void TOutgoingRequestQueueItem::SetFailed( TInt aError )
    {
    iError = aError;
    }

// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::IsFailed
// -----------------------------------------------------------------------------
//        
TBool TOutgoingRequestQueueItem::IsFailed() const
    {
    return ( iError != KErrNone );
    }

// -----------------------------------------------------------------------------
// TOutgoingRequestQueueItem::Error
// -----------------------------------------------------------------------------
//
TInt TOutgoingRequestQueueItem::Error() const
    {
    return iError;
    }
    
// End of file
