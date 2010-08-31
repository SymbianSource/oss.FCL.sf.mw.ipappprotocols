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
#include "UT_CRtpSession.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "rtpsession.h"
#include "rtppacketext.h"
#include "rtpcommsend.h"
const TUint KDefaultIap = 6;
//_LIT( KDefaultPath, "c:\\system\\data\\RtpStpPacket.dll" );
const TUint8 KRtpPayloadClockConversions[KRtpNumPayloadTypes] =
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
const TUint KUTRtpMaxPayloadTypes = 128; 

_LIT8(KRTCPPacket1, "80C90001DEADBEEF9E30159059A23363F0FBDB62DC85AF52875C944495C5B00280000001ABCDEF1234567890ABCDEF1234567890DFC98F01DE16DA30F20D");
_LIT8(KRTCPPacket2, "80C80006DEADBEEFC7B1ACD423FE000000E12F18000000010000020081CA0015DEADBEEF010A52747055492075736572020455736572030D7573657240686F6D652E6E6574040B2B313233343536373839300504486F6D65060B527470206578616D706C65070641637469766508010000000000");
_LIT8(KRTCPPacket3, "80C90001000000029E30159059A23363F0FBDB62DC85AF52875C944495C5B00280000001ABCDEF1234567890ABCDEF1234567890DFC98F01DE16DA30F20D");
_LIT8(KRTCPPacket4, "80CB0001000000029E30159059A23363F0FBDB62DC85AF52875C944495C5B00280000001");
   
   
_LIT8(KRTPPacket1, "8001CDDD00000001DEADBEEF9206D8823BACDD");   
_LIT8(KRTPPacket2, "8001CDE000000004DEADBEEF3958B1FB3F0E");


#define RTP_EUNIT_ASSERT_EQUALS( func, val2 ) {\
TInt unittestRetVal = func;\
if ( val2 != KErrNoMemory && unittestRetVal == KErrNoMemory ){\
	User::Leave( KErrNoMemory );\
	}\
else{\
	EUNIT_ASSERT_EQUALS( unittestRetVal, val2 );\
	}}
	
#define RTP_EUNIT_ASSERT_RTP_ID_ON
#ifdef RTP_EUNIT_ASSERT_RTP_ID_ON
#define RTP_EUNIT_ASSERT_RTP_ID( val1 ) \
if ( val1 == KNullId )\
    {\
    User::Leave( KErrNoMemory );\
    }
#else
#define RTP_EUNIT_ASSERT_RTP_ID( val1 )
EUNIT_ASSERT( val1 != KNullId );
#endif

// CONSTRUCTION
UT_CRtpSession* UT_CRtpSession::NewL()
    {
    UT_CRtpSession* self = UT_CRtpSession::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CRtpSession* UT_CRtpSession::NewLC()
    {
    UT_CRtpSession* self = new( ELeave ) UT_CRtpSession();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CRtpSession::~UT_CRtpSession()
    {
    }

// Default constructor
UT_CRtpSession::UT_CRtpSession()
    {
    }

// Second phase construct
void UT_CRtpSession::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_CRtpSession::SetupL(  )
    {
    iPacketsExpected = 0;
 

    TCreateSessionParams sessionParams;
    TUint port( 5000 );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iSessionBWidth = 65000;
    TInt err( KErrNone );

    
    for ( TUint k = 0; k < KRtpNumPayloadTypes; k++ )
        {
        iProfileRTPTimeRates[k] = ( TUint32 ) KRtpPayloadClockConversions[k];
        }

    TBuf8<3> cName; 
    TBuf8<3> userName;
    TBuf8<3> email;
    TBuf8<3> phoneNumber;
    TBuf8<3> location;
    TBuf8<3> swToolName;
    TBuf8<3> noticeStatus;
    TBuf8<3> privateStr;

    // Create SDES
    cName.Format( _L8( "ab" ) );
    cName.ZeroTerminate();
    userName.Format( _L8( "bc" ) );
    userName.ZeroTerminate();
    email.Format( _L8( "cd" ) );
    email.ZeroTerminate();
    phoneNumber.Format( _L8( "de" ) );
    phoneNumber.ZeroTerminate();
    location.Format( _L8( "ef" ) );
    location.ZeroTerminate();
    swToolName.Format( _L8( "fg" ) );
    swToolName.ZeroTerminate();
    noticeStatus.Format( _L8( "gh" ) );
    noticeStatus.ZeroTerminate();
    privateStr.Format( _L8( "hi" ) );
    privateStr.ZeroTerminate();

    TRtpSdesParams sdes;
    sdes.iCName.Set( cName.Ptr() );
    sdes.iUserName.Set( userName.Ptr() );
    sdes.iEmail.Set( email.Ptr() );
    sdes.iPhoneNumber.Set( phoneNumber.Ptr() );
    sdes.iLocation.Set( location.Ptr() );
    sdes.iSwToolName.Set( swToolName.Ptr() );
    sdes.iNoticeStatus.Set( noticeStatus.Ptr() );
    sdes.iPrivate.Set( privateStr.Ptr() );
    
    // Set it
    iSdes = CRtpSDES::NewL( sdes );

    

    // Opens a session to the socket server
    // i.e. creates an IPC communication channel to the socket server
    if ( ( err = iSocketServ.Connect() ) != KErrNone )
        {
        //"CRtpUtRtpCommSend::ConstructL()") );
        //"RSocketServ - Open Connection Error
        User::Leave( KErrGeneral );
        }

    
    TCommDbConnPref prefs;
    prefs.SetDirection( ECommDbConnectionDirectionOutgoing );
    prefs.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );    
    prefs.SetIapId ( KDefaultIap );//6

    if ( ( err = iConn.Open( iSocketServ ) ) != KErrNone )
        {
        //"CRtpUtRtpCommSend::ConstructL()") );
        //"RConnection - Start error %d"), err );
        User::Leave( KErrGeneral );
        }
    if ( ( err = iConn.Start( prefs ) ) != KErrNone )
        {
        ////"CRtpUtRtpCommSend::ConstructL()") );
        ////"RConnection - Start error %d"), err );
        User::Leave( KErrGeneral );
        }

    rtcpParams.iSessionBWidth = 6000; // less than the minimum of 8000
    rtcpParams.iRtcpFraction = 1.5; // more than the maximum of 1.0

    // Create a session with RTCP and bad parameters -> default values in use
    iSession = CRtpSession::NewL( sessionParams,
                                  port,
                                  ETrue, // enable RTCP
                                  &rtcpParams, // Rtcp params
                                  iSocketServ,
                                  iConn,
                                  1,
                                  iSdes,
                                  iProfileRTPTimeRates,
                                  ETrue,
                                  iLibrary,
                                  *this,
                                  *this
                                );

    if ( iSession->iNumOfRxStreams != 0 ||
         iSession->iNumOfTxStreams != 0 ||
         iSession->iSendSdesCounter != KSDES_SEND_PACKET ||
         iSession->iAverageRtcpSize != KAverageRtcpPacketLength ||
         iSession->iTotalParticipantsSession != 1 ||
         iSession->iRtcpTimeInterval == 0 ||
         iSession->iBandWidth != KRtpDefaultBandWidth ||
         iSession->iFraction != KRtpDefaultRtcpFrac ||
         iSession->iDefaultSSRC == 0 ||
         iSession->iRtpObserver != 0 ||
         iSession->iRtcpObserver != 0 
        )
        {
        //"Parameters wrongly updated during construction(2)" ) );
        //"CRtpSession::NewL() - FAILED" ) );
        User::Leave( KErrGeneral );
        }

    
    
    // Test the random SSRC generation, now that we have a few SSRC values
    
    // An SSRC collision here is improbable, though possible

    iTranstreamId=1 ;
    iRecvstreamId=2;
    TRtpSSRC ssrcOut( 1234 );
    TRtpSSRC ssrcOut2(5555);
    TRtpSSRC ssrcIn( 4321 );
    TRtpPayloadType payloadType( 0 );
    TInt error(KErrNone);
    error =iSession->CreateTransmitStreamL( iTranstreamId, payloadType, ssrcOut );
	EUNIT_ASSERT(error==KErrNone);
	
	error = iSession->CreateReceiveStreamL( iRecvstreamId, payloadType );   
	              
	EUNIT_ASSERT(error==KErrNone);
	
	error = iSession->CreateTransmitStreamExtL(iTranstreamId, payloadType, ssrcOut2);
	TInetAddr remoteAddr( INET_ADDR( 127, 0, 0, 1 ), 5000 );

    error = iSession->SetRemoteAddress( remoteAddr );    
    
    }

void UT_CRtpSession::Setup2L(  )
    {
    TPtrC16 fileName( _L16( "rtpstppacket.dll" ) );
  	User::LeaveIfError( iLibrary.Load( fileName ) );
  	
  	SetupL();
    }
    
void UT_CRtpSession::Teardown(  )
    {
    if ( iSession  )
        {
        if ( iRollBackRtcp )
            {
            iSession->iRtcpEnabled = iRollBackRtcpValue;
            }
            
        if (iSession->iPktExtSnd )
            {
            iSession->iPktExtSnd->Close();
            iSession->iPktExtSnd = NULL;
            }
        }
    delete iSession;
    iSession = NULL;


    delete iSdes;
    iSdes = NULL;

    iConn.Close();
    iSocketServ.Close();
    
    iLibrary.Close();
    
    iRollBackRtcp = EFalse;
    }

void UT_CRtpSession::Hex(HBufC8& aString)
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
void UT_CRtpSession::UT_CRtpSession_NewLL(  )
    {
    TCreateSessionParams sessionParams;
    TUint port( 5000 );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iSessionBWidth = 65000;
    TRtpSSRC ssrc1;
    TRtpSSRC ssrc2;
    TRtpSSRC ssrc3;
 	ssrc3 = iSession->SSRC(); // store the random SSRC for the test below
	
	delete iSession;
	iSession = NULL;
	
    // Create a session without RTCP
    iSession = CRtpSession::NewL( sessionParams,
                                  port,
                                  EFalse, // disable RTCP
                                  NULL, // Rtcp params
                                  iSocketServ,
                                  iConn,
                                  1,
                                  iSdes,
                                  iProfileRTPTimeRates,
                                  ETrue,
                                  iLibrary,
                                  *this,
                                  *this);

    ssrc1 = iSession->SSRC(); // store the random SSRC for the test below
    delete iSession;
    iSession = NULL;
    
    // Create a session with RTCP and good parameters
    iSession = CRtpSession::NewL( sessionParams,
                                  port,
                                  ETrue, // enable RTCP
                                  &rtcpParams, // Rtcp params
                                  iSocketServ,
                                  iConn,
                                  1,
                                  iSdes,
                                  iProfileRTPTimeRates,
                                  ETrue,
                                  iLibrary,
                                  *this,
                                  *this
                                );

    if ( iSession->iNumOfRxStreams != 0 ||
         iSession->iNumOfTxStreams != 0 ||
         iSession->iSendSdesCounter != KSDES_SEND_PACKET ||
         iSession->iAverageRtcpSize != KAverageRtcpPacketLength ||
         iSession->iTotalParticipantsSession != 1 ||
         iSession->iRtcpTimeInterval == 0 ||
         iSession->iBandWidth != rtcpParams.iSessionBWidth ||
         iSession->iFraction != rtcpParams.iRtcpFraction ||
         iSession->iDefaultSSRC == 0 ||
         iSession->iRtpObserver != 0 ||
         iSession->iRtcpObserver != 0 
        )
        {
        //"Parameters wrongly updated during construction (1) " ) );
        //"CRtpSession::NewL() - FAILED" ) );
        User::Leave( KErrGeneral );
        }

    ssrc2 = iSession->SSRC(); // store the random SSRC for the test below
    iSession->iSdesArray->ResetAndDestroy();
   
    if ( ssrc1 == ssrc2 ||
         ssrc1 == ssrc3 ||
         ssrc2 == ssrc3 )
        {
        // _L( "SSRC collision" ) );
        EUNIT_ASSERT(EFalse); 
        }
        
    // Create a session with non-default socket buf size
    delete iSession;
    iSession = NULL;
    sessionParams.iSocketBufSize = 10000;
    iSession = CRtpSession::NewL( sessionParams,
                                  port,
                                  ETrue, // enable RTCP
                                  &rtcpParams, // Rtcp params
                                  iSocketServ,
                                  iConn,
                                  1,
                                  iSdes,
                                  iProfileRTPTimeRates,
                                  ETrue,
                                  iLibrary,
                                  *this,
                                  *this );
                                  
    EUNIT_ASSERT_EQUALS( iSession->iCommNet->MaxSocketSize(), 10000 );                           
    }

void UT_CRtpSession::UT_CRtpSession_SSRCL(  )
    {
    EUNIT_ASSERT( ETrue );
    //Tested in SetUp
    }

void UT_CRtpSession::UT_CRtpSession_StartSessionL(  )
    {
    TInt error(KErrNone);
    TInt tempNumOfTxStreams( iSession->iNumOfTxStreams );
    TInt tempNumOfRxStreams( iSession->iNumOfRxStreams );

    iSession->iNumOfTxStreams = 0;
    iSession->iNumOfRxStreams = 0;

    // With the stream amounts set to 0, the function call should do nothing
    error = iSession->StartSession();
    if ( error != KErrNotReady )
        {
        EUNIT_ASSERT(EFalse);
        }

    
    // Restore the previous Rx value
    iSession->iNumOfRxStreams = tempNumOfRxStreams;

    // This should start the RTP & RTCP reception
    error = iSession->StartSession();
    if ( error != KErrNone || !iSession->iSessionStarted )
        {
        EUNIT_ASSERT(EFalse);
        }

    // Restore the previous Tx value
    iSession->iNumOfTxStreams = tempNumOfTxStreams;

    // This should start RTCP transmission
    error = iSession->StartSession();
    if ( error != KErrNone || !iSession->iSessionStarted )
        {
        EUNIT_ASSERT(EFalse);
        }

    iSession->iRtcpEnabled = EFalse; // temporarily

    // This should cover the rest of the branches
    error = iSession->StartSession();
    if ( error != KErrNone || !iSession->iSessionStarted )
        {
        EUNIT_ASSERT(EFalse);
        }

    iSession->iRtcpEnabled = ETrue; // restore

    }

void UT_CRtpSession::UT_CRtpSession_SetRemoteAddressL(  )
    {
    TInt error( KErrNone );

    TInetAddr remoteAddr( INET_ADDR( 127, 0, 0, 1 ), 5000 );

    // Try setting the address when the communications are down
    CRtpComm* tempComm = iSession->iCommNet;
    iSession->iCommNet = NULL;

    // Run the function
    error = iSession->SetRemoteAddress( remoteAddr );
    if ( error != KErrGeneral )
        {     
        EUNIT_ASSERT(EFalse);
        }

    // Restore communications
    iSession->iCommNet = tempComm;

    // Try again. This should work
    error = iSession->SetRemoteAddress( remoteAddr );
    if ( error != KErrNone )
        {
        EUNIT_ASSERT(EFalse);
        }

    // Check that the address and the session parameters are really updated
    TInetAddr tempAddr;    
           
    if (! iSession->iRemoteAddr[ERTPPort].Match(remoteAddr))
        {
        EUNIT_ASSERT(EFalse);
        } 

    // Flip the session started status to reach more branches
    iSession->iSessionStarted = !iSession->iSessionStarted;
    error = iSession->SetRemoteAddress( remoteAddr );
    if ( error != KErrNone )
        {
        EUNIT_ASSERT(EFalse);
        }

    // Restore
    iSession->iSessionStarted = !iSession->iSessionStarted;

    // Flip the RTCP status to reach more branches
    iSession->iRtcpEnabled = !iSession->iRtcpEnabled;
    error = iSession->SetRemoteAddress( remoteAddr );
    if ( error != KErrNone )
        {
        EUNIT_ASSERT(EFalse);
        }

    // Restore
    iSession->iRtcpEnabled = !iSession->iRtcpEnabled;
	}

void UT_CRtpSession::UT_CRtpSession_SetRemoteRtcpAddressL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpSession::UT_CRtpSession_CreateReceiveStreamLL(  )
    {
    TRtpPayloadType payloadType( 0 );
    TRtpId streamId( 3 );
    TRtpSSRC ssrc( 0 );
    TInt error( KErrNone );
    
    // Try to remove streams that do not exist
    TInt tempRx( iSession->iNumOfRxStreams );
    TInt tempTx( iSession->iNumOfTxStreams );

    iSession->CloseStream( 49384 );

    if ( tempRx != iSession->iNumOfRxStreams ||
         tempTx != iSession->iNumOfTxStreams )
        {
        EUNIT_ASSERT( EFalse );
        }


    // Create first transmit stream
    iSession->CreateTransmitStreamL( streamId, payloadType, ssrc );
    if ( iSession->iNumOfTxStreams != 1+tempTx )
        {
        User::Leave( KErrNoMemory );
        EUNIT_ASSERT( EFalse );
        }
    if ( ssrc != iSession->SSRC() )
        {
        EUNIT_ASSERT( ETrue );
        }

    // Create another stream to reach more branches
    streamId = 4;
    ssrc = 0;
    iSession->CreateTransmitStreamL( streamId, payloadType, ssrc );

    // Check the results
    if ( iSession->iNumOfTxStreams != 2 + tempTx)
        {
        User::Leave( KErrNoMemory );
        EUNIT_ASSERT( EFalse );
        }

    // Remove the transmit streams
    iSession->CloseStream( streamId );
    if ( iSession->iNumOfTxStreams != 1+tempTx )
        {
        EUNIT_ASSERT( EFalse );
        }

    streamId = 3;

    iSession->CloseStream( streamId );
    if ( iSession->iNumOfTxStreams != tempTx )
        {
        User::Leave( KErrNoMemory );
        EUNIT_ASSERT( EFalse );
        }

    // Create the stream stream again. This time, keep it
    iSession->CreateTransmitStreamL( streamId, payloadType, ssrc );

    // Check the results
    if ( iSession->iNumOfTxStreams != 1+tempTx )
        {
        User::Leave( KErrNoMemory );
        EUNIT_ASSERT( EFalse );
        }

    iTxStreamSSRC = ssrc;

    // Now do the same for receive streams

    streamId = 4;

    // Create first receive stream
    iSession->CreateReceiveStreamL( streamId, payloadType );
    if ( iSession->iNumOfRxStreams != 1 +tempRx)
        {
        User::Leave( KErrNoMemory );
        EUNIT_ASSERT( EFalse );
        }

    // Remove it
    iSession->CloseStream( streamId );
    if ( iSession->iNumOfRxStreams != tempRx )
        {
        User::Leave( KErrNoMemory );
        EUNIT_ASSERT( EFalse );
        }

    // Create the receive stream again. This time, keep it
    iSession->CreateReceiveStreamL( streamId, payloadType );
    if ( iSession->iNumOfRxStreams != 1+tempRx )
        {
        User::Leave( KErrNoMemory );
        EUNIT_ASSERT( EFalse );
        }
	iSession->CloseStream( streamId );
    }

void UT_CRtpSession::UT_CRtpSession_CreateTransmitStreamLL(  )
    {
    EUNIT_ASSERT( ETrue );
    //See above test
    }

void UT_CRtpSession::UT_CRtpSession_CreateTransmitStreamExtLL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpSession::UT_CRtpSession_CloseStreamL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpSession::UT_CRtpSession_ResetTxStreamStatsL(  )
    {
	TInt error( KErrNone );
    TRtpPeerStat stat1;
    TRtpPeerStat stat2;
    stat1.iNumPacketsSent = 0;
    stat1.iCumNumOctetsSent = 0;
    stat1.iRoundTripDelay = 0;
    stat1.iTxBandwidth = 0;
    stat1.iCumNumPacketsLost = 0;
    stat1.iFractionLost = 0;
    stat1.iArrivalJitter = 0;
    stat1.iRxBandwidth = 0;
    stat1.iChannelBufferSize = 0;
    stat1.iNTPTimeStampSec = 0;
    stat1.iNTPTimeStampFrac = 0;
    stat1.iTimeStamp = 0;
    stat2 = stat1;
	
    // RX stats ////////////////////////////////////////

    // Get the current Tx stream statistics
    error = iSession->GetStreamStatistics( 1/*Tx stream ID*/, stat1 );
    if ( error != KErrNone )
        {
        //"Should have found Tx stream" ) );
        //"CRtpSession::GetStreamStatistics() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    if ( PeerStatsEqual( stat1, stat2 ) )
        {
        //"Tx statistics empty, cannot verify success" ) );
        //"CRtpSession::ResetTxStreamStats() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    // Clear the statistics
    iSession->ResetTxStreamStats();

    // Get the stats again. Now they should be empty
    
    TInt count = iSession->iStreamTxArray->Count(); 
    
    TInt count2 = iSession->iStreamRxArray->Count();
   
   	TRtpPayloadType payloadType( 0 );
    TRtpId streamId( 2 );
    TRtpSSRC ssrc( 0 );
    iSession->CreateTransmitStreamL( streamId, payloadType, ssrc );
    error = iSession->GetStreamStatistics( 2/*Tx stream ID*/, stat1 );
    if ( error != KErrNone )
        {
        //"Should have found Tx stream" ) );
        //"CRtpSession::GetStreamStatistics() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    if ( !PeerStatsEqual( stat1, stat2 ) )
        {
        ////"Tx statistics not reset" ) );
        ////"CRtpSession::ResetTxStreamStats() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    //"CRtpSession::ResetTxStreamStats() - passed" ) );

    // RX stats ////////////////////////////////////////
    
    stat1.iNumPacketsSent = 42;
    stat1.iCumNumOctetsSent = 42;
    stat1.iRoundTripDelay = 42;
    stat1.iTxBandwidth = 42;
    stat1.iCumNumPacketsLost = 42;
    stat1.iFractionLost = 42;
    stat1.iArrivalJitter = 42;
    stat1.iRxBandwidth = 42;
    stat1.iChannelBufferSize = 42;
    stat1.iNTPTimeStampSec = 42;
    stat1.iNTPTimeStampFrac = 42;
    stat1.iTimeStamp = 42;
    stat2 = stat1;

    // Get the current Rx stream statistics
    error = iSession->GetStreamStatistics( 2/*Rx stream ID*/, stat1 );
    if ( error != KErrNone )
        {
        ////"Should have found Rx stream" ) );
        ////"CRtpSession::GetStreamStatistics() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    if ( PeerStatsEqual( stat1, stat2 ) )
        {
        // "Rx statistics empty, cannot verify success" ) );
        // "CRtpSession::ResetRxStreamStats() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    // Clear the comparison statistics
    stat2.iNumPacketsSent = 0;
    stat2.iCumNumOctetsSent = 0;
    stat2.iRoundTripDelay = 0;
    stat2.iTxBandwidth = 0;
    stat2.iCumNumPacketsLost = 0;
    stat2.iFractionLost = 0;
    stat2.iArrivalJitter = 0;
    stat2.iRxBandwidth = 0;
    stat2.iChannelBufferSize = 0;
    stat2.iNTPTimeStampSec = 0;
    stat2.iNTPTimeStampFrac = 0;
    stat2.iTimeStamp = 0;

    // Clear the session's statistics
    iSession->ResetRxStreamStats();

    // Get the stats again. Now they should be empty
    error = iSession->GetStreamStatistics( 2/*Rx stream ID*/, stat1 );
    if ( error != KErrNone )
        {
        //"Should have found Rx stream" ) );
        //"CRtpSession::GetStreamStatistics() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    if ( !PeerStatsEqual( stat1, stat2 ) )
        {
        //"Rx statistics not reset" ) );
        //"CRtpSession::ResetRxStreamStats() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    }

void UT_CRtpSession::UT_CRtpSession_ResetRxStreamStatsL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpSession::UT_CRtpSession_RegisterRtpObserverL(  )
    {
      // Register RTP observer
    
    iSession->RegisterRtpObserver( *this );

    if ( iSession->iRtpObserver == NULL )
        {
        //"Rtp observer member not updated" ) );
        //"CRtpSession::RegisterRtpObserver() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    //"CRtpSession::RegisterRtpObserver() - passed" ) );

    // Register RTCP observer
    //Test description: Test registering an RTCP observer" ) );
    //"CRtpSession::RegisterRtcpObserver()" ) );

    iSession->RegisterRtcpObserver( *this );

    if ( iSession->iRtcpObserver == NULL )
        {
        //"Rtcp observer member not updated" ) );
        //"CRtpSession::RegisterRtcpObserver() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    //"CRtpSession::RegisterRtcpObserver() - passed" ) );

    // Unregister RTP observer
    
    //"Test description: Test unregistering an RTP observer" ) );
    // _L( "CRtpSession::UnregisterRtpObserver()" ) );
    iSession->UnregisterRtpObserver();

    if ( iSession->iRtpObserver != NULL )
        {
        //"Rtp observer member not unregistered" ) );
        //"CRtpSession::RegisterRtpObserver() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    //"CRtpSession::RegisterRtpObserver() - passed" ) );

    // Unregister RTCP observer
    //"" ) );
    //"Test description: Test registering an RTCP observer" ) );
    //"CRtpSession::UnregisterRtcpObserver()" ) );

    iSession->UnregisterRtcpObserver();

    if ( iSession->iRtcpObserver != NULL )
        {
        //"Rtcp observer member not unregistered" ) );
        //"CRtpSession::UnregisterRtcpObserver() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    //"CRtpSession::UnregisterRtcpObserver() - passed" ) );


    // Register non-RTP data observer
    //"" ) );
    //"Test description: Test registering a non RTP observer" ) );
    //"CRtpSession::RegisterNonRtpDataObserver()" ) );

    iSession->SetNonRTPDataObserver( this );

    if ( iSession->iNonRTPDataObserver == NULL )
        {
        //"Non-Rtp observer member not updated" ) );
        //"CRtpSession::SetNonRTPDataObserver( this ) - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    //"CRtpSession::SetNonRTPDataObserver( this ) - passed" ) );

    // Unregister Non-RTP observer
    //"" ) );
    //"Test description: Test unregistering a non RTP data observer" ) );
    //"CRtpSession::SetNonRTPDataObserver(NULL)" ) );
    iSession->SetNonRTPDataObserver(NULL);

    if ( iSession->iNonRTPDataObserver != NULL )
        {
        //"Non-Rtp observer member not unregistered" ) );
        //"CRtpSession::SetNonRTPDataObserver(NULL) - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    //"CRtpSession::SetNonRTPDataObserver(NULL) - passed" ) );


    // Now that we know the function works,
    // register this object as RTP and RTCP observer again so we can proceed
    iSession->RegisterRtpObserver( *this );
    iSession->RegisterRtcpObserver( *this );

    }

void UT_CRtpSession::UT_CRtpSession_UnregisterRtpObserverL(  )
    {
    EUNIT_ASSERT( ETrue);
    }

void UT_CRtpSession::UT_CRtpSession_SetNonRTPDataObserverL(  )
    {
    EUNIT_ASSERT( ETrue);
    }

void UT_CRtpSession::UT_CRtpSession_SendRtpPacketL(  )
    {
    TInt error( KErrNone );

    // Initialise lots of values
    TRtpSendHeader header;
    header.iHeaderExtension = NULL;
    header.iMarker = 0;
    header.iPadding = 0;
    header.iPayloadType = 0;
    header.iTimestamp = 50;

    TBuf8<KMaxRtpPacketSize+1> dataBig;
    dataBig.Format( _L8( "foo" ) );
    dataBig.SetLength( KMaxRtpPacketSize + 1 );
    TBuf8<5> data;
    data.Format( _L8( "foo" ) );

    TRtpSequence seqNum = 0;
    TRequestStatus status;
    TUint packetSize = 2048;
    
    
    //"Test description: Test sending RTP packets" ) );
    //"CRtpSession::SendRtpPacket( TRtpId, TRtpSendHeader, TDesC8, TRequestStatus )" ) );

    // Test sending RTP packets with invalid stream ID
    error = iSession->SendRtpPacket( 4876, header, data, status ); 
    RTP_EUNIT_ASSERT_EQUALS(error, KErrNotFound );
        
    // Test sending RTP packets with too big size
    error = iSession->SendRtpPacket( 1, header, dataBig, status ); 
    RTP_EUNIT_ASSERT_EQUALS ( error, KErrOverflow );     

    // Test sending a valid RTP packet asynchronously
    error = iSession->SendRtpPacket( 1, header, data, status ); 
    RTP_EUNIT_ASSERT_EQUALS( error, KErrNone );
    
    User::WaitForRequest( iSession->iCommNet->iSender[ERTPPort]->iStatus, status );
    RTP_EUNIT_ASSERT_EQUALS( status.Int(), KRequestPending );
    iSession->iCommNet->iSender[ERTPPort]->RunL();
    iSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
    User::WaitForRequest( status );
    RTP_EUNIT_ASSERT_EQUALS( status.Int(), KErrNone );
    
    // Test sending RTP packets with invalid stream ID
    error = iSession->SendRtpPacket( 4876, header, data ); 
    RTP_EUNIT_ASSERT_EQUALS ( error, KErrNotFound );
    
    // Test sending RTP packets with too big size
    error = iSession->SendRtpPacket( 1, header, dataBig ); 
    RTP_EUNIT_ASSERT_EQUALS ( error, KErrOverflow );
    
     // Test sending a big packet when sendbufsize has been modified
    TInt oldSocketSize = iSession->iCommNet->iSocketSize;
    iSession->iCommNet->iSocketSize = 10000;
    TBuf8<8000> dataBig2;
    dataBig2.Format( _L8( "foo" ) );
    dataBig2.SetLength( 8000 );
    error = iSession->SendRtpPacket( 1, header, dataBig2, status ); 
    RTP_EUNIT_ASSERT_EQUALS( error, KErrNone );
    User::WaitForRequest( iSession->iCommNet->iSender[ERTPPort]->iStatus, status );
    RTP_EUNIT_ASSERT_EQUALS( status.Int(), KRequestPending );
    iSession->iCommNet->iSender[ERTPPort]->RunL();
    iSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
    User::WaitForRequest( status );
    RTP_EUNIT_ASSERT_EQUALS( status.Int(), KErrNone );
    
    // Test sending a too big packet when sendbufsize has been modified
    TBuf8<20000> dataBig3;
    dataBig3.Format( _L8( "foo" ) );
    dataBig3.SetLength( 20000 );
    error = iSession->SendRtpPacket( 1, header, dataBig3 ); 
    RTP_EUNIT_ASSERT_EQUALS( error, KErrOverflow );

    // Test sending a valid RTP packet asynchronously using sequence number
    iSession->iCommNet->iSocketSize = oldSocketSize;
    error = iSession->SendRtpPacket( 1, 15, header, data, status ); 
    RTP_EUNIT_ASSERT_EQUALS ( error, KErrNone );
    
    User::WaitForRequest( iSession->iCommNet->iSender[ERTPPort]->iStatus, status );
    RTP_EUNIT_ASSERT_EQUALS( status.Int(), KRequestPending );
    iSession->iCommNet->iSender[ERTPPort]->RunL();
    iSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
    User::WaitForRequest( status );
    RTP_EUNIT_ASSERT_EQUALS( status.Int(), KErrNone );
    
    // Test sending RTP packets with invalid stream ID
    error = iSession->SendRtpPacket( 4876, 15 /* seqnum */, header, data, status ); 
    RTP_EUNIT_ASSERT_EQUALS( error, KErrNotFound )
    
    // Test sending RTP packets with too big size
    error = iSession->SendRtpPacket( 1, 15 /* seqnum */, header, dataBig, status ); 
    RTP_EUNIT_ASSERT_EQUALS( error, KErrOverflow );
    
    // Test sending a valid RTP packet synchronously
    error = iSession->SendRtpPacket( 1, header, data ); 
    RTP_EUNIT_ASSERT_EQUALS( error, KErrNone );
    
    User::WaitForRequest( iSession->iCommNet->iSender[ERTPPort]->iStatus );
    iSession->iCommNet->iSender[ERTPPort]->RunL();
    iSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
    }

void UT_CRtpSession::UT_CRtpSession_SendRtpPacket_OneL(  )
    {
    TCreateSessionParams sessionParams;
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iSessionBWidth = 65000;
   	TUint port1( 6000 );
   	TInt error( KErrNone );
   	TRtpSSRC ssrcOut( 22 );
    
    TRtpSSRC ssrcIn( 33 );
    TRtpPayloadType payloadType( 0 );
    TInetAddr remoteAddr( INET_ADDR( 127, 0, 0, 1 ), 5000 );
    
 	iSession2 = CRtpSession::NewL( sessionParams,
                                  port1,
                                  ETrue, // enable RTCP
                                  &rtcpParams, // Rtcp params
                                  iSocketServ,
                                  iConn,
                                  2,
                                  iSdes,
                                  iProfileRTPTimeRates,
                                  EFalse,
                                  iLibrary,
                                  *this,
                                  *this);   
   	error =iSession2->CreateTransmitStreamL( iTranstreamId, payloadType, ssrcOut );
	EUNIT_ASSERT(error==KErrNone);
	
	error = iSession2->CreateReceiveStreamL( iRecvstreamId, payloadType );                    
	EUNIT_ASSERT(error==KErrNone);     
	error = iSession2->SetRemoteAddress( remoteAddr );                              

  
    // Initialise lots of values
    TRtpSendHeader header;
    header.iHeaderExtension = NULL;
    header.iMarker = 0;
    header.iPadding = 0;
    header.iPayloadType = 0;
    header.iTimestamp = 50;

    TBuf8<KMaxRtpPacketSize+1> dataBig;
    dataBig.Format( _L8( "foo" ) );
    dataBig.SetLength( KMaxRtpPacketSize + 1 );
    TBuf8<5> data;
    data.Format( _L8( "foo" ) );

    TRtpSequence seqNum = 0;
    TRequestStatus status;
    TUint packetSize = 2048;
   
    error = iSession2->SendRtpPacket( header, data ); 
          
    EUNIT_ASSERT( error == KErrNotFound );  
    
    // Test sending RTP packets with too big size
    error = iSession->SendRtpPacket( header, dataBig); 
    EUNIT_ASSERT(  error == KErrOverflow );       

  
    error = iSession->SendRtpPacket(header, data ); 
    RTP_EUNIT_ASSERT_EQUALS( error, KErrNone );    
   
   	delete iSession2;
	iSession2=NULL;  
  
    }

void UT_CRtpSession::UT_CRtpSession_SendRtpPacket_TwoL(  )
    {
    TInt error( KErrNone );

    // Initialise lots of values
    TRtpSendHeader header;
    header.iHeaderExtension = NULL;
    header.iMarker = 0;
    header.iPadding = 0;
    header.iPayloadType = 0;
    header.iTimestamp = 50;

    TBuf8<KMaxRtpPacketSize+1> dataBig;
    dataBig.Format( _L8( "foo" ) );
    dataBig.SetLength( KMaxRtpPacketSize + 1 );
    TBuf8<5> data;
    data.Format( _L8( "foo" ) );

    TRtpSequence seqNum = 0;
    TRequestStatus status;
    TUint packetSize = 2048;
  	TLibraryFunction entry = iLibrary.Lookup( 1 );
    iSession->iPktExtSnd =  ( MRtpPacketExt * ) entry();
    iSession->iPktExtSnd->ConstructL( KMaxRtpPacketSize, 1 );
    error = iSession->SendRtpPacket(header, data, status ); 
    RTP_EUNIT_ASSERT_EQUALS( error, KErrNone );
        
    User::WaitForRequest( iSession->iCommNet->iSender[ERTPPort]->iStatus, status );
    RTP_EUNIT_ASSERT_EQUALS( status.Int(), KRequestPending );
    iSession->iCommNet->iSender[ERTPPort]->RunL();
    iSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
    User::WaitForRequest( status );
    RTP_EUNIT_ASSERT_EQUALS( status.Int(), KErrNone );
    
    // Test sending RTP packets with too big size
    error = iSession->SendRtpPacket(header, dataBig, status ); 
    RTP_EUNIT_ASSERT_EQUALS( error, KErrOverflow );       
	iSession->iPktExtSnd->RtpPacketReset();
	
	error = iSession->SendRtpPacket(header, dataBig); 
    EUNIT_ASSERT( error == KErrOverflow );       
	iSession->iPktExtSnd->RtpPacketReset();
	
	TRequestStatus status2;
    error = iSession->SendRtpPacket( header, data, status2 ); 
    RTP_EUNIT_ASSERT_EQUALS( error, KErrNone );  
    
    User::WaitForRequest( iSession->iCommNet->iSender[ERTPPort]->iStatus, status2 );
    RTP_EUNIT_ASSERT_EQUALS( status2.Int(), KRequestPending );
    iSession->iCommNet->iSender[ERTPPort]->RunL();
    iSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
    User::WaitForRequest( status2 );
    RTP_EUNIT_ASSERT_EQUALS( status2.Int(), KErrNone );
    }

void UT_CRtpSession::UT_CRtpSession_SendRtpPacket_3L(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpSession::UT_CRtpSession_SendRtpPacket_4L(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpSession::UT_CRtpSession_SendDataL(  )
    {
    TInt error( KErrNone );
    TRequestStatus status;
    TBuf8<KMaxRtpPacketSize+1> dataBig;
    dataBig.Format( _L8( "foo" ) );
    dataBig.SetLength( KMaxRtpPacketSize + 1 );
    
    //"Test description: Test sending non-RTP packets" ) );
    //_L( "CRtpSession::SendData( .. )" ) );
    
    // Test sending a non-RTP packet bigger than max RTP packet
    iSession->SendData( ETrue, dataBig, status ); 
    
    User::WaitForRequest( iSession->iCommNet->iSender[ERTPPort]->iStatus, status );
    RTP_EUNIT_ASSERT_EQUALS( status.Int(), KRequestPending );
    iSession->iCommNet->iSender[ERTPPort]->RunL();
    iSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
    User::WaitForRequest( status );
    RTP_EUNIT_ASSERT_EQUALS( status.Int(), KErrTooBig );
    }

void UT_CRtpSession::UT_CRtpSession_CancelSendL(  )
    {
    //"Test description: Test processing RTCP reports" ) );
    //"CRtpSession::CancelSend()" ) );

    TInt error( KErrNone );

    // Initialise lots of values
    TRtpSendHeader header;
    header.iHeaderExtension = NULL;
    header.iMarker = 0;
    header.iPadding = 0;
    header.iPayloadType = 0;
    header.iTimestamp = 50;

    TBuf8<500> data;
    data.Format( _L8( "123456789012345678901234567890123456789012345678901234567890" ) );
    data.Append( _L8( "123456789012345678901234567890123456789012345678901234567890" ) );
    data.Append( _L8( "123456789012345678901234567890123456789012345678901234567890" ) );
    data.Append( _L8( "123456789012345678901234567890123456789012345678901234567890" ) );

    TRtpSequence seqNum = 0;
    TRequestStatus status;
    TUint packetSize = 2048;
    
  
    //"Test description: Test sending RTP packets" ) );
    //"CRtpSession::SendRtpPacket( TRtpId, TRtpSendHeader, TDesC8, TRequestStatus )" ) );

    error = iSession->SendRtpPacket( 1, header, data, status ); 
	status = KErrNone;
    iSession->CancelSend();

    User::WaitForRequest( status );

    if ( status != KErrCancel && status != KErrNone /* The packet may be sent already*/ )
        {
        //"Packet cancellation failed" ) );
        //"Error was %d, status was %d" ), error, status );
        //"CRtpSession::CancelSend() - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
    
    //"CRtpSession::CancelSend() - passed" ) );
    }

void UT_CRtpSession::UT_CRtpSession_RegisterRtcpObserverL(  )
    {
       // now that we have streams, test setting RTCP observer to RX and TX stream
    
    //"Test description: Test registering an RTCP observer with streams" ) );
    //"CRtpSession::TestRegUnregRTCPWithStreamsL()" ) );    

    //unregistering RTCP observer from RX streams
    iSession->UnregisterRtcpObserver();  

    //check results
    CRtpRecvStream* tempRecvStream;
    TInt count = iSession->iStreamRxArray->Count();    
    for(int index = 0; index < count; index++ )
        {
        tempRecvStream = ( CRtpRecvStream* )
                     ( iSession->iStreamRxArray->At( index ).GetStreamAddress() );                     
        if (tempRecvStream->iRtcpObserver!=NULL)
            {
            //"Session internal members incorrectly updated (1)" ) );
            //"CRtpSession::TestRegUnregRTCPWithStreamsL() - FAILED" ) );
            EUNIT_ASSERT( EFalse );        
            }        
        }                  
    //unregistering RTCP observer from TX streams
    CRtpTranStream* tempTxStream;
    count = iSession->iStreamTxArray->Count();    
    for(int index = 0; index < count; index++ )
        {
        tempTxStream = ( CRtpTranStream* )
                     ( iSession->iStreamTxArray->At( index ).GetStreamAddress() );
        if (tempRecvStream->iRtcpObserver!=NULL)
            {
            //"Session internal members incorrectly updated (2)" ) );
            //"CRtpSession::TestRegUnregRTCPWithStreamsL() - FAILED" ) );
            EUNIT_ASSERT( EFalse );        
            }                
        }                          
      
    iSession->RegisterRtcpObserver( *this );

    //now check results, first session
    if (iSession->iRtcpObserver != this)
        {        
        //"Session internal members incorrectly updated (3)" ) );
        //"CRtpSession::TestRegUnregRTCPWithStreamsL() - FAILED" ) );
        EUNIT_ASSERT( EFalse );        
        }
        
    //check RX streams
    count = iSession->iStreamRxArray->Count();    
    for(int index = 0; index < count; index++ )
        {
        tempRecvStream = ( CRtpRecvStream* )
                     ( iSession->iStreamRxArray->At( index ).GetStreamAddress() );                     
        if (tempRecvStream->iRtcpObserver !=this)
            {        
            //"Session internal members incorrectly updated (4)" ) );
            //"CRtpSession::TestRegUnregRTCPWithStreamsL() - FAILED" ) );
            EUNIT_ASSERT( EFalse );        
            }        
        }    
                      
    //check TX streams
    count = iSession->iStreamTxArray->Count();    
    for(int index = 0; index < count; index++ )
        {
        tempTxStream = ( CRtpTranStream* )
                     ( iSession->iStreamTxArray->At( index ).GetStreamAddress() );
        if (tempTxStream->iRtcpObserver != this)
            {        
            //"Session internal members incorrectly updated (5)" ) );
            //"CRtpSession::TestRegUnregRTCPWithStreamsL() - FAILED" ) );
            EUNIT_ASSERT( EFalse );        
            }        
        }               

    
    }

void UT_CRtpSession::UT_CRtpSession_UnregisterRtcpObserverL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpSession::UT_CRtpSession_SendRtcpByePacketLL(  )
    {
    // "Test description: Test building BYE packets" ) );
    // "CRtpSession::SendRtcpByePacketL()" ) );
	TInt error( KErrNone );
	error = iSession->StartSession();
    if ( error != KErrNone)
        {
        __ASSERT_ALWAYS( error != KErrNoMemory, User::Leave( KErrNoMemory ) );
        EUNIT_ASSERT(EFalse);
        }
    
    // Initialise lots of values
    TRtpSendHeader header;
    header.iHeaderExtension = NULL;
    header.iMarker = 0;
    header.iPadding = 0;
    header.iPayloadType = 0;
    header.iTimestamp = 50;

    TBuf8<5> data;
    data.Format( _L8( "foo" ) );

    TRtpSequence seqNum = 0;
    TRequestStatus status;
    TUint packetSize = 2048;
    
    error = iSession->SendRtpPacket( 1, header, data ); 
    RTP_EUNIT_ASSERT_EQUALS(error, KErrNone );
    
    //Then start rtcp packet sending
    TBuf8<KMaxRtpPacketSize+1> reasonBig;
    reasonBig.Format( _L8( "foo" ) );
    reasonBig.SetLength( KMaxRtpPacketSize + 1 );
    TBuf8<5> reason;
    reason.Format( _L8( "foo" ) );
    
	
    // Try to send a BYE packet that is too big
    error = iSession->SendRtcpByePacketL( 1, reasonBig );
    if ( error != KErrTooBig )
        {
        __ASSERT_ALWAYS( error != KErrNoMemory, User::Leave( KErrNoMemory ) );
        // "Should not have built too big BYE packet" ) );
        // "CRtpSession::SendRtcpByePacketL() - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }

    // Try to send a BYE packet using an invalid stream
    error = iSession->SendRtcpByePacketL( 3498/* invalid Tx stream*/, reason );
    if ( error != KErrNotFound )
        {
        __ASSERT_ALWAYS( error != KErrNoMemory, User::Leave( KErrNoMemory ) );
        // "Should not have built BYE packet with invalid Tx stream ID" ) );
        // "CRtpSession::SendRtcpByePacketL() - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }

    // Send a valid BYE packet
    error = iSession->SendRtcpByePacketL( 1/* valid Tx stream*/, reason );
    if ( error!= KErrNone )
        {
        __ASSERT_ALWAYS( error != KErrNoMemory, User::Leave( KErrNoMemory ) );
        // "Should have Send RTP packet before sending built BYE packet" ) );
        // "CRtpSession::SendRtcpByePacketL() - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }

    }

void UT_CRtpSession::UT_CRtpSession_SendRtcpAppPacketL(  )
    {
    // "Test description: Test building APP packets" ) );
    // "CRtpSession::SendRtcpAppPacket()" ) );
	TInt error( KErrNone );
	error = iSession->StartSession();
    if ( error != KErrNone)
        {
        __ASSERT_ALWAYS( error != KErrNoMemory, User::Leave( KErrNoMemory ) );
        EUNIT_ASSERT(EFalse);
        }
    
    // Initialise lots of values
    TRtpSendHeader header;
    header.iHeaderExtension = NULL;
    header.iMarker = 0;
    header.iPadding = 0;
    header.iPayloadType = 0;
    header.iTimestamp = 50;

    TBuf8<5> data;
    data.Format( _L8( "foo" ) );

    TRtpSequence seqNum = 0;
    TRequestStatus status;
    TUint packetSize = 2048;
    
    error = iSession->SendRtpPacket( 1, header, data ); 
    RTP_EUNIT_ASSERT_EQUALS(error, KErrNone );
    //now start RTCP
    
    TRtcpApp app;
    app.iSubType = 0;
    app.iName[0] = 0;
    app.iAppData[0] = 0;
    app.iAppDataLen = KMaxRtcpAppData + 1;

    // Try to send an APP packet that is too big
    error = iSession->SendRtcpAppPacketL( 1, app );
    if ( error != KErrTooBig )
        {
        __ASSERT_ALWAYS( error != KErrNoMemory, User::Leave( KErrNoMemory ) );
        // "Should not have built too big APP packet" ) );
        // "CRtpSession::SendRtcpAppPacket() - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }

    app.iAppDataLen = 0;

    // Try to send an APP packet using an invalid stream
    error = iSession->SendRtcpAppPacketL( 345/* invalid Tx stream*/, app );
    if ( error != KErrNotFound )
        {
        __ASSERT_ALWAYS( error != KErrNoMemory, User::Leave( KErrNoMemory ) );
        // "Should not have built APP packet with invalid Tx stream ID" ) );
        // "CRtpSession::SendRtcpAppPacket() - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }

    // Send a valid APP packet
    error = iSession->SendRtcpAppPacketL( 1/* valid Tx stream*/, app );
    if ( error != KErrNone )
        {
        __ASSERT_ALWAYS( error != KErrNoMemory, User::Leave( KErrNoMemory ) );
        // "Should have built APP packet" ) );
        // "CRtpSession::SendRtcpAppPacket() - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }

    // "CRtpSession::SendRtcpAppPacket() - passed" ) );
      
    }

void UT_CRtpSession::UT_CRtpSession_SendRtcpSrPacket1L(  )
    {
    
    // "Test description: Testing RTCP functions with RTCP disabled..." ) );
    
    // Store the previous status of RTCP,it will be restored at taerdown
    iRollBackRtcp = ETrue;
    iRollBackRtcpValue = iSession->iRtcpEnabled;
    iSession->iRtcpEnabled = EFalse;

    TBuf8<5> dummyBuf8;

    EUNIT_ASSERT( iSession->SendRtcpByePacketL( 0, dummyBuf8 ) == KErrNotSupported );
    
    TRtcpApp* app = new( ELeave ) TRtcpApp;
    CleanupStack::PushL( app );
    EUNIT_ASSERT ( iSession->SendRtcpAppPacketL( 0, *app ) == KErrNotSupported );
        
    CleanupStack::PopAndDestroy( app );    

    EUNIT_ASSERT ( iSession->SendRtcpSrPacketL( 0 ) == KErrNotSupported );
        

    EUNIT_ASSERT ( iSession->SendRtcpRrPacketL( 0 ) == KErrNotSupported );
        
    EUNIT_ASSERT ( iSession->GetRtcpSocket() == NULL );
        
    TRtcpParams params;
    EUNIT_ASSERT( iSession->InitialiseRtcp( &params ) == KErrGeneral );
        
    EUNIT_ASSERT( iSession->SetRtcpParameters( params ) == KErrGeneral );
        
    EUNIT_ASSERT ( iSession->RegisterRtcpObserver( *this ) == KErrGeneral );
        
    EUNIT_ASSERT ( iSession->IssueRtcpRecv() == KErrGeneral )
    
    EUNIT_ASSERT ( iSession->DoBuildSendRTCPReportIfNotSentL() == EFalse )

    TInt dummy( 0 );
    //CRtpTranStream* dummyTranStream( NULL );
    TRtpPacketStreamParam dummyStreamParam;
    TRtpPacketIOParam dummyInitParam;

    // These cannot be verified, as they are void functions
    iSession->DetermineSourceCountAndLength( dummy, dummy, NULL );
    iSession->DoBuildSendRTCPReportL();
    iSession->BuildSendRTCPReportL();
    iSession->SetParamsForFullRTCPReport( NULL, &dummyStreamParam,
                                          &dummyInitParam, NULL );
    iSession->BuildSrSection( NULL );
    iSession->BuildRrSection( 0, NULL, 0 );
    iSession->BuildEmptyRr( NULL );
    iSession->BuildSdesSection( NULL );
    
    if ( iSession->ShouldBuildEmptyRR( NULL ) != EFalse )
        {
        // "ShouldBuildEmptyRR() did not return as expected" ) );
        // "Testing RTCP functions with RTCP disabled - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
    
    if ( iSession->BuildRTCPReport( NULL, 0 ) != KErrGeneral )
        {
        // "BuildRTCPReport() did not return as expected" ) );
        // "Testing RTCP functions with RTCP disabled - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
    
    if ( iSession->ProcessOneRTCPReportSectionL( dummyInitParam, 0 )
         != ERTCP_PACKET_ERROR )
        {
        // "ProcessOneRTCPReportSection() did not return as expected" ) );
        // "Testing RTCP functions with RTCP disabled - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
    
    if ( iSession->ProcessRTCPReportL() != ERTCP_PACKET_ERROR )
        {
        // "ProcessRTCPReport() did not return as expected" ) );
        // "Testing RTCP functions with RTCP disabled - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
    
    if ( iSession->ProcessSRSectionL( 0 ) != ERTCP_PACKET_ERROR )
        {
        // "ProcessSRSection() did not return as expected" ) );
        // "Testing RTCP functions with RTCP disabled - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
    
    if ( iSession->ProcessRRSectionL( 0 ) != ERTCP_PACKET_ERROR )
        {
        // "ProcessRRSection() did not return as expected" ) );
        // "Testing RTCP functions with RTCP disabled - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
    
    TRtpSdesParams dummySdesParams;
    //CRtpRecvStream* dummyRecvStream( NULL );
    //CRtpSDES* dummySdes( NULL );

    if ( iSession->GetSDESFromSDESData( &dummySdesParams, NULL,
                                        NULL ) != KErrGeneral )
        {
        // "GetSDESFromSDESData() did not return as expected" ) );
        // "Testing RTCP functions with RTCP disabled - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
    
    if ( iSession->ProcessSDESDataL( dummyStreamParam, dummyInitParam, 0 )
         != ERTCP_PACKET_ERROR )
        {
        // "ProcessSDESData() did not return as expected" ) );
        // "Testing RTCP functions with RTCP disabled - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
    
    if ( iSession->ProcessSDESSectionL( 0 ) != ERTCP_PACKET_ERROR )
        {
        // "ProcessSDESSection() did not return as expected" ) );
        // "Testing RTCP functions with RTCP disabled - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
    
    if ( iSession->ProcessBYESectionL() != ERTCP_PACKET_ERROR )
        {
        // "ProcessBYESection() did not return as expected" ) );
        // "Testing RTCP functions with RTCP disabled - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
    
    if ( iSession->ProcessAPPSectionL( 0, 0 ) != ERTCP_PACKET_ERROR )
        {
        // "ProcessAPPSection() did not return as expected" ) );
        // "Testing RTCP functions with RTCP disabled - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
    
    if ( iSession->DoAddSdesToArrayL( NULL ) != NULL )
        {
        // "FUNCTION() did not return as expected" ) );
        // "Testing RTCP functions with RTCP disabled - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
    
    TUint dummyUint;
    EUNIT_ASSERT ( iSession->AddSdesToArrayL( dummyUint, NULL ) == EFalse );
        
    EUNIT_ASSERT( iSession->RemoveSdesFromArray( 0 ) == KErrGeneral );
    }
void UT_CRtpSession::UT_CRtpSession_RemoveSdesFromArrayL()
	{
	iSession->iRtcpEnabled = ETrue;	
	TRtpPacketIOParam dummyExtractParam;
    TRtpPacketStreamParam streamParam;
    //result = AddSdesToArrayL( tempSdesAdress,&aExtractParam ) 
	dummyExtractParam.TRTCP_HEADER.pt=ERTCP_SDES;
    streamParam.TRTCP_SDES.SSRC= iTranstreamId;
   	iSession->iSdesArray->Reset();
    iSession->iNumOfRxStreams=0;
    iSession->RegisterRtcpObserver(*this);
	///////////////////////TEST CASE 1///////////////////////////////////
	TRtpRtcpEnum rtcpEnum = iSession->ProcessSDESDataL(streamParam, dummyExtractParam, iRecvstreamId);
	if ( rtcpEnum == ERTCP_PACKET_ERROR || iSession->iSdesArray->Count() == 0 )
	    {
	    User::Leave( KErrNoMemory );
	    }
	EUNIT_ASSERT( rtcpEnum == ERTCP_NO_ERROR);
	RTP_EUNIT_ASSERT_EQUALS(iSession->RemoveSdesFromArray(iTranstreamId), KErrNotFound);
	//
	///////////////////////TEST CASE 2///////////////////////////////////
	
	RTP_EUNIT_ASSERT_EQUALS(iSession->RemoveSdesFromArray(iRecvstreamId), KErrNone);
	iSession->iSdesArray->Reset();	
	}
void UT_CRtpSession::UT_CRtpSession_SendRtcpSrPacket2L(  )
    {
    
    // "Test description: Testing RTCP functions with RTCP disabled..." ) );
    
    TBool prevRtcpStatus( iSession->iRtcpEnabled );
    TBuf8<5> dummyBuf8;

    // Store the previous status of RTCP
    iSession->iRtcpEnabled = EFalse;

    EUNIT_ASSERT ( iSession->SendRtcpSrPacketL( NULL ) == KErrNotSupported );
	
    iSession->iRtcpEnabled = ETrue;
   	EUNIT_ASSERT ( iSession->SendRtcpSrPacketL( NULL ) == KErrNotReady );
    iSession->iSessionStarted = ETrue;
    EUNIT_ASSERT ( iSession->SendRtcpSrPacketL( iTranstreamId ) == KErrGeneral);
    
     CRtpTranStream* tempStream( NULL );
    // find stream
    TUint streamAddress = 0;
    TInt ret = iSession->FindStream( iTranstreamId, streamAddress );

    if ( ret == KErrNone )
        {
        // Make sure the stream has received packets
        tempStream = 
            reinterpret_cast<CRtpTranStream*>( streamAddress );
        tempStream->iFlagFirstPkg = EFalse;
        EUNIT_ASSERT ( iSession->SendRtcpSrPacketL( iTranstreamId ) == KErrNone );
        }
   }
   
void UT_CRtpSession::UT_CRtpSession_SendRtcpRrPacketL(  )
    {
   
    // "Test description: Testing RTCP functions with RTCP disabled..." ) );
    
    TBool prevRtcpStatus( iSession->iRtcpEnabled );
    TBuf8<5> dummyBuf8;

    // Store the previous status of RTCP
    iSession->iRtcpEnabled = EFalse;

    EUNIT_ASSERT ( iSession->SendRtcpRrPacketL( NULL ) == KErrNotSupported );

    iSession->iSessionStarted = EFalse;
    iSession->iRtcpEnabled = ETrue;
   	EUNIT_ASSERT ( iSession->SendRtcpRrPacketL( NULL ) == KErrNotReady );
    // Restore the previous RTCP status
    iSession->iSessionStarted = ETrue;
      
    EUNIT_ASSERT ( iSession->SendRtcpRrPacketL( iRecvstreamId ) == KErrNotReady );
    
    CRtpRecvStream* tempStream( NULL );
    // find stream
    TUint streamAddress = 0;
    TInt ret = iSession->FindStream( iRecvstreamId, streamAddress );

    if ( ret == KErrNone )
        {
        // Make sure the stream has received packets
        tempStream = 
            reinterpret_cast<CRtpRecvStream*>( streamAddress );
        tempStream->iFlagFirstPkg = EFalse;
        EUNIT_ASSERT ( iSession->SendRtcpRrPacketL( iRecvstreamId ) == KErrNone );
        }
    }


void UT_CRtpSession::UT_CRtpSession_DoBuildSendRTCPReportIfNotSentL()
	{
	iSession->iRtcpEnabled = ETrue;
	EUNIT_ASSERT(iSession->DoBuildSendRTCPReportIfNotSentL()== ETrue);	
	}
	
void UT_CRtpSession::UT_CRtpSession_DoBuildSendRTCPReportL()
	{
	iSession->iRtcpEnabled = ETrue;
	iSession->DoBuildSendRTCPReportL();
	
	CRtpTranStream* tempStream( NULL );
    // find stream
    TUint streamAddress = 0;
    TInt ret = iSession->FindStream( iTranstreamId, streamAddress );

    if ( ret == KErrNone )
        {
        // Make sure the stream has received packets
        tempStream = 
            reinterpret_cast<CRtpTranStream*>( streamAddress );
        EUNIT_ASSERT(tempStream->iFSentRtcpReport == ETrue);
        }
		
	}
	
void UT_CRtpSession::UT_CRtpSession_BuildSendRTCPReport1L()
	{
	iSession->iRtcpEnabled = ETrue;
	iSession->BuildSendRTCPReportL();
	
	CRtpTranStream* tempStream( NULL );
    // find stream
    TUint streamAddress = 0;
    TInt ret = iSession->FindStream( iTranstreamId, streamAddress );

    if ( ret == KErrNone )
        {
        // Make sure the stream has received packets
        tempStream = 
            reinterpret_cast<CRtpTranStream*>( streamAddress );
        EUNIT_ASSERT(tempStream->iFSentRtcpReport == ETrue);
        }
   	          
	}	
	
void UT_CRtpSession::UT_CRtpSession_BuildSendRTCPReport2L()
	{
	iSession->iRtcpEnabled = ETrue;
	
	
	CRtpTranStream* tempStream( NULL );
    // find stream
                
	TInt totalTxStream( iSession->iStreamTxArray->Count() );
	   
	    //Delete all TX Streams starting with the last one 
    while ( totalTxStream > 0 )
    	{
            
         CRtpTranStream* tempStream = reinterpret_cast<CRtpTranStream*>(
               ( iSession->iStreamTxArray->At( totalTxStream - 1 ).GetStreamAddress() ) );
         delete tempStream;
            
        iSession->iStreamTxArray->Delete( totalTxStream - 1 );
        totalTxStream = iSession->iStreamTxArray->Count();
        }
             
    iSession->iStreamTxArray->Reset();   
    iSession->iStreamTxArray->Compress();
	iSession->BuildSendRTCPReportL();	
	}
void UT_CRtpSession::UT_CRtpSession_SendRTCPReportL(  )
    {
    //increaing the coverage
    iSession->iRtcpEnabled = EFalse;
    iSession->SendRTCPReport();
    //iWriteStatus=ECommsFailed
    iSession->iRtcpEnabled = ETrue;
    if ( iSession->iCommNet->iSender[ERTCPPort] )
    	{
    	iSession->iCommNet->iSender[ERTCPPort]->iWriteStatus=CRtpCommSend::ECommsFailed;
    	}
    iSession->SendRTCPReport();
    }
    
void UT_CRtpSession::UT_CRtpSession_CreateSDESL( )  
	{
	//Add coverage
	iSession->iRtcpEnabled = EFalse;
	iSession->CreateSDES(1,iTranstreamId );
	iSession->iRtcpEnabled = ETrue;
	iSession->CreateSDES(1,iTranstreamId );
	}

void UT_CRtpSession::UT_CRtpSession_ProcessOneRTCPReportSectionL()
	{
	iSession->iRtcpEnabled = ETrue;

    TRtpPacketIOParam dummyExtractParam;
    dummyExtractParam.TRTCP_HEADER.pt=ERTCP_SDES;
    
	HBufC8* srtcpPacket =HBufC8::NewLC(KRTCPPacket2().Length());
	*srtcpPacket=KRTCPPacket2;
    Hex(*srtcpPacket);
    TInt len= KRTCPPacket2().Length();
	srtcpPacket->Des().SetLength(len/2);
	iSession->iPktRtcpRcv->SetSize(len/2);
	iSession->iPktRtcpRcv->iData=const_cast<TUint8*>( srtcpPacket->Des().Ptr() );
	CleanupStack::Pop(srtcpPacket);
	iSession->iRtcpEnabled = ETrue;
	iSession->iPktRtcpRcv->RtpPacketResetPtr();
	EUNIT_ASSERT (iSession->ProcessOneRTCPReportSectionL(
						dummyExtractParam,iRecvstreamId )== ERTCP_PACKET_ERROR);
	//iSession->DeallocateMemoryForSdes( &dummyExtractParam );
	
	dummyExtractParam.TRTCP_HEADER.pt=ERTCP_BYE;
	EUNIT_ASSERT (iSession->ProcessOneRTCPReportSectionL(
						dummyExtractParam,iRecvstreamId )== ERTCP_PACKET_ERROR);

	
	dummyExtractParam.TRTCP_HEADER.pt=ERTCP_APP;
	EUNIT_ASSERT (iSession->ProcessOneRTCPReportSectionL(
						dummyExtractParam,iRecvstreamId )==ERTCP_PACKET_ERROR);
						
	dummyExtractParam.TRTCP_HEADER.pt=ERTCP_SR;
	EUNIT_ASSERT (iSession->ProcessOneRTCPReportSectionL(
						dummyExtractParam,iRecvstreamId )== ERTCP_PACKET_ERROR);
						
	dummyExtractParam.TRTCP_HEADER.pt=ERTCP_RR;
	iSession->RegisterRtcpObserver(*this);
	EUNIT_ASSERT (iSession->ProcessOneRTCPReportSectionL(
						dummyExtractParam,iRecvstreamId )== ERTCP_PACKET_MORE);
	delete srtcpPacket;			
	}
	
void UT_CRtpSession::UT_CRtpSession_ProcessRTCPReportL()
	{
	HBufC8* srtcpPacket =HBufC8::NewLC(KRTCPPacket2().Length());
	*srtcpPacket=KRTCPPacket2;
    Hex(*srtcpPacket);
    TInt len= KRTCPPacket2().Length();
	srtcpPacket->Des().SetLength(len/2);
	iSession->iPktRtcpRcv->SetSize(len/2);
	iSession->iPktRtcpRcv->iData=const_cast<TUint8*>( srtcpPacket->Des().Ptr() );
	CleanupStack::Pop(srtcpPacket);
	iSession->iRtcpEnabled = ETrue;
	EUNIT_ASSERT(iSession->ProcessRTCPReportL()==ERTCP_PACKET_ERROR);
	//special size to cover branch
	srtcpPacket->Des().SetLength(28);
	iSession->iPktRtcpRcv->SetSize(28);
	iSession->iPktRtcpRcv->iData=const_cast<TUint8*>( srtcpPacket->Des().Ptr() );
	iSession->ProcessRTCPReportL();
	delete srtcpPacket;
	
	//Get more branch
	HBufC8* rtcpPacket2 =HBufC8::NewLC(KRTCPPacket3().Length());
	*rtcpPacket2=KRTCPPacket3;
    Hex(*rtcpPacket2);
    rtcpPacket2->Des().SetLength(28);
	iSession->iPktRtcpRcv->SetSize(28);
	iSession->iPktRtcpRcv->iData=const_cast<TUint8*>( rtcpPacket2->Des().Ptr() );
	CleanupStack::Pop(rtcpPacket2);
	EUNIT_ASSERT(iSession->ProcessRTCPReportL()==ERTCP_PACKET_ERROR);
	delete rtcpPacket2;
	//Get Branch to error of FoundRxStream
	HBufC8* rtcpPacket3 =HBufC8::NewLC(KRTCPPacket4().Length());
	*rtcpPacket3=KRTCPPacket4;
    Hex(*rtcpPacket3);
    rtcpPacket3->Des().SetLength(28);
	iSession->iPktRtcpRcv->SetSize(28);
	iSession->iPktRtcpRcv->iData=const_cast<TUint8*>( rtcpPacket3->Des().Ptr() );
	CleanupStack::Pop(rtcpPacket3);
	EUNIT_ASSERT(iSession->ProcessRTCPReportL()==ERTCP_PACKET_ERROR);
	delete rtcpPacket3;
	}

void UT_CRtpSession::UT_CRtpSession_ProcessSDESDataL()
	{
	iSession->iRtcpEnabled = ETrue;

    TRtpPacketIOParam dummyExtractParam;
    TRtpPacketStreamParam streamParam;
    dummyExtractParam.TRTCP_HEADER.pt=ERTCP_SDES;
    streamParam.TRTCP_SDES.SSRC= iTranstreamId;
	EUNIT_ASSERT(iSession->
			ProcessSDESDataL(streamParam, dummyExtractParam, iRecvstreamId)
			== ERTCP_PACKET_ERROR);
	//SSRC for received stream should be remote SSRC
	
	dummyExtractParam.TRTCP_HEADER.pt=ERTCP_SDES;
    streamParam.TRTCP_SDES.SSRC= 0;
	EUNIT_ASSERT(iSession->
		ProcessSDESDataL(streamParam, dummyExtractParam, iRecvstreamId)
		==ERTCP_NO_ERROR);	
		
	// matchResult != KErrNone && iNumOfTxStreams && !iNumOfRxStreams		
	dummyExtractParam.TRTCP_HEADER.pt=ERTCP_SDES;
    streamParam.TRTCP_SDES.SSRC= iTranstreamId;
    iSession->iNumOfRxStreams=0;
	EUNIT_ASSERT(iSession->
			ProcessSDESDataL(streamParam, dummyExtractParam, iRecvstreamId)
			== ERTCP_NO_ERROR);

	
	}
void UT_CRtpSession::UT_CRtpSession_ProcessSDESData2L()
	{
	iSession->iRtcpEnabled = ETrue;	
	TRtpPacketIOParam dummyExtractParam;
    TRtpPacketStreamParam streamParam;
    //result = AddSdesToArrayL( tempSdesAdress,&aExtractParam ) 
	dummyExtractParam.TRTCP_HEADER.pt=ERTCP_SDES;
    streamParam.TRTCP_SDES.SSRC= iTranstreamId;
   	iSession->iSdesArray->Reset();
    iSession->iNumOfRxStreams=0;
    iSession->RegisterRtcpObserver(*this);
	EUNIT_ASSERT(iSession->
			ProcessSDESDataL(streamParam, dummyExtractParam, iRecvstreamId)
			==ERTCP_NO_ERROR);
	}


void UT_CRtpSession::UT_CRtpSession_ProcessRRSectionL()
	{
	
	HBufC8* srtcpPacket =HBufC8::NewLC(KRTCPPacket2().Length());
	*srtcpPacket=KRTCPPacket2;
    Hex(*srtcpPacket);
    TInt len= KRTCPPacket2().Length();
	srtcpPacket->Des().SetLength(len/2);
	iSession->iPktRtcpRcv->SetSize(len/2);
	iSession->iPktRtcpRcv->iData=const_cast<TUint8*>( srtcpPacket->Des().Ptr() );
	CleanupStack::Pop(srtcpPacket);

	
	iSession->iRtcpEnabled = ETrue;
    TRtpPacketIOParam dummyExtractParam;
    TRtpPacketStreamParam streamParam;
    dummyExtractParam.TRTCP_HEADER.pt=ERTCP_RR;
    
	EUNIT_ASSERT(iSession->ProcessRRSectionL(1)== ERTCP_PACKET_ERROR);

	delete srtcpPacket;
	}
	
void UT_CRtpSession::UT_CRtpSession_ProcessSRSectionL()
	{
	
	HBufC8* srtcpPacket =HBufC8::NewLC(KRTCPPacket2().Length());
	*srtcpPacket=KRTCPPacket2;
    Hex(*srtcpPacket);
    TInt len= KRTCPPacket2().Length();
	srtcpPacket->Des().SetLength(len/2);
	iSession->iPktRtcpRcv->SetSize(len/2);
	iSession->iPktRtcpRcv->iData=const_cast<TUint8*>( srtcpPacket->Des().Ptr() );
	CleanupStack::Pop(srtcpPacket);

	
	iSession->iRtcpEnabled = ETrue;
    TRtpPacketIOParam dummyExtractParam;
    TRtpPacketStreamParam streamParam;
    dummyExtractParam.TRTCP_HEADER.pt=ERTCP_SR;
    
	EUNIT_ASSERT(iSession->ProcessSRSectionL(1)== ERTCP_PACKET_ERROR);

	delete srtcpPacket;
	}
void UT_CRtpSession::UT_CRtpSession_ProcessBYESectionL()
	{
	
	HBufC8* srtcpPacket =HBufC8::NewLC(KRTCPPacket2().Length());
	*srtcpPacket=KRTCPPacket2;
    Hex(*srtcpPacket);
    TInt len= KRTCPPacket2().Length();
	srtcpPacket->Des().SetLength(len/2);
	iSession->iPktRtcpRcv->SetSize(len/2);
	iSession->iPktRtcpRcv->iData=const_cast<TUint8*>( srtcpPacket->Des().Ptr() );
	CleanupStack::Pop(srtcpPacket);

	
	iSession->iRtcpEnabled = ETrue;
    TRtpPacketIOParam dummyExtractParam;
    TRtpPacketStreamParam streamParam;
    dummyExtractParam.TRTCP_HEADER.pt=ERTCP_BYE;
    
	EUNIT_ASSERT(iSession->ProcessBYESectionL()== ERTCP_PACKET_ERROR);

	delete srtcpPacket;
	}	
		
void UT_CRtpSession::UT_CRtpSession_GetRtpSocketL(  )
    {
       // Test that the session ID is updated
    //"") );
    // "Test description: Test that the session ID is correct" ) );
    // "CRtpSession::GetSessionId()" ) );
    if ( iSession->GetSessionId() != 1 )
        {
        // "Invalid session ID" ) );
        // "CRtpSession::GetSessionId() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    // "CRtpSession::GetSessionId() - passed" ) );

    // Test getting the RTP socket without communications 
    // "" ) );
    // "Test description: Test getting the RTP socket" ) );
    // "CRtpSession::GetRtpSocket()" ) );
    CRtpComm* tempComm = iSession->iCommNet;
    iSession->iCommNet = NULL;
    if ( iSession->GetRtpSocket() )
        {
        // "Should not return any value" ) );
        // "CRtpSession::GetRtpSocket() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    // Test getting the RTP socket with communications 
    iSession->iCommNet = tempComm;

    TSockAddr addr1;
    TSockAddr addr2;
    RSocket* sock1 = iSession->GetRtpSocket();
    sock1->RemoteName( addr1 );
    RSocket* sock2 = tempComm->GetSocket( ERTPPort );
    sock2->RemoteName( addr2 );
    if ( addr1 != addr2 )
        {
        // "Sockets do not match" ) );
        // "CRtpSession::GetRtpSocket() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    // "CRtpSession::GetRtpSocket() - passed" ) );

    // Test getting the RTCP socket without communications 
    // "" ) );
    // "Test description: Test getting the RTCP socket" ) );
    // "CRtpSession::GetRtcpSocket()" ) );
    iSession->iCommNet = NULL;
    if ( iSession->GetRtcpSocket() )
        {
        // "Should not return any value" ) );
        // "CRtpSession::GetRtcpSocket() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    // Test getting the RTCP socket with communications 
    iSession->iCommNet = tempComm;

    sock1 = iSession->GetRtcpSocket();
    sock1->RemoteName( addr1 );
    sock2 = tempComm->GetSocket( ERTCPPort );
    sock2->RemoteName( addr2 );
    if ( addr1 != addr2 )
        {
        // "Sockets do not match" ) );
        // "CRtpSession::GetRtcpSocket() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    // "CRtpSession::GetRtcpSocket() - passed" ) );

    // Test getting the magic key of a TStream 
    // "" ) );
    // "Test description: Test getting the magic key of a TStream" ) );
    // "TStream::GetMagicKey()" ) );
    TRtpId magicKey( 5 );
    TStream stream( magicKey );    
    if ( stream.GetMagicKey() != magicKey )
        {
        // "Magic keys do not match" ) );
        // "TStream::GetMagicKey() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    // "TStream::GetMagicKey() - passed" ) );
    }

void UT_CRtpSession::UT_CRtpSession_GetRtcpSocketL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpSession::UT_CRtpSession_GetStreamStatisticsL(  )
    {
       //"") );
    // "Test description: Test getting stream statistics" ) );
    // "CRtpSession::GetStreamStatistics()" ) );

    TInt error( KErrNone );
    TRtpPeerStat stat1;
    TRtpPeerStat stat2;
    stat1.iNumPacketsSent = 42;
    stat1.iCumNumOctetsSent = 42;
    stat1.iRoundTripDelay = 42;
    stat1.iTxBandwidth = 42;
    stat1.iCumNumPacketsLost = 42;
    stat1.iFractionLost = 42;
    stat1.iArrivalJitter = 42;
    stat1.iRxBandwidth = 42;
    stat1.iChannelBufferSize = 42;
    stat1.iNTPTimeStampSec = 42;
    stat1.iNTPTimeStampFrac = 42;
    stat1.iTimeStamp = 42;
    stat2 = stat1;

    // Try to get statistics using invalid ID
    error = iSession->GetStreamStatistics( 3948 /*invalid stream*/, stat1 );
    if ( error != KErrNotFound )
        {
        // "Should not have found stream using invalid ID" ) );
        // "CRtpSession::GetStreamStatistics() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    if ( !PeerStatsEqual( stat1, stat2 ) )
        {
        // "Should not have updated statistics" ) );
        // "CRtpSession::GetStreamStatistics() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    // Get statistics for Tx stream (ID 1)
    error = iSession->GetStreamStatistics( 1, stat1 );
    if ( error != KErrNone )
        {
        // "Should have found Tx stream" ) );
        // "CRtpSession::GetStreamStatistics() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    if ( PeerStatsEqual( stat1, stat2 ) )
        {
        // "Tx statistics not properly copied" ) );
        // "CRtpSession::GetStreamStatistics() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    // Clear the statistics (cannot verify, besides that is the concern of CRtpStream)
    stat1 = stat2;

    // Get statistics for Rx stream (ID 1)
    error = iSession->GetStreamStatistics( 1, stat1 );
    if ( error != KErrNone )
        {
        // "Should have found Rx stream" ) );
        // "CRtpSession::GetStreamStatistics() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    if ( PeerStatsEqual( stat1, stat2 ) )
        {
        // "Rx statistics not properly copied" ) );
        // "CRtpSession::GetStreamStatistics() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    // "CRtpSession::GetStreamStatistics() - passed" ) );
   
    }

void UT_CRtpSession::UT_CRtpSession_SetRtcpParametersL(  )
    {
    
    // "Test description: Testing setting the RTCP parameters" ) );
    // "CRtpSession::SetRtcpParameters( TRtcpParams& )" ) );

    TRtcpParams params;

    // Give values inside the acceptable range
    params.iSessionBWidth = KRtpMinimumBandWidth + 10;
    params.iRtcpFraction = 0.5;

    iSession->SetRtcpParameters( params );
    if ( iSession->iBandWidth != params.iSessionBWidth ||
         iSession->iFraction != params.iRtcpFraction )
        {
        // "Values not updated" ) );
        // "CRtpSession::SetRtcpParameters() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    // Give values outside the acceptable range -> will be set to the defaults
    params.iSessionBWidth = KRtpMinimumBandWidth - 1;
    params.iRtcpFraction = 0;

    iSession->SetRtcpParameters( params );
    if ( iSession->iBandWidth != KRtpDefaultBandWidth ||
         iSession->iFraction != KRtpDefaultRtcpFrac )
        {
        // "Should have set default values" ) );
        // "CRtpSession::SetRtcpParameters() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    // "CRtpSession::SetRtcpParameters() - passed" ) );

    }

void UT_CRtpSession::UT_CRtpSession_GetSessionIdL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpSession::UT_CRtpSession_StopRtcpSendingL(  )
    {
     // "Test description: Test stopping the RTCP timer" ) );
    // "CRtpSession::StopRtcpSending()" ) );
    
    // Store the current timer somewhere
    CRtpTimer* tmpTimer = iSession->iSndRtcpTimer;

    // Create a new timer with this object as listener
    iSession->iSndRtcpTimer = CRtpTimer::NewL( CActive::EPriorityStandard, *this );

    // Start our own timer as watchdog
    RTimer localTimer;
    localTimer.CreateLocal();
    TRequestStatus localTimerStatus;
    localTimer.After( localTimerStatus, 100000 );

    // Start the session's timer with a longer delay
    iSession->iSndRtcpTimer->After( 200000 );

    // Cancel the session's timer
    iSession->StopRtcpSending();

    User::WaitForRequest( localTimerStatus );
    if ( iSession->iSndRtcpTimer->IsActive() )
        {
        // "Timer not deactivated" ) );
        // "CRtpSession::StopRtcpSending() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    // Restore the session's previous timer
    delete iSession->iSndRtcpTimer;
    localTimer.Close();
    iSession->iRtcpEnabled=ETrue;
    
    iSession->iSndRtcpTimer = tmpTimer;
    

    }

void UT_CRtpSession::UT_CRtpSession_ResumeRtcpSendingL(  )
    {
    EUNIT_ASSERT( ETrue);
    }

void UT_CRtpSession::UT_CRtpSession_IsRtcpSendingSuspendedL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpSession::UT_CRtpSession_CheckRemoteSsrcL(  )
    {
    TInt result( KErrNone );
	// "Test description: Test SSRC collision detection without streams" ) );
    // "CRtpSession::CheckRemoteSsrc()" ) );
    
    TRtpSSRC ssrc( 10 );

    // There are no TX streams, so this should return KErrNone
    TInt ssrcChanged( iSession->CheckRemoteSsrcL( ssrc ) );
    EUNIT_ASSERT ( ssrcChanged == KErrNone || ssrc == 10 );
    ///////////////////////TEST CASE 2///////////////////////////////////
    ssrc=1;
    iSession->iSSRCJumps =1;
    EUNIT_ASSERT( iSession->CheckRemoteSsrcL(ssrc)==KErrNone) ; 
    ///////////////////////TEST CASE 3///////////////////////////////////  
    iSession->iSSRCJumps=0; 
    ssrc=5555;
    EUNIT_ASSERT( iSession->CheckRemoteSsrcL(ssrc)==1) ; 
        }

void UT_CRtpSession::UT_CRtpSession_OnExpiredL(  )
    {
    // "Test description: Test processing RTCP reports" ) );
    // "CRtpSession::OnExpired()" ) );

    // Disable RTCP, so this should do nothing    
    TBool tempRtcpStatus = iSession->iRtcpEnabled;
    iSession->iRtcpEnabled = EFalse;
    iSession->OnExpiredL( 5 /* dummy value */);

    // Restore the previous values
    iSession->iRtcpEnabled = tempRtcpStatus;
    
    // "CRtpSession::OnExpired() - passed" ) );
    }
void UT_CRtpSession::UT_CRtpSession_ScheduleRtcpSendL()
    {
    //"") );
    // "Test description: Test scheduling RTCP transmission" ) );
    // "CRtpSession::ScheduleRtcpSend()" ) );

    // Disable RTCP, so this should do nothing    
    TBool tempRtcpStatus = iSession->iRtcpEnabled;
    iSession->iRtcpEnabled = EFalse;
    iSession->ScheduleRtcpSendL();

    // Restore the previous values
    iSession->iRtcpEnabled = tempRtcpStatus;
    
    // Disable RTCP temporarily, so this should do nothing    
    tempRtcpStatus = iSession->iRtcpSendingSuspended;
    iSession->iRtcpSendingSuspended = ETrue;
    iSession->ScheduleRtcpSendL();

    // Restore the previous values
    iSession->iRtcpSendingSuspended = tempRtcpStatus;
	EUNIT_ASSERT(ETrue);
    }
    

void UT_CRtpSession::UT_CRtpSession_OnRtpReceivedLL(  )
    {
    CRtpPacket* packet = CRtpPacket::NewL( 2048, iProfileRTPTimeRates );
    TRtpPacketStreamParam aStreamParam;
    TRtpPacketIOParam aIoParam;
    TBuf8<10> data;
    data.Format( _L8( "dummydata" ) );

    // Test receiving too short RTP packet
    // Set the session's receive buffer to the packet data
    iSession->iRtpRecvBuf.Set( const_cast<unsigned char*>( data.Ptr() ),
                               data.Length(), data.MaxLength() );

    TInt tempSize = iSession->iPktRcv->Size();
    
    // Packet should not be processed, as it is too short
    iSession->OnRtpReceivedL();

    if ( tempSize != iSession->iPktRcv->Size() )
        {
        // "Invalid (too short) packet was processed" ) );
        EUNIT_ASSERT(EFalse);
        }
    
    // Set the parameters
    aStreamParam.TRTP.SSRC = 2002;
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
    
    // Test receiving valid RTP packet
    // "Test description: Test receiving valid RTP packet" ) );
    
    // Set the session's receive buffer to the packet data
    iSession->iRtpRecvBuf.Set( packet->Data(), packet->Size(), packet->Size() );
	tempSize = iSession->iPktRcv->Size();
    
    // Packet should be processed
    iPacketsExpected++;
    iSession->OnRtpReceivedL();

    if ( tempSize == iSession->iPktRcv->Size() )
        {
        // "Valid packet was not processed" ) );
        // "CRtpSession::OnRtpReceivedL() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
     packet->RtpPacketResetPtr();    
  
   

    // Set the session's receive buffer to the packet data
    iSession->iRtpRecvBuf.Set( packet->Data(), packet->Size(), packet->Size() );

    // Send the packet again to reach more branches
    iPacketsExpected++;
    iSession->OnRtpReceivedL();
     
    
  	  //above before modified  

    //test with new receive baned stream     
  	packet->RtpPacketResetPtr();    
    CRtpRecvStream* tempStream = reinterpret_cast<CRtpRecvStream*>(
                ( iSession->iStreamRxArray->At( 0).GetStreamAddress() ) );
    delete tempStream;
    iSession->iStreamRxArray->Delete(0 );
    data.Format( _L8( "testdata2" ) );
      // Set the parameters
	TRtpSSRC ssrc( 5005 );
    aStreamParam.TRTP.SSRC = 5005;
    aStreamParam.TRTP.payload= 20;
    aStreamParam.TRTP.seqNum = 55;
    aStreamParam.TRTP.timeStamp = 99;
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
 	iSession->iRxSSRCArray.Close();
 	packet->SetType( ERTP );
 	packet->RtpPacketBuild( &aStreamParam, &aIoParam );
 	User::LeaveIfError (iSession->iRxSSRCArray.Append(iSession->iPktRcv->RtpPacketGetSSRC()));
    //"") );
    // "Test description: Test NewRxStream with baned SSRC" ) );
     // Set the session's receive buffer to the packet data
	iSession->iRtpRecvBuf.Set( packet->Data(), packet->Size(), packet->Size() );
 
    tempSize = iSession->iPktRcv->Size();
    
    // Packet should be baned
    iSession->OnRtpReceivedL();
    
	  	   
  	//Test Rx stream with new SSRC
  	packet->RtpPacketResetPtr();   
	iSession->iRxSSRCArray.Close();
    // Build the packet
    packet->SetType( ERTP );
    packet->RtpPacketBuild( &aStreamParam, &aIoParam );
    
    // Test receiving valid RTP packet
    // "Test description: Test NewRxStream with New SSRC" ) );
    
    // Set the session's receive buffer to the packet data
    iSession->iRtpRecvBuf.Set( packet->Data(), packet->Size(), packet->Size() );
	//iPktRcv has been set with previus sending	
    tempSize = iSession->iPktRcv->Size();
    
    
    // Packet should be processed
    iPacketsExpected++;
    iSession->OnRtpReceivedL();

    if ( tempSize != iSession->iPktRcv->Size() )
        {
        // "Valid packet was not processed" ) );
        // "CRtpSession::OnRtpReceivedL() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    delete packet;

    }

void UT_CRtpSession::UT_CRtpSession_OnRtpReceivedL2L(  )
    {
        CRtpPacket* packet = CRtpPacket::NewL( 2048, iProfileRTPTimeRates );
    TRtpPacketStreamParam aStreamParam;
    TRtpPacketIOParam aIoParam;
    TBuf8<10> data;
    data.Format( _L8( "dummydata" ) );

    // Test receiving too short RTP packet
    //"") );
    // "Test description: Test receiving too short RTP packet" ) );
    // "CRtpSession::OnRtpReceivedL()" ) );

    // Set the session's receive buffer to the packet data
    iSession->iRtpRecvBuf.Set( const_cast<unsigned char*>( data.Ptr() ),
                               data.Length(), data.MaxLength() );

    TInt tempSize = iSession->iPktRcv->Size();
    
    // Packet should not be processed, as it is too short
    iSession->OnRtpReceivedL();

    if ( tempSize != iSession->iPktRcv->Size() )
        {
        // "Invalid (too short) packet was processed" ) );
        // "CRtpSession::OnRtpReceivedL() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    
    // "CRtpSession::OnRtpReceivedL() - passed" ) );

    // Set the parameters
    aStreamParam.TRTP.SSRC = 2002;
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
    
    // Test receiving valid RTP packet
    //"") );
    // "Test description: Test receiving valid RTP packet" ) );
    // "CRtpSession::OnRtpReceivedL()" ) );

    // Set the session's receive buffer to the packet data
    iSession->iRtpRecvBuf.Set( packet->Data(), packet->Size(), packet->Size() );

    tempSize = iSession->iPktRcv->Size();
    
    // Packet should be processed
    iPacketsExpected++;
    iSession->OnRtpReceivedL();

    if ( tempSize == iSession->iPktRcv->Size() )
        {
        // "Valid packet was not processed" ) );
        // "CRtpSession::OnRtpReceivedL() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
     packet->RtpPacketResetPtr();    
  
   

    // Set the session's receive buffer to the packet data
    iSession->iRtpRecvBuf.Set( packet->Data(), packet->Size(), packet->Size() );

    // Send the packet again to reach more branches
    iPacketsExpected++;
    iSession->OnRtpReceivedL();
     
    
  	  //above before modified  

    //test with new receive baned stream     
  	packet->RtpPacketResetPtr();    
    CRtpRecvStream* tempStream = reinterpret_cast<CRtpRecvStream*>(
                ( iSession->iStreamRxArray->At( 0).GetStreamAddress() ) );
    delete tempStream;
    iSession->iStreamRxArray->Delete(0 );
    data.Format( _L8( "testdata2" ) );
      // Set the parameters
	TRtpSSRC ssrc( 5005 );
    aStreamParam.TRTP.SSRC = 5005;
    aStreamParam.TRTP.payload= 20;
    aStreamParam.TRTP.seqNum = 55;
    aStreamParam.TRTP.timeStamp = 99;
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
 	iSession->iRxSSRCArray.Close();
 	packet->SetType( ERTP );
 	packet->RtpPacketBuild( &aStreamParam, &aIoParam );
 	User::LeaveIfError (iSession->iRxSSRCArray.Append(iSession->iPktRcv->RtpPacketGetSSRC()));
    //"") );
    // "Test description: Test NewRxStream with baned SSRC" ) );
    // "CRtpSession::OnRtpReceivedL()" ) );

     // Set the session's receive buffer to the packet data
	iSession->iRtpRecvBuf.Set( packet->Data(), packet->Size(), packet->Size() );
 
    tempSize = iSession->iPktRcv->Size();
    
    // Packet should be baned
    iSession->OnRtpReceivedL();
    
	  	   
  	//Test Rx stream with new SSRC
  	packet->RtpPacketResetPtr();   
	iSession->iRxSSRCArray.Close();
    // Build the packet
    packet->SetType( ERTP );
    packet->RtpPacketBuild( &aStreamParam, &aIoParam );
    
    // Test receiving valid RTP packet
    //"") );
    // "Test description: Test NewRxStream with New SSRC" ) );
    // "CRtpSession::OnRtpReceivedL()" ) );

    // Set the session's receive buffer to the packet data
    iSession->iRtpRecvBuf.Set( packet->Data(), packet->Size(), packet->Size() );
	//iPktRcv has been set with previus sending	
    tempSize = iSession->iPktRcv->Size();
    
    
    // Packet should be processed
    iPacketsExpected++;
    iSession->OnRtpReceivedL();

    if ( tempSize != iSession->iPktRcv->Size() )
        {
        // "Valid packet was not processed" ) );
        // "CRtpSession::OnRtpReceivedL() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    
   
    //below before modified
    // "CRtpSession::OnRtpReceivedL() - passed" ) );

    delete packet;

    }
void UT_CRtpSession::UT_CRtpSession_OnRtpReceivedL3L(  )
    {
    CRtpPacket* packet = CRtpPacket::NewL( 2048, iProfileRTPTimeRates );
    TRtpPacketStreamParam aStreamParam;
    TRtpPacketIOParam aIoParam;
    TBuf8<10> data;
    data.Format( _L8( "dummydata" ) );

    // Test receiving too short RTP packet
    //"") );
    // "Test description: Test receiving too short RTP packet" ) );
    // "CRtpSession::OnRtpReceivedL()" ) );

    // Set the session's receive buffer to the packet data
    iSession->iRtpRecvBuf.Set( const_cast<unsigned char*>( data.Ptr() ),
                               data.Length(), data.MaxLength() );

    TInt tempSize = iSession->iPktRcv->Size();
    
    // Packet should not be processed, as it is too short
    iSession->OnRtpReceivedL();

    if ( tempSize != iSession->iPktRcv->Size() )
        {
        // "Invalid (too short) packet was processed" ) );
        // "CRtpSession::OnRtpReceivedL() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    
    // "CRtpSession::OnRtpReceivedL() - passed" ) );

    // Set the parameters
    aStreamParam.TRTP.SSRC = 2002;
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
    
    aIoParam.TRTP.fHeaderExtension = 1;    
    aIoParam.TRTP.extension.data = const_cast<TUint8*>(data.Ptr());
    aIoParam.TRTP.extension.length = data.Length();
   
    
    
    // Build the packet
    packet->SetType( ERTP );
    packet->RtpPacketBuild( &aStreamParam, &aIoParam );
    
   iSession->iRtpRecvBuf.Set( packet->Data(), packet->Size(), packet->Size() );

    tempSize = iSession->iPktRcv->Size();
    
    // Packet should be processed
    iPacketsExpected++;
    
    iSession->OnRtpReceivedL();

    if ( tempSize == iSession->iPktRcv->Size() )
        {
        // "Valid packet was not processed" ) );
        // "CRtpSession::OnRtpReceivedL() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
     packet->RtpPacketResetPtr();    
  
   

    // Set the session's receive buffer to the packet data
    iSession->iRtpRecvBuf.Set( packet->Data(), packet->Size(), packet->Size() );

    // Send the packet again to reach more branches
    iPacketsExpected++;
    iSession->OnRtpReceivedL();
     
    
  
    delete packet;

    }    

void UT_CRtpSession::UT_CRtpSession_OnRtcpReceivedLL(  )
    {
        TBuf8<10> data;
    TInt rtcpErrors( iSession->iRtcpErrors );
    data.Format( _L8( "foo" ) );

    // Store the current receive buffer pointer
    TPtr8 tempPtr( iSession->iRtcpRecvBuf );

    // Set the session's receive buffer to the packet data
    iSession->iRtcpRecvBuf.Set( const_cast<unsigned char*>( data.Ptr() ),
                               data.Length(), data.MaxLength() );

    // This should only cause an error
    iSession->OnRtcpReceivedL();
    if ( iSession->iPktRtcpRcv->Size() != iSession->iRtcpRecvBuf.Size() )
        {
        // "Session internal pameters not updated" ) );
        // "CRtpSession::OnRtcpReceivedL() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    if ( rtcpErrors == iSession->iRtcpErrors )
        {
        // "Packet processing should have failed" ) );
        // "CRtpSession::OnRtcpReceivedL() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    /////////////////////////////////////////////////////////

    CRtpPacket* packet = iSession->iPktRtcpRcv;//CRtpPacket::NewL( 2048, iProfileRTPTimeRates );
    TRtpPacketStreamParam aStreamParam;
    TRtpPacketIOParam aIoParam;

    // Make a note of how many errors we've had so far
    rtcpErrors = iSession->iRtcpErrors;

    // Use our current Tx stream
    CRtpTranStream* tranStream = ( CRtpTranStream* )
                        ( iSession->iStreamTxArray->At( 0 ).GetStreamAddress() );

    // Test receiving SR packet without RTCP header
    //"") );
    // "Test description: Test receiving SR packet with only RTCP header" ) );
    // "CRtpSession::OnRtcpReceivedL()" ) );

    // Create the RTCP report    
    tranStream->RtpStreamCreateRtcpReportSection( packet );

    // Make sure the packet can be processed
    packet->RtpPacketResetPtr();
    packet->SetSize( 24 ); // size of SR section
    packet->SetType( ERTCP_SR );

    // Set the session's receive buffer to the packet data
    iSession->iRtcpRecvBuf.Set( packet->Data(), packet->Size(), packet->Size() );

    // This should cause an error, because there is no RTCP header in the packet
    iSession->OnRtcpReceivedL();
    if ( rtcpErrors == iSession->iRtcpErrors )
        {
        // "Packet processing should have failed (no RTCP header)" ) );
        // "CRtpSession::OnRtcpReceivedL() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    // "CRtpSession::OnRtcpReceivedL() - passed" ) );

    /////////////////////////////////////////////////////////

    // This time, build an RTCP header
    TRtpPacketStreamParam streamParam;
    TRtpPacketIOParam inParam;

    // Make a note of how many errors we've had so far
    rtcpErrors = iSession->iRtcpErrors;

    inParam.TRTCP_HEADER.pt = ERTCP_SR;
    streamParam.TRTCP_HEADER.SSRC = iSession->iDefaultSSRC;
    inParam.TRTCP_HEADER.sourceCount = 0; /* num of Rx streams */
    inParam.TRTCP_HEADER.length = 6 * inParam.TRTCP_HEADER.sourceCount;/* num of 32-bit fields */

    packet->SetType( ERTCP_HEADER );
    packet->RtpPacketReset();
    packet->RtpPacketBuild( &streamParam, &inParam );    

    // Set the session's receive buffer to the packet data
    iSession->iRtcpRecvBuf.Set( packet->Data(), packet->Size(), packet->Size() );

    //"") );
    // "Test description: Test receiving SR packet without RTCP header" ) );
    // "CRtpSession::OnRtcpReceivedL()" ) );

    // This should cause an error, because there is only an RTCP header in the packet
    iSession->OnRtcpReceivedL();
    if ( rtcpErrors != iSession->iRtcpErrors )
        {
        // "Packet processing should not have failed (RTCP header)" ) );
        // "CRtpSession::OnRtcpReceivedL() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    // Restore the previous receive buffer pointer
    iSession->iRtcpRecvBuf.Set( tempPtr );

    //delete packet;

    // "CRtpSession::OnRtcpReceivedL() - passed" ) );


    // Test reception error
    //"") );
    // "Test description: Test reception error" ) );
    // "CRtpSession::OnReceptionError()" ) );

    iSession->OnReceptionError( ERTCPPort, KErrDisconnected );
    }

// -----------------------------------------------------------------------------
// Test building RTCP reports without streams
// -----------------------------------------------------------------------------
//
void UT_CRtpSession::UT_CRtpSession_BuildRTCPReportWithoutStreams()
    {
    //"") );
    // "Test description: Test building RTCP reports without streams" ) );
    // "CRtpSession::BuildRTCPReport( TRtcpParams*, TInt )" ) );

    TInt result( iSession->BuildRTCPReport( NULL, 0 ) );
    if ( result != KErrGeneral )
        {
        // "Unexpected result" ) );
        // "CRtpSession::BuildRTCPReport() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    // "CRtpSession::BuildRTCPReport() - passed" ) );
    EUNIT_ASSERT(ETrue);
    }

// -----------------------------------------------------------------------------
// Test building RTCP reports when no packets received
// -----------------------------------------------------------------------------
//
void UT_CRtpSession::UT_CRtpSession_BuildRTCPReportWithoutPackets()
    {
    //"") );
    // "Test description: Test building RTCP reports when no packets received" ) );
    // "CRtpSession::BuildRTCPReport( TRtcpParams*, TInt )" ) );

    TInt result( iSession->BuildRTCPReport( NULL, 0 ) );
    if ( result != KErrGeneral )
        {
        // "Unexpected result" ) );
        // "CRtpSession::BuildRTCPReport() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    TUint txAddress( 0 );
    iSession->FindStream( 1/* Tx stream id */, txAddress );
    CRtpTranStream* tx( reinterpret_cast<CRtpTranStream*>( txAddress ) );
    result = iSession->BuildRTCPReport( tx, 0 );

    if ( result != KErrGeneral )
        {
        // "Unexpected result" ) );
        // "CRtpSession::BuildRTCPReport() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    // "CRtpSession::BuildRTCPReport() - passed" ) );
    EUNIT_ASSERT(ETrue);
    }


void UT_CRtpSession::UT_CRtpSession_OnReceptionErrorL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpSession::UT_CRtpSession_OnPacketRejectedL(  )
    {
    iSession ->OnPacketRejected(ERTPPort);
    iSession->OnPacketRejected(ERTCPPort);
    }
    
void UT_CRtpSession::UT_CRtpSession_DeallocateMemoryForSdesL( )    
	{
    CRtpPacket* packet = CRtpPacket::NewL( 2048, iProfileRTPTimeRates );
    TRtpPacketStreamParam aStreamParam;
    TRtpPacketIOParam aIoParam;
    TBuf8<10> data;
    data.Format( _L8( "dummydata" ) );

   
    iSession->iRtpRecvBuf.Set( const_cast<unsigned char*>( data.Ptr() ),
                               data.Length(), data.MaxLength() );

    TInt tempSize = iSession->iPktRcv->Size();
    
    // Packet should not be processed, as it is too short
    iSession->OnRtpReceivedL();

    EUNIT_ASSERT(tempSize == iSession->iPktRcv->Size());
    
    
    // Set the parameters
    aStreamParam.TRTP.SSRC = 2002;
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
    packet->SetType( ERTCP_SDES );
    packet->RtpPacketBuild( &aStreamParam, &aIoParam );
    
   
    iSession->iRtcpRecvBuf.Set( packet->Data(), packet->Size(), packet->Size() );

    tempSize = iSession->iPktRcv->Size();
    
    // Packet should be processed
    iPacketsExpected++;
    iSession->OnRtpReceivedL();
	iSession->DeallocateMemoryForSdes(&aIoParam);
	iSession->ProcessSDESSectionL( 0 );
 
    delete packet;
  
	}

void UT_CRtpSession::UT_CRtpSession_MatchSSRCToStreamL( )
	{
	TRtpPacketStreamParam streamParam;
	TRtpPacketIOParam extractParam;

    TRtpRtcpEnum parseResult = ERTCP_NO_ERROR;
	iSession->iPktRtcpRcv->SetType( ERTCP_BYE );
    //parseResult = iSession->iPktRtcpRcv->RtpPacketProcess( &streamParam, &extractParam );



    // find the receiving stream that belongs to remote participant SSRC
    CRtpSession::TStreamType streamType;
    TUint rcvStreamAddress = NULL;
    TRtpId rcvStreamId = 0;
    TInt ret = KErrNone;
    //Return KErrNotFound
	ret = iSession->MatchSSRCToStream( rcvStreamAddress, streamParam.TRTCP_BYE.SSRC,
                             streamType );	
	}
	
void UT_CRtpSession::UT_CRtpSession_NextRTCPIntervalL()
	{
	iSession->NextRtcpInterval();
	
	}	
	
void UT_CRtpSession::UT_CRtpSession_MatchSSRCToStream2L()
	{
	
    TRtpSSRC tempSSRC( NULL );
    TUint streamAddress( 0 );
    CRtpSession ::TStreamType streamType;
    TRtpSSRC ssrcOut( 1234 );
    
    TRtpPayloadType payloadType( 0 );
    TInt error(KErrNone);
    error =iSession->CreateTransmitStreamL( iTranstreamId, payloadType, ssrcOut );
	RTP_EUNIT_ASSERT_EQUALS(error, KErrNone);
	
	error = iSession->CreateReceiveStreamL( iRecvstreamId, payloadType );                    
	RTP_EUNIT_ASSERT_EQUALS(error, KErrNone);
	
	iSession->MatchSSRCToStream(streamAddress, 
                                ssrcOut, 
                                streamType );
                                
    EUNIT_ASSERT(CRtpSession ::ETxStream ==streamType);
	
	}		
void UT_CRtpSession::UT_CRtpSession_RcvStreamReceivedRtpPacketL(  )	
	{
	EUNIT_ASSERT(iSession->RcvStreamReceivedRtpPacket(0)==EFalse);
	CRtpRecvStream* tempRcvStream = reinterpret_cast<CRtpRecvStream*>(
        iSession->iStreamRxArray->At( 0).GetStreamAddress() );
	tempRcvStream->iFlagReceivedRTPPackets=ETrue;
	EUNIT_ASSERT(iSession->RcvStreamReceivedRtpPacket(0)==ETrue);
	}
void UT_CRtpSession::UT_CRtpSession_GetRcvStreamByIndexL( )
    {
	CRtpRecvStream* tempRcvStream = reinterpret_cast<CRtpRecvStream*>(
        iSession->GetRcvStreamByIndex(0));
    }

void UT_CRtpSession::UT_CRtpSession_CreateNewRecvStreamLL()
	{
	CRtpPacket* packet = CRtpPacket::NewL( 2048, iProfileRTPTimeRates );
    TRtpPacketStreamParam aStreamParam;
    TRtpPacketIOParam aIoParam;
    TBuf8<10> data;
    data.Format( _L8( "dummydata" ) );

    // Test receiving too short RTP packet
    // Set the session's receive buffer to the packet data
    iSession->iRtpRecvBuf.Set( const_cast<unsigned char*>( data.Ptr() ),
                               data.Length(), data.MaxLength() );
	 // stream will not be created because the remote addr != addr
    iSession->CreateNewRecvStreamL() ;
    //stream will not be created if SSRC=0
     // Set the parameters
    iSession->iPktRcv->RtpPacketResetPtr(); 
    aStreamParam.TRTP.SSRC =0;
    aStreamParam.TRTP.payload= 0;
    aStreamParam.TRTP.seqNum = 0;
    aStreamParam.TRTP.timeStamp = 0;
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
    CRtpPacket* tmppacket =iSession->iPktRcv;
    delete tmppacket;
    iSession->iPktRcv=packet;
    // Test receiving valid RTP packet
    // Set the session's receive buffer to the packet data
    iSession->iRtpRecvBuf.Set( packet->Data(), packet->Size(), packet->Size() );
	//Stream will not created  
    iSession->CreateNewRecvStreamL();

   	//Set Remote Addr= iFrom Addr in CommRecv
	TInetAddr remoteAddr( INET_ADDR( 127, 0, 0, 1 ), 5000 );
	iSession->iCommNet->iReceiver[ERTPPort]->iFromAddr=remoteAddr ;    
    // Set the parameters
    packet->RtpPacketResetPtr(); 
    
    
    aStreamParam.TRTP.SSRC = 2002;
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
    
    // Test receiving valid RTP packet
    // Set the session's receive buffer to the packet data
    iSession->iRtpRecvBuf.Set( packet->Data(), packet->Size(), packet->Size() );
	
    //Stream will be created
    iSession->CreateNewRecvStreamL();
    packet->RtpPacketResetPtr();    
  
  	// Set the session's receive buffer to the packet data
  	iSession->iNumOfRxStreams=101;
    iSession->iRtpRecvBuf.Set( packet->Data(), packet->Size(), packet->Size() );
    iSession->CreateNewRecvStreamL();
    
	}

void UT_CRtpSession::UT_CRtpSession_SendSrtcpReportLL()
	{
	EUNIT_ASSERT_SPECIFIC_LEAVE( iSession->SendSRTCPReportL(0), KErrNotSupported );
	}
	
void UT_CRtpSession::ErrorNotify( TInt /*aErrCode*/ )
	{
	
	}
void UT_CRtpSession::RtpPacketReceived( TRtpId /*aStreamId*/,
                                const TRtpRecvHeader& /*aHeaderInfo*/, 
                                const TDesC8& /*aPayloadData*/ )
{
    if ( iPacketsExpected <= 0 )
        {
		EUNIT_ASSERT(EFalse);
        }
    else
        {
        iPacketsExpected--;
        }
}
    	// from MRtcpObserver

void UT_CRtpSession::SdesReceived( TRtpSSRC /*aSSRC*/,
                           const TRtpSdesParams& /*aParams*/ )
	{
	
	}    
void UT_CRtpSession::ByeReceived( TRtpId /*aStreamId*/, TRtpSSRC /*aSSRC*/,
                          const TDesC8& /*aReason*/ )
	{
	
	}
void UT_CRtpSession::AppReceived( TRtpId /*aStreamId*/, TRtpSSRC /*aSSRC*/,const TRtcpApp& /*aApp*/ )
	{
	
	}                          
    
void UT_CRtpSession::SrReceived( TRtpId /*aStreamId*/, 
							TRtpSSRC /*aSSRC*/,const TTimeStamps& /*aTimeStamps*/ )
	{
		
	}                         
    
void UT_CRtpSession::RrReceived( TRtpId /*aStreamId*/, TRtpSSRC /*aSSRC*/ )
	{
		
	}
		// from MNonRTPDataObserver
void UT_CRtpSession::NonRTPDataReceived( TUint /*aPort*/, TBool /*aRTPport*/, 
									const TDesC8& /*aNonRTPData*/ )	
	{
		
	}
void UT_CRtpSession::OnExpiredL( TInt /*aStatus*/ )
	{
	}
TRtpId UT_CRtpSession::AssignUniqueID()
	{
	return ++iNumOfObjects;
	}
	
TInt UT_CRtpSession::AddStreamToSession(TRtpId /*aSessionId*/, TRtpId /*aStreamId*/)
	{
	//call back function
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
// Compares TRtpPeerStat objects, returns ETrue if the member values are equal
// -----------------------------------------------------------------------------
//
TBool UT_CRtpSession::PeerStatsEqual( TRtpPeerStat aStat1, TRtpPeerStat aStat2 )
    {
    if ( aStat1.iNumPacketsSent == aStat2.iNumPacketsSent &&
        aStat1.iCumNumOctetsSent == aStat2.iCumNumOctetsSent &&
        aStat1.iRoundTripDelay == aStat2.iRoundTripDelay &&
        aStat1.iCumNumPacketsLost == aStat2.iCumNumPacketsLost &&
        aStat1.iFractionLost == aStat2.iFractionLost &&
        aStat1.iArrivalJitter == aStat2.iArrivalJitter &&
        aStat1.iRxBandwidth == aStat2.iRxBandwidth &&
        aStat1.iChannelBufferSize == aStat2.iChannelBufferSize &&
        aStat1.iNTPTimeStampSec == aStat2.iNTPTimeStampSec &&
        aStat1.iNTPTimeStampFrac == aStat2.iNTPTimeStampFrac &&
        aStat1.iTimeStamp == aStat2.iTimeStamp &&
        aStat1.iTxBandwidth == aStat2.iTxBandwidth )
        {
        return ETrue;
        }
    return EFalse;
    }
	

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CRtpSession,
    "CRtpSession",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "CRtpSession",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_NewLL, Teardown)

EUNIT_TEST(
    "SSRC - test ",
    "CRtpSession",
    "SSRC",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_SSRCL, Teardown)

EUNIT_TEST(
    "StartSession - test ",
    "CRtpSession",
    "StartSession",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_StartSessionL, Teardown)

EUNIT_TEST(
    "SetRemoteAddress - test ",
    "CRtpSession",
    "SetRemoteAddress",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_SetRemoteAddressL, Teardown)

EUNIT_TEST(
    "SetRemoteRtcpAddress - test ",
    "CRtpSession",
    "SetRemoteRtcpAddress",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_SetRemoteRtcpAddressL, Teardown)

EUNIT_TEST(
    "CreateReceiveStreamL - test ",
    "CRtpSession",
    "CreateReceiveStreamL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_CreateReceiveStreamLL, Teardown)

EUNIT_TEST(
    "CreateTransmitStreamL - test ",
    "CRtpSession",
    "CreateTransmitStreamL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_CreateTransmitStreamLL, Teardown)

EUNIT_TEST(
    "CreateTransmitStreamExtL - test ",
    "CRtpSession",
    "CreateTransmitStreamExtL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_CreateTransmitStreamExtLL, Teardown)

EUNIT_TEST(
    "CloseStream - test ",
    "CRtpSession",
    "CloseStream",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_CloseStreamL, Teardown)

EUNIT_TEST(
    "ResetTxStreamStats - test ",
    "CRtpSession",
    "ResetTxStreamStats",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_ResetTxStreamStatsL, Teardown)

EUNIT_TEST(
    "ResetRxStreamStats - test ",
    "CRtpSession",
    "ResetRxStreamStats",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_ResetRxStreamStatsL, Teardown)

EUNIT_TEST(
    "RegisterRtpObserver - test ",
    "CRtpSession",
    "RegisterRtpObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_RegisterRtpObserverL, Teardown)

EUNIT_TEST(
    "UnregisterRtpObserver - test ",
    "CRtpSession",
    "UnregisterRtpObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_UnregisterRtpObserverL, Teardown)

EUNIT_TEST(
    "SetNonRTPDataObserver - test ",
    "CRtpSession",
    "SetNonRTPDataObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_SetNonRTPDataObserverL, Teardown)

EUNIT_TEST(
    "SendRtpPacket - test ",
    "CRtpSession",
    "SendRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_SendRtpPacketL, Teardown)

/*EUNIT_TEST(
    "SendRtpPacket1 - test ",
    "CRtpSession",
    "SendRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_SendRtpPacket_OneL, Teardown)
*/

EUNIT_TEST(
    "SendRtpPacket2 - test ",
    "CRtpSession",
    "SendRtpPacket",
    "FUNCTIONALITY",
    Setup2L, UT_CRtpSession_SendRtpPacket_TwoL, Teardown)

EUNIT_TEST(
    "SendRtpPacket3 - test ",
    "CRtpSession",
    "SendRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_SendRtpPacket_3L, Teardown)

EUNIT_TEST(
    "SendRtpPacket4 - test ",
    "CRtpSession",
    "SendRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_SendRtpPacket_4L, Teardown)

EUNIT_TEST(
    "SendData - test ",
    "CRtpSession",
    "SendData",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_SendDataL, Teardown)

EUNIT_TEST(
    "CancelSend - test ",
    "CRtpSession",
    "CancelSend",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_CancelSendL, Teardown)

EUNIT_TEST(
    "RegisterRtcpObserver - test ",
    "CRtpSession",
    "RegisterRtcpObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_RegisterRtcpObserverL, Teardown)

EUNIT_TEST(
    "UnregisterRtcpObserver - test ",
    "CRtpSession",
    "UnregisterRtcpObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_UnregisterRtcpObserverL, Teardown)

EUNIT_TEST(
    "SendRtcpByePacketL - test ",
    "CRtpSession",
    "SendRtcpByePacketL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_SendRtcpByePacketLL, Teardown)

EUNIT_TEST(
    "SendRtcpAppPacket - test ",
    "CRtpSession",
    "SendRtcpAppPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_SendRtcpAppPacketL, Teardown)

EUNIT_TEST(
    "SendRtcpSrPacket1 - test ",
    "CRtpSession",
    "SendRtcpSrPacket1",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_SendRtcpSrPacket1L, Teardown)
EUNIT_TEST(
    "SendRtcpSrPacket2 - test ",
    "CRtpSession",
    "SendRtcpSrPacket2",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_SendRtcpSrPacket2L, Teardown)
    

EUNIT_TEST(
    "SendRtcpRrPacket - test ",
    "CRtpSession",
    "SendRtcpRrPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_SendRtcpRrPacketL, Teardown)

EUNIT_TEST(
    "GetRtpSocket - test ",
    "CRtpSession",
    "GetRtpSocket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_GetRtpSocketL, Teardown)

EUNIT_TEST(
    "GetRtcpSocket - test ",
    "CRtpSession",
    "GetRtcpSocket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_GetRtcpSocketL, Teardown)

EUNIT_TEST(
    "GetStreamStatistics - test ",
    "CRtpSession",
    "GetStreamStatistics",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_GetStreamStatisticsL, Teardown)

EUNIT_TEST(
    "SetRtcpParameters - test ",
    "CRtpSession",
    "SetRtcpParameters",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_SetRtcpParametersL, Teardown)

EUNIT_TEST(
    "GetSessionId - test ",
    "CRtpSession",
    "GetSessionId",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_GetSessionIdL, Teardown)

EUNIT_TEST(
    "StopRtcpSending - test ",
    "CRtpSession",
    "StopRtcpSending",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_StopRtcpSendingL, Teardown)

EUNIT_TEST(
    "ResumeRtcpSending - test ",
    "CRtpSession",
    "ResumeRtcpSending",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_ResumeRtcpSendingL, Teardown)

EUNIT_TEST(
    "IsRtcpSendingSuspended - test ",
    "CRtpSession",
    "IsRtcpSendingSuspended",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_IsRtcpSendingSuspendedL, Teardown)

EUNIT_TEST(
    "CheckRemoteSsrc - test ",
    "CRtpSession",
    "CheckRemoteSsrc",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_CheckRemoteSsrcL, Teardown)

EUNIT_TEST(
    "OnExpired - test ",
    "CRtpSession",
    "OnExpired",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_OnExpiredL, Teardown)
EUNIT_TEST(
    "ScheduleRtcpSend - test ",
    "CRtpSession",
    "ScheduleRtcpSend",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_ScheduleRtcpSendL, Teardown)
    

EUNIT_TEST(
    "OnRtpReceivedL - test ",
    "CRtpSession",
    "OnRtpReceivedL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_OnRtpReceivedLL, Teardown)

EUNIT_TEST(
    "2OnRtpReceivedL - test ",
    "CRtpSession",
    "OnRtpReceivedL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_OnRtpReceivedL3L, Teardown)    

EUNIT_TEST(
    "OnRtcpReceivedL - test ",
    "CRtpSession",
    "OnRtcpReceivedL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_OnRtcpReceivedLL, Teardown)

EUNIT_TEST(
    "OnReceptionError - test ",
    "CRtpSession",
    "OnReceptionError",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_OnReceptionErrorL, Teardown)

EUNIT_TEST(
    "OnPacketRejected - test ",
    "CRtpSession",
    "OnPacketRejected",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_OnPacketRejectedL, Teardown)

EUNIT_TEST(
    "BuildRTCPReportWithoutStreams() ",
    "CRtpSession",
    "BuildRTCPReportWithoutStreams",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_BuildRTCPReportWithoutStreams, Teardown)

EUNIT_TEST(
    "BuildRTCPReportWithoutPackets ",
    "CRtpSession",
    "BuildRTCPReportWithoutPackets",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_BuildRTCPReportWithoutPackets, Teardown)
EUNIT_TEST(
    "DeallocateMemoryForSdes ",
    "CRtpSession",
    "DeallocateMemoryForSdes",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_DeallocateMemoryForSdesL, Teardown)
     

EUNIT_TEST(
    "MatchSSRCToStream1 ",
    "CRtpSession",
    "UT_CRtpSession_MatchSSRCToStream",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_MatchSSRCToStreamL, Teardown)
    
EUNIT_TEST(
    "MatchSSRCToStream2 ",
    "CRtpSession",
    "UT_CRtpSession_MatchSSRCToStream",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_MatchSSRCToStream2L, Teardown)
        
EUNIT_TEST(
    "RcvStreamReceivedRtpPacket ",
    "CRtpSession",
    "UT_CRtpSession_RcvStreamReceivedRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_RcvStreamReceivedRtpPacketL, Teardown)
EUNIT_TEST(
    "NextRTCPIntervalL ",
    "CRtpSession",
    "UT_CRtpSession_NextRTCPIntervalL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_NextRTCPIntervalL, Teardown)
EUNIT_TEST(
    "GetRcvStreamByIndex ",
    "CRtpSession",
    "UT_CRtpSession_GetRcvStreamByIndexL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_GetRcvStreamByIndexL, Teardown)
EUNIT_TEST(
    "DoBuildSendRTCPReportIfNotSent",
    "CRtpSession",
    "DoBuildSendRTCPReportIfNotSent",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_DoBuildSendRTCPReportIfNotSentL, Teardown)
EUNIT_TEST(
    "DoBuildSendRTCPReport",
    "CRtpSession",
    "DoBuildSendRTCPReport",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_DoBuildSendRTCPReportL, Teardown)    
EUNIT_TEST(
    "BuildSendRTCPReport1",
    "CRtpSession",
    "BuildSendRTCPReport",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_BuildSendRTCPReport1L, Teardown)  

EUNIT_TEST(
    "BuildSendRTCPReport2",
    "CRtpSession",
    "BuildSendRTCPReport",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_BuildSendRTCPReport2L, Teardown)   
EUNIT_TEST(
    "SendRTCPReport",
    "CRtpSession",
    "SendRTCPReport",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_SendRTCPReportL, Teardown)      
EUNIT_TEST(
    "CreateSDESL",
    "CRtpSession",
    "CreateSDESL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_CreateSDESL, Teardown)      

EUNIT_TEST(
    "ProcessOneRTCPReportSectionL",
    "CRtpSession",
    "ProcessOneRTCPReportSectionL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_ProcessOneRTCPReportSectionL, Teardown)      
EUNIT_TEST(
    "ProcessRTCPReport",
    "CRtpSession",
    "ProcessRTCPReport",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_ProcessRTCPReportL, Teardown)   
EUNIT_TEST(
    "ProcessSDESData1",
    "CRtpSession",
    "ProcessSDESData1",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_ProcessSDESDataL, Teardown)   
EUNIT_TEST(
    "ProcessSDESData2",
    "CRtpSession",
    "ProcessSDESData2",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_ProcessSDESData2L, Teardown)   
EUNIT_TEST(
    "ProcessRRSectionL",
    "CRtpSession",
    "ProcessRRSectionL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_ProcessRRSectionL, Teardown)   
    
EUNIT_TEST(
    "ProcessSRSectionL",
    "CRtpSession",
    "ProcessSRSectionL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_ProcessSRSectionL, Teardown)         
EUNIT_TEST(
    "ProcessBYESection",
    "CRtpSession",
    "ProcessBYESection",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_ProcessBYESectionL, Teardown)       
EUNIT_TEST(
    "RemoveSdesFromArray",
    "CRtpSession",
    "RemoveSdesFromArray",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_RemoveSdesFromArrayL, Teardown)    
EUNIT_TEST(
    "CreateNewRecvStream",
    "CRtpSession",
    "CreateNewRecvStream",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_CreateNewRecvStreamLL, Teardown)    
EUNIT_TEST(
    "SendSrtpcpReportL",
    "CRtpSession",
    "SendSrtpcpReportL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSession_SendSrtcpReportLL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE
