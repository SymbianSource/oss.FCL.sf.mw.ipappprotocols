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
// Name        : COutgoingRequestQueue.cpp
// Part of     : ConnectionMgr
// Version     : SIP/5.0
//



#include "COutgoingRequestQueue.h"
#include "MOutgoingRequestQueueContext.h"
#include "CTransport.h"
#include "siprequest.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcontactheader.h"
#include "sipviaheader.h"
#include "siptoheader.h"
#include "sipfromheader.h"
#include "uricontainer.h"
#include "sipuri.h"
#include "sipaddress.h"
#include "siphostport.h"
#include "TSIPTransportParams.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "SIPHeaderLookup.h"
#include "siperr.h"
#include <sipnattraversalcontroller.h> 

const TInt KRequestQueueGranularity = 4;
    
// -----------------------------------------------------------------------------
// COutgoingRequestQueue::NewLC
// -----------------------------------------------------------------------------
//
COutgoingRequestQueue* COutgoingRequestQueue::NewL( 
    MOutgoingRequestQueueContext& aContext,
    CSIPNATTraversalController& aNATTraversal,
    RConnection& aConnection,
    TUint32 aIapId )
    {
    COutgoingRequestQueue* self = 
        new ( ELeave ) COutgoingRequestQueue( aContext, 
                                              aNATTraversal, 
                                              aConnection, 
                                              aIapId );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// COutgoingRequestQueue::~COutgoingRequestQueue
// -----------------------------------------------------------------------------
//
COutgoingRequestQueue::~COutgoingRequestQueue()
    {
    iNATTraversal.FreeResources( iIapId );
    iRequestQueue.Close();
    iStrNATTraversalRequired.Close();
    }

// -----------------------------------------------------------------------------
// COutgoingRequestQueue::RequestComplete
// From MSIPNATTraversalRequestObserver
// -----------------------------------------------------------------------------
//
void COutgoingRequestQueue::RequestComplete( 
    TUint32 aRequestId, 
    TInt aCompletionCode )
    {
    TInt index = FindRequestQueueItemIndex( aRequestId );
    if ( index != KErrNotFound )
        {
        Complete( iRequestQueue[ index ], aCompletionCode, EFalse );
        iRequestQueue.Remove( index );
        iRequestQueue.Compress();
        }  
    ActivatePendingRequests();
    }

// -----------------------------------------------------------------------------
// COutgoingRequestQueue::PublicAddrResolved
// From MSIPNATTraversalRequestObserver
// -----------------------------------------------------------------------------
//		    
void COutgoingRequestQueue::PublicAddrResolved( 
    TUint32 aRequestId, 
    const TInetAddr& aPublicAddr )
    {
    TInt index = FindRequestQueueItemIndex( aRequestId );
    if ( index != KErrNotFound )
        {
        Update( iRequestQueue[ index ], aPublicAddr );
        iRequestQueue.Remove( index );
        iRequestQueue.Compress();
        }
    ActivatePendingRequests();
    }

// -----------------------------------------------------------------------------
// COutgoingRequestQueue::UsagePermissionChanged
// From MSocketUsagePermissionObserver
// -----------------------------------------------------------------------------
//    
void COutgoingRequestQueue::UsagePermissionChanged( 
    RSocket& aSocket, 
    TBool aPermissionToUse )
    {
    TInt lastIndex( iRequestQueue.Count() - 1 );
    for ( TInt i = lastIndex; i >= 0; i-- )
        {
        TOutgoingRequestQueueItem& item = iRequestQueue[ i ];
        if ( item.Match( aSocket ) )
            {
            // May lead to removal of the item
            HandleUsagePermissionChange( item, aPermissionToUse );
            }
        }
    ActivatePendingRequests();
    }

// -----------------------------------------------------------------------------
// COutgoingRequestQueue::NATTraverseL
// -----------------------------------------------------------------------------
//	
void COutgoingRequestQueue::NATTraverseL( 
    const TSIPTransportParams& aParams,
    CSIPRequest& aRequest,
    TBool aForceUDP,
	const TInetAddr& aAddr,
	const CUri8* aOutboundProxy,
	TUint aOrigTransport,
	TRequestStatus &aStatus,
	const TInetAddr& aLocalAddr )
    {
    HBufC8* localAddrBuf = CTransport::ConvertInetAddrToTextL( aLocalAddr );
	CleanupStack::PushL( localAddrBuf );
    TUint localPort = ContactPortL( aRequest, *localAddrBuf );
    CleanupStack::PopAndDestroy( localAddrBuf );    
    
    if ( aRequest.Method() == SIPStrings::StringF( SipStrConsts::ERegister ) &&
         aRequest.To()->HasParam( iStrNATTraversalRequired ) &&
         !aLocalAddr.IsUnspecified() ) //Nat Traversal for both IPv6 and IPv4 addresses.
        {
        RemoveNATTraversalParameter( aRequest );
        TBool permissionToUse( EFalse );
        iLocalAddr = aLocalAddr;
        RSocket& udpSocket = iContext.GetUdpSocketL( aParams, 
                                                     aAddr, 
                                                     localPort, 
                                                     this, 
                                                     permissionToUse ); 
    
        TOutgoingRequestQueueItem queueItem( aParams, 
                                             aRequest, 
                                             aForceUDP,
                                             aAddr, 
                                             localPort,
                                             aOutboundProxy,
                                             aOrigTransport,
                                             aStatus,
                                             udpSocket,
                                             permissionToUse );
    
        // If there's already active request for this particular UDP socket,
        // request cannot be activated before existing request has completed.   
        if ( permissionToUse && !HasRequestForSocket( udpSocket, EFalse ) )
            {
            IssueRequestL( queueItem );
            }                                                  
                           
        iRequestQueue.AppendL( queueItem );
        
        // Request status has to be set to pending before going to async.
        // Set it when this synchronous execution path cannot leave anymore. 
        aStatus = KRequestPending;        
        }
    else
        {
        iNATTraversal.UpdateNextHop( 
            aAddr, 
            const_cast<TSIPTransportParams&>( aParams ).NATBindingObserver() );

        iContext.ContinueSendToTransportL( aParams,
                                           aRequest, 
                                           aForceUDP,
                                           aAddr,
                                           localPort,
                                           aOrigTransport,
                                           aStatus,
                                           EFalse );
        }
    }

// -----------------------------------------------------------------------------
// COutgoingRequestQueue::FreeResources
// -----------------------------------------------------------------------------
//    
void COutgoingRequestQueue::FreeResources( 
    MSIPNATBindingObserver& aSIPNATBindingObserver )
    {
    iNATTraversal.FreeResources( aSIPNATBindingObserver );
    }

// -----------------------------------------------------------------------------
// COutgoingRequestQueue::CancelSend
// -----------------------------------------------------------------------------
//     
TBool COutgoingRequestQueue::CancelSend( TRequestStatus& aStatus )
    {
    for ( TInt i = 0; i < iRequestQueue.Count(); i++ )
        {
        TOutgoingRequestQueueItem& item = iRequestQueue[ i ];
        if ( item.Match( aStatus ) )
            {
            iNATTraversal.Cancel( item.RequestId() );
            Complete( item, KErrSIPTransportFailure, EFalse );
            iRequestQueue.Remove( i );
            iRequestQueue.Compress();
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// COutgoingRequestQueue::Update
// -----------------------------------------------------------------------------
//
void COutgoingRequestQueue::Update( 
    TOutgoingRequestQueueItem& aItem, 
    const TInetAddr& aPublicAddr )
    {
    TRAPD( err, UpdateHeadersL( aItem, iLocalAddr, aPublicAddr ) );
    Complete( aItem, err, ETrue );
    }
    
// -----------------------------------------------------------------------------
// COutgoingRequestQueue::Complete
// -----------------------------------------------------------------------------
//
void COutgoingRequestQueue::Complete( 
    TOutgoingRequestQueueItem& aItem, 
    TInt aCompletionCode,
    TBool aPublicAddrResolved )
    {     
    if ( !HasRequestForSocket( aItem.Socket(), ETrue ) )
        {
        // Notify if no pending requests exists for the socket
        iContext.UdpSocketFree( aItem.Params(), 
                                aItem.Addr(), 
                                aItem.LocalPort(), 
                                this,
                                aItem.Socket() );
        }
    
    if ( !aCompletionCode )
        {
        TRAP( aCompletionCode,
              iContext.ContinueSendToTransportL( aItem.Params(),
                                                 aItem.Request(), 
                                                 aItem.ForceUDP(),
                                                 aItem.Addr(),
                                                 aItem.LocalPort(),
                                                 aItem.OrigTransport(),
                                                 aItem.Status(),
                                                 aPublicAddrResolved ) );
        }
        
    if ( aCompletionCode )
        {
        iContext.RequestFailed( aItem.Status(), aCompletionCode );
        }
    }
    
// -----------------------------------------------------------------------------
// COutgoingRequestQueue::UpdateHeadersL
// -----------------------------------------------------------------------------
//
void COutgoingRequestQueue::UpdateHeadersL(
    TOutgoingRequestQueueItem& aItem,
    const TInetAddr& aLocalAddr, 
    const TInetAddr& aPublicAddr )
    {
    // Some NAT-aware proxies/registrars detect from the private IP address
    // that the UA is behind a NAT and maintain a persistent TCP-connection.
    // If the public address is used with TCP, 
    // these proxies/registrars will not maintain persistent TCP-connection. 
    if ( aItem.OrigTransport() == KProtocolInetUdp )
        {
        HBufC8* localAddrBuf = 
            CTransport::ConvertInetAddrToTextL( aLocalAddr );
    	CleanupStack::PushL( localAddrBuf );
    	
    	HBufC8* publicAddrBuf = 
    	    CTransport::ConvertInetAddrToTextL( aPublicAddr );
    	CleanupStack::PushL( publicAddrBuf );
    	
    	TUint publicPort( aPublicAddr.Port() );
    	
    	UpdateContactHeaderL( aItem.Request(), *localAddrBuf, 
    	                      *publicAddrBuf, publicPort );
    	                      
    	UpdateViaHeaderL( aItem.Request(), *publicAddrBuf, publicPort );                    
    	
    	CleanupStack::PopAndDestroy( publicAddrBuf );
    	CleanupStack::PopAndDestroy( localAddrBuf );
        }
    }

// -----------------------------------------------------------------------------
// COutgoingRequestQueue::UpdateContactHeaderL
// -----------------------------------------------------------------------------
//
void COutgoingRequestQueue::UpdateContactHeaderL(
    CSIPMessage& aMessage,
    const TDesC8& aLocalAddr, 
    const TDesC8& aPublicAddr,
    TUint aPublicPort )  
    {
    if ( aMessage.HasHeader( 
                        SIPStrings::StringF(SipStrConsts::EContactHeader) ) )
		{
        TSglQueIter<CSIPHeaderBase> iter = 
            aMessage.Headers( 
                SIPStrings::StringF(SipStrConsts::EContactHeader) );
		CSIPHeaderBase* header = 0;
		while ( ( header = iter++ ) != 0 )
			{
			CSIPContactHeader* contact = 
			        static_cast<CSIPContactHeader*>( header );
			CSIPAddress* sipAddr = contact->SIPAddress();

			if ( sipAddr )
			    {
			    CURIContainer& uriContainer = sipAddr->URI();
			    if ( uriContainer.IsSIPURI() && 
			         uriContainer.SIPURI()->HostPort().Host().Compare( 
			            aLocalAddr ) == 0 )
			        {
			        uriContainer.SIPURI()->HostPort().SetHostL( aPublicAddr );
			        uriContainer.SIPURI()->HostPort().SetPort( aPublicPort );
			        }
			    }
			}
		}
    }
    
// -----------------------------------------------------------------------------
// COutgoingRequestQueue::UpdateViaHeaderL
// -----------------------------------------------------------------------------
//
void COutgoingRequestQueue::UpdateViaHeaderL(
    CSIPMessage& aMessage,
    const TDesC8& aPublicAddr,
    TUint aPublicPort )
    {
    CSIPViaHeader* via = CTransport::TopViaHeader( &aMessage );
    if ( via )
        {
        via->SentByHostPort().SetHostL( aPublicAddr );
    	via->SentByHostPort().SetPort( aPublicPort );
        }
    }    
    
// -----------------------------------------------------------------------------
// COutgoingRequestQueue::HandleUsagePermissionChange
// -----------------------------------------------------------------------------
//   
void COutgoingRequestQueue::HandleUsagePermissionChange( 
    TOutgoingRequestQueueItem& aItem, 
    TBool aPermissionToUse )
    {     
    aItem.SetPermissionToUse( aPermissionToUse );
    
    if ( !aPermissionToUse && aItem.IsActiveRequest() )
        {
        CancelSend( aItem.Status() );
        }
    }

// -----------------------------------------------------------------------------
// COutgoingRequestQueue::HasRequestForSocket
// -----------------------------------------------------------------------------
//
TBool COutgoingRequestQueue::HasRequestForSocket( 
    RSocket& aSocket, 
    TBool aFindPendingRequest )
    {
    for ( TInt i = 0; i < iRequestQueue.Count(); i++ )
        {
        TOutgoingRequestQueueItem& item = iRequestQueue[ i ];
        if ( item.Match( aSocket ) && 
             !item.IsFailed() &&
             ( item.IsActiveRequest() ^ aFindPendingRequest ) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// COutgoingRequestQueue::CleanupFailedRequests
// -----------------------------------------------------------------------------
//
void COutgoingRequestQueue::CleanupFailedRequests()
    {
    TInt lastIndex( iRequestQueue.Count() - 1 );
    for ( TInt i = lastIndex; i >= 0; i-- )
        {
        TOutgoingRequestQueueItem& item = iRequestQueue[ i ];
        if ( item.IsFailed() )
            {
            iContext.RequestFailed( item.Status(), item.Error() );
            iRequestQueue.Remove( i );
            iRequestQueue.Compress();
            }
        }
    }

// -----------------------------------------------------------------------------
// COutgoingRequestQueue::ActivatePendingRequests
// -----------------------------------------------------------------------------
//    
void COutgoingRequestQueue::ActivatePendingRequests()
    {
    for ( TInt i = 0; i < iRequestQueue.Count(); i++ )
        {
        TOutgoingRequestQueueItem& item = iRequestQueue[ i ];
        
        // Activate if there's no other active request for the socket
        // this pending request uses.
        if ( !item.IsActiveRequest() && 
             !HasRequestForSocket( item.Socket(), EFalse ) &&
             item.PermissionToUse() )
            {
            TRAPD( err, IssueRequestL( item ) );
            item.SetFailed( err );
            if ( err && !HasRequestForSocket( item.Socket(), ETrue ) )
                {
                // If no more pending requests, inform that socket is free
                iContext.UdpSocketFree( item.Params(), 
                                        item.Addr(), 
                                        item.LocalPort(), 
                                        this,
                                        item.Socket() );
                }            
            }
        }
    CleanupFailedRequests();
    }

// -----------------------------------------------------------------------------
// COutgoingRequestQueue::IssueRequestL
// -----------------------------------------------------------------------------
// 
void COutgoingRequestQueue::IssueRequestL( TOutgoingRequestQueueItem& aItem )
    {
    __ASSERT_ALWAYS( !HasRequestForSocket( aItem.Socket(), EFalse ), 
                     User::Leave( KErrInUse ) );      
                     
    TPtrC8 domain( KNullDesC8 );
    CSIPURI* toSIPURI = aItem.Request().To()->SIPAddress().URI().SIPURI();
    if ( toSIPURI )
        {
        domain.Set( toSIPURI->HostPort().Host() );
        }
                     
    // Have to do const cast for transport params to get binding observer
    TUint32 requestId = iNATTraversal.ResolvePublicAddrL( 
        iIapId, 
        iConnection, 
        iLocalAddr,
        domain,
        aItem.Socket(),
        aItem.Addr(),
        const_cast<TSIPTransportParams&>( aItem.Params() ).NATBindingObserver(),
        *this );
    
    aItem.SetRequestId( requestId );
    aItem.SetFailed( KErrNone );
    }
    
// -----------------------------------------------------------------------------
// COutgoingRequestQueue::FindRequestQueueItemIndex
// -----------------------------------------------------------------------------
//  
TInt COutgoingRequestQueue::FindRequestQueueItemIndex( TUint32 aRequestId ) const
    {
    for ( TInt i = 0; i < iRequestQueue.Count(); i++ )
        {
        if ( iRequestQueue[ i ].Match( aRequestId ) )
            {
            return i;
            }
        }
    return KErrNotFound;
    }
    
// -----------------------------------------------------------------------------
// COutgoingRequestQueue::RemoveNATTraversalParameter
// -----------------------------------------------------------------------------
//
void COutgoingRequestQueue::RemoveNATTraversalParameter( CSIPRequest& aRequest )
    {
    CSIPToHeader* to = aRequest.To();
    if ( to )
        {
        to->DeleteParam( iStrNATTraversalRequired );
        }
    CSIPFromHeader* from = aRequest.From();
    if ( from )
        {
        from->DeleteParam( iStrNATTraversalRequired );
        }   
    }

// -----------------------------------------------------------------------------
// COutgoingRequestQueue::ContactPortL
// -----------------------------------------------------------------------------
//   
TUint COutgoingRequestQueue::ContactPortL( 
    CSIPRequest& aRequest,
    const TDesC8& aLocalAddr )
    {
    if ( aRequest.HasHeader( 
            SIPStrings::StringF(SipStrConsts::EContactHeader) ) )
		{
        TSglQueIter<CSIPHeaderBase> iter = 
            aRequest.Headers( 
                SIPStrings::StringF(SipStrConsts::EContactHeader) );
		CSIPHeaderBase* header = 0;
		while ( ( header = iter++ ) != 0 )
			{
			CSIPContactHeader* contact = 
			        static_cast<CSIPContactHeader*>( header );
			CSIPAddress* sipAddr = contact->SIPAddress();
			
			if ( sipAddr )
			    {
			    CURIContainer& uriContainer = sipAddr->URI();
			    if ( uriContainer.IsSIPURI() && 
			         uriContainer.SIPURI()->HostPort().Host().Compare( 
			            aLocalAddr ) == 0 &&
			         uriContainer.SIPURI()->HostPort().HasPort() )
			        {
			        return uriContainer.SIPURI()->HostPort().Port();
			        }
			    }
			}
		}
    return 0;
    }

// -----------------------------------------------------------------------------
// COutgoingRequestQueue::ConvertToSIPURIRollBack
// -----------------------------------------------------------------------------
//
void COutgoingRequestQueue::ConvertToSIPURIRollBack(TAny* /*aDummy*/)
    {
    SIPHeaderLookup::SetConvertToSIPURI(ETrue);
    }

// -----------------------------------------------------------------------------
// COutgoingRequestQueue::ConstructL
// -----------------------------------------------------------------------------
// 
void COutgoingRequestQueue::ConstructL()
    {   
    _LIT8( KNATTraversalRequired, "nat_traversal_required" );    
    
    iStrNATTraversalRequired = 
        SIPStrings::Pool().OpenFStringL( KNATTraversalRequired ); 
    }
    
// -----------------------------------------------------------------------------
// COutgoingRequestQueue::COutgoingRequestQueue
// -----------------------------------------------------------------------------
//    
COutgoingRequestQueue::COutgoingRequestQueue(
    MOutgoingRequestQueueContext& aContext,
    CSIPNATTraversalController& aNATTraversal,
    RConnection& aConnection,
    TUint32 aIapId ) :
    iContext( aContext ),
    iNATTraversal( aNATTraversal ),
    iConnection( aConnection ),
    iIapId( aIapId ),
    iRequestQueue( KRequestQueueGranularity )
    {
    }
    
// End of file
