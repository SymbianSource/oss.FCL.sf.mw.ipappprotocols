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
// Name          : CNATTraversalNotIntegrated.h
// Part of       : ConnectionMgr
// Version       : SIP/5.0 
//



#include "CNATTraversalNotIntegrated.h"

#include <sipnattraversalrequestobserver.h>


const TInt KCompletionTime = 1;

// -----------------------------------------------------------------------------
// TPendingAsyncRequest::TPendingAsyncRequest
// -----------------------------------------------------------------------------
//
TPendingAsyncRequest::TPendingAsyncRequest( 
    TDeltaTimerEntry aTimerEntry,
    TUint32 aIapId,
    MSIPNATTraversalRequestObserver& aObserver,
    TUint32 aRequestId ) :
    iTimerEntry( aTimerEntry ),
    iIapId( aIapId ),
    iObserver( aObserver ),
    iRequestId( aRequestId )
    {
    }  

// -----------------------------------------------------------------------------
// TPendingAsyncRequest::MatchRequestId
// -----------------------------------------------------------------------------
//    
TBool TPendingAsyncRequest::MatchRequestId( TUint32 aRequestId ) const
    {
    return ( iRequestId == aRequestId );
    }
    
// -----------------------------------------------------------------------------
// TPendingAsyncRequest::MatchIapId
// -----------------------------------------------------------------------------
//    
TBool TPendingAsyncRequest::MatchIapId( TUint32 aIapId ) const
    {
    return ( iIapId == aIapId );
    }

// -----------------------------------------------------------------------------
// CNATTraversalNotIntegrated::NewL
// -----------------------------------------------------------------------------
//
CNATTraversalNotIntegrated* CNATTraversalNotIntegrated::NewL()
    {
    CNATTraversalNotIntegrated* self = 
        new ( ELeave ) CNATTraversalNotIntegrated;
    CleanupStack::PushL( self );
    self->ConstructL();    
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CNATTraversalNotIntegrated::~CNATTraversalNotIntegrated
// -----------------------------------------------------------------------------
//
CNATTraversalNotIntegrated::~CNATTraversalNotIntegrated()
    {
    for ( TInt i = 0; i < iAsyncRequests.Count(); i++ )
	    {
	    if ( iAsyncCompletionTimer )
	        {
	        iAsyncCompletionTimer->Remove( iAsyncRequests[ i ].iTimerEntry );
	        }
	    }
	iAsyncRequests.Close();
    delete iAsyncCompletionTimer;
    }

// -----------------------------------------------------------------------------
// CNATTraversalNotIntegrated::ResolvePublicAddrL
// -----------------------------------------------------------------------------
//
TUint32 CNATTraversalNotIntegrated::ResolvePublicAddrL(
    TUint32 aIapId,
    RConnection& /*aConnection*/,
    const TInetAddr& /*aLocalAddr*/,
    const TDesC8& /*aDomain*/,
    RSocket& /*aUdpSocket*/,
    const TInetAddr& /*aNextHopAddr*/,
    MSIPNATBindingObserver* /*aBindingObserver*/,
    MSIPNATTraversalRequestObserver& aRequestObserver )
    {
    TUint32 requestId( NextRequestId() );
    AddAsyncRequestL( aIapId, aRequestObserver, requestId );
    return requestId;
    }

// -----------------------------------------------------------------------------
// CNATTraversalNotIntegrated::Cancel
// -----------------------------------------------------------------------------
//
void CNATTraversalNotIntegrated::Cancel( TUint32 aRequestId )
    {
    TBool found( EFalse );
    for ( TInt i = 0; i < iAsyncRequests.Count() && !found; i++ )
	    {
	    TPendingAsyncRequest& request = iAsyncRequests[ i ];
	    if ( request.MatchRequestId( aRequestId ) )
	        {
	        found = ETrue;
    	    iAsyncCompletionTimer->Remove( request.iTimerEntry );
    	    iAsyncRequests.Remove( i );
    	    iAsyncRequests.Compress();
	        }
	    }
    }

// -----------------------------------------------------------------------------
// CNATTraversalNotIntegrated::RefreshNATBindingL
// -----------------------------------------------------------------------------
//
TBool CNATTraversalNotIntegrated::RefreshNATBindingL( 
    RSocket& /*aSocket*/,
    const MSIPNATBindingObserver* /*aBindingObserver*/ )
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CNATTraversalNotIntegrated::RefreshNATBindingL
// -----------------------------------------------------------------------------
//            
TBool CNATTraversalNotIntegrated::RefreshNATBindingL(
    CSecureSocket& /*aSecureSocket*/,
    const MSIPNATBindingObserver* /*aBindingObserver*/ )
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CNATTraversalNotIntegrated::UpdateNextHop
// -----------------------------------------------------------------------------
//
void CNATTraversalNotIntegrated::UpdateNextHop( 
    const TInetAddr& /*aNextHopAddr*/,
    const MSIPNATBindingObserver* /*aBindingObserver*/ )
    {
    }

// -----------------------------------------------------------------------------
// CNATTraversalNotIntegrated::SocketIdle
// -----------------------------------------------------------------------------
//
void CNATTraversalNotIntegrated::SocketIdle( 
    TBool /*aIdle*/,
    RSocket& /*aSocket*/ )
    {
    }

// -----------------------------------------------------------------------------
// CNATTraversalNotIntegrated::SocketIdle
// -----------------------------------------------------------------------------
//     
void CNATTraversalNotIntegrated::SocketIdle( 
    TBool /*aIdle*/,
    CSecureSocket& /*aSecureSocket*/ )
    {
    }

// -----------------------------------------------------------------------------
// CNATTraversalNotIntegrated::DataReceivedL
// -----------------------------------------------------------------------------
//       
void CNATTraversalNotIntegrated::DataReceivedL( 
    const TDesC8& /*aData*/,
    const RSocket& /*aUdpSocket*/,
    TBool& aHandled )
    {
    aHandled = EFalse;
    }

// -----------------------------------------------------------------------------
// CNATTraversalNotIntegrated::FreeResources
// -----------------------------------------------------------------------------
//
void CNATTraversalNotIntegrated::FreeResources( TUint32 aIapId )
    {
    TInt lastIndex( iAsyncRequests.Count() - 1 );
    for ( TInt i = lastIndex; i >= 0; i-- )
	    {
	    TPendingAsyncRequest& request = iAsyncRequests[ i ];
	    if ( request.MatchIapId( aIapId ) )
	        {
    	    iAsyncCompletionTimer->Remove( request.iTimerEntry );
    	    iAsyncRequests.Remove( i );
    	    iAsyncRequests.Compress();
	        }
	    }
    }

// -----------------------------------------------------------------------------
// CNATTraversalNotIntegrated::FreeResources
// -----------------------------------------------------------------------------
//
void CNATTraversalNotIntegrated::FreeResources( 
    MSIPNATBindingObserver& /*aBindingObserver*/ )
    {
    }

// -----------------------------------------------------------------------------
// CNATTraversalNotIntegrated::CompletionCallback
// -----------------------------------------------------------------------------
//
TInt CNATTraversalNotIntegrated::CompletionCallback( TAny* aAny )
	{
	CNATTraversalNotIntegrated* self = 
	    reinterpret_cast< CNATTraversalNotIntegrated* >( aAny );

    if ( self )
        {
        self->CompletePendingAsyncRequest();
        }
    
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CNATTraversalNotIntegrated::ConstructL
// -----------------------------------------------------------------------------
//	
void CNATTraversalNotIntegrated::ConstructL()
    {
    iAsyncCompletionTimer = CDeltaTimer::NewL( CActive::EPriorityStandard );  
    } 

// -----------------------------------------------------------------------------
// CNATTraversalNotIntegrated::CNATTraversalNotIntegrated
// -----------------------------------------------------------------------------
//
CNATTraversalNotIntegrated::CNATTraversalNotIntegrated()
    {
    }

// -----------------------------------------------------------------------------
// CNATTraversalNotIntegrated::AddAsyncRequestL
// -----------------------------------------------------------------------------
//    
void CNATTraversalNotIntegrated::AddAsyncRequestL( 
    TUint32 aIapId,
    MSIPNATTraversalRequestObserver& aRequestObserver,
    TUint32 aRequestId )
    {
    TCallBack callback( CompletionCallback, this );	
	TDeltaTimerEntry entry( callback );
	
	TPendingAsyncRequest request( entry, 
	                              aIapId,
	                              aRequestObserver,
	                              aRequestId );
	
	iAsyncRequests.AppendL( request );
	
    iAsyncCompletionTimer->Queue( 
        KCompletionTime, 
        iAsyncRequests[ iAsyncRequests.Count() - 1 ].iTimerEntry );    
    }

// -----------------------------------------------------------------------------
// CNATTraversalNotIntegrated::CompletePendingAsyncRequest
// -----------------------------------------------------------------------------
//       
void CNATTraversalNotIntegrated::CompletePendingAsyncRequest()
    {
    if ( iAsyncRequests.Count() > 0 )
        {
	    TPendingAsyncRequest& request = iAsyncRequests[ 0 ];
    	iAsyncCompletionTimer->Remove( request.iTimerEntry );
    	
    	TUint32 requestId = request.iRequestId;
    	MSIPNATTraversalRequestObserver& observer = request.iObserver;
	    iAsyncRequests.Remove( 0 );
	    iAsyncRequests.Compress();
	    
    	observer.RequestComplete( requestId, KErrNone );
	    }
    }
    
// -----------------------------------------------------------------------------
// CNATTraversalNotIntegrated::NextRequestId
// -----------------------------------------------------------------------------
//
TUint32 CNATTraversalNotIntegrated::NextRequestId()
	{
    if ( iRequestIdCounter == KMaxTUint32 )
		{
		iRequestIdCounter = 1;
		}
	else
		{
		iRequestIdCounter++;
		}
	return iRequestIdCounter;
	}

// End of file
