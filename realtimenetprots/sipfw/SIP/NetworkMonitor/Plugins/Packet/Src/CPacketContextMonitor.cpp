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
// Name        : CPacketContextMonitor.cpp
// Part of     : NetworkMonitor
// Implementation
// Version     : SIP/4.0
//



#include "CPacketContextMonitor.h"
#include "CPacketContextWatcher.h"
#include "sipbearermonitorobserver.h"

#include <pcktcs.h>
#include <es_sock.h>
#include <es_enum_partner.h>
#include <featmgr.h>

const TInt KContextWatchersGranularity( 2 );

// Retry timer values in millisecs
const TUint32 KContextMonitorRetryAfter3Min =   180000;
const TUint32 KContextMonitorRetryAfter5Min =   300000;
const TUint32 KContextMonitorRetryAfter15Min =  900000;
const TUint32 KContextMonitorRetryAfter1Hour =  3600000;
const TUint32 KContextMonitorRetryAfter4Hours = 14400000;

const TInt KMaxOtherMonitoringRetries = 2;
	
// -----------------------------------------------------------------------------
// CPacketContextMonitor::NewL
// -----------------------------------------------------------------------------
//
CPacketContextMonitor* CPacketContextMonitor::NewL( 
    MSIPBearerMonitorObserver& aObserver,
    TInt aError )
	{
	CPacketContextMonitor* self = 
	        new( ELeave ) CPacketContextMonitor( aObserver );
	CleanupStack::PushL( self );
	self->ConstructL( aError );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CPacketContextMonitor::~CPacketContextMonitor
// -----------------------------------------------------------------------------
//
CPacketContextMonitor::~CPacketContextMonitor()
	{
	iContextWatchers.ResetAndDestroy();
	iSocketSrv.Close();
	if(FeatureManager::FeatureSupported( KFeatureIdFfSipCustomPdpHandling ))
		{
		StopRetryTimer();
		delete iRetryTimer;
		}
	}

// -----------------------------------------------------------------------------
// CPacketContextMonitor::RefreshL
// -----------------------------------------------------------------------------
//	    
void CPacketContextMonitor::RefreshL( TInt aError )
    {
	if(FeatureManager::FeatureSupported( KFeatureIdFfSipCustomPdpHandling ))
		{

		if ( aError == KErrNone )
        	{
        	Reset();
        	}
        
    	StartMonitoringL( aError );
		}
		else
		{

    	// If watchers exist already/still, ignore this call.
    	// Normally contexts watchers have been deleted before context was informed
    	// to be available. If informed observer gets context in use and then later
    	// looses it, old wathcers would not necessarily watch correct contexts and
    	// therefore RefreshL call has to create new valid watchers.
    	if( iContextWatchers.Count() > 0 )
        	{
        	return;
        	}
        
    	CreateLineWatchersL( aError );
		}
    }
	
// -----------------------------------------------------------------------------
// CPacketContextMonitor::ContinueMonitoring
// -----------------------------------------------------------------------------
//
TBool CPacketContextMonitor::ContinueMonitoring( TInt aError )
    {
    return ( IsTemporaryErrorRequiringRetryTimer( aError ) ||  
             IsTemporaryErrorRequiringContextMonitoring( aError ) ||
             IsTemporaryErrorRequiringOtherMonitoring( aError ) );
    }
    
// -----------------------------------------------------------------------------
// CPacketContextMonitor::IsTemporaryErrorRequiringRetryTimer
// -----------------------------------------------------------------------------
//  
TBool CPacketContextMonitor::IsTemporaryErrorRequiringRetryTimer( TInt aError )
    {
    return ( aError == KErrGprsLlcOrSndcpFailure ||
             aError == KErrGprsInsufficientResources ||
             aError == KErrGprsServiceOptionTemporarilyOutOfOrder ||
             aError == KErrGprsNSAPIAlreadyUsed ||
             aError == KErrGprsRegularContextDeactivation ||
             aError == KErrGsmMMNetworkFailure ||
             aError == KErrGprsReactivationRequested ||
             aError == KErrGsmMMUnspecifiedProtocolError );
	}
	
// -----------------------------------------------------------------------------
// CPacketContextMonitor::IsPDPCtxError
// -----------------------------------------------------------------------------
//     
TBool CPacketContextMonitor::IsPDPCtxError( TInt aError )
    {
    return ( aError == KErrGprsActivationRejectedByGGSN ||
             aError == KErrGprsActivationRejected ||
             aError == KErrGprsServiceOptionNotSupported ||
             aError == KErrGprsServiceOptionTemporarilyOutOfOrder ||
             aError == KErrGprsRequestedServiceOptionNotSubscribed ||
             aError == KErrGprsPDPContextWithoutTFTAlreadyActivated ||
             aError == KErrUmtsMaxNumOfContextExceededByPhone ||
             aError == KErrUmtsMaxNumOfContextExceededByNetwork ||
             aError == KErrGprsServicesNotAllowed ||
             aError == KErrGprsAndNonGprsServicesNotAllowed ||
             aError == KErrGprsMSCTemporarilyNotReachable ||
             aError == KErrGprsOfflineMode );
    }
    
// -----------------------------------------------------------------------------
// CPacketContextMonitor::IsTemporaryErrorRequiringContextMonitoring
// -----------------------------------------------------------------------------
//     
TBool CPacketContextMonitor::IsTemporaryErrorRequiringContextMonitoring( 
    TInt aError )
    {
    return ( aError == KErrUmtsMaxNumOfContextExceededByPhone ||
             aError == KErrUmtsMaxNumOfContextExceededByNetwork );
    }

// -----------------------------------------------------------------------------
// CPacketContextMonitor::IsTemporaryErrorRequiringOtherMonitoring
// -----------------------------------------------------------------------------
//     
TBool CPacketContextMonitor::IsTemporaryErrorRequiringOtherMonitoring( 
    TInt aError )
    {
    TBool requireMonitoring( EFalse );
    if ( aError == KErrGprsActivationRejected ||
         aError == KErrGprsOfflineMode )
        {
        requireMonitoring = 
            ( iOtherMonitoringRetryCount < KMaxOtherMonitoringRetries );
        }
    return requireMonitoring;
    }

// -----------------------------------------------------------------------------
// CPacketContextMonitor::WatcherStateChanged
// -----------------------------------------------------------------------------
//
void CPacketContextMonitor::WatcherStateChanged( TInt aError )
	{
	TBool available( EFalse );
	for( TInt i = 0; !available && i < iContextWatchers.Count(); i++ )
	    {
	    if( iContextWatchers[ i ]->IsContextAvailable() )
	        {
	        available = ETrue;
	        }
	    }
	    
	iIsAvailable = available;
	
	// If context is available, current watchers are not needed anymore
	// New watchers may be created later by RefreshL call.
	// Report only if some pdp ctx became available or error occured.
	if( iIsAvailable || aError )
	    {
	    iContextWatchers.ResetAndDestroy();
	    iObserver.MonitorStateChanged( aError );
	    }
	}
	
// -----------------------------------------------------------------------------
// CPacketContextMonitor::CPacketContextMonitor
// -----------------------------------------------------------------------------
//
CPacketContextMonitor::CPacketContextMonitor( 
    MSIPBearerMonitorObserver& aObserver )
	: iObserver( aObserver ),
	  iContextWatchers( KContextWatchersGranularity ),
	  iIsAvailable( ETrue ),
	  iRetryTimerCallback( RetryTimerExpired, this ),
	  iRetryTimerEntry( iRetryTimerCallback ),
	  iRetryAfter( KContextMonitorRetryAfter3Min )
	{
	}

// -----------------------------------------------------------------------------
// CPacketContextMonitor::ConstructL
// -----------------------------------------------------------------------------
//
void CPacketContextMonitor::ConstructL( TInt aError )
	{
    User::LeaveIfError( iSocketSrv.Connect() );
	if(FeatureManager::FeatureSupported( KFeatureIdFfSipCustomPdpHandling ))
		{    
    	iRetryTimer = CDeltaTimer::NewL( CActive::EPriorityStandard );  
    	StartMonitoringL( aError );
    	}
	else
		{
    	CreateLineWatchersL( aError );
		}
	}

// -----------------------------------------------------------------------------
// CPacketContextMonitor::StartMonitoringL
// Depending on error, may start retry timer for automatic retry after certain
// period or create line watchers for active pdp contexts.
// -----------------------------------------------------------------------------
//
void CPacketContextMonitor::StartMonitoringL( TInt aError )
    {    
    if ( aError == KErrNone )
        {
        return;
        }
    else if ( IsTemporaryErrorRequiringRetryTimer( aError ) )
        {
        StartRetryTimer();
        }
    else if ( IsTemporaryErrorRequiringContextMonitoring( aError ) )
        {
        CreateLineWatchersL( aError );
        }
    else if ( IsTemporaryErrorRequiringOtherMonitoring( aError ) )
        {
        OtherMonitoring();
        }
    else 
        {
        // Some fatal error, cannot continue
        User::Leave( aError );
        }
    }
	
// -----------------------------------------------------------------------------
// CPacketContextMonitor::CreateLineWatchersL
// -----------------------------------------------------------------------------
//   
void CPacketContextMonitor::CreateLineWatchersL( TInt aError )
    { 
	
	if(FeatureManager::FeatureSupported( KFeatureIdFfSipCustomPdpHandling ))
		{
		StopRetryTimer();
    
    	// If watchers exist already/still, ignore this call.
    	// Normally contexts watchers have been deleted before context was informed
    	// to be available. If informed observer gets context in use and then later
    	// looses it, old watchers would not necessarily watch correct contexts and
    	// therefore RefreshL call has to create new valid watchers.
    	if( iContextWatchers.Count() > 0 )
        	{
        	return;
        	}
		}
	else
		{
    	if( !IsPDPCtxError( aError ) )
        	{
        	return;
        	}
		}
		
    RConnection rcon;
    User::LeaveIfError( rcon.Open( iSocketSrv ) );
    CleanupClosePushL( rcon );
    
    TUint activeCount( 0 );
    User::LeaveIfError( rcon.EnumerateConnections( activeCount ) );
    
    if( activeCount > 0  )
        {
        // Context is not available, create watchers for every active context
        iIsAvailable = EFalse;
        
        for( TUint i = 1; i <= activeCount; i++ )
            {
            TPckgBuf<TConnectionInfoV2> connectionInfo;
            User::LeaveIfError( rcon.GetConnectionInfo( i, connectionInfo ) );
            
            TInt connectionType = connectionInfo().ConnectionType();
            
            // Don't start to watch any WLAN connections etc.
            if( connectionType == EConnectionGeneric || 
              ( connectionType >= EConnectionGPRS && 
                connectionType < EConnectionEthernet ) )
                {
                // TConnectionInfoV2 will be downcasted to TConnectionInfo 
                // since RConnection::Attach will leave if watched connection 
                // is not "compatible" with TConnectionInfoV2
                CPacketContextWatcher* watcher = 
                    CPacketContextWatcher::NewLC( *this, 
                                                  iSocketSrv,
                                                  connectionInfo() );
                User::LeaveIfError( iContextWatchers.Append( watcher ) );
                CleanupStack::Pop( watcher );
                }
            }
        }
    else
        {
        // Error was PDP ctx related but no active connections could
        // be found and therefore monitoring is not possible or needed.
        // Report availability immediately to observer
        iIsAvailable = ETrue;
        iObserver.MonitorStateChanged( KErrNone );
        }
    
    CleanupStack::PopAndDestroy(); //rcon
    }

// -----------------------------------------------------------------------------
// CPacketContextMonitor::Reset
// -----------------------------------------------------------------------------
// 
void CPacketContextMonitor::Reset()
    {   
    iContextWatchers.ResetAndDestroy();
    StopRetryTimer();
    iRetryAfter = KContextMonitorRetryAfter3Min;
    iIsAvailable = EFalse;
    iOtherMonitoringRetryCount = 0;
    }
    
// -----------------------------------------------------------------------------
// CPacketContextMonitor::StartRetryTimer
// -----------------------------------------------------------------------------
// 
void CPacketContextMonitor::StartRetryTimer()
    {
    iContextWatchers.ResetAndDestroy();
    
    StopRetryTimer();
    
    // Have to use 64 bit val as 4 hours in microsecs wouldn't fit to 32 bit val
    const TInt64 KMillisecsToMicrosec = 1000;
    TTimeIntervalMicroSeconds 
        retryAfterInMicroSecs( iRetryAfter * KMillisecsToMicrosec );
    
    iRetryTimer->QueueLong( retryAfterInMicroSecs, iRetryTimerEntry );    
    
    // Update value used for retry at next round    
    switch ( iRetryAfter )
        {
        case KContextMonitorRetryAfter3Min:
            {
            iRetryAfter = KContextMonitorRetryAfter5Min;
            break;
            }
        case KContextMonitorRetryAfter5Min:
            {
            iRetryAfter = KContextMonitorRetryAfter15Min;
            break;
            }
        case KContextMonitorRetryAfter15Min:
            {
            iRetryAfter = KContextMonitorRetryAfter1Hour;
            break;
            }
        case KContextMonitorRetryAfter1Hour:
            {
            iRetryAfter = KContextMonitorRetryAfter4Hours;
            break;
            }
        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CPacketContextMonitor::OtherMonitoring
// -----------------------------------------------------------------------------
//
void CPacketContextMonitor::OtherMonitoring()
    {
    iContextWatchers.ResetAndDestroy();
    
    StopRetryTimer();
    
    iOtherMonitoringRetryCount++;    
    }
    
// -----------------------------------------------------------------------------
// CPacketContextMonitor::StopRetryTimer
// -----------------------------------------------------------------------------
//     
void CPacketContextMonitor::StopRetryTimer()
    {
    if ( iRetryTimer )
        {
        iRetryTimer->Remove( iRetryTimerEntry );
        }
    }

// -----------------------------------------------------------------------------
// CPacketContextMonitor::RetryTimerExpired
// -----------------------------------------------------------------------------
// 
TInt CPacketContextMonitor::RetryTimerExpired( TAny* aAny )
    {
    CPacketContextMonitor* self = reinterpret_cast<CPacketContextMonitor*>( aAny );
            
    if ( self != NULL )
        {
        self->iObserver.MonitorStateChanged( KErrNone );
        }
    return ETrue;
    }
