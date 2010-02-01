/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include <CoreApplicationUIsSDKCRKeys.h>
#include <centralrepository.h>
#include "CSystemStateConnUsagePermissionMonitor.h"
#include "MSystemStateConnUsagePermissionObserver.h"

// -----------------------------------------------------------------------------
// CSystemStateConnUsagePermissionMonitor::NewL
// -----------------------------------------------------------------------------
//
CSystemStateConnUsagePermissionMonitor* 
CSystemStateConnUsagePermissionMonitor::NewL()
    {
    CSystemStateConnUsagePermissionMonitor* self =
		new ( ELeave ) CSystemStateConnUsagePermissionMonitor();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSystemStateConnUsagePermissionMonitor::CSystemStateConnUsagePermissionMonitor
// -----------------------------------------------------------------------------
//
CSystemStateConnUsagePermissionMonitor::CSystemStateConnUsagePermissionMonitor() 
    : CActive( EPriorityStandard ),
	iCurrentUsagePermission( EFalse )
	{   
	CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CSystemStateConnUsagePermissionMonitor::ConstructL
// -----------------------------------------------------------------------------
//
void CSystemStateConnUsagePermissionMonitor::ConstructL()
    {
    iRepository = CRepository::NewL( KCRUidCoreApplicationUIs );
	IssueMonitoringL();
	}

// -----------------------------------------------------------------------------
// CSystemStateConnUsagePermissionMonitor::~CSystemStateConnUsagePermissionMonitor
// -----------------------------------------------------------------------------
//
CSystemStateConnUsagePermissionMonitor::~CSystemStateConnUsagePermissionMonitor()
    {
    iObservers.Close();
    Cancel();
	delete iRepository;
    }

// -----------------------------------------------------------------------------
// CSystemStateConnUsagePermissionMonitor::RunL
// -----------------------------------------------------------------------------
//  
void CSystemStateConnUsagePermissionMonitor::RunL()
    {
    TInt status( iStatus.Int() );
    
    TInt previousPermission = iCurrentUsagePermission;
    
    CurrentUsagePermission(); 
    
    // Strangely cenrep may complete the request with positive value,
    // do not interpret it as an error.
    if ( status >= KErrNone )
        {
        status = KErrNone;
        IssueMonitoringL();
        }
    
    if ( previousPermission != iCurrentUsagePermission )
        {
        for ( TInt i = iObservers.Count()-1; i >= 0; i-- )
            {
            iObservers[i]->UsagePermissionChanged( iCurrentUsagePermission,
                                                   status );
            }
        }
    }

// -----------------------------------------------------------------------------
// CSystemStateConnUsagePermissionMonitor::DoCancel
// -----------------------------------------------------------------------------
//  	    
void CSystemStateConnUsagePermissionMonitor::DoCancel()
    {
    iRepository->NotifyCancelAll();
    }

// -----------------------------------------------------------------------------
// CSystemStateConnUsagePermissionMonitor::RunError
// -----------------------------------------------------------------------------
//  
TInt CSystemStateConnUsagePermissionMonitor::RunError( TInt aError )
    {
    for ( TInt i = iObservers.Count()-1; i >= 0; i-- )
        {
        iObservers[i]->UsagePermissionChanged( iCurrentUsagePermission,
                                               aError );
        }
    if ( aError != KErrNoMemory )
        {
        return KErrNone;
        }
    return aError;
    }
    
// -----------------------------------------------------------------------------
// CSystemStateConnUsagePermissionMonitor::CurrentUsagePermission
// -----------------------------------------------------------------------------
//
TInt CSystemStateConnUsagePermissionMonitor::CurrentUsagePermission()
    {
    TInt value( KErrNotFound );
    TInt err = iRepository->Get( KCoreAppUIsNetworkConnectionAllowed, value );
    if ( err == KErrNone)
        {
        iCurrentUsagePermission = TranslateConnectionAllowedValue( value );
        }
    else
        {
        iCurrentUsagePermission = err;
        }
    return iCurrentUsagePermission;
    }

// -----------------------------------------------------------------------------
// CSystemStateConnUsagePermissionMonitor::IssueMonitoringL
// -----------------------------------------------------------------------------
//
void CSystemStateConnUsagePermissionMonitor::IssueMonitoringL()
    {
    if ( !IsActive() )
        {
        User::LeaveIfError( 
            iRepository->NotifyRequest( KCoreAppUIsNetworkConnectionAllowed, 
                                        iStatus ) );
        SetActive();
        }
    }
    
// -----------------------------------------------------------------------------
// CSystemStateConnUsagePermissionMonitor::TranslateConnectionAllowedValue
// -----------------------------------------------------------------------------
//
TBool CSystemStateConnUsagePermissionMonitor::TranslateConnectionAllowedValue( 
    TInt aValue )
    {
    TBool retVal( EFalse );
    switch ( aValue )
        {
        case ECoreAppUIsNetworkConnectionNotAllowed:
            {
            retVal = EFalse;
            break;
            }
        case ECoreAppUIsNetworkConnectionAllowed:
            {
            retVal = ETrue;
            break;
            }
        default:
            {
            break;
            }
        }
    return retVal;
    }
    
// -----------------------------------------------------------------------------
// CSystemStateConnUsagePermissionMonitor::AddObserverL
// -----------------------------------------------------------------------------
//
void CSystemStateConnUsagePermissionMonitor::AddObserverL( 
    MSystemStateConnUsagePermissionObserver& aObserver )
    {
    iObservers.InsertInAddressOrderL( &aObserver );
    }

// -----------------------------------------------------------------------------
// CSystemStateConnUsagePermissionMonitor::RemoveObserver
// -----------------------------------------------------------------------------
//
void CSystemStateConnUsagePermissionMonitor::RemoveObserver( 
    MSystemStateConnUsagePermissionObserver& aObserver )
    {
    TInt index = iObservers.Find( &aObserver );
    if ( index >= 0 )
        {
        iObservers.Remove( index ); 
        }
    }

// End of file
