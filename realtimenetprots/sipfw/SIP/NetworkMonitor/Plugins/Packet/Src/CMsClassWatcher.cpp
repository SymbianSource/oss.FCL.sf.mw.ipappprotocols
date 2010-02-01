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
// Name        : CMsClassWatcher.cpp
// Part of     : NetworkMonitor
// Implementation
// Version     : SIP/6.0
//



#include "CMsClassWatcher.h"
#include "CPacketServiceMonitor.h"
#include <pcktcs.h>
#include "SipLogs.h"

// -----------------------------------------------------------------------------
// CMsClassWatcher::NewL
// -----------------------------------------------------------------------------
//
CMsClassWatcher* CMsClassWatcher::NewL( CPacketServiceMonitor& aParent,
										RPacketService& aPacketService  )
	{
	CMsClassWatcher* self = new( ELeave ) CMsClassWatcher( aParent,
														   aPacketService);
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CMsClassWatcher::~CMsClassWatcher
// -----------------------------------------------------------------------------
//
CMsClassWatcher::~CMsClassWatcher()
	{
	Cancel();
	}

// -----------------------------------------------------------------------------
// CMsClassWatcher::IsLineMonitoringNeeded
// -----------------------------------------------------------------------------
//
TBool CMsClassWatcher::IsLineMonitoringNeeded() const
	{
	return ( iMsClass == RPacketService::EMSClassSuspensionRequired );
	}

// -----------------------------------------------------------------------------
// CMsClassWatcher::IsSupportedMSClass
// -----------------------------------------------------------------------------
//	
TBool CMsClassWatcher::IsSupportedMSClass() const
    {
    // Only class A and B are supported, however take in count that
    // at some points (e.g. phone start-up) MS class information might
    // not be available. 
        
    /*return ( !IsMSClassInfoAvailable() ||
             iMsClass == RPacketService::EMSClassDualMode || 
             iMsClass == RPacketService::EMSClassSuspensionRequired );
             */
    __SIP_INT_LOG1( "CMsClassWatcher::IsSupportedMSClass, return TRUE always (iMsClass)", iMsClass )
    return TRUE;
    
    }

// -----------------------------------------------------------------------------
// CMsClassWatcher::UpdateMSClassInfoAvailability
// -----------------------------------------------------------------------------
//
void CMsClassWatcher::UpdateMSClassInfoAvailability()
    {
    iMSClassInfoAvailable = ( iMsClass != RPacketService::EMSClassUnknown );
    }
    
// -----------------------------------------------------------------------------
// CMsClassWatcher::IsMSClassInfoAvailable
// -----------------------------------------------------------------------------
//
TBool CMsClassWatcher::IsMSClassInfoAvailable() const
    {
    return iMSClassInfoAvailable;
    }
    
// -----------------------------------------------------------------------------
// CMsClassWatcher::DoCancel
// -----------------------------------------------------------------------------
//
void CMsClassWatcher::DoCancel()
	{
	iPacketService.CancelAsyncRequest( EPacketNotifyMSClassChange );
	}
  
// -----------------------------------------------------------------------------
// CMsClassWatcher::RunL
// -----------------------------------------------------------------------------
//
void CMsClassWatcher::RunL()
	{
	UpdateMSClassInfoAvailability();

    TInt errorCode( iStatus.Int() );

	// If TSY/HW doesn't support this, stop monitoring and ignore the error
    if ( errorCode != KErrNotSupported )
    	{
		// Request notification from ETel about MS class change
		iPacketService.NotifyMSClassChange( iStatus, iMsClass );
		SetActive();

		// Report to our parent after everything else has been done, since it can
		// lead to destruction of this CMsClassWatcher instance.
		iParent.WatcherStateChanged( errorCode );
    	}
	}

// -----------------------------------------------------------------------------
// CMsClassWatcher::CMsClassWatcher
// -----------------------------------------------------------------------------
//
CMsClassWatcher::CMsClassWatcher( CPacketServiceMonitor& aParent,
								  RPacketService& aPacketService  )
	: CActive( CActive::EPriorityStandard ),
	  iParent( aParent ), iPacketService( aPacketService ),
	  iMsClass( RPacketService::EMSClassUnknown ),
	  iMSClassInfoAvailable( EFalse )
	{
	CActiveScheduler::Add( this );
	}

// -----------------------------------------------------------------------------
// CMsClassWatcher::ConstructL
// -----------------------------------------------------------------------------
//
void CMsClassWatcher::ConstructL()
	{
	// Request current MS class from ETel
	TRequestStatus status;
	iPacketService.GetMSClass( status, iMsClass, iMaxMsClass );
	User::WaitForRequest( status );
	User::LeaveIfError( status.Int() );
	
	UpdateMSClassInfoAvailability();
	
	__ASSERT_ALWAYS( IsSupportedMSClass(), User::Leave( KErrNotSupported ) );

	// Request notification from ETel about MS class change
	iPacketService.NotifyMSClassChange( iStatus, iMsClass );
	SetActive();
	}

