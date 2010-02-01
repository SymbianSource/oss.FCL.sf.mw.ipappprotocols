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
// Name        : CLineWatcher.cpp
// Part of     : NetworkMonitor
// Implementation
// Version     : SIP/4.0
//



#include "CLineMonitor.h"
#include "CLineWatcher.h"
#include "CCallWatcher.h"

// -----------------------------------------------------------------------------
// CLineWatcher::NewLC
// -----------------------------------------------------------------------------
//
CLineWatcher* CLineWatcher::NewLC( CLineMonitor& aParent,
								   RPhone& aPhone, TDesC& aLineName )
	{
	CLineWatcher* self = new( ELeave ) CLineWatcher( aParent );
	CleanupStack::PushL( self );
	self->ConstructL( aPhone, aLineName );
	return self;
	}

// -----------------------------------------------------------------------------
// CLineWatcher::~CLineWatcher
// -----------------------------------------------------------------------------
//
CLineWatcher::~CLineWatcher()
	{
	iCallWatchers.ResetAndDestroy();
	iLine.Close();
	}

// -----------------------------------------------------------------------------
// CLineWatcher::IsLineIdle
// -----------------------------------------------------------------------------
//
TBool CLineWatcher::IsLineIdle() const
	{
	TBool allCallsIdle( ETrue );
	TInt count = iCallWatchers.Count();
	for( TInt i = 0; i < count; i++ )
		{
		allCallsIdle &= iCallWatchers[ i ]->IsIdle();
		}
	return allCallsIdle;
	}

// -----------------------------------------------------------------------------
// CLineWatcher::CLineWatcher
// -----------------------------------------------------------------------------
//
CLineWatcher::CLineWatcher( CLineMonitor& aParent )
	: iParent( aParent )
	{
	}

// -----------------------------------------------------------------------------
// CLineWatcher::ConstructL
// -----------------------------------------------------------------------------
//
void CLineWatcher::ConstructL( RPhone& aPhone, TDesC& aLineName )
	{
	User::LeaveIfError( iLine.Open( aPhone, aLineName ) );
	TInt calls;
	User::LeaveIfError( iLine.EnumerateCall( calls ) );			
	for( TInt i=0; i < calls; i++ ) 
		{
		RCall call;
		RCall::TStatus callStatus;
		RLine::TCallInfo callInfo;
		User::LeaveIfError( iLine.GetCallInfo( i, callInfo ) );
		User::LeaveIfError( call.OpenExistingCall( iLine, 
		                                           callInfo.iCallName ) );
		CleanupClosePushL( call );
		User::LeaveIfError( call.GetStatus( callStatus ) );			
		if (callStatus != RCall::EStatusIdle)
			{
			CCallWatcher* callWatcher = 
				CCallWatcher::NewLC( iParent, iLine, callInfo.iCallName );
			User::LeaveIfError( iCallWatchers.Append( callWatcher ) );
			CleanupStack::Pop( callWatcher );
			}
		CleanupStack::PopAndDestroy(); // call
		}
	}

