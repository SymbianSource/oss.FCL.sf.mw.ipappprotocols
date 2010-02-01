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
// Name        : CLineMonitor.cpp
// Part of     : NetworkMonitor
// Implementation
// Version     : SIP/4.0
//



#include "CLineMonitor.h"
#include "CLineWatcher.h"
#include "sipbearermonitorobserver.h"

// Watcher array granularity. Typically a TSY has max 4 lines
const TInt KWatchersGranularity( 4 );

// -----------------------------------------------------------------------------
// CLineMonitor::NewL
// -----------------------------------------------------------------------------
//
CLineMonitor* CLineMonitor::NewL( MSIPBearerMonitorObserver& aObserver )
	{
	CLineMonitor* self = new( ELeave ) CLineMonitor( aObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CLineMonitor::~CLineMonitor
// -----------------------------------------------------------------------------
//
CLineMonitor::~CLineMonitor()
	{
	iLineWatchers.ResetAndDestroy();
	iPhone.Close();
	iTelServer.Close();
	}

// -----------------------------------------------------------------------------
// CLineMonitor::AllLinesIdle
// -----------------------------------------------------------------------------
//
TBool CLineMonitor::AllLinesIdle() const
	{
	return iAllLinesIdle;
	}

// -----------------------------------------------------------------------------
// CLineMonitor::WatcherStateChanged
// -----------------------------------------------------------------------------
//
void CLineMonitor::WatcherStateChanged( TInt aError )
	{
	// Combine line watcher states
	TBool allLinesIdle( ETrue );
	TInt count = iLineWatchers.Count();
	for( TInt i = 0; i < count; i++ )
		{
		allLinesIdle &= iLineWatchers[ i ]->IsLineIdle();
		}
		
	// Notify observers about state change or any errors
	if( ( allLinesIdle != iAllLinesIdle ) || ( aError != KErrNone ) )
		{
		iAllLinesIdle = allLinesIdle;
		iObserver.MonitorStateChanged( aError );
		}
	}
	
// -----------------------------------------------------------------------------
// CLineMonitor::CLineMonitor
// -----------------------------------------------------------------------------
//
CLineMonitor::CLineMonitor( MSIPBearerMonitorObserver& aObserver )
	: iObserver( aObserver ),
	  iLineWatchers( KWatchersGranularity ),
	  iAllLinesIdle( ETrue )
	{
	}

// -----------------------------------------------------------------------------
// CLineMonitor::ConstructL
// -----------------------------------------------------------------------------
//
void CLineMonitor::ConstructL()
	{
	// Look for phones, report failure if none was found
	TInt phones;
	User::LeaveIfError( iTelServer.Connect() );
	User::LeaveIfError( iTelServer.EnumeratePhones( phones ) );

	if( phones > 0 )
		{
		// Get a phone
		RTelServer::TPhoneInfo phoneinfo;
		User::LeaveIfError( iTelServer.GetPhoneInfo( 0, phoneinfo ) );
		User::LeaveIfError( iPhone.Open( iTelServer, phoneinfo.iName ) );
		
		// Look for lines (we should get something like four lines)
		TInt lineCount;
		User::LeaveIfError( iPhone.EnumerateLines( lineCount ) );
		if( lineCount <= 0 )
			{
			User::Leave( KErrNotSupported );
			}

		// Register an observer for each line
		for( TInt i = 0; i < lineCount; i++ )
			{
			// Get line name by line index
			RPhone::TLineInfo lineInfo;
			User::LeaveIfError( iPhone.GetLineInfo( i,  lineInfo ) );
			
			// Create an observer for the line's status change events
			CLineWatcher* watcher = CLineWatcher::NewLC( *this, iPhone,
														 lineInfo.iName );
			iLineWatchers.AppendL( watcher );
			CleanupStack::Pop( watcher );
			
			// Combine current state
			iAllLinesIdle &= watcher->IsLineIdle();
			}
		}
	else
		{
		User::Leave( KErrNotSupported );
		}
	}
