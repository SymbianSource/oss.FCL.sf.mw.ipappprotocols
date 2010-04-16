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
#include "UT_CRtpStream.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "rtpstream.h"

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
_LIT8(KRTPPacket, "8180CDE000000004DEADBEEFFFFFFF7F3958B1FB3F0E");

// CONSTRUCTION
UT_CRtpStream* UT_CRtpStream::NewL()
    {
    UT_CRtpStream* self = UT_CRtpStream::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CRtpStream* UT_CRtpStream::NewLC()
    {
    UT_CRtpStream* self = new( ELeave ) UT_CRtpStream();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CRtpStream::~UT_CRtpStream()
    {
    }

// Default constructor
UT_CRtpStream::UT_CRtpStream()
    {
    }

// Second phase construct
void UT_CRtpStream::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_CRtpStream::SetupL(  )
    {
   TRtpPayloadType payloadType( 0 );

    for ( TUint k = 0; k < KRtpMaxPayloadTypes; k++ )
        {
        iProfileRTPTimeRates[k] = ( TUint32 ) KRtpPayloadClockConversions[k];
        }

    iObs = this;
        
    iRecvStream = CRtpRecvStream::NewL( payloadType,
                                        1, // sessionId
                                        2, // rcvstreamid
                                        &iObs,
                                        this,
                                        iProfileRTPTimeRates,
                                        this );

    iTranStream = CRtpTranStream::NewL( payloadType,
                                        1, // sessionId
                                        3, // transtreamid
                                        1000, // SSRC
                                        this,
                                        iProfileRTPTimeRates );
    iExpectingExtensionPacket = EFalse;
    }

void UT_CRtpStream::Teardown(  )
    {
    delete iRecvStream;
    delete iTranStream;
	
	iRecvStream = NULL;
    iTranStream = NULL;
    }

void UT_CRtpStream::UT_CRtpStream_ResetStreamStatL(  )
    {
   // RECEIVE STREAM

    // "Test description: Test the reset of statistics" ) );
    // "CRtpRecvStream::ResetStreamStat()" ) );
    
    // Create some arbitrary data
    iRecvStream->iNumReceivedPackets = 45;
    iRecvStream->iNumWrapAround = 3;
    iRecvStream->iSyncJitter.iLastPacketR = 45;
    iRecvStream->iSyncJitter.iLastPacketS = 45;
    iRecvStream->iSyncJitter.iJitterTime = 45;
    iRecvStream->iSyncJitter.iWaitTime = 45;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iFractionLost = 45;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iCumNumPacketsLost = 45;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iSeqNumReceived = 45;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iArrivalJitter = 45;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iRoundTripDelay = 45;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iBandwidth = 45;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iTxBandwidth = 45;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iChannelBufferSize = 45;
    
    // Do the reset
    iRecvStream->ResetStreamStat();

    // Check the results
    if ( iRecvStream->iNumReceivedPackets != 0 ||
         iRecvStream->iNumWrapAround != 0 ||
         iRecvStream->iSyncJitter.iLastPacketR != 0 ||
         iRecvStream->iSyncJitter.iLastPacketS != 0 ||
         iRecvStream->iSyncJitter.iJitterTime != 0 ||
         iRecvStream->iSyncJitter.iWaitTime != 0 ||
         iRecvStream->iRtcpStats.iRtcpReceiverStats.iFractionLost != 0 ||
         iRecvStream->iRtcpStats.iRtcpReceiverStats.iCumNumPacketsLost != 0 ||
         iRecvStream->iRtcpStats.iRtcpReceiverStats.iSeqNumReceived != 0 ||
         iRecvStream->iRtcpStats.iRtcpReceiverStats.iArrivalJitter != 0 ||
         iRecvStream->iRtcpStats.iRtcpReceiverStats.iRoundTripDelay != 0 ||
         iRecvStream->iRtcpStats.iRtcpReceiverStats.iChannelBufferSize != 0 ||
         // maintain the bandwidth information
         iRecvStream->iRtcpStats.iRtcpReceiverStats.iBandwidth != 45 ||
         iRecvStream->iRtcpStats.iRtcpReceiverStats.iTxBandwidth != 45 )
        {
        // "Values incorrectly reset" ) );
        // "CRtpRecvStream:ResetStreamStat() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    else         
        {
        EUNIT_ASSERT(ETrue);
        }

    // TRANSMIT STREAM

    // "Test description: Test the reset of statistics" ) );
    // "CRtpTranStream::ResetStreamStat()" ) );
    
    // Create some arbitrary data
    iTranStream->iRtcpStats.iRtcpSenderStats.iNumPacketsSent = 1;
    iTranStream->iRtcpStats.iRtcpSenderStats.iCumNumOctetsSent = 2;
    iTranStream->iRtcpStats.iRtcpSenderStats.iNTPTimeStampSec = 3;
    iTranStream->iRtcpStats.iRtcpSenderStats.iNTPTimeStampFrac = 4;
    iTranStream->iRtcpStats.iRtcpSenderStats.iTimeStamp = 5;
    iTranStream->iFlagSentRTPPackets = 6;
    iTranStream->iFSentRtcpReport = 7;
    iTranStream->iCumNumOctetsSent = 8;
    iTranStream->iCumNumOctetsSent_last = 9;
    iTranStream->iPreviousTime = 10;
    iTranStream->iPreviousRemoteSN = 11;
    iTranStream->iRemoteBandwidth = 13;
    iTranStream->iSeqNumCycles = 14;
    iTranStream->iSN_size[0] = 15;
    iTranStream->iSN_size[1] = 16;

    // Do the reset
    iTranStream->ResetStreamStat();

    // Check the results
    if ( iTranStream->iFlagSentRTPPackets != 0 ||
         iTranStream->iFSentRtcpReport != 0 ||
         iTranStream->iCumNumOctetsSent != 0 ||
         iTranStream->iCumNumOctetsSent_last != 0 ||
         iTranStream->iPreviousTime != 0 ||
         iTranStream->iPreviousRemoteSN != 0 ||
         iTranStream->iSeqNumCycles != 0 ||
         iTranStream->iSN_size[0] != 0 ||
         iTranStream->iSN_size[1] != 0 ||
         iTranStream->iRtcpStats.iRtcpSenderStats.iNumPacketsSent != 0 ||
         iTranStream->iRtcpStats.iRtcpSenderStats.iCumNumOctetsSent != 0 ||
         iTranStream->iRtcpStats.iRtcpSenderStats.iNTPTimeStampSec != 0 ||
         iTranStream->iRtcpStats.iRtcpSenderStats.iNTPTimeStampFrac != 0 ||
         iTranStream->iRtcpStats.iRtcpSenderStats.iTimeStamp != 0 ||
         // Preserve bandwidth information
         iTranStream->iRemoteBandwidth != 13 )
        {
        // "Values incorrectly reset" ) );
        // "CRtpTranStream:ResetStreamStat() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    else         
        {
        EUNIT_ASSERT(ETrue);
        }
    }

void UT_CRtpStream::UT_CRtpStream_FirstPkgL(  )
    {
    // THESE ARE THE GETTERS AND SETTERS OF CRtpStream, THE BASE CLASS

    // "Test description: Testing getters and setters" ) );
    // "CRtpStream::SetFirstPkg( TBool )" ) );

    // Store the previous value
    TBool tempBool( iRecvStream->iFlagFirstPkg );

    // Initialise it
    iRecvStream->iFlagFirstPkg = EFalse;

    // Use the function
    iRecvStream->SetFirstPkg( ETrue );

    // Run the test
    if ( iRecvStream->iFlagFirstPkg == EFalse )
        {
        // "Setter did not update member value" ) );
        // "CRtpStream::SetFirstPkg() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    else
        {
        EUNIT_ASSERT(ETrue);
        }
    
    // "CRtpStream::FirstPkg()" ) );
    
    // Set it
    iRecvStream->iFlagFirstPkg = ETrue;

    // Run the test
    if ( !iRecvStream->FirstPkg() )
        {
        // "Invalid return value" ) );
        // "CRtpStream::FirstPkg() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    else
        {
       	EUNIT_ASSERT(ETrue);
        }

    // Restore the previous value
    iRecvStream->iFlagFirstPkg = tempBool;

    ///////////////////////////////////////////////////////////

    // "CRtpStream::SetLocalSSRC( TRtpSSRC )" ) );

    // Store the previous value
    TRtpSSRC tempSSRC( iRecvStream->iLocalSSRC );

    // Initialise it
    iRecvStream->iLocalSSRC = 12;

    // Use the function
    iRecvStream->SetLocalSSRC( 13 );

    // Run the test
    if ( iRecvStream->iLocalSSRC != 13 )
        {
        // "Setter did not update member value" ) );
        // "CRtpStream::SetLocalSSRC() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    else
        {
        EUNIT_ASSERT(ETrue);
        }

    // "CRtpStream::GetLocalSSRC()" ) );

    // Initialise it
    iRecvStream->iLocalSSRC = 14;

    // Run the test
    if ( iRecvStream->GetLocalSSRC() != 14 )
        {
        // "Invalid return value" ) );
        // "CRtpStream::GetLocalSSRC() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    else
        {
        EUNIT_ASSERT(ETrue);
        }
    
    // Restore the previous value
    iRecvStream->iLocalSSRC = tempSSRC;

    ///////////////////////////////////////////////////////////

    // "CRtpStream::GetStreamID()" ) );

    // Store the previous value
    TRtpId tempId( iRecvStream->iStreamId );

    // Initialise it
    iRecvStream->iStreamId = 20;

    // Run the test
    if ( iRecvStream->GetStreamID() != 20 )
        {
        // "Invalid return value" ) );
        // "CRtpStream::GetStreamID() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    else
        {
        EUNIT_ASSERT(ETrue);
        }

    // Restore the previous value
    iRecvStream->iStreamId = tempId;

    ///////////////////////////////////////////////////////////

    // "CRtpStream::RtcpStats( TRtcpStats )" ) );

    // Store the previous value
    TRtcpStats tempStats( iRecvStream->iRtcpStats );

    TRtcpStats stats;
    TRtcpReceiverStats recvStats;
    TRtcpSenderStats sendStats;

    recvStats.iArrivalJitter = 1;
    recvStats.iBandwidth = 2;
    recvStats.iChannelBufferSize = 3;
    recvStats.iCumNumPacketsLost = 4;
    recvStats.iFractionLost = 5;
    recvStats.iRoundTripDelay = 6;
    recvStats.iSeqNumReceived = 7;
    recvStats.iSSRC = 8;
    recvStats.iTxBandwidth = 9;
    sendStats.iCumNumOctetsSent = 10;
    sendStats.iNTPTimeStampFrac = 11;
    sendStats.iNTPTimeStampSec = 12;
    sendStats.iNumPacketsSent = 13;
    sendStats.iSSRC = 14;
    sendStats.iTimeStamp = 15;
    stats.iRtcpReceiverStats = recvStats;
    stats.iRtcpSenderStats = sendStats;
    
    // Set the stats
    iRecvStream->iRtcpStats = stats;
    
    // Get the stats (function under test)
    TRtcpStats compareStats;
    iRecvStream->RtcpStats( compareStats );
    
    // Compare
    if ( compareStats.iRtcpReceiverStats.iArrivalJitter != stats.iRtcpReceiverStats.iArrivalJitter ||
         compareStats.iRtcpReceiverStats.iBandwidth != stats.iRtcpReceiverStats.iBandwidth ||
         compareStats.iRtcpReceiverStats.iChannelBufferSize != stats.iRtcpReceiverStats.iChannelBufferSize ||
         compareStats.iRtcpReceiverStats.iCumNumPacketsLost != stats.iRtcpReceiverStats.iCumNumPacketsLost ||
         compareStats.iRtcpReceiverStats.iFractionLost != stats.iRtcpReceiverStats.iFractionLost ||
         compareStats.iRtcpReceiverStats.iRoundTripDelay != stats.iRtcpReceiverStats.iRoundTripDelay ||
         compareStats.iRtcpReceiverStats.iSeqNumReceived != stats.iRtcpReceiverStats.iSeqNumReceived ||
         compareStats.iRtcpReceiverStats.iSSRC != stats.iRtcpReceiverStats.iSSRC ||
         compareStats.iRtcpReceiverStats.iTxBandwidth != stats.iRtcpReceiverStats.iTxBandwidth ||
         compareStats.iRtcpSenderStats.iCumNumOctetsSent != stats.iRtcpSenderStats.iCumNumOctetsSent ||
         compareStats.iRtcpSenderStats.iNTPTimeStampFrac != stats.iRtcpSenderStats.iNTPTimeStampFrac ||
         compareStats.iRtcpSenderStats.iNTPTimeStampSec != stats.iRtcpSenderStats.iNTPTimeStampSec ||
         compareStats.iRtcpSenderStats.iNumPacketsSent != stats.iRtcpSenderStats.iNumPacketsSent ||
         compareStats.iRtcpSenderStats.iSSRC != stats.iRtcpSenderStats.iSSRC ||
         compareStats.iRtcpSenderStats.iTimeStamp != stats.iRtcpSenderStats.iTimeStamp )
        {
        // "Invalid return value" ) );
        // "CRtpStream::RtcpStats() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    else
        {
        EUNIT_ASSERT(ETrue);
        }

    ///////////////////////////////////////////////////////////
    // THESE ARE THE GETTERS AND SETTERS OF CRtpRecvStream

    // "CRtpRecvStream::SetReceivedRTPPackets( TRtpSSRC )" ) );

    // Store the previous value
    TBool tempFlag( iRecvStream->iFlagReceivedRTPPackets );

    // Initialise it
    iRecvStream->iFlagReceivedRTPPackets = EFalse;

    // Use the function
    iRecvStream->SetReceivedRTPPackets( ETrue );

    // Run the test
    if ( !iRecvStream->iFlagReceivedRTPPackets )
        {
        // "Setter did not update member value" ) );
        // "CRtpRecvStream::SetReceivedRTPPackets() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    else
        {
        EUNIT_ASSERT(ETrue);
        }

    // "CRtpRecvStream::ReceivedRTPPackets()" ) );

    // Initialise it
    iRecvStream->iFlagReceivedRTPPackets = ETrue;

    // Run the test
    if ( !iRecvStream->ReceivedRTPPackets() )
        {
        // "Invalid return value" ) );
        // "CRtpRecvStream::ReceivedRTPPackets() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    else
        {
        // "CRtpRecvStream::ReceivedRTPPackets() - passed" ) );
        }
    
    // Restore the previous value
    iRecvStream->iFlagReceivedRTPPackets = tempFlag;

    ///////////////////////////////////////////////////////////
    // THESE ARE THE GETTERS AND SETTERS OF CRtpTranStream
   
    // "CRtpTranStream::SetSentRTPPackets( TBool )" ) );

    // Store the previous value
    tempFlag = iTranStream->iFlagSentRTPPackets;

    // Initialise it
    iTranStream->iFlagSentRTPPackets = EFalse;

    // Use the function
    iTranStream->SetSentRTPPackets( ETrue );

    // Run the test
    if ( !iTranStream->iFlagSentRTPPackets )
        {
        // "Setter did not update member value" ) );
        // "CRtpTranStream::SetSentRTPPackets() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    else
        {
        EUNIT_ASSERT(ETrue);
        }

    // "CRtpTranStream::SentRTPPackets()" ) );

    // Initialise it
    iTranStream->iFlagSentRTPPackets = ETrue;

    // Run the test
    if ( !iTranStream->SentRTPPackets() )
        {
        // "Invalid return value" ) );
        // "CRtpTranStream::SentRTPPackets() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    else
        {
        EUNIT_ASSERT(ETrue);
        }
    
    // Restore the previous value
    iTranStream->iFlagSentRTPPackets = tempFlag;

    // "CRtpTranStream::SetRtcpReportFlag( TBool )" ) );

    // Store the previous value
    tempFlag = iTranStream->iFSentRtcpReport;

    // Initialise it
    iTranStream->iFSentRtcpReport = EFalse;

    // Use the function
    iTranStream->SetRtcpReportFlag();

    // Run the test
    if ( !iTranStream->iFSentRtcpReport )
        {
        // "Setter did not update member value" ) );
        // "CRtpTranStream::SetRtcpReportFlag() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    else
        {
        // "CRtpTranStream::SetRtcpReportFlag() - passed" ) );
        }

    // "CRtpTranStream::ResetRtcpReportFlag()" ) );

    // Initialise it
    iTranStream->iFSentRtcpReport = ETrue;

    // Use the function
    iTranStream->ResetRtcpReportFlag();

    // Run the test
    if ( iTranStream->iFSentRtcpReport )
        {
        // "Reset did not update member value" ) );
        // "CRtpTranStream::ResetRtcpReportFlag() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    else
        {
        EUNIT_ASSERT(ETrue);
        }

    // "CRtpTranStream::SentRtcpReport()" ) );

    // Initialise it
    iTranStream->iFSentRtcpReport = ETrue;

    // Run the test
    if ( !iTranStream->SentRtcpReport() )
        {
        // "Invalid return value" ) );
        // "CRtpTranStream::SentRtcpReport() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    else
        {
        EUNIT_ASSERT(ETrue);
        }
    
    // Restore the previous value
    iTranStream->iFSentRtcpReport = tempFlag;

    }

void UT_CRtpStream::UT_CRtpStream_SetFirstPkgL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpStream::UT_CRtpStream_GetLocalSSRCL(  )
    {
    EUNIT_ASSERT( ETrue);
    }

void UT_CRtpStream::UT_CRtpStream_SetLocalSSRCL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpStream::UT_CRtpStream_RegisterRtcpObserverL(  )
    {
    //EUNIT_ASSERT( EFalse );
    }

void UT_CRtpStream::UT_CRtpStream_UnRegisterRtcpObserverL(  )
    {
    //EUNIT_ASSERT( EFalse );
    }

void UT_CRtpStream::UT_CRtpStream_GetStreamIDL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpStream::UT_CRtpStream_RtcpStatsL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpStream::UT_CRtpStream_GetStreamStatL(  )
    {
        TBuf8<3> cName; 
    TBuf8<3> userName;
    TBuf8<3> email;
    TBuf8<3> phoneNumber;
    TBuf8<3> location;
    TBuf8<3> swToolName;
    TBuf8<3> noticeStatus;
    TBuf8<3> privateStr;

    //"") );
    // "Test description: Testing getting remote SDES info" ) );
    // "CRtpRecvStream::GetRemoteStreamInfo( const TRtpSdesParams& )" ) );

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

    TRtpSdesParams params;
    params.iCName.Set( cName.Ptr() );
    params.iUserName.Set( userName.Ptr() );
    params.iEmail.Set( email.Ptr() );
    params.iPhoneNumber.Set( phoneNumber.Ptr() );
    params.iLocation.Set( location.Ptr() );
    params.iSwToolName.Set( swToolName.Ptr() );
    params.iNoticeStatus.Set( noticeStatus.Ptr() );
    params.iPrivate.Set( privateStr.Ptr() );
    
    // Set it
    iRecvStream->iRemoteSDES->SetSDES( params );

    // Run the function
    TRtpSdesParams params2;
    iRecvStream->GetRemoteStreamInfo( params2 );
    
    // Compare the results
    if ( params.iCName != cName
      || params.iUserName != userName
      || params.iEmail != email
      || params.iPhoneNumber != phoneNumber
      || params.iLocation != location
      || params.iSwToolName != swToolName
      || params.iNoticeStatus != noticeStatus
      || params.iPrivate != privateStr )
        {
        // "Parameters incorrectly updated" ) );
        // "CRtpRecvStream::GetRemoteStreamInfo() - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }

    // "CRtpRecvStream::GetRemoteStreamInfo() - passed" ) );

    //"") );
    // "Test description: Testing getting stream statistics" ) );
    // "CRtpRecvStream::GetStreamStat( TRtpPeerStat& )" ) );

    TRtpPeerStat stats;
    
    // Set some parameters
    iRecvStream->iRtcpStats.iRtcpSenderStats.iCumNumOctetsSent = 101;
    iRecvStream->iRtcpStats.iRtcpSenderStats.iNumPacketsSent = 102;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iTxBandwidth = 103;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iArrivalJitter = 104;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iCumNumPacketsLost = 105;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iFractionLost = 106;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iRoundTripDelay = 107;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iBandwidth = 108;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iChannelBufferSize = 109;
    
    // Get the info
    iRecvStream->GetStreamStat( stats );
    
    // Compare it with our original data
    if ( stats.iCumNumOctetsSent != 101 ||
         stats.iNumPacketsSent != 102 ||
         stats.iTxBandwidth != 103 ||
         stats.iArrivalJitter != 104 ||
         stats.iCumNumPacketsLost != 105 ||
         stats.iFractionLost != 106 ||
         stats.iRoundTripDelay != 107 ||
         stats.iRxBandwidth != 108 ||
         stats.iChannelBufferSize != 109 )
        {
        // "Parameters incorrectly updated" ) );
        // "CRtpRecvStream::GetStreamStat() - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
    }

void UT_CRtpStream::UT_CRtpStream_RtpStreamProcessRtcpReportSectionL(  )
    {
    //"") );
    // "Test description: Test creating and parsing an SR packet" ) );
    // "CRtpTranStream::RtpStreamCreateRtcpReportSection( CRtpPacket* )" ) );
    // "CRtpRecvStream::RtpStreamProcessRtcpReportSection( CRtpPacket* )" ) );
    
    // Create a packet to work with
    TUint packetSize = 2048;
    CRtpPacket* packet = CRtpPacket::NewL( packetSize, iProfileRTPTimeRates );
    CleanupStack::PushL( packet );

    // Set some parameters
    iTranStream->iLocalSSRC = 1;
    iTranStream->iBaseSeqNum = 0;
    iTranStream->iSeqNumCycles = 0;
    iTranStream->iSeqNum = 2;
    iTranStream->iCumNumOctetsSent = 3;
    iTranStream->iTimeStamp = 4;
    
    // Create the RTCP report    
    iTranStream->RtpStreamCreateRtcpReportSection( packet );

    // Make sure the packet can be processed
    packet->RtpPacketResetPtr();
    packet->iSize = 24; // size of SR section
    packet->SetType( ERTCP_SR );

    // Process the report
    TRtpRtcpEnum error = iRecvStream->RtpStreamProcessRtcpReportSectionL( packet );

    // Check the results
    if ( error != KErrNone )
        {
        // "Error parsing SR packet" ) );
        // "CRtpTranStream::RtpStreamCreateRtcpReportSection() - FAILED" ) );
        // "CRtpRecvStream::RtpStreamProcessRtcpReportSection() - FAILED" ) );
        CleanupStack::PopAndDestroy( packet );
        EUNIT_ASSERT( EFalse );
        }

    // Try again with an invalid packet type (RR)
    TRtpPacketStreamParam streamParam;
    TRtpPacketIOParam initParam;

    streamParam.TRTCP_RR.SSRC = 1;
    streamParam.TRTCP_RR.fractionLost = 2;
    streamParam.TRTCP_RR.cumNumPacketsLost = 3;
    streamParam.TRTCP_RR.seqNumReceived = 4;
    streamParam.TRTCP_RR.arrivalJitter = 0;
    initParam.TRTCP_RR.lastSRTimeStamp = 0;
    initParam.TRTCP_RR.delaySinceLSR = 0;
    
    //iTranStream->iSeqNum = 3;
    //iTranStream->iCumNumOctetsSent = 4;
    //iTranStream->iTimeStamp = 5;
    packet->RtpPacketReset();
    //iTranStream->RtpStreamCreateRtcpReportSection( packet );

    packet->RtpPacketBuildApp( &streamParam, &initParam );
    
    packet->RtpPacketResetPtr();
    packet->iSize = 24; // size of APP section
    packet->SetType( ERTCP_RR );
    error = iRecvStream->RtpStreamProcessRtcpReportSectionL( packet );
    
    // Check the results
    if ( error == KErrNone )
        {
        // "Should have received error parsing SR packet" ) );
        // "CRtpTranStream::RtpStreamCreateRtcpReportSection() - FAILED" ) );
        // "CRtpRecvStream::RtpStreamProcessRtcpReportSection() - FAILED" ) );
        CleanupStack::PopAndDestroy( packet );
        EUNIT_ASSERT( EFalse );
        }

    // "CRtpTranStream::RtpStreamCreateRtcpReportSection() - passed" ) );
    // "CRtpRecvStream::RtpStreamProcessRtcpReportSection() - passed" ) );
    
    CleanupStack::PopAndDestroy( packet );
    }

void UT_CRtpStream::UT_CRtpStream_RtpStreamCreateRtcpReportSectionL(  )
    {
    //"") );
    // "Test description: Test creating and parsing an RR packet" ) );
    // "CRtpRecvStream::RtpStreamCreateRtcpReportSection( CRtpPacket* )" ) );
    // "CRtpTranStream::RtpStreamProcessRtcpReportSection( CRtpPacket* )" ) );
    // Create a packet to work with
    TUint packetSize = 2048;
    CRtpPacket* packet = CRtpPacket::NewL( packetSize, iProfileRTPTimeRates );
    CleanupStack::PushL( packet );

    // Set some parameters
    iRecvStream->iSeqNum = 0;
    iRecvStream->iBaseSeqNum = 0;
    iRecvStream->iSeqNumCycles = 0;
    iRecvStream->iNumWrapAround = 0;
    iRecvStream->iLastRR_numExpectedPackets = 1;
    iRecvStream->iSyncJitter.iTimeStampResolution = 0;
    iRecvStream->iNumReceivedPackets = 5;
    
    // Create the RTCP report    
    iRecvStream->RtpStreamCreateRtcpReportSection( packet );

    // Make sure the packet can be processed
    packet->RtpPacketResetPtr();
    packet->iSize = 24; // size of SR section
    packet->SetType( ERTCP_RR );

    // Process the report
    TRtpRtcpEnum error = iTranStream->RtpStreamProcessRtcpReportSectionL( packet );

    // Check the results
    if ( error != KErrNone )
        {
        // "Error parsing RR packet" ) );
        // "CRtpRecvStream::RtpStreamCreateRtcpReportSection() - FAILED" ) );
        // "CRtpTranStream::RtpStreamProcessRtcpReportSection() - FAILED" ) );
        CleanupStack::PopAndDestroy( packet );
        EUNIT_ASSERT( EFalse );
        }

    // Set some parameters differently to reach more branches
    iRecvStream->iNumReceivedPackets = 0;
    iRecvStream->iLastRR_numExpectedPackets = 0;
    iRecvStream->iSyncJitter.iTimeStampResolution = 10;

    // Create the RTCP report    
    iRecvStream->RtpStreamCreateRtcpReportSection( packet );

    // Make sure the packet can be processed
    packet->RtpPacketResetPtr();
    packet->iSize = 24; // size of SR section
    packet->SetType( ERTCP_RR );

    // Process the report
    error = iTranStream->RtpStreamProcessRtcpReportSectionL( packet );

    // Check the results
    if ( error != KErrNone )
        {
        // "Error parsing RR packet" ) );
        // "CRtpRecvStream::RtpStreamCreateRtcpReportSection() - FAILED" ) );
        // "CRtpTranStream::RtpStreamProcessRtcpReportSection() - FAILED" ) );
        CleanupStack::PopAndDestroy( packet );
        EUNIT_ASSERT( EFalse );
        }

    // Set some parameters differently to reach more branches
    iRecvStream->iLastRR_numExpectedPackets = 0xFFFF0000;

    // Create the RTCP report    
    iRecvStream->RtpStreamCreateRtcpReportSection( packet );

    // Make sure the packet can be processed
    packet->RtpPacketResetPtr();
    packet->iSize = 24; // size of SR section
    packet->SetType( ERTCP_RR );

    // Process the report
    error = iTranStream->RtpStreamProcessRtcpReportSectionL( packet );

    // Check the results
    if ( error != KErrNone )
        {
        // "Error parsing RR packet" ) );
        // "CRtpRecvStream::RtpStreamCreateRtcpReportSection() - FAILED" ) );
        // "CRtpTranStream::RtpStreamProcessRtcpReportSection() - FAILED" ) );
        CleanupStack::PopAndDestroy( packet );
        EUNIT_ASSERT( EFalse );
        }

    // "CRtpRecvStream::RtpStreamCreateRtcpReportSection() - passed" ) );
    // "CRtpTranStream::RtpStreamProcessRtcpReportSection() - passed" ) );

    CleanupStack::PopAndDestroy( packet );
    }
    
void UT_CRtpStream::UT_CRtpStream_UpdateParamL()
    {
  
    // "Test description: Test update of statistics" ) );
    // "CRtpStream::RtpStreamUpdateParam( TRtpPacketType, TRtpPacketStreamParam* )" ) );
    
    // Set some values
    TRtpPacketType type( ERTCP_HEADER ); // invalid
    TRtpPacketStreamParam param;

    // Run the function. This should fail
    TInt result( iRecvStream->RtpStreamUpdateParamL( type, &param ) );

    if ( result == KErrNone )
        {
        // "Function should only process ERTP type packets" ) );
        // "CRtpStream::RtpStreamUpdateParam() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    
    // Set some values
    type = ERTP; // valid
    iRecvStream->iFlagFirstPkg = EFalse;
    param.TRTP.SSRC = 10; // different from iRecvStream->iSSRC
    
    // Run the function. This should fail
    result = iRecvStream->RtpStreamUpdateParamL( type, &param );

    if ( result == KErrNone )
        {
        // "Function should only proceed if SSRC matches" ) );
        // "CRtpStream::RtpStreamUpdateParam() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    // Set some values
    iRecvStream->iFlagFirstPkg = ETrue;
    iRecvStream->iRemoteSSRC = param.TRTP.SSRC = 10;
    param.TRTP.payload = 100; // This should make iRecvStream->iSyncJitter.iTimeStampResolution 0
    param.TRTP.seqNum = 30;
    param.TRTP.timeStamp = 40;
    TInt prevRecvPackets( iRecvStream->iNumReceivedPackets );
    
    // Run the function
    result = iRecvStream->RtpStreamUpdateParamL( type, &param );
    
    if ( result != KErrNone ||
         iRecvStream->iRemoteSSRC != param.TRTP.SSRC ||
         iRecvStream->iPayload != param.TRTP.payload ||
         iRecvStream->iBaseSeqNum != param.TRTP.seqNum ||
         //iRecvStream->iBaseTimeStamp != param.TRTP.timeStamp ||
         iRecvStream->iSyncJitter.iTimeStampResolution != 0 ||
         iRecvStream->iSeqNumCycles != 0 ||
         iRecvStream->iSeqNum != param.TRTP.seqNum ||
         iRecvStream->iTimeStamp != param.TRTP.timeStamp ||
         iRecvStream->iFlagFirstPkg != EFalse ||
         iRecvStream->iNumReceivedPackets != prevRecvPackets + 1 )
        {
        // "Parameters not properly updated (1)" ) );
        // "CRtpStream::RtpStreamUpdateParam() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    iRecvStream->iFlagFirstPkg = ETrue;
    param.TRTP.payload = 5; // This should make iRecvStream->iSyncJitter.iTimeStampResolution != 0

    // Run the function
    result = iRecvStream->RtpStreamUpdateParamL( type, &param );
    if ( result != KErrNone ||
         iRecvStream->iSyncJitter.iTimeStampResolution == 0 )
        {
        // "Parameters not properly updated (2)" ) );
        // "CRtpStream::RtpStreamUpdateParam() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }        

    EUNIT_ASSERT(ETrue);
    }    

void UT_CRtpStream::UT_CRtpStream_RtpByeAppMethodsL()
    {
    //"") );
    // "Test description: Test creating a BYE packet" ) );
    // "CRtpTranStream::BuildRtcpBYEPacket( const TDesC8&, CRtpPacket* )" ) );
    
    // Create a packet to work with
    TUint packetSize = 2048;
    CRtpPacket* packet = CRtpPacket::NewL( packetSize, iProfileRTPTimeRates );
    CleanupStack::PushL( packet );

    // Create the BYE packet
    TBuf8<15> reason;
    reason.Format( _L8( "Hello" ) );
    TInt error( KErrNone );
    error = iTranStream->BuildRtcpBYEPacket( reason, packet );

    // Check the results (can't really verify anything here)
    if ( error != KErrNone )
        {
        // "BYE packet creation failed" ) );
        // "CRtpTranStream::BuildRtcpBYEPacket() - FAILED" ) );
        CleanupStack::PopAndDestroy( packet );
        EUNIT_ASSERT(EFalse);
        }

    // "CRtpTranStream::BuildRtcpBYEPacket() - passed" ) );

    //"") );
    // "Test description: Test creating an APP packet" ) );
    // "CRtpTranStream::BuildRtcpAPPPacket( const TRtcpApp&, CRtpPacket* )" ) );

    // Create the APP packet
    TRtcpApp app;
    app.iName[0] = 'a';
    app.iName[1] = 'b';
    app.iName[2] = 'c';
    app.iName[3] = 'd';
    app.iAppData[0] = 'e';
    app.iAppData[1] = 'f';
    app.iAppDataLen = 2;

    error = iTranStream->BuildRtcpAPPPacket( app, packet );

    // Check the results (can't really verify anything here)
    if ( error != KErrNone )
        {
        // "APP packet creation failed" ) );
        // "CRtpTranStream::BuildRtcpAppPacket() - FAILED" ) );
        CleanupStack::PopAndDestroy( packet );
        EUNIT_ASSERT(EFalse);
        }

    // "CRtpTranStream::BuildRtcpAPPPacket() - passed" ) );
    
    CleanupStack::PopAndDestroy( packet );
  	EUNIT_ASSERT(ETrue);
    }
 
void UT_CRtpStream::UT_CRtpStream_TestRtpSRMethodsL()
    {
    //"") );
    // "Test description: Test creating and parsing an SR packet" ) );
    // "CRtpTranStream::RtpStreamCreateRtcpReportSection( CRtpPacket* )" ) );
    // "CRtpRecvStream::RtpStreamProcessRtcpReportSection( CRtpPacket* )" ) );
    
    // Create a packet to work with
    TUint packetSize = 2048;
    CRtpPacket* packet = CRtpPacket::NewL( packetSize, iProfileRTPTimeRates );
    CleanupStack::PushL( packet );

    // Set some parameters
    iTranStream->iLocalSSRC = 1;
    iTranStream->iBaseSeqNum = 0;
    iTranStream->iSeqNumCycles = 0;
    iTranStream->iSeqNum = 2;
    iTranStream->iCumNumOctetsSent = 3;
    iTranStream->iTimeStamp = 4;
    
    // Create the RTCP report    
    iTranStream->RtpStreamCreateRtcpReportSection( packet );

    // Make sure the packet can be processed
    packet->RtpPacketResetPtr();
    packet->iSize = 24; // size of SR section
    packet->SetType( ERTCP_SR );

    // Process the report
    TRtpRtcpEnum error = iRecvStream->RtpStreamProcessRtcpReportSectionL( packet );

    // Check the results
    if ( error != KErrNone )
        {
        // "Error parsing SR packet" ) );
        // "CRtpTranStream::RtpStreamCreateRtcpReportSection() - FAILED" ) );
        // "CRtpRecvStream::RtpStreamProcessRtcpReportSection() - FAILED" ) );
        CleanupStack::PopAndDestroy( packet );
        EUNIT_ASSERT(EFalse);
        }

    // Try again with an invalid packet type (RR)
    TRtpPacketStreamParam streamParam;
    TRtpPacketIOParam initParam;

    streamParam.TRTCP_RR.SSRC = 1;
    streamParam.TRTCP_RR.fractionLost = 2;
    streamParam.TRTCP_RR.cumNumPacketsLost = 3;
    streamParam.TRTCP_RR.seqNumReceived = 4;
    streamParam.TRTCP_RR.arrivalJitter = 0;
    initParam.TRTCP_RR.lastSRTimeStamp = 0;
    initParam.TRTCP_RR.delaySinceLSR = 0;
    
    //iTranStream->iSeqNum = 3;
    //iTranStream->iCumNumOctetsSent = 4;
    //iTranStream->iTimeStamp = 5;
    packet->RtpPacketReset();
    //iTranStream->RtpStreamCreateRtcpReportSection( packet );

    packet->RtpPacketBuildApp( &streamParam, &initParam );
    
    packet->RtpPacketResetPtr();
    packet->iSize = 24; // size of APP section
    packet->SetType( ERTCP_RR );
    error = iRecvStream->RtpStreamProcessRtcpReportSectionL( packet );
    
    // Check the results
    if ( error == KErrNone )
        {
        // "Should have received error parsing SR packet" ) );
        // "CRtpTranStream::RtpStreamCreateRtcpReportSection() - FAILED" ) );
        // "CRtpRecvStream::RtpStreamProcessRtcpReportSection() - FAILED" ) );
        CleanupStack::PopAndDestroy( packet );
        EUNIT_ASSERT(EFalse);
        }

    // "CRtpTranStream::RtpStreamCreateRtcpReportSection() - passed" ) );
    // "CRtpRecvStream::RtpStreamProcessRtcpReportSection() - passed" ) );
    
    CleanupStack::PopAndDestroy( packet );
    EUNIT_ASSERT(ETrue);
    }  
    
void UT_CRtpStream::UT_CRtpStream_TestRtpRRMethodsL()
    {
    //"") );
    // "Test description: Test creating and parsing an RR packet" ) );
    // "CRtpRecvStream::RtpStreamCreateRtcpReportSection( CRtpPacket* )" ) );
    // "CRtpTranStream::RtpStreamProcessRtcpReportSection( CRtpPacket* )" ) );
    // Create a packet to work with
    TUint packetSize = 2048;
    CRtpPacket* packet = CRtpPacket::NewL( packetSize, iProfileRTPTimeRates );
    CleanupStack::PushL( packet );

    // Set some parameters
    iRecvStream->iSeqNum = 0;
    iRecvStream->iBaseSeqNum = 0;
    iRecvStream->iSeqNumCycles = 0;
    iRecvStream->iNumWrapAround = 0;
    iRecvStream->iLastRR_numExpectedPackets = 1;
    iRecvStream->iSyncJitter.iTimeStampResolution = 0;
    iRecvStream->iNumReceivedPackets = 5;
    
    // Create the RTCP report    
    iRecvStream->RtpStreamCreateRtcpReportSection( packet );

    // Make sure the packet can be processed
    packet->RtpPacketResetPtr();
    packet->iSize = 24; // size of SR section
    packet->SetType( ERTCP_RR );

    // Process the report
    TRtpRtcpEnum error = iTranStream->RtpStreamProcessRtcpReportSectionL( packet );

    // Check the results
    if ( error != KErrNone )
        {
        // "Error parsing RR packet" ) );
        // "CRtpRecvStream::RtpStreamCreateRtcpReportSection() - FAILED" ) );
        // "CRtpTranStream::RtpStreamProcessRtcpReportSection() - FAILED" ) );
        CleanupStack::PopAndDestroy( packet );
        EUNIT_ASSERT(EFalse);
        }

    // Set some parameters differently to reach more branches
    iRecvStream->iNumReceivedPackets = 0;
    iRecvStream->iLastRR_numExpectedPackets = 0;
    iRecvStream->iSyncJitter.iTimeStampResolution = 10;

    // Create the RTCP report    
    iRecvStream->RtpStreamCreateRtcpReportSection( packet );

    // Make sure the packet can be processed
    packet->RtpPacketResetPtr();
    packet->iSize = 24; // size of SR section
    packet->SetType( ERTCP_RR );

    // Process the report
    error = iTranStream->RtpStreamProcessRtcpReportSectionL( packet );

    // Check the results
    if ( error != KErrNone )
        {
        // "Error parsing RR packet" ) );
        // "CRtpRecvStream::RtpStreamCreateRtcpReportSection() - FAILED" ) );
        // "CRtpTranStream::RtpStreamProcessRtcpReportSection() - FAILED" ) );
        CleanupStack::PopAndDestroy( packet );
        EUNIT_ASSERT(EFalse);
        }

    // Set some parameters differently to reach more branches
    iRecvStream->iLastRR_numExpectedPackets = 0xFFFF0000;

    // Create the RTCP report    
    iRecvStream->RtpStreamCreateRtcpReportSection( packet );

    // Make sure the packet can be processed
    packet->RtpPacketResetPtr();
    packet->iSize = 24; // size of SR section
    packet->SetType( ERTCP_RR );

    // Process the report
    error = iTranStream->RtpStreamProcessRtcpReportSectionL( packet );

    // Check the results
    if ( error != KErrNone )
        {
        // "Error parsing RR packet" ) );
        // "CRtpRecvStream::RtpStreamCreateRtcpReportSection() - FAILED" ) );
        // "CRtpTranStream::RtpStreamProcessRtcpReportSection() - FAILED" ) );
        CleanupStack::PopAndDestroy( packet );
        EUNIT_ASSERT(EFalse);
        }

    // "CRtpRecvStream::RtpStreamCreateRtcpReportSection() - passed" ) );
    // "CRtpTranStream::RtpStreamProcessRtcpReportSection() - passed" ) );

    CleanupStack::PopAndDestroy( packet );
    EUNIT_ASSERT(ETrue);
    }

// -----------------------------------------------------------------------------
// Test BYE and APP related methods of CRtpTranStream
// -----------------------------------------------------------------------------
//
void UT_CRtpStream::UT_CRtpStream_TestStreamStatAndSdesMethods()
    {
    TBuf8<3> cName; 
    TBuf8<3> userName;
    TBuf8<3> email;
    TBuf8<3> phoneNumber;
    TBuf8<3> location;
    TBuf8<3> swToolName;
    TBuf8<3> noticeStatus;
    TBuf8<3> privateStr;

    //"") );
    // "Test description: Testing getting remote SDES info" ) );
    // "CRtpRecvStream::GetRemoteStreamInfo( const TRtpSdesParams& )" ) );

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

    TRtpSdesParams params;
    params.iCName.Set( cName.Ptr() );
    params.iUserName.Set( userName.Ptr() );
    params.iEmail.Set( email.Ptr() );
    params.iPhoneNumber.Set( phoneNumber.Ptr() );
    params.iLocation.Set( location.Ptr() );
    params.iSwToolName.Set( swToolName.Ptr() );
    params.iNoticeStatus.Set( noticeStatus.Ptr() );
    params.iPrivate.Set( privateStr.Ptr() );
    
    // Set it
    iRecvStream->iRemoteSDES->SetSDES( params );

    // Run the function
    TRtpSdesParams params2;
    iRecvStream->GetRemoteStreamInfo( params2 );
    
    // Compare the results
    if ( params.iCName != cName
      || params.iUserName != userName
      || params.iEmail != email
      || params.iPhoneNumber != phoneNumber
      || params.iLocation != location
      || params.iSwToolName != swToolName
      || params.iNoticeStatus != noticeStatus
      || params.iPrivate != privateStr )
        {
        // "Parameters incorrectly updated" ) );
        // "CRtpRecvStream::GetRemoteStreamInfo() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    // "CRtpRecvStream::GetRemoteStreamInfo() - passed" ) );

    //"") );
    // "Test description: Testing getting stream statistics" ) );
    // "CRtpRecvStream::GetStreamStat( TRtpPeerStat& )" ) );

    TRtpPeerStat stats;
    
    // Set some parameters
    iRecvStream->iRtcpStats.iRtcpSenderStats.iCumNumOctetsSent = 101;
    iRecvStream->iRtcpStats.iRtcpSenderStats.iNumPacketsSent = 102;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iTxBandwidth = 103;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iArrivalJitter = 104;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iCumNumPacketsLost = 105;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iFractionLost = 106;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iRoundTripDelay = 107;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iBandwidth = 108;
    iRecvStream->iRtcpStats.iRtcpReceiverStats.iChannelBufferSize = 109;
    
    // Get the info
    iRecvStream->GetStreamStat( stats );
    
    // Compare it with our original data
    if ( stats.iCumNumOctetsSent != 101 ||
         stats.iNumPacketsSent != 102 ||
         stats.iTxBandwidth != 103 ||
         stats.iArrivalJitter != 104 ||
         stats.iCumNumPacketsLost != 105 ||
         stats.iFractionLost != 106 ||
         stats.iRoundTripDelay != 107 ||
         stats.iRxBandwidth != 108 ||
         stats.iChannelBufferSize != 109 )
        {
        // "Parameters incorrectly updated" ) );
        // "CRtpRecvStream::GetStreamStat() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }
    
    EUNIT_ASSERT(ETrue);
    }

void UT_CRtpStream::UT_CRtpStream_RtpStreamProcessRtpPacketL()
	{
	TInt result(KErrNone);
	TInt error(KErrNone);
	
    CRtpPacket* pktSnd = CRtpPacket::NewL( KMaxRtpPacketSize, iProfileRTPTimeRates );
	CleanupStack::PushL( pktSnd );	
	// "Test description: build a RTP packet" ) );
	// "CRtpPacket::RtpPacketBuildRtp( &streamParam, &initParam )" ));
	
	///////////////////////TEST CASE 1///////////////////////////////////
    TRtpPacketStreamParam iStreamParam;
    TRtpPacketIOParam iIoParam;
    
    
    pktSnd->RtpPacketReset();
    pktSnd->RtpPacketResetPtr();
    pktSnd->SetType( ERTP );
    
    TBuf8<15> data; 
    data.Format( _L8( "12345678" ) );
    data.Append( _L8( "9" ) );
    
    iIoParam.TRTP.extension.data = const_cast<TUint8*>(data.Ptr());
    iIoParam.TRTP.extension.length = data.Length();
   	result = pktSnd->RtpPacketBuild( &iStreamParam, &iIoParam );
   	result = iRecvStream->RtpStreamProcessRtpPacketL(pktSnd, EFalse);
    EUNIT_ASSERT( result == KErrNone );
    CleanupStack::PopAndDestroy( pktSnd );
	}    
             
//From Observer    
void UT_CRtpStream::RtpPacketReceived( TRtpId /*aStreamId*/, 
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
	
void UT_CRtpStream::SdesReceived( TRtpSSRC /*aSSRC*/, const TRtpSdesParams& /*aParams*/ )
{
	
}
void UT_CRtpStream::ByeReceived( TRtpId /*aStreamId*/, TRtpSSRC /*aSSRC*/, 
								const TDesC8& /*aReason*/ )
{
	
}
void UT_CRtpStream::AppReceived( TRtpId /*aStreamId*/, TRtpSSRC /*aSSRC*/,
								 const TRtcpApp& /*aApp*/ )
{
	
}
void UT_CRtpStream::SrReceived( TRtpId /*aStreamId*/, 
							TRtpSSRC /*aSSRC*/, const TTimeStamps& /*aTimeStamps*/ )
{
	
}
void UT_CRtpStream::RrReceived( TRtpId /*aStreamId*/, TRtpSSRC /*aSSRC*/ )
{
	
}	// from MSsrcCheckCallback
TInt UT_CRtpStream::CheckRemoteSsrcL( TRtpSSRC& /*aSSRC*/ )
{
return iWantedResult;	
}

TBool UT_CRtpStream::CheckRemoteAddr( )
{
return ETrue;	
}

void UT_CRtpStream::Hex(HBufC8& aString)
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
    UT_CRtpStream,
    "CRtpStream",
    "UNIT" )

EUNIT_TEST(
    "ResetStreamStat - test ",
    "CRtpStream",
    "ResetStreamStat",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStream_ResetStreamStatL, Teardown)

EUNIT_TEST(
    "FirstPkg - test ",
    "CRtpStream",
    "FirstPkg",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStream_FirstPkgL, Teardown)

EUNIT_TEST(
    "SetFirstPkg - test ",
    "CRtpStream",
    "SetFirstPkg",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStream_SetFirstPkgL, Teardown)

EUNIT_TEST(
    "GetLocalSSRC - test ",
    "CRtpStream",
    "GetLocalSSRC",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStream_GetLocalSSRCL, Teardown)

EUNIT_TEST(
    "SetLocalSSRC - test ",
    "CRtpStream",
    "SetLocalSSRC",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStream_SetLocalSSRCL, Teardown)

EUNIT_TEST(
    "RegisterRtcpObserver - test ",
    "CRtpStream",
    "RegisterRtcpObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStream_RegisterRtcpObserverL, Teardown)

EUNIT_TEST(
    "UnRegisterRtcpObserver - test ",
    "CRtpStream",
    "UnRegisterRtcpObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStream_UnRegisterRtcpObserverL, Teardown)

EUNIT_TEST(
    "GetStreamID - test ",
    "CRtpStream",
    "GetStreamID",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStream_GetStreamIDL, Teardown)

EUNIT_TEST(
    "RtcpStats - test ",
    "CRtpStream",
    "RtcpStats",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStream_RtcpStatsL, Teardown)

EUNIT_TEST(
    "GetStreamStat - test ",
    "CRtpStream",
    "GetStreamStat",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStream_GetStreamStatL, Teardown)

EUNIT_TEST(
    "RtpStreamProcessRtcpReportSection - test ",
    "CRtpStream",
    "RtpStreamProcessRtcpReportSection",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStream_RtpStreamProcessRtcpReportSectionL, Teardown)

EUNIT_TEST(
    "RtpStreamCreateRtcpReportSection - test ",
    "CRtpStream",
    "RtpStreamCreateRtcpReportSection",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStream_RtpStreamCreateRtcpReportSectionL, Teardown)

EUNIT_TEST(
    "UpdateParam ",
    "CRtpStream",
    "UT_CRtpStream_UpdateParam",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStream_UpdateParamL, Teardown)    
EUNIT_TEST(
    "RtpByeAppMethods",
    "CRtpStream",
    "RtpByeAppMethods",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStream_RtpByeAppMethodsL, Teardown)     

EUNIT_TEST(
    "TestRtpSRMethods",
    "CRtpStream",
    "TestRtpSRMethods",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStream_TestRtpSRMethodsL, Teardown)    

EUNIT_TEST(
    "TestRtpRRMethods",
    "CRtpStream",
    "TestRtpRRMethods",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStream_TestRtpRRMethodsL, Teardown)   

EUNIT_TEST(
    "TestRtpRRMethods",
    "CRtpStream",
    "TestRtpRRMethods",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStream_TestStreamStatAndSdesMethods, Teardown)  
EUNIT_TEST(
    "StreamProcessRtpPacket",
    "CRtpStream",
    "RtpStreamProcessRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStream_RtpStreamProcessRtpPacketL, Teardown)      
    


EUNIT_END_TEST_TABLE

//  END OF FILE
