// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CConnectionContainer.cpp
// Part of       : ConnectionMgr
// Implementation
// Version       : SIP/4.0
//



#include "CConnectionContainer.h"
#include "CConnectionMonitor.h"
#include "sipnetworkobserver.h"
#include "SipLogs.h"

// -----------------------------------------------------------------------------
// CConnectionContainer::NewL
// -----------------------------------------------------------------------------
//
CConnectionContainer* CConnectionContainer::NewL( 
    MSIPNetworkObserver& aObserver )
	{
	CConnectionContainer* self = 
		new( ELeave ) CConnectionContainer( aObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CConnectionContainer::CConnectionContainer
// -----------------------------------------------------------------------------
//
CConnectionContainer::CConnectionContainer( MSIPNetworkObserver& aObserver ) :
	CActive( CActive::EPriorityStandard ),
	iObserver( aObserver )
	{
	CActiveScheduler::Add( this );
	}
	
// -----------------------------------------------------------------------------
// CConnectionContainer::ConstructL
// -----------------------------------------------------------------------------
//	
void CConnectionContainer::ConstructL()
	{	
	iMonitor = CConnectionMonitor::NewL(iObserver,iConnection);
	}

// -----------------------------------------------------------------------------
// CConnectionContainer::~CConnectionContainer
// -----------------------------------------------------------------------------
//
CConnectionContainer::~CConnectionContainer()
	{
	delete iMonitor;
	iConnection.Close();	
	Cancel();
	}

// -----------------------------------------------------------------------------
// CConnectionContainer::RunL
// -----------------------------------------------------------------------------
//
void CConnectionContainer::RunL()
	{
	__SIP_INT_LOG1( "CConnectionContainer::RunL", iStatus.Int() )
	
	iMonitor->StartMonitoring();

	iObserver.NetworkStateChanged( MSIPNetworkObserver::ENetworkConnected,
	                               iStatus.Int() );
	}

// -----------------------------------------------------------------------------
// CConnectionContainer::DoCancel
// -----------------------------------------------------------------------------
//
void CConnectionContainer::DoCancel()
	{
	iConnection.Close();
	}

// -----------------------------------------------------------------------------
// CConnectionContainer::Connection
// -----------------------------------------------------------------------------
//
RConnection& CConnectionContainer::Connection()
	{
	return iConnection;
	}

// -----------------------------------------------------------------------------
// CConnectionContainer::OpenConnection
// -----------------------------------------------------------------------------
//
TInt CConnectionContainer::OpenConnection( TUint32 aIapId,
										   RSocketServ& aServer )
	{
	__SIP_INT_LOG1( "CConnectionContainer::OpenConnection entered", aIapId )
	
	TInt err = KErrNone;
	if( !IsActive() )
		{
		err = OpenConnection( aServer );

		if (!err)
			{
		// Set connection preferences
		iExtPrefs.SetIapId( aIapId );
		TRAPD(error, iPrefList.AppendL(&iExtPrefs));
		if(error)
			{
		return error;
			}
    		// Start connecting
    		iConnection.Start( iPrefList, iStatus );
    		SetActive();
			}
		}
		
	__SIP_INT_LOG1( "CConnectionContainer::OpenConnection returns", err )	
		
	return err;
	}

// -----------------------------------------------------------------------------
// CConnectionContainer::CloseConnection
// -----------------------------------------------------------------------------
//
void CConnectionContainer::CloseConnection()
	{
	__SIP_LOG( "CConnectionContainer::CloseConnection returns" )
	
	iMonitor->StopMonitoring();
	iConnection.Close();	
	Cancel();
	
	__SIP_LOG( "CConnectionContainer::CloseConnection returns" )
	}
	
// -----------------------------------------------------------------------------
// CConnectionContainer::MonitorConnectionClosure
// -----------------------------------------------------------------------------
//	
TInt CConnectionContainer::MonitorConnectionClosure( RSocketServ& aServer )
    {
    TInt err = OpenConnection( aServer );
        
    if ( !err )
        {
        iMonitor->MonitorConnectionClosure();
        }
    
    return err;
    }

// -----------------------------------------------------------------------------
// CConnectionContainer::OpenConnection
// -----------------------------------------------------------------------------
//
TInt CConnectionContainer::OpenConnection( RSocketServ& aServer )
	{
    TInt err( KErrNone );
    // If iConnection is already open don't reopen it.
	if( !iConnection.SubSessionHandle() )
        {
        err = iConnection.Open( aServer );
        }
    return err;
	}
	
// End of File
