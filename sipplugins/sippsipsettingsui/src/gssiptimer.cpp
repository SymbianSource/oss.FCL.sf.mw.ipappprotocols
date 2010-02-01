// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description:  Timer for Hight light listbox item Container
//
//


// INCLUDE FILES

#include "gssiptimer.h"
#include  "gssippluginlogger.h"

// LOCAL CONSTANTS AND MACROS
const TInt KTimerPeriod = 6;


// -----------------------------------------------------------------------------
// CGSSIPTimer::NewL()
// -----------------------------------------------------------------------------
//
CGSSIPTimer* CGSSIPTimer::NewL( const CCoeControl& aContainer, 
                                CListItemDrawer& aListItemDrawer )
    {
    __GSLOGSTRING("CGSSIPTimer::NewL" )
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
      iContainer ( aContainer ),
      iListItemDrawer ( aListItemDrawer ),
      iIfStart ( EFalse )
    {
    __GSLOGSTRING("CGSSIPTimer::CGSSIPTimer" )
    }

// -----------------------------------------------------------------------------
// CGSSIPTimer::~CGSSIPTimer()
// DestructL.
// -----------------------------------------------------------------------------
//
CGSSIPTimer::~CGSSIPTimer()
    {
    __GSLOGSTRING("CGSSIPTimer::~CGSSIPTimer" )
    delete iHeart;
    iHeart = NULL;
    }

// -----------------------------------------------------------------------------
// CGSSIPTimer::ConstructL()
// ConstructL.
// -----------------------------------------------------------------------------
//
void CGSSIPTimer::ConstructL()
    {
    iHeart = CHeartbeat::NewL( CActive::EPriorityStandard );
    }

// -----------------------------------------------------------------------------
// CGSSIPTimer::StartTimer()
// Start a Heart beat.
// -----------------------------------------------------------------------------
//
void CGSSIPTimer::StartTimer()
    {
    __GSLOGSTRING("CGSSIPTimer::StartTimer" )
    iTotal = 0;
    
    if ( !iIfStart )
        {
        iIfStart = ETrue;
        iHeart->Start( ETwelveOClock, this );
        }
    
    }

// -----------------------------------------------------------------------------
// CGSSIPTimer::StopTimer()
// Stop a Heart beat.
// -----------------------------------------------------------------------------
//
void CGSSIPTimer::StopTimer()
    {
    __GSLOGSTRING("CGSSIPTimer::StopTimer" )
    if (iIfStart )
        {
         iHeart->Cancel();
        iIfStart = EFalse;
        iTotal = 0;
        }
    else
        {
        iListItemDrawer.SetFlags( CTextListItemDrawer::EDisableHighlight );
        iContainer.DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// CGSSIPTimer::Beat()
// Count and stop stop.
// -----------------------------------------------------------------------------
//
void CGSSIPTimer::Beat()
    {
    iTotal++;
     if( iTotal >= KTimerPeriod )
         {
          StopTimer();
          iListItemDrawer.SetFlags( CTextListItemDrawer::EDisableHighlight );
          iContainer.DrawNow();
         }
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
    return iIfStart;
    }

//  End of File  
