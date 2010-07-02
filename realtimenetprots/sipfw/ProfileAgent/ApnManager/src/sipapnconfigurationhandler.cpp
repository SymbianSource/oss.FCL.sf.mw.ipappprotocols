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
// Name        : sipapnconfigurationhandler.cpp
// Part of     : SIP Profile Server
// implementation
// Version     : 1.0
//
#include <commsdattypesv1_1.h>
#include <commdb.h>
#include <commsdat.h>
#include <cmgenconnsettings.h>
#include <cmmanagerkeys.h>
#include "sipapnconfigurationhandler.h"
#include "SipProfileLog.h"

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::NewL
// -----------------------------------------------------------------------------
//
CSIPApnConfigurationHandler* CSIPApnConfigurationHandler::NewL( 
    MSIPApnChangeObserver& aObserver, TUint32 aIapId)
	{
	CSIPApnConfigurationHandler* self = 
	    CSIPApnConfigurationHandler::NewLC( aObserver, aIapId );
	CleanupStack::Pop( self );
	return self;
	}
	
// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::NewLC
// -----------------------------------------------------------------------------
//
CSIPApnConfigurationHandler* CSIPApnConfigurationHandler::NewLC( 
    MSIPApnChangeObserver& aObserver, TUint32 aIapId)
	{
	CSIPApnConfigurationHandler* self = 
	    new ( ELeave ) CSIPApnConfigurationHandler( aObserver, aIapId );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}


// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::~CSIPApnConfigurationHandler
// -----------------------------------------------------------------------------
//
CSIPApnConfigurationHandler::~CSIPApnConfigurationHandler()
	{
	PROFILE_DEBUG1( 
	        "CSIPApnConfigurationHandler::~CSIPApnConfigurationHandler()" )
	        
	Cancel();
	iConnection.Close();
	iSocketSrv.Close();
	delete iApnProposal;
	delete iCurrentApn;
    delete iPrimaryApn;
    delete iSecondaryApn;
    delete iRepository;
	delete iCommsDatabase;
	
	PROFILE_DEBUG1( 
	        "CSIPApnConfigurationHandler::~CSIPApnConfigurationHandler() exit" )
	}

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::SetApnL
// -----------------------------------------------------------------------------
//
void CSIPApnConfigurationHandler::SetApnL( 
    const TDesC8& aApn, 
    TBool aUseSecureAuthentication,
    TBool aAllowAsync )
    {
    PROFILE_DEBUG6( 
            "CSIPApnConfigurationHandler::SetApnL() apn", aApn )
    
    // Cancel if waiting for connection closure, will be re-issued if needed
      Cancel();
    
    // Store current apn setting
    HBufC8* apn = aApn.AllocL();
    delete iApnProposal;
    iApnProposal = apn;
    iApnUseSecureAuthProposal = aUseSecureAuthentication;
    
    if ( !ApnChangeNeededL( *iApnProposal ) )
        {
        PROFILE_DEBUG1( 
                "CSIPApnConfigurationHandler::SetApnL() apn already correct" )
        
        SendApnChangedNotificationL( *iApnProposal );
        return;
        }
    
    iDBMonitoringRetryCount = 0;
    
    ChangeApnIfNotInUseL( aAllowAsync );
        
    PROFILE_DEBUG1( 
            "CSIPApnConfigurationHandler::SetApnL() exit" )
    }

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::IsPrimaryApnUsed
// -----------------------------------------------------------------------------
//
TBool CSIPApnConfigurationHandler::IsPrimaryApnUsed()
    {
    return ( iCurrentApn && iCurrentApn->Compare( PrimaryApn() ) == 0 );
    }

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::ReadCurrentApnL
// -----------------------------------------------------------------------------
//
void CSIPApnConfigurationHandler::ReadCurrentApnL()
	{
	HBufC8* apn(NULL);
		
	using namespace CommsDat;
	
	CMDBSession* db = CMDBSession::NewL( CMDBSession::LatestVersion() );
    CleanupStack::PushL( db );
    // Set any attributes if any
    db->SetAttributeMask( ECDHidden ); 

    // Create an iap record
    CCDIAPRecord* iapRecord = 
        static_cast<CCDIAPRecord*>( 
            CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
    CleanupStack::PushL( iapRecord );
    
    iapRecord->SetRecordId( iIapId );

    iapRecord->LoadL( *db );
    
    // serviceType identifies the servicing table to use
    CMDBField<TDesC>* serviceType = 
        ( CMDBField<TDesC>* )iapRecord->GetFieldByIdL( KCDTIdIAPServiceType );
    
    __ASSERT_ALWAYS( serviceType && !serviceType->IsNull(), 
                     User::Leave( KErrNotFound ) );
	// Only this service type has APN in the service record
    if ( TPtrC( KCDTypeNameOutgoingWCDMA ).Compare( *serviceType ) == 0 )
        {
        PROFILE_DEBUG1(  
        "CSIPApnConfigurationHandler::ReadCurrentApnL(), wcdma service" )
        
        // iapRecord->iService field is a link to the servicing table. It tells
        // which record to use from the table.
        iapRecord->iService.LoadL( *db );
        
        if ( !iapRecord->iService.iLinkedRecord )
            {
            PROFILE_DEBUG1( 
            "CSIPApnConfigurationHandler::ReadCurrentApnL(), creating linked" )
        
            // Ownership of created record is transferred
            iapRecord->iService.iLinkedRecord = 
                static_cast<CCDOutgoingGprsRecord*>(
                    CCDRecordBase::RecordFactoryL( KCDTIdOutgoingGprsRecord ) );
            iapRecord->iService.iLinkedRecord->SetRecordId( iapRecord->iService );
            
            iapRecord->iService.iLinkedRecord->LoadL( *db );
            }
            
        PROFILE_DEBUG1( 
        "CSIPApnConfigurationHandler::ReadCurrentApnL(), linked service loaded" )
        
        CCDOutgoingGprsRecord* serviceRecord = 
        static_cast<CCDOutgoingGprsRecord*>( iapRecord->iService.iLinkedRecord ); 

        TDesC& currApn = serviceRecord->iGPRSAPN.GetL();
        PROFILE_DEBUG6( 
        	"CSIPApnConfigurationHandler::ReadCurrentApnL(), current apn",
        	currApn );

		apn = HBufC8::NewL( currApn.Length() );
		apn->Des().Copy( currApn );
		
		delete iCurrentApn;
		iCurrentApn = NULL;
		iCurrentApn = apn;
        }
    
    db->ClearAttributeMask( ECDHidden );
    
    CleanupStack::PopAndDestroy( iapRecord );
    CleanupStack::PopAndDestroy( db );
	}

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::HasPendingTasks
// -----------------------------------------------------------------------------
//
TBool CSIPApnConfigurationHandler::HasPendingTasks() const
    {
    return (MonitoringState() != EMonitoringIdle || IsFailed());
    }

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::DoCancel
// -----------------------------------------------------------------------------
//
void CSIPApnConfigurationHandler::DoCancel()
	{	
	TSipApnMonitoringState currentState = MonitoringState();

	PROFILE_DEBUG3( 
	        "CSIPApnConfigurationHandler::DoCancel() state", currentState );
	
	if ( currentState == EMonitoringConnection )
	    {
	    iConnection.CancelProgressNotification();
	    }
	else if ( currentState == EMonitoringDatabase )
	    {
	    if ( iCommsDatabase )
	        {
	        iCommsDatabase->CancelRequestNotification();
	        }
	    }
	else
	    {
	    // NOP
	    }
	
	SetMonitoringState( EMonitoringIdle );
	    
	PROFILE_DEBUG1( 
	        "CSIPApnConfigurationHandler::DoCancel() exit" )
	}

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::RunL
// -----------------------------------------------------------------------------
//
void CSIPApnConfigurationHandler::RunL()
	{	
	TInt error = iStatus.Int();
	
	TSipApnMonitoringState currentState = MonitoringState();
	
	PROFILE_DEBUG3( 
	        "CSIPApnConfigurationHandler::RunL() err", error );
	PROFILE_DEBUG3( 
	        "CSIPApnConfigurationHandler::RunL() state", currentState );
	
	SetMonitoringState( EMonitoringIdle ); // Clear current state
	    	
	if ( currentState == EMonitoringConnection )
	    {
	    ConnectionMonitoringCompletedL( error );
	    }
	else if ( currentState == EMonitoringDatabase )
	    {
	    DatabaseMonitoringCompletedL( error );
	    }
	    
    PROFILE_DEBUG1( 
            "CSIPApnConfigurationHandler::RunL() exit" )
	}

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::RunError
// -----------------------------------------------------------------------------
//
TInt CSIPApnConfigurationHandler::RunError( TInt aError )
    {
    PROFILE_DEBUG3( 
            "CSIPApnConfigurationHandler::RunError() err", aError );
    
    if(iCellularDataBlocked)
        {
        AllowCellularDataUsage();
        }
    
    if ( aError != KErrNoMemory && aError != KErrNone )
        {
        iObserver.ApnChanged( *iApnProposal, iIapId, aError );
        aError = KErrNone;
        }
        
    return aError;
    }
    
// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::CSIPApnConfigurationHandler
// -----------------------------------------------------------------------------
//
CSIPApnConfigurationHandler::CSIPApnConfigurationHandler( 
    MSIPApnChangeObserver& aObserver, TUint32 aIapId ) : 
    CActive( CActive::EPriorityStandard ),
    iObserver( aObserver ),
    iMonitoringState( EMonitoringIdle )
	{
	CActiveScheduler::Add( this );
	iIapId = aIapId;
	iIsFailed = EFalse;
	iIsFatalFailure = EFalse;
	iCellularDataBlocked = EFalse;
	iApnUseSecureAuthProposal = EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPApnConfigurationHandler::ConstructL()
	{	
	PROFILE_DEBUG1( 
	        "CSIPApnConfigurationHandler::ConstructL()" )
	
	User::LeaveIfError( iSocketSrv.Connect() );
	iRepository = CRepository::NewL( KCRUidCmManager );
	ReadCurrentApnL();
	PROFILE_DEBUG1( 
	        "CSIPApnConfigurationHandler::ConstructL() exit" )
	}

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::IsInUseL
// -----------------------------------------------------------------------------
//
TBool CSIPApnConfigurationHandler::IsInUseL( TConnectionInfo& aConnectionInfo )
    {
    PROFILE_DEBUG1( 
            "CSIPApnConfigurationHandler::IsInUseL()" )
    
    TBool inUse( EFalse );
    RConnection rcon;
    User::LeaveIfError( rcon.Open( iSocketSrv ) );
    CleanupClosePushL( rcon );
    
    TUint activeCount( 0 );
    User::LeaveIfError( rcon.EnumerateConnections( activeCount ) );
    
    if ( activeCount > 0  )
        {
        // Indexing is unordinary
        for( TUint i = 1; i <= activeCount && !inUse; i++ )
            {
            TPckgBuf<TConnectionInfoV2> connectionInfo;
            User::LeaveIfError( rcon.GetConnectionInfo( i, connectionInfo ) );
            
            if ( connectionInfo().iIapId == iIapId )
                {
                inUse = ETrue;
                aConnectionInfo = connectionInfo();
                }
            }
        }
        
    CleanupStack::PopAndDestroy( &rcon );
    
    PROFILE_DEBUG3( 
            "CSIPApnConfigurationHandler::IsInUseL() inuse", inUse )
    
    return inUse;
    }

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::StartMonitoringConnectionL
// -----------------------------------------------------------------------------
//
void CSIPApnConfigurationHandler::StartMonitoringConnectionL( 
    TConnectionInfo& aConnectionInfo )
    {
    PROFILE_DEBUG1( 
            "CSIPApnConfigurationHandler::StartMonitoringConnectionL()" )
    
    __ASSERT_ALWAYS( !IsActive(), User::Leave( KErrInUse ) );
    
    if ( iConnection.SubSessionHandle() )
        {
        PROFILE_DEBUG1( 
        "CSIPApnConfigurationHandler::   close existing connection" )
        
        iConnection.Close();
        }
     
    iConnectionInfo = aConnectionInfo;   
    
    User::LeaveIfError( iConnection.Open( iSocketSrv ) );    
    
    PROFILE_DEBUG1( 
            "CSIPApnConfigurationHandler::   attaching" )
    
    User::LeaveIfError( 
        iConnection.Attach( iConnectionInfo, RConnection::EAttachTypeMonitor ) );
    
    WatchConnectionStatusChange();
    
    PROFILE_DEBUG1( 
            "CSIPApnConfigurationHandler::StartMonitoringConnectionL() exit" )
    }

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::WatchConnectionStatusChange
// -----------------------------------------------------------------------------
//	
void CSIPApnConfigurationHandler::WatchConnectionStatusChange()
    {
    PROFILE_DEBUG1( 
            "CSIPApnConfigurationHandler::WatchConnectionStatusChange()" )
    
    Cancel();
    
    iConnection.ProgressNotification( iProgress, iStatus, KConnectionClosed );
    SetActive();
        
    SetMonitoringState( EMonitoringConnection );
    
    PROFILE_DEBUG1( 
            "CSIPApnConfigurationHandler::WatchConnectionStatusChange(), exit" )
    }
    
// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::WatchDatabaseStatusChangeL
// -----------------------------------------------------------------------------
//	
void CSIPApnConfigurationHandler::WatchDatabaseStatusChangeL()
    {
    PROFILE_DEBUG1( 
            "CSIPApnConfigurationHandler::WatchDatabaseStatusChangeL()" )
    
    Cancel();
    
    if ( !iCommsDatabase )
        {
        PROFILE_DEBUG1( 
                "CSIPApnConfigurationHandler::WatchDatabaseStatusChangeL create commsdb" )
        iCommsDatabase = CCommsDatabase::NewL();
        }
    
    PROFILE_DEBUG1( 
            "CSIPApnConfigurationHandler::WatchDatabaseStatusChangeL request notification" )
    
    // Start monitoring for db events, there will be lots of them pouring in
    // as there's no filtering feature. We are interested only in
    // unlocked events.    

    if(iDBMonitoringRetryCount > KDBMaxRetryCount)
        {
        PROFILE_DEBUG1("CSIPApnConfigurationHandler::WatchDatabaseStatusChangeL max retries reached!" )       
        User::Leave( KErrAbort );
        }
    
    User::LeaveIfError( iCommsDatabase->RequestNotification( iStatus ) );
    SetActive();
          
    SetMonitoringState( EMonitoringDatabase );
    
    PROFILE_DEBUG1( 
            "CSIPApnConfigurationHandler::WatchDatabaseStatusChangeL(), exit" )
    }
        
// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::ApnChangeNeededL
// -----------------------------------------------------------------------------
//
TBool CSIPApnConfigurationHandler::ApnChangeNeededL( const TDesC8& aApn )
    {
    PROFILE_DEBUG1( 
            "CSIPApnConfigurationHandler::ApnChangeNeededL()" )

 	TBool apnChangeNeeded( EFalse );
	if ( iCurrentApn && iCurrentApn->Compare( aApn ) != 0 )
        {
        // Apn is not the same as wanted
        apnChangeNeeded = ETrue;
        }

	PROFILE_DEBUG3( 
	        "CSIPApnConfigurationHandler::ApnChangeNeededL(), apnChangeNeeded",
	        apnChangeNeeded )
    return apnChangeNeeded;
    }

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::ChangeApnIfNotInUseL
// -----------------------------------------------------------------------------
//
TBool CSIPApnConfigurationHandler::ChangeApnIfNotInUseL( TBool aAllowAsync )
    {
    PROFILE_DEBUG1( 
            "CSIPApnConfigurationHandler::ChangeApnIfNotInUseL()" )
    
    TBool apnChanged( EFalse );
    
    TConnectionInfo connectionInfo;
    if ( IsInUseL( connectionInfo ) )
        {
        // If iap is in use, apn cannot be changed until everyone has stopped
        // using it
        __ASSERT_ALWAYS( aAllowAsync, User::Leave( KErrInUse ) );
        
        StartMonitoringConnectionL( connectionInfo );
        }
    else
        {
        apnChanged = IssueApnChangeL(
                iIapId, *iApnProposal, iApnUseSecureAuthProposal, aAllowAsync );
        }
        
    PROFILE_DEBUG3( 
            "CSIPApnConfigurationHandler::ChangeApnIfNotInUseL(), apnChanged",
            apnChanged )
    return apnChanged;
    }
    
// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::IssueApnChangeL
// -----------------------------------------------------------------------------
//
TBool CSIPApnConfigurationHandler::IssueApnChangeL( 
    TUint32 aIapId, 
    const TDesC8& aApn,
    TBool aUseSecureAuthentication,
    TBool aAllowAsync )
    {
    PROFILE_DEBUG1( 
            "CSIPApnConfigurationHandler::IssueApnChangeL()" )
    
    TBool apnChanged( EFalse );
    if(aIapId == iIapId)
        {
        TRAPD( err, ChangeApnL( aApn, aUseSecureAuthentication ) );
        if ( err == KErrLocked || err == KErrAccessDenied )
            {
            // Database transaction lock may cause errors if some other client is
            // accessing the same record at the same time. In such case, start
            // monitoring for database events and retry apn changing at each
            // unlock/rollback event.
        
            __ASSERT_ALWAYS( aAllowAsync, User::Leave( KErrInUse ) );
        
            WatchDatabaseStatusChangeL();
            }
        else 
            {
            User::LeaveIfError( err );
            apnChanged = ETrue;
            }
        }
       
    PROFILE_DEBUG3( 
            "CSIPApnConfigurationHandler::IssueApnChangeL(), apnChanged",
            apnChanged )
    return apnChanged;
    }
    
// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::ChangeApnL
// -----------------------------------------------------------------------------
//
void CSIPApnConfigurationHandler::ChangeApnL( 
    const TDesC8& aApn,
    TBool aUseSecureAuthentication )
    {
    PROFILE_DEBUG1( 
            "CSIPApnConfigurationHandler::ChangeApnL()" )
    
   	using namespace CommsDat;
	
	CMDBSession* db = CMDBSession::NewL( KCDVersion1_1 );
    CleanupStack::PushL( db );
    
	db->OpenTransactionL();
    CleanupStack::PushL(TCleanupItem(RollBackDBTransaction, db));
    
    // Set attributes so that also protected iaps can be accessed
    db->SetAttributeMask( ECDHidden | ECDProtectedWrite ); 

    // Create an iap record
    CCDIAPRecord* iapRecord = 
        static_cast<CCDIAPRecord*>( 
            CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
    CleanupStack::PushL( iapRecord );
    
    iapRecord->SetRecordId( iIapId );
    
    TBool clearedProtectedIap = ClearProtectedRecord( *iapRecord );

    iapRecord->LoadL( *db );
    
    iapRecord->iService.LoadL( *db );

    if ( !iapRecord->iService.iLinkedRecord )
        {
        // Ownership of created record is transferred
        iapRecord->iService.iLinkedRecord = 
            static_cast<CCDOutgoingGprsRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdOutgoingGprsRecord ) );

        iapRecord->iService.iLinkedRecord->SetRecordId( iapRecord->iService );        
        iapRecord->iService.iLinkedRecord->LoadL( *db );
        }
        
    PROFILE_DEBUG1( 
    	"CSIPApnConfigurationHandler::ChangeApnL linked service loaded" )

    CCDOutgoingGprsRecord* serviceRecord = 
    static_cast<CCDOutgoingGprsRecord*>( iapRecord->iService.iLinkedRecord );  
    
    TBool clearedProtectedService = ClearProtectedRecord( *serviceRecord );
        
    PROFILE_DEBUG6( 
            "CSIPApnConfigurationHandler::ChangeApnL() curr apn",
            serviceRecord->iGPRSAPN.GetL() );

    HBufC* apn = HBufC::NewLC( aApn.Length() );
    apn->Des().Copy( aApn );
    serviceRecord->iGPRSAPN.SetL( *apn );
    CleanupStack::PopAndDestroy( apn );
    
    PROFILE_DEBUG3( 
            "CSIPApnConfigurationHandler::ChangeApnL() curr security",
            serviceRecord->iGPRSDisablePlainTextAuth );
    
    // Set CHAP/PAP (CHAP is enabled if plain text auth is disabled) 
    serviceRecord->iGPRSDisablePlainTextAuth = aUseSecureAuthentication;

    PROFILE_DEBUG3( 
            "CSIPApnConfigurationHandler::ChangeApnL() new security",
            aUseSecureAuthentication )
	                
    serviceRecord->ModifyL( *db ); 
    
    if ( clearedProtectedIap )
        {
        // Set protection back
        iapRecord->SetAttributes( ECDProtectedWrite );
        iapRecord->ModifyL( *db );
        }
    
    if ( clearedProtectedService )
        {
        // Set protection back
        serviceRecord->SetAttributes( ECDProtectedWrite );
        serviceRecord->ModifyL( *db );
        }
        
    db->ClearAttributeMask( ECDHidden | ECDProtectedWrite );
    
    CleanupStack::PopAndDestroy( iapRecord );
    
    db->CommitTransactionL();
    
	CleanupStack::Pop(); //cleanup item
	
    CleanupStack::PopAndDestroy( db );
    
    if (iCellularDataBlocked)
        {
        AllowCellularDataUsage();
        }  
    
    SendApnChangedNotificationL( aApn );        
    
    PROFILE_DEBUG1( 
            "CSIPApnConfigurationHandler::ChangeApnL(), exit" )
    }

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::ClearProtectedRecord
// -----------------------------------------------------------------------------
//
TBool CSIPApnConfigurationHandler::ClearProtectedRecord( 
    CommsDat::CCDRecordBase& aRecord )
    {
    TBool cleared( EFalse );
    if ( aRecord.IsSetAttribute( CommsDat::ECDProtectedWrite ) )
        {
        aRecord.ClearAttributes( CommsDat::ECDProtectedWrite );
        cleared = ETrue;
        }
    return cleared;
    }

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::SetMonitoringState
// -----------------------------------------------------------------------------
//
void CSIPApnConfigurationHandler::SetMonitoringState( 
    TSipApnMonitoringState aMonitoringState )
    {
   	PROFILE_DEBUG3( 
   	        "CSIPApnConfigurationHandler::SetMonitoringState, state",
   	        aMonitoringState );
    iMonitoringState = aMonitoringState;
    }

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::MonitoringState
// -----------------------------------------------------------------------------
//
CSIPApnConfigurationHandler::TSipApnMonitoringState 
    CSIPApnConfigurationHandler::MonitoringState() const
    {
    return iMonitoringState;
    }
    
// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::ConnectionMonitoringCompletedL
// -----------------------------------------------------------------------------
//
void CSIPApnConfigurationHandler::ConnectionMonitoringCompletedL( TInt aError )
    {
	PROFILE_DEBUG3( 
	        "CSIPApnConfigurationHandler::ConnectionMonitoringCompletedL   progress.err",
	        iProgress().iError );
	PROFILE_DEBUG3( 
	        "CSIPApnConfigurationHandler::ConnectionMonitoringCompletedL   progress.stage",
	        iProgress().iStage );
	                
    if ( !aError )
        {
    	if ( iProgress().iStage == KConnectionClosed ||
    	     iProgress().iStage == KLinkLayerClosed ||
    	     iProgress().iStage == KConnectionFailure )
            {
            // Changing is now possible
            IssueApnChangeL( 
                iConnectionInfo().iIapId, *iApnProposal, iApnUseSecureAuthProposal );
            }
        else if ( !iProgress().iError )
            {
            // Changing not yet possible. Request further notifications
            // from RConnection only if possible
            WatchConnectionStatusChange();
            }
        }
    }

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::DatabaseMonitoringCompletedL
// Only interested about unlock and rollback events. When such occur, it might
// be possible that other client has released transaction lock and we can
// finally modify apn. Note: cancelling notifications may complete with
// KErrCancel, in that case we don't want to reissue notifications.
// -----------------------------------------------------------------------------
//
void CSIPApnConfigurationHandler::DatabaseMonitoringCompletedL( TInt aError )
    {
    TBool apnChanged( EFalse ); 
    if ( aError == KErrCancel )
        {
        return;
        }
        
    if ( aError == RDbNotifier::EUnlock || 
         aError == RDbNotifier::ERollback || 
         aError == RDbNotifier::EClose )
        {
        // Changing may be now possible, if not, db notifications or connection
        // monitoring is re-enabled inside following method
        PROFILE_DEBUG1("DatabaseMonitoringCompletedL::DatabaseMonitoringCompletedL BlockCellularDataUsageL" );
        BlockCellularDataUsageL();
        apnChanged = ChangeApnIfNotInUseL();
        }
    else
        {
        iDBMonitoringRetryCount++;
        WatchDatabaseStatusChangeL();        
        }
    
    if(apnChanged)
        {
        iDBMonitoringRetryCount = 0;
        }
    }

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::SendApnChangedNotificationL
// -----------------------------------------------------------------------------
//
void CSIPApnConfigurationHandler::SendApnChangedNotificationL( 
    const TDesC8& aNewApn, 
    TInt aError )
    {
    HBufC8* currentApn = aNewApn.AllocL();
    delete iCurrentApn;
    iCurrentApn = NULL;
    iCurrentApn = currentApn;
    if ( IsPrimaryApnUsed() )
        {
        iObserver.ApnChanged( *iCurrentApn, iIapId, aError );
        }
    }

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::IsFailed
// -----------------------------------------------------------------------------
//
TBool CSIPApnConfigurationHandler::IsFailed() const
    {
    return iIsFatalFailure;
    }

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::IsFailed
// -----------------------------------------------------------------------------
//
void CSIPApnConfigurationHandler::SetFailed( TBool aIsFailed, TBool aIsFatalFailure )
    {
    PROFILE_DEBUG4( 
                "CSIPApnConfigurationHandler::SetFailed() (failed, fatal)",
                aIsFailed, aIsFatalFailure )
        PROFILE_DEBUG4( 
                "CSIPApnConfigurationHandler::SetFailed() (curr failed, curr fatal)",
                iIsFailed, iIsFatalFailure )
                        
        if ( iIsFailed != aIsFailed )
            {
            TInt err( KErrNone );
            if ( aIsFailed )
                {
                TRAP( err, SetApnL( 
                    SecondaryApn(), EFalse, ETrue) );
                }
            else if ( !iIsFatalFailure )
                {
                TRAP( err, SetApnL( 
                    PrimaryApn(), EFalse, ETrue) );
                }
            else
                {
                PROFILE_DEBUG1( 
                        "CSIPApnConfigurationHandler::SetFailed(), ignored" )
                
                // State change is ignored as state change to "not-failed" was
                // attempted while fatal error had occured earlier
                aIsFailed = iIsFailed;
                aIsFatalFailure = iIsFatalFailure;
                }
                
            if ( err )
                {
                PROFILE_DEBUG3(  
                "CSIPApnConfigurationHandler::SetFailed(), Setting apn failed, err", err )
                }
            }   
        
        iIsFailed = aIsFailed;
        iIsFatalFailure = aIsFatalFailure;
        }

// ----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::PrimaryApn
// ----------------------------------------------------------------------------
//
TDesC8& CSIPApnConfigurationHandler::PrimaryApn()
    {
    return *iPrimaryApn;
    }

// ----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::SecondaryApn
// ----------------------------------------------------------------------------
//
TDesC8& CSIPApnConfigurationHandler::SecondaryApn()
    {
    return *iSecondaryApn;
    }

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::HandlerIapId
// -----------------------------------------------------------------------------
//
TUint32 CSIPApnConfigurationHandler::HandlerIapId() const
    {
    return iIapId;
    }

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::UpdateApnL
// -----------------------------------------------------------------------------
//
void CSIPApnConfigurationHandler::UpdateApnL( TBool aIsPrimaryApn, const TDesC8& aApn )
    {
PROFILE_DEBUG3( 
            "CSIPApnConfigurationHandler::UpdateApnL isPrimary", aIsPrimaryApn )
    PROFILE_DEBUG6( 
            "CSIPApnConfigurationHandler::UpdateApnL apn", aApn )

    HBufC8* newApn = aApn.AllocL();
    if ( aIsPrimaryApn )
        {
        delete iPrimaryApn;
        iPrimaryApn = newApn;
        }
    else
        {
        delete iSecondaryApn;
        iSecondaryApn = newApn;
        }
    }

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::BlockCellularDataUsageL
// -----------------------------------------------------------------------------
//
void CSIPApnConfigurationHandler::BlockCellularDataUsageL()
    {
    PROFILE_DEBUG1("DatabaseMonitoringCompletedL::BlockCellularDataUsageL Enter" );
    //Current Usage Status;
    iRepository->Get( KCurrentCellularDataUsage, iCurrentUsageStatus );
    iRepository->Set( KCurrentCellularDataUsage, ECmCellularDataUsageDisabled );
    iCellularDataBlocked = ETrue;
    PROFILE_DEBUG1("DatabaseMonitoringCompletedL::BlockCellularDataUsageL Exit" );
    }
// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::AllowCellularDataUsage
// -----------------------------------------------------------------------------
//
void CSIPApnConfigurationHandler::AllowCellularDataUsage()
    {
    PROFILE_DEBUG1("DatabaseMonitoringCompletedL::AllowCellularDataUsage Enter" );
    iRepository->Set( KCurrentCellularDataUsage, iCurrentUsageStatus );
    iDBMonitoringRetryCount = 0;
    iCellularDataBlocked = EFalse;
    PROFILE_DEBUG1("DatabaseMonitoringCompletedL::AllowCellularDataUsage Exit" );
    }

// -----------------------------------------------------------------------------
// CSIPApnConfigurationHandler::RollBackDBTransaction
// -----------------------------------------------------------------------------
//
void CSIPApnConfigurationHandler::RollBackDBTransaction(TAny* aDb)
    {
    CMDBSession* db = static_cast<CMDBSession*>(aDb);
    TRAP_IGNORE(db->RollbackTransactionL());
    }

// End of file
