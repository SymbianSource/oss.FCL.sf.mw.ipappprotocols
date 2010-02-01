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

#include "ut_SIPSettingsContainer.h"
#include "SIPSettingsPlugin.h"
#include "gssipsettingsplugin.hrh"

//  SYSTEM INCLUDES
#include <eunitmacros.h>

//gssiptimerstub.cpp
extern TInt gStartTimerInvokedTimes;
extern TInt gStopTimerInvokedTimes;
//sipsettingspluginstub.cpp
extern TInt gEditProfileLInvokedTimes;
extern TInt gDeleteProfileLInvokedTimes;
extern TInt gChangeDefaultProfileLInvokedTimes;
extern TInt gHandleCommandLInvokedTimes;



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
UT_CSIPSettingsContainer* UT_CSIPSettingsContainer::NewL()
    {
    UT_CSIPSettingsContainer* self = UT_CSIPSettingsContainer::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
UT_CSIPSettingsContainer* UT_CSIPSettingsContainer::NewLC()
    {
    UT_CSIPSettingsContainer* self = new( ELeave ) UT_CSIPSettingsContainer();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
UT_CSIPSettingsContainer::~UT_CSIPSettingsContainer()
    {
    // NOP
    }


// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------
//
UT_CSIPSettingsContainer::UT_CSIPSettingsContainer()
    {
    // NOP
    }


// -----------------------------------------------------------------------------
// Second phase construct
// -----------------------------------------------------------------------------
//
void UT_CSIPSettingsContainer::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }
    

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void UT_CSIPSettingsContainer::SetupL()
    {
    TAny *aparam = NULL;
//    CSIPSettingsPluginAdapter *sipsettingspluginadapter = CSIPSettingsPluginAdapter::NewL( aparam );
//    iSipSettingsContainer = new (ELeave) CSIPSettingsContainer( (CSIPSettingsPlugin*) sipsettingspluginadapter );
    
    CSIPSettingsPlugin *sipsettingplugin = CSIPSettingsPlugin::NewL( aparam );
    iSipSettingsContainer = new (ELeave) CSIPSettingsContainer( sipsettingplugin );
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void UT_CSIPSettingsContainer::Teardown()
    {
    if (iSipSettingsContainer != NULL)
        {
        delete iSipSettingsContainer;
        iSipSettingsContainer = NULL;
        }
    }



// TEST CASES

// -----------------------------------------------------------------------------
// UT_CSIPSettingsContainer_OfferKeyEventLL()
// -----------------------------------------------------------------------------
//
void UT_CSIPSettingsContainer::UT_CSIPSettingsContainer_OfferKeyEventLL()
    {
//    TKeyEvent keyevent;
//    keyevent.iCode = EKeyUpArrow;
//    TEventCode eventcode;
//    eventcode = EEventKey;
//
//    gStartTimerInvokeTimes = 0;
//    iSipSettingsContainer->OfferKeyEventL( keyevent, eventcode );
//    EUNIT_ASSERT_EQUALS( gStartTimerInvokeTimes, 1 ); 
    
    
//    keyevent.iCode = EKeyDownArrow;
//    keyevent.iCode = EKeyBackspace;
//    TEventCode eventcode;
//    eventcode = EEventKey;
    
    }
    
// -----------------------------------------------------------------------------
// UT_CSIPSettingsContainer_FocusChangedL()
// -----------------------------------------------------------------------------
//
void UT_CSIPSettingsContainer::UT_CSIPSettingsContainer_FocusChangedL()
	{
    }

// -----------------------------------------------------------------------------
// UT_CSIPSettingsContainer_HandleLongTapEventLL()
// -----------------------------------------------------------------------------
//
void UT_CSIPSettingsContainer::UT_CSIPSettingsContainer_HandleLongTapEventLL()
	{
    //Yes
    }

// -----------------------------------------------------------------------------
// UT_CSIPSettingsContainer_ProcessCommandLL()
// -----------------------------------------------------------------------------
//
void UT_CSIPSettingsContainer::UT_CSIPSettingsContainer_ProcessCommandLL()
	{
	gEditProfileLInvokedTimes = 0;
	iSipSettingsContainer->ProcessCommandL( EGSCmdAppEdit );
	EUNIT_ASSERT_EQUALS( gEditProfileLInvokedTimes, 1 ); 
	
	gDeleteProfileLInvokedTimes = 0;
	iSipSettingsContainer->ProcessCommandL( EGSCmdAppDelete );
	EUNIT_ASSERT_EQUALS( gDeleteProfileLInvokedTimes, 1 );
	
	gChangeDefaultProfileLInvokedTimes = 0;
	iSipSettingsContainer->ProcessCommandL( EGSCmdAppDefault );
	EUNIT_ASSERT_EQUALS( gChangeDefaultProfileLInvokedTimes, 1 );	
	
	iSipSettingsContainer->ProcessCommandL( EGSCmdAppNew );
	EUNIT_ASSERT_EQUALS( gEditProfileLInvokedTimes, 1 ); 
	EUNIT_ASSERT_EQUALS( gDeleteProfileLInvokedTimes, 1 );
	EUNIT_ASSERT_EQUALS( gChangeDefaultProfileLInvokedTimes, 1 );
    }

// -----------------------------------------------------------------------------
// UT_CSIPSettingsContainer_HandlePointerEventLL()
// -----------------------------------------------------------------------------
//
void UT_CSIPSettingsContainer::UT_CSIPSettingsContainer_HandlePointerEventLL()
	{
	//Yes
    }

// -----------------------------------------------------------------------------
// UT_CSIPSettingsContainer_IfPointerEventL()
// -----------------------------------------------------------------------------
//
void UT_CSIPSettingsContainer::UT_CSIPSettingsContainer_IfPointerEventL()
	{
	//Yes
    }

// -----------------------------------------------------------------------------
// UT_CSIPSettingsContainer_SetEmphasisL()
// -----------------------------------------------------------------------------
//
void UT_CSIPSettingsContainer::UT_CSIPSettingsContainer_SetEmphasisL()
	{
	iSipSettingsContainer->SetEmphasis(NULL,NULL);
    }

// -----------------------------------------------------------------------------
// UT_CSIPSettingsContainer_LongPressStatusL()
// -----------------------------------------------------------------------------
//
void UT_CSIPSettingsContainer::UT_CSIPSettingsContainer_LongPressStatusL()
	{
	TBool result = EFalse;

	iSipSettingsContainer->iLongPress = ETrue;
	result = iSipSettingsContainer->LongPressStatus();
	EUNIT_ASSERT_EQUALS( result, ETrue ); 
	
	iSipSettingsContainer->iLongPress = EFalse;
	result = iSipSettingsContainer->LongPressStatus();
	EUNIT_ASSERT_EQUALS( result, EFalse ); 
    }

// -----------------------------------------------------------------------------
// UT_CSIPSettingsContainer_SetLongPress()
// -----------------------------------------------------------------------------
//
void UT_CSIPSettingsContainer::UT_CSIPSettingsContainer_SetLongPressL()
	{
	iSipSettingsContainer->SetLongPress(ETrue);
	EUNIT_ASSERT_EQUALS( iSipSettingsContainer->iLongPress, ETrue ); 
	
	iSipSettingsContainer->SetLongPress(EFalse);
	EUNIT_ASSERT_EQUALS( iSipSettingsContainer->iLongPress, EFalse ); 
    }

// -----------------------------------------------------------------------------
// UT_CSIPSettingsContainer_StylusPopupMenuL()
// -----------------------------------------------------------------------------
//
void UT_CSIPSettingsContainer::UT_CSIPSettingsContainer_StylusPopupMenuL()
	{
	CAknStylusPopUpMenu *result = NULL;
	result = iSipSettingsContainer->StylusPopupMenu();
	}

// -----------------------------------------------------------------------------
// UT_CSIPSettingsContainer_SetStylusPopupMenuL()
// -----------------------------------------------------------------------------
//
void UT_CSIPSettingsContainer::UT_CSIPSettingsContainer_SetStylusPopupMenuL()
	{
	CAknStylusPopUpMenu *styluspopupmenu = NULL;
	styluspopupmenu = iSipSettingsContainer->iStylusPopupMenu;
	
	iSipSettingsContainer->SetStylusPopupMenu(NULL);
	EUNIT_ASSERT(iSipSettingsContainer->iStylusPopupMenu == NULL);
	
	iSipSettingsContainer->iStylusPopupMenu = styluspopupmenu;
	
	}

// -----------------------------------------------------------------------------
// UT_CSIPSettingsContainer_PenEventLocationL()
// -----------------------------------------------------------------------------
//
void UT_CSIPSettingsContainer::UT_CSIPSettingsContainer_PenEventLocationL()
	{
	TPoint oldpoint;
	
	TPoint point;
	point.iX = 10;
	point.iY = 10;
	
	TPoint pointresult;
	
	oldpoint = iSipSettingsContainer->iPoint;
	iSipSettingsContainer->iPoint = point;
	pointresult = iSipSettingsContainer->PenEventLocation();
	EUNIT_ASSERT_EQUALS( pointresult.iX, 10 );
	EUNIT_ASSERT_EQUALS( pointresult.iY, 10 );
	
	iSipSettingsContainer->iPoint = oldpoint;
	}


//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CSIPSettingsContainer,
    "UT_CSIPSettingsContainer",
    "UNIT" )

EUNIT_TEST(
    "OfferKeyEventL - test ",
    "CSIPSettingsContainer",
    "OfferKeyEventL",
    "FUNCTIONALITY",
    SetupL, UT_CSIPSettingsContainer_OfferKeyEventLL, Teardown)
    
EUNIT_TEST(
    "FocusChanged - test ",
    "CSIPSettingsContainer",
    "FocusChanged",
    "FUNCTIONALITY",
    SetupL, UT_CSIPSettingsContainer_FocusChangedL, Teardown)
    
EUNIT_TEST(
    "HandleLongTapEventL - test ",
    "CSIPSettingsContainer",
    "HandleLongTapEventL",
    "FUNCTIONALITY",
    SetupL, UT_CSIPSettingsContainer_HandleLongTapEventLL, Teardown)
    
EUNIT_TEST(
    "ProcessCommandL - test ",
    "CSIPSettingsContainer",
    "ProcessCommandL",
    "FUNCTIONALITY",
    SetupL, UT_CSIPSettingsContainer_ProcessCommandLL, Teardown)
    
EUNIT_TEST(
    "HandlePointerEventL - test ",
    "CSIPSettingsContainer",
    "HandlePointerEventL",
    "FUNCTIONALITY",
    SetupL, UT_CSIPSettingsContainer_HandlePointerEventLL, Teardown)
    
EUNIT_TEST(
    "IfPointerEvent - test ",
    "CSIPSettingsContainer",
    "IfPointerEvent",
    "FUNCTIONALITY",
    SetupL, UT_CSIPSettingsContainer_IfPointerEventL, Teardown)
    
EUNIT_TEST(
    "SetEmphasis - test ",
    "CSIPSettingsContainer",
    "SetEmphasis",
    "FUNCTIONALITY",
    SetupL, UT_CSIPSettingsContainer_SetEmphasisL, Teardown)
    
EUNIT_TEST(
    "LongPressStatus - test ",
    "CSIPSettingsContainer",
    "LongPressStatus",
    "FUNCTIONALITY",
    SetupL, UT_CSIPSettingsContainer_LongPressStatusL, Teardown)    
    
EUNIT_TEST(
    "SetLongPressL - test ",
    "CSIPSettingsContainer",
    "SetLongPressL",
    "FUNCTIONALITY",
    SetupL, UT_CSIPSettingsContainer_SetLongPressL, Teardown)  
//yes    
EUNIT_TEST(
    "StylusPopupMenuL - test ",
    "CSIPSettingsContainer",
    "StylusPopupMenuL",
    "FUNCTIONALITY",
    SetupL, UT_CSIPSettingsContainer_StylusPopupMenuL, Teardown)  
    
EUNIT_TEST(
    "SetStylusPopupMenuL - test ",
    "CSIPSettingsContainer",
    "SetStylusPopupMenuL",
    "FUNCTIONALITY",
    SetupL, UT_CSIPSettingsContainer_SetStylusPopupMenuL, Teardown)      
    
EUNIT_TEST(
    "PenEventLocationL - test ",
    "CSIPSettingsContainer",
    "PenEventLocationL",
    "FUNCTIONALITY",
    SetupL, UT_CSIPSettingsContainer_PenEventLocationL, Teardown)         
    
EUNIT_END_TEST_TABLE

//  END OF FILE


