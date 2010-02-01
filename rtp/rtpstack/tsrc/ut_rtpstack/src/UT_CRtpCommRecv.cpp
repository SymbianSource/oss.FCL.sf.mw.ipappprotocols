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
#include "UT_CRtpCommRecv.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "rtpcommrecv.h"
const TInt KTimerDelay = 100;
const TUint KIAPID = 6;
const TInt KLength = 50;
const TInt KPort = 500;

_LIT8(KHello, "hello...receivingtesting");
// CONSTRUCTION
UT_CRtpCommRecv* UT_CRtpCommRecv::NewL()
    {
    UT_CRtpCommRecv* self = UT_CRtpCommRecv::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CRtpCommRecv* UT_CRtpCommRecv::NewLC()
    {
    UT_CRtpCommRecv* self = new( ELeave ) UT_CRtpCommRecv();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CRtpCommRecv::~UT_CRtpCommRecv()
    {
    }

// Default constructor
UT_CRtpCommRecv::UT_CRtpCommRecv()
    {
    }

// Second phase construct
void UT_CRtpCommRecv::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_CRtpCommRecv::SetupL(  )
    {
	TCommDbConnPref prefs;
    prefs.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );	
	prefs.SetDirection( ECommDbConnectionDirectionOutgoing );
    prefs.SetIapId( KIAPID );//6
	
    
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
 	
 	err = iSocket.Open( iSocketServ, 
                                  KAfInet,
                                  KSockDatagram, 
                                  KProtocolInetUdp, 
                                  iConnection 
                                );   
    TCreateSessionParams params;
    iPriority = 0;/*aParams.iPriority;*/                                     
	iReceiver= CRtpCommRecv::NewL( ERTPPort, iSocket, 
                                                  iPriority, *this, ETrue );
    TInetAddr remoteAddr( INET_ADDR( 127, 0, 0, 1 ), 5000 );
    iReceiver->SetRemoteAddress(remoteAddr);                                              

	iEventCount = 0;    
	iErr=0;
	iPacketCount=0;
	iTimer = CPeriodic::NewL( EPriorityNormal );
    }

void UT_CRtpCommRecv::Teardown(  )
    {
	if( iEventCount > 0)
		{
		if( !iWait.IsStarted() )
			{
			iWait.Start();
			}
		}
	delete iTimer;
	iTimer = NULL;

	if (iReceiver)
		{		
		iReceiver->DoCancel();
		}
	delete iReceiver;
    iReceiver =NULL;
    iSocket.Close();
    iConnection.Close();
    iSocketServ.Close();

    delete iData;
    iData= NULL;
    }

void UT_CRtpCommRecv::UT_CRtpCommRecv_NewLL(  )
    {
    delete iReceiver; iReceiver=0;
    iReceiver= CRtpCommRecv::NewL( ERTPPort, iSocket, 
                                   iPriority, *this, EFalse );
    }

void UT_CRtpCommRecv::UT_CRtpCommRecv_RegisterReceivedNotifyL(  )
    {
    iReceiver->RegisterReceivedNotify(this);
    }

void UT_CRtpCommRecv::UT_CRtpCommRecv_RecvL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpCommRecv::UT_CRtpCommRecv_SetRemoteAddressL(  )
    {
    TInetAddr remoteAddr( INET_ADDR( 127, 0, 0, 1 ), 5000 );
    iReceiver->SetRemoteAddress(remoteAddr);
    EUNIT_ASSERT(iReceiver->iRemoteAddr.Address()==remoteAddr.Address());
    TInetAddr remoteAddr2( KInet6AddrNone, 5000 );
    iReceiver->SetRemoteAddress(remoteAddr2);
    }

void UT_CRtpCommRecv::UT_CRtpCommRecv_SetNonRtpObserverFlagL(  )
    {
    iReceiver->SetNonRtpObserverFlag(ETrue);
    }

void UT_CRtpCommRecv::UT_CRtpCommRecv_RunL1L(  )
    {
    iReceiver->RegisterReceivedNotify(this);
	iReceiver->iPortType =ERTPPort;
 	
 	TRequestStatus* status = &iReceiver->iStatus;
   	User::RequestComplete(status, KErrNone);
	
	EUNIT_ASSERT_NO_LEAVE( iReceiver->RunL() );
	User::WaitForAnyRequest();
    EUNIT_ASSERT( iErr==KErrNone);
    EUNIT_ASSERT(iPacketCount==1);
    
    }
    
void UT_CRtpCommRecv::UT_CRtpCommRecv_RunL2L(  )
    {
    iReceiver->RegisterReceivedNotify(this);
	iReceiver->iPortType =ERTCPPort;
 	TRequestStatus* status = &iReceiver->iStatus;
   	User::RequestComplete(status, KErrNone);
	EUNIT_ASSERT_NO_LEAVE( iReceiver->RunL() );
	User::WaitForAnyRequest();
    EUNIT_ASSERT( iErr==KErrNone);
    EUNIT_ASSERT(iPacketCount==1);
        
    }
    
void UT_CRtpCommRecv::UT_CRtpCommRecv_RunL3L(  )
    {
    iReceiver->RegisterReceivedNotify(this);
	iReceiver->iPortType =ERTCPPort;
 	TRequestStatus* status = &iReceiver->iStatus;
   	User::RequestComplete(status, KErrInUse);
	EUNIT_ASSERT_NO_LEAVE( iReceiver->RunL() );
	
    EUNIT_ASSERT( iErr==KErrInUse);
    EUNIT_ASSERT(iPacketCount==0);
    EUNIT_ASSERT(iPortType== ERTCPPort);
    iReceiver->DoCancel();
    User::WaitForAnyRequest();    
    }

void UT_CRtpCommRecv::UT_CRtpCommRecv_RunL4L(  )
    {
	iReceiver->iPortType =ERTPPort;
 	EUNIT_ASSERT_NO_LEAVE( iReceiver->RunL() );
	EUNIT_ASSERT( iErr==KErrNotReady);
    EUNIT_ASSERT(iPacketCount==0);
    }
    
void UT_CRtpCommRecv::UT_CRtpCommRecv_RunL5L(  )
    {
    iReceiver->RegisterReceivedNotify(this);
	iReceiver->iPortType =ERTPPort;
 	iReceiver->iNonRtpObserverSet = EFalse;
 	iReceiver->iRemoteAddrSet= ETrue ;
 	TInetAddr fromAddr( INET_ADDR( 10, 10, 0, 1 ), 5000 );
 	iReceiver->iFromAddr= fromAddr;
 	TRequestStatus* status = &iReceiver->iStatus;
   	User::RequestComplete(status, KErrNone);
	            
	EUNIT_ASSERT_NO_LEAVE( iReceiver->RunL() );
	User::WaitForAnyRequest();
    EUNIT_ASSERT( iPortType==ERTPPort);
    EUNIT_ASSERT(iPacketCount==0);
    }      

void UT_CRtpCommRecv::UT_CRtpCommRecv_IsSetRemoteAdressL()
	{
	EUNIT_ASSERT(!iReceiver->IsSetRemoteAdress());
	TInetAddr fromAddr( INET_ADDR( 127, 0, 0, 1 ), 5000 );
 	iReceiver->iFromAddr= fromAddr;
 	EUNIT_ASSERT(iReceiver->IsSetRemoteAdress()); 
	}
          
void UT_CRtpCommRecv::UT_CRtpCommRecv_DoCancelL(  )
    {
    iReceiver->DoCancel();
    }
void UT_CRtpCommRecv::ErrorNotify( TInt aErrCode ) 
	{
	iErr= aErrCode;
	}

void UT_CRtpCommRecv::OnRtpReceivedL() 
	{
	iPacketCount++;
	}

void UT_CRtpCommRecv::OnRtcpReceivedL() 
	{
	iPacketCount++;

	}

void UT_CRtpCommRecv::OnReceptionError( TPortType aPort, TInt aError ) 
	{
	iErr= aError;
	iPortType= aPort;
	}

void UT_CRtpCommRecv::OnPacketRejected(TPortType aPort) 
	{
	iPortType= aPort;
	}

TInt UT_CRtpCommRecv::StopScheduler( TAny* aThis )
	{
	UT_CRtpCommRecv* self = reinterpret_cast< UT_CRtpCommRecv* >( aThis );
	// set test to pass and stop scheduler
	self->iCallBackCalled = ETrue;
	CActiveScheduler::Stop();
	return 0;
	}
	
// This delay function gives opportunity for the scheduler to run other
// active objects. The use of this function also ensures that all requests
// are completed within the test function.
void UT_CRtpCommRecv::DelayL()
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
    UT_CRtpCommRecv,
    "CRtpCommRecv",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "CRtpCommRecv",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpCommRecv_NewLL, Teardown)

EUNIT_TEST(
    "RegisterReceivedNotify - test ",
    "CRtpCommRecv",
    "RegisterReceivedNotify",
    "FUNCTIONALITY",
    SetupL, UT_CRtpCommRecv_RegisterReceivedNotifyL, Teardown)

EUNIT_TEST(
    "Recv - test ",
    "CRtpCommRecv",
    "Recv",
    "FUNCTIONALITY",
    SetupL, UT_CRtpCommRecv_RecvL, Teardown)

EUNIT_TEST(
    "SetRemoteAddress - test ",
    "CRtpCommRecv",
    "SetRemoteAddress",
    "FUNCTIONALITY",
    SetupL, UT_CRtpCommRecv_SetRemoteAddressL, Teardown)

EUNIT_TEST(
    "SetNonRtpObserverFlag - test ",
    "CRtpCommRecv",
    "SetNonRtpObserverFlag",
    "FUNCTIONALITY",
    SetupL, UT_CRtpCommRecv_SetNonRtpObserverFlagL, Teardown)

EUNIT_TEST(
    "RunL1 - test ",
    "CRtpCommRecv",
    "RunL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpCommRecv_RunL1L, Teardown)
EUNIT_TEST(
    "RunL2 - test ",
    "CRtpCommRecv",
    "RunL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpCommRecv_RunL2L, Teardown)
EUNIT_TEST(
    "RunL3 - test ",
    "CRtpCommRecv",
    "RunL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpCommRecv_RunL3L, Teardown)        
EUNIT_TEST(
    "RunL4 - test ",
    "CRtpCommRecv",
    "RunL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpCommRecv_RunL4L, Teardown)        
EUNIT_TEST(
    "RunL5- test ",
    "CRtpCommRecv",
    "RunL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpCommRecv_RunL5L, Teardown)        

EUNIT_TEST(
    "DoCancel - test ",
    "CRtpCommRecv",
    "DoCancel",
    "FUNCTIONALITY",
    SetupL, UT_CRtpCommRecv_DoCancelL, Teardown)
EUNIT_TEST(
    "IsSetRemoteAdressL",
    "CRtpCommRecv",
    "IsSetRemoteAdressL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpCommRecv_IsSetRemoteAdressL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE
