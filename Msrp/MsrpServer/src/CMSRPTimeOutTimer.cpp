/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

// CLASS HEADER
#include    "CMSRPTimeOutTimer.h"

// INTERNAL INCLUDES
#include    "MMSRPTimeOutObserver.h"
#include    "MsrpCommon.h"

// -----------------------------------------------------------------------------
// CMSRPTimeOutTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMSRPTimeOutTimer* CMSRPTimeOutTimer::NewL(
    MMSRPTimeOutObserver& aTimeOutNotify )
    {
    CMSRPTimeOutTimer* self = new( ELeave ) CMSRPTimeOutTimer( aTimeOutNotify );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CMSRPTimeOutTimer::CMSRPTimeOutTimer
// Constructor.
// -----------------------------------------------------------------------------
//
CMSRPTimeOutTimer::CMSRPTimeOutTimer( MMSRPTimeOutObserver& aTimeOutNotify )
    : CTimer( EPriorityStandard ), iNotifier( aTimeOutNotify )
    {
    }

// -----------------------------------------------------------------------------
// CMSRPTimeOutTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMSRPTimeOutTimer::ConstructL()
    {
	CTimer::ConstructL();
	CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CMSRPTimeOutTimer::~CMSRPTimeOutTimer
// Destructor.
// -----------------------------------------------------------------------------
//
CMSRPTimeOutTimer::~CMSRPTimeOutTimer()
    {
	Cancel();
    }

// -----------------------------------------------------------------------------
// CMSRPTimeOutTimer::RunL
// To inform the notifier that xx seconds is up.
// -----------------------------------------------------------------------------
//
void CMSRPTimeOutTimer::RunL()
    {
    MSRPLOG( "CMSRPTimeOutTimer::RunL() enter" )
	// Tell the notifier that times out.
	iNotifier.TimerExpiredL();
    MSRPLOG( "CMSRPTimeOutTimer::RunL() exit" )
    }

//  End of File
