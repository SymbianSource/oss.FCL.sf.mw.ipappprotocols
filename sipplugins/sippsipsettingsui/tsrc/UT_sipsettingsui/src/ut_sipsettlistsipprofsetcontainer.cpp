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
/*
 * ut_sipsettlistsipprofsetcontainer.cpp
 *
 *  Created on: 2009-11-6
 *      Author: cninshmi
 */

//  INTERNAL INCLUDES


//  SYSTEM INCLUDES
#include <eunitmacros.h>
#include <e32std.h>
#include "ut_sipsettlistsipprofsetcontainer.h"
#include "SIPSettListSIPProfSetModel.h"
#include "SIPSettListSIPProfSetView.h"
#include "gssipmodel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
UT_SIPSettListSIPProfSetContainer* UT_SIPSettListSIPProfSetContainer::NewL()
    {
    UT_SIPSettListSIPProfSetContainer* self = UT_SIPSettListSIPProfSetContainer::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
UT_SIPSettListSIPProfSetContainer* UT_SIPSettListSIPProfSetContainer::NewLC()
    {
    UT_SIPSettListSIPProfSetContainer* self = new( ELeave ) UT_SIPSettListSIPProfSetContainer();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
UT_SIPSettListSIPProfSetContainer::~UT_SIPSettListSIPProfSetContainer()
    {
    // NOP
    }


// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------
//
UT_SIPSettListSIPProfSetContainer::UT_SIPSettListSIPProfSetContainer()
    {
    // NOP
    }


// -----------------------------------------------------------------------------
// Second phase construct
// -----------------------------------------------------------------------------
//
void UT_SIPSettListSIPProfSetContainer::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }
    

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void UT_SIPSettListSIPProfSetContainer::SetupL()
    {
    CGSSIPModel* handler = CGSSIPModel::NewL();
    iSipProfContainer = new ( ELeave )CSIPSettListSIPProfSetContainer;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void UT_SIPSettListSIPProfSetContainer::Teardown()
    {
    if (iSipProfContainer != NULL)
        {
        delete iSipProfContainer;
        iSipProfContainer = NULL;
        }
    }



// TEST CASES

// -----------------------------------------------------------------------------
// TestConstructL()
// -----------------------------------------------------------------------------
//
void UT_SIPSettListSIPProfSetContainer::TestNewL()
    {
    
    }
    

// -----------------------------------------------------------------------------
// TestEditCurrentListItemL()
// -----------------------------------------------------------------------------
//
void UT_SIPSettListSIPProfSetContainer::TestEditCurrentListItemL()
    {
    
    }

// -----------------------------------------------------------------------------
// TestStoreSettingsL()
// -----------------------------------------------------------------------------
//
void UT_SIPSettListSIPProfSetContainer::TestStoreSettingsL()
    {
    
    }

// -----------------------------------------------------------------------------
// TestCountComponentControls()
// -----------------------------------------------------------------------------
//
void UT_SIPSettListSIPProfSetContainer::TestCountComponentControls()
    {
    
    }

// -----------------------------------------------------------------------------
// TestComponentControl()
// -----------------------------------------------------------------------------
//
void UT_SIPSettListSIPProfSetContainer::TestComponentControl()
    {
    
    }

// -----------------------------------------------------------------------------
// TestSizeChanged()
// -----------------------------------------------------------------------------
//
void UT_SIPSettListSIPProfSetContainer::TestSizeChanged()
    {
    
    }

// -----------------------------------------------------------------------------
// TestOfferKeyEventL()
// -----------------------------------------------------------------------------
//
void UT_SIPSettListSIPProfSetContainer::TestOfferKeyEventL()
    {
    //Test case 1: aKeyEvent.iCode == EKeyUpArrow
    {
    
    }
    
    //Test case 2: aKeyEvent.iCode == EKeyDownArrow
    {
    
    }
    
    //Test case 3: aKeyEvent.iCode != any one above
    {
    
    }

    }

// -----------------------------------------------------------------------------
// TestFocusChanged()
// -----------------------------------------------------------------------------
//
void UT_SIPSettListSIPProfSetContainer::TestFocusChanged()
    {
    //Test case 1: iItemList = NULL
    {
    
    }
    
    //Test case 2: iItemList != NULL
    {
    
    }
    }

// -----------------------------------------------------------------------------
// TestHandleResourceChange()
// -----------------------------------------------------------------------------
//
void UT_SIPSettListSIPProfSetContainer::TestHandleResourceChange()
    {
    
    }

// -----------------------------------------------------------------------------
// TestGetHelpContext()
// -----------------------------------------------------------------------------
//
void UT_SIPSettListSIPProfSetContainer::TestGetHelpContext()
    {
    
    }


//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
	UT_SIPSettListSIPProfSetContainer,
    "UT_SIPSettListSIPProfSetContainer",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "SIPSettListSIPProfSetContainer",
    "NewL",
    "FUNCTIONALITY",
    SetupL, TestNewL, Teardown)
    
EUNIT_TEST(
    "EditCurrentListItemL - test ",
    "SIPSettListSIPProfSetContainer",
    "EditCurrentListItemL",
    "FUNCTIONALITY",
    SetupL, TestEditCurrentListItemL, Teardown)
    
EUNIT_TEST(
    "StoreSettingsL - test ",
    "SIPSettListSIPProfSetContainer",
    "StoreSettingsL",
    "FUNCTIONALITY",
    SetupL, TestStoreSettingsL, Teardown)
    
EUNIT_TEST(
    "CountComponentControls - test ",
    "SIPSettListSIPProfSetContainer",
    "CountComponentControls",
    "FUNCTIONALITY",
    SetupL, TestCountComponentControls, Teardown)
    
EUNIT_TEST(
    "ComponentControl - test ",
    "SIPSettListSIPProfSetContainer",
    "ComponentControl",
    "FUNCTIONALITY",
    SetupL, TestComponentControl, Teardown)
    
EUNIT_TEST(
    "SizeChanged - test ",
    "SIPSettListSIPProfSetContainer",
    "SizeChanged",
    "FUNCTIONALITY",
    SetupL, TestSizeChanged, Teardown)
    
EUNIT_TEST(
    "OfferKeyEventL - test ",
    "SIPSettListSIPProfSetContainer",
    "OfferKeyEventL",
    "FUNCTIONALITY",
    SetupL, TestOfferKeyEventL, Teardown)
    
EUNIT_TEST(
    "HandleResourceChange - test ",
    "SIPSettListSIPProfSetContainer",
    "HandleResourceChange",
    "FUNCTIONALITY",
    SetupL, TestHandleResourceChange, Teardown)
    
EUNIT_TEST(
    "GetHelpContext - test ",
    "SIPSettListSIPProfSetContainer",
    "GetHelpContext",
    "FUNCTIONALITY",
    SetupL, TestGetHelpContext, Teardown)
    
EUNIT_TEST(
    "FocusChanged - test ",
    "SIPSettListSIPProfSetContainer",
    "FocusChanged",
    "FUNCTIONALITY",
    SetupL, TestFocusChanged, Teardown)
    
        
EUNIT_END_TEST_TABLE

//  END OF FILE


