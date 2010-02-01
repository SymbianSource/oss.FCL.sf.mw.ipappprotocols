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
// Name          : CSIPSecDigestObserver.cpp
// Part of       : SIPSec
// Version       : SIP/6.0
//



#include "SipLogs.h"
#include "CSIPSecDigestObserver.h"
#include "CSIPSecDigestCacheEntry.h"


// ============================ MEMBER FUNCTIONS ===============================


// ----------------------------------------------------------------------------
// CSIPSecDigestObserver::CSIPSecDigestObserver
// ----------------------------------------------------------------------------
//	
CSIPSecDigestObserver::CSIPSecDigestObserver(
	MSIPSecSecurityMechanismObserver& aObserver ) :
    iPendingOperations( 0 ),
    iAtLeastOneSuccessfulOperation( EFalse ),
    iObserver( &aObserver ),
    iStatus( EWaiting )
	{
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestObserver::CSIPSecDigestObserver
// ----------------------------------------------------------------------------
//	
CSIPSecDigestObserver::~CSIPSecDigestObserver()
    {    
    }
	
// ----------------------------------------------------------------------------
// CSIPSecDigestObserver::OperationStarted
// ----------------------------------------------------------------------------
//
void CSIPSecDigestObserver::OperationStarted()
	{
	++iPendingOperations;
	}
    
// ----------------------------------------------------------------------------
// CSIPSecDigestObserver::OperationCompleted
// ----------------------------------------------------------------------------
//
void CSIPSecDigestObserver::OperationCompleted( TStatus aReason,
												TBool /*aAuthenticated*/ )
	{
	__SIP_INT_LOG1( "SIPSecDigestObs:OperCompl status", aReason )
	__ASSERT_ALWAYS( iPendingOperations > 0,
		User::Panic( _L( "Digest: OpComp no pending ops" ), KErrNotFound ) );

	switch ( aReason )
		{
		case EComplete:
    		iAtLeastOneSuccessfulOperation = ETrue;
    		break;
	
		case ECancelled:
	    	iStatus = aReason;
	    	break;

		case EFailure:
			if ( iStatus != ECancelled )
	    		{
	    		iStatus = aReason;
	    		}
	    	break;

	    default: // EWaiting
	    	User::Panic( _L( "Digest: OpComp waiting" ), KErrArgument );
		}

	if( --iPendingOperations == 0)
		{		
		// Inform observer also if the challenge was ignored (ECancelled)
		DoCompleted();

		iStatus = EComplete;

		// Clear the flag, in case CSIPSecDigestObserver will be re-used.
		iAtLeastOneSuccessfulOperation = EFalse;
		}
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestObserver::IsCompleted
// ----------------------------------------------------------------------------
//
TBool CSIPSecDigestObserver::IsCompleted() const
	{
	return iStatus == EComplete;
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestObserver::DoCompleted
// ----------------------------------------------------------------------------
//
void CSIPSecDigestObserver::DoCompleted()
	{
	__SIP_INT_LOG1( "SIPSecDigestObs:DoCompl, iAtLeastOne", 
	                iAtLeastOneSuccessfulOperation )

	iObserver->CacheUpdated( iAtLeastOneSuccessfulOperation, ETrue );

	__SIP_LOG( "SIPSecDigestObs:DoCompl ends" )
	}
	
// ----------------------------------------------------------------------------
// CSIPSecDigestObserver::HasObserver
// ----------------------------------------------------------------------------
//	
TBool CSIPSecDigestObserver::HasObserver(
			const MSIPSecSecurityMechanismObserver& aObserver ) const
	{
	return &aObserver == iObserver;
	}	
