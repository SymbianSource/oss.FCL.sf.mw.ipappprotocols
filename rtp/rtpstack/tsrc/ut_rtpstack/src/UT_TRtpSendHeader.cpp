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
#include "UT_TRtpSendHeader.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "rtpheader.h"

// CONSTRUCTION
UT_TRtpSendHeader* UT_TRtpSendHeader::NewL()
    {
    UT_TRtpSendHeader* self = UT_TRtpSendHeader::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_TRtpSendHeader* UT_TRtpSendHeader::NewLC()
    {
    UT_TRtpSendHeader* self = new( ELeave ) UT_TRtpSendHeader();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_TRtpSendHeader::~UT_TRtpSendHeader()
    {
    }

// Default constructor
UT_TRtpSendHeader::UT_TRtpSendHeader()
    {
    }

// Second phase construct
void UT_TRtpSendHeader::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_TRtpSendHeader::SetupL(  )
    {

    }

void UT_TRtpSendHeader::Teardown(  )
    {

    }

void UT_TRtpSendHeader::UT_TRtpSendHeader_TRtpSendHeaderL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_TRtpSendHeader,
    "TRtpSendHeader",
    "UNIT" )

EUNIT_TEST(
    "TRtpSendHeader - test ",
    "TRtpSendHeader",
    "TRtpSendHeader",
    "FUNCTIONALITY",
    SetupL, UT_TRtpSendHeader_TRtpSendHeaderL, Teardown)


EUNIT_END_TEST_TABLE

//  END OF FILE
