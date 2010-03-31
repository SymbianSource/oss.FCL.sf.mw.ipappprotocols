/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


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
	    CSipSnapAvailabilityMonitor::NewLC( 
	        aSnapId, aObserver );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::NewL
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
    TUint32 aSnapId)
 : CActive( EPriorityStandard ),
   iSnapId( aSnapId ),
   iIsConnected( EFalse )
    {
	CActiveScheduler::Add( this );
	}

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::ConstructL
// -----------------------------------------------------------------------------
//
void CSipSnapAvailabilityMonitor::ConstructL( 
    MSipSystemStateObserver& aObserver )
	{
	iObservers.AppendL( &aObserver );
	
	User::LeaveIfError( iConnMon.ConnectL() );
	iIsConnected = ETrue;
	User::LeaveIfError( iConnMon.NotifyEventL( *this ) );
	iConnMon.GetPckgAttribute( EBearerIdAll, 0, KSNAPsAvailability,
							   iSnapInfoBuf, iStatus );
	SetActive();
	}

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::~CSipSnapAvailabilityMonitor
// -----------------------------------------------------------------------------
//
CSipSnapAvailabilityMonitor::~CSipSnapAvailabilityMonitor()
	{
	CActive::Cancel();

	// Cancel notify events here and not in DoCancel, since they are not related
	// to CSipSnapAvailabilityMonitor being active.
	if ( iIsConnected )
		{
		iConnMon.CancelNotifications();
		}

	iConnMon.Close();
	iObservers.Close();
	}

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::RunL
// -----------------------------------------------------------------------------
//
void CSipSnapAvailabilityMonitor::RunL()
    {    
    if ( iStatus == KErrNone )
        {
        iSnapAvailable = IsSnapAvailable( iSnapInfoBuf(), iSnapId );
        NotifyObservers();
        }
    }

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::DoCancel
// -----------------------------------------------------------------------------
//	
void CSipSnapAvailabilityMonitor::DoCancel()
    {
    // Cancel the asynchronous GetPckgAttribute request
    iConnMon.CancelAsyncRequest( EConnMonGetPckgAttribute );
    }

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::EventL
// -----------------------------------------------------------------------------
//
void CSipSnapAvailabilityMonitor::EventL( 
    const CConnMonEventBase& aConnMonEvent )
    {    
	// We're interested only on SNAP availability notifications
	if( aConnMonEvent.EventType() == EConnMonSNAPsAvailabilityChange )
	    {	    
	    // Typecast event to appropriate event type
		const CConnMonSNAPsAvailabilityChange& event =
			static_cast< const CConnMonSNAPsAvailabilityChange& >( 
			    aConnMonEvent );

        TBool snapAvailable = 
            IsSnapAvailable( event.SNAPAvailability(), iSnapId );

        if ( SetCurrentState( snapAvailable ) )
            {
            NotifyObservers();
            }
	    }
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
    CSipSystemStateMonitor::TSnapAvailability snapAvailability = 
        CSipSystemStateMonitor::ESnapNotAvailable;
    
    if ( iSnapAvailable )
        {
        snapAvailability = CSipSystemStateMonitor::ESnapAvailable;
        }
    
    return snapAvailability;
    }    

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::AddObserverL
// -----------------------------------------------------------------------------
//
void CSipSnapAvailabilityMonitor::AddObserverL( 
    MSipSystemStateObserver& aObserver )
    {
    iObservers.InsertInAddressOrderL( &aObserver );
    }

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::RemoveObserver
// -----------------------------------------------------------------------------
//
void CSipSnapAvailabilityMonitor::RemoveObserver( 
    MSipSystemStateObserver& aObserver )
    {
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

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::NotifyObservers
// -----------------------------------------------------------------------------
//
void CSipSnapAvailabilityMonitor::NotifyObservers() const
    {
    CSipSystemStateMonitor::TSnapAvailability snapAvailability = 
        CSipSystemStateMonitor::ESnapNotAvailable;
    
    if ( CanSnapBeUsed() )
        {
        snapAvailability = CSipSystemStateMonitor::ESnapAvailable;
        }
        
    for ( TInt i = iObservers.Count()-1; i >= 0; i-- )
        {
        iObservers[i]->SystemVariableUpdated( 
            CSipSystemStateMonitor::ESnapAvailability,
            iSnapId, 
            snapAvailability );
        }
    }

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::IsSnapAvailable
// -----------------------------------------------------------------------------
//
TBool CSipSnapAvailabilityMonitor::IsSnapAvailable( 
    const TConnMonSNAPInfo& aSnapInfo, 
    TUint32 aSnapId ) const
    {
    for ( TUint32 i = 0; i < aSnapInfo.iCount; i++ )
        {
        if ( aSnapInfo.iSNAP[ i ].iSNAPId == aSnapId )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::SetCurrentState
// -----------------------------------------------------------------------------
//
TBool CSipSnapAvailabilityMonitor::SetCurrentState(    
    TBool aSnapAvailable )
    {
    TBool updated( EFalse );
    if ( aSnapAvailable != iSnapAvailable )
        {
        iSnapAvailable = aSnapAvailable;
        updated = ETrue;
        }    
    return updated;
    }

// -----------------------------------------------------------------------------
// CSipSnapAvailabilityMonitor::CanSnapBeUsed
// -----------------------------------------------------------------------------
//
TBool CSipSnapAvailabilityMonitor::CanSnapBeUsed() const
    {
    return ( iSnapAvailable ); 
    }
   
// End of File
