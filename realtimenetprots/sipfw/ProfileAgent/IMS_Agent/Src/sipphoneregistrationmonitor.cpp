// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : sipphoneregistrationmonitor.cpp
// Part of     : SIP Profile Agent / IMS Agent
// Version     : %version: 2 %
//


#include "sipphoneregistrationmonitor.h"
#include "sipphoneregistrationobserver.h"

// -----------------------------------------------------------------------------
// CSipPhoneRegistrationMonitor::NewL
// -----------------------------------------------------------------------------
//
CSipPhoneRegistrationMonitor* CSipPhoneRegistrationMonitor::NewL(
    RTelServer& aTelServer,
    const RTelServer::TPhoneInfo& aPhoneInfo, 
    MSipPhoneRegistrationObserver& aObserver )
	{
	CSipPhoneRegistrationMonitor* self 
	    = new( ELeave ) CSipPhoneRegistrationMonitor( aObserver );
	CleanupStack::PushL( self );
	self->ConstructL( aTelServer, aPhoneInfo );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CSipPhoneRegistrationMonitor::CSipPhoneRegistrationMonitor
// -----------------------------------------------------------------------------
//
CSipPhoneRegistrationMonitor::CSipPhoneRegistrationMonitor( 
    MSipPhoneRegistrationObserver& aObserver )
	: CActive( CActive::EPriorityStandard ),
	  iObserver( aObserver ),
	  iRegistrationStatus( RMobilePhone::ERegistrationUnknown )
	{
	CActiveScheduler::Add( this );
	}

// -----------------------------------------------------------------------------
// CSipPhoneRegistrationMonitor::ConstructL
// -----------------------------------------------------------------------------
//
#ifdef __WINSCW__

void CSipPhoneRegistrationMonitor::ConstructL( 
    RTelServer& /*aTelServer*/,
    const RTelServer::TPhoneInfo& /*aPhoneInfo*/ )
	{
    iRegistrationStatus = RMobilePhone::ERegisteredOnHomeNetwork;
	}

#else

void CSipPhoneRegistrationMonitor::ConstructL( 
    RTelServer& aTelServer,
    const RTelServer::TPhoneInfo& aPhoneInfo )
	{
	
	User::LeaveIfError( iPhone.Open( aTelServer, aPhoneInfo.iName ) );

    // Get current status
    TRequestStatus status;
    iPhone.GetNetworkRegistrationStatus( status, iRegistrationStatus );	
    User::WaitForRequest( status );
    User::LeaveIfError( status.Int() );
		
    // Start to monitor the status
    MonitorStatus();
	}
    
#endif

// -----------------------------------------------------------------------------
// CSipPhoneRegistrationMonitor::~CSipPhoneRegistrationMonitor
// -----------------------------------------------------------------------------
//
CSipPhoneRegistrationMonitor::~CSipPhoneRegistrationMonitor()
	{
	Cancel();
	iPhone.Close();
	}

// -----------------------------------------------------------------------------
// CSipPhoneRegistrationMonitor::Status
// -----------------------------------------------------------------------------
//
CSipPhoneRegistrationMonitor::TStatus 
CSipPhoneRegistrationMonitor::Status() const
	{
	TStatus status = ENotRegistered;
	switch ( iRegistrationStatus )
	    {
	    case RMobilePhone::ERegisteredOnHomeNetwork:
	        status = ERegisteredOnHomeNetwork;
	        break;
	    case RMobilePhone::ERegisteredRoaming:
	        status = ERegisteredRoaming;
	        break;
	    default:
	        break;
	    }
	return status;
	}

// -----------------------------------------------------------------------------
// CSipPhoneRegistrationMonitor::DoCancel
// -----------------------------------------------------------------------------
//
void CSipPhoneRegistrationMonitor::DoCancel()
	{
	iPhone.CancelAsyncRequest( 
	    EMobilePhoneNotifyNetworkRegistrationStatusChange );
	}

// -----------------------------------------------------------------------------
// CSipPhoneRegistrationMonitor::RunL
// -----------------------------------------------------------------------------
//
void CSipPhoneRegistrationMonitor::RunL()
	{
	TInt err = iStatus.Int();
	
	if ( err )
	    {
	    iObserver.PhoneRegistrationStatusError( err );
	    }
	else
	    {
	    MonitorStatus();
	    iObserver.PhoneRegistrationStatusChangedL();	
	    }
	}

// -----------------------------------------------------------------------------
// CSipPhoneRegistrationMonitor::RunError
// -----------------------------------------------------------------------------
//
TInt CSipPhoneRegistrationMonitor::RunError(TInt aError)
    {
    iObserver.PhoneRegistrationStatusError( aError );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSipPhoneRegistrationMonitor::MonitorStatus
// -----------------------------------------------------------------------------
//	
void CSipPhoneRegistrationMonitor::MonitorStatus()
	{
#ifndef __WINSCW__

	// Request further notification from ETel
	iPhone.NotifyNetworkRegistrationStatusChange( iStatus,
												  iRegistrationStatus );
	SetActive();
	
#endif	
	}
