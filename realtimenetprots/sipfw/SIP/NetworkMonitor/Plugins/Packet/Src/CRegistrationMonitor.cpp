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
// Name        : CRegistrationMonitor.cpp
// Part of     : NetworkMonitor
// Implementation
// Version     : SIP/4.0
//



#include "CRegistrationMonitor.h"
#include "sipnetworkobserver.h"
#include "sipbearermonitorobserver.h"

// -----------------------------------------------------------------------------
// CRegistrationMonitor::NewL
// -----------------------------------------------------------------------------
//
CRegistrationMonitor* CRegistrationMonitor::NewL( 
    MSIPBearerMonitorObserver& aObserver )
	{
	CRegistrationMonitor* self = new( ELeave ) CRegistrationMonitor( aObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CRegistrationMonitor::~CRegistrationMonitor
// -----------------------------------------------------------------------------
//
CRegistrationMonitor::~CRegistrationMonitor()
	{
	Cancel();
	iPhone.Close();
	iTelServer.Close();
	}

// -----------------------------------------------------------------------------
// CRegistrationMonitor::IsRegistered
// -----------------------------------------------------------------------------
//
TBool CRegistrationMonitor::IsRegistered() const
	{
	return ( iRegistrationStatus == RMobilePhone::ERegisteredOnHomeNetwork ||
		     iRegistrationStatus == RMobilePhone::ERegisteredRoaming );
	}

// -----------------------------------------------------------------------------
// CRegistrationMonitor::DoCancel
// -----------------------------------------------------------------------------
//
void CRegistrationMonitor::DoCancel()
	{
	iPhone.CancelAsyncRequest(
					EMobilePhoneNotifyNetworkRegistrationStatusChange );
	}

// -----------------------------------------------------------------------------
// CCoverageMonitor::RunL
// -----------------------------------------------------------------------------
//
void CRegistrationMonitor::RunL()
	{
	TBool isRegistered = IsRegistered();	
	TInt errorCode = iStatus.Int();
	
	MonitorStatus();

	// Notify observers about state changes and errors
	if( ( isRegistered != iIsRegistered ) || ( errorCode != KErrNone ) )
		{
		iIsRegistered = isRegistered;
		iObserver.MonitorStateChanged( errorCode );
		}	
	}

// -----------------------------------------------------------------------------
// CRegistrationMonitor::CRegistrationMonitor
// -----------------------------------------------------------------------------
//
CRegistrationMonitor::CRegistrationMonitor( 
    MSIPBearerMonitorObserver& aObserver )
	: CActive( CActive::EPriorityStandard ),
	  iObserver( aObserver ),
	  iRegistrationStatus( RMobilePhone::ERegistrationUnknown ),
	  iIsRegistered( EFalse )
	{
	CActiveScheduler::Add( this );
	}

// -----------------------------------------------------------------------------
// CRegistrationMonitor::ConstructL
// -----------------------------------------------------------------------------
//
void CRegistrationMonitor::ConstructL()
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

		// Get current status
		TRequestStatus status;
		iPhone.GetNetworkRegistrationStatus( status, iRegistrationStatus );
		User::WaitForRequest( status );
		User::LeaveIfError( status.Int() );
		iIsRegistered = IsRegistered();
		
		// Start to monitor the status
		MonitorStatus();
		}
	else
		{
		User::Leave( KErrNotSupported );
		}
	}

// -----------------------------------------------------------------------------
// CRegistrationMonitor::MonitorStatus
// -----------------------------------------------------------------------------
//	
void CRegistrationMonitor::MonitorStatus()
	{
	// Request further notification from ETel
	iPhone.NotifyNetworkRegistrationStatusChange( iStatus,
												  iRegistrationStatus );
	SetActive();	
	}
	
