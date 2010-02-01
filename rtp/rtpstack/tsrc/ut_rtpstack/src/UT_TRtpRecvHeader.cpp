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
#include "UT_TRtpRecvHeader.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "rtpheader.h"

// CONSTRUCTION
UT_TRtpRecvHeader* UT_TRtpRecvHeader::NewL()
    {
    UT_TRtpRecvHeader* self = UT_TRtpRecvHeader::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_TRtpRecvHeader* UT_TRtpRecvHeader::NewLC()
    {
    UT_TRtpRecvHeader* self = new( ELeave ) UT_TRtpRecvHeader();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_TRtpRecvHeader::~UT_TRtpRecvHeader()
    {
    }

// Default constructor
UT_TRtpRecvHeader::UT_TRtpRecvHeader()
    {
    }

// Second phase construct
void UT_TRtpRecvHeader::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_TRtpRecvHeader::SetupL(  )
    {

    }

void UT_TRtpRecvHeader::Teardown(  )
    {

    }

void UT_TRtpRecvHeader::UT_TRtpRecvHeader_TRtpRecvHeaderL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_TRtpRecvHeader,
    "TRtpRecvHeader",
    "UNIT" )

EUNIT_TEST(
    "TRtpRecvHeader - test ",
    "TRtpRecvHeader",
    "TRtpRecvHeader",
    "FUNCTIONALITY",
    SetupL, UT_TRtpRecvHeader_TRtpRecvHeaderL, Teardown)


EUNIT_END_TEST_TABLE

//  END OF FILE
