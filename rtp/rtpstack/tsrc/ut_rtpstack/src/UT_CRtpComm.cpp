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
#include "UT_CRtpComm.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "rtpcomm.h"

const TInt KTimerDelay = 200;
const TInt KDefaultIap=6;
const TInt KRTPReceived=1;
const TInt KRTCPReceived=2;
const TInt KPacketReject=3;

#define RTP_EUNIT_ASSERT_EQUALS( val1, val2 ) \
if ( val2 != KErrNoMemory && val1 == KErrNoMemory ){\
	User::Leave( KErrNoMemory );\
	}\
else{\
	EUNIT_ASSERT_EQUALS( val1, val2 );\
	}

// CONSTRUCTION
UT_CRtpComm* UT_CRtpComm::NewL()
    {
    UT_CRtpComm* self = UT_CRtpComm::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CRtpComm* UT_CRtpComm::NewLC()
    {
    UT_CRtpComm* self = new( ELeave ) UT_CRtpComm();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CRtpComm::~UT_CRtpComm()
    {
    }

// Default constructor
UT_CRtpComm::UT_CRtpComm()
    {
    }

// Second phase construct
void UT_CRtpComm::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_CRtpComm::SetupL(  )
    {
	iTimer = CPeriodic::NewL( EPriorityNormal );
	iErr=0;
	TCommDbConnPref prefs;
    prefs.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );	
	prefs.SetDirection( ECommDbConnectionDirectionOutgoing );
    prefs.SetIapId( KDefaultIap );//6
	
    
    TInt err( KErrCouldNotConnect );
        
    // Opens a session to the socket server
    // i.e. creates an IPC communication channel to the socket server
    if ( ( err = iSocketServ.Connect() ) != KErrNone )
    	{
    	EUNIT_ASSERT( err==KErrNone );
    	}
    // Opens a new RConnection instance
    else if (( err = iConnection.Open( iSocketServ )) != KErrNone )
       	{
		iConnection.Close(); 
		EUNIT_ASSERT( err==KErrNone );
        }
    else if (( err = iConnection.Start( prefs )) != KErrNone )
        {	
        iConnection.Close();
        EUNIT_ASSERT( err==KErrNone );
        }
    }

void UT_CRtpComm::Teardown(  )
    {
    delete iTimer;
	iTimer = NULL;
	delete iRtpComm;
    iRtpComm = NULL;
    iConnection.Close();
    iSocketServ.Close();

    }

void UT_CRtpComm::UT_CRtpComm_NewLL(  )
    {
    TCreateSessionParams params;
    TInt err( KErrNone );
    TInetAddr rtpAddr;
    TInetAddr rtcpAddr;
    iLocalPort = 7777; // this should be changed to an even number by CRtpComm
    
    /////////////////////////////////////////////////
    // This object will be destroyed below
    // It is used for testing construction without RTCP on an odd port
    TRAP( err, iRtpComm = CRtpComm::NewL( iLocalPort,
                               iSocketServ,
                               iConnection,
                               params,
                               *this,
                               EFalse ) );
                               
    RTP_EUNIT_ASSERT_EQUALS ( err, KErrNone );
        
    // Check some initial values
    if ( iLocalPort != 7776 ||
         iRtpComm->iSender[ERTPPort] != NULL ||
         iRtpComm->iSender[ERTCPPort] != NULL ||
         iRtpComm->iReceiver[ERTPPort] != NULL ||
         iRtpComm->iReceiver[ERTCPPort] != NULL )
        {
        EUNIT_ASSERT ( EFalse );
    
        }
    
    delete iRtpComm;
    iRtpComm = NULL;

    
    iLocalPort = 5000; // use default port 5000
    
    /////////////////////////////////////////////////
    // This object will be destroyed below
    // It is used for testing construction without RTCP on an even port
    TRAP( err, iRtpComm = CRtpComm::NewL( iLocalPort,
                               iSocketServ,
                               iConnection,
                               params,
                               *this,
                               EFalse ) );
                               
    RTP_EUNIT_ASSERT_EQUALS ( err, KErrNone );
        
    // Check some initial values
    if ( iLocalPort == 0 ||
         iRtpComm->iSender[ERTPPort] != NULL ||
         iRtpComm->iSender[ERTCPPort] != NULL ||
         iRtpComm->iReceiver[ERTPPort] != NULL ||
         iRtpComm->iReceiver[ERTCPPort] != NULL )
        {
        EUNIT_ASSERT ( EFalse );
    	}


    // This should only construct the RTP sender
    iRtpComm->ConstructSenderL(rtpAddr, rtcpAddr);

    if ( iRtpComm->iSender[ERTPPort] == NULL ||
         iRtpComm->iSender[ERTCPPort] != NULL )
        {
        EUNIT_ASSERT ( EFalse );
        }

    // Calling the function again should do nothing
    CRtpCommSend* tmpSender = iRtpComm->iSender[ERTPPort];
    iRtpComm->ConstructSenderL(rtpAddr, rtcpAddr);

    if ( iRtpComm->iSender[ERTPPort] == NULL ||
         iRtpComm->iSender[ERTCPPort] != NULL ||
         iRtpComm->iSender[ERTPPort] != tmpSender )
        {
        EUNIT_ASSERT ( EFalse );
        }


    // This should only construct the RTP receiver
    iRtpComm->ConstructReceiverL(EFalse);
    if ( iRtpComm->iReceiver[ERTPPort] == NULL ||
         iRtpComm->iReceiver[ERTCPPort] != NULL )
        {
        EUNIT_ASSERT ( EFalse );
        }

    // Calling the function again should do nothing
    CRtpCommRecv* tmpReceiver = iRtpComm->iReceiver[ERTPPort];
    iRtpComm->ConstructReceiverL(EFalse);

    if ( iRtpComm->iReceiver[ERTPPort] == NULL ||
         iRtpComm->iReceiver[ERTCPPort] != NULL ||
         iRtpComm->iReceiver[ERTPPort] != tmpReceiver )
        {
        EUNIT_ASSERT ( EFalse );
        }

    delete iRtpComm;
    iRtpComm = NULL;

    //////////////////////////////////////////////////////
    // Now, lets create a new CRtpComm object WITH RTCP. This time, we'll keep
    // it and work with it in other methods of this test class.

    TRAP( err, iRtpComm = CRtpComm::NewL( iLocalPort,
                               iSocketServ,
                               iConnection,
                               params,
                               *this,
                               ETrue ) );
    
    if ( err == KErrNoMemory )
        {
        User::Leave( KErrNoMemory );
        }
    if ( err != KErrNone )
        {
        EUNIT_ASSERT ( EFalse );
        }
        
    // Check some initial values
    if ( iLocalPort == 0 ||
         iRtpComm->iSender[ERTPPort] != NULL ||
         iRtpComm->iSender[ERTCPPort] != NULL ||
         iRtpComm->iReceiver[ERTPPort] != NULL ||
         iRtpComm->iReceiver[ERTCPPort] != NULL )
        {
        EUNIT_ASSERT ( EFalse );
        }

 
    // This should only construct the RTP sender
    iRtpComm->ConstructSenderL(rtpAddr, rtcpAddr);

    if ( iRtpComm->iSender[ERTPPort] == NULL ||
         iRtpComm->iSender[ERTCPPort] == NULL )
        {
        EUNIT_ASSERT ( EFalse );
        }

    // Calling the function again should do nothing
    tmpSender = iRtpComm->iSender[ERTPPort];
    iRtpComm->ConstructSenderL(rtpAddr, rtcpAddr);

    if ( iRtpComm->iSender[ERTPPort] == NULL ||
         iRtpComm->iSender[ERTCPPort] == NULL ||
         iRtpComm->iSender[ERTPPort] != tmpSender )
        {
        EUNIT_ASSERT ( EFalse );
        }

    // This should only construct the RTP receiver
    iRtpComm->ConstructReceiverL(EFalse);
    if ( iRtpComm->iReceiver[ERTPPort] == NULL ||
         iRtpComm->iReceiver[ERTCPPort] == NULL )
        {
		EUNIT_ASSERT ( EFalse );
        }

    // Calling the function again should do nothing
    tmpReceiver = iRtpComm->iReceiver[ERTPPort];
    iRtpComm->ConstructReceiverL(EFalse);

    if ( iRtpComm->iReceiver[ERTPPort] == NULL ||
         iRtpComm->iReceiver[ERTCPPort] == NULL ||
         iRtpComm->iReceiver[ERTPPort] != tmpReceiver )
        {
		EUNIT_ASSERT ( EFalse );
        }

    }

void UT_CRtpComm::UT_CRtpComm_ConstructSenderLL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpComm::UT_CRtpComm_ConstructReceiverLL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpComm::UT_CRtpComm_SetToAddressL(  )
    {
    TCreateSessionParams params;
    TInt err( KErrNone );
    TInetAddr aRtpAddr;
    TInetAddr aRtcpAddr;
    iLocalPort = 7777; // this should be changed to an even number by CRtpComm
    
    /////////////////////////////////////////////////
    // This object will be destroyed below
    // It is used for testing construction without RTCP on an odd port
    TRAP( err, iRtpComm = CRtpComm::NewL( iLocalPort,
                               iSocketServ,
                               iConnection,
                               params,
                               *this,
                               EFalse ) );
                               
    RTP_EUNIT_ASSERT_EQUALS ( err, KErrNone );
        
    // Check some initial values
    if ( iLocalPort != 7776 ||
         iRtpComm->iSender[ERTPPort] != NULL ||
         iRtpComm->iSender[ERTCPPort] != NULL ||
         iRtpComm->iReceiver[ERTPPort] != NULL ||
         iRtpComm->iReceiver[ERTCPPort] != NULL )
        {
        EUNIT_ASSERT ( EFalse );
    
        }
    
    
    iLocalPort = 5000; // use default port 5000
  
    TInt result( KErrNone );
    TBuf8<5> sendBuf;
    TBuf8<5> recvBuf;

    // Test setting a port to an address
    
    // Run the method
    TInetAddr dummyAddr( INET_ADDR( 127, 0, 0, 1 ), 5050 );
    TInetAddr remoteAddr( INET_ADDR( 127, 0, 0, 1 ), 5000 );
    TInt error = iRtpComm->SetToAddress( ERTPPort, dummyAddr );
    EUNIT_ASSERT ( error == KErrNone )
    
    // Test writing synchronously to a port
    // Run the method
    
    //
	iRtpComm->ConstructSenderL(remoteAddr, dummyAddr);
	err = iRtpComm->Send( ERTPPort, sendBuf );
	DelayL();
	iRtpComm->CancelSend( ERTPPort );
	DelayL();
	 // Check the results
    RTP_EUNIT_ASSERT_EQUALS ( err, KErrNone );
        
    // Run the method
    TRequestStatus status;
    iRtpComm->Send( ERTPPort, sendBuf, status );
    EUNIT_ASSERT( KRequestPending == status.Int() );
    User::WaitForRequest( iRtpComm->iSender[ERTPPort]->iStatus );
    iRtpComm->iSender[ERTPPort]->RunL();
    iRtpComm->iSender[ERTPPort]->iStatus = TRequestStatus();
    User::WaitForRequest( status );
    
    // Check the results
    EUNIT_ASSERT ( status == KErrNone )
        
    // Run the method
    iRtpComm->CancelSend( ERTPPort );
	DelayL();
	
    // Can't verify success here   
    
    // Run the method
    iRtpComm->RegisterReceivedNotify( this );
    
  
    iRtpComm->ConstructReceiverL(EFalse);    
    // Can't verify success here   
    err = iRtpComm->Receive( ERTPPort, recvBuf );
	RTP_EUNIT_ASSERT_EQUALS ( err, KErrNone );
    
	err = iRtpComm->Send( ERTPPort, sendBuf );
	DelayL();
	iRtpComm->CancelSend( ERTPPort );
	DelayL();
    // Check the results
    RTP_EUNIT_ASSERT_EQUALS ( err, KErrNone );
    
    RSocket* socket( NULL );
    
    // Run the method
    socket = iRtpComm->GetSocket( ERTPPort );

    TProtocolDesc desc1, desc2;
    
    socket->Info( desc1 );
    iRtpComm->iSocket[ERTPPort].Info( desc2 );

    // The info should match
    if ( desc1.iAddrFamily != desc2.iAddrFamily ||
         desc1.iByteOrder != desc2.iByteOrder ||
         desc1.iMessageSize != desc2.iMessageSize ||
         desc1.iName != desc2.iName ||
         desc1.iNamingServices != desc2.iNamingServices ||
         desc1.iProtocol != desc2.iProtocol ||
         desc1.iSecurity != desc2.iSecurity ||
         desc1.iServiceInfo != desc2.iServiceInfo ||
         desc1.iSockType != desc2.iSockType )
        {
        EUNIT_ASSERT(EFalse);
        }
    }
    
void UT_CRtpComm::UT_CRtpComm_CommReceiveL(  )
    {
    TCreateSessionParams params;
    TInt err( KErrNone );
    TInetAddr aRtpAddr;
    TInetAddr aRtcpAddr;
    iLocalPort = 7777; // this should be changed to an even number by CRtpComm
    
    /////////////////////////////////////////////////
    // This object will be destroyed below
    // It is used for testing construction without RTCP on an odd port
    TRAP( err, iRtpComm = CRtpComm::NewL( iLocalPort,
                               iSocketServ,
                               iConnection,
                               params,
                               *this,
                               EFalse ) );
                               
    RTP_EUNIT_ASSERT_EQUALS ( err, KErrNone );
        
    // Check some initial values
    if ( iLocalPort != 7776 ||
         iRtpComm->iSender[ERTPPort] != NULL ||
         iRtpComm->iSender[ERTCPPort] != NULL ||
         iRtpComm->iReceiver[ERTPPort] != NULL ||
         iRtpComm->iReceiver[ERTCPPort] != NULL )
        {
        EUNIT_ASSERT ( EFalse );
    
        }
    
    
    iLocalPort = 5000; // use default port 5000
  
    TInt result( KErrNone );
    TBuf8<5> sendBuf;
    TBuf8<5> recvBuf;

    // Test setting a port to an address
    
    // Run the method
    TInetAddr dummyAddr( INET_ADDR( 127, 0, 0, 1 ), 5050 );
    TInetAddr remoteAddr( INET_ADDR( 127, 0, 0, 1 ), 5000 );
    TInt error = iRtpComm->SetToAddress( ERTPPort, dummyAddr );
    EUNIT_ASSERT ( error == KErrNone )
    
    // Test writing synchronously to a port
    // Run the method
    
 
    iRtpComm->ConstructReceiverL(EFalse);    
    // Can't verify success here   
    err = iRtpComm->Receive( ERTPPort, recvBuf );
	RTP_EUNIT_ASSERT_EQUALS ( err, KErrNone );
    

    }    

void UT_CRtpComm::UT_CRtpComm_SetAcceptedFromAddressL(  )
    {
    TCreateSessionParams params;
    TInt err( KErrNone );
    TInetAddr rtpAddr;
    TInetAddr rtcpAddr;
    iLocalPort = 7777; // this should be changed to an even number by CRtpComm
    
    /////////////////////////////////////////////////
    // This object will be destroyed below
    // It is used for testing construction without RTCP on an odd port
    TRAP( err, iRtpComm = CRtpComm::NewL( iLocalPort,
                               iSocketServ,
                               iConnection,
                               params,
                               *this,
                               ETrue ) );
                               
    RTP_EUNIT_ASSERT_EQUALS ( err, KErrNone );
        
    // Check some initial values
    if ( iLocalPort != 7776 ||
         iRtpComm->iSender[ERTPPort] != NULL ||
         iRtpComm->iSender[ERTCPPort] != NULL ||
         iRtpComm->iReceiver[ERTPPort] != NULL ||
         iRtpComm->iReceiver[ERTCPPort] != NULL )
        {
        EUNIT_ASSERT ( EFalse );
    
        }
    
    iLocalPort = 5000; // use default port 5000
  
    TInt result( KErrNone );
    TBuf8<5> sendBuf;
    TBuf8<5> recvBuf;

    // Test setting a port to an address
    
    // Run the method
    TInetAddr dummyAddr( INET_ADDR( 127, 0, 0, 1 ), 5000 );
     iRtpComm->RegisterReceivedNotify( this );
    
    iRtpComm->ConstructReceiverL(EFalse);    
   	iRtpComm->SetAcceptedFromAddress( dummyAddr );
    EUNIT_ASSERT ( iRtpComm->iReceiver[ERTPPort] ->iRemoteAddr==dummyAddr );
    
    iRtpComm->SetNonRtpObserverFlag(ETrue);
    }

void UT_CRtpComm::UT_CRtpComm_SetRecvCallbackL(  )
    {
    EUNIT_ASSERT( ETrue );
    //no actual implementation in CRtpComm class
    }

void UT_CRtpComm::UT_CRtpComm_SendL(  )
    {
    EUNIT_ASSERT( ETrue );
    //Tested above
    
    }

void UT_CRtpComm::UT_CRtpComm_Send_1L(  )
    {
    EUNIT_ASSERT( ETrue );
    //Tested above
    }

void UT_CRtpComm::UT_CRtpComm_CancelSendL(  )
    {
    EUNIT_ASSERT( ETrue );
    //Tested above
    }

void UT_CRtpComm::UT_CRtpComm_ReceiveL(  )
    {
    EUNIT_ASSERT( ETrue );
    //Tested above
    }

void UT_CRtpComm::UT_CRtpComm_RegisterReceivedNotifyL(  )
    {
    EUNIT_ASSERT( ETrue );
    //Tested above
    }

void UT_CRtpComm::UT_CRtpComm_GetSocketL(  )
    {
    EUNIT_ASSERT( ETrue );
    //Tested above
    }

void UT_CRtpComm::UT_CRtpComm_SetNonRtpObserverFlagL(  )
    {
    TInt err(KErrNone);
    TCreateSessionParams params;
     TRAP( err, iRtpComm = CRtpComm::NewL( iLocalPort,
                               iSocketServ,
                               iConnection,
                               params,
                               *this,
                               EFalse ) );
                               
    RTP_EUNIT_ASSERT_EQUALS ( err, KErrNone );
     TInetAddr dummyAddr( INET_ADDR( 127, 0, 0, 1 ), 5000 );
    iRtpComm->RegisterReceivedNotify( this );
    
    iRtpComm->ConstructReceiverL(EFalse);    
   	iRtpComm->SetAcceptedFromAddress( dummyAddr );
   	delete iRtpComm->iReceiver[ERTCPPort];
   	iRtpComm->iReceiver[ERTCPPort]=NULL;
    iRtpComm->SetNonRtpObserverFlag(ETrue);
    delete iRtpComm->iReceiver[ERTPPort];
    iRtpComm->iReceiver[ERTPPort]=NULL;
    iRtpComm->SetNonRtpObserverFlag(EFalse);
    delete iRtpComm; iRtpComm=NULL;
    }

void UT_CRtpComm::UT_CRtpComm_MaxSocketSizeL()
    {
    TCreateSessionParams params;
    params.iSocketBufSize = 10000;
    iRtpComm = CRtpComm::NewL( iLocalPort,
                               iSocketServ,
                               iConnection,
                               params,
                               *this,
                               EFalse );
    EUNIT_ASSERT_EQUALS( iRtpComm->MaxSocketSize(), 10000 );
    }
    
//From Observer
// -----------------------------------------------------------------------------
// Receive notify
// -----------------------------------------------------------------------------
//
void UT_CRtpComm::OnRtpReceivedL()
    {
    iErr=KRTPReceived;
    }

// -----------------------------------------------------------------------------
// Receive notify
// -----------------------------------------------------------------------------
//
void UT_CRtpComm::OnRtcpReceivedL()
    {
    iErr=KRTCPReceived;
    }

// -----------------------------------------------------------------------------
// Error notify
// -----------------------------------------------------------------------------
//
void UT_CRtpComm::OnReceptionError( TPortType aPort, TInt aError )
    {
    if ( aPort == ERTPPort )
        {
        iErr=aError;
        }
    else
        {
        iErr=aError;
        }
    }

// -----------------------------------------------------------------------------
// packet rejected
// -----------------------------------------------------------------------------
//
void UT_CRtpComm::OnPacketRejected(TPortType /*aPort*/)
    {
    iErr= KPacketReject;    
    }
void UT_CRtpComm::ErrorNotify( TInt aErrCode )
	{
	iErr=aErrCode;
	}
//test AO 
TInt UT_CRtpComm::StopScheduler( TAny* aThis )
	{
	UT_CRtpComm* self = reinterpret_cast< UT_CRtpComm* >( aThis );
	// set test to pass and stop scheduler
	self->iCallBackCalled = ETrue;
	CActiveScheduler::Stop();
	return 0;
	}
	
// This delay function gives opportunity for the scheduler to run other
// active objects. The use of this function also ensures that all requests
// are completed within the test function.
void UT_CRtpComm::DelayL()
	{
	// set the end condition into false
	iCallBackCalled = EFalse;

	// set a timer for the test
	TCallBack callBack( StopScheduler, this );
	iTimer->Start( KTimerDelay, KTimerDelay, callBack );

	// wait for completion
	CActiveScheduler::Start();

	// cancel requests
	iTimer->Cancel();

	EUNIT_ASSERT( iCallBackCalled );
	}    

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CRtpComm,
    "CRtpComm Test",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "CRtpComm",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpComm_NewLL, Teardown)

EUNIT_TEST(
    "ConstructSenderL - test ",
    "CRtpComm",
    "ConstructSenderL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpComm_ConstructSenderLL, Teardown)

EUNIT_TEST(
    "ConstructReceiverL - test ",
    "CRtpComm",
    "ConstructReceiverL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpComm_ConstructReceiverLL, Teardown)

EUNIT_TEST(
    "SetToAddress - test ",
    "CRtpComm",
    "SetToAddress",
    "FUNCTIONALITY",
    SetupL, UT_CRtpComm_SetToAddressL, Teardown)

EUNIT_TEST(
    "SetAcceptedFromAddress - test ",
    "CRtpComm",
    "SetAcceptedFromAddress",
    "FUNCTIONALITY",
    SetupL, UT_CRtpComm_SetAcceptedFromAddressL, Teardown)

EUNIT_TEST(
    "SetRecvCallback - test ",
    "CRtpComm",
    "SetRecvCallback",
    "FUNCTIONALITY",
    SetupL, UT_CRtpComm_SetRecvCallbackL, Teardown)

EUNIT_TEST(
    "Send - test ",
    "CRtpComm",
    "Send",
    "FUNCTIONALITY",
    SetupL, UT_CRtpComm_SendL, Teardown)

EUNIT_TEST(
    "Send - test ",
    "CRtpComm",
    "Send",
    "FUNCTIONALITY",
    SetupL, UT_CRtpComm_Send_1L, Teardown)

EUNIT_TEST(
    "CancelSend - test ",
    "CRtpComm",
    "CancelSend",
    "FUNCTIONALITY",
    SetupL, UT_CRtpComm_CancelSendL, Teardown)

EUNIT_TEST(
    "Receive - test ",
    "CRtpComm",
    "Receive",
    "FUNCTIONALITY",
    SetupL, UT_CRtpComm_ReceiveL, Teardown)

EUNIT_TEST(
    "RegisterReceivedNotify - test ",
    "CRtpComm",
    "RegisterReceivedNotify",
    "FUNCTIONALITY",
    SetupL, UT_CRtpComm_RegisterReceivedNotifyL, Teardown)

EUNIT_TEST(
    "GetSocket - test ",
    "CRtpComm",
    "GetSocket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpComm_GetSocketL, Teardown)

EUNIT_TEST(
    "SetNonRtpObserverFlag - test ",
    "CRtpComm",
    "SetNonRtpObserverFlag",
    "FUNCTIONALITY",
    SetupL, UT_CRtpComm_SetNonRtpObserverFlagL, Teardown)
EUNIT_TEST(
    "CommReceiveL - test ",
    "CRtpComm",
    "CommReceiveL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpComm_CommReceiveL, Teardown)
    
EUNIT_TEST(
    "MaxSocketSize - test ",
    "CRtpComm",
    "MaxSocketSize",
    "FUNCTIONALITY",
    SetupL, UT_CRtpComm_MaxSocketSizeL, Teardown)
    
EUNIT_END_TEST_TABLE

//  END OF FILE
