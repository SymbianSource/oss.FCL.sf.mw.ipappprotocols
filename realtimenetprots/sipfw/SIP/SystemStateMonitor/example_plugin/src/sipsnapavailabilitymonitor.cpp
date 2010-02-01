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
// Name        : sipsnapavailabilitymonitor.cpp
// Part of     : SIP / SIP System State Monitor
// Version     : SIP/6.0
//



#include "sipsnapavailabilitymonitor.h"
#include <sipsystemstateobserver.h>

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::NewL
// -----------------------------------------------------------------------------
//
CSipSnapAvailabilityMonitor* CSipSnapAvailabilityMonitor::NewL( 
    TUint32 aSnapId,
    MSipSystemStateObserver& aObserver )
	{
	CSipSnapAvailabilityMonitor* self = 
	    CSipSnapAvailabilityMonitor::NewLC( aSnapId, aObserver );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::NewLC
// -----------------------------------------------------------------------------
//
CSipSnapAvailabilityMonitor* CSipSnapAvailabilityMonitor::NewLC( 
    TUint32 aSnapId,
    MSipSystemStateObserver& aObserver )
	{
	CSipSnapAvailabilityMonitor* self = 
	    new( ELeave ) CSipSnapAvailabilityMonitor( aSnapId );
	CleanupStack::PushL( self );
	self->ConstructL( aObserver );
	return self;
	}

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::CSipSnapAvailabilityMonitor
// -----------------------------------------------------------------------------
//
CSipSnapAvailabilityMonitor::CSipSnapAvailabilityMonitor( 
    TUint32 aSnapId )
 : CActive( EPriorityStandard ),
   iSnapId( aSnapId )
    {
	CActiveScheduler::Add( this );
	}

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::ConstructL
// -----------------------------------------------------------------------------
//
void CSipSnapAvailabilityMonitor::ConstructL( 
    MSipSystemStateObserver& /* aObserver */ )
	{
	// Check the current availability for iSnapId and set iSnapAvailability
	// accordingly. Subscribe to an asynchronous service that provides the
	// SNAP availability information.
	}

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::~CSipSnapAvailabilityMonitor
// -----------------------------------------------------------------------------
//
CSipSnapAvailabilityMonitor::~CSipSnapAvailabilityMonitor()
	{
	CActive::Cancel();
	iObservers.Close();
	}

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::RunL
// -----------------------------------------------------------------------------
//
void CSipSnapAvailabilityMonitor::RunL()
    {
    // Set value for iSnapAvailability according to the notified SNAP
    // availability, re-subscribe to the asynchronous service that provides
    // the SNAP availability information and notify the observers.
    }

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::DoCancel
// -----------------------------------------------------------------------------
//
void CSipSnapAvailabilityMonitor::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::SnapId
// -----------------------------------------------------------------------------
//
TUint32 CSipSnapAvailabilityMonitor::SnapId() const
    {
    return iSnapId;
    }

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::SnapAvailability
// -----------------------------------------------------------------------------
//
CSipSystemStateMonitor::TSnapAvailability 
CSipSnapAvailabilityMonitor::SnapAvailability() const
    {
    return iSnapAvailability;
    }    

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::AddObserverL
// -----------------------------------------------------------------------------
//
void CSipSnapAvailabilityMonitor::AddObserverL( 
    MSipSystemStateObserver& aObserver )
    {
    // Add observer to the observer list
    iObservers.InsertInAddressOrderL( &aObserver );
    }

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::RemoveObserver
// -----------------------------------------------------------------------------
//
void CSipSnapAvailabilityMonitor::RemoveObserver( 
    MSipSystemStateObserver& aObserver )
    {
    // Remove observer from the observer list
    TInt index = iObservers.Find( &aObserver );
    if ( index >= 0 )
        {
        iObservers.Remove( index ); 
        }
    }

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::HasObservers
// -----------------------------------------------------------------------------
//
TBool CSipSnapAvailabilityMonitor::HasObservers() const
    {
    return ( iObservers.Count() > 0 );
    }
    
// End of File
