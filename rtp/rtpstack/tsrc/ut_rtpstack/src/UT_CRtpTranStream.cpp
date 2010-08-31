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
#include "UT_CRtpTranStream.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "rtptranstream.h"

const TUint8 KRtpPayloadClockConversions[KRtpMaxPayloadTypes] =
    {
    125, 125, 125, 125, 125, 125, 63, 125, 125, 63, // 0-9 
    23, 23, 125, 125, 11, 125, 91, 45, 125, 0,      // 10-19
    0, 0, 0, 0, 0, 11, 11, 0, 11, 0,                // 20-29
    0, 11, 11, 11, 11, 0, 0, 0, 0, 0,               // 30-39
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 40-49
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 50-59
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 60-69
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 70-79
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 80-89
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 90-99
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 100-109
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 110-119
    0, 0, 0, 0, 0, 0, 0, 0                          // 120-127
    };
    
const TInt KExtType( 2 );
const TInt KExtLength( 4 );
_LIT8( KExtData, "extD\0" ); 

_LIT8(KRTCPPacket1, "80C90001DEADBEEF9E30159059A23363F0FBDB62DC85AF52875C944495C5B00280000001ABCDEF1234567890ABCDEF1234567890DFC98F01DE16DA30F20D");
_LIT8(KRTCPPacket2, "80C80006DEADBEEFC7B1ACD423FE000000E12F18000000010000020081CA0015DEADBEEF010A52747055492075736572020455736572030D7573657240686F6D652E6E6574040B2B313233343536373839300504486F6D65060B527470206578616D706C65070641637469766508010000000000");
_LIT8(KRTCPPacket3, "80C90001000000029E30159059A23363F0FBDB62DC85AF52875C944495C5B00280000001ABCDEF1234567890ABCDEF1234567890DFC98F01DE16DA30F20D");
_LIT8(KRTCPPacket4, "80CB0001000000029E30159059A23363F0FBDB62DC85AF52875C944495C5B00280000001");
   
// CONSTRUCTION
UT_CRtpTranStream* UT_CRtpTranStream::NewL()
    {
    UT_CRtpTranStream* self = UT_CRtpTranStream::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CRtpTranStream* UT_CRtpTranStream::NewLC()
    {
    UT_CRtpTranStream* self = new( ELeave ) UT_CRtpTranStream();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CRtpTranStream::~UT_CRtpTranStream()
    {
    }

// Default constructor
UT_CRtpTranStream::UT_CRtpTranStream()
    {
    }

// Second phase construct
void UT_CRtpTranStream::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

void UT_CRtpTranStream::Hex(HBufC8& aString)
{
    TPtr8 ptr=aString.Des();
    TInt length = aString.Length();
    if (aString.Length()%2)
        {
        ptr.SetLength(0);
        return;
        }
    TInt i;
    for (i=0;i<aString.Length();i+=2)
        {
        TUint8 tmp;
        tmp=(TUint8)(aString[i]-(aString[i]>'9'?('A'-10):'0'));
        tmp*=16;
        tmp|=(TUint8)(aString[i+1]-(aString[i+1]>'9'?('A'-10):'0'));
        ptr[i/2]=tmp;
        }
    ptr.SetLength(aString.Length()/2);

}

void UT_CRtpTranStream::SetupL(  )
    {
  	TRtpPayloadType payloadType( 0 );

    for ( TUint k = 0; k < KRtpMaxPayloadTypes; k++ )
        {
        iProfileRTPTimeRates[k] = ( TUint32 ) KRtpPayloadClockConversions[k];
        }

    iObs = this;
        
   
    iTranStream = CRtpTranStream::NewL( payloadType,
                                        1, // sessionId
                                        3, // transtreamid
                                        1000, // SSRC
                                        this,
                                        iProfileRTPTimeRates );
    iExpectingExtensionPacket = EFalse;
    }

void UT_CRtpTranStream::Teardown(  )
    {
 	delete iTranStream;
    iTranStream = NULL;
    }

void UT_CRtpTranStream::UT_CRtpTranStream_NewLL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpTranStream::UT_CRtpTranStream_BuildRtpPacketL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpTranStream::UT_CRtpTranStream_BuildRtcpBYEPacketL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpTranStream::UT_CRtpTranStream_BuildRtcpAPPPacketL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpTranStream::UT_CRtpTranStream_SentRTPPacketsL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpTranStream::UT_CRtpTranStream_SetSentRTPPacketsL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpTranStream::UT_CRtpTranStream_SentRtcpReportL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpTranStream::UT_CRtpTranStream_SetRtcpReportFlagL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpTranStream::UT_CRtpTranStream_ResetRtcpReportFlagL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpTranStream::UT_CRtpTranStream_SetBandwidthL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpTranStream::UT_CRtpTranStream_ResetStreamStatL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpTranStream::UT_CRtpTranStream_GetStreamStatL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpTranStream::UT_CRtpTranStream_RtpStreamProcessRtcpReportSectionL(  )
    {
    HBufC8* srtcpPacket =HBufC8::NewLC(KRTCPPacket2().Length());
	*srtcpPacket=KRTCPPacket2;
    Hex(*srtcpPacket);
    TInt len= KRTCPPacket2().Length();
	srtcpPacket->Des().SetLength(len/2);
	
	
	
    CRtpPacket* packet = CRtpPacket::NewL( 2048, iProfileRTPTimeRates );
    CleanupStack::PushL( packet );
    packet->SetSize(len/2);
	packet->iData=const_cast<TUint8*>( srtcpPacket->Des().Ptr() );
    iTranStream->RtpStreamProcessRtcpReportSectionL(packet);
    CleanupStack::PopAndDestroy(packet);
    CleanupStack::PopAndDestroy(srtcpPacket);
    
    HBufC8* srtcpPacket2 =HBufC8::NewLC(KRTCPPacket4().Length());
	*srtcpPacket2=KRTCPPacket4;
    Hex(*srtcpPacket2);
    TInt len2= KRTCPPacket4().Length();
	srtcpPacket2->Des().SetLength(len2);
	
    CRtpPacket* packet2 = CRtpPacket::NewL( 2048, iProfileRTPTimeRates );
    CleanupStack::PushL( packet2 );
    packet2->SetSize(135);
    packet2->iData=0;
	packet2->iDataPtr=const_cast<TUint8*>( srtcpPacket2->Des().Ptr() );
	packet2->SetType(ERTCP_SR);
    iTranStream->RtpStreamProcessRtcpReportSectionL(packet2);
    
    CleanupStack::PopAndDestroy(packet2);
    CleanupStack::PopAndDestroy(srtcpPacket2);
    }

void UT_CRtpTranStream::UT_CRtpTranStream_RtpStreamCreateRtcpReportSectionL(  )
    {
    EUNIT_ASSERT( ETrue );
    }
void UT_CRtpTranStream::RtpPacketReceived( TRtpId /*aStreamId*/, 
                                const TRtpRecvHeader& aHeaderInfo, 
                                const TDesC8& /*aPayloadData*/ )
	{

    if ( iExpectingExtensionPacket )
        {
        if ( aHeaderInfo.iExtension == 0 )
            {
            // "Extension indicator was 0" ));
            // "CRtpStream::RtpStreamProcessRtpPacket() - FAILED" ) );
            User::Panic( _L("CRtpUtStream"), KErrGeneral );
            }
        if ( aHeaderInfo.iHeaderExtension == NULL )
            {
            // "Header extension was null" ));
            // "CRtpStream::RtpStreamProcessRtpPacket() - FAILED" ) );
            User::Panic( _L("CRtpUtStream"), KErrGeneral );
            }
        if ( aHeaderInfo.iHeaderExtension->iType != KExtType ||
             aHeaderInfo.iHeaderExtension->iLength != KExtLength / 4 )
            {
            // "Invalid header extension values" ));
            // "CRtpStream::RtpStreamProcessRtpPacket() - FAILED" ) );
            User::Panic( _L("CRtpUtStream"), KErrGeneral );
            }                         
        TBuf8<KExtLength+1> tempBuf1;
        TBuf8<KExtLength+1> tempBuf2;
        TBuf8<1> zero;
        zero.Format( _L8( "%d"), 0 );
        tempBuf1.Format( _L8( "%s" ), aHeaderInfo.iHeaderExtension->iData );
        tempBuf2.Format( KExtData );
        tempBuf1.Replace( KExtLength, 1, zero);
        tempBuf2.Replace( KExtLength, 1, zero);
        if ( tempBuf1 != tempBuf2 )
            {
            // "Invalid header extension data" ));
            // "CRtpStream::RtpStreamProcessRtpPacket() - FAILED" ) );
            User::Panic( _L( "CRtpUtStream" ), KErrGeneral );
            }                         
        iExpectingExtensionPacket = EFalse;
        }
                                	
	}
void UT_CRtpTranStream::SdesReceived( TRtpSSRC /*aSSRC*/, const TRtpSdesParams& /*aParams*/ )
{
	
}
void UT_CRtpTranStream::ByeReceived( TRtpId /*aStreamId*/, TRtpSSRC /*aSSRC*/, 
								const TDesC8& /*aReason*/ )
{
	
}
void UT_CRtpTranStream::AppReceived( TRtpId /*aStreamId*/, TRtpSSRC /*aSSRC*/,
								 const TRtcpApp& /*aApp*/ )
{
	
}
void UT_CRtpTranStream::SrReceived( TRtpId /*aStreamId*/, 
							TRtpSSRC /*aSSRC*/, const TTimeStamps& /*aTimeStamps*/ )
{
	
}
void UT_CRtpTranStream::RrReceived( TRtpId /*aStreamId*/, TRtpSSRC /*aSSRC*/ )
{
	
}	
//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CRtpTranStream,
    "CRtpTranStream",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "CRtpTranStream",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpTranStream_NewLL, Teardown)

EUNIT_TEST(
    "BuildRtpPacket - test ",
    "CRtpTranStream",
    "BuildRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpTranStream_BuildRtpPacketL, Teardown)

EUNIT_TEST(
    "BuildRtcpBYEPacket - test ",
    "CRtpTranStream",
    "BuildRtcpBYEPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpTranStream_BuildRtcpBYEPacketL, Teardown)

EUNIT_TEST(
    "BuildRtcpAPPPacket - test ",
    "CRtpTranStream",
    "BuildRtcpAPPPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpTranStream_BuildRtcpAPPPacketL, Teardown)

EUNIT_TEST(
    "SentRTPPackets - test ",
    "CRtpTranStream",
    "SentRTPPackets",
    "FUNCTIONALITY",
    SetupL, UT_CRtpTranStream_SentRTPPacketsL, Teardown)

EUNIT_TEST(
    "SetSentRTPPackets - test ",
    "CRtpTranStream",
    "SetSentRTPPackets",
    "FUNCTIONALITY",
    SetupL, UT_CRtpTranStream_SetSentRTPPacketsL, Teardown)

EUNIT_TEST(
    "SentRtcpReport - test ",
    "CRtpTranStream",
    "SentRtcpReport",
    "FUNCTIONALITY",
    SetupL, UT_CRtpTranStream_SentRtcpReportL, Teardown)

EUNIT_TEST(
    "SetRtcpReportFlag - test ",
    "CRtpTranStream",
    "SetRtcpReportFlag",
    "FUNCTIONALITY",
    SetupL, UT_CRtpTranStream_SetRtcpReportFlagL, Teardown)

EUNIT_TEST(
    "ResetRtcpReportFlag - test ",
    "CRtpTranStream",
    "ResetRtcpReportFlag",
    "FUNCTIONALITY",
    SetupL, UT_CRtpTranStream_ResetRtcpReportFlagL, Teardown)

EUNIT_TEST(
    "SetBandwidth - test ",
    "CRtpTranStream",
    "SetBandwidth",
    "FUNCTIONALITY",
    SetupL, UT_CRtpTranStream_SetBandwidthL, Teardown)

EUNIT_TEST(
    "ResetStreamStat - test ",
    "CRtpTranStream",
    "ResetStreamStat",
    "FUNCTIONALITY",
    SetupL, UT_CRtpTranStream_ResetStreamStatL, Teardown)

EUNIT_TEST(
    "GetStreamStat - test ",
    "CRtpTranStream",
    "GetStreamStat",
    "FUNCTIONALITY",
    SetupL, UT_CRtpTranStream_GetStreamStatL, Teardown)

EUNIT_TEST(
    "RtpStreamProcessRtcpReportSection - test ",
    "CRtpTranStream",
    "RtpStreamProcessRtcpReportSection",
    "FUNCTIONALITY",
    SetupL, UT_CRtpTranStream_RtpStreamProcessRtcpReportSectionL, Teardown)

EUNIT_TEST(
    "RtpStreamCreateRtcpReportSection - test ",
    "CRtpTranStream",
    "RtpStreamCreateRtcpReportSection",
    "FUNCTIONALITY",
    SetupL, UT_CRtpTranStream_RtpStreamCreateRtcpReportSectionL, Teardown)


EUNIT_END_TEST_TABLE

//  END OF FILE
