// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : sipalrsnapmonitor.cpp
// Part of       : SIP / SIP ALR Monitor
// Version       : SIP/6.0 
//



//  INCLUDES
#include "sipalrobserver.h"
#include "sipalrsnapmonitor.h"
#include "../../Profile/Inc/SipProfileLog.h"

// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::NewLC
// -----------------------------------------------------------------------------
//
CSipAlrSnapMonitor* CSipAlrSnapMonitor::NewLC(
    TUint32 aSnapId,
    MSipAlrObserver& aObserver,
    RSocketServ& aSocketServer,
    CSipSystemStateMonitor& aSystemStateMonitor )
	{
  	CSipAlrSnapMonitor* self = 
  	    new ( ELeave ) CSipAlrSnapMonitor( 
  	        aSnapId, aSocketServer, aSystemStateMonitor );
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    return self;	
	}

// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::CSipAlrSnapMonitor
// -----------------------------------------------------------------------------
//	
CSipAlrSnapMonitor::CSipAlrSnapMonitor( 
    TUint32 aSnapId,
    RSocketServ& aSocketServer,
    CSipSystemStateMonitor& aSystemStateMonitor ) : 
    CActive ( CActive::EPriorityStandard ),
    iSnapId( aSnapId ),
    iSocketServer( aSocketServer ),
	iSystemStateMonitor( aSystemStateMonitor )									
	{
	iPrefs.SetSnap( iSnapId );
	CActiveScheduler::Add( this );
	}

// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::ConstructL
// -----------------------------------------------------------------------------
//
void CSipAlrSnapMonitor::ConstructL( MSipAlrObserver& aObserver )
	{
	PROFILE_DEBUG1( "CSipAlrSnapMonitor::ConstructL entered" )
	
	iSystemStateMonitor.StartMonitoringL( 
	    CSipSystemStateMonitor::ESnapAvailability, iSnapId, *this );

  	AddObserverL( aObserver );
	
	PROFILE_DEBUG1( "CSipAlrSnapMonitor::ConstructL returns" )
	}
	
// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::~CSipAlrSnapMonitor
// -----------------------------------------------------------------------------
//
CSipAlrSnapMonitor::~CSipAlrSnapMonitor()
	{
	PROFILE_DEBUG1( "CSipAlrSnapMonitor::~CSipAlrSnapMonitor entered" )
	
    // MMobilityProtocolResp::Error is called from ~CActiveCommsMobilityApiExt.
    // Flag to prevent starting the RConnection.
	iDying = ETrue;
	
	ResetState();
	
	iObservers.Close();
	
	iSystemStateMonitor.StopMonitoring( 
	    CSipSystemStateMonitor::ESnapAvailability, iSnapId, *this );
	    
	PROFILE_DEBUG1( "CSipAlrSnapMonitor::~CSipAlrSnapMonitor returns" )
	}

// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::RunL
// -----------------------------------------------------------------------------
// 
void CSipAlrSnapMonitor::RunL()
    {
    PROFILE_DEBUG3( "CSipAlrSnapMonitor::RunL, status:", iStatus.Int() )
    
    TInt err = iStatus.Int();
    
    if ( err == KErrNone )
        {
        iConnectionActive = ETrue;
        CreateMobilityAoL();
        
        // Generate initial IAP availability event
        TUint32 iap( 0 );
        iConnection.GetIntSetting( _L("IAP\\Id"), iap );
        NotifyObservers( iap ); // Gives only IAP available event
        
        if ( !iFirstStartHasSucceeded || iMigrationAllowedByClient )
            {
            // Don't wait for migration permission when the SNAP is started for
            // the very first time or when the whole SNAP has been unavailable.
            NotifyInitializedObservers( iap, MSipAlrObserver::EIapActive );
            iFirstStartHasSucceeded = ETrue;
            }
        }
	else if ( err == KErrCancel )
		{
		for ( TInt i = iObservers.Count() - 1; i >= 0; --i )
			{
			iObservers[i].iObserver->AlrEvent(
				MSipAlrObserver::EOfferedIapRejected, iSnapId, KNoIap );
			}
		}
    else
        {
        ResetState();
        if ( err != KErrNotFound )
            {
            // Do not start SNAP again here if the error is KErrNotFound.
            // This error could indicate that the only IAP available is invalid 
            // and there would be an infinite loop until another valid IAP
            // becomes available.
            StartSnap();
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::DoCancel
// -----------------------------------------------------------------------------
// 	
void CSipAlrSnapMonitor::DoCancel()
    {
    iConnection.Close();
    }
    
// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::RunError
// -----------------------------------------------------------------------------
// 		
TInt CSipAlrSnapMonitor::RunError( TInt /*aError*/ )
    {    
    ResetState();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::PreferredCarrierAvailable
// -----------------------------------------------------------------------------
// 
void CSipAlrSnapMonitor::PreferredCarrierAvailable(
    TAccessPointInfo /*aOldAP*/, 
    TAccessPointInfo aNewAP, 
    TBool /*aIsUpgrade*/, 
    TBool /*aIsSeamless*/ )
    {
    PROFILE_DEBUG1( "CSipAlrSnapMonitor::PreferredCarrierAvailable" )
    iPreferredCarrierAvailableCalled = ETrue;
    NotifyObservers( aNewAP.AccessPoint() );
    }
    
// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::NewCarrierActive
// -----------------------------------------------------------------------------
//    	    
void CSipAlrSnapMonitor::NewCarrierActive(
    TAccessPointInfo aNewAP, 
    TBool /*aIsSeamless*/ )
    {
    PROFILE_DEBUG3( "CSipSnapAlrMonitor::NewCarrierActive", iSnapId )
    
    NotifyInitializedObservers( aNewAP.AccessPoint(), 
                                MSipAlrObserver::EIapActive );
    }

// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::Error
// -----------------------------------------------------------------------------
// 	    
void CSipAlrSnapMonitor::Error( TInt aError )
    {
    PROFILE_DEBUG3( "CSipSnapAlrMonitor::Error aError=", aError )
    if ( !iCommsMobilityAO )
    	{
    	PROFILE_DEBUG1( "CSipSnapAlrMonitor::Error iCommsMobilityAO==NULL")
    	return;
    	}
    if ( aError == KErrNotFound)
	    {
	    NotifyInitializedObservers( KNoIap,
	    							MSipAlrObserver::ENoNewIapAvailable );
	    }

    // MMobilityProtocolResp::Error is called from ~CActiveCommsMobilityApiExt.
    // Flag iDying prevents starting the RConnection if not needed.
    if ( !iDying )
        {
        ResetState();
        StartSnap();
        }
    }

// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::SystemVariableUpdated
// -----------------------------------------------------------------------------
//
void CSipAlrSnapMonitor::SystemVariableUpdated( 
    CSipSystemStateMonitor::TSystemVariable aVariable,
    TInt aObjectId,
    TInt aValue )
    {
    PROFILE_DEBUG4( "CSipAlrSnapMonitor::SystemVariableUpdated, snap, value",
                    aObjectId, aValue )
    
    if ( !IsActive() &&
         aVariable == CSipSystemStateMonitor::ESnapAvailability &&
         aObjectId == iSnapId )
        {
        if ( aValue == CSipSystemStateMonitor::ESnapAvailable )
            {
            if ( StartSnap() != KErrNone )
                {
                ResetState();
                }
            }
        else
            {
            ResetState();
            }
        }
    }

// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::SnapId
// -----------------------------------------------------------------------------
//		
TUint32 CSipAlrSnapMonitor::SnapId() const
	{
	return iSnapId;
	}
			
// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::AddObserverL
// -----------------------------------------------------------------------------
//
void CSipAlrSnapMonitor::AddObserverL( MSipAlrObserver& aObserver )
	{
	PROFILE_DEBUG1( "CSipAlrSnapMonitor::AddObserverL" )
		
	TSipAlrSnapObserverInfo newObserver;
	newObserver.iObserver = &aObserver;
	newObserver.iInitialEventDone = EFalse;
	iObservers.AppendL( newObserver );
	}
	
// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::DetachObserver
// -----------------------------------------------------------------------------
//
TBool CSipAlrSnapMonitor::DetachObserver( MSipAlrObserver& aObserver )
	{
	PROFILE_DEBUG1( "CSipAlrSnapMonitor::DetachObserver" )

	for ( TInt i = iObservers.Count() - 1; i >= 0; i-- )
		{
		if ( iObservers[i].iObserver == &aObserver )
			{
			iObservers.Remove( i );	
			}
		}

	return iObservers.Count() == 0;
	}
	  
// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::RefreshL
// -----------------------------------------------------------------------------
//
void CSipAlrSnapMonitor::RefreshL()
	{
	PROFILE_DEBUG1( "CSipAlrSnapMonitor::RefreshL entered" )

    if ( iConnectionActive )
        {
		DestroyMobilityAo();
		PROFILE_DEBUG1( "CSipAlrSnapMonitor::RefreshL old ApiExt deleted" )

		// Earlier a new iCommsMobilityAO instance was created before deleting
		// old iCommsMobilityAO, but since it causes jam, now new instance is
		// created after deleting the old one.
		// If NewL fails, this CSipAlrSnapMonitor is useless and won't get
		// PreferredCarrierAvailable. If would let NewL leave here, then would
		// have to TRAP it from each place leading here.
		TRAPD( err, iCommsMobilityAO =
			   CActiveCommsMobilityApiExt::NewL( iConnection, *this ) );
		if ( err != KErrNone )
			{
			// Notify observers starting from the end of the array as it causes
			// entries to remove themselves from the array. This also leads to
			// CSipAlrMonitor::FreeResources, deleting this CSipAlrSnapMonitor.
			// So after last AlrEvent(), don't access this object's data.
			PROFILE_DEBUG3( "CSipAlrSnapMonitor::RefreshL error", err )
			for ( TInt i = iObservers.Count() - 1; i >= 0; --i )
				{
				iObservers[i].iObserver->AlrEvent(
					MSipAlrObserver::ERefreshError, iSnapId, KNoIap );
				}
			}
        }

    PROFILE_DEBUG1( "CSipAlrSnapMonitor::Refresh returns" )
	}

// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::AllowMigration
// -----------------------------------------------------------------------------
//
void CSipAlrSnapMonitor::AllowMigration()
    {
    PROFILE_DEBUG1( "CSipAlrSnapMonitor::AllowMigration" )
    
    iMigrationAllowedByClient = ETrue;
    if ( iPreferredCarrierAvailableCalled && iCommsMobilityAO )
        {
        PROFILE_DEBUG1( "CSipAlrSnapMonitor::AllowMigration: migrating" )
        iCommsMobilityAO->MigrateToPreferredCarrier();
        }
    }
// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::DisallowMigration
// -----------------------------------------------------------------------------
//
void CSipAlrSnapMonitor::DisallowMigration()
    {
    PROFILE_DEBUG1( "CSipAlrSnapMonitor::DisallowMigration" )
    
    iMigrationAllowedByClient = EFalse;
    if ( iPreferredCarrierAvailableCalled && iCommsMobilityAO )
        {
        PROFILE_DEBUG1( "CSipAlrSnapMonitor::DisallowMigration: ignoring" )
        iCommsMobilityAO->IgnorePreferredCarrier();   
        }
    }
    
// -----------------------------------------------------------------------------
// CSipAlrMonitor::NewIapAccepted
// -----------------------------------------------------------------------------
//    
void CSipAlrSnapMonitor::NewIapAccepted()    
    {
    PROFILE_DEBUG1( "CSipAlrSnapMonitor::NewIapAccepted" )
    
    if ( iPreferredCarrierAvailableCalled && iCommsMobilityAO )
        {
        PROFILE_DEBUG1( "CSipAlrSnapMonitor::NewIapAccepted: accepting" )
        iCommsMobilityAO->NewCarrierAccepted();  
        }
    }
    
// -----------------------------------------------------------------------------
// CSipAlrMonitor::NewIapRejected
// -----------------------------------------------------------------------------
//  
void CSipAlrSnapMonitor::NewIapRejected()
    {
    PROFILE_DEBUG1( "CSipAlrSnapMonitor::NewCarrierRejected" )
    
    if ( iCommsMobilityAO )
        {
        PROFILE_DEBUG1( "CSipAlrSnapMonitor::NewCarrierRejected: rejecting" )
        iCommsMobilityAO->NewCarrierRejected();
        }
    }

// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::StartSnap
// Start SNAP if it is available.
// Otherwise just keep on waiting for the SNAP to become available.
// -----------------------------------------------------------------------------
//
TInt CSipAlrSnapMonitor::StartSnap()
    {
    PROFILE_DEBUG1( "CSipAlrSnapMonitor::StartSnap entered" )

    TInt err = KErrNone;
    if ( !IsActive() && IsSnapAvailable() )
        {
        PROFILE_DEBUG1( "CSipAlrSnapMonitor::StartSnap SNAP is available" )

    	err = iConnection.Open( iSocketServer );
		PROFILE_DEBUG3( "CSipAlrSnapMonitor::StartSnap Open done, err=", err )

    	if ( err == KErrNone )
    	    {
        	iConnection.Start( iPrefs, iStatus );
        	SetActive();
    	    }
        }

	PROFILE_DEBUG3( "CSipAlrSnapMonitor::StartSnap returns", err )
    return err;
    }

// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::CreateMobilityAoL
// -----------------------------------------------------------------------------
//	
void CSipAlrSnapMonitor::CreateMobilityAoL()
    {
    PROFILE_DEBUG1( "CSipAlrSnapMonitor::CreateMobilityAoL entered" )
    
    CActiveCommsMobilityApiExt* newAo = 
        CActiveCommsMobilityApiExt::NewL( iConnection, *this );
    DestroyMobilityAo();
    iCommsMobilityAO = newAo;
    
    PROFILE_DEBUG1( "CSipAlrSnapMonitor::CreateMobilityAoL returns" )
    }

// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::ResetState
// Delete all the existing objects and wait for the SNAP become available.
// -----------------------------------------------------------------------------
//
void CSipAlrSnapMonitor::ResetState()
    {
    PROFILE_DEBUG1( "CSipAlrSnapMonitor::ResetState entered" )

    DestroyMobilityAo();
    Cancel();
    iConnection.Close();
    iMigrationAllowedByClient = EFalse;
    iConnectionActive = EFalse;
    iPreferredCarrierAvailableCalled = EFalse;
    PROFILE_DEBUG1( "CSipAlrSnapMonitor::ResetState returns" )
    }

// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::DestroyMobilityAo
// MMobilityProtocolResp::Error is called from ~CActiveCommsMobilityApiExt.
// To prevent an infinite loop, set iCommsMobilityAO to NULL before deleting it.
// -----------------------------------------------------------------------------
//
void CSipAlrSnapMonitor::DestroyMobilityAo()
    {
    CActiveCommsMobilityApiExt* tmp = iCommsMobilityAO;
    iCommsMobilityAO = NULL;
    delete tmp;
    }

// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::NotifyObservers
// -----------------------------------------------------------------------------
//
void CSipAlrSnapMonitor::NotifyObservers( TUint aIapId )
	{
	PROFILE_DEBUG4( "CSipSnapAlrMonitor::NotifyObservers SNAP, IAP",
	                iSnapId, aIapId )

	NotifyInitializedObservers( aIapId, MSipAlrObserver::EIapAvailable );
	NotifyNewObservers( aIapId );
	}
	
// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::NotifyInitializedObservers
// -----------------------------------------------------------------------------
//
void CSipAlrSnapMonitor::NotifyInitializedObservers( 
    TUint32 aIapId,
    MSipAlrObserver::TEvent aEvent )
	{
	for ( TInt i = 0; i < iObservers.Count(); i++ )
		{
		if ( iObservers[i].iInitialEventDone )
			{
			PROFILE_DEBUG3( "CSipAlrSnapMonitor::NotifyIntitializedObservers",
			                iSnapId )			
			
			iObservers[i].iObserver->AlrEvent( aEvent, iSnapId, aIapId );
			}
		}
	}
	
// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::NotifyNewObservers
// -----------------------------------------------------------------------------
//
void CSipAlrSnapMonitor::NotifyNewObservers( TUint32 aIapId )
	{
	for ( TInt i = 0; i < iObservers.Count(); i++ )
		{
		if ( !iObservers[i].iInitialEventDone )
			{
			PROFILE_DEBUG3( "CSipAlrSnapMonitor::NotifyNewObservers", iSnapId )

			iObservers[i].iObserver->AlrEvent(
			    MSipAlrObserver::EIapAvailable, iSnapId, aIapId );

			iObservers[i].iInitialEventDone = ETrue;
			}
		}
	}

// -----------------------------------------------------------------------------
// CSipAlrSnapMonitor::IsSnapAvailable
// -----------------------------------------------------------------------------
//
TBool CSipAlrSnapMonitor::IsSnapAvailable() const
    {
    return ( iSystemStateMonitor.CurrentValue( 
	             CSipSystemStateMonitor::ESnapAvailability, iSnapId ) == 
	                 CSipSystemStateMonitor::ESnapAvailable );
    }
