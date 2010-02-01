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
// Name        : CPacketContextWatcher.cpp
// Part of     : NetworkMonitor
// Implementation
// Version     : SIP/4.0
//



#include "CPacketContextWatcher.h"
#include "CPacketContextMonitor.h"


// -----------------------------------------------------------------------------
// CPacketContextWatcher::NewL
// -----------------------------------------------------------------------------
//
CPacketContextWatcher* CPacketContextWatcher::NewL( 
    CPacketContextMonitor& aParent,
    RSocketServ& aSocketSrv,
    TConnectionInfo aConnectionInfo )
	{
	CPacketContextWatcher* self = CPacketContextWatcher::NewLC( aParent, 
	                                                            aSocketSrv,
	                                                            aConnectionInfo );
	CleanupStack::Pop( self );
	return self;
	}
	
// -----------------------------------------------------------------------------
// CPacketContextWatcher::NewLC
// -----------------------------------------------------------------------------
//
CPacketContextWatcher* CPacketContextWatcher::NewLC( 
    CPacketContextMonitor& aParent,
    RSocketServ& aSocketSrv,
    TConnectionInfo aConnectionInfo )
	{
	CPacketContextWatcher* self = new( ELeave ) CPacketContextWatcher( 
	    aParent, aSocketSrv, aConnectionInfo );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}


// -----------------------------------------------------------------------------
// CPacketContextWatcher::~CPacketContextWatcher
// -----------------------------------------------------------------------------
//
CPacketContextWatcher::~CPacketContextWatcher()
	{
	Cancel();
	iConnection.Close();
	}

// -----------------------------------------------------------------------------
// CPacketContextWatcher::WatchStatusChange
// -----------------------------------------------------------------------------
//	
void CPacketContextWatcher::WatchStatusChange()
    {
    if( !IsActive() )
        {
        iConnection.ProgressNotification( iProgress, iStatus, KConnectionClosed );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CPacketContextWatcher::DoCancel
// -----------------------------------------------------------------------------
//
void CPacketContextWatcher::DoCancel()
	{
	iConnection.CancelProgressNotification();
	}

// -----------------------------------------------------------------------------
// CPacketContextWatcher::RunL
// -----------------------------------------------------------------------------
//
void CPacketContextWatcher::RunL()
	{	
	TInt error = iStatus.Int();
	
	// Request further notifications from RConnection if possible
	if( !error && !iProgress().iError )
	    {  
    	WatchStatusChange();
	    }
	    
	// Notify observers about PDP context state changes and errors.
	// NOTE: may lead to self deletion
	if ( error != KErrCancel )
		{
	    iParent.WatcherStateChanged( error );
		}
	}
    
// -----------------------------------------------------------------------------
// CPacketContextWatcher::IsContextAvailable
// -----------------------------------------------------------------------------
//
TBool CPacketContextWatcher::IsContextAvailable()
    {
    return ( iProgress().iStage == KLinkLayerClosed ||
             iProgress().iStage == KConnectionClosed ||
             iProgress().iStage == KConnectionFailure );
    }

// -----------------------------------------------------------------------------
// CPacketContextWatcher::CPacketContextWatcher
// -----------------------------------------------------------------------------
//
CPacketContextWatcher::CPacketContextWatcher( CPacketContextMonitor& aParent,
                                              RSocketServ& aSocketSrv,
                                              TConnectionInfo aConnectionInfo )
	: CActive( CActive::EPriorityStandard ),
	  iParent( aParent ),
	  iSocketSrv( aSocketSrv ),
	  iConnectionInfo( aConnectionInfo )
	{
	CActiveScheduler::Add( this );
	}

// -----------------------------------------------------------------------------
// CPacketContextWatcher::ConstructL
// -----------------------------------------------------------------------------
//
void CPacketContextWatcher::ConstructL()
	{	
    User::LeaveIfError( iConnection.Open( iSocketSrv ) );    
    
    User::LeaveIfError( 
        iConnection.Attach( iConnectionInfo, RConnection::EAttachTypeMonitor ) );
    
    WatchStatusChange();
	}

