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
#include "UT_CRtpCommSend.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "rtpcommsend.h"

// CONSTRUCTION
UT_CRtpCommSend* UT_CRtpCommSend::NewL()
    {
    UT_CRtpCommSend* self = UT_CRtpCommSend::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CRtpCommSend* UT_CRtpCommSend::NewLC()
    {
    UT_CRtpCommSend* self = new( ELeave ) UT_CRtpCommSend();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CRtpCommSend::~UT_CRtpCommSend()
    {
    }

// Default constructor
UT_CRtpCommSend::UT_CRtpCommSend()
    {
    }

// Second phase construct
void UT_CRtpCommSend::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_CRtpCommSend::SetupL(  )
    {

    }

void UT_CRtpCommSend::Teardown(  )
    {

    }

void UT_CRtpCommSend::UT_CRtpCommSend_NewLL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpCommSend::UT_CRtpCommSend_SendL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpCommSend::UT_CRtpCommSend_Send_1L(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpCommSend::UT_CRtpCommSend_CancelAsyncSendL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpCommSend::UT_CRtpCommSend_SetToAddressL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CRtpCommSend,
    "CRtpCommSend",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "CRtpCommSend",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpCommSend_NewLL, Teardown)

EUNIT_TEST(
    "Send - test ",
    "CRtpCommSend",
    "Send",
    "FUNCTIONALITY",
    SetupL, UT_CRtpCommSend_SendL, Teardown)

EUNIT_TEST(
    "Send - test ",
    "CRtpCommSend",
    "Send",
    "FUNCTIONALITY",
    SetupL, UT_CRtpCommSend_Send_1L, Teardown)

EUNIT_TEST(
    "CancelAsyncSend - test ",
    "CRtpCommSend",
    "CancelAsyncSend",
    "FUNCTIONALITY",
    SetupL, UT_CRtpCommSend_CancelAsyncSendL, Teardown)

EUNIT_TEST(
    "SetToAddress - test ",
    "CRtpCommSend",
    "SetToAddress",
    "FUNCTIONALITY",
    SetupL, UT_CRtpCommSend_SetToAddressL, Teardown)


EUNIT_END_TEST_TABLE

//  END OF FILE
