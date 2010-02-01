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
// Name          : CTransportTcp.cpp
// Part of       : ConnectionMgr
// Version       : SIP/6.0 
//




#include "CTransportTcp.h"
#include "CSipConnection.h"
#include "CErrorHandler.h"
#include "MTransactionFinder.h"
#include "MTransportOwner.h"
#include "MServerTaFactory.h"
#include "CSender.h"
#include "CSenderTcp.h"
#include "CReceiverTcp.h"
#include "CListener.h"
#include "CTcpTransState.h"
#include "CTcpTransConnected.h"
#include "CTcpTransConnecting.h"
#include "CommonConsts.h"
#include "siperr.h"
#include "NetworkInfo.h"
#include "COwnerSettingsList.h"
#include "CSocketContainer.h"
#include "MSIPTransportRemovalObserver.h"
#include <sipnattraversalcontroller.h>


// -----------------------------------------------------------------------------
// CTransportTcp::NewL
// -----------------------------------------------------------------------------
//
CTransportTcp* CTransportTcp::NewL(RSocketServ& aServer,
				MTransactionFinder& aFinder, 
				MServerTaFactory& aTaFactory,
				MTransportOwner& aTransportOwner,
				MTimerManager& aTimer,
				const TSIPTransportParams& aTransportParams,
				const TInetAddr& aRemoteAddr,
				TUint aTOne,
				CSIPServerResolver& aServerResolver,
				RConnection& aConnection,
				MSigCompController& aSigCompHandler,
				TUint aLocalSendPort,
				CNetworkInfo& aNetworkInfo,
				COwnerSettingsList& aSettingsList,
				CSIPNATTraversalController& aNATTraversal)
	{
	CTransportTcp* self = NewLC(aServer, aFinder, aTaFactory, 
								aTransportOwner, aTimer, aTransportParams,
								aRemoteAddr, aTOne, aServerResolver, 
								aConnection, aSigCompHandler, aLocalSendPort,
								aNetworkInfo, aSettingsList, aNATTraversal);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CTransportTcp::NewLC
// -----------------------------------------------------------------------------
//
CTransportTcp* CTransportTcp::NewLC(RSocketServ& aServer,
				MTransactionFinder& aFinder, 
				MServerTaFactory& aTaFactory,
				MTransportOwner& aTransportOwner,
				MTimerManager& aTimer,
				const TSIPTransportParams& aTransportParams,
				const TInetAddr& aRemoteAddr,
				TUint aTOne,
				CSIPServerResolver& aServerResolver,
				RConnection& aConnection,
				MSigCompController& aSigCompHandler,
				TUint aLocalSendPort,
				CNetworkInfo& aNetworkInfo,
				COwnerSettingsList& aSettingsList,
				CSIPNATTraversalController& aNATTraversal)
	{
	CTransportTcp* self = new (ELeave) CTransportTcp(aFinder, aTaFactory, 
										aTransportOwner, aTransportParams,
										aServerResolver, aSigCompHandler,
										aNetworkInfo, aSettingsList, 
										aNATTraversal);
	CleanupStack::PushL(self);
	self->ConstructL(aServer, 
	                 aTimer, 
	                 aRemoteAddr, 
	                 aTOne, 
	                 aConnection, 
	                 aLocalSendPort);
	return self;
	}

// -----------------------------------------------------------------------------
// CTransportTcp::CTransportTcp
// -----------------------------------------------------------------------------
//
CTransportTcp::CTransportTcp(MTransactionFinder& aFinder, 
							 MServerTaFactory& aTaFactory,
							 MTransportOwner& aTransportOwner,
							 const TSIPTransportParams& aTransportParams,
							 CSIPServerResolver& aServerResolver,
							 MSigCompController& aSigCompHandler,							 
							 CNetworkInfo& aNetworkInfo,
							 COwnerSettingsList& aSettingsList,
							 CSIPNATTraversalController& aNATTraversal) : 
	CTransport(aFinder, aTaFactory, aTransportOwner,
			   aServerResolver, aSigCompHandler,
			   aTransportParams, aNetworkInfo, 
			   aSettingsList, aNATTraversal),
	iIsSending( EFalse )
	{
	}

// -----------------------------------------------------------------------------
// CTransportTcp::ConstructL
// -----------------------------------------------------------------------------
//
void CTransportTcp::ConstructL( RSocketServ& aServer,
							    MTimerManager& aTimer,
    							const TInetAddr& aRemoteAddr,
    							TUint aTOne,
    							RConnection& aConnection,
    							TUint aLocalSendPort )
	{
	iTimerValue = (64 * aTOne);
	iSocketServ = &aServer;
	iConnection = &aConnection;
	iTimer = &aTimer;
	iRemoteAddr = aRemoteAddr;
	iConnectedState = CTcpTransportConnected::NewL( iTransportParams );
	iConnectingState = CTcpTransportConnecting::NewL( iTransportParams );
	iConnectingState->SetNeighborState( iConnectedState );
	SetCurrentState( iConnectingState );
	iErrorHandler =	CTransportErrorHandler::NewL( *this );
	User::LeaveIfError( iSocket.Open( *iSocketServ, KAfInet, KSockStream,
									  KProtocolInetTcp, *iConnection ) );

    iSocketContainer = CSocketContainer::NewL( iSocket );
    
    // If local send port is defined, socket is binded to it								
	if ( aLocalSendPort > 0 && aLocalSendPort != KDefaultSipPort )
	    {	       
	    iSocket.SetOpt( KSoReuseAddr, KSolInetIp, 1 );
	                                                                                  
	    TInetAddr address;
    	iTransportOwner->GetLocalAddrL( address, &aRemoteAddr );
    	address.SetPort( aLocalSendPort );
    	TInt err = iSocket.Bind( address );
    	if ( err == KErrInUse )
    		{
    		User::Leave( KErrSIPTransportFailure );
    		}
    	else
    		{
    		User::LeaveIfError( err );
    		}
	    }
	    
	iReceiver = CReceiverTcp::NewL( *this );
	
	iSender = CSenderTcp::NewL( *this, iSettingsList );
	}


// -----------------------------------------------------------------------------
// CTransportTcp::~CTransportTcp
// -----------------------------------------------------------------------------
//
CTransportTcp::~CTransportTcp()
	{
	HandleShutdownCompletion();
	StopTimer();
	delete iErrorHandler;
	delete iConnectedState;
	delete iConnectingState;
	delete iReceiver;
	delete iSender;
	delete iSocketContainer;
	iNATTraversal.SocketIdle( EFalse, iSocket );
	iSocket.Close();
	}
	
// -----------------------------------------------------------------------------
// CTransportTcp::HandleMessage
// -----------------------------------------------------------------------------
//
TBool CTransportTcp::HandleMessage(const TSIPTransportParams& aParams,
                                   RStringF aProtocol,
								   const TInetAddr& aRemoteAddr,
								   TUint /*aLocalPort*/,
								   TBool /*aIsStrict*/)
	{ 
	return ( iCurrentState->HandleMessage(aProtocol, aRemoteAddr, this) &&
	         Match( aParams ) );
	}

// -----------------------------------------------------------------------------
// CTransportTcp::SourcePort
// -----------------------------------------------------------------------------
//
TUint CTransportTcp::SourcePort()
	{
	return iSocket.LocalPort();
	}

// -----------------------------------------------------------------------------
// CTransportTcp::SendToNetL
// -----------------------------------------------------------------------------
//
void CTransportTcp::SendToNetL(const TSIPTransportParams& aParams,
                               const TInetAddr& aAddress, 
							   CSIPMessage& aMessage, 
							   TBool /*aForceUDP*/,
							   TUint aOrigTransport,
							   TRequestStatus &aStatus)
	{
	HandleMixedAddressFamilysL( aMessage, aAddress );
	
	iCurrentState->SendToNetL(aParams,
	                          aMessage,
							  aAddress,
							  aStatus,
							  aOrigTransport,
							  this);
	}

// -----------------------------------------------------------------------------
// CTransportTcp::CancelSend
// -----------------------------------------------------------------------------
//
TBool CTransportTcp::CancelSend(TRequestStatus& aStatus)
	{
	return iCurrentState->CancelSend(aStatus, this);
	}

// -----------------------------------------------------------------------------
// CTransportTcp::WaitL
// -----------------------------------------------------------------------------
//
void CTransportTcp::WaitL()
	{
	iCurrentState->Wait(this);
	}

// -----------------------------------------------------------------------------
// CTransportTcp::IsWaiting
// -----------------------------------------------------------------------------
//
TBool CTransportTcp::IsWaiting(TUint /*aProtocol*/, TUint /*aPort*/)
	{
	return EFalse;	
	}

// -----------------------------------------------------------------------------
// CTransportTcp::Disconnect
// -----------------------------------------------------------------------------
//
void CTransportTcp::Disconnect(const TInetAddr& /*aRemoteHost*/)
	{
	// If this is reserved or persistent transport, 
	// only all send operations are cleared
	if ( IsReservedTransport() || IsPersistent() )
	    {
	    iSender->CancelAllRequests( KErrSIPTransportFailure );
	    }
	else
	    {
	    Remove();
	    }
	}

// -----------------------------------------------------------------------------
// CTransportTcp::Shutdown
// -----------------------------------------------------------------------------
//	     
TBool CTransportTcp::Shutdown( 
    TUint32 /*aTransportId*/,
    MSIPTransportRemovalObserver* aRemovalObserver )
    {
    if ( aRemovalObserver )
        {
        SetShutdownObserver( aRemovalObserver );
        iReceiver->Cancel();
	    iSender->Cancel();
        iReceiver->Shutdown();
        return ETrue;
        }
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// CTransportTcp::PersistentRemoteAddr
// -----------------------------------------------------------------------------
// 
TInetAddr* CTransportTcp::PersistentRemoteAddr( 
    MSIPNATBindingObserver* aBindingObserver )
    {
    TInt index = iBindingObservers.FindInAddressOrder( aBindingObserver );
    if ( index >= 0 && index < iBindingObservers.Count() )
        {
        return &iRemoteAddr;
        }
    return 0;
    }
    
// -----------------------------------------------------------------------------
// CTransportTcp::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CTransportTcp::TimerExpiredL(TTimerId /*aTimerId*/, TAny* /*aTimerParam*/)
	{
	// Reserved transport uses persistent tcp connection and therefore
	// only send operations are cleared when timer expires
	if ( IsReservedTransport() || IsPersistent() )
	    {
	    iSender->CancelAllRequests( KErrSIPTransportFailure );
	    }
	else
	    {
    	iCurrentState->TimerExpiredL( this );
	    }
	}

// -----------------------------------------------------------------------------
// CTransportTcp::StartTimerL
// -----------------------------------------------------------------------------
//
void CTransportTcp::StartTimerL()
	{
	iTimer->Stop(iMyTimerId);
	iMyTimerId = iTimer->StartL(this, iTimerValue);
	}

// -----------------------------------------------------------------------------
// CTransportTcp::StopTimer
// -----------------------------------------------------------------------------
//
void CTransportTcp::StopTimer()
    {
    iTimer->Stop(iMyTimerId);
    }
    
// -----------------------------------------------------------------------------
// CTransportTcp::CancelAllRequests
// -----------------------------------------------------------------------------
//
void CTransportTcp::CancelAllRequests(CSIPConnection::TState aReason)
	{
	iCurrentState->CancelAllRequests(aReason, this);
	}

// -----------------------------------------------------------------------------
// CTransportTcp::SocketContainer
// -----------------------------------------------------------------------------
//
CSocketContainer& CTransportTcp::SocketContainer() 
    {
    return *iSocketContainer;
    }
    
// -----------------------------------------------------------------------------
// CTransportTcp::SocketContainer
// -----------------------------------------------------------------------------
//
CSocketContainer* CTransportTcp::SocketContainer( 
    TUint /*aIPAddrFamily*/ )
    {
    return iSocketContainer;
    }     
    
// -----------------------------------------------------------------------------
// CTransportTcp::ReceivedDataL
// -----------------------------------------------------------------------------
//
void CTransportTcp::ReceivedDataL(CSIPMessage* aMessage,
								  const TInetAddr& aRemoteAddr,
								  TInt aError,
								  TBool aCompressed)
	{
	// As starting timer can leave, do it before passing ownership of aMessage
	StartTimerL();
	RecvL(aMessage, aRemoteAddr, aError, aCompressed);
	}

// -----------------------------------------------------------------------------
// CTransportTcp::ResetSocketL
// -----------------------------------------------------------------------------
//
void CTransportTcp::ResetSocketL()
	{
	}

// -----------------------------------------------------------------------------
// CTransportTcp::ContinueL
// -----------------------------------------------------------------------------
//
void CTransportTcp::ContinueL()
	{
	}

// -----------------------------------------------------------------------------
// CTransportTcp::StopL
// -----------------------------------------------------------------------------
//
TBool CTransportTcp::StopL()
	{
	TBool removed( EFalse );
	
	if (iCurrentState == iConnectingState)
		{
		iTransportOwner->RemoveTransport(this);
		removed = ETrue;
		}
		
    return removed;
	}

// -----------------------------------------------------------------------------
// CTransportTcp::ConnectionOpenL
// -----------------------------------------------------------------------------
//
TBool CTransportTcp::ConnectionOpenL()
	{
	iTransportOwner->TcpConnected(iRemoteAddr);
	iCurrentState->ConnectionOpenL(this);
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CTransportTcp::SetCurrentState
// -----------------------------------------------------------------------------
//
void CTransportTcp::SetCurrentState(CTcpTransportState* aState)
	{
	iCurrentState = aState;
	}

// -----------------------------------------------------------------------------
// CTransportTcp::ConnectSocket
// -----------------------------------------------------------------------------
//
void CTransportTcp::ConnectSocket()
    {
    iReceiver->Connect( iRemoteAddr );
    }
    
// -----------------------------------------------------------------------------
// CTransportTcp::TransportOwner
// -----------------------------------------------------------------------------
//
MTransportOwner* CTransportTcp::TransportOwner()
	{
	return iTransportOwner;
	}
	
// -----------------------------------------------------------------------------
// CTransportTcp::GetSocketContainerL
// -----------------------------------------------------------------------------
//	
CSocketContainer& CTransportTcp::GetSocketContainerL( 
    const TInetAddr& /*aRemoteAddr*/ )
    {
    return *iSocketContainer;
    }
    
// -----------------------------------------------------------------------------
// CTransportTcp::InformSendingStatus
// -----------------------------------------------------------------------------
//    
void CTransportTcp::InformSendingStatus( const TInetAddr& /*aRemoteAddr*/ )
    {
    iNATTraversal.SocketIdle( !iIsSending, iSocket );
    }

// -----------------------------------------------------------------------------
// CTransportTcp::Sender
// -----------------------------------------------------------------------------
//
CSender* CTransportTcp::Sender(const TSIPTransportParams& aParams,
                               TUint aProtocol, 
                               const TInetAddr& aRemoteAddr)
	{
	if ( Protocol() == aProtocol && 
	     aRemoteAddr.CmpAddr( iRemoteAddr ) && 
	     Match( aParams ) )
		{
		return iSender;
		}
	return 0;
	}

// -----------------------------------------------------------------------------
// CTransportTcp::Remove
// -----------------------------------------------------------------------------
//
TInt CTransportTcp::Remove()
	{
	return iTransportOwner->RemoveTransport( this );
	}

// -----------------------------------------------------------------------------
// CTransportTcp::IapId
// -----------------------------------------------------------------------------
//
TUint32 CTransportTcp::IapId()
	{
	return iTransportOwner->IapId();
	}
	
// -----------------------------------------------------------------------------
// CTransportTcp::Sending
// -----------------------------------------------------------------------------
//
void CTransportTcp::Sending( TBool aIsSending )
    {
    iIsSending = aIsSending;
    iNATTraversal.SocketIdle( !iIsSending, iSocket );
    }
	
// -----------------------------------------------------------------------------
// CTransportTcp::ReRouteL
// -----------------------------------------------------------------------------
//
void CTransportTcp::ReRouteL(TUint aProtocol, 
                             COutgoingData* aData)
	{
	iTransportOwner->ReRouteL(aProtocol, aData);
	}

// -----------------------------------------------------------------------------
// CTransportTcp::DisconnectedL
// -----------------------------------------------------------------------------
//
TBool CTransportTcp::DisconnectedL()
	{
	HandleShutdownCompletion();	
	
	// Flow failure notification may lead to immediate self deletion
	if ( !NotifyFlowFailure() )
	    {
	    iCurrentState->DisconnectedL( this );
	    }
	
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CTransportTcp::GetRemoteAddr
// -----------------------------------------------------------------------------
//
void CTransportTcp::GetRemoteAddr(TInetAddr& aAddr)
	{
	aAddr = iRemoteAddr;
	}

// -----------------------------------------------------------------------------
// CTransportTcp::SigCompHandler
// -----------------------------------------------------------------------------
//
MSigCompController* CTransportTcp::SigCompHandler()
	{
	return SigCompressionHandler();
	}
    
// -----------------------------------------------------------------------------
// CTransportTcp::GetSender
// -----------------------------------------------------------------------------
//   
CSender* CTransportTcp::GetSender()
    {
    return iSender;
    }

// -----------------------------------------------------------------------------
// CTransportTcp::SetShutdownObserver
// -----------------------------------------------------------------------------
//
void CTransportTcp::SetShutdownObserver( 
    MSIPTransportRemovalObserver* aRemovalObserver )
    {
    iShutdownObserver = aRemovalObserver;
    }
    
// -----------------------------------------------------------------------------
// CTransportTcp::HandleShutdownCompletion
// -----------------------------------------------------------------------------
//     
void CTransportTcp::HandleShutdownCompletion()
    {
    if ( iShutdownObserver )
        {
        iShutdownObserver->RemovalCompleted( iTransportParams.TransportId() );
        iShutdownObserver = 0;
        }
    }
