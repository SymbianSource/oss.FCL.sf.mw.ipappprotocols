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
* Description:  Implementation.
*
*/

#include "CWLanBearerMonitor.h"
#include "sipbearerowner.h"
#include "CWLanConnUsagePermissionMonitor.h"
#include "sipwlanlog.h"
#include <wlanerrorcodes.h>

const TInt KCompletionTime = 1;

// -----------------------------------------------------------------------------
// CWLanBearerMonitor::NewL
// -----------------------------------------------------------------------------
//
CWLanBearerMonitor* CWLanBearerMonitor::NewL( TAny* aParams )
	{
	CWLanBearerMonitor* self = new( ELeave ) CWLanBearerMonitor( aParams );
	CleanupClosePushL( *self );
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

// -----------------------------------------------------------------------------
// CWLanBearerMonitor::~CWLanBearerMonitor
// -----------------------------------------------------------------------------
//
CWLanBearerMonitor::~CWLanBearerMonitor()
	{
	if( iAsyncCompletionTimer )
	    {
    	iAsyncCompletionTimer->Remove( iTimerEntry );
	    }
    delete iAsyncCompletionTimer;
    
	delete iUsagePermissionMonitor;
	
	// Cancel notifications and close connection monitor session
	iConnMon.CancelNotifications();
	iConnMon.Close();
	}

// -----------------------------------------------------------------------------
// CWLanBearerMonitor::RefreshL
// -----------------------------------------------------------------------------
//	
void CWLanBearerMonitor::RefreshL( TInt aError )
    {  

    SIPWLAN_LOG("CWLanBearerMonitor::RefreshL, calling SetCurrentAvailabilityL()")
    SetCurrentAvailabilityL();
	if(!aError)
	{
    SIPWLAN_LOG("CWLanBearerMonitor::RefreshL, calling IssueActivityNotification()")
    IssueActivityNotification();
	}
	
    }
// -----------------------------------------------------------------------------
// CWLanBearerMonitor::ContinueMonitoring
// -----------------------------------------------------------------------------
//        
TBool CWLanBearerMonitor::ContinueMonitoring( TInt aError )
    {
    SIPWLAN_INTLOG("CWLanBearerMonitor::ContinueMonitoring", aError)
    return !IapSettingsInvalid( aError );
    }

// -----------------------------------------------------------------------------
// CWLanBearerMonitor::EventL
// -----------------------------------------------------------------------------
//
void CWLanBearerMonitor::EventL( const CConnMonEventBase& aConnMonEvent )
    {
    SIPWLAN_LOG("CWLanBearerMonitor::EventL")
    
	// We're interested only on iap availability notifications
	if( ( aConnMonEvent.EventType() == EConnMonIapAvailabilityChange ) &&
		( aConnMonEvent.ConnectionId() == EBearerIdAll ) )
	    {	    
	    // Typecast event to appropriate event type
		const CConnMonIapAvailabilityChange& event =
			static_cast< const CConnMonIapAvailabilityChange& >( aConnMonEvent );
			
		// Check availability and report to our owner
		MSIPNetworkObserver::TNetworkState newState = 
		    IsAvailable( event.IapAvailability(), iIapId ) ? 
            	    MSIPNetworkObserver::ENetworkActive : 
            	    MSIPNetworkObserver::ENetworkInactive;

        SIPWLAN_INTLOG("CWLanBearerMonitor::EventL: new state", newState)

        // Report only if state of monitored iap changed
	    if ( iState != newState )
	        {
	        iState = newState;
	        iParent.BearerStateChanged( this, KErrNone );
	        }
	    }
	}

// -----------------------------------------------------------------------------
// CWLanBearerMonitor::UsagePermissionChanged
// -----------------------------------------------------------------------------
//	
void CWLanBearerMonitor::UsagePermissionChanged( 
    TBool aPermissionToUse, 
    TInt aError )
    {
    iUsagePermission = aPermissionToUse;
    
    if ( !aError )
        {
        // Don't wait for availability changed event if not necessary since
        // it may take several minutes (even if IAP is already available).
        TRAP( aError, SetCurrentAvailabilityL() );
        }

    IssueActivityNotification( aError );
    }
    
// -----------------------------------------------------------------------------
// CWLanBearerMonitor::CompletionCallback
// -----------------------------------------------------------------------------
//
TInt CWLanBearerMonitor::CompletionCallback( TAny* aAny )
	{
	CWLanBearerMonitor* self = 
	    reinterpret_cast< CWLanBearerMonitor* >( aAny );

    if ( self )
        {
        self->InformActivity();
        }
    
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CWLanBearerMonitor::CWLanBearerMonitor
// -----------------------------------------------------------------------------
//
CWLanBearerMonitor::CWLanBearerMonitor( TAny* aParams )	: 
    CSIPBearerMonitor( aParams ),
	iCallBack( CompletionCallback, this ),
	iTimerEntry( iCallBack ),
	iLastError( KErrNone )
	{
	}

// -----------------------------------------------------------------------------
// CWLanBearerMonitor::ConstructL
// -----------------------------------------------------------------------------
//
void CWLanBearerMonitor::ConstructL()
	{
	// These are really very peculiar functions. They can
	// both leave AND return an error code..
	User::LeaveIfError( iConnMon.ConnectL() );
	User::LeaveIfError( iConnMon.NotifyEventL( *this ) );
	
	iUsagePermissionMonitor = CWLanConnUsagePermissionMonitor::NewL( *this );
	
	iAsyncCompletionTimer = CDeltaTimer::NewL( CActive::EPriorityStandard );
    
    SetCurrentAvailabilityL();
    
    iUsagePermission = iUsagePermissionMonitor->CurrentUsagePermissionL();
            	    
    IssueActivityNotification();
	}

// -----------------------------------------------------------------------------
// CWLanBearerMonitor::SetCurrentAvailabilityL
// -----------------------------------------------------------------------------
//
void CWLanBearerMonitor::SetCurrentAvailabilityL()
    {                         
    // Get current availability of this monitor owner's wlan access point
    // and set current network state according it
    //
    TConnMonIapInfoBuf iapInfo;
	TRequestStatus status;
	iConnMon.GetPckgAttribute( EBearerIdWLAN, 0, KIapAvailability,
							   iapInfo, status );
	User::WaitForRequest( status );
	User::LeaveIfError( status.Int() );
	
	iState = IsAvailable( iapInfo(), iIapId ) ? 
            	    MSIPNetworkObserver::ENetworkActive : 
            	    MSIPNetworkObserver::ENetworkInactive;
    SIPWLAN_INTLOG("CWLanBearerMonitor::SetCurrentAvailabilityL", iState)        	    
    }
    
// -----------------------------------------------------------------------------
// CWLanBearerMonitor::IsAvailable
// -----------------------------------------------------------------------------
//
TBool CWLanBearerMonitor::IsAvailable( 
    const TConnMonIapInfo& aIapInfo, 
    TInt aIapId )
    {
    // Check if aIapId is in the list of available access points
    TInt count = aIapInfo.iCount;
    for ( TInt i = 0; i < count; i++ )
        {
        if ( aIapInfo.iIap[ i ].iIapId == aIapId )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CWLanBearerMonitor::IssueActivityNotification
// -----------------------------------------------------------------------------
//    
void CWLanBearerMonitor::IssueActivityNotification( TInt aError )
    {
    // Notify activity asynchronously
    iAsyncCompletionTimer->Remove( iTimerEntry );
    iAsyncCompletionTimer->Queue( KCompletionTime, iTimerEntry );
    iLastError = aError;
    }

// -----------------------------------------------------------------------------
// CWLanBearerMonitor::InformActivity
// -----------------------------------------------------------------------------
//    
void CWLanBearerMonitor::InformActivity()
    {
    if ( iLastError ||
       ( iState == MSIPNetworkObserver::ENetworkActive && iUsagePermission ) )
        {
        iParent.BearerStateChanged( this, iLastError );
        }
    }

// -----------------------------------------------------------------------------
// CWLanBearerMonitor::IapSettingsInvalid
// -----------------------------------------------------------------------------
//
TBool CWLanBearerMonitor::IapSettingsInvalid( TInt aError ) const
    {
    if ( aError == KErrNotFound || 
         aError == KErrBadName ||
         aError == KErrNotSupported ||
         aError == KErrWlanOpenAuthFailed ||
         aError == KErrWlanSharedKeyAuthRequired ||
         aError == KErrWlanSharedKeyAuthFailed ||
         aError == KErrWlanWpaAuthRequired ||
         aError == KErrWlanIllegalEncryptionKeys ||
         aError == KErrWlanPskModeRequired ||
         aError == KErrWlanEapModeRequired ||
         aError == KErrWlanSimNotInstalled ||
         aError == KErrWlanNotSubscribed ||
         aError == KErrWlanAccessBarred ||
         aError == KErrWlanPasswordExpired ||
         aError == KErrWlanNoDialinPermissions ||
         aError == KErrWlanAccountDisabled ||
         aError == KErrWlanRestrictedLogonHours ||
         aError == KErrWlanServerCertificateExpired ||
         aError == KErrWlanCerficateVerifyFailed ||
         aError == KErrWlanNoUserCertificate ||
         aError == KErrWlanNoCipherSuite ||
         aError == KErrWlanUserRejected ||
         aError == KErrWlanUserCertificateExpired)
        {
	SIPWLAN_INTLOG("CWLanBearerMonitor::IapSettingsInvalid, Fatal", aError)
        return ETrue;
        }
		
	else if(
		aError == KErrWlanWpaAuthFailed ||
		aError == KErrWlan802dot1xAuthFailed ||
		aError == KErrWlanIllegalWpaPskKey ||
		aError == KErrWlanEapSimFailed ||
		aError == KErrWlanEapTlsFailed ||
        aError == KErrWlanEapPeapFailed ||
		aError == KErrWlanEapMsChapv2 ||
        aError == KErrWlanEapAkaFailed ||
        aError == KErrWlanEapTtlsFailed ||
		aError == KErrWlanLeapFailed ||
		aError == KErrWlanEapGtcFailed )
		{
	SIPWLAN_INTLOG("CWLanBearerMonitor::IapSettingsInvalid, Less Fatal", aError)
		return EFalse;
		}
		 
		 
    return EFalse;
    }

// End of File
