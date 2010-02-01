/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/
// INCLUDE FILES

#include "gssiptimer.h"

// LOCAL CONSTANTS AND MACROS
const TInt KTimerPeriod = 6;

TInt gStartTimerInvokeTimes = 0;
TInt gStopTimerInvokeTimes = 0;

// -----------------------------------------------------------------------------
// CGSSIPTimer::NewL()
// -----------------------------------------------------------------------------
//
CGSSIPTimer* CGSSIPTimer::NewL( const CCoeControl& aContainer, 
		                        CListItemDrawer& aListItemDrawer )
	{
	CGSSIPTimer* self = new( ELeave ) CGSSIPTimer( aContainer, aListItemDrawer );
	    
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );

	return self;
	}

// -----------------------------------------------------------------------------
// CGSSIPTimer::CGSSIPTimer()
// -----------------------------------------------------------------------------
//
CGSSIPTimer::CGSSIPTimer( const CCoeControl& aContainer, 
                          CListItemDrawer& aListItemDrawer )
    : iTotal( 0 ),
      iIfStart ( EFalse ),
      iContainer ( aContainer ),
      iListItemDrawer ( aListItemDrawer )
	{
	
	}

// -----------------------------------------------------------------------------
// CGSSIPTimer::~CGSSIPTimer()
// DestructL.
// -----------------------------------------------------------------------------
//
CGSSIPTimer::~CGSSIPTimer()
	{
	}

// -----------------------------------------------------------------------------
// CGSSIPTimer::ConstructL()
// ConstructL.
// -----------------------------------------------------------------------------
//
void CGSSIPTimer::ConstructL()
	{
	}

// -----------------------------------------------------------------------------
// CGSSIPTimer::StartTimer()
// Start a Heart beat.
// -----------------------------------------------------------------------------
//
void CGSSIPTimer::StartTimer()
	{
	gStartTimerInvokeTimes++;
	}

// -----------------------------------------------------------------------------
// CGSSIPTimer::StopTimer()
// Stop a Heart beat.
// -----------------------------------------------------------------------------
//
void CGSSIPTimer::StopTimer()
	{
	gStopTimerInvokeTimes++;
	}

// -----------------------------------------------------------------------------
// CGSSIPTimer::Beat()
// Count and stop stop.
// -----------------------------------------------------------------------------
//
void CGSSIPTimer::Beat()
	{
	}

// -----------------------------------------------------------------------------
// CGSSIPTimer::Synchronize()
// Synchronize.
// -----------------------------------------------------------------------------
//
void CGSSIPTimer::Synchronize()
	{
	return;
	}

// -----------------------------------------------------------------------------
// CGSSIPTimer::IsStarted()
// IsStarted.
// -----------------------------------------------------------------------------
//
TBool CGSSIPTimer::IsStarted() const
    {
    return ETrue;
    }

//  End of File  
