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
#include "UT_CRtpRecvStream.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "rtprecvstream.h"

// CONSTRUCTION
UT_CRtpRecvStream* UT_CRtpRecvStream::NewL()
    {
    UT_CRtpRecvStream* self = UT_CRtpRecvStream::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CRtpRecvStream* UT_CRtpRecvStream::NewLC()
    {
    UT_CRtpRecvStream* self = new( ELeave ) UT_CRtpRecvStream();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CRtpRecvStream::~UT_CRtpRecvStream()
    {
    }

// Default constructor
UT_CRtpRecvStream::UT_CRtpRecvStream()
    {
    }

// Second phase construct
void UT_CRtpRecvStream::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_CRtpRecvStream::SetupL(  )
    {

    }

void UT_CRtpRecvStream::Teardown(  )
    {

    }

void UT_CRtpRecvStream::UT_CRtpRecvStream_NewLL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpRecvStream::UT_CRtpRecvStream_RtpStreamUpdateParamL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpRecvStream::UT_CRtpRecvStream_SetReceivedRTPPacketsL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpRecvStream::UT_CRtpRecvStream_RegisterRtpObserverL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpRecvStream::UT_CRtpRecvStream_UnRegisterRtpObserverL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpRecvStream::UT_CRtpRecvStream_RtpStreamProcessRtpPacketL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpRecvStream::UT_CRtpRecvStream_ReceivedRTPPacketsL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpRecvStream::UT_CRtpRecvStream_GetRemoteStreamInfoL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpRecvStream::UT_CRtpRecvStream_GetRemoteSSRCL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpRecvStream::UT_CRtpRecvStream_ResetStreamStatL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpRecvStream::UT_CRtpRecvStream_GetStreamStatL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpRecvStream::UT_CRtpRecvStream_RtpStreamProcessRtcpReportSectionL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpRecvStream::UT_CRtpRecvStream_RtpStreamCreateRtcpReportSectionL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CRtpRecvStream,
    "CRtpRecvStream.",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "CRtpRecvStream",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpRecvStream_NewLL, Teardown)

EUNIT_TEST(
    "RtpStreamUpdateParam - test ",
    "CRtpRecvStream",
    "RtpStreamUpdateParam",
    "FUNCTIONALITY",
    SetupL, UT_CRtpRecvStream_RtpStreamUpdateParamL, Teardown)

EUNIT_TEST(
    "SetReceivedRTPPackets - test ",
    "CRtpRecvStream",
    "SetReceivedRTPPackets",
    "FUNCTIONALITY",
    SetupL, UT_CRtpRecvStream_SetReceivedRTPPacketsL, Teardown)

EUNIT_TEST(
    "RegisterRtpObserver - test ",
    "CRtpRecvStream",
    "RegisterRtpObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpRecvStream_RegisterRtpObserverL, Teardown)

EUNIT_TEST(
    "UnRegisterRtpObserver - test ",
    "CRtpRecvStream",
    "UnRegisterRtpObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpRecvStream_UnRegisterRtpObserverL, Teardown)

EUNIT_TEST(
    "RtpStreamProcessRtpPacket - test ",
    "CRtpRecvStream",
    "RtpStreamProcessRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpRecvStream_RtpStreamProcessRtpPacketL, Teardown)

EUNIT_TEST(
    "ReceivedRTPPackets - test ",
    "CRtpRecvStream",
    "ReceivedRTPPackets",
    "FUNCTIONALITY",
    SetupL, UT_CRtpRecvStream_ReceivedRTPPacketsL, Teardown)

EUNIT_TEST(
    "GetRemoteStreamInfo - test ",
    "CRtpRecvStream",
    "GetRemoteStreamInfo",
    "FUNCTIONALITY",
    SetupL, UT_CRtpRecvStream_GetRemoteStreamInfoL, Teardown)

EUNIT_TEST(
    "GetRemoteSSRC - test ",
    "CRtpRecvStream",
    "GetRemoteSSRC",
    "FUNCTIONALITY",
    SetupL, UT_CRtpRecvStream_GetRemoteSSRCL, Teardown)

EUNIT_TEST(
    "ResetStreamStat - test ",
    "CRtpRecvStream",
    "ResetStreamStat",
    "FUNCTIONALITY",
    SetupL, UT_CRtpRecvStream_ResetStreamStatL, Teardown)

EUNIT_TEST(
    "GetStreamStat - test ",
    "CRtpRecvStream",
    "GetStreamStat",
    "FUNCTIONALITY",
    SetupL, UT_CRtpRecvStream_GetStreamStatL, Teardown)

EUNIT_TEST(
    "RtpStreamProcessRtcpReportSection - test ",
    "CRtpRecvStream",
    "RtpStreamProcessRtcpReportSection",
    "FUNCTIONALITY",
    SetupL, UT_CRtpRecvStream_RtpStreamProcessRtcpReportSectionL, Teardown)

EUNIT_TEST(
    "RtpStreamCreateRtcpReportSection - test ",
    "CRtpRecvStream",
    "RtpStreamCreateRtcpReportSection",
    "FUNCTIONALITY",
    SetupL, UT_CRtpRecvStream_RtpStreamCreateRtcpReportSectionL, Teardown)


EUNIT_END_TEST_TABLE

//  END OF FILE
