/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  implementation
*
*/

#include <CoreApplicationUIsSDKCRKeys.h>
#include <centralrepository.h>
#include "CWLanConnUsagePermissionMonitor.h"
#include "MWLanConnUsagePermissionObserver.h"


// -----------------------------------------------------------------------------
// CWLanConnUsagePermissionMonitor::NewL
// -----------------------------------------------------------------------------
//
CWLanConnUsagePermissionMonitor* CWLanConnUsagePermissionMonitor::NewL(
	MWLanConnUsagePermissionObserver& aObserver )
    {
    CWLanConnUsagePermissionMonitor* self =
		new ( ELeave ) CWLanConnUsagePermissionMonitor( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CWLanConnUsagePermissionMonitor::~CWLanConnUsagePermissionMonitor
// -----------------------------------------------------------------------------
//
CWLanConnUsagePermissionMonitor::~CWLanConnUsagePermissionMonitor()
    {
    Cancel();
	delete iRepository;
    }

// -----------------------------------------------------------------------------
// CWLanConnUsagePermissionMonitor::RunL
// -----------------------------------------------------------------------------
//  
void CWLanConnUsagePermissionMonitor::RunL()
    {
    TInt status( iStatus.Int() );
    
    CurrentUsagePermissionL(); 
    
    // Strangely cenrep may complete the request with positive value,
    // do not interpret it as an error.
    if ( status >= KErrNone )
        {
        status = KErrNone;
        IssueMonitoringL();
        }
      
    iObserver.UsagePermissionChanged( iCurrentUsagePermission, status );
    }

// -----------------------------------------------------------------------------
// CWLanConnUsagePermissionMonitor::DoCancel
// -----------------------------------------------------------------------------
//  	    
void CWLanConnUsagePermissionMonitor::DoCancel()
    {
    iRepository->NotifyCancelAll();
    }

// -----------------------------------------------------------------------------
// CWLanConnUsagePermissionMonitor::RunError
// -----------------------------------------------------------------------------
//  
TInt CWLanConnUsagePermissionMonitor::RunError( TInt aError )
    {
    iObserver.UsagePermissionChanged( iCurrentUsagePermission, aError );
    if ( aError != KErrNoMemory )
		{
		return KErrNone;
		}
	return aError;
    }
    
// -----------------------------------------------------------------------------
// CWLanConnUsagePermissionMonitor::CurrentUsagePermissionL
// -----------------------------------------------------------------------------
//
TBool CWLanConnUsagePermissionMonitor::CurrentUsagePermissionL()
    {
    TInt value( KErrNotFound );
    User::LeaveIfError( 
        iRepository->Get( KCoreAppUIsNetworkConnectionAllowed, value ) ); 
    
    iCurrentUsagePermission = TranslateConnectionAllowedValue( value ); 
       
    return iCurrentUsagePermission;
    }
    
// -----------------------------------------------------------------------------
// CWLanConnUsagePermissionMonitor::CWLanConnUsagePermissionMonitor
// -----------------------------------------------------------------------------
//
CWLanConnUsagePermissionMonitor::CWLanConnUsagePermissionMonitor(
	MWLanConnUsagePermissionObserver& aObserver) :
	CActive( EPriorityStandard ),
	iObserver( aObserver ),
	iCurrentUsagePermission( EFalse )
	{   
	CActiveScheduler::Add( this );
    }
        
// -----------------------------------------------------------------------------
// CWLanConnUsagePermissionMonitor::ConstructL
// -----------------------------------------------------------------------------
//
void CWLanConnUsagePermissionMonitor::ConstructL()
    {
    iRepository = CRepository::NewL( KCRUidCoreApplicationUIs );

	IssueMonitoringL();
	}

// -----------------------------------------------------------------------------
// CWLanConnUsagePermissionMonitor::IssueMonitoringL
// -----------------------------------------------------------------------------
//
void CWLanConnUsagePermissionMonitor::IssueMonitoringL()
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
// CWLanConnUsagePermissionMonitor::TranslateConnectionAllowedValue
// -----------------------------------------------------------------------------
//
TBool CWLanConnUsagePermissionMonitor::TranslateConnectionAllowedValue( 
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
    
// End of file

