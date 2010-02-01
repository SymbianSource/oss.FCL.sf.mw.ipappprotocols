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
// Name          : CServerTransport.cpp
// Part of       : ConnectionMgr
// Version       : SIP/4.0 
//




#include "CServerTransport.h"
#include "MServerTransportOwner.h"
#include "CSender.h"
#include "CSenderTcp.h"
#include "CReceiverTcp.h"
#include "sipmessage.h"
#include "CommonConsts.h"
#include "CSocketContainer.h"
#include "MSIPTransportRemovalObserver.h"
#include "siperr.h"
#include <sipnattraversalcontroller.h>

const TInt CServerTransport::iOffset = _FOFF(CServerTransport, iLink);

// -----------------------------------------------------------------------------
// CServerTransport::NewL
// -----------------------------------------------------------------------------
//
CServerTransport* CServerTransport::NewL( 
    RSocketServ& aServer, 
    MServerTransportOwner& aMyOwner,
    MTimerManager& aTimer,
    TUint aTOne,
    CSIPNATTraversalController& aNATTraversal,
    COwnerSettingsList& aSettingsList )
	{
	CServerTransport* self = NewLC( aServer, 
	                                aMyOwner, 
	                                aTimer, 
	                                aTOne, 
	                                aNATTraversal,
	                                aSettingsList );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CServerTransport::NewLC
// -----------------------------------------------------------------------------
//
CServerTransport* CServerTransport::NewLC( 
    RSocketServ& aServer, 
    MServerTransportOwner& aMyOwner,
    MTimerManager& aTimer,
    TUint aTOne,
    CSIPNATTraversalController& aNATTraversal,
    COwnerSettingsList& aSettingsList )
	{
	CServerTransport* self = new (ELeave) CServerTransport( aMyOwner, 
	                                                        aTimer, 
	                                                        aNATTraversal,
	                                                        aSettingsList );
	CleanupStack::PushL( self );
	self->ConstructL( aServer, aTOne );
	return self;
	}

// -----------------------------------------------------------------------------
// CServerTransport::SendL
// -----------------------------------------------------------------------------
//
void CServerTransport::SendL( const TSIPTransportParams& aParams,
                              CSIPMessage& aMessage, 
                              TRequestStatus &aStatus )
	{
	if ( iAccepted && iSender )
		{
		// Remember currently used tpparams
		iTransportParams = aParams;
		
		iSender->SendL( aParams, aMessage, aStatus );
		StartTimerL();
		}
	}
	
// -----------------------------------------------------------------------------
// CServerTransport::Handle
// -----------------------------------------------------------------------------
//
TBool CServerTransport::Handle( const TInetAddr& aRemoteAddr )
	{
	if ( iAccepted )
		{
		TInetAddr connect;
		iSocket.RemoteName( connect );
		if ( connect.CmpAddr( aRemoteAddr ) )
			{
			return ETrue;
			}
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CServerTransport::AcceptedL
// -----------------------------------------------------------------------------
//
void CServerTransport::AcceptedL()
	{
	__ASSERT_ALWAYS( !iSender && !iReceiver, User::Leave( KErrInUse ) );
	
	iSocket.RemoteName( iRemoteAddr );
	
	iReceiver = CReceiverTcp::NewL( *this );
	iSender = CSenderTcp::NewL( *this, iSettingsList );

	iReceiver->ReceiveDataL();
	iAccepted = ETrue;
	}

// -----------------------------------------------------------------------------
// CServerTransport::TransportParams
// -----------------------------------------------------------------------------
//	
TSIPTransportParams& CServerTransport::TransportParams()
    {
    return iTransportParams;
    }
    
// -----------------------------------------------------------------------------
// CServerTransport::CancelAllRequests
// -----------------------------------------------------------------------------
//
void CServerTransport::CancelAllRequests( TInt aReason )
	{
	if ( iSender )
		{
		iSender->CancelAllRequests( aReason );
		}
	}

// -----------------------------------------------------------------------------
// CServerTransport::Sender
// -----------------------------------------------------------------------------
//	
CSender* CServerTransport::Sender() 
    { 
    return iSender; 
    }

// -----------------------------------------------------------------------------
// CServerTransport::SetPersistency
// -----------------------------------------------------------------------------
//   
void CServerTransport::SetPersistency( TBool aIsPersistent )
    {
    iIsPersistent = aIsPersistent;
    }

// -----------------------------------------------------------------------------
// CServerTransport::IsPersistent
// -----------------------------------------------------------------------------
//     
TBool CServerTransport::IsPersistent() const
    {
    return iIsPersistent;
    }

// -----------------------------------------------------------------------------
// CServerTransport::InformSendingStatus
// -----------------------------------------------------------------------------
//     
void CServerTransport::InformSendingStatus()
    {
    iNATTraversal.SocketIdle( !iIsSending, iSocket );
    }

// -----------------------------------------------------------------------------
// CServerTransport::Removal
// -----------------------------------------------------------------------------
//    
TBool CServerTransport::Removal( MSIPNATBindingObserver& aBindingObserver )
    {
    return ( iTransportParams.NATBindingObserver() == &aBindingObserver &&
             IsPersistent() &&
             !iMyOwner.IsReservedTransportOwner() );
    }

// -----------------------------------------------------------------------------
// CServerTransport::Shutdown
// -----------------------------------------------------------------------------
//	     
TBool CServerTransport::Shutdown( 
    MSIPTransportRemovalObserver* aRemovalObserver )
    {
    if ( iSender && iReceiver && aRemovalObserver )
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
// CServerTransport::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CServerTransport::TimerExpiredL( TTimerId /*aTimerId*/,
									  TAny* /*aTimerParam*/ )
	{
	// Reserved transport uses persistent tcp connection and therefore
	// only send operations are cleared when timer expires
	if ( iMyOwner.IsReservedTransportOwner() || IsPersistent() )
	    {
	    CancelAllRequests( KErrSIPTransportFailure );
	    }
	else
	    {    
	    RemoveL();
	    }
	}

// -----------------------------------------------------------------------------
// CServerTransport::SocketContainer
// -----------------------------------------------------------------------------
//
CSocketContainer& CServerTransport::SocketContainer() 
    {
    return *iSocketContainer;
    }
// -----------------------------------------------------------------------------
// CServerTransport::SocketContainer
// -----------------------------------------------------------------------------
//
CSocketContainer* CServerTransport::SocketContainer(
    TUint /*aIPAddrFamily*/) 
    {
    return iSocketContainer;
    }
    
// -----------------------------------------------------------------------------
// CServerTransport::ReceivedDataL
// -----------------------------------------------------------------------------
//
void CServerTransport::ReceivedDataL( CSIPMessage* aMessage,
									  const TInetAddr& aRemoteAddr,
									  TInt aError,
									  TBool aCompressed )
	{
	// As starting timer can leave, do it before passing ownership of aMessage
	StartTimerL();
	iMyOwner.ReceiveL( aMessage, aRemoteAddr, aError, aCompressed );
	}

// -----------------------------------------------------------------------------
// CServerTransport::CancelSend
// -----------------------------------------------------------------------------
//
TBool CServerTransport::CancelSend( TRequestStatus& aStatus )
	{
	if ( iSender )
		{
		return iSender->FindAndCancel( aStatus );
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CServerTransport::Remove
// Timer will remove this CServerTransport asynchronously
// -----------------------------------------------------------------------------
//
TInt CServerTransport::Remove()
	{
	TRAP_IGNORE( StartTimerL() )
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CServerTransport::StartTimerL
// As starting new timer may fail, do it before stopping old timer
// -----------------------------------------------------------------------------
//
void CServerTransport::StartTimerL()
	{
	TTimerId timerId = iTimer.StartL( this, iTimerValue );
	StopTimer();
	iTimerId = timerId;
	}

// -----------------------------------------------------------------------------
// CServerTransport::StopTimer
// -----------------------------------------------------------------------------
//
void CServerTransport::StopTimer()
    {
    iTimer.Stop( iTimerId );
    }

// -----------------------------------------------------------------------------
// CServerTransport::ReRouteL
// -----------------------------------------------------------------------------
//
void CServerTransport::ReRouteL( TUint /*aProtocol*/, 
                                 COutgoingData* /*aData*/ )
	{
	}

// -----------------------------------------------------------------------------
// CServerTransport::DisconnectedL
// -----------------------------------------------------------------------------
//
TBool CServerTransport::DisconnectedL()
	{
	__ASSERT_ALWAYS( iReceiver && iSender, User::Leave( KErrNotFound ) );
	
	iReceiver->Cancel();
	iSender->Cancel();
	COutgoingData* data( NULL );
	while ( ( data = iSender->Data() ) != 0 )
		{
		if ( !data->Message().IsRequest() )
			{
			CleanupStack::PushL( data );
			iMyOwner.ReRouteL( data );
			CleanupStack::Pop( data );
			}
		else
			{
			TRequestStatus* stat = data->Status();
			User::RequestComplete( stat, KErrCouldNotConnect );
			delete data;
			}
		}
		
	RemoveL();
	
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CServerTransport::SigCompHandler
// -----------------------------------------------------------------------------
//
MSigCompController* CServerTransport::SigCompHandler()
	{
	return iMyOwner.SigCompHandler();
	}

// -----------------------------------------------------------------------------
// CServerTransport::IapId
// -----------------------------------------------------------------------------
//
TUint32 CServerTransport::IapId()
	{
	return iMyOwner.IapId();
	}
	
// -----------------------------------------------------------------------------
// CServerTransport::Sending
// -----------------------------------------------------------------------------
//
void CServerTransport::Sending( TBool aIsSending )
    {    
    iIsSending = aIsSending;
    iNATTraversal.SocketIdle( !iIsSending, iSocket );
    }
    
// -----------------------------------------------------------------------------
// CServerTransport::SetShutdownObserver
// -----------------------------------------------------------------------------
//
void CServerTransport::SetShutdownObserver( 
    MSIPTransportRemovalObserver* aRemovalObserver )
    {
    iShutdownObserver = aRemovalObserver;
    }
    
// -----------------------------------------------------------------------------
// CServerTransport::HandleShutdownCompletion
// -----------------------------------------------------------------------------
//     
void CServerTransport::HandleShutdownCompletion()
    {
    if ( iShutdownObserver )
        {
        iShutdownObserver->RemovalCompleted( iTransportParams.TransportId() );
        iShutdownObserver = 0;
        }
    }

// -----------------------------------------------------------------------------
// CServerTransport::RemoveL
// -----------------------------------------------------------------------------
//    
void CServerTransport::RemoveL()
    {
    MSIPTransportRemovalObserver* shutdownObserver = iShutdownObserver;
    iShutdownObserver = 0;
	iMyOwner.RemoveL( this, iRemoteAddr, shutdownObserver );
    }
    
// -----------------------------------------------------------------------------
// CServerTransport::~CServerTransport
// -----------------------------------------------------------------------------
//
CServerTransport::~CServerTransport()
	{
	HandleShutdownCompletion();
	StopTimer();
	delete iReceiver;
    delete iSender;
    delete iSocketContainer;
	iSocket.Close();
	}

// -----------------------------------------------------------------------------
// CServerTransport::CServerTransport
// -----------------------------------------------------------------------------
//
CServerTransport::CServerTransport( 
	MServerTransportOwner& aMyOwner,
    MTimerManager& aTimer,
    CSIPNATTraversalController& aNATTraversal,
    COwnerSettingsList& aSettingsList ) :
    iMyOwner( aMyOwner ),
    iTimer( aTimer ),
    iNATTraversal( aNATTraversal ),
    iSettingsList( aSettingsList ),
    iTransportParams( KDefaultOwnerId, iMyOwner.IapId() ),
    iIsPersistent( EFalse ),
    iIsSending( EFalse ),
    iShutdownObserver( 0 )
	{
	}

// -----------------------------------------------------------------------------
// CServerTransport::ConstructL
// -----------------------------------------------------------------------------
//
void CServerTransport::ConstructL( RSocketServ& aServer, TUint aTOne )
	{
	iTimerValue = ( 64 * aTOne );
	iAccepted = EFalse;
	User::LeaveIfError( iSocket.Open( aServer ) );
	iSocketContainer = CSocketContainer::NewL( iSocket );
	}
