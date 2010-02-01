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
// Name        : CConnectionMonitor.cpp
// Part of     : NetworkMonitor
// Implementation
// Version     : SIP/4.0
//



#include <connectprog.h>
#include "CConnectionMonitor.h"
#include "sipnetworkobserver.h"
#include "SipLogs.h"

// -----------------------------------------------------------------------------
// CConnectionMonitor::NewLC
// -----------------------------------------------------------------------------
//
CConnectionMonitor* CConnectionMonitor::NewL( MSIPNetworkObserver& aObserver,
								 			  RConnection& aConnection )
	{
	return new( ELeave ) CConnectionMonitor( aObserver, aConnection );
	}

// -----------------------------------------------------------------------------
// CConnectionMonitor::~CConnectionMonitor
// -----------------------------------------------------------------------------
//
CConnectionMonitor::~CConnectionMonitor()
	{
	StopMonitoring();
	}
	
// -----------------------------------------------------------------------------
// CConnectionMonitor::StartMonitoring
// -----------------------------------------------------------------------------
//
void CConnectionMonitor::StartMonitoring()
	{
	__SIP_LOG( "CConnectionMonitor::StartMonitoring entered" )
	
	iMonitorTotalClosure = EFalse;	
	StartMonitor();
	
	__SIP_LOG( "CConnectionMonitor::StartMonitoring returns" )
	}
		
// -----------------------------------------------------------------------------
// CConnectionMonitor::StopMonitoring
// -----------------------------------------------------------------------------
//
void CConnectionMonitor::StopMonitoring()
	{
	__SIP_LOG( "CConnectionMonitor::StopMonitoring entered" )
	
	Cancel();
	
	__SIP_LOG( "CConnectionMonitor::StopMonitoring returns" )
	}

// -----------------------------------------------------------------------------
// CConnectionMonitor::MonitorConnectionClosure
// -----------------------------------------------------------------------------
//	
void CConnectionMonitor::MonitorConnectionClosure()
    {
    iMonitorTotalClosure = ETrue;
    StartMonitor();
    }

// -----------------------------------------------------------------------------
// CConnectionMonitor::DoCancel
// -----------------------------------------------------------------------------
//
void CConnectionMonitor::DoCancel()
	{
	iConnection.CancelProgressNotification();
	}

// -----------------------------------------------------------------------------
// CConnectionMonitor::RunL
// -----------------------------------------------------------------------------
//
void CConnectionMonitor::RunL()
	{
	__SIP_INT_LOG1( "CConnectionMonitor::RunL", iStatus.Int() )
	
	TBool isConnected = TranslateStage( iProgress() );

	// Notify observers about state changes and errors
	TInt err = iStatus.Int();
	if( ( !isConnected ) || ( err != KErrNone ) )
		{
		iObserver.NetworkStateChanged(MSIPNetworkObserver::EConnectionLost,err);
		}
	else
		{		
		StartMonitor();
		}
	}

// -----------------------------------------------------------------------------
// CConnectionMonitor::StartMonitor
// -----------------------------------------------------------------------------
//
void CConnectionMonitor::StartMonitor()
    {
    if (!IsActive())
		{
		// Request further notification from NifMan
		iConnection.ProgressNotification( iProgress, iStatus );
		SetActive();
		}
    }
		
// -----------------------------------------------------------------------------
// CConnectionMonitor::TranslateStage
// -----------------------------------------------------------------------------
//
TBool CConnectionMonitor::TranslateStage( const TNifProgress& aProgress )
	{
	__SIP_INT_LOG1( "CConnectionMonitor::TranslateStage", aProgress.iStage )
	
	if ( iMonitorTotalClosure )
	    {
	    return ( aProgress.iStage != KConnectionUninitialised );
	    }
	    
	return ( aProgress.iStage != KConnectionClosed ) &&
		   ( aProgress.iStage != KLinkLayerClosed ) &&
		   ( aProgress.iStage != KConnectionFailure );
	}

// -----------------------------------------------------------------------------
// CConnectionMonitor::CConnectionMonitor
// -----------------------------------------------------------------------------
//
CConnectionMonitor::CConnectionMonitor( MSIPNetworkObserver& aObserver,
										RConnection& aConnection )
	: CActive( CActive::EPriorityStandard ),
	  iObserver( aObserver ),
	  iConnection( aConnection ),
	  iMonitorTotalClosure( EFalse )
	{
	CActiveScheduler::Add( this );
	}
