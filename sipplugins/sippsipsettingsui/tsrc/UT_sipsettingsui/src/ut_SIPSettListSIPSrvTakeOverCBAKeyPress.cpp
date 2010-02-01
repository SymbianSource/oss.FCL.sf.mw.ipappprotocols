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

#include "UT_SIPSettListSIPSrvTakeOverCBAKeyPress.h"
#include "SIPSettListSIPSrvTakeOverCBAKeyPress.h"

//  SYSTEM INCLUDES
#include <eunitmacros.h>
#include <coecntrl.h>

/*
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
UT_CSIPSettListSIPSrvTakeOverCBAKeyPress* UT_CSIPSettListSIPSrvTakeOverCBAKeyPress::NewL()
    {
    UT_CSIPSettListSIPSrvTakeOverCBAKeyPress* self = UT_CSIPSettListSIPSrvTakeOverCBAKeyPress::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
UT_CSIPSettListSIPSrvTakeOverCBAKeyPress* UT_CSIPSettListSIPSrvTakeOverCBAKeyPress::NewLC()
    {
    UT_CSIPSettListSIPSrvTakeOverCBAKeyPress* self = new( ELeave ) UT_CSIPSettListSIPSrvTakeOverCBAKeyPress();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
UT_CSIPSettListSIPSrvTakeOverCBAKeyPress::~UT_CSIPSettListSIPSrvTakeOverCBAKeyPress()
    {
    // NOP
    }


// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------
//
UT_CSIPSettListSIPSrvTakeOverCBAKeyPress::UT_CSIPSettListSIPSrvTakeOverCBAKeyPress()
    {
    // NOP
    }


// -----------------------------------------------------------------------------
// Second phase construct
// -----------------------------------------------------------------------------
//
void UT_CSIPSettListSIPSrvTakeOverCBAKeyPress::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }
    

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void UT_CSIPSettListSIPSrvTakeOverCBAKeyPress::SetupL()
    {
    iTakeOverCBAKeys = CSIPSettListSIPSrvTakeOverCBAKeyPress::NewL();
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void UT_CSIPSettListSIPSrvTakeOverCBAKeyPress::Teardown()
    {
    delete iTakeOverCBAKeys;
    }



// TEST CASES

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void UT_CSIPSettListSIPSrvTakeOverCBAKeyPress::UT_CSIPSettListSIPSrvTakeOverCBAKeyPress_OfferKeyEventLL()
    {
	TKeyEvent testKeyEvent;
	TEventCode testEventCode;
	testKeyEvent.iScanCode = EStdKeyDevice1;
	
	EUNIT_ASSERT_EQUALS( iTakeOverCBAKeys->BackCalled(), EFalse );
	EUNIT_ASSERT_EQUALS( iTakeOverCBAKeys->OfferKeyEventL ( testKeyEvent, EEventKeyDown ), EKeyWasNotConsumed );
	EUNIT_ASSERT_EQUALS( iTakeOverCBAKeys->BackCalled(), ETrue );
	
    }
    


//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CSIPSettListSIPSrvTakeOverCBAKeyPress,
    "UT_CSIPSettListSIPSrvTakeOverCBAKeyPress",
    "UNIT" )

EUNIT_TEST(
    "OfferKeyEventL - test ",
    "CSIPSettListSIPSrvTakeOverCBAKeyPress",
    "OfferKeyEventLL",
    "FUNCTIONALITY",
    SetupL, UT_CSIPSettListSIPSrvTakeOverCBAKeyPress_OfferKeyEventLL, Teardown)
    
        
EUNIT_END_TEST_TABLE

//  END OF FILE
*/