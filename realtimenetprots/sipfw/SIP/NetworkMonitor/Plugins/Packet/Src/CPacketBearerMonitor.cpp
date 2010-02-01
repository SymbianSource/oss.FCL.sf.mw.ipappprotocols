// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : CPacketBearerMonitor.cpp
// Part of     : NetworkMonitor
// Implementation.
// Version     : SIP/4.0
//


#include <featmgr.h>
#include "CLineMonitor.h"
#include "CPacketBearerMonitor.h"
#include "CPacketServiceMonitor.h"
#include "CRegistrationMonitor.h"
#include "CPacketContextMonitor.h"
#include "CAccessNetworkMonitor.h"
#include "sipbearerowner.h"
#include "sipnetworkinfoobserver.h"

// -----------------------------------------------------------------------------
// CPacketBearerMonitor::NewL
// -----------------------------------------------------------------------------
//
CPacketBearerMonitor* CPacketBearerMonitor::NewL( TAny* aParams )
	{
	CPacketBearerMonitor* self = new( ELeave ) CPacketBearerMonitor( aParams );

	CleanupClosePushL( *self );
	self->ConstructL();
	CleanupStack::Pop(); // self

	return self;
	}

// -----------------------------------------------------------------------------
// CPacketBearerMonitor::CPacketBearerMonitor
// -----------------------------------------------------------------------------
//
CPacketBearerMonitor::CPacketBearerMonitor( TAny* aParams ) :
	CSIPBearerMonitor( aParams )
	{
	}

// -----------------------------------------------------------------------------
// CPacketBearerMonitor::ConstructL
// -----------------------------------------------------------------------------
//
void CPacketBearerMonitor::ConstructL()
	{
	iLineMonitor = CLineMonitor::NewL( *this );
	iPacketServiceMonitor = CPacketServiceMonitor::NewL( *this );
	iRegistrationMonitor = CRegistrationMonitor::NewL( *this );
	iContextMonitor = CPacketContextMonitor::NewL( *this, iError );
	iAccessNetworkMonitor = CAccessNetworkMonitor::NewL( *this );
	}

// -----------------------------------------------------------------------------
// CPacketBearerMonitor::~CPacketBearerMonitor
// -----------------------------------------------------------------------------
//
CPacketBearerMonitor::~CPacketBearerMonitor()
	{
	delete iLineMonitor;
	delete iPacketServiceMonitor;
	delete iRegistrationMonitor;
	delete iContextMonitor;
	delete iAccessNetworkMonitor;
	}

// -----------------------------------------------------------------------------
// CPacketBearerMonitor::MonitorStateChanged
// -----------------------------------------------------------------------------
//
void CPacketBearerMonitor::MonitorStateChanged( TInt aError )
	{
	// Deactivate / activate Line Monitoring based on device MS class.
	// We don't watch for Lines on dual mode or if class information is
	// not yet available.
	if( iPacketServiceMonitor->IsLineMonitoringNeeded() )
		{
		CreateLineMonitor();
		}
	else
		{
		RemoveLineMonitor();
		}		
		
	iState = CombineStates();
		
	if ( iState == MSIPNetworkObserver::ENetworkActive )
		{
		RemoveLineMonitor();
		}
	else
		{
		CreateLineMonitor();	
		}	
	
	// If no error has occured, check whether current MS class is supported.
	// Unsupported class is reported as a error to the parent.	
	if ( aError == KErrNone &&
	     !iPacketServiceMonitor->IsSupportedMSClass() )
	    {
	    aError = KErrNotSupported;
	    }

	iParent.BearerStateChanged( this, aError );
	}

// -----------------------------------------------------------------------------
// CPacketBearerMonitor::RefreshL
// -----------------------------------------------------------------------------
//	
void CPacketBearerMonitor::RefreshL( TInt aError )
    {
    iError = aError;
    iContextMonitor->RefreshL( iError );
    }

// -----------------------------------------------------------------------------
// CPacketBearerMonitor::ContinueMonitoring
// -----------------------------------------------------------------------------
//	    
TBool CPacketBearerMonitor::ContinueMonitoring( TInt aError )
    {
    // Check if aError is PDP ctx related
    // If so, suggest continuation of monitoring, also KErrNone leads to 
    // positive decision
    if(FeatureManager::FeatureSupported( KFeatureIdFfSipCustomPdpHandling ))
		{
    	return (aError == KErrNone || iContextMonitor->ContinueMonitoring(aError));
		}
    else
		{
    	return ( aError == KErrNone || iContextMonitor->IsPDPCtxError( aError ) );
    	}
    }

// -----------------------------------------------------------------------------
// CPacketBearerMonitor::CreateLineMonitor
// -----------------------------------------------------------------------------
//	
void CPacketBearerMonitor::CreateLineMonitor()
	{
	if( iPacketServiceMonitor->IsLineMonitoringNeeded() && !iLineMonitor )
		{
		TRAP_IGNORE( ( iLineMonitor = CLineMonitor::NewL( *this ) ) )		
		}	
	}

// -----------------------------------------------------------------------------
// CPacketBearerMonitor::RemoveLineMonitor
// -----------------------------------------------------------------------------
//		
void CPacketBearerMonitor::RemoveLineMonitor()
	{
	if( iLineMonitor )
		{
		delete iLineMonitor;
		iLineMonitor = NULL;
		}	
	}

// -----------------------------------------------------------------------------
// CPacketBearerMonitor::CombineStates()
// -----------------------------------------------------------------------------
//	
MSIPNetworkObserver::TNetworkState CPacketBearerMonitor::CombineStates()
	{
	// Determine our state by combining states from monitors	
	MSIPNetworkObserver::TNetworkState state = 
	    MSIPNetworkObserver::ENetworkUnknown;

	if( iPacketServiceMonitor->IsSuspended() )
		{
		if( iRegistrationMonitor->IsRegistered() )
			{
			state = MSIPNetworkObserver::ENetworkSuspended;
			}
		else
			{
			state = MSIPNetworkObserver::ENetworkInactive;
			}
		}
	else
		{
		if( AllLinesIdle() &&
		    iRegistrationMonitor->IsRegistered() )
			{
			state = MSIPNetworkObserver::ENetworkActive;
			}
		else
			{
			state = MSIPNetworkObserver::ENetworkInactive;
			}
		}
		
	return state;
	}

// -----------------------------------------------------------------------------
// CPacketBearerMonitor::AllLinesIdle()
// -----------------------------------------------------------------------------
//	
TBool CPacketBearerMonitor::AllLinesIdle()
	{
	// Get line state, if we are in Dual Mode (and don't have a Line Monitor)
	// specify that all lines are idle.
	TBool allLinesIdle( ETrue );
	if( iLineMonitor )
		{
		allLinesIdle = iLineMonitor->AllLinesIdle();
		}
	return allLinesIdle;	
	}

// -----------------------------------------------------------------------------
// CPacketBearerMonitor::InfoChanged
// -----------------------------------------------------------------------------
//	
void CPacketBearerMonitor::InfoChanged( const TSIPAccessNetworkInfo& aInfo )
	{	
	iNetworkInfoObserver.InfoChanged( aInfo );
	}
