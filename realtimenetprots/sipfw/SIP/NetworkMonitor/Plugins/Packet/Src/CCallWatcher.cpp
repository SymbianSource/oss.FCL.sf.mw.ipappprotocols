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
// Name        : CCallWatcher.cpp
// Part of     : NetworkMonitor
// Implementation
// Version     : SIP/4.0
//



#include "CLineMonitor.h"
#include "CCallWatcher.h"

// -----------------------------------------------------------------------------
// CCallWatcher::NewLC
// -----------------------------------------------------------------------------
//
CCallWatcher* CCallWatcher::NewLC( CLineMonitor& aParent,
                                   RLine& aLine,
                                   TDesC& aCallName  )
	{
	CCallWatcher* self = new( ELeave ) CCallWatcher( aParent );
	CleanupStack::PushL( self );
	self->ConstructL( aLine, aCallName );
	return self;
	}

// -----------------------------------------------------------------------------
// CCallWatcher::~CCallWatcher
// -----------------------------------------------------------------------------
//
CCallWatcher::~CCallWatcher()
	{
	Cancel();
	iCall.Close();
	}

// -----------------------------------------------------------------------------
// CCallWatcher::IsLineIdle
// -----------------------------------------------------------------------------
//
TBool CCallWatcher::IsIdle() const
	{
	return ( iCallStatus == RCall::EStatusIdle );
	}
	
// -----------------------------------------------------------------------------
// CCallWatcher::DoCancel
// -----------------------------------------------------------------------------
//
void CCallWatcher::DoCancel()
	{
	iCall.NotifyStatusChangeCancel();
	}

// -----------------------------------------------------------------------------
// CCallWatcher::RunL
// -----------------------------------------------------------------------------
//
void CCallWatcher::RunL()
	{
	// Renew status notification until the line is idle.
	if( !IsIdle() )
		{
		iCall.NotifyStatusChange( iStatus, iCallStatus );
		SetActive();		
		}
	else
		{	
		iParent.WatcherStateChanged( iStatus.Int() );
		}
	}

// -----------------------------------------------------------------------------
// CCallWatcher::CCallWatcher
// -----------------------------------------------------------------------------
//
CCallWatcher::CCallWatcher( CLineMonitor& aParent )
	: CActive( CActive::EPriorityStandard ),
	  iParent( aParent )
	{
	CActiveScheduler::Add( this );
	}

// -----------------------------------------------------------------------------
// CCallWatcher::ConstructL
// -----------------------------------------------------------------------------
//
void CCallWatcher::ConstructL( RLine& aLine, TDesC& aCallName)
	{
	User::LeaveIfError( iCall.OpenExistingCall( aLine, aCallName ) );	
	
	// Get current call status
	User::LeaveIfError( iCall.GetStatus( iCallStatus ) );
	
	// Request notification from ETel about line status change
	iCall.NotifyStatusChange( iStatus, iCallStatus );
	SetActive();
	}

