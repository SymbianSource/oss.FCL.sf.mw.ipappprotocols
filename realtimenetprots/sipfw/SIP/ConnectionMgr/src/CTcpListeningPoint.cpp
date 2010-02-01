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
// Name          : CTcpListeningPoint.cpp
// Part of       : ConnectionMgr
// Version       : SIP/4.0 
//




#include "CTcpListeningPoint.h"
#include "CServerTransport.h"
#include "MTransactionFinder.h"
#include "MTransportOwner.h"
#include "MServerTaFactory.h"
#include "CListener.h"
#include "CErrorHandler.h"
#include "SipLogs.h"
#include "COutgoingData.h"
#include "CommonConsts.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "COwnerSettingsList.h"
#include "CSocketContainer.h"
#include "MSIPTransportRemovalObserver.h"
#include "siperr.h"


const TInt KTransportUsersGranularity = 2;

// -----------------------------------------------------------------------------
// CTcpListeningPoint::NewL
// -----------------------------------------------------------------------------
//
CTcpListeningPoint* CTcpListeningPoint::NewL(
    RSocketServ& aServer,
	MTransactionFinder& aFinder, 
	MServerTaFactory& aTaFactory,
	MTransportOwner& aTransportOwner,
	MTimerManager& aTimer,
	TUint aPort,
	TUint aTOne,
	CSIPServerResolver& aServerResolver,
	RConnection& aConnection,
	MSigCompController& aSigCompHandler,
	CNetworkInfo& aNetworkInfo,
	COwnerSettingsList& aSettingsList,
	const TSIPTransportParams& aTransportParams,
	CSIPNATTraversalController& aNATTraversal )
	{
	CTcpListeningPoint* self = NewLC( aServer, aFinder, aTaFactory, 
								aTransportOwner, aTimer, aPort, aTOne,
								aServerResolver, aConnection, aSigCompHandler,
								aNetworkInfo, aSettingsList, aTransportParams,
								aNATTraversal );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CTcpListeningPoint::NewLC
// -----------------------------------------------------------------------------
//
CTcpListeningPoint* CTcpListeningPoint::NewLC(
    RSocketServ& aServer,
	MTransactionFinder& aFinder, 
	MServerTaFactory& aTaFactory,
	MTransportOwner& aTransportOwner,
	MTimerManager& aTimer,
	TUint aPort,
	TUint aTOne,
	CSIPServerResolver& aServerResolver,
	RConnection& aConnection,
	MSigCompController& aSigCompHandler,
	CNetworkInfo& aNetworkInfo,
	COwnerSettingsList& aSettingsList,
	const TSIPTransportParams& aTransportParams,
	CSIPNATTraversalController& aNATTraversal )
	{
	CTcpListeningPoint* self =
		new (ELeave) CTcpListeningPoint( aServer, aFinder, aTaFactory, 
										 aTransportOwner, aTimer,
										 aPort, aTOne, aServerResolver,
										 aConnection, aSigCompHandler, 
										 aNetworkInfo, aSettingsList, 
										 aTransportParams, aNATTraversal );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CTcpListeningPoint::~CTcpListeningPoint
// -----------------------------------------------------------------------------
//
CTcpListeningPoint::~CTcpListeningPoint()
	{
	delete iListener;
	CServerTransport* listItem;
	iListIter.SetToFirst();
	while ( ( listItem = iListIter++ ) != 0 )
		{
		iList.Remove(*listItem);
		delete listItem;
		}
	delete iErrorHandler;
	iTransportUsers.Close();
	}

// -----------------------------------------------------------------------------
// CTcpListeningPoint::Handle
// -----------------------------------------------------------------------------
//
TBool CTcpListeningPoint::Handle( TUint32 aTransportId )
    {
    return ( ( aTransportId > 0 ) && 
             ( iTransportUsers.Find( aTransportId ) != KErrNotFound ) );
    }
   
// -----------------------------------------------------------------------------
// CTcpListeningPoint::HandleMessage
// -----------------------------------------------------------------------------
//
TBool CTcpListeningPoint::HandleMessage( const TSIPTransportParams& aParams,
                                         RStringF aProtocol,
										 const TInetAddr& aRemoteAddr,
										 TUint /*aLocalPort*/,
										 TBool /*aIsStrict*/ )
	{
	if ( aProtocol == SIPStrings::StringF(SipStrConsts::ETCP) && 
	     Match( aParams ) )
		{
		return ( FindServerTransport( aRemoteAddr ) != 0 );
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CTcpListeningPoint::SendToNetL
// -----------------------------------------------------------------------------
//
void CTcpListeningPoint::SendToNetL( const TSIPTransportParams& aParams,
                                     const TInetAddr& aAddress,
									 CSIPMessage& aMessage,
									 TBool /*aForceUDP*/,
									 TUint /*aOrigTransport*/,
									 TRequestStatus &aStatus )
	{
	HandleMixedAddressFamilysL( aMessage, aAddress );
	
	CServerTransport* listItem;
	iListIter.SetToFirst();
	while ( ( listItem = iListIter++ ) != 0 )
		{
		if ( listItem->Handle( aAddress ) )
			{
			listItem->SendL( aParams, aMessage, aStatus );
			return;
			}
		}
	TRequestStatus* stat = &aStatus;
	User::RequestComplete( stat, KErrNotFound );
	}

// -----------------------------------------------------------------------------
// CTcpListeningPoint::CancelSend
// -----------------------------------------------------------------------------
//
TBool CTcpListeningPoint::CancelSend( TRequestStatus& aStatus )
	{
	CServerTransport* listItem;
	iListIter.SetToFirst();
	while ( ( listItem = iListIter++ ) != 0 )
		{
		if ( listItem->CancelSend( aStatus ) )
			{
			return ETrue;
			}
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CTcpListeningPoint::AcceptedL
// -----------------------------------------------------------------------------
//
void CTcpListeningPoint::AcceptedL()
	{
	__SIP_LOG( "CTcpListeningPoint::AcceptedL " )
	if ( iWaitingAccept )
		{
		iErrorOn = EFalse;
		
		if ( IsRemoteAllowed( *iWaitingAccept ) )
		    {
		    iWaitingAccept->AcceptedL();
		    }
		else
		    {
		    RemoveWaitingAccept();
		    }
		    
		WaitL();
		}
	else
		{
		__ASSERT_DEBUG( EFalse,
			User::Panic( _L("CTcpListeningPoint::Wait() was not called"),
			KErrGeneral ) );
		}
	}

// -----------------------------------------------------------------------------
// CTcpListeningPoint::AcceptErrorL
// -----------------------------------------------------------------------------
//
void CTcpListeningPoint::AcceptErrorL( TInt /*aError*/ )
	{
	__SIP_LOG( "CTcpListeningPoint::AcceptErrorL" )
	iErrorOn = ETrue;
	RemoveWaitingAccept();
	}

// -----------------------------------------------------------------------------
// CTcpListeningPoint::WaitL
// -----------------------------------------------------------------------------
//
void CTcpListeningPoint::WaitL()
	{
	CServerTransport* transport =
			CServerTransport::NewL( *iSocketServ, 
			                        *this, 
			                        *iTimer, 
			                        iTimerOne,
			                        iNATTraversal,
			                        iSettingsList );
	iList.AddLast( *transport );
	iWaitingAccept = transport;
	iListener->Accept( transport->SocketContainer().Socket() );
	}

// -----------------------------------------------------------------------------
// CTcpListeningPoint::IsWaiting
// -----------------------------------------------------------------------------
//
TBool CTcpListeningPoint::IsWaiting( TUint aProtocol, TUint aPort )
	{
	return ( aProtocol == KProtocolInetTcp && iSourcePort == aPort );
	}

// -----------------------------------------------------------------------------
// CTcpListeningPoint::CancelAllRequests
// -----------------------------------------------------------------------------
//
void CTcpListeningPoint::CancelAllRequests( CSIPConnection::TState aReason )
	{
	CServerTransport* listItem;
	iListIter.SetToFirst();
	while ( ( listItem = iListIter++ ) != 0 )
		{
		listItem->CancelAllRequests( aReason );
		}
	}

// -----------------------------------------------------------------------------
// CTcpListeningPoint::RemoveL
// -----------------------------------------------------------------------------
//
void CTcpListeningPoint::RemoveL( CServerTransport* aTransport,
								  const TInetAddr& aRemoteAddr,
								  MSIPTransportRemovalObserver* aRemovalObserver )
	{
	RemoveResponseRoute( aRemoteAddr );
	__SIP_LOG( "CTcpListeningPoint::RemoveL" )
	if ( iWaitingAccept == aTransport )
		{
		iListener->Cancel();
		}
		
    TUint32 transportId = aTransport->TransportParams().TransportId();
    
	iList.Remove( *aTransport );
	delete aTransport;
	if ( iErrorOn )
		{
		WaitL();
		iErrorOn = EFalse;
		}
	
	// May lead to self deletion
    HandleShutdownCompletion( aRemovalObserver, transportId );
	}

// -----------------------------------------------------------------------------
// CTcpListeningPoint::Disconnect
// -----------------------------------------------------------------------------
//
void CTcpListeningPoint::Disconnect( const TInetAddr& aRemoteHost )
	{
	CServerTransport* listItem;
	while ( ( listItem = iListIter++ ) != 0 )
		{
		if ( listItem->Handle( aRemoteHost ) )
			{
			// If this is reserved or persistent transport,
			// only all send operations are cleared
			if ( IsReservedTransport() || listItem->IsPersistent() )
			    {
			    listItem->CancelAllRequests( KErrSIPTransportFailure );
			    }
			else
			    {
			    iList.Remove( *listItem );
			    delete listItem;
			    }
			}
		}
	}

// -----------------------------------------------------------------------------
// CTcpListeningPoint::ReceiveL
// -----------------------------------------------------------------------------
//
void CTcpListeningPoint::ReceiveL( CSIPMessage* aMessage,
								   const TInetAddr& aRemoteAddr,
								   TInt aError,
								   TBool aCompressed )
	{
	RecvL(aMessage, aRemoteAddr, aError, aCompressed);
	}

// -----------------------------------------------------------------------------
// CTcpListeningPoint::GetSocketContainerL
// -----------------------------------------------------------------------------
//	
CSocketContainer& CTcpListeningPoint::GetSocketContainerL( 
    const TInetAddr& aRemoteAddr )
    {
    CServerTransport* serverTransport = FindServerTransport( aRemoteAddr );
    __ASSERT_ALWAYS( serverTransport, User::Leave( KErrNotFound ) );
    
    return serverTransport->SocketContainer();
    }
    
// -----------------------------------------------------------------------------
// CTcpListeningPoint::InformSendingStatus
// -----------------------------------------------------------------------------
//
void CTcpListeningPoint::InformSendingStatus( const TInetAddr& aRemoteAddr )
    {
    CServerTransport* serverTransport = FindServerTransport( aRemoteAddr );  
    if ( serverTransport )
        {
        serverTransport->InformSendingStatus();
        }
    }
    
// -----------------------------------------------------------------------------
// CTcpListeningPoint::SetPersistencyL
// -----------------------------------------------------------------------------
//	
void CTcpListeningPoint::SetPersistencyL( 
    const TInetAddr& aRemoteAddr, 
    TBool aIsPersistent,
    MSIPNATBindingObserver* aBindingObserver )
    {
    CServerTransport* serverTransport = FindServerTransport( aRemoteAddr );
	if ( serverTransport )
	    {
	    serverTransport->SetPersistency( aIsPersistent );
	    CTransport::SetPersistencyL( aRemoteAddr, aIsPersistent, aBindingObserver );
	    }
    }
    
// -----------------------------------------------------------------------------
// CTcpListeningPoint::Sender
// -----------------------------------------------------------------------------
//
CSender* CTcpListeningPoint::Sender( const TSIPTransportParams& /*aParams*/,
                                     TUint aProtocol,
									 const TInetAddr& aRemoteAddr )
	{
	if ( Protocol() == aProtocol )
		{
		CServerTransport* serverTransport = FindServerTransport( aRemoteAddr );
		if ( serverTransport )
		    {
		    return serverTransport->Sender();
		    }
		}
	return 0;
	}

// -----------------------------------------------------------------------------
// CTcpListeningPoint::IsReservedTransportOwner
// -----------------------------------------------------------------------------
//    
TBool CTcpListeningPoint::IsReservedTransportOwner() const
    {
    return IsReservedTransport();
    }
    
// -----------------------------------------------------------------------------
// CTcpListeningPoint::Remove
// -----------------------------------------------------------------------------
//
TBool CTcpListeningPoint::Remove( TUint32 aTransportId )
    {	
	TInt index = iTransportUsers.Find( aTransportId );	
    if ( index != KErrNotFound )
        {
        iTransportUsers.Remove( index );
        iTransportUsers.Compress();
        }
    
	return RemovalAllowed();
    }

// -----------------------------------------------------------------------------
// CTcpListeningPoint::Remove
// -----------------------------------------------------------------------------
// 
TBool CTcpListeningPoint::Remove( 
    MSIPNATBindingObserver& aSIPNATBindingObserver )
    {
    CServerTransport* listItem;
	iListIter.SetToFirst();
	while ( ( listItem = iListIter++ ) != 0 )
		{
		// Cleanup persistent server transports.
        if ( listItem->Removal( aSIPNATBindingObserver ) )
            {
            RemoveServerTransport( listItem );
            }
		}
    return CTransport::Remove( aSIPNATBindingObserver );
    }

// -----------------------------------------------------------------------------
// CTcpListeningPoint::Shutdown
// -----------------------------------------------------------------------------
//   
TBool CTcpListeningPoint::Shutdown( 
    TUint32 aTransportId,
    MSIPTransportRemovalObserver* aRemovalObserver )
    {
    CServerTransport* listItem;
	iListIter.SetToFirst();
	while ( ( listItem = iListIter++ ) != 0 )
	    {
	    if ( listItem->TransportParams().TransportId() == aTransportId )
	        {
	        if ( listItem->Shutdown( aRemovalObserver ) )
	            {
	            iShutdownCount++;
	            }
	        else
	            {
	            RemoveServerTransport( listItem );
	            }
	        }
	    }
	
	return ( iShutdownCount > 0 );
    }
    
// -----------------------------------------------------------------------------
// CTcpListeningPoint::AddUserL
// -----------------------------------------------------------------------------
//    
TBool CTcpListeningPoint::AddUserL(
    const TSIPTransportParams& aParams, 
    TUint aReceivePort )
    {
    __ASSERT_ALWAYS( aReceivePort == iSourcePort, User::Leave( KErrArgument ) );
    
    // Don't allow duplicate entries
    User::LeaveIfError( iTransportUsers.InsertInOrder( aParams.TransportId() ) );
    
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CTcpListeningPoint::ReRouteL
// -----------------------------------------------------------------------------
//
void CTcpListeningPoint::ReRouteL( COutgoingData* data )
	{
	iTransportOwner->ReRouteL( Protocol(), data );
	}

// -----------------------------------------------------------------------------
// CTcpListeningPoint::IapId
// -----------------------------------------------------------------------------
//
TUint32 CTcpListeningPoint::IapId()
	{
	return iTransportOwner->IapId();
	}

// -----------------------------------------------------------------------------
// CTcpListeningPoint::SigCompHandler
// -----------------------------------------------------------------------------
//
MSigCompController* CTcpListeningPoint::SigCompHandler()
	{
	return SigCompressionHandler();
	}

// -----------------------------------------------------------------------------
// CTcpListeningPoint::GetLocalAddrL
// -----------------------------------------------------------------------------
//
void CTcpListeningPoint::GetLocalAddrL( TInetAddr& aAddr )
	{
	iTransportOwner->GetLocalAddrL( aAddr, NULL );
	}

// -----------------------------------------------------------------------------
// CTcpListeningPoint::FindServerTransport
// -----------------------------------------------------------------------------
//
CServerTransport* CTcpListeningPoint::FindServerTransport( 
    const TInetAddr& aRemoteAddr )
    {
	CServerTransport* listItem;
	iListIter.SetToFirst();
	while ( ( listItem = iListIter++ ) != 0 )
	    {
		if ( listItem->Handle( aRemoteAddr ) )
			{
			return listItem;
			}
	    }
    return 0;	
    }

// -----------------------------------------------------------------------------
// CTcpListeningPoint::IsRemoteAllowed
// -----------------------------------------------------------------------------
//    
TBool CTcpListeningPoint::IsRemoteAllowed( CServerTransport& aTransport )
    {
    TInetAddr remote;
    aTransport.SocketContainer().Socket().RemoteName( remote );
    TUint32 tpId = iSettingsList.FindTransportId( remote );
    
    // All remote addresses are allowed if transport is not reserved one
    return ( !IsReservedTransport() || Handle( tpId ) );
    }

// -----------------------------------------------------------------------------
// CTcpListeningPoint::RemoveWaitingAccept
// -----------------------------------------------------------------------------
//
void CTcpListeningPoint::RemoveWaitingAccept()
    {
    if ( iWaitingAccept )
        {
        iList.Remove( *iWaitingAccept );
	    delete iWaitingAccept; 
	    iWaitingAccept = 0;
        }
    }
    
// -----------------------------------------------------------------------------
// CTcpListeningPoint::HandleShutdownCompletion
// -----------------------------------------------------------------------------
//     
void CTcpListeningPoint::HandleShutdownCompletion( 
    MSIPTransportRemovalObserver* aRemovalObserver,
    TUint32 aTransportId )
    {
    if ( aRemovalObserver && iShutdownCount > 0 )
        {
        aRemovalObserver->RemovalCompleted( aTransportId );
        iShutdownCount--;
        if ( !iShutdownCount && RemovalAllowed() )
            {
            iTransportOwner->RemoveTransport( this );
            }
        }
    }

// -----------------------------------------------------------------------------
// CTcpListeningPoint::RemovalAllowed
// -----------------------------------------------------------------------------
//   
TBool CTcpListeningPoint::RemovalAllowed()
    {
    // If no more transport users left and this is reserved listening point,
	// whole transport can be removed
    return ( iTransportUsers.Count() == 0 && IsReservedTransport() );
    }

// -----------------------------------------------------------------------------
// CTcpListeningPoint::RemoveServerTransport
// -----------------------------------------------------------------------------
//   
void CTcpListeningPoint::RemoveServerTransport( 
    CServerTransport* aServerTransport )
    {
    // Leads to deletion of aServerTransport
    TBool removed( EFalse );
	TRAP_IGNORE( ( removed = aServerTransport->DisconnectedL() ) )
    if ( !removed )
        {
        iList.Remove( *aServerTransport );
	    delete aServerTransport;
        }
    }
    
// -----------------------------------------------------------------------------
// CTcpListeningPoint::ConstructL
// -----------------------------------------------------------------------------
//
void CTcpListeningPoint::ConstructL()
	{
	iListener = 
	    CListener::NewL( *this, *iSocketServ, iSourcePort, *iConnection );
	iErrorHandler = CTransportErrorHandler::NewL( *this );
	iTransportUsers.AppendL( iTransportParams.TransportId() );
	}

// -----------------------------------------------------------------------------
// CTcpListeningPoint::CTcpListeningPoint
// -----------------------------------------------------------------------------
//
CTcpListeningPoint::CTcpListeningPoint(
    RSocketServ& aServer,
	MTransactionFinder& aFinder, 
	MServerTaFactory& aTaFactory,
	MTransportOwner& aTransportOwner,
	MTimerManager& aTimer,
	TUint aPort,
	TUint aTOne,
	CSIPServerResolver& aServerResolver,
	RConnection& aConnection,
	MSigCompController& aSigCompHandler,
	CNetworkInfo& aNetworkInfo,
	COwnerSettingsList& aSettingsList,
	const TSIPTransportParams& aTransportParams,
	CSIPNATTraversalController& aNATTraversal ) :
	CTransport( aFinder, aTaFactory, aTransportOwner, aServerResolver,
	            aSigCompHandler, aTransportParams, aNetworkInfo, 
	            aSettingsList, aNATTraversal ),
	iSocketServ( &aServer ),
	iTimer( &aTimer ),
	iSourcePort( aPort ),
	iTimerOne( aTOne ),
	iErrorOn( ETrue ),
	iConnection( &aConnection ),
	iList( CServerTransport::iOffset ), iListIter( iList ),
	iTransportUsers( KTransportUsersGranularity )
	{
	}
