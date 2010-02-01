
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
//



// INCLUDES
#include    "SIPExTimeOutTimer.h"
#include    "SIPExTimeOutNotify.h"

// -----------------------------------------------------------------------------
// CSIPExTimeOutTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPExTimeOutTimer* CSIPExTimeOutTimer::NewL( 
    MSIPExTimeOutNotify& aTimeOutNotify )
    {
    CSIPExTimeOutTimer* self = new( ELeave ) CSIPExTimeOutTimer( aTimeOutNotify );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CSIPExTimeOutTimer::CSIPExTimeOutTimer
// Constructor.
// -----------------------------------------------------------------------------
//
CSIPExTimeOutTimer::CSIPExTimeOutTimer( MSIPExTimeOutNotify& aTimeOutNotify )
    : CTimer( EPriorityStandard ), iNotifier( aTimeOutNotify )
    {
    }

// -----------------------------------------------------------------------------
// CSIPExTimeOutTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIPExTimeOutTimer::ConstructL()
    {
	CTimer::ConstructL();
	CActiveScheduler::Add( this );
    }
   
// -----------------------------------------------------------------------------
// CSIPExTimeOutTimer::~CSIPExTimeOutTimer
// Destructor.
// -----------------------------------------------------------------------------
//
CSIPExTimeOutTimer::~CSIPExTimeOutTimer()
    {
	Cancel();
    }

// -----------------------------------------------------------------------------
// CSIPExTimeOutTimer::RunL
// To inform the notifier that xx seconds is up.
// -----------------------------------------------------------------------------
//
void CSIPExTimeOutTimer::RunL()
    {
	// Tell the notifier that times out.
	iNotifier.TimerExpired();
    }

//  End of File  
