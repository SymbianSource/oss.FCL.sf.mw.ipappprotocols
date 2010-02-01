/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


//  INTERNAL INCLUDES

#include "ut_gssiptimer.h"

//  SYSTEM INCLUDES
#include <eunitmacros.h>



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
UT_GSSIPTimer* UT_GSSIPTimer::NewL()
    {
    UT_GSSIPTimer* self = UT_GSSIPTimer::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
UT_GSSIPTimer* UT_GSSIPTimer::NewLC()
    {
    UT_GSSIPTimer* self = new( ELeave ) UT_GSSIPTimer();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
UT_GSSIPTimer::~UT_GSSIPTimer()
    {
    // NOP
    }


// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------
//
UT_GSSIPTimer::UT_GSSIPTimer()
    {
    // NOP
    }


// -----------------------------------------------------------------------------
// Second phase construct
// -----------------------------------------------------------------------------
//
void UT_GSSIPTimer::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }
    

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void UT_GSSIPTimer::SetupL()
    {
    CCoeControl* coeControl = new ( ELeave ) CCoeControl;
    CTextListItemDrawer* itemDraw = new ( ELeave ) CTextListItemDrawer;
    iTimer = CGSSIPTimer::NewL( *coeControl, *itemDraw );
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void UT_GSSIPTimer::Teardown()
    {
    delete iItemDraw;
    iItemDraw = NULL;
    
    delete iCoeControl;
    iCoeControl = NULL;
    
    delete iTimer;
    iTimer = NULL;
    }



// TEST CASES

// -----------------------------------------------------------------------------
// UT_GSSIPTimer_Beat()
// -----------------------------------------------------------------------------
//
void UT_GSSIPTimer::UT_GSSIPTimer_Beat()
    {
    const TInt KTimerPeriod = 6;
    
    iTimer->iIfStart = EFalse;
    iTimer->iTotal = KTimerPeriod - 1;
    iTimer->Beat();
    EUNIT_ASSERT_EQUALS( iTimer->iTotal, KTimerPeriod );
    EUNIT_ASSERT_EQUALS( iTimer->iIfStart, EFalse );   
    
    iTimer->iIfStart = ETrue;
    iTimer->iTotal = KTimerPeriod - 1;
    iTimer->Beat();
    EUNIT_ASSERT_EQUALS( iTimer->iTotal, 0 );
    EUNIT_ASSERT_EQUALS( iTimer->iIfStart, EFalse );   
    
    iTimer->Beat();
    EUNIT_ASSERT_EQUALS( iTimer->iTotal, 1 );
    }


// -----------------------------------------------------------------------------
// UT_GSSIPTimer_Synchronize()
// -----------------------------------------------------------------------------
//
void UT_GSSIPTimer::UT_GSSIPTimer_Synchronize()
    {
    iTimer->Synchronize();
    EUNIT_ASSERT( ETrue );
    }


// -----------------------------------------------------------------------------
// UT_GSSIPTimer_StartTimer()
// -----------------------------------------------------------------------------
//
void UT_GSSIPTimer::UT_GSSIPTimer_StartTimer()
    {
    iTimer->iTotal = 2;
    iTimer->iIfStart = EFalse;
    iTimer->StartTimer();
    EUNIT_ASSERT_EQUALS( iTimer->iIfStart, ETrue );
    EUNIT_ASSERT_EQUALS( iTimer->iTotal, 0 )  
    
    iTimer->iTotal = 2;
    iTimer->iIfStart = ETrue;
    iTimer->StartTimer();
    EUNIT_ASSERT_EQUALS( iTimer->iIfStart, ETrue );
    EUNIT_ASSERT_EQUALS( iTimer->iTotal, 0 )
    }


// -----------------------------------------------------------------------------
// UT_GSSIPTimer_StopTimer()
// -----------------------------------------------------------------------------
//
void UT_GSSIPTimer::UT_GSSIPTimer_StopTimer()
    {
    iTimer->iIfStart = EFalse;
    iTimer->StopTimer();
    EUNIT_ASSERT_EQUALS( iTimer->iIfStart, EFalse );
          
    iTimer->iIfStart = ETrue;
    iTimer->StopTimer();
    EUNIT_ASSERT_EQUALS( iTimer->iIfStart, EFalse );   
    }


// -----------------------------------------------------------------------------
// UT_GSSIPTimer_IsStarted()
// -----------------------------------------------------------------------------
//
void UT_GSSIPTimer::UT_GSSIPTimer_IsStarted()
    {
    iTimer->iIfStart = EFalse;
    EUNIT_ASSERT_EQUALS( iTimer->IsStarted(), EFalse );
    
    iTimer->iIfStart = ETrue;
    EUNIT_ASSERT_EQUALS( iTimer->IsStarted(), ETrue );   
    }


//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_GSSIPTimer,
    "UT_GSSIPTimer",
    "UNIT" )
    
EUNIT_TEST(
    "Beat - test ",
    "UT_GSSIPTimer",
    "Beat",
    "FUNCTIONALITY",
    SetupL, UT_GSSIPTimer_Beat, Teardown)    

EUNIT_TEST(
    "Synchronize - test ",
    "UT_GSSIPTimer",
    "Synchronize",
    "FUNCTIONALITY",
    SetupL, UT_GSSIPTimer_Synchronize, Teardown)    

EUNIT_TEST(
    "StartTimer - test ",
    "UT_GSSIPTimer",
    "StartTimer",
    "FUNCTIONALITY",
    SetupL, UT_GSSIPTimer_StartTimer, Teardown)    

EUNIT_TEST(
    "StopTimer - test ",
    "UT_GSSIPTimer",
    "StopTimer",
    "FUNCTIONALITY",
    SetupL, UT_GSSIPTimer_StopTimer, Teardown)    
    
EUNIT_TEST(
    "IsStarted - test ",
    "UT_GSSIPTimer",
    "IsStarted",
    "FUNCTIONALITY",
    SetupL, UT_GSSIPTimer_IsStarted, Teardown)        
        
EUNIT_END_TEST_TABLE

//  END OF FILE


