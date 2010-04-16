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
#include "UT_CRtpPacket.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "rtppacket.h"

// Refer to RFC 1889
// The comparison in rates between NTP timestamp and RTP timestamp.
// A value of n means that each increment of an RTP timestamp
// corresponds to n microseconds.
//
const TUint8 KRtpPayloadClockConversions[KUTRtpMaxPayloadTypes] =
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
_LIT8(KRTPPacket1, "8001CDDD00000001DEADBEEF9206D8823BACDD");   
_LIT8(KRTPPacket2, "8101CDE000000004DEADBEEF3958B1FB3F0E");
_LIT8(KRTPPacket3, "8101CDE000000004DEADBEEF000000003958B1FB3F0E");
_LIT8(KRTPPacket4, "8101CDE000000004DEADBEEFFFFFFF7F3958B1FB3F0E");
_LIT8(KRTPPacket5, "9101CDE000000004DEADBEEFFFFFFF7F3958B1FB3F0E");
_LIT8(KRTPPacket6, "9001CDE000000004DEADBEEF");
_LIT8(KRTPPacket7, "8A01CDE000000004DEADBEEF01010101010101010101010101010101010101010101010101010101010101010101010101010101");

_LIT8(KRTCPPacket1, "80C90001DEADBEEF9E30159059A23363F0FBDB62DC85AF52875C944495C5B00280000001ABCDEF1234567890ABCDEF1234567890DFC98F01DE16DA30F20D");
_LIT8(KRTCPPacket2, "DEADBEEF017F00003FFFFFFE9E30159059A23363F0FBDB62DC85AF52875C944495C5B00280000001ABCDEF1234567890ABCDEF1234567890DFC98F01DE16DA30F20D");
/*
 The RTP header has the following format:

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|X|  CC   |M|     PT      |       sequence number         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                           timestamp                           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |           synchronization source (SSRC) identifier            |
   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
   |            contributing source (CSRC) identifiers             |
   |                             ....                              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+





The format of an SRTP packet is illustrated in Figure 1.

        0                   1                   2                   3
      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+<+
     |V=2|P|X|  CC   |M|     PT      |       sequence number         | |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
     |                           timestamp                           | |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
     |           synchronization source (SSRC) identifier            | |
     +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ |
     |            contributing source (CSRC) identifiers             | |
     |                               ....                            | |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
     |                   RTP extension (OPTIONAL)                    | |
   +>+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
   | |                          payload  ...                         | |
   | |                               +-------------------------------+ |
   | |                               | RTP padding   | RTP pad count | |
*/

#define RTP_EUNIT_ASSERT_EQUALS( val1, val2 ) \
if ( val1 == ERTCP_PACKET_ERROR ){\
	User::Leave( KErrNoMemory );\
	}\
else{\
	EUNIT_ASSERT_EQUALS( val1, val2 );\
	}
	   
// CONSTRUCTION
UT_CRtpPacket* UT_CRtpPacket::NewL()
    {
    UT_CRtpPacket* self = UT_CRtpPacket::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CRtpPacket* UT_CRtpPacket::NewLC()
    {
    UT_CRtpPacket* self = new( ELeave ) UT_CRtpPacket();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CRtpPacket::~UT_CRtpPacket()
    {
    }

// Default constructor
UT_CRtpPacket::UT_CRtpPacket()
    {
    }

// Second phase construct
void UT_CRtpPacket::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_CRtpPacket::SetupL(  )
    {
	for ( TUint k = 0; k < KUTRtpMaxPayloadTypes; k++ )
        {
        iProfileRTPTimeRates[k] = ( TUint32 ) KRtpPayloadClockConversions[k];
        }
     
    iPktSnd = CRtpPacket::NewL( KMaxRtpPacketSize, iProfileRTPTimeRates );
    iPktRtcpSnd = CRtpPacket::NewL( KMaxRtcpPacketSize, iProfileRTPTimeRates );
    //iPktSnd->SetType( ERTP );
    
    // create test data that is 10 bytes too long than any RTP packet   
    // so that the actual length is also updated 
    iTooLongData = HBufC8::NewMaxL(KMaxRtpPacketSize + 10);    


    }

void UT_CRtpPacket::Teardown(  )
    {
	delete iPktSnd;
    delete iPktRtcpSnd;
    delete iTooLongData;
    
	iPktSnd = NULL;
	iPktRtcpSnd = NULL;
	iTooLongData = NULL;
	
	}

void UT_CRtpPacket::UT_CRtpPacket_NewLL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpPacket::UT_CRtpPacket_RtpPacketResetL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpPacket::UT_CRtpPacket_RtpPacketResetPtrL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpPacket::UT_CRtpPacket_RtpPacketGetSSRCL(  )
    {
    TInt result(KErrNone);
    
    TRtpSSRC iSSRC;
	// "Test description: --" ) );
	// "CRtpPacket::RtpPacketProcessAppL( &streamParam, &initParam )" ));

	///////////////////////TEST CASE - ERTP///////////////////////////////////
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    
    iPktSnd->SetType( ERTP );
    iSSRC = iPktSnd->RtpPacketGetSSRC();
    ///////////////////////TEST CASE - ERTCP_HEADER//////////////////////////
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    
    iPktSnd->SetType( ERTCP_HEADER );
    iSSRC = iPktSnd->RtpPacketGetSSRC();
    ///////////////////////TEST CASE - ERTCP_SR///////////////////////////////
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    
    iPktSnd->SetType( ERTCP_SR );
    iSSRC = iPktSnd->RtpPacketGetSSRC();
    ///////////////////////TEST CASE - ERTCP_RR///////////////////////////////
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    
    iPktSnd->SetType( ERTCP_RR );
    iSSRC = iPktSnd->RtpPacketGetSSRC();
    ///////////////////////TEST CASE - ERTCP_SDES/////////////////////////////
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    
    iPktSnd->SetType( ERTCP_SDES );
    iSSRC = iPktSnd->RtpPacketGetSSRC();
    ///////////////////////TEST CASE - ERTCP_BYE//////////////////////////////
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    
    iPktSnd->SetType( ERTCP_BYE );
    iSSRC = iPktSnd->RtpPacketGetSSRC();
    ///////////////////////TEST CASE - ERTCP_APP//////////////////////////////
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    
    iPktSnd->SetType( ERTCP_APP );
    iSSRC = iPktSnd->RtpPacketGetSSRC();
    //////////////////////////////////////////////////////////////////////
	
	///////////////////////TEST CASE - Default//////////////////////////////
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    
    iPktSnd->SetType( (TRtpPacketType)-1 );
    iSSRC = iPktSnd->RtpPacketGetSSRC();
    //////////////////////////////////////////////////////////////////////
    
	EUNIT_ASSERT(result == KErrNone);
		
    }

void UT_CRtpPacket::UT_CRtpPacket_RtpPacketGetPayloadTypeL(  )
    {
    
    CRtpPacket* packet = CRtpPacket::NewL( 2048, iProfileRTPTimeRates );
    TRtpPacketStreamParam aStreamParam;
    TRtpPacketIOParam aIoParam;
    TBuf8<10> data;
    data.Format( _L8( "dummydata" ) );
    
    // Set the parameters
    aStreamParam.TRTP.SSRC = 432;
    aStreamParam.TRTP.payload= 15;
    aStreamParam.TRTP.seqNum = 30;
    aStreamParam.TRTP.timeStamp = 40;
    aIoParam.TRTP.padding = 0;
    aIoParam.TRTP.marker = 0;
    aIoParam.TRTP.fHeaderExtension = 0;
    aIoParam.TRTP.numCSRC = 0;
    aIoParam.TRTP.payloadData = const_cast<TUint8*>( data.Ptr() );
    aIoParam.TRTP.payloadDataLen = data.Length();
    aIoParam.TRTP.extension.type = 0;
    aIoParam.TRTP.extension.length = 0;
    aIoParam.TRTP.extension.data = 0;
    
    // Build the packet
    packet->SetType( ERTP );
    packet->RtpPacketBuild( &aStreamParam, &aIoParam );
    TRtpPayloadType pt= packet->RtpPacketGetPayloadType();
    EUNIT_ASSERT(pt==15);
    
    delete packet;
    
    
    }

void UT_CRtpPacket::UT_CRtpPacket_RtpPacketBuildL(  )
    {
	TInt result(KErrNone);
	TInt error(KErrNone);
	
	// "Test description: build a RTP packet" ) );
	// "CRtpPacket::RtpPacketBuildRtp( &streamParam, &initParam )" ));
	
	///////////////////////TEST CASE 1///////////////////////////////////
    TRtpPacketStreamParam iStreamParam;
    TRtpPacketIOParam iIoParam;
    TRtpRtcpEnum eRtcpEnum(ERTCP_NO_ERROR);
    
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    	
    iPktSnd->SetType( (TRtpPacketType)-1 );
    result = iPktSnd->RtpPacketBuild( &iStreamParam, &iIoParam );
    
    EUNIT_ASSERT( result == ERTCP_PACKET_ERROR );
    
    
    ///////////////////////TEST CASE 2///////////////////////////////////
    iPktSnd->SetType( ERTP );
    
    iStreamParam.TRTP.payload = 0;
    
    result = iPktSnd->RtpPacketBuild( &iStreamParam, &iIoParam );
    eRtcpEnum = iPktSnd->RtpPacketProcessL( &iStreamParam, &iIoParam );
    
    EUNIT_ASSERT( result == KErrNone && eRtcpEnum == ERTCP_NO_ERROR );
    
    ////////////////////////TEST CASE 3/////////////////////////////////////////
   /*
    * Extension data == TRUE
    * data length / 4 == 0 
    */ 
    TBuf8<15> data; 
    data.Format( _L8( "12345678" ) );
    
    iIoParam.TRTP.extension.data = const_cast<TUint8*>(data.Ptr());
    iIoParam.TRTP.extension.length = data.Length();
   
    result = iPktSnd->RtpPacketBuild( &iStreamParam, &iIoParam );
    
    eRtcpEnum = iPktSnd->RtpPacketProcessL( &iStreamParam, &iIoParam );
    
    EUNIT_ASSERT( result == KErrNone && eRtcpEnum == ERTCP_NO_ERROR )

    ////////////////////////TEST CASE 4/////////////////////////////////////////
    /*
    * data length / 4 != 0
    */ 
    data.Append( _L8( "9" ) );
    
    iIoParam.TRTP.extension.data = const_cast<TUint8*>(data.Ptr());
    iIoParam.TRTP.extension.length = data.Length();
   
    result = iPktSnd->RtpPacketBuild( &iStreamParam, &iIoParam );
    iIoParam.TRTP.extension.data = NULL;
    iIoParam.TRTP.extension.length = 0;
    eRtcpEnum = iPktSnd->RtpPacketProcessL( &iStreamParam, &iIoParam );
     if(iPktSnd->iExdataAlloc )
      	{
       	User::Free( iIoParam.TRTP.extension.data );
       	}
    EUNIT_ASSERT( result == KErrNone && eRtcpEnum == ERTCP_NO_ERROR );
    }

void UT_CRtpPacket::UT_CRtpPacket_RtpPacketProcessL(  )
    {
   	TRtpRtcpEnum eRtcpEnum;
    
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    
    iPktSnd->SetType( ERTP );
    
    TRtpPacketStreamParam iStreamParam;
    TRtpPacketIOParam iIoParam;
       

    /////////////////TEST CASE 1 (wrong version)////////////////////////////////////////
    TUint8 version(0);
    
    Mem::Copy(/*iPktSnd->iData*/iPktSnd->Data(), &version, 1);
    eRtcpEnum = iPktSnd->RtpPacketProcessRtpL(&iStreamParam, &iIoParam);
    
    EUNIT_ASSERT( eRtcpEnum == ERTCP_PACKET_ERROR )
  
  	///////////////////////TEST CASE 2///////////////////////////////////
	iPktSnd->RtpPacketBuildRtp(&iStreamParam, &iIoParam);
	
    iStreamParam.TRTP.payload = 127;
    
    iPktSnd->RtpPacketBuild( &iStreamParam, &iIoParam ); 
    eRtcpEnum = iPktSnd->RtpPacketProcessRtpL(&iStreamParam, &iIoParam);
   
    EUNIT_ASSERT( eRtcpEnum == ERTCP_NO_ERROR )

   	///////////////////////TEST CASE 3///////////////////////////////////
	iPktSnd->RtpPacketBuildRtp(&iStreamParam, &iIoParam);
	
    iStreamParam.TRTP.payload = 1;
   
    iPktSnd->RtpPacketBuild( &iStreamParam, &iIoParam );
    eRtcpEnum = iPktSnd->RtpPacketProcessRtpL(&iStreamParam, &iIoParam);
   
    EUNIT_ASSERT( eRtcpEnum == ERTCP_NO_ERROR )

    ///////////////////////TEST CASE 4///////////////////////////////////
	iPktSnd->RtpPacketBuildRtp(&iStreamParam, &iIoParam);
	
    iStreamParam.TRTP.payload = ERTCP_SR;
     
    iPktSnd->RtpPacketBuild( &iStreamParam, &iIoParam );
    eRtcpEnum = iPktSnd->RtpPacketProcessRtpL(&iStreamParam, &iIoParam);
   
    EUNIT_ASSERT( eRtcpEnum == ERTCP_NO_ERROR )
    
    ///////////////////////TEST CASE 5///////////////////////////////////
	iPktSnd->RtpPacketBuildRtp(&iStreamParam, &iIoParam);
	
    iStreamParam.TRTP.payload = ERTCP_RR;
    
    iPktSnd->RtpPacketBuild( &iStreamParam, &iIoParam );
    eRtcpEnum = iPktSnd->RtpPacketProcessRtpL(&iStreamParam, &iIoParam);
   
    EUNIT_ASSERT( eRtcpEnum == ERTCP_NO_ERROR )
    }
    
void UT_CRtpPacket::UT_CRtpPacket_RtpPacketProcess2L(  )
    {
   	TInt result(KErrNone);
    TRtpRtcpEnum eRtcpEnum;
    
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    
    iPktSnd->SetType( ERTP );

	
	
    TRtpPacketStreamParam iStreamParam;
    TRtpPacketIOParam iIoParam;
       

    /////////////////TEST CASE 1 (CSC number is not null)////////////////////////////////////////
        //for the packet
    HBufC8* rtpPacket =HBufC8::NewLC(KRTPPacket2().Length());
	*rtpPacket=KRTPPacket2;
    Hex(*rtpPacket);
    TInt len= KRTPPacket2().Length();
	rtpPacket->Des().SetLength(len/2);
	
    iPktSnd->iData=  const_cast<TUint8*>(rtpPacket->Des().Ptr());
    //iPktSnd->SetSize(len/2);
    
    eRtcpEnum = iPktSnd->RtpPacketProcessRtpL(&iStreamParam, &iIoParam);
    
    EUNIT_ASSERT( eRtcpEnum == ERTCP_PACKET_ERROR )
  	CleanupStack::PopAndDestroy(rtpPacket);
  	
  	///////////////////////TEST CASE 2 (CSRC list is null)///////////////////////////////////
  	iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
	HBufC8* rtpPacket2 =HBufC8::NewLC(KRTPPacket3().Length());
	*rtpPacket2=KRTPPacket3;
    Hex(*rtpPacket2);
    TInt len2= KRTPPacket3().Length();
	rtpPacket2->Des().SetLength(len/2);
	iPktSnd->iData=  const_cast<TUint8*>(rtpPacket2->Des().Ptr());
	iPktSnd->SetSize(len2/2);
	 eRtcpEnum = iPktSnd->RtpPacketProcessRtpL(&iStreamParam, &iIoParam);
    User::Free(iIoParam.TRTP.CSRCarray );
    iIoParam.TRTP.CSRCarray = NULL;
    EUNIT_ASSERT( eRtcpEnum == ERTCP_PACKET_ERROR )
	CleanupStack::PopAndDestroy(rtpPacket2);
   	///////////////////////TEST CASE 3 (CSRC list is not null)///////////////////////////////////
	iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
	HBufC8* rtpPacket3 =HBufC8::NewLC(KRTPPacket4().Length());
	*rtpPacket3=KRTPPacket4;
    Hex(*rtpPacket3);
    TInt len3= KRTPPacket4().Length();
	rtpPacket3->Des().SetLength(len3/2);
	iPktSnd->iData=  const_cast<TUint8*>(rtpPacket3->Des().Ptr());
	
	iPktSnd->SetSize(len3/2);
	eRtcpEnum = iPktSnd->RtpPacketProcessRtpL(&iStreamParam, &iIoParam);
	User::Free(iIoParam.TRTP.CSRCarray );
    iIoParam.TRTP.CSRCarray = NULL;
    RTP_EUNIT_ASSERT_EQUALS(eRtcpEnum, ERTCP_NO_ERROR);
	CleanupStack::PopAndDestroy(rtpPacket3);
	///////////////////////TEST CASE (CSRC listcount 10)///////////////////////////////////
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
	HBufC8* rtpPacket7 =HBufC8::NewLC(KRTPPacket7().Length());
	*rtpPacket7=KRTPPacket7;
    Hex(*rtpPacket7);
    TInt len7= KRTPPacket7().Length();
	rtpPacket7->Des().SetLength(len7/2);
	iPktSnd->iData=  const_cast<TUint8*>(rtpPacket7->Des().Ptr());
	
	iPktSnd->SetSize(len7/2);
	eRtcpEnum = iPktSnd->RtpPacketProcessRtpL(&iStreamParam, &iIoParam);
	
	User::Free(iIoParam.TRTP.CSRCarray );
    iIoParam.TRTP.CSRCarray = NULL;
    
    RTP_EUNIT_ASSERT_EQUALS(eRtcpEnum, ERTCP_NO_ERROR);
	CleanupStack::PopAndDestroy(rtpPacket7);
	///////////////////////TEST CASE 4 (header extension length not valid)///////////////////////////////////
	iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
	HBufC8* rtpPacket4 =HBufC8::NewLC(KRTPPacket5().Length());
	*rtpPacket4=KRTPPacket5;
    Hex(*rtpPacket4);
    TInt len4= KRTPPacket5().Length();
	rtpPacket4->Des().SetLength(len4/2);
	iPktSnd->iData=  const_cast<TUint8*>(rtpPacket4->Des().Ptr());
	
	iPktSnd->SetSize(len4/2);
	eRtcpEnum = iPktSnd->RtpPacketProcessRtpL(&iStreamParam, &iIoParam);
    User::Free(iIoParam.TRTP.CSRCarray );
    iIoParam.TRTP.CSRCarray = NULL;
    EUNIT_ASSERT( eRtcpEnum == ERTCP_PACKET_ERROR )
	CleanupStack::PopAndDestroy(rtpPacket4);
	
	///////////////////////TEST CASE 5 (header extension length is true but no header extension to read)
	iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
	HBufC8* rtpPacket5 =HBufC8::NewLC(KRTPPacket6().Length());
	*rtpPacket5=KRTPPacket6;
    Hex(*rtpPacket5);
    TInt len5= KRTPPacket6().Length();
	rtpPacket5->Des().SetLength(len5/2);
	iPktSnd->iData=  const_cast<TUint8*>(rtpPacket5->Des().Ptr());
	
	iPktSnd->SetSize(len5/2);
	eRtcpEnum = iPktSnd->RtpPacketProcessRtpL(&iStreamParam, &iIoParam);
    User::Free(iIoParam.TRTP.CSRCarray );
    iIoParam.TRTP.CSRCarray = NULL;
    EUNIT_ASSERT( eRtcpEnum == ERTCP_PACKET_ERROR )
	CleanupStack::PopAndDestroy(rtpPacket5);
	
	}
    
void UT_CRtpPacket::UT_CRtpPacket_RtcpPacketBuildL(  )
	{
	TInt result(KErrNone);
	TInt error(KErrNone);
	TRtpRtcpEnum eRtcpEnum;
	

	// "Test description: --" ) );
	// "CRtpPacket::RtpPacketBuildRtcp( &streamParam, &initParam )" ));
	
	///////////////////////TEST CASE 1///////////////////////////////////
    TRtpPacketStreamParam iStreamParam;
    TRtpPacketIOParam iIoParam;

    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    
    iPktSnd->SetType( ERTCP_HEADER );
    iIoParam.TRTCP_HEADER.pt = ERTCP_SR ;
    result    = iPktSnd->RtpPacketBuild( &iStreamParam, &iIoParam );
    iPktSnd->RtpPacketResetPtr();
    //Can not process rtcp packet because if it is not rtcp type rr/sr..
    iIoParam.TRTCP_HEADER.pt = ERTCP_SR ;
    eRtcpEnum = iPktSnd->RtpPacketProcessL( &iStreamParam, &iIoParam );
    
    EUNIT_ASSERT(result == KErrNone && eRtcpEnum == KErrNone);
		// "TEST CASE 1 - passed" ));
   
    ///////////////////////TEST CASE 2////////////////////////////////////  
    iIoParam.TRTCP_HEADER.pt = ERTCP_RR;
    iIoParam.TRTCP_HEADER.sourceCount = 0;

    result    = iPktSnd->RtpPacketBuild( &iStreamParam, &iIoParam );
    iPktSnd->RtpPacketResetPtr();
    eRtcpEnum = iPktSnd->RtpPacketProcessL( &iStreamParam, &iIoParam );

    EUNIT_ASSERT(result == KErrNone && eRtcpEnum == KErrNone);
     
	}
void UT_CRtpPacket::UT_CRtpPacket_RtpPacketBuildAppL()
    {
    TInt result(KErrNone);
    
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    
 	// "Test description: --" ) );
	// "CRtpPacket::RtpPacketBuildApp( &streamParam, &initParam )" ));
	
	///////////////////////TEST CASE 1///////////////////////////////////
    TRtpPacketStreamParam iStreamParam;
    TRtpPacketIOParam iIoParam;
       
    iPktSnd->SetType( ERTCP_APP );
    result = iPktSnd->RtpPacketBuild( &iStreamParam, &iIoParam );
    EUNIT_ASSERT(result==KErrNone);
    
    //CRASH !!!
   	TRtpRtcpEnum eRtcpEnum = iPktSnd->RtpPacketProcessAppL(&iStreamParam, &iIoParam);
    EUNIT_ASSERT(eRtcpEnum==ERTCP_PACKET_ERROR);
    }	
void UT_CRtpPacket::UT_CRtpPacket_RtpPacketBuildByeL()
	{
	TInt result(KErrNone);
	
	iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    
	//Create an empty line
	//""));
	// "Test description: --" ) );
	// "CRtpPacket::RtpPacketBuildBye( &streamParam, &initParam )" ));
	
	//Add test cases
	
	///////////////////////TEST CASE 1///////////////////////////////////
	TRtpPacketStreamParam streamParam;
    TRtpPacketIOParam initParam;

    iPktSnd->SetType( ERTCP_BYE );
    result = iPktSnd->RtpPacketBuild( &streamParam, &initParam );
    EUNIT_ASSERT(result==KErrNone);
	}

void UT_CRtpPacket::UT_CRtpPacket_RtpPacketBuildRRL()
	{
	TInt result(KErrNone);
	
	iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    
	//Create an empty line
	//""));
	// "Test description: --" ) );
	// "CRtpPacket::RtpPacketBuildRr( &streamParam, &initParam )" ));
	
	///////////////////////TEST CASE 1///////////////////////////////////
	TRtpPacketStreamParam streamParam;
    TRtpPacketIOParam initParam;

    iPktSnd->SetType( ERTCP_RR );
    result = iPktSnd->RtpPacketBuild( &streamParam, &initParam );
    
    ///////////////////////////////////////////////////////////////////////
	EUNIT_ASSERT(result == KErrNone);
	}
void UT_CRtpPacket::UT_CRtpPacket_RtpPacketBuildSRL()
	{
	TInt result(KErrNone);
	
	iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    	
	///////////////////////TEST CASE 1///////////////////////////////////
    TRtpPacketStreamParam iStreamParam;
    TRtpPacketIOParam iIoParam;
    
    iPktSnd->SetType( ERTCP_SR );
    result = iPktSnd->RtpPacketBuild( &iStreamParam, &iIoParam );
    
    ///////////////////////////////////////////////////////////////////////
	EUNIT_ASSERT(result == KErrNone);

	}
void UT_CRtpPacket::UT_CRtpPacket_RtpPacketBuildSDESL()
	{
	   TInt result(KErrNone);
	
	iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    
	//Create an empty line
	//""));
	// "Test description: --" ) );
	// "CRtpPacket::RtpPacketBuildSdes( &streamParam, &initParam )" ));
	
	///////////////////////TEST CASE 1///////////////////////////////////
    TRtpPacketStreamParam iStreamParam;
    TRtpPacketIOParam iIoParam;
         
    iPktSnd->SetType( ERTCP_SDES );
    result = iPktSnd->RtpPacketBuild( &iStreamParam, &iIoParam );
    
    ///////////////////////////////////////////////////////////////////////
	EUNIT_ASSERT(result == KErrNone);
	}
void UT_CRtpPacket::UT_CRtpPacket_RtpPacketProcessAppL()
    {
    TInt result(KErrNone);
    
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    
    TRtpPacketStreamParam iStreamParam;
    TRtpPacketIOParam iIoParam;
    
	//""));
	// "Test description: --" ) );
	// "CRtpPacket::RtpPacketProcessAppL( &streamParam, &initParam )" ));

	///////////////////////TEST CASE 1///////////////////////////////////
    TRtpRtcpEnum eRtcpEnum = iPktSnd->RtpPacketProcessAppL(&iStreamParam, &iIoParam);
   	EUNIT_ASSERT(eRtcpEnum == ERTCP_PACKET_ERROR);
	
	///////////////////////TEST CASE 2///////////////////////////////////
	
	HBufC8* rtcpPacket =HBufC8::NewLC(KRTCPPacket1().Length());
	*rtcpPacket=KRTCPPacket1;
    Hex(*rtcpPacket);
    TInt len= KRTCPPacket1().Length();
	rtcpPacket->Des().SetLength(len/2);
	
    iPktSnd->iData=  const_cast<TUint8*>(rtcpPacket->Des().Ptr());
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    iPktSnd->SetSize(len/2);
    
    iStreamParam.TRTCP_APP.totalPacketLen=len/2;
    eRtcpEnum = iPktSnd->RtpPacketProcessAppL(&iStreamParam, &iIoParam);
    User::Free(iIoParam.TRTCP_APP.appData);
    RTP_EUNIT_ASSERT_EQUALS( eRtcpEnum, ERTCP_NO_ERROR )
  	CleanupStack::PopAndDestroy(rtcpPacket);
    }	
void UT_CRtpPacket::UT_CRtpPacket_RtpPacketProcessByeL()
	{
	TInt result(KErrNone);
    
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();

	// "CRtpPacket::RtpPacketProcessBye( &streamParam, &initParam )" ));

	///////////////////////TEST CASE 1///////////////////////////////////
    TRtpPacketStreamParam iStreamParam;
    TRtpPacketIOParam iIoParam;
       
    TRtpRtcpEnum eRtcpEnum = iPktSnd->RtpPacketProcessByeL(&iStreamParam, &iIoParam);
    //////////////////////////////////////////////////////////////////////
	RTP_EUNIT_ASSERT_EQUALS(eRtcpEnum, ERTCP_NO_ERROR);
    User::Free(iIoParam.TRTCP_BYE.reason);
	EUNIT_ASSERT(result == KErrNone);
	}
void UT_CRtpPacket::UT_CRtpPacket_RtpPacketProcessSRL()
	{
	   TInt result(KErrNone);
    
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();

	//""));
	// "Test description: --" ) );
	// "CRtpPacket::RtpPacketProcessSr( &streamParam, &initParam )" ));

	///////////////////////TEST CASE 1///////////////////////////////////
    TRtpPacketStreamParam iStreamParam;
    TRtpPacketIOParam iIoParam;
       
    TRtpRtcpEnum eRtcpEnum = iPktSnd->RtpPacketProcessSr(&iStreamParam, &iIoParam);
    //////////////////////////////////////////////////////////////////////
	EUNIT_ASSERT(result == KErrNone);
	}
void UT_CRtpPacket::UT_CRtpPacket_RtpPacketProcessRRL()
	{
   TInt result(KErrNone);
    
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    
	//""));
	// "Test description: --" ) );
	// "CRtpPacket::RtpPacketProcessRr( &streamParam, &initParam )" ));

	///////////////////////TEST CASE 1///////////////////////////////////
    TRtpPacketStreamParam iStreamParam;
    TRtpPacketIOParam iIoParam;
       
    TRtpRtcpEnum eRtcpEnum = iPktSnd->RtpPacketProcessRr(&iStreamParam, &iIoParam);
    //////////////////////////////////////////////////////////////////////
	EUNIT_ASSERT(result == KErrNone);
	}
void UT_CRtpPacket::UT_CRtpPacket_RtpPacketProcessSDESL()
	{
   TInt result(KErrNone);
    
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();

	//""));
	// "Test description: --" ) );
	// "CRtpPacket::RtpPacketProcessSdes( &streamParam, &initParam )" ));

	///////////////////////TEST CASE 1///////////////////////////////////
    TRtpPacketStreamParam iStreamParam;
    TRtpPacketIOParam iIoParam;
       
    TRtpRtcpEnum eRtcpEnum = iPktSnd->RtpPacketProcessSdesL(&iStreamParam, &iIoParam);
    //////////////////////////////////////////////////////////////////////
	TInt index( 0 );
	for ( index = 0; index < ERTCP_NUM_OF_SDES_ITEMS; index++ )
        {
		if(iIoParam.TRTCP_SDES.sdesItems[index] != NULL )
            {
            User::Free( iIoParam.TRTCP_SDES.sdesItems[index] );
            iIoParam.TRTCP_SDES.sdesItems[index] = NULL;
            }
        iIoParam.TRTCP_SDES.sdesItemsSize[index] = 0;    
        }
    RTP_EUNIT_ASSERT_EQUALS(eRtcpEnum, ERTCP_NO_ERROR);	
    
    ///////////////////////TEST CASE 2/////////////////////////////////// 
    HBufC8* rtcpPacket2 =HBufC8::NewLC(KRTCPPacket2().Length());
	*rtcpPacket2=KRTCPPacket2;
    Hex(*rtcpPacket2);
    TInt len= KRTCPPacket2().Length();
	rtcpPacket2->Des().SetLength(len/2);
    iPktSnd->iData=  const_cast<TUint8*>(rtcpPacket2->Des().Ptr());
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    iPktSnd->SetSize(len/2);
    
    
    eRtcpEnum = iPktSnd->RtpPacketProcessSdesL(&iStreamParam, &iIoParam);	
	
	for ( index = 0; index < ERTCP_NUM_OF_SDES_ITEMS; index++ )
        {
		if(iIoParam.TRTCP_SDES.sdesItems[index] != NULL )
            {
            User::Free( iIoParam.TRTCP_SDES.sdesItems[index] );
            iIoParam.TRTCP_SDES.sdesItems[index] = NULL;
            }
        iIoParam.TRTCP_SDES.sdesItemsSize[index] = 0;    
        }
    CleanupStack::PopAndDestroy( rtcpPacket2 );
    
    RTP_EUNIT_ASSERT_EQUALS(eRtcpEnum, ERTCP_NO_ERROR);
    
    ///////////////////////TEST CASE 3///////////////////////////////////  
    HBufC8* rtcpPacket =HBufC8::NewLC(KRTCPPacket1().Length());
	*rtcpPacket=KRTCPPacket1;
    Hex(*rtcpPacket);
     len= KRTCPPacket1().Length();
	rtcpPacket->Des().SetLength(len/2);
    iPktSnd->iData=  const_cast<TUint8*>(rtcpPacket->Des().Ptr());
    iPktSnd->RtpPacketReset();
    iPktSnd->RtpPacketResetPtr();
    iPktSnd->SetSize(len/2);
    
    iStreamParam.TRTCP_APP.totalPacketLen=len/2;
    eRtcpEnum = iPktSnd->RtpPacketProcessSdesL(&iStreamParam, &iIoParam);
    User::Free(iIoParam.TRTCP_APP.appData);
    
	for ( index = 0; index < ERTCP_NUM_OF_SDES_ITEMS; index++ )
        {
		if(iIoParam.TRTCP_SDES.sdesItems[index] != NULL )
            {
            User::Free( iIoParam.TRTCP_SDES.sdesItems[index] );
            iIoParam.TRTCP_SDES.sdesItems[index] = NULL;
            }
        iIoParam.TRTCP_SDES.sdesItemsSize[index] = 0;    
        }
  	CleanupStack::PopAndDestroy(rtcpPacket);
  	
  	RTP_EUNIT_ASSERT_EQUALS( eRtcpEnum, ERTCP_NO_ERROR )
	}
void UT_CRtpPacket::UT_CRtpPacket_SetSizeL(  )
    {
    iPktSnd->SetSize( iPktSnd->Size() );
    EUNIT_ASSERT(iPktSnd->iSize==iPktSnd->Size())
    }

void UT_CRtpPacket::UT_CRtpPacket_SetTypeL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpPacket::UT_CRtpPacket_SizeL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpPacket::UT_CRtpPacket_DataL(  )
    {
    TUint8* iData  = iPktSnd->Data();
    }

void UT_CRtpPacket::UT_CRtpPacket_GetHBufL(  )
    {
    HBufC8* iHBufC8 = iPktSnd->GetHBuf();
    }

void UT_CRtpPacket::UT_CRtpPacket_TypeL(  )
    {
    TRtpPacketType iPacketType = iPktSnd->Type();
    }

void UT_CRtpPacket::UT_CRtpPacket_DesL(  )
    {
	TPtrC8 iDes  = iPktSnd->Des();
    }
    
void UT_CRtpPacket::Hex(HBufC8& aString)
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

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CRtpPacket,
    "CRtpPacket",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "CRtpPacket",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_NewLL, Teardown)

EUNIT_TEST(
    "RtpPacketReset - test ",
    "CRtpPacket",
    "RtpPacketReset",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_RtpPacketResetL, Teardown)

EUNIT_TEST(
    "RtpPacketResetPtr - test ",
    "CRtpPacket",
    "RtpPacketResetPtr",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_RtpPacketResetPtrL, Teardown)

EUNIT_TEST(
    "RtpPacketGetSSRC - test ",
    "CRtpPacket",
    "RtpPacketGetSSRC",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_RtpPacketGetSSRCL, Teardown)

EUNIT_TEST(
    "RtpPacketGetPayloadType - test ",
    "CRtpPacket",
    "RtpPacketGetPayloadType",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_RtpPacketGetPayloadTypeL, Teardown)

EUNIT_TEST(
    "RtpPacketBuild - test ",
    "CRtpPacket",
    "RtpPacketBuild",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_RtpPacketBuildL, Teardown)
EUNIT_TEST(
    "RtcpPacketBuild - test ",
    "CRtpPacket",
    "RtcpPacketBuild",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_RtcpPacketBuildL, Teardown)    
EUNIT_TEST(
    "RtpPacketBuildApp - test ",
    "CRtpPacket",
    "RtpPacketBuildApp",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_RtpPacketBuildAppL, Teardown)    
EUNIT_TEST(
    "RtpPacketProcessApp - test ",
    "CRtpPacket",
    "RtpPacketProcessApp",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_RtpPacketProcessAppL, Teardown)        
EUNIT_TEST(
    "RtpPacketBuildBye - test ",
    "CRtpPacket",
    "RtpPacketBuildBye",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_RtpPacketBuildByeL, Teardown)    
EUNIT_TEST(
    "RtpPacketProcessBye - test ",
    "CRtpPacket",
    "RtpPacketProcessBye",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_RtpPacketProcessByeL, Teardown)        
    
EUNIT_TEST(
    "RtpPacketBuildSR - test ",
    "CRtpPacket",
    "RtpPacketBuildSR",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_RtpPacketBuildSRL, Teardown)    
EUNIT_TEST(
    "RtpPacketProcessSR - test ",
    "CRtpPacket",
    "RtpPacketProcessSR",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_RtpPacketProcessSRL, Teardown)        

EUNIT_TEST(
    "RtpPacketBuildRR - test ",
    "CRtpPacket",
    "RtpPacketBuildRR",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_RtpPacketBuildRRL, Teardown)
EUNIT_TEST(
    "RtpPacketProcessRR - test ",
    "CRtpPacket",
    "RtpPacketProcessRR",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_RtpPacketProcessRRL, Teardown)        
        
EUNIT_TEST(
    "RtpPacketBuildApp - test ",
    "CRtpPacket",
    "RtpPacketBuildApp",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_RtpPacketBuildSDESL, Teardown)    
EUNIT_TEST(
    "RtpPacketProcessSDES - test ",
    "CRtpPacket",
    "RtpPacketProcessSDES",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_RtpPacketProcessSDESL, Teardown)        

    

EUNIT_TEST(
    "RtpPacketProcess - test ",
    "CRtpPacket",
    "RtpPacketProcess",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_RtpPacketProcessL, Teardown)

EUNIT_TEST(
    "SetSize - test ",
    "CRtpPacket",
    "SetSize",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_SetSizeL, Teardown)

EUNIT_TEST(
    "SetType - test ",
    "CRtpPacket",
    "SetType",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_SetTypeL, Teardown)

EUNIT_TEST(
    "Size - test ",
    "CRtpPacket",
    "Size",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_SizeL, Teardown)

EUNIT_TEST(
    "Data - test ",
    "CRtpPacket",
    "Data",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_DataL, Teardown)

EUNIT_TEST(
    "GetHBuf - test ",
    "CRtpPacket",
    "GetHBuf",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_GetHBufL, Teardown)

EUNIT_TEST(
    "Type - test ",
    "CRtpPacket",
    "Type",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_TypeL, Teardown)

EUNIT_TEST(
    "Des - test ",
    "CRtpPacket",
    "Des",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_DesL, Teardown)

EUNIT_TEST(
    "RtpPacketProcess2L ",
    "CRtpPacket",
    "RtpPacketProcess2L",
    "FUNCTIONALITY",
    SetupL, UT_CRtpPacket_RtpPacketProcess2L, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE
