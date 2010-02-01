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
#include "UT_TRtpHeaderExtension.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "rtpheader.h"

// CONSTRUCTION
UT_TRtpHeaderExtension* UT_TRtpHeaderExtension::NewL()
    {
    UT_TRtpHeaderExtension* self = UT_TRtpHeaderExtension::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_TRtpHeaderExtension* UT_TRtpHeaderExtension::NewLC()
    {
    UT_TRtpHeaderExtension* self = new( ELeave ) UT_TRtpHeaderExtension();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_TRtpHeaderExtension::~UT_TRtpHeaderExtension()
    {
    }

// Default constructor
UT_TRtpHeaderExtension::UT_TRtpHeaderExtension()
    {
    }

// Second phase construct
void UT_TRtpHeaderExtension::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_TRtpHeaderExtension::SetupL(  )
    {

    }

void UT_TRtpHeaderExtension::Teardown(  )
    {

    }

void UT_TRtpHeaderExtension::UT_TRtpHeaderExtension_TRtpHeaderExtensionL(  )
    {
      TInt result( KErrNone );

    // "" ) );
    // "Test description: TRtpHeaderExtension constructor" ) );
    // "TRtpHeaderExtension::TRtpHeaderExtension()") );

    TRtpHeaderExtension obj;
    if ( obj.iData != 0 ||
         obj.iLength != 0 ||
         obj.iType != 0 )
        {
        // "Bad initial values" ) );
        // "TRtpHeaderExtension::TRtpHeaderExtension() - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
    else
        {
        EUNIT_ASSERT(ETrue);
        }        

    // "" ) );
    // "Test description: TRtpSendHeader constructor" ) );
    // "TRtpSendHeader::TRtpSendHeader()") );
    TRtpSendHeader obj2;
    if ( obj2.iHeaderExtension != NULL ||
         obj2.iMarker != 0 ||
         obj2.iPayloadType != 0 ||
         obj2.iTimestamp != 0 ||
         obj2.iPadding != 0 )
        {
        // "Bad initial values" ) );
        // "TRtpSendHeader::TRtpSendHeader() - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
    else
        {
        EUNIT_ASSERT(ETrue);
        }
    
    // "" ) );
    // "Test description: TRtpRecvHeader constructor" ) );
    // "TRtpRecvHeader::TRtpRecvHeader()") );
    TRtpRecvHeader obj3;
    if ( obj3.iHeaderExtension != NULL ||
         obj3.iMarker != 0 ||
         obj3.iPayloadType != 0 ||
         obj3.iTimestamp != 0 ||
         obj3.iPadding != 0 ||
         obj3.iCsrcCount != 0 ||
         obj3.iCsrcList != 0 ||
         obj3.iExtension != 0 ||
         obj3.iSeqNum != 0 )
        {
        // "Bad initial values" ) );
        // "TRtpRecvHeader::TRtpRecvHeader() - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
    else
        {
        EUNIT_ASSERT(ETrue);
        }

    // "") );
    // "CRtpUtRtpHeader: returning %d" ), result );
    // "CRtpUtRtpHeader: RunTest completed") );

    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_TRtpHeaderExtension,
    "TRtpHeaderExtension.",
    "UNIT" )

EUNIT_TEST(
    "TRtpHeaderExtension - test ",
    "TRtpHeaderExtension",
    "TRtpHeaderExtension",
    "FUNCTIONALITY",
    SetupL, UT_TRtpHeaderExtension_TRtpHeaderExtensionL, Teardown)


EUNIT_END_TEST_TABLE

//  END OF FILE
