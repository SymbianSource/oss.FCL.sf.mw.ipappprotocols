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
// Name        : CPacketServiceMonitor.cpp
// Part of     : NetworkMonitor
// Implementation
// Version     : SIP/4.0
//



#include "CMsClassWatcher.h"
#include "CPacketServiceMonitor.h"
#include "sipbearermonitorobserver.h"
#include <pcktcs.h>

// -----------------------------------------------------------------------------
// CPacketServiceMonitor::NewL
// -----------------------------------------------------------------------------
//
CPacketServiceMonitor* CPacketServiceMonitor::NewL( 
    MSIPBearerMonitorObserver& aObserver )
	{
	CPacketServiceMonitor* self = 
	    new( ELeave ) CPacketServiceMonitor( aObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}


// -----------------------------------------------------------------------------
// CPacketServiceMonitor::~CPacketServiceMonitor
// -----------------------------------------------------------------------------
//
CPacketServiceMonitor::~CPacketServiceMonitor()
	{
	Cancel();
	delete iMsClassWatcher;
	iPacketService.Close();
	iPhone.Close();
	iTelServer.Close();
	}

// -----------------------------------------------------------------------------
// CPacketServiceMonitor::IsSuspended
// -----------------------------------------------------------------------------
//
TBool CPacketServiceMonitor::IsSuspended() const
	{
	return iSuspended;
	}

// -----------------------------------------------------------------------------
// CPacketServiceMonitor::IsLineMonitoringNeeded
// -----------------------------------------------------------------------------
//
TBool CPacketServiceMonitor::IsLineMonitoringNeeded() const
	{
	return iMsClassWatcher->IsLineMonitoringNeeded();
	}

// -----------------------------------------------------------------------------
// CPacketServiceMonitor::IsSupportedMSClass
// -----------------------------------------------------------------------------
//	
TBool CPacketServiceMonitor::IsSupportedMSClass() const
    {
    return iMsClassWatcher->IsSupportedMSClass();
    }

// -----------------------------------------------------------------------------
// CPacketServiceMonitor::WatcherStateChanged
// -----------------------------------------------------------------------------
//
void CPacketServiceMonitor::WatcherStateChanged( TInt aError )
	{
	iObserver.MonitorStateChanged( aError );
	}

// -----------------------------------------------------------------------------
// CPacketServiceMonitor::DoCancel
// -----------------------------------------------------------------------------
//
void CPacketServiceMonitor::DoCancel()
	{
	iPacketService.CancelAsyncRequest( EPacketNotifyStatusChange );
	}

// -----------------------------------------------------------------------------
// CPacketServiceMonitor::RunL
// -----------------------------------------------------------------------------
//
void CPacketServiceMonitor::RunL()
	{
	TBool suspended = IsSuspended( iPacketServiceStatus );
	TBool stateChanged = ( iSuspended != suspended );

	// Notify observers about state changes and errors
	TInt errorCode = iStatus.Int();


	// Request further notification from ETel, before calling observer, which
	// can lead to deletion of this object.
	iPacketService.NotifyStatusChange( iStatus, iPacketServiceStatus );
	SetActive();


	if( stateChanged || ( errorCode != KErrNone ) )
		{
		iSuspended = suspended;
		iObserver.MonitorStateChanged( errorCode );
		}	
	}

// -----------------------------------------------------------------------------
// CPacketServiceMonitor::IsSuspended
// -----------------------------------------------------------------------------
//
TBool 
CPacketServiceMonitor::IsSuspended( RPacketService::TStatus aStatus ) const
	{
	return ( aStatus == RPacketService::EStatusSuspended );
	}

// -----------------------------------------------------------------------------
// CPacketServiceMonitor::CPacketServiceMonitor
// -----------------------------------------------------------------------------
//
CPacketServiceMonitor::CPacketServiceMonitor( 
    MSIPBearerMonitorObserver& aObserver )
	: CActive( CActive::EPriorityStandard ),
	  iObserver( aObserver ),
	  iSuspended( EFalse )
	{
	CActiveScheduler::Add( this );
	}

// -----------------------------------------------------------------------------
// CPacketServiceMonitor::ConstructL
// -----------------------------------------------------------------------------
//
void CPacketServiceMonitor::ConstructL()
	{
	// Look for phones, report failure if none was found
	TInt phones;
	User::LeaveIfError( iTelServer.Connect() );
	User::LeaveIfError( iTelServer.EnumeratePhones( phones ) );

	if( phones > 0 )
		{
		// Get a phone
		RTelServer::TPhoneInfo phoneinfo;
		User::LeaveIfError( iTelServer.GetPhoneInfo( 0, phoneinfo ) );
		User::LeaveIfError( iPhone.Open( iTelServer, phoneinfo.iName ) );

		// Open packet service from the phone
		User::LeaveIfError( iPacketService.Open( iPhone ) );

		// Get current status
		User::LeaveIfError( iPacketService.GetStatus( iPacketServiceStatus ) );
		iSuspended = IsSuspended( iPacketServiceStatus );

		// Request a notification from ETel for status changes
		iPacketService.NotifyStatusChange( iStatus, iPacketServiceStatus );
		SetActive();
		}
	else
		{
		User::Leave( KErrNotSupported );
		}

	iMsClassWatcher = CMsClassWatcher::NewL( *this, iPacketService );
	}
