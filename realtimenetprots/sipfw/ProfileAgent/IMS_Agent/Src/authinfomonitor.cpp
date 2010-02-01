// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : authinfomonitor.cpp
// Part of       : ProfileAgent
// Version       : SIP/5.0
//



#include <etelmm.h>
#include "authinfomonitor.h"
#include "authinfoobserver.h"
#include "SipProfileLog.h"


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CAuthInfoMonitor::NewL
// ----------------------------------------------------------------------------
//
CAuthInfoMonitor* CAuthInfoMonitor::NewL( MAuthInfoObserver& aObserver,
										  RMobilePhone& aPhone )
    {
    return new (ELeave) CAuthInfoMonitor( aObserver, aPhone );
    }

// ----------------------------------------------------------------------------
// CAuthInfoMonitor::CAuthInfoMonitor
// ----------------------------------------------------------------------------
//
CAuthInfoMonitor::CAuthInfoMonitor( MAuthInfoObserver& aObserver,
									RMobilePhone& aPhone ) :
    CActive( CActive::EPriorityStandard ),
    iObserver( aObserver ),
    iPhone( aPhone )    
    {
    CActiveScheduler::Add( this );
    MonitorChanges();
    }

// ----------------------------------------------------------------------------
// CAuthInfoMonitor::~CAuthInfoMonitor
// ----------------------------------------------------------------------------
//
CAuthInfoMonitor::~CAuthInfoMonitor()
    {
    Cancel();
    }

// ----------------------------------------------------------------------------
// CAuthInfoMonitor::MonitorChanges
// ----------------------------------------------------------------------------
//
void CAuthInfoMonitor::MonitorChanges()
    {
	iPhone.NotifyImsAuthorizationInfoChange( iStatus );
	SetActive();
    }

// ----------------------------------------------------------------------------
// CAuthInfoMonitor::DoCancel
// ----------------------------------------------------------------------------
//
void CAuthInfoMonitor::DoCancel()
    {
 	iPhone.CancelAsyncRequest( EMobilePhoneNotifyImsAuthorizationInfoChanged );
    }
      	
// ----------------------------------------------------------------------------
// CAuthInfoMonitor::RunL
// ----------------------------------------------------------------------------
//
void CAuthInfoMonitor::RunL()
    {
    TInt status = iStatus.Int();

    PROFILE_DEBUG3( "CAuthInfoMonitor::RunL status:", status )
                    
	if ( status == KErrNone )
        {
        // Keep monitoring only if no error occured.
        // Currently NotifyImsAuthorizationInfoChange fails always and
        // therefore continuation of monitoring in case of error would
        // lead to forever loop
        MonitorChanges();
        
        iObserver.AuthInfoUpdatedL();
        }
    }

// ----------------------------------------------------------------------------
// CAuthInfoMonitor::RunError
// ----------------------------------------------------------------------------
//
TInt CAuthInfoMonitor::RunError( TInt aError )
    {
  	if ( aError == KErrNoMemory )
  		{
  		return aError;
  		}
  	return KErrNone;
    }

// End of File
