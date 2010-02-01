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

#include "ut_sipsettlistsipsrvsetview.h"
#include "SIPSettListSIPSrvSetModel.h"
#include "SIPSettListSIPSrvSetContainer.h"
#include "FeatMgr.h"


//  SYSTEM INCLUDES
#include <eunitmacros.h>
#include <eikmenup.h>
#include <GSSIPSettingsPluginRsc.rsg>
#include <avkon.hrh>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
UT_SIPSettListSIPRegSetView* UT_SIPSettListSIPRegSetView::NewL()
    {
    UT_SIPSettListSIPRegSetView* self = UT_SIPSettListSIPRegSetView::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
UT_SIPSettListSIPRegSetView* UT_SIPSettListSIPRegSetView::NewLC()
    {
    UT_SIPSettListSIPRegSetView* self = new( ELeave ) UT_SIPSettListSIPRegSetView();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
UT_SIPSettListSIPRegSetView::~UT_SIPSettListSIPRegSetView()
    {
    // NOP
    }


// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------
//
UT_SIPSettListSIPRegSetView::UT_SIPSettListSIPRegSetView()
    {
    // NOP
    }


// -----------------------------------------------------------------------------
// Second phase construct
// -----------------------------------------------------------------------------
//
void UT_SIPSettListSIPRegSetView::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }
    

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void UT_SIPSettListSIPRegSetView::SetupL()
    {
    iSipModel = CGSSIPModel::NewL();
    iView = new ( ELeave ) CSIPSettListSIPRegSetView( iSipModel );
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void UT_SIPSettListSIPRegSetView::Teardown()
    {
    if (iView != NULL)
        {
        delete iView;
        iView = NULL;
        }
    
    if (iSipModel != NULL)
        {
        delete iSipModel;
        iSipModel = NULL;
        }
    }



// TEST CASES

// -----------------------------------------------------------------------------
// UT_SIPSettListSIPRegSetView_DynInitMenuPaneL()
// -----------------------------------------------------------------------------
//
void UT_SIPSettListSIPRegSetView::UT_SIPSettListSIPRegSetView_DynInitMenuPaneL()
    {
    _LIT8 ( KHelpCommand, "Help");
    
    CEikMenuPane* menuPane = new ( ELeave )CEikMenuPane( iView );
    CEikMenuPaneItem::SData data;
    data.iText.Copy(KHelpCommand);
    data.iCommandId = EAknCmdHelp;
    data.iCascadeId=0;
    data.iFlags=0;
    data.iExtraText=KNullDesC; 
    menuPane->AddMenuItemL(data);
    
    // Case: 1
    FeatureManager::IDHelpNotSupported();
    iView->DynInitMenuPaneL( R_GS_SIP_SERVER_SETTING_VIEW_MENU, menuPane );
    
    // Case: 2
    menuPane->AddMenuItemL(data);
    FeatureManager::IDHelpSupported();
    iView->DynInitMenuPaneL( R_GS_SIP_SERVER_SETTING_VIEW_MENU, menuPane );
    
    // Case: 3
    iView->DynInitMenuPaneL( 0, menuPane );
 
    delete menuPane;
    
    }


//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_SIPSettListSIPRegSetView,
    "UT_SIPSettListSIPRegSetView",
    "UNIT" )
    
EUNIT_TEST(
    "DynInitMenuPaneL - test ",
    "UT_SIPSettListSIPRegSetView",
    "DynInitMenuPaneL",
    "FUNCTIONALITY",
    SetupL, UT_SIPSettListSIPRegSetView_DynInitMenuPaneL, Teardown)    

EUNIT_END_TEST_TABLE

//  END OF FILE


