// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : CAccessNetworkMonitor.cpp
// Part of     : NetworkMonitor
// Implementation
// Version     : SIP/6.0
//



#include "CAccessNetworkMonitor.h"
#include "MAccessNetworkObserver.h"

// -----------------------------------------------------------------------------
// CAccessNetworkMonitor::NewL
// -----------------------------------------------------------------------------
//
CAccessNetworkMonitor*
CAccessNetworkMonitor::NewL( MAccessNetworkInfoObserver& aObserver )
	{	
	CAccessNetworkMonitor* self =
		new( ELeave ) CAccessNetworkMonitor( aObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );	
	return self;
	}

// -----------------------------------------------------------------------------
// CAccessNetworkMonitor::CAccessNetworkMonitor
// -----------------------------------------------------------------------------
//
CAccessNetworkMonitor::CAccessNetworkMonitor(
    MAccessNetworkInfoObserver& aObserver ) :
	CActive( CActive::EPriorityStandard ),
	iObserver( aObserver )
	{	
	CActiveScheduler::Add( this );
	}

// -----------------------------------------------------------------------------
// CAccessNetworkMonitor::ConstructL
// Get current information, then request notification of status changes
// -----------------------------------------------------------------------------
//
void CAccessNetworkMonitor::ConstructL()
	{	
	User::LeaveIfError( iTelServer.Connect() );

	TInt phones( 0 );
	User::LeaveIfError( iTelServer.EnumeratePhones( phones ) );
	__ASSERT_ALWAYS( phones > 0, User::Leave( KErrNotFound ) );	

	RTelServer::TPhoneInfo phoneinfo;
	User::LeaveIfError( iTelServer.GetPhoneInfo( 0, phoneinfo ) );	
	User::LeaveIfError( iPhone.Open( iTelServer, phoneinfo.iName ) );	

	iPhone.GetCurrentNetwork( iStatus, iNetworkInfoPckg, iLocationArea );	
	User::WaitForRequest( iStatus );	
	User::LeaveIfError( iStatus.Int() );	

	TSIPAccessNetworkInfo networkInfo( iNetworkInfoPckg, iLocationArea );
	iAccessNetworkInfo = networkInfo;	
	iObserver.InfoChanged( iAccessNetworkInfo );

	MonitorForChanges();
	}

// -----------------------------------------------------------------------------
// CAccessNetworkMonitor::~CAccessNetworkMonitor
// -----------------------------------------------------------------------------
//
CAccessNetworkMonitor::~CAccessNetworkMonitor()
	{	
	Cancel();
	iPhone.Close();
	iTelServer.Close();	
	}

// -----------------------------------------------------------------------------
// CAccessNetworkMonitor::DoCancel
// -----------------------------------------------------------------------------
//
void CAccessNetworkMonitor::DoCancel()
	{	
	iPhone.CancelAsyncRequest( EMobilePhoneNotifyCurrentNetworkChange );	
	}

// -----------------------------------------------------------------------------
// CAccessNetworkMonitor::RunL
// Notify observer only if information has changed.
// -----------------------------------------------------------------------------
//
void CAccessNetworkMonitor::RunL()
	{
	TInt errorCode( iStatus.Int() );

	MonitorForChanges();

	if ( errorCode == KErrNone)
		{
		TSIPAccessNetworkInfo newInfo( iNetworkInfoPckg, iLocationArea );
		if(!( newInfo == iAccessNetworkInfo ))
			{
			iAccessNetworkInfo = newInfo;
			iObserver.InfoChanged( iAccessNetworkInfo );
			}
		}
	}

// -----------------------------------------------------------------------------
// CAccessNetworkMonitor::MonitorForChanges
// -----------------------------------------------------------------------------
//
void CAccessNetworkMonitor::MonitorForChanges()
	{
	__ASSERT_ALWAYS( !IsActive(),
		User::Panic( _L( "AccNetwMoni:Mntr"), KErrNotReady ) );
	
	iPhone.NotifyCurrentNetworkChange( iStatus,
									   iNetworkInfoPckg,
									   iLocationArea );
	SetActive();	
	}
