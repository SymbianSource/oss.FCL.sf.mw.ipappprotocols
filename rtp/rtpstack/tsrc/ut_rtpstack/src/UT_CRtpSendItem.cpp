/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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




//  CLASS HEADER
#include "UT_CRtpSendItem.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "rtpsenditem.h"

// CONSTRUCTION
UT_CRtpSendItem* UT_CRtpSendItem::NewL()
    {
    UT_CRtpSendItem* self = UT_CRtpSendItem::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CRtpSendItem* UT_CRtpSendItem::NewLC()
    {
    UT_CRtpSendItem* self = new( ELeave ) UT_CRtpSendItem();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CRtpSendItem::~UT_CRtpSendItem()
    {
    }

// Default constructor
UT_CRtpSendItem::UT_CRtpSendItem()
    {
    }

// Second phase construct
void UT_CRtpSendItem::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_CRtpSendItem::SetupL(  )
    {

    }

void UT_CRtpSendItem::Teardown(  )
    {

    }

void UT_CRtpSendItem::UT_CRtpSendItem_NewLL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpSendItem::UT_CRtpSendItem_GetDataL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CRtpSendItem,
    "CRtpSendItem",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "CRtpSendItem",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSendItem_NewLL, Teardown)

EUNIT_TEST(
    "GetData - test ",
    "CRtpSendItem",
    "GetData",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSendItem_GetDataL, Teardown)


EUNIT_END_TEST_TABLE

//  END OF FILE
