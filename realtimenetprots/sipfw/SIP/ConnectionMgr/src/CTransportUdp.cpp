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
// Name          : CTransportUdp.cpp
// Part of       : ConnectionMgr
// Version       : SIP/4.0 
//




#include "CTransportUdp.h"
#include "CSipConnection.h"
#include "CErrorHandler.h"
#include "CReceiver.h"
#include "CSenderUdp.h"
#include "CSIPServerResolver.h"
#include "sipviaheader.h"
#include "siphostport.h"
#include "sipmessage.h"
#include "sipresponse.h"
#include "Lwtimer.h"
#include "SipLogs.h"
#include "CResHandler.h"
#include "utf.h"
#include "CommonConsts.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "siperr.h"
#include "NetworkInfo.h"
#include "COwnerSettingsList.h"
#include "CSocketContainer.h"
#include "MSocketUsagePermissionObserver.h"
#include <sipnattraversalcontroller.h>


const TInt KTransportUsersGranularity = 2;


// -----------------------------------------------------------------------------
// CTransportUdp::NewL
// -----------------------------------------------------------------------------
//
CTransportUdp* CTransportUdp::NewL( 
    RSocketServ& aServer, 
    TUint aPort,
    MTransactionFinder& aFinder, 
    MServerTaFactory& aTaFactory,
    MTransportOwner& aTransportOwner,
    MTimerManager& aTimerManager,
    CSIPServerResolver& aServerResolver,
    RConnection& aConnection,
    MSigCompController& aSigCompHandler,
    TUint aICMPErrorWaitTime,
    CNetworkInfo& aNetworkInfo,
    COwnerSettingsList& aSettingsList,
    const TSIPTransportParams& aTransportParams,
    CSIPNATTraversalController& aNATTraversal,
    TBool aOnlyForReceiving )
	{
	CTransportUdp* self = NewLC( aServer, aPort, aFinder, aTaFactory, 
						     	 aTransportOwner, aTimerManager, 
						     	 aServerResolver, aConnection, aSigCompHandler,
						    	 aICMPErrorWaitTime, aNetworkInfo,
						    	 aSettingsList, aTransportParams,
						    	 aNATTraversal, aOnlyForReceiving );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CTransportUdp::NewLC
// -----------------------------------------------------------------------------
//
CTransportUdp* CTransportUdp::NewLC( 
    RSocketServ& aServer, 
    TUint aPort,
    MTransactionFinder& aFinder, 
    MServerTaFactory& aTaFactory,
    MTransportOwner& aTransportOwner,
    MTimerManager& aTimerManager,
    CSIPServerResolver& aServerResolver,
    RConnection& aConnection,
    MSigCompController& aSigCompHandler,
    TUint aICMPErrorWaitTime,
    CNetworkInfo& aNetworkInfo,
    COwnerSettingsList& aSettingsList,
    const TSIPTransportParams& aTransportParams,
    CSIPNATTraversalController& aNATTraversal,
    TBool aOnlyForReceiving )
	{
	CTransportUdp* self = new ( ELeave ) CTransportUdp( aServer, 
	                                                    aPort,
	                                                    aFinder, 
	                                                    aTaFactory, 
													    aTransportOwner,
													    aTimerManager,
													    aServerResolver,
													    aConnection,
													    aSigCompHandler,
													    aICMPErrorWaitTime,
													    aNetworkInfo,
													    aSettingsList,
													    aTransportParams,
													    aNATTraversal,
													    aOnlyForReceiving );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CTransportUdp::CTransportUdp
// -----------------------------------------------------------------------------
//
CTransportUdp::CTransportUdp( 
    RSocketServ& aServer,
    TUint aPort, 
    MTransactionFinder& aFinder, 
    MServerTaFactory& aTaFactory,
    MTransportOwner& aTransportOwner,
    MTimerManager& aTimerManager,
    CSIPServerResolver& aServerResolver,
    RConnection& aConnection,
    MSigCompController& aSigCompHandler,
    TUint aICMPErrorWaitTime,
    CNetworkInfo& aNetworkInfo,
    COwnerSettingsList& aSettingsList,
    const TSIPTransportParams& aTransportParams,
    CSIPNATTraversalController& aNATTraversal,
    TBool aOnlyForReceiving ) 
        :
	CTransport( aFinder, aTaFactory, aTransportOwner, aServerResolver,
	            aSigCompHandler, aTransportParams, aNetworkInfo, 
	            aSettingsList, aNATTraversal ),
	iSocketServ( &aServer ),
	iConnection( &aConnection ),
	iSourcePort( aPort ),
	iTimerManager( aTimerManager ),
	iList( CResponseHandler::iOffset ),
	iListIter( iList ),
	iICMPErrorWaitTime( aICMPErrorWaitTime ),
	iTransportUsers( KTransportUsersGranularity ),
	iOnlyForReceiving( aOnlyForReceiving ),
	iIsSending( EFalse )
	{		
	}

// -----------------------------------------------------------------------------
// CTransportUdp::ConstructL
// -----------------------------------------------------------------------------
//
void CTransportUdp::ConstructL()
	{
	iErrorHandler =	CTransportErrorHandler::NewL( *this );
	
    RefreshSocketContainerL( KAfInet );
    RefreshSocketContainerL( KAfInet6 );	                                   	
	
	iSender = CSenderUdp::NewL( *this, iSettingsList );
	
	iTransportUsers.AppendL( iTransportParams.TransportId() );
	}

// -----------------------------------------------------------------------------
// CTransportUdp::~CTransportUdp
// -----------------------------------------------------------------------------
//
CTransportUdp::~CTransportUdp()
	{
    if ( iSocketUsagePermissionObserver )
	    {
	    iSocketUsagePermissionObserver->UsagePermissionChanged( iIPv4Socket, EFalse );
	    iSocketUsagePermissionObserver->UsagePermissionChanged( iIPv6Socket, EFalse );
	    }
	
	iNATTraversal.SocketIdle( EFalse, iIPv4Socket );
	iNATTraversal.SocketIdle( EFalse, iIPv6Socket );
	
	CResponseHandler* listItem;
	iListIter.SetToFirst();
	while ( ( listItem = iListIter++ ) != 0 )
		{
		iList.Remove( *listItem );
		delete listItem;
		}
	delete iErrorHandler;
	delete iIPv4Receiver;
	delete iIPv6Receiver;
	delete iSender;
	delete iIPv4SocketContainer;
	delete iIPv6SocketContainer;
	iNATTraversal.SocketIdle( EFalse, iIPv4Socket );
	iIPv4Socket.Close();
	iNATTraversal.SocketIdle( EFalse, iIPv6Socket );
	iIPv6Socket.Close();
	iTransportUsers.Close();
	}
	
// -----------------------------------------------------------------------------
// CTransportUdp::Handle
// -----------------------------------------------------------------------------
//
TBool CTransportUdp::Handle( TUint32 aTransportId )
    {
    // Only interested about "protected" transports
    return ( ( aTransportId > 0 ) && 
             ( iTransportUsers.Find( aTransportId ) != KErrNotFound ) );
    }

// -----------------------------------------------------------------------------
// CTransportUdp::HandleMessage
// -----------------------------------------------------------------------------
//
TBool CTransportUdp::HandleMessage( const TSIPTransportParams& aParams,
                                    RStringF aProtocol,
								    const TInetAddr& /*aRemoteAddr*/,
								    TUint aLocalPort,
								    TBool aIsStrict )
    {
    // If not strict comparison, receiveonly mode has no significance
	return ( aProtocol == SIPStrings::StringF( SipStrConsts::EUDP ) && 
	         Match( aParams ) &&
	         ( aLocalPort == 0 || iSourcePort == aLocalPort ) &&
	         ( !aIsStrict || !iOnlyForReceiving ) );
	}

// -----------------------------------------------------------------------------
// CTransportUdp::ResetSocketL
// -----------------------------------------------------------------------------
//
void CTransportUdp::ResetSocketL()
	{
	iIPv4Socket.Close();
	CSocketContainer* tmp = CreateSocketContainerL( KAfInet, iIPv4Socket );
	delete iIPv4SocketContainer;
	iIPv4SocketContainer = tmp;
	
	iIPv6Socket.Close();
	tmp = CreateSocketContainerL( KAfInet6, iIPv6Socket );
	delete iIPv6SocketContainer;
	iIPv6SocketContainer = tmp;	
	}
		
// -----------------------------------------------------------------------------
// CTransportUdp::StopL
// -----------------------------------------------------------------------------
//
TBool CTransportUdp::StopL()
	{
	if ( iIPv4Receiver )
	    {
	    iIPv4Receiver->Cancel();
	    }	
	if ( iIPv6Receiver )
	    {
	    iIPv6Receiver->Cancel();
	    }
	iSender->Cancel();
		
	// If there's binding observers, notify that old socket
	// cannot be used anymore
	return NotifyFlowFailure();
	}

// -----------------------------------------------------------------------------
// CTransportUdp::ContinueL
// -----------------------------------------------------------------------------
//
void CTransportUdp::ContinueL()
	{
	WaitL();
	iSender->SendNextL();
	}

// -----------------------------------------------------------------------------
// CTransportUdp::SourcePort
// -----------------------------------------------------------------------------
//
TUint CTransportUdp::SourcePort()
	{
	return iSourcePort;
	}

// -----------------------------------------------------------------------------
// CTransportUdp::WaitL
// -----------------------------------------------------------------------------
//
void CTransportUdp::WaitL()
	{
	if ( iIPv4Receiver )
	    {
	    iIPv4Receiver->Listen();
	    }
	if ( iIPv6Receiver )
	    {
	    iIPv6Receiver->Listen();
	    }
	}

// -----------------------------------------------------------------------------
// CTransportUdp::SendToNetL
// -----------------------------------------------------------------------------
//
void CTransportUdp::SendToNetL( const TSIPTransportParams& aParams,
                                const TInetAddr& aAddress, 
							    CSIPMessage& aMessage,
							    TBool aForceUDP,
							    TUint aOrigTransport,
							    TRequestStatus &aStatus )
	{
	RefreshSocketContainerL( CSender::IpAddrType( aAddress ) );
	HandleMixedAddressFamilysL( aMessage, aAddress );
	
	iSender->SendL(
	    aParams, 
	    aMessage, 
	    aAddress, 
	    aOrigTransport, 
	    aStatus, 
	    EFalse, 
	    aForceUDP );
	}

// -----------------------------------------------------------------------------
// CTransportUdp::SendToNetL
// -----------------------------------------------------------------------------
//
void CTransportUdp::SendToNetL( const TSIPTransportParams& aParams,
                                const TInetAddr& aAddress,
							    CSIPMessage& aMessage,
							    TRequestStatus &aStatus )
	{
	RefreshSocketContainerL( CSender::IpAddrType( aAddress ) );
	HandleMixedAddressFamilysL( aMessage, aAddress );
	
	iSender->SendL( 
	    aParams, 
	    aMessage, 
	    aAddress, 
	    0, 
	    aStatus, 
	    EFalse, 
	    EFalse );
	}

// -----------------------------------------------------------------------------
// CTransportUdp::SendToNetL
// -----------------------------------------------------------------------------
//
void CTransportUdp::SendToNetL( const TSIPTransportParams& aParams,
                                const TInetAddr& aAddress, 
                                TTransactionId aId, 
							    CSIPMessage& aMessage, 
							    TRequestStatus &aStatus )
	{
	RefreshSocketContainerL( CSender::IpAddrType( aAddress ) );
	HandleMixedAddressFamilysL( aMessage, aAddress );
	
	CResponseHandler* listItem = 0;
	iListIter.SetToFirst();
	while ( ( listItem = iListIter++ ) != 0 )
		{
		if(listItem->Handle( aId ) ) 
			{
			listItem->SendL( aParams, 
			                 static_cast<CSIPResponse&>( aMessage ), 
			                 aAddress, 
			                 aStatus );
			return;
			}
		}
	CResponseHandler* responsehandler =
		CResponseHandler::NewL( *this, iTimerManager, *iServerResolver, aId,
							    iICMPErrorWaitTime );
	iList.AddLast( *responsehandler );
	responsehandler->SendL( aParams, 
	                        static_cast<CSIPResponse&>( aMessage ), 
	                        aAddress, 
	                        aStatus);
	}

// -----------------------------------------------------------------------------
// CTransportUdp::CancelSend
// -----------------------------------------------------------------------------
//
TBool CTransportUdp::CancelSend( TRequestStatus& aStatus )
	{
	return iSender->FindAndCancel( aStatus );
	}

// -----------------------------------------------------------------------------
// CTransportUdp::CancelAllRequests
// -----------------------------------------------------------------------------
//
void CTransportUdp::CancelAllRequests( CSIPConnection::TState aReason )
	{
	iSender->CancelAllRequests( aReason );
	}
    
// -----------------------------------------------------------------------------
// CTransportUdp::CancelResponseSend
// -----------------------------------------------------------------------------
//
TBool CTransportUdp::CancelResponseSend( 
    TTransactionId aId, 
    TBool aCancelAlso2xxResponses )
	{
	CResponseHandler* listItem = 0;
	iListIter.SetToFirst();
	while ( ( listItem = iListIter++ ) != 0 )
		{
		if ( listItem->Handle( aId ) )
			{
			return listItem->CancelAll( aId, aCancelAlso2xxResponses );
			}
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CTransportUdp::IsWaiting
// -----------------------------------------------------------------------------
//
TBool CTransportUdp::IsWaiting( TUint aProtocol, TUint aPort )
	{
	return ( aPort == SourcePort() && aProtocol == KProtocolInetUdp );
	}

// -----------------------------------------------------------------------------
// CTransportUdp::SocketContainer
// -----------------------------------------------------------------------------
//
CSocketContainer& CTransportUdp::SocketContainer()
	{
    if ( iIPv6SocketContainer  )
	    {
	    return *iIPv6SocketContainer;
	    } 
	return *iIPv4SocketContainer;
	}
// -----------------------------------------------------------------------------
// CTransportUdp::SocketContainer
// -----------------------------------------------------------------------------
//
CSocketContainer* CTransportUdp::SocketContainer( TUint aIPAddrFamily )
	{
	if ( aIPAddrFamily == KAfInet )
	    {
	    if ( iIPv4SocketContainer  )
	        {
	        return iIPv4SocketContainer;
	        }
	    return iIPv6SocketContainer;
	    }
	else
	    {
	    if ( iIPv6SocketContainer  )
	        {
	        return iIPv6SocketContainer;
	        } 	    
	    }
	return NULL; // Only IPv6 socket can be used for sending to a IPv6 address
	}

// -----------------------------------------------------------------------------
// CTransportUdp::ICMPErrorL
// -----------------------------------------------------------------------------
//
void CTransportUdp::ICMPErrorL( CSipConnectionMgr::TICMPError aICMPError,
							    const TInetAddr& aSourceAddr )
	{
	CResponseHandler* listItem = 0;
	iListIter.SetToFirst();
	while ( ( listItem = iListIter++ ) != 0 )
		{
		listItem->ICMPErrorL( aICMPError, aSourceAddr );
		}
	}

// -----------------------------------------------------------------------------
// CTransportUdp::RemoveResHandler
// -----------------------------------------------------------------------------
//
void CTransportUdp::RemoveResHandler( CResponseHandler* aHandler )
	{
	if ( !iList.IsEmpty() )
		{
		iList.Remove( *aHandler );
		delete aHandler;
		__SIP_LOG( "CResHandler removed" )
		}
	}

// -----------------------------------------------------------------------------
// CTransportUdp::ConnectionOpenL
// -----------------------------------------------------------------------------
//
TBool CTransportUdp::ConnectionOpenL()
	{
	User::Leave( KErrNotSupported );
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CTransportUdp::DisconnectedL
// -----------------------------------------------------------------------------
//
TBool CTransportUdp::DisconnectedL()
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CTransportUdp::GetSocketContainerL
// -----------------------------------------------------------------------------
//	
CSocketContainer& CTransportUdp::GetSocketContainerL( 
    const TInetAddr& aRemoteAddr )
    {
    CSocketContainer* socketContainer = 
        SocketContainer( CSender::IpAddrType( aRemoteAddr ) );
    __ASSERT_ALWAYS( socketContainer, User::Leave( KErrNotFound ) );
    return *socketContainer;
    }

// -----------------------------------------------------------------------------
// CTransportUdp::InformSendingStatus
// -----------------------------------------------------------------------------
//    
void CTransportUdp::InformSendingStatus( const TInetAddr& /*aRemoteAddr*/ )
    {
    __SIP_INT_LOG2( "CTransportUdp::InformSendingStatus (socket IPv4,sending)", 
                    iIPv4Socket.SubSessionHandle(), iIsSending )

    __SIP_INT_LOG2( "CTransportUdp::InformSendingStatus (socket IPv6,sending)", 
                    iIPv6Socket.SubSessionHandle(), iIsSending )
    
    iNATTraversal.SocketIdle( !iIsSending, iIPv4Socket );
    iNATTraversal.SocketIdle( !iIsSending, iIPv6Socket );
    }

// -----------------------------------------------------------------------------
// CTransportUdp::Sender
// -----------------------------------------------------------------------------
//
CSender* CTransportUdp::Sender( const TSIPTransportParams& aParams,
                                TUint aProtocol,
							    const TInetAddr& /*aRemoteAddr*/)
	{
	if ( aProtocol == Protocol() && Match( aParams ) )
		{
		return iSender;
		}
    return 0;
	}

// -----------------------------------------------------------------------------
// CTransportUdp::Remove
// -----------------------------------------------------------------------------
//
TBool CTransportUdp::Remove( TUint32 aTransportId )
    {
    TInt index = iTransportUsers.Find( aTransportId );	
    if ( index != KErrNotFound )
        {
        iTransportUsers.Remove( index );
        iTransportUsers.Compress();
        }
    
	// If no more transport users left and this is "protected" transport,
	// the whole transport can be removed
	return ( iTransportUsers.Count() == 0 && IsReservedTransport() );
    }
	
// -----------------------------------------------------------------------------
// CTransportUdp::AddUserL
// -----------------------------------------------------------------------------
//
TBool CTransportUdp::AddUserL( 
    const TSIPTransportParams& aParams, 
    TUint aReceivePort )
    {
    __ASSERT_ALWAYS( aReceivePort == iSourcePort, User::Leave( KErrArgument ) );
    
    // Don't allow duplicate entries
    User::LeaveIfError( iTransportUsers.InsertInOrder( aParams.TransportId() ) );
    
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CTransportUdp::AcquireUdpSocket
// -----------------------------------------------------------------------------
//    
RSocket* CTransportUdp::AcquireUdpSocket( 
    MSocketUsagePermissionObserver* aObserver,
    const TInetAddr& aRemoteAddr,
    TBool& aPermissionToUse )
    {    
    aPermissionToUse = EFalse;
    
    // If observer is not provided, caller cannot use the socket
    if ( aObserver )
        {
        // If sender is active, we cannot give permission until the
        // send is complete.
        aPermissionToUse = !iSender->IsActive();
    
        // When we give permission for the caller to use the socket, 
        // sender cannot send until UdpSocketReleased is called.
        iSender->SetSendPermission( !aPermissionToUse );
    
        iSocketUsagePermissionObserver = aObserver;
        }

    RSocket* socket = NULL;
    if ( aRemoteAddr.Address() && iIPv4SocketContainer )
        {
        socket = &iIPv4Socket;
        }
    else
        {
        socket = &iIPv6Socket;
        }
        
    return socket;
    }

// -----------------------------------------------------------------------------
// CTransportUdp::UdpSocketReleased
// -----------------------------------------------------------------------------
//                                        
void CTransportUdp::UdpSocketReleased( 
    MSocketUsagePermissionObserver* aObserver,
    RSocket& aSocket )
    {
    if ( aObserver && 
         ( iIPv4Socket.SubSessionHandle() == aSocket.SubSessionHandle() ||
           iIPv6Socket.SubSessionHandle() == aSocket.SubSessionHandle() ) )
        {
        iSocketUsagePermissionObserver = 0;
        iSender->SetSendPermission( ETrue );
        TRAP_IGNORE( iSender->SendNextL() )
        }
    }
        
// -----------------------------------------------------------------------------
// CTransportUdp::Remove
// -----------------------------------------------------------------------------
//
TInt CTransportUdp::Remove()
	{
	return iTransportOwner->RemoveTransport( this );
	}

// -----------------------------------------------------------------------------
// CTransportUdp::IapId
// -----------------------------------------------------------------------------
//
TUint32 CTransportUdp::IapId()
	{
	return iTransportOwner->IapId();
	}

// -----------------------------------------------------------------------------
// CTransportUdp::Sending
// -----------------------------------------------------------------------------
//
void CTransportUdp::Sending( TBool aIsSending )
    {
    __SIP_INT_LOG2( "CTransportUdp::Sending (socket IPv4,sending)", 
                    iIPv4Socket.SubSessionHandle(), iIsSending )

    __SIP_INT_LOG2( "CTransportUdp::Sending (socket IPv6,sending)", 
                    iIPv6Socket.SubSessionHandle(), iIsSending )
    
    iIsSending = aIsSending; 
    
    if ( iSocketUsagePermissionObserver )
        {
        iSocketUsagePermissionObserver->UsagePermissionChanged( iIPv4Socket, 
                                                                !iIsSending );
        iSocketUsagePermissionObserver->UsagePermissionChanged( iIPv6Socket, 
                                                                !iIsSending );
        }
       
    iNATTraversal.SocketIdle( !iIsSending, iIPv4Socket );
    iNATTraversal.SocketIdle( !iIsSending, iIPv6Socket );
    }
    
// -----------------------------------------------------------------------------
// CTransportUdp::ReRouteL
// -----------------------------------------------------------------------------
//
void CTransportUdp::ReRouteL( TUint aProtocol, 
                              COutgoingData* aData )
	{
	iTransportOwner->ReRouteL( aProtocol, aData );
	}

// -----------------------------------------------------------------------------
// CTransportUdp::ReceivedDataL
// -----------------------------------------------------------------------------
//
void CTransportUdp::ReceivedDataL( HBufC8* aData,
								   const TInetAddr& aRemoteAddr,
								   TBool aCompressed )
	{
	RecvL( aData, aRemoteAddr, aCompressed );
	}

// -----------------------------------------------------------------------------
// CTransportUdp::SigCompHandler
// -----------------------------------------------------------------------------
//
MSigCompController* CTransportUdp::SigCompHandler()
	{
	return SigCompressionHandler();
	}
// -----------------------------------------------------------------------------
// CTransportUdp::RefreshSocketContainerL
// -----------------------------------------------------------------------------
//
void CTransportUdp::RefreshSocketContainerL( TUint aFamily )
    {
    if ( aFamily == KAfInet && !iIPv4SocketContainer )
        {
    	iIPv4SocketContainer = CreateSocketContainerL( KAfInet, iIPv4Socket );
    	if ( iIPv4SocketContainer )
    	    {
    	    iIPv4Receiver = CReceiver::NewL( *this, KAfInet );
    	    iIPv4Receiver->Listen();
    	    }
        }
    if ( aFamily == KAfInet6 && !iIPv6SocketContainer )
        {
    	iIPv6SocketContainer = CreateSocketContainerL( KAfInet6, iIPv6Socket );
    	if ( iIPv6SocketContainer )
    	    {
    	    iIPv6Receiver = CReceiver::NewL( *this, KAfInet6 );
    	    iIPv6Receiver->Listen();
    	    }
        }        
    }
// -----------------------------------------------------------------------------
// CTransportUdp::CreateSocketContainerL
// -----------------------------------------------------------------------------
//
CSocketContainer* CTransportUdp::CreateSocketContainerL( 
    TUint aFamily, 
    RSocket& aSocket )
    {
    CSocketContainer* socketContainer( NULL );
 	TInetAddr addr;
	if ( iTransportOwner->GetLocalIPAddr( aFamily, addr ) )
	    {
    	User::LeaveIfError( 
    	    aSocket.Open( *iSocketServ, KAfInet, KSockDatagram,
    	                  KProtocolInetUdp, *iConnection ) );
    	socketContainer = CSocketContainer::NewLC( aSocket );
    	aSocket.SetOpt( KSoReuseAddr, KSolInetIp, 1 );
    	addr.SetPort( iSourcePort );
    	addr.SetFamily( aFamily );
    	TInt err = aSocket.Bind( addr );
    	if ( err == KErrInUse )
    		{
    		User::Leave( KErrSIPTransportFailure );
    		}
    	else
    		{
    		User::LeaveIfError( err );
    		}
    	CleanupStack::Pop( socketContainer );	
	    }
	return socketContainer;
    }
