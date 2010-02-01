// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSipConnectionMgr.cpp
// Part of       : ConnectionMgr
// Version       : SIP/4.0 
//



#include "CSipConnectionMgr.h"
#include "CSipConnection.h"
#include "MTransactionFinder.h"
#include "Lwtimer.h"
#include "SipLogs.h"
#include "CleanupResetAndDestroy.h"
#include "sipsigcomphandler.h"
#include "sipsigcompnotintegrated.h"
#include "CNetworkManager.h"
#include "TSIPTransportParams.h"
#include "sipsigcomphandlerinitparams.h"
#include "NetworkInfo.h"
#include "CNATTraversalNotIntegrated.h"
#include "SipAssert.h"
#include "cellulardatausagemonitor.h"
#include <sipnattraversalcontroller.h>
#include <sipnattraversalcontrollerinitparams.h>
#include <sipsystemstatemonitor.h>

const TUint KSIPMessageSlots = 32;

const TInt CSipConnection::iConnectionOffset =
	_FOFF(CSipConnection, iConnectionLink);


// -----------------------------------------------------------------------------
// CSipConnectionMgr::NewL
// -----------------------------------------------------------------------------
//
CSipConnectionMgr* CSipConnectionMgr::NewL( MTransactionFinder& aFinder, 
		MServerTaFactory& aTaFactory, MTimerManager& aTimer, TUint aTOne, 
		MConnectionStateObserver& aNetworkObserver )
	{
	CSipConnectionMgr* self = NewLC( aFinder, aTaFactory, aTimer, 
								   	 aTOne, aNetworkObserver );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CSipConnectionMgr::NewLC
// -----------------------------------------------------------------------------
//
CSipConnectionMgr* CSipConnectionMgr::NewLC( MTransactionFinder& aFinder, 
		MServerTaFactory& aTaFactory, MTimerManager& aTimer, TUint aTOne, 
		MConnectionStateObserver& aNetworkObserver )
	{
	CSipConnectionMgr* self =
		new (ELeave) CSipConnectionMgr( aFinder, aTaFactory,
									    aTimer, aTOne, aNetworkObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSipConnectionMgr::CSipConnectionMgr
// -----------------------------------------------------------------------------
//
CSipConnectionMgr::CSipConnectionMgr( MTransactionFinder& aFinder, 
		MServerTaFactory& aTaFactory, MTimerManager& aTimer, TUint aTOne, 
		MConnectionStateObserver& aNetworkObserver ) : 
		iFinder( &aFinder ), iTaFactory( &aTaFactory ),
		iTimer( &aTimer ), iTOne( aTOne ), 
		iConnectionList( CSipConnection::iConnectionOffset ), 
		iConnectionListIter( iConnectionList )
	{
	iTUNetStatusObserver = &aNetworkObserver;
	}

// -----------------------------------------------------------------------------
// CSipConnectionMgr::ConstructL
// -----------------------------------------------------------------------------
//
void CSipConnectionMgr::ConstructL()
	{
	User::LeaveIfError( iSocketServ.Connect( KSIPMessageSlots ) );	
	iNetworkManager = CNetworkManager::NewL();	
	iNetworkInfo = CNetworkInfo::NewL(*iNetworkManager);	
	LoadSigCompL();
	LoadNATTraversalL();
	LoadSystemStateMonitorL();
	iCellularDataUsageMonitor = CSipCellularDataUsageMonitor::NewL(*this);
	}

// -----------------------------------------------------------------------------
// CSipConnectionMgr::~CSipConnectionMgr
// -----------------------------------------------------------------------------
//
CSipConnectionMgr::~CSipConnectionMgr()
	{
	RemoveAllConnections();
	iSocketServ.Close();
	delete iSigCompController;
	delete iNATTraversal;
	delete iSystemStateMonitor;
	delete	iCellularDataUsageMonitor;
	//Must delete iNetworkInfo before iNetworkManager
	delete iNetworkInfo;
	delete iNetworkManager;	
	}

// -----------------------------------------------------------------------------
// CSipConnectionMgr::OpenL
// -----------------------------------------------------------------------------
//
CSIPConnection::TState CSipConnectionMgr::OpenL(
    const TSIPTransportParams& aParams)
    {
    TUint32 iapId = aParams.IapId();
    
   	__ASSERT_ALWAYS( iapId > 0, User::Leave(KErrBadName) );
	
	CSipConnection* connection = 0;
	if ( ConnectionExists( iapId ) )
		{
		connection = Connection( iapId );
		}
	else
		{
		connection = CreateNewConnectionL( aParams );
		}
	connection->IncreaseUserCount();
	return connection->State();
    }

// -----------------------------------------------------------------------------
// CSipConnectionMgr::IsOpen
// -----------------------------------------------------------------------------
//
TBool CSipConnectionMgr::IsOpen( const TSIPTransportParams& aParams )
    {
    return ConnectionExists( aParams.IapId() );
    }

// -----------------------------------------------------------------------------
// CSipConnectionMgr::Close
// -----------------------------------------------------------------------------
//
void CSipConnectionMgr::Close( const TSIPTransportParams& aParams )
    {
    TUint32 iapId = aParams.IapId();
    
    if ( ConnectionExists( iapId ) )
		{
		if ( Connection( iapId )->DecreaseUserCount( aParams ) == 0 )
			{
			RemoveConnection( iapId );
			}
		}
    }

// -----------------------------------------------------------------------------
// CSipConnectionMgr::SetOptL
// -----------------------------------------------------------------------------
//	
void CSipConnectionMgr::SetOptL(
    const TSIPTransportParams& aParams,
    TUint aOptionName,
	TUint aOptionLevel,
	const TDesC8& aOption )
    {
    __ASSERT_ALWAYS( aParams.OwnerId() > 0, User::Leave( KErrArgument ) );
    TUint32 iapId = aParams.IapId();
    if ( ConnectionExists( iapId ) )
		{
		Connection( iapId )->SetOptL( 
		    aParams, aOptionName, aOptionLevel, aOption );
		}
	else
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CSipConnectionMgr::SetOptL
// -----------------------------------------------------------------------------
//	         	                         
void CSipConnectionMgr::SetOptL(
    const TSIPTransportParams& aParams,
    TUint aOptionName,
	TUint aOptionLevel,
	TInt aOption )
    {
    __ASSERT_ALWAYS( aParams.OwnerId() > 0, User::Leave( KErrArgument ) );
    TUint32 iapId = aParams.IapId();
    if ( ConnectionExists( iapId ) )
		{
		Connection( iapId )->SetOptL( 
		    aParams, aOptionName, aOptionLevel, aOption );
		}
	else
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CSipConnectionMgr::SendL
// -----------------------------------------------------------------------------
//
void CSipConnectionMgr::SendL(
    const TSIPTransportParams& aParams,
	CSIPRequest& aRequest,
	TBool aForceUDP,
	const CUri8* aOutboundProxy,
	const TInetAddr& aRemoteAddr, 
	TRequestStatus& aStatus )
    {
    TUint32 iapId = aParams.IapId();
    if ( ConnectionExists( iapId ) )
		{
		Connection( iapId )->SendL( aParams, 
		                            aRequest,
		                            aForceUDP, 
		                            aOutboundProxy, 
		                            aRemoteAddr, 
		                            aStatus );
		}
	else
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CSipConnectionMgr::SendL
// -----------------------------------------------------------------------------
//
void CSipConnectionMgr::SendL(
    const TSIPTransportParams& aParams,
    CSIPResponse& aResponse,
    TTransactionId aId,
    TRequestStatus& aStatus )
    {
    TUint32 iapId = aParams.IapId();
    if ( ConnectionExists( iapId ) )
		{
		Connection( iapId )->SendL( aParams, aResponse, aId, aStatus );
		}
    else
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CSipConnectionMgr::CancelSend
// -----------------------------------------------------------------------------
//
TInt CSipConnectionMgr::CancelSend( TRequestStatus &aStatus )
    {
    CSipConnection* connection = 0;
    TInt ret( KErrNotFound );
	iConnectionListIter.SetToFirst();	
	while ( ( connection = iConnectionListIter++ ) != 0 && 
	        ( ret == KErrNotFound ) )
	    {
	    ret = connection->CancelSend( aStatus );
	    }
	return ret;
    }


// -----------------------------------------------------------------------------
// CSipConnectionMgr::CancelResponseSend
// -----------------------------------------------------------------------------
//
TInt CSipConnectionMgr::CancelResponseSend( 
    TTransactionId aId, 
    TBool aCancelAlso2xxResponses )
    {
    CSipConnection* connection = 0;
    TInt ret( KErrNotFound );
	iConnectionListIter.SetToFirst();
	while ( ( connection = iConnectionListIter++ ) != 0 &&
	        ( ret == KErrNotFound ) )
	    {
	    ret = connection->CancelResponseSend( aId, aCancelAlso2xxResponses );
	    }
	return ret;
    }

// -----------------------------------------------------------------------------
// CSipConnectionMgr::TcpDisconnect
// -----------------------------------------------------------------------------
//
void CSipConnectionMgr::TcpDisconnect(
    const TSIPTransportParams& aParams,
	const TInetAddr& aRemoteAddress )
    {
    TUint32 iapId = aParams.IapId();
    if ( ConnectionExists( iapId ) )
		{
		Connection( iapId )->TcpDisconnect( aParams, aRemoteAddress );
		}
    }

// -----------------------------------------------------------------------------
// CSipConnectionMgr::FreeResources
// -----------------------------------------------------------------------------
//
void CSipConnectionMgr::FreeResources( 
    MSIPNATBindingObserver& aSIPNATBindingObserver )
    {
    CSipConnection* connection = 0;
	iConnectionListIter.SetToFirst();
	while ( (connection = iConnectionListIter++) != 0 )
		{
		connection->FreeResources( aSIPNATBindingObserver );
		}
    }
    
// -----------------------------------------------------------------------------
// CSipConnectionMgr::GetByURIL
// -----------------------------------------------------------------------------
//
void CSipConnectionMgr::GetByURIL( 
    const CURIContainer& aURI,
    RPointerArray<MSIPResolvingResult>& aResult,
	const TSIPTransportParams& aParams,
	MSIPServerResolverObserver* aObserver )
	{
	TUint32 iapId = aParams.IapId();
	if ( ConnectionExists( iapId ) )
		{
		Connection( iapId )->GetByURIL( aParams, aURI, aResult, aObserver );
		}
	else
		{
	    User::Leave( KErrNotReady );
		}
	}

// -----------------------------------------------------------------------------
// CSipConnectionMgr::CancelGetByUri
// -----------------------------------------------------------------------------
//
void CSipConnectionMgr::CancelGetByUri( 
    MSIPServerResolverObserver* aObserver )
	{
	CSipConnection* connection = 0;
	iConnectionListIter.SetToFirst();
	while ( (connection = iConnectionListIter++) != 0 )
		{
		connection->CancelGetByUri( aObserver );
		}
	}

// -----------------------------------------------------------------------------
// CSipConnectionMgr::IsSupported
// -----------------------------------------------------------------------------
//
TBool CSipConnectionMgr::IsSupported() const
	{
	return iSigCompController->IsSupported();
	}

// -----------------------------------------------------------------------------
// CSipConnectionMgr::CreateCompartmentL
// -----------------------------------------------------------------------------
//
TUint32 CSipConnectionMgr::CreateCompartmentL( 
    TUint32 aIapId )
    {
    __ASSERT_ALWAYS( iSigCompController->IsSupported(), 
                     User::Leave( KErrNotSupported ) );
    
	return iSigCompController->CreateCompartmentL( aIapId );
    }

// -----------------------------------------------------------------------------
// CSipConnectionMgr::RemoveCompartment
// -----------------------------------------------------------------------------
//
void CSipConnectionMgr::RemoveCompartment( TUint32 aCompartmentId )
    {
   	if ( iSigCompController->IsSupported() )
		{
		iSigCompController->RemoveCompartment( aCompartmentId );
		}
    }

// -----------------------------------------------------------------------------
// CSipConnectionMgr::CompartmentCount
// -----------------------------------------------------------------------------
//
TInt CSipConnectionMgr::CompartmentCount()
	{
	return iSigCompController->CompartmentCount();
	}

// -----------------------------------------------------------------------------
// CSipConnectionMgr::SendFailedL
// -----------------------------------------------------------------------------
//
void CSipConnectionMgr::SendFailedL( TUint32 aCompartmentId ) 
	{
	iSigCompController->SendFailedL( aCompartmentId );
	}

// -----------------------------------------------------------------------------
// CSipConnectionMgr::Match
// -----------------------------------------------------------------------------
//
TBool CSipConnectionMgr::Match( const TDesC8& aAddress )
	{
	CSipConnection* connection = 0;
	iConnectionListIter.SetToFirst();
	while ( ( connection = iConnectionListIter++ ) != 0 )
		{
		if ( connection->Match( aAddress ) )
			{
			return ETrue;
			}
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSipConnectionMgr::IsSystemStarted
// -----------------------------------------------------------------------------
//
TBool CSipConnectionMgr::IsSystemStarted() const
    {            
    return ( iSystemState == CSipSystemStateMonitor::ESystemReady );
    }


// -----------------------------------------------------------------------------
// CSipConnectionMgr::Destroy
// -----------------------------------------------------------------------------
//	
void CSipConnectionMgr::Destroy( TUint32 aIapId )
	{
	RemoveConnection( aIapId );
	}

// -----------------------------------------------------------------------------
// CSipConnectionMgr::IsCellularDataUsageAllowed
// -----------------------------------------------------------------------------
//	

TBool CSipConnectionMgr:: IsCellularDataUsageAllowed() const
{

TBool val =  (MSipCellularDataUsageObserver::EDataUsageAllowed == iCellularDataUsageMonitor->CellularUsageStatus());
__SIP_INT_LOG1( "CSipConnectionMgr::IsCellularDataUsageAllowed, return value", val )  
	return val;
}


// -----------------------------------------------------------------------------
// CSipConnectionMgr::ReserveTransportL
// -----------------------------------------------------------------------------
//		
void CSipConnectionMgr::ReserveTransportL( 
    TSIPTransportParams& aTransportParams,
    TUint aLocalSendPort,
	TUint aLocalReceivePort,
	const TInetAddr& aRemoteAddr,
	TUint aRemoteSendPort )
    {
    TUint32 iapId = aTransportParams.IapId();
   	if ( ConnectionExists( iapId ) )
		{
		// Old transport params are possibly used (if tp id is not default)
		// for mapping two related transports together. This is for the IPSec
		// need where two records are sharing the same listening port
		TSIPTransportParams oldTransportParams( aTransportParams );
		
		aTransportParams.SetTransportId( CreateTransportId() );
		
		CSipConnection* con = Connection( iapId );
		TRAPD( err, con->ReserveTransportL( oldTransportParams,
		                                    aTransportParams,
                                            aLocalSendPort,
                                            aLocalReceivePort,
                                            aRemoteAddr,
                                            aRemoteSendPort ) );
		if ( err )
		    {
		    TInt ignored( 0 );
		    con->RemoveReservedTransport( aTransportParams.TransportId(), 
		                                  0, 
		                                  ignored );
		    User::Leave( err );
		    }
		}
	else
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CSipConnectionMgr::CreateTLSTransportL
// -----------------------------------------------------------------------------
//
void CSipConnectionMgr::CreateTLSTransportL( 
    TSIPTransportParams& aTransportParams )
    {
    TUint32 iapId = aTransportParams.IapId();
   	if ( ConnectionExists( iapId ) )
		{
		aTransportParams.SetTransportId( CreateTransportId() );
		
		CSipConnection* con = Connection( iapId );
		TRAPD( err, con->ReserveTLSTransportL( aTransportParams ) );
		if ( err )
		    {
		    TInt ignored( 0 );
		    con->RemoveReservedTransport( aTransportParams.TransportId(), 
		                                  0, 
		                                  ignored );
		    User::Leave( err );
		    }
		}
	else
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CSipConnectionMgr::RemoveTransport
// -----------------------------------------------------------------------------
//   
void CSipConnectionMgr::RemoveTransport( TUint32 aTransportId )
    {
    RemoveTransport( aTransportId, 0 );
    }
    
// -----------------------------------------------------------------------------
// CSipConnectionMgr::RemoveTransport
// -----------------------------------------------------------------------------
//				                
TInt CSipConnectionMgr::RemoveTransport( 
    TUint32 aTransportId,
    MSIPTransportRemovalObserver* aRemovalObserver )
    {
    TInt numAsyncRemovals( 0 );
    
    if ( aTransportId == 0 )
        {
        return numAsyncRemovals;
        }
    CSipConnection* connection = 0;
    iConnectionListIter.SetToFirst();
    TBool removed( EFalse );
	while ( ( connection = iConnectionListIter++ ) != 0 && !removed )
		{
		removed = connection->RemoveReservedTransport( aTransportId, 
		                                               aRemovalObserver, 
		                                               numAsyncRemovals );
		}
    return numAsyncRemovals; 
    }
    
// -----------------------------------------------------------------------------
// CSipConnectionMgr::GetLocalAddress
// -----------------------------------------------------------------------------
//
TInt CSipConnectionMgr::GetLocalAddress( TUint32 aIapId, TInetAddr& aAddr )
	{
	if ( ConnectionExists( aIapId ) )
		{
		return Connection( aIapId )->GetLocalAddress( aAddr );
		}
	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSipConnectionMgr::SystemVariableUpdated
// -----------------------------------------------------------------------------
//
void CSipConnectionMgr::SystemVariableUpdated( 
    CSipSystemStateMonitor::TSystemVariable aVariable,
    TInt /*aObjectId*/,
    TInt aValue )
    {
    __SIP_INT_LOG1( "CSipConnectionMgr::SystemVariableUpdated variable", 
                    aVariable )
    
    __SIP_INT_LOG1( "CSipConnectionMgr::SystemVariableUpdated value", 
                    aValue )    
    
	if ( aVariable == CSipSystemStateMonitor::ESystemState &&
	     aValue == CSipSystemStateMonitor::ESystemReady &&
	     iSystemState != CSipSystemStateMonitor::ESystemReady )
	    {
	    iSystemState = CSipSystemStateMonitor::ESystemReady; 
        
        CSipConnection* connection = 0;
        iConnectionListIter.SetToFirst();
        while ( ( connection = iConnectionListIter++ ) != 0 )
            {
            connection->SystemStarted();
            }
	    }
    }

// -----------------------------------------------------------------------------
// CSipConnectionMgr::CellularDataUsageChangedL
// -----------------------------------------------------------------------------
//

	void CSipConnectionMgr::CellularDataUsageChangedL(MSipCellularDataUsageObserver::TCellularDataUsageStatus aStatus)
		{
		//First the Status must be retrived from the Monitor.
		//Then, if the Status is to be allowed only then the below code must execute.
		// Status check must typically happen by using CSipCellularDataUsageMonitor::CellularUsageStatus()
		// and the value must be EDataUsageAllowed.
		if(MSipCellularDataUsageObserver::EDataUsageAllowed == aStatus)
			{
			__SIP_LOG( "CSipConnectionMgr::CellularDataUsageChangedL(), Notified with CellularUsage Allowed" )

				CSipConnection* connection = 0;
        		iConnectionListIter.SetToFirst();
		        while ( ( connection = iConnectionListIter++ ) != 0 )
        		    {
		            connection->CellularUsageAllowed();
        		    }
			}
		}
// -----------------------------------------------------------------------------
// CSipConnectionMgr::CreateNewConnectionL
// -----------------------------------------------------------------------------
//
CSipConnection* CSipConnectionMgr::CreateNewConnectionL( 
    const TSIPTransportParams& aParams )
	{
	CSipConnection* connection = 
		CSipConnection::NewLC( *iFinder, *iTaFactory, *iTimer, iTOne, 
				               aParams, iSocketServ, *iSigCompController, 
				               *iTUNetStatusObserver, *iNetworkManager, *this,
							   *iNetworkInfo, *iNATTraversal );
	CleanupStack::Pop( connection );
	iConnectionList.AddLast( *connection );
	return connection;
	}

// -----------------------------------------------------------------------------
// CSipConnectionMgr::RemoveConnection
// -----------------------------------------------------------------------------
//
void CSipConnectionMgr::RemoveConnection( TUint32 aIapId )
	{
	CSipConnection* connection = Connection( aIapId );
	if ( connection )
		{
		iConnectionList.Remove( *connection );
		delete connection;
		}
	}

// -----------------------------------------------------------------------------
// CSipConnectionMgr::ConnectionExists
// -----------------------------------------------------------------------------
//
TBool CSipConnectionMgr::ConnectionExists( TUint32 aIapId )
	{
	return Connection( aIapId ) != 0;
	}

// -----------------------------------------------------------------------------
// CSipConnectionMgr::Connection
// -----------------------------------------------------------------------------
//
CSipConnection* CSipConnectionMgr::Connection( TUint32 aIapId )
	{
	CSipConnection* connection = 0;
	iConnectionListIter.SetToFirst();
	while ( ( connection = iConnectionListIter++ ) != 0 )
		{
		if ( connection->Handle( aIapId ) )
			{
			return connection;
			}
		}
	return NULL;
	}

// -----------------------------------------------------------------------------
// CSipConnectionMgr::RemoveAllConnections
// -----------------------------------------------------------------------------
//
void CSipConnectionMgr::RemoveAllConnections()
	{
	CSipConnection* connection = 0;
	iConnectionListIter.SetToFirst();
	while ( ( connection = iConnectionListIter++ ) != 0 )
		{
		iConnectionList.Remove( *connection );
		delete connection;
		}
	}
	
// -----------------------------------------------------------------------------
// CSipConnectionMgr::CreateTransportId
// -----------------------------------------------------------------------------
//
TUint32 CSipConnectionMgr::CreateTransportId()
	{
	if ( iTransportIdCounter < KMaxTUint32 )
		{
		iTransportIdCounter++;
		}
	else
		{
		iTransportIdCounter = 1;
		}	
	return iTransportIdCounter;
	}

// -----------------------------------------------------------------------------
// CSipConnectionMgr::LoadSigCompL
// -----------------------------------------------------------------------------
//
void CSipConnectionMgr::LoadSigCompL()
	{
	TEComResolverParams resolverParams;
	RImplInfoPtrArray infoArray;
	REComSession::ListImplementationsL( KSIPSigCompInterfaceUid,
	                                    infoArray );
	CleanupResetAndDestroyPushL( infoArray );
	
	if ( infoArray.Count() > 0 && infoArray[ 0 ] )
	    {
	    CImplementationInformation& info = *( infoArray[ 0 ] );
	    TSIPSigCompHandlerInitParams initparam( *this );
	    resolverParams.SetDataType( info.DataType() );
	    	    
	    iSigCompController = 
	        reinterpret_cast< CSigCompHandler* >( 
	            REComSession::CreateImplementationL(
	                KSIPSigCompInterfaceUid, 
                    _FOFF(CSigCompHandler,iInstanceKey),
                    &initparam,
                    resolverParams ) );
                    
        __SIP_DES8_LOG( "SigComp plugin loaded", info.DataType() )
	    }
	else
	    {
	    iSigCompController = CSigCompNotIntegrated::NewL();
	    __SIP_LOG( "SigComp plugin not loaded" )
	    }
	    
	CleanupStack::PopAndDestroy( 1 ); // infoArray	
	}	
	
// -----------------------------------------------------------------------------
// CSipConnectionMgr::LoadNATTraversalL
// -----------------------------------------------------------------------------
//
void CSipConnectionMgr::LoadNATTraversalL()
	{	
	TEComResolverParams resolverParams;
	RImplInfoPtrArray infoArray;
	REComSession::ListImplementationsL( KSIPNATTraversalControllerIFUid,
	                                    infoArray );
	CleanupResetAndDestroyPushL( infoArray );
	
	if ( infoArray.Count() > 0 && infoArray[ 0 ] )
	    {
	    CImplementationInformation& info = *( infoArray[ 0 ] );
	    resolverParams.SetDataType( info.DataType() );
	    TSIPNATTraversalControllerInitParams initParams( iSocketServ );
	    	    
	    iNATTraversal = 
	        reinterpret_cast< CSIPNATTraversalController* >( 
	            REComSession::CreateImplementationL(
	                KSIPNATTraversalControllerIFUid, 
                    _FOFF( CSIPNATTraversalController, iInstanceKey ),
                    &initParams,
                    resolverParams ) );    
                    
        __SIP_DES8_LOG( "NATTraversal plugin loaded", info.DataType() )
	    }
	else
	    {
	    iNATTraversal = CNATTraversalNotIntegrated::NewL();
	    __SIP_LOG( "NATTraversal plugin not loaded" )
	    }
	    
	CleanupStack::PopAndDestroy( 1 ); // infoArray
	}

// -----------------------------------------------------------------------------
// CSipConnectionMgr::LoadSystemStateMonitorL
// -----------------------------------------------------------------------------
//
void CSipConnectionMgr::LoadSystemStateMonitorL()
    {
	__ASSERT_ALWAYS( !iSystemStateMonitor, User::Leave( KErrAlreadyExists ) );

	RImplInfoPtrArray infoArray;
	REComSession::ListImplementationsL( KSipSystemStateMonitorIfUid, 
	                                    infoArray );

	if ( infoArray.Count() > 0 && infoArray[0] )
	    {
		// Create a specific plugin with its UID
		iSystemStateMonitor = reinterpret_cast<CSipSystemStateMonitor*>(
			REComSession::CreateImplementationL(
				infoArray[0]->ImplementationUid(),
				_FOFF(CSipSystemStateMonitor, iInstanceKey ) ) );
        __SIP_LOG( "System State Monitor loaded" )
  		iSystemStateMonitor->StartMonitoringL( 
  		    CSipSystemStateMonitor::ESystemState, 0, *this );
  		iSystemState = iSystemStateMonitor->State();
  		
  		__SIP_INT_LOG1( "Current system state", iSystemState )
	    }
	else
	    {
	    __SIP_LOG( "System State Monitor not loaded" )
	    
	    iSystemState = CSipSystemStateMonitor::ESystemReady;
	    }
	infoArray.ResetAndDestroy();    
    }

// -----------------------------------------------------------------------------
// CSipConnectionMgr::RefreshConnection
// -----------------------------------------------------------------------------
//
void CSipConnectionMgr::RefreshConnectionL(TUint32 aIapId)
	{ 
		iNetworkManager->RefreshConnectionL(aIapId);
	}

// -----------------------------------------------------------------------------
// CSipConnectionMgr::ConnectionErrorL
// -----------------------------------------------------------------------------
//
TInt CSipConnectionMgr::ConnectionErrorL(TUint32 aIapId)
	{
	CSipConnection* conn = Connection(aIapId);
	__SIP_ASSERT_LEAVE(conn, KErrNotFound);
	return conn->ConnectionError();
	}
// End of file
