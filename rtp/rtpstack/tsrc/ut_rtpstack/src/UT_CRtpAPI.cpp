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
#include "UT_CRtpAPI.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>

//#include "SRTPCryptoHandler.h"
#include <srtpmastersalt.h>
#include <srtpmasterkey.h>
#include "srtpcryptocontext.h"
#include "msrtprekeyingobserver.h"
#include <srtpcryptoparams.h>

#include <rtpapi.h>
#include <srtpstreamin.h>
#include "rtpmanager.h"
#include "srtpsession.h"

//  INTERNAL INCLUDES


const TUint KDefaultIap = 6;
const TUint KDefaultIap2 = 1;
const TInt KLength = 20;
_LIT8(KRFC3711_TestMasterKey128bits,    "E1F97A0D3E018BE0D64FA32C06DE4139");
_LIT8(KRFC3711_TestMasterSalt112bits,   "0EC675AD498AFEEBB6960B3AABE6");	

_LIT8(KTestMKI128Bits,      "ABCDEF1234567890ABCDEF1234567890");

#define RTP_EUNIT_ASSERT_EQUALS( val1, val2 ) \
if ( val2 != KErrNoMemory && val1 == KErrNoMemory ){\
	User::Leave( KErrNoMemory );\
	}\
else{\
	EUNIT_ASSERT_EQUALS( val1, val2 );\
	}
 
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

_LIT8(KHello, "hello...testing");
_LIT8(KNoCName, "");

// CONSTRUCTION
UT_CRtpAPI* UT_CRtpAPI::NewL()
    {
    UT_CRtpAPI* self = UT_CRtpAPI::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CRtpAPI* UT_CRtpAPI::NewLC()
    {
    UT_CRtpAPI* self = new( ELeave ) UT_CRtpAPI();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CRtpAPI::~UT_CRtpAPI()
    {
    }

// Default constructor
UT_CRtpAPI::UT_CRtpAPI()
    {
    }

// Second phase construct
void UT_CRtpAPI::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

void UT_CRtpAPI::Hex(HBufC8& aString)
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

void UT_CRtpAPI::SetupL(  )
    {
	iRtpAPI = CRtpAPI::NewL( *this );
    iStpAPI = CRtpAPI::NewL( *this );
	iErr=0;
    }

void UT_CRtpAPI::Teardown(  )
    {
    iRtpAPI->Close();
    delete iRtpAPI;
	iRtpAPI = NULL;

    iStpAPI->Close();
    delete iStpAPI;
    iStpAPI = NULL;
	}

void UT_CRtpAPI::UT_CRtpAPI_NewLL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpAPI::UT_CRtpAPI_OpenLL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    EUNIT_ASSERT(result==KErrNone);
    
    /*if ( iRtpAPI->iManager == NULL )
        {
        EUNIT_ASSERT(iRtpAPI->iManager);
        }*/
    }

void UT_CRtpAPI::UT_CRtpAPI_StartConnectionL(  )
    {
    //Asychronize
	TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    EUNIT_ASSERT(result==KErrNone);

    TInt error(KErrNone);
    TRequestStatus status;
    error = iRtpAPI->StartConnection( status, KDefaultIap ); // KDefaultIap
    EUNIT_ASSERT ( error == KErrNone );

    // Cancel the start immediately
    iRtpAPI->CancelStart();
    User::WaitForRequest( status );

    // Start connection again
    error = iRtpAPI->StartConnection( status, KDefaultIap ); // KDefaultIap
    EUNIT_ASSERT ( error == KErrNone );

    // Wait until the connection is really up
    User::WaitForRequest( status );

    // Close the connection =)
    //iRtpAPI->Close();
	iRtpAPI->CancelStart();
    }

void UT_CRtpAPI::UT_CRtpAPI_StartConnection_OneL(  )
    {
    //Synchronize
    TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    EUNIT_ASSERT(result==KErrNone);
    TInt error( KErrNone );
	error = iRtpAPI->StartConnection( KDefaultIap2 ); // 0
    EUNIT_ASSERT( error == KErrNone );
    iRtpAPI->Close();
    }

void UT_CRtpAPI::UT_CRtpAPI_CancelStartL(  )
    {
    EUNIT_ASSERT( ETrue);
    }

void UT_CRtpAPI::UT_CRtpAPI_CloseL(  )
    {
    iRtpAPI->Close();
	iStpAPI->Close();
    
    EUNIT_ASSERT( iRtpAPI->iManager->iConnected == EFalse);
    }

void UT_CRtpAPI::UT_CRtpAPI_SetLocalSdesL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    EUNIT_ASSERT(result==KErrNone);
    
    iRtpAPI->SetLocalSdes( params );

    EUNIT_ASSERT( ETrue);
    }

void UT_CRtpAPI::UT_CRtpAPI_GetLocalIPAddressLL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    EUNIT_ASSERT(result==KErrNone);
    
    TInt error( KErrNone );
    TInetAddr addr;

	TRAP( error, addr = iRtpAPI->GetLocalIPAddressL() );
	//"Should not have found local IP when not started"
    EUNIT_ASSERT ( error == KErrNotSupported );
    
    //"Test description: Test getting local IP address" ) );
    // _L( "CRtpAPI::GetLocalIPAddressL()" ) );
    //Test when IAP ID<0
	TRequestStatus status;
    result = iRtpAPI->StartConnection( status, -6 ); // KDefaultIap
  	EUNIT_ASSERT ( result == KErrNone );
 // Wait until the connection is really up
    User::WaitForRequest( status );
  	TRAP( error, addr = iRtpAPI->GetLocalIPAddressL() )
    
    //Then test when IAP ID>0
    
    iRtpAPI->CancelStart();
    
    //result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    //EUNIT_ASSERT(result==KErrNone);
    
	
    result = iRtpAPI->StartConnection( status, KDefaultIap ); // KDefaultIap
    EUNIT_ASSERT ( result == KErrNone );
 	// Wait until the connection is really up
    User::WaitForRequest( status );

  	TRAP( error, addr = iRtpAPI->GetLocalIPAddressL() )
    if ( error != KErrNone )
        {
        EUNIT_ASSERT( EFalse);
        }

    TInetAddr compareAddr( KInetAddrAny );
    if ( addr == compareAddr )
        {
        EUNIT_ASSERT( EFalse);
        }
    }

void UT_CRtpAPI::UT_CRtpAPI_CreateSessionLL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    EUNIT_ASSERT(result==KErrNone);
    
    TRequestStatus status;
    result = iRtpAPI->StartConnection( status, KDefaultIap ); // KDefaultIap
    RTP_EUNIT_ASSERT_EQUALS( result, KErrNone );
 // Wait until the connection is really up
    User::WaitForRequest( status );



    TCreateSessionParams sessionParams;
    sessionParams.iPriority = TCreateSessionParams::EPriorityStandard;
    sessionParams.iSocketBufSize = KSocketBufSize;
    TUint port( 5000 );
    TBool enableRtcp( EFalse );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iRtcpTimeOut = 0;
    rtcpParams.iSessionBWidth = 32000;

    TRtpId error( KNullId );
    error = iRtpAPI->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    RTP_EUNIT_ASSERT_RTP_ID( error );
     
    }
    
void UT_CRtpAPI::UT_CRtpAPI_CreateSessionL2L(  )
    {
    
    //Create SRTP Session And Stream
        HBufC8* masterKey = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = KRFC3711_TestMasterKey128bits;
	
    
    
    HBufC8* masterSalt =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFC3711_TestMasterSalt112bits;
	
	
	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	
	CSRTPMasterKey* mk = CSRTPMasterKey::NewL( *masterKey, *mki );
	CleanupStack::PushL( mk );
    CSRTPMasterSalt* ms = CSRTPMasterSalt::NewL( *masterSalt );
    CleanupStack::PushL( ms );
    
    TSrtpCryptoParams cryptoparams;
    CSRTPCryptoContext* context = CSRTPCryptoContext::NewL(mk, ms, cryptoparams );
    CleanupStack::Pop( ms );
    CleanupStack::Pop( mk );
    CleanupStack::PopAndDestroy( mki );
    CleanupStack::PopAndDestroy( masterSalt );
    CleanupStack::PopAndDestroy( masterKey ); 
    CleanupStack::PushL( context );
    CSRTPSession* srtpSession = CSRTPSession::NewL(iDestination,
    												context,
    												*this); 
    CleanupStack::Pop( context );
    CleanupStack::PushL( srtpSession );
    
    //automatically added stream in the constructor												
    CSRTPStreamIn* tempStreamIn = CSRTPStreamIn::NewL(*srtpSession, (TUint)1);
    CleanupStack::PushL( tempStreamIn );

    //End of Create SRTPSession

    //Create RTPSession
    TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    EUNIT_ASSERT(result==KErrNone);
    
    TRequestStatus status;
    result = iRtpAPI->StartConnection( status, KDefaultIap ); // KDefaultIap
    EUNIT_ASSERT ( result == KErrNone );
 // Wait until the connection is really up
    User::WaitForRequest( status );



    TCreateSessionParams sessionParams;
    sessionParams.iPriority = TCreateSessionParams::EPriorityStandard;
    sessionParams.iSocketBufSize = KSocketBufSize;
    TUint port( 5000 );
    TBool enableRtcp( EFalse );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iRtcpTimeOut = 0;
    rtcpParams.iSessionBWidth = 32000;

    TRtpId error( KNullId );
    error = iRtpAPI->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams,
                                    *srtpSession );
    RTP_EUNIT_ASSERT_RTP_ID( error );
    //Delete SRTPSession
    
    CleanupStack::PopAndDestroy( tempStreamIn );
    CleanupStack::PopAndDestroy( srtpSession );
    
       
    }    

void UT_CRtpAPI::UT_CRtpAPI_StartSessionL(  )
    {
   	TInt invalidSession( 42 );
    TInt result( KErrNone );

    result = iRtpAPI->StartSession( invalidSession );
    EUNIT_ASSERT ( result == KErrNotFound )
    // Can't verify these but they shouldn't do anything
    iRtpAPI->CloseSession( invalidSession );
    iRtpAPI->UnregisterRtpObserver( invalidSession );
    iRtpAPI->UnregisterRtcpObserver( invalidSession );
    iRtpAPI->CancelSend( invalidSession );
    }

void UT_CRtpAPI::UT_CRtpAPI_CloseSessionL(  )
    {
    EUNIT_ASSERT( ETrue );//See above test case
    }

void UT_CRtpAPI::UT_CRtpAPI_TestStartSession2L(  )
    {
    
    //Create SRTP Session And Stream
        HBufC8* masterKey = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = KRFC3711_TestMasterKey128bits;
	
    
    
    HBufC8* masterSalt =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFC3711_TestMasterSalt112bits;
	
	
	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	
	CSRTPMasterKey* mk = CSRTPMasterKey::NewL( *masterKey, *mki );
	CleanupStack::PushL( mk );
    CSRTPMasterSalt* ms = CSRTPMasterSalt::NewL( *masterSalt );
    CleanupStack::PushL( ms );
    
    TSrtpCryptoParams cryptoparams;
    CSRTPCryptoContext* context = CSRTPCryptoContext::NewL(mk, ms, cryptoparams );
    CleanupStack::Pop( ms );
    CleanupStack::Pop( mk );
    CleanupStack::PopAndDestroy( mki );
    CleanupStack::PopAndDestroy( masterSalt );
    CleanupStack::PopAndDestroy( masterKey ); 
    CleanupStack::PushL( context );
    CSRTPSession* srtpSession = CSRTPSession::NewL(iDestination,
    												context,
    												*this); 
    CleanupStack::Pop( context );
    CleanupStack::PushL( srtpSession );
    //automatically added stream in the constructor												
    CSRTPStreamIn* tempStreamIn = CSRTPStreamIn::NewL(*srtpSession, (TUint)1);
    CleanupStack::PushL( tempStreamIn );

    //End of Create SRTPSession

    //Create RTPSession
    TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    EUNIT_ASSERT(result==KErrNone);
    
    TRequestStatus status;
    result = iRtpAPI->StartConnection( status, KDefaultIap ); // KDefaultIap
    RTP_EUNIT_ASSERT_EQUALS ( result, KErrNone );
 // Wait until the connection is really up
    User::WaitForRequest( status );



    TCreateSessionParams sessionParams;
    sessionParams.iPriority = TCreateSessionParams::EPriorityStandard;
    sessionParams.iSocketBufSize = KSocketBufSize;
    TUint port( 5000 );
    TBool enableRtcp( EFalse );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iRtcpTimeOut = 0;
    rtcpParams.iSessionBWidth = 32000;

    TRtpId sessionId( KNullId );
    TRtpId error( KNullId );
    sessionId = iRtpAPI->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams,
                                    *srtpSession );
    RTP_EUNIT_ASSERT_RTP_ID( sessionId );
    TRcvStreamParams recvparams;
    recvparams.iPayloadType = 0;
    
    TRtpId streamId( 1 );
    TRtpSSRC ssrc( 0 );
    TInt err( KErrNone );
    err = iRtpAPI->CreateReceiveStreamL( sessionId /*session id*/, recvparams );
    result = iRtpAPI->StartSession( sessionId );//error is sessionID
    
    CRtpSession* session = iRtpAPI->iManager->GetSession( sessionId);
    session->OnRtpReceivedL();
    
    iRtpAPI->CloseSession( error );
    iRtpAPI->UnregisterRtpObserver( error );
    iRtpAPI->UnregisterRtcpObserver( error);
    iRtpAPI->CancelSend( error );
    //Delete SRTPSession
    
    CleanupStack::PopAndDestroy( tempStreamIn );
    CleanupStack::PopAndDestroy( srtpSession );
    
       
    }    
void UT_CRtpAPI::UT_CRtpAPI_SetRemoteAddressL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    EUNIT_ASSERT(result==KErrNone);
    
    TRequestStatus status;
    result = iRtpAPI->StartConnection( status, KDefaultIap ); // KDefaultIap
    RTP_EUNIT_ASSERT_EQUALS ( result, KErrNone );
 // Wait until the connection is really up
    User::WaitForRequest( status );

	TCreateSessionParams sessionParams;
    sessionParams.iPriority = TCreateSessionParams::EPriorityStandard;
    sessionParams.iSocketBufSize = KSocketBufSize;
    TUint port( 5000 );
    TBool enableRtcp( EFalse );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iRtcpTimeOut = 0;
    rtcpParams.iSessionBWidth = 32000;

    TRtpId error( KNullId );
    error = iRtpAPI->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    RTP_EUNIT_ASSERT_RTP_ID( error );
 
   	TInetAddr addr;
    TUint32 ipAddr = INET_ADDR( 127, 0, 0, 1 );
    addr.SetAddress( ipAddr );
    addr.SetPort( 5000 ); 
    TInt err( KErrNone );
    
    err = iRtpAPI->SetRemoteAddress( 1/* session id*/, addr );
    EUNIT_ASSERT(err== KErrNone );
    }

void UT_CRtpAPI::UT_CRtpAPI_SetRemoteRtcpAddressL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    RTP_EUNIT_ASSERT_EQUALS(result, KErrNone);
    
    TRequestStatus status;
    result = iRtpAPI->StartConnection( status, KDefaultIap ); // KDefaultIap
    RTP_EUNIT_ASSERT_EQUALS ( result, KErrNone );
 // Wait until the connection is really up
    User::WaitForRequest( status );

	TCreateSessionParams sessionParams;
    sessionParams.iPriority = TCreateSessionParams::EPriorityStandard;
    sessionParams.iSocketBufSize = KSocketBufSize;
    TUint port( 5000 );
    TBool enableRtcp( EFalse );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iRtcpTimeOut = 0;
    rtcpParams.iSessionBWidth = 32000;

    TRtpId error( KNullId );
    error = iRtpAPI->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    RTP_EUNIT_ASSERT_RTP_ID( error );
 
    TInetAddr addr;
    TUint32 ipAddr = INET_ADDR( 127, 0, 0, 1 );
    addr.SetAddress( ipAddr );
    addr.SetPort( 5011 );
    TInt err( KErrNone );
    
    err = iRtpAPI->SetRemoteRtcpAddress( 1/* session id*/, addr );
    //RTCP is not enabled
    RTP_EUNIT_ASSERT_EQUALS( err, KErrNotSupported );
    
    //RTCP is enabled
    TBool renableRtcp( ETrue );
    TUint port1( 5050 );
    
    error = iRtpAPI->CreateSessionL( sessionParams,
                                    port1,
                                    renableRtcp,
                                    &rtcpParams );
    RTP_EUNIT_ASSERT_RTP_ID( error );
    
    err = iRtpAPI->SetRemoteRtcpAddress(error/* session id*/, addr );
    RTP_EUNIT_ASSERT_EQUALS( err, KErrNone );
    
    err = iRtpAPI->SetRemoteRtcpAddress( 4/* session id*/, addr );
    RTP_EUNIT_ASSERT_EQUALS( err, KErrNotFound );
    }

void UT_CRtpAPI::UT_CRtpAPI_CreateReceiveStreamLL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    EUNIT_ASSERT(result==KErrNone);
    
    TRequestStatus status;
    result = iRtpAPI->StartConnection( status, KDefaultIap ); // KDefaultIap
    RTP_EUNIT_ASSERT_EQUALS ( result, KErrNone );
 // Wait until the connection is really up
    User::WaitForRequest( status );

	//create session
	TCreateSessionParams sessionParams;
    sessionParams.iPriority = TCreateSessionParams::EPriorityStandard;
    sessionParams.iSocketBufSize = KSocketBufSize;
    TUint port( 5000 );
    TBool enableRtcp( EFalse );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iRtcpTimeOut = 0;
    rtcpParams.iSessionBWidth = 32000;

    TRtpId error( KNullId );
    error = iRtpAPI->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    RTP_EUNIT_ASSERT_RTP_ID( error );
    
    
    TRcvStreamParams recvparams;
    recvparams.iPayloadType = 0;

    TInt err( KErrNone );
    err = iRtpAPI->CreateReceiveStreamL( 1 /*session id*/, recvparams );
    RTP_EUNIT_ASSERT_RTP_ID ( err );
    
    }

void UT_CRtpAPI::UT_CRtpAPI_CreateTransmitStreamLL(  )
    {
       TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    EUNIT_ASSERT(result==KErrNone);
    
    TRequestStatus status;
    result = iRtpAPI->StartConnection( status, KDefaultIap ); // KDefaultIap
    RTP_EUNIT_ASSERT_EQUALS ( result, KErrNone );
 // Wait until the connection is really up
    User::WaitForRequest( status );

		//create session
	TCreateSessionParams sessionParams;
    sessionParams.iPriority = TCreateSessionParams::EPriorityStandard;
    sessionParams.iSocketBufSize = KSocketBufSize;
    TUint port( 5000 );
    TBool enableRtcp( EFalse );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iRtcpTimeOut = 0;
    rtcpParams.iSessionBWidth = 32000;

    TRtpId error( KNullId );
    error = iRtpAPI->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    RTP_EUNIT_ASSERT_RTP_ID( error );
    
    TTranStreamParams transparams;
    transparams.iPayloadType = 0;
    TRtpSSRC ssrc( 0 );

    TInt err( KErrNone );
    err = iRtpAPI->CreateTransmitStreamL( error /*session id*/, transparams, ssrc );
    RTP_EUNIT_ASSERT_RTP_ID ( error );
    
    }

void UT_CRtpAPI::UT_CRtpAPI_CreateTransmitStreamExtLL(  )
    {
	TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    EUNIT_ASSERT(result==KErrNone);
    
    TRequestStatus status;
    result = iRtpAPI->StartConnection( status, KDefaultIap ); // KDefaultIap
    RTP_EUNIT_ASSERT_EQUALS ( result, KErrNone );
 // Wait until the connection is really up
    User::WaitForRequest( status );

	//create session
	TCreateSessionParams sessionParams;
    sessionParams.iPriority = TCreateSessionParams::EPriorityStandard;
    sessionParams.iSocketBufSize = KSocketBufSize;
    TUint port( 5000 );
    TBool enableRtcp( EFalse );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iRtcpTimeOut = 0;
    rtcpParams.iSessionBWidth = 32000;

    TRtpId error( KNullId );
    error = iRtpAPI->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    RTP_EUNIT_ASSERT_RTP_ID( error );
    
    TTranStreamParams recvparams;
    recvparams.iPayloadType = 0;
    TRtpSSRC ssrc( 0 );

    TInt err( KErrNone );
    err = iStpAPI->CreateTransmitStreamExtL( error /*session id*/, recvparams, ssrc );
    RTP_EUNIT_ASSERT_RTP_ID ( error );
	}

void UT_CRtpAPI::UT_CRtpAPI_CloseStreamL(  )
    {
    // "Test description: Test giving an invalid stream number to" ) );
    // "                  methods that take a stream as parameter." ) );
    // "CRtpUtRtpAPI::TestMethodsWithoutStreams()" ) );

    TInt invalidStream( 42 );
    TInt result( KErrNone );

    // Can't verify this but it shouldn't do anything
    iRtpAPI->CloseStream( invalidStream );

    const TRtpSendHeader rtpHeader;
    TBufC8<5> dummy;
    result = iRtpAPI->SendRtpPacket( invalidStream, rtpHeader, dummy );
    if ( result != KErrNotFound )
        {
        // "Should not have found invalid stream, %d" ), result );
        // "CRtpAPI::SendRtpPacket() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    TRequestStatus status;
    result = iRtpAPI->SendRtpPacket( invalidStream, rtpHeader, dummy, status );
    if ( result != KErrNotFound )
        {
        // "Should not have found invalid stream (async)" ) );
        // "CRtpAPI::SendRtpPacket() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    result = iRtpAPI->SendRtpPacket( invalidStream, 0, rtpHeader, dummy, status );
    if ( result != KErrNotFound )
        {
        // "Should not have found invalid stream (async w/ seqnum)" ) );
        // "CRtpAPI::SendRtpPacket() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    result = iRtpAPI->SendRtcpByePacket( invalidStream, dummy );
    if ( result != KErrNotFound )
        {
        // "Should not have found invalid stream %d" ), result );
        // "CRtpAPI::SendRtcpByePacket() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    TRtcpApp app;
    result = iRtpAPI->SendRtcpAppPacket( invalidStream, app );
    if ( result != KErrNotFound )
        {
        // "Should not have found invalid stream" ) );
        // "CRtpAPI::SendRtcpAppPacket() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    result = iRtpAPI->SendRtcpSrPacket( invalidStream );
    if ( result != KErrNotFound )
        {
        // "Should not have found invalid stream" ) );
        // "CRtpAPI::SendRtcpSrPacket() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    result = iRtpAPI->SendRtcpRrPacket( invalidStream );
    if ( result != KErrNotFound )
        {
        // "Should not have found invalid stream" ) );
        // "CRtpAPI::SendRtcpRrPacket() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    result = iRtpAPI->GetSessionId( invalidStream );
    if ( result != KNullId )
        {
        // "Should not have found invalid stream" ) );
        // "CRtpAPI::GetSessionId() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    TRtpPeerStat stat;
    result = iRtpAPI->GetStreamStatistics( invalidStream, stat );
    if ( result != KErrGeneral )
        {
        // "Should not have found invalid stream" ) );
        // "CRtpAPI::GetStreamStatistics() - FAILED" ) );
        EUNIT_ASSERT(EFalse);
        }

    // "CRtpUtRtpAPI::TestMethodsWithoutStreams() - passed" ) );
    
    }

void UT_CRtpAPI::UT_CRtpAPI_RegisterRtpObserverL(  )
    {
      TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    EUNIT_ASSERT(result==KErrNone);
    
    TRequestStatus status;
    result = iRtpAPI->StartConnection( status, KDefaultIap ); // KDefaultIap
    RTP_EUNIT_ASSERT_EQUALS ( result, KErrNone );
 // Wait until the connection is really up
    User::WaitForRequest( status );

	//create session
	TCreateSessionParams sessionParams;
    sessionParams.iPriority = TCreateSessionParams::EPriorityStandard;
    sessionParams.iSocketBufSize = KSocketBufSize;
    TUint port( 5000 );
    TBool enableRtcp( EFalse );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iRtcpTimeOut = 0;
    rtcpParams.iSessionBWidth = 32000;

    TRtpId error( KNullId );
    error = iRtpAPI->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    RTP_EUNIT_ASSERT_RTP_ID( error );
    
    TRcvStreamParams recvparams;
    recvparams.iPayloadType = 0;

	TRtpId error2( KNullId );
    TInt err( KErrNone );
    error2 = iRtpAPI->CreateReceiveStreamL( error /*session id*/, recvparams );
    RTP_EUNIT_ASSERT_RTP_ID ( error2 );
        
    
    err = iRtpAPI->RegisterRtpObserver( error, *this );
    EUNIT_ASSERT ( err == KErrNone )
    }

void UT_CRtpAPI::UT_CRtpAPI_UnregisterRtpObserverL(  )
    {
    //tested in startsession
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpAPI::UT_CRtpAPI_SetNonRTPDataObserverL(  )
    {
    TInt error( KErrNone );
    error = iRtpAPI->SetNonRTPDataObserver( 1, this );
    EUNIT_ASSERT ( error == KErrNone )
    }

void UT_CRtpAPI::UT_CRtpAPI_SendRtpPacketL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    RTP_EUNIT_ASSERT_EQUALS(result, KErrNone);
    
    TRequestStatus status;
    result = iRtpAPI->StartConnection( status, KDefaultIap ); // KDefaultIap
    RTP_EUNIT_ASSERT_EQUALS ( result, KErrNone );
 // Wait until the connection is really up
    User::WaitForRequest( status );

		//create session
	TCreateSessionParams sessionParams;
    sessionParams.iPriority = TCreateSessionParams::EPriorityStandard;
    sessionParams.iSocketBufSize = KSocketBufSize;
    TUint port( 5000 );
    TBool enableRtcp( EFalse );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iRtcpTimeOut = 0;
    rtcpParams.iSessionBWidth = 32000;

    TRtpId error( KNullId );
    error = iRtpAPI->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    RTP_EUNIT_ASSERT_RTP_ID( error );
    
    TTranStreamParams transparams;
    transparams.iPayloadType = 0;
    TRtpSSRC ssrc( 0 );

    TInt err( KErrNone );
    err = iRtpAPI->CreateTransmitStreamL( error /*session id*/, transparams, ssrc );
    RTP_EUNIT_ASSERT_RTP_ID ( err );
    
    
    TRtpSendHeader header; 
    header.iHeaderExtension = NULL;
    header.iMarker = 0;
    header.iPadding = 0;
    header.iPayloadType = 0;
    header.iTimestamp = 10;
    TBuf8<5> payload;
    payload.Format( _L8( "" ) );

    TInt error2( KErrNone );
    
    // Try sending synchronously
    error2 = iRtpAPI->SendRtpPacket( err /*Tx stream*/, header, payload );
    RTP_EUNIT_ASSERT_EQUALS ( error2, KErrNone );
    CRtpSession* rtpSession = iRtpAPI->iManager->GetSession( err );
    User::WaitForRequest( rtpSession->iCommNet->iSender[ERTPPort]->iStatus );
    rtpSession->iCommNet->iSender[ERTPPort]->RunL();
    rtpSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
        
    // Try sending asynchronously
    error2 = iRtpAPI->SendRtpPacket( err /*Tx stream*/, header, payload, status );
    RTP_EUNIT_ASSERT_EQUALS ( error2, KErrNone );
    EUNIT_ASSERT( KRequestPending == status.Int() );
    User::WaitForRequest( rtpSession->iCommNet->iSender[ERTPPort]->iStatus );
    rtpSession->iCommNet->iSender[ERTPPort]->RunL();
    rtpSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
    User::WaitForRequest( status );
    EUNIT_ASSERT( KErrNone == status.Int() );
    
    // Try sending asynchronously, specifying the sequence number
    error2 = iRtpAPI->SendRtpPacket( err /*Tx stream*/, 42 /*seqnum*/, header, payload, status );
    RTP_EUNIT_ASSERT_EQUALS ( error2, KErrNone );
    EUNIT_ASSERT( KRequestPending == status.Int() );
    User::WaitForRequest( rtpSession->iCommNet->iSender[ERTPPort]->iStatus );
    rtpSession->iCommNet->iSender[ERTPPort]->RunL();
    rtpSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
    User::WaitForRequest( status );
    EUNIT_ASSERT( KErrNone == status.Int() );
	}

void UT_CRtpAPI::UT_CRtpAPI_SendRtpPacket_ThreeL()
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
     
    
    TPtrC16 fileName( _L16( "rtpstppacket.dll" ) );
    
    result = iRtpAPI->OpenL( params, &fileName, NULL, NULL );
    EUNIT_ASSERT(result==KErrNone);
    
    TRequestStatus status;
    result = iRtpAPI->StartConnection( status, KDefaultIap ); // KDefaultIap
    RTP_EUNIT_ASSERT_EQUALS ( result, KErrNone );
 // Wait until the connection is really up
    User::WaitForRequest( status );

		//create session
	TCreateSessionParams sessionParams;
    sessionParams.iPriority = TCreateSessionParams::EPriorityStandard;
    sessionParams.iSocketBufSize = KSocketBufSize;
    TUint port( 5000 );
    TBool enableRtcp( EFalse );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iRtcpTimeOut = 0;
    rtcpParams.iSessionBWidth = 32000;

    TRtpId error( KNullId );
    error = iRtpAPI->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    RTP_EUNIT_ASSERT_RTP_ID( error );
    
    TTranStreamParams transparams;
    transparams.iPayloadType = 0;
    TRtpSSRC ssrc( 0 );

    TInt err( KErrNone );
    err = iRtpAPI->CreateTransmitStreamL( error /*session id*/, transparams, ssrc );
    RTP_EUNIT_ASSERT_RTP_ID ( err );
    
    
    TRtpSendHeader header; 
    header.iHeaderExtension = NULL;
    header.iMarker = 0;
    header.iPadding = 0;
    header.iPayloadType = 0;
    header.iTimestamp = 10;
    TBuf8<5> payload;
    payload.Format( _L8( "" ) );

    TInt error2( KErrNone );

    // Try sending synchronously
    error2 = iRtpAPI->SendRtpPacket( err /*Tx stream*/, header, payload );
    RTP_EUNIT_ASSERT_EQUALS ( error2, KErrNone );
    CRtpSession* rtpSession = iRtpAPI->iManager->GetSession( err );
    User::WaitForRequest( rtpSession->iCommNet->iSender[ERTPPort]->iStatus );
    rtpSession->iCommNet->iSender[ERTPPort]->RunL();
    rtpSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();

    // Try sending asynchronously
    error2 = iRtpAPI->SendRtpPacket( err /*Tx stream*/, header, payload, status );
    RTP_EUNIT_ASSERT_EQUALS ( error2, KErrNone );
    EUNIT_ASSERT( KRequestPending == status.Int() );
    User::WaitForRequest( rtpSession->iCommNet->iSender[ERTPPort]->iStatus );
    rtpSession->iCommNet->iSender[ERTPPort]->RunL();
    rtpSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
    User::WaitForRequest( status );
    EUNIT_ASSERT( KErrNone == status.Int() );

    // Try sending asynchronously, specifying the sequence number
    error2 = iRtpAPI->SendRtpPacket( err /*Tx stream*/, 42 /*seqnum*/, header, payload, status );
    RTP_EUNIT_ASSERT_EQUALS ( error2, KErrNotSupported );
    EUNIT_ASSERT( KErrNone == status.Int() );
    }

void UT_CRtpAPI::UT_CRtpAPI_SendRtpPacket_1L(  )
    {
    EUNIT_ASSERT( ETrue );
    //Tested SendRtpPacketL(  )
    }

void UT_CRtpAPI::UT_CRtpAPI_SendRtpPacket_2L(  )
    {
    EUNIT_ASSERT( ETrue );
    //Tested SendRtpPacketL(  )
    }

void UT_CRtpAPI::UT_CRtpAPI_SendDataLL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    EUNIT_ASSERT(result==KErrNone);
    
    TRequestStatus status;
    result = iRtpAPI->StartConnection( status, KDefaultIap ); // KDefaultIap
    RTP_EUNIT_ASSERT_EQUALS ( result, KErrNone );
 // Wait until the connection is really up
    User::WaitForRequest( status );

		//create session
	TCreateSessionParams sessionParams;
    sessionParams.iPriority = TCreateSessionParams::EPriorityStandard;
    sessionParams.iSocketBufSize = KSocketBufSize;
    TUint port( 5000 );
    TBool enableRtcp( EFalse );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iRtcpTimeOut = 0;
    rtcpParams.iSessionBWidth = 32000;

    TRtpId error( KNullId );
    error = iRtpAPI->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    RTP_EUNIT_ASSERT_RTP_ID( error );
    
    TTranStreamParams transparams;
    transparams.iPayloadType = 0;
    TRtpSSRC ssrc( 0 );

    TInt err( KErrNone );
    err = iRtpAPI->CreateTransmitStreamL( error /*session id*/, transparams, ssrc );
    RTP_EUNIT_ASSERT_RTP_ID ( err );
    
    // Try sending asynchronously
    HBufC8* data = HBufC8::NewLC(KLength);
	data->Des().Copy(KHello);
    status=KErrNone;
  	iRtpAPI->SendDataL( err /*Tx stream*/, ETrue, *data, status );
    
    CRtpSession* rtpSession = iRtpAPI->iManager->GetSession( err );
    User::WaitForRequest( rtpSession->iCommNet->iSender[ERTPPort]->iStatus, status );
    RTP_EUNIT_ASSERT_EQUALS( status.Int(), KRequestPending );
    rtpSession->iCommNet->iSender[ERTPPort]->RunL();
    rtpSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
    User::WaitForRequest( status );
    RTP_EUNIT_ASSERT_EQUALS( status.Int(), KErrNone );
    CleanupStack::PopAndDestroy( data );
    }

void UT_CRtpAPI::UT_CRtpAPI_CancelSendL(  )
    {
    EUNIT_ASSERT( ETrue );
    //Tested in StartSession
    }

void UT_CRtpAPI::UT_CRtpAPI_RegisterRtcpObserverL(  )
    {
    TInt invalidSession( 42 );
    TInt result( KErrNone );

    result = iRtpAPI->RegisterRtcpObserver( invalidSession, *this );
    EUNIT_ASSERT( result == KErrGeneral );
    }

void UT_CRtpAPI::UT_CRtpAPI_UnregisterRtcpObserverL(  )
    {
    EUNIT_ASSERT( ETrue );
    //Tested in StartSession
    }

void UT_CRtpAPI::UT_CRtpAPI_SendRtcpByePacketL(  )
    {
    TInt invalidStream( 42 );
    TInt result( KErrNone );
	TBufC8<5> dummy;
    result = iRtpAPI->SendRtcpByePacket( invalidStream, dummy );
    EUNIT_ASSERT ( result == KErrNotFound );
    }

void UT_CRtpAPI::UT_CRtpAPI_SendRtcpAppPacketL(  )
    {
    TInt invalidStream( 42 );
    TInt result( KErrNone );
    TRtcpApp app;
    result = iRtpAPI->SendRtcpAppPacket( invalidStream, app );
    EUNIT_ASSERT ( result == KErrNotFound );
    }

void UT_CRtpAPI::UT_CRtpAPI_SendRtcpRrPacketL(  )
    {
    TInt invalidStream( 42 );
    TInt result( KErrNone );
    result = iRtpAPI->SendRtcpRrPacket( invalidStream );
    EUNIT_ASSERT ( result == KErrNotFound );
    }

void UT_CRtpAPI::UT_CRtpAPI_SendRtcpSrPacketL(  )
    {
    TInt invalidStream( 42 );
    TInt result( KErrNone );
    result = iRtpAPI->SendRtcpSrPacket( invalidStream );
    EUNIT_ASSERT ( result == KErrNotFound );
    }

void UT_CRtpAPI::UT_CRtpAPI_SuspendRtcpSendingL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    EUNIT_ASSERT(result==KErrNone);
    
    TRequestStatus status;
    result = iRtpAPI->StartConnection( status, KDefaultIap ); // KDefaultIap
    RTP_EUNIT_ASSERT_EQUALS ( result, KErrNone );
 // Wait until the connection is really up
    User::WaitForRequest( status );

	//create session
	TCreateSessionParams sessionParams;
    sessionParams.iPriority = TCreateSessionParams::EPriorityStandard;
    sessionParams.iSocketBufSize = KSocketBufSize;
    TUint port( 5000 );
    TBool enableRtcp( ETrue );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iRtcpTimeOut = 0;
    rtcpParams.iSessionBWidth = 32000;

    TRtpId error( KNullId );
    error = iRtpAPI->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    RTP_EUNIT_ASSERT_RTP_ID( error );
    
    // This will disable automatic RTCP sending
    TInt err( KErrNone );
    TBool autoSending( EFalse );
    err = iRtpAPI->SuspendRtcpSending( error, EFalse );
    RTP_EUNIT_ASSERT_EQUALS ( err, KErrNone );
   
    err = iRtpAPI->IsRtcpSendingSuspended( error, autoSending );
    RTP_EUNIT_ASSERT_EQUALS ( err, KErrNone );
   
    EUNIT_ASSERT ( autoSending==EFalse )
        
    // This will re-enable automatic RTCP sending
    err = iRtpAPI->SuspendRtcpSending( error, ETrue );
    RTP_EUNIT_ASSERT_EQUALS ( err, KErrNone );
    
    err = iRtpAPI->IsRtcpSendingSuspended( error, autoSending );
    RTP_EUNIT_ASSERT_EQUALS ( err, KErrNone );
    
    EUNIT_ASSERT ( autoSending )
    }

void UT_CRtpAPI::UT_CRtpAPI_IsRtcpSendingSuspendedL(  )
    {
   	TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    EUNIT_ASSERT(result==KErrNone);
    
    TRequestStatus status;
    result = iRtpAPI->StartConnection( status, KDefaultIap ); // KDefaultIap
    RTP_EUNIT_ASSERT_EQUALS ( result, KErrNone );
 // Wait until the connection is really up
    User::WaitForRequest( status );

    // Create a new session to work with
    TCreateSessionParams sessionParams;
    sessionParams.iPriority = TCreateSessionParams::EPriorityStandard;
    sessionParams.iSocketBufSize = KSocketBufSize;
    TUint port( 5010 );
    TBool enableRtcp( ETrue );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iRtcpTimeOut = 0;
    rtcpParams.iSessionBWidth = 32000;

    TRtpId error( KNullId );
    error = iRtpAPI->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    RTP_EUNIT_ASSERT_RTP_ID ( error )
  
    

    // By default, the RTCP sending should be enabled
    TInt err(KErrNone);
    TBool autoSending( EFalse );
    err = iRtpAPI->IsRtcpSendingSuspended( error, autoSending );
    RTP_EUNIT_ASSERT_EQUALS ( err, KErrNone );
        
    EUNIT_ASSERT ( autoSending )
  
  
    // This will disable automatic RTCP sending
    err = iRtpAPI->SuspendRtcpSending( error, EFalse );
    RTP_EUNIT_ASSERT_EQUALS ( err, KErrNone );
    
    err = iRtpAPI->IsRtcpSendingSuspended( error, autoSending );
    RTP_EUNIT_ASSERT_EQUALS ( err, KErrNone );
	EUNIT_ASSERT ( !autoSending )

    // This will re-enable automatic RTCP sending
    err = iRtpAPI->SuspendRtcpSending( error, ETrue );
    RTP_EUNIT_ASSERT_EQUALS ( err, KErrNone );
    err = iRtpAPI->IsRtcpSendingSuspended( error, autoSending );
    RTP_EUNIT_ASSERT_EQUALS ( err,  KErrNone );
    EUNIT_ASSERT ( autoSending )
	}

void UT_CRtpAPI::UT_CRtpAPI_GetSessionIdL(  )
    {
    TInt invalidStream( 42 );
    TInt result( KErrNone );
    result = iRtpAPI->GetSessionId( invalidStream );
    EUNIT_ASSERT ( result == KNullId )
    }

void UT_CRtpAPI::UT_CRtpAPI_GetRtpSocketL(  )
    {
    TInt invalidSession( 42 );
    TInt result( KErrNone );

    RSocket* socket( NULL );
    socket = iRtpAPI->GetRtpSocket( invalidSession );
    EUNIT_ASSERT  ( socket == NULL )
    }

void UT_CRtpAPI::UT_CRtpAPI_GetRtcpSocketL(  )
    {
   	TInt invalidSession( 42 );
    TInt result( KErrNone );
	RSocket* socket( NULL );
    socket = iRtpAPI->GetRtcpSocket( invalidSession );
    EUNIT_ASSERT  ( socket == NULL );
    }

void UT_CRtpAPI::UT_CRtpAPI_GetStreamStatisticsL(  )
    {
   	TInt invalidStream( 42 );
    TInt result( KErrNone );

    TRtpPeerStat stat;
    result = iRtpAPI->GetStreamStatistics( invalidStream, stat );
    EUNIT_ASSERT ( result == KErrGeneral )
        
    }

void UT_CRtpAPI::UT_CRtpAPI_GetSamplingRateL(  )
    {
    TUint32 rate( 0 );
    TInt badIndex( 128 + 1 );

    // This rate index is out of range
    rate = iRtpAPI->GetSamplingRate( badIndex );
    EUNIT_ASSERT ( rate == 0 )

    // This rate should be 8000, according to RFC 3551
    rate = iRtpAPI->GetSamplingRate( 0 ); 
    EUNIT_ASSERT ( rate == 8000 )
        
    const TInt index( 5 );

    // Store the previous rate for payload type "index"
    TUint32 prevRate( iRtpAPI->GetSamplingRate( index ) );

    TInt error( KErrNone );

    // Try to set the rate with an out-of-range index
    error = iRtpAPI->SetSamplingRate( badIndex, prevRate );
    EUNIT_ASSERT ( error == KErrArgument )
        
    // Try to set a zero rate -> divide by zero
    error = iRtpAPI->SetSamplingRate( index, 0 );
    EUNIT_ASSERT ( error == KErrDivideByZero )

    EUNIT_ASSERT  ( iRtpAPI->GetSamplingRate( index ) == prevRate )
 

    // Set a valid sample rate
    error = iRtpAPI->SetSamplingRate( index, prevRate / 2 );
    EUNIT_ASSERT  ( error == KErrNone )
    
    EUNIT_ASSERT( iRtpAPI->GetSamplingRate( index ) == prevRate / 2 )
        
    }

void UT_CRtpAPI::UT_CRtpAPI_SetSamplingRateL(  )
    {
    EUNIT_ASSERT( ETrue);
    //tested with GetSamplingRateL
    }

void UT_CRtpAPI::UT_CRtpAPI_SetRtcpParametersL(  )
    {
    TInt invalidSession( 42 );
    TInt result( KErrNone );
    TRtcpParams rtcpParams;
    result = iRtpAPI->SetRtcpParameters( invalidSession, rtcpParams );
    EUNIT_ASSERT ( result == KErrNotFound )
    }

void UT_CRtpAPI::UT_CRtpAPI_CustomCommandSyncL(  )
    {
  
    TInt result( KErrNone );
    TBuf8<5> dummy;
    TBufC8<5> dummyC;
    result = iRtpAPI->CustomCommandSync( 0, dummyC, dummyC, dummy );
    EUNIT_ASSERT ( result == KErrNotSupported )
    }

void UT_CRtpAPI::UT_CRtpAPI_CustomCommandAsyncL(  )
    {
    TInt result( KErrNone );
    TBuf8<5> dummy;
    TBufC8<5> dummyC;
    TRequestStatus status;
    result = iRtpAPI->CustomCommandAsync( 0, dummyC, dummyC, dummy, status );
    EUNIT_ASSERT ( result == KErrNotSupported )
        
    }

void UT_CRtpAPI::UT_CRtpAPI_VersionL(  )
    {
    TVersion version;
    version = iRtpAPI->Version();
    EUNIT_ASSERT ( version.iMajor == 2 ||
         version.iMinor == 0 ||
         version.iBuild == 0 )
    }

void UT_CRtpAPI::ErrorNotify( TInt aErrCode ) 
	{
	iErr= aErrCode;
	}
	
//from Obs	
void UT_CRtpAPI::RtpPacketReceived( TRtpId /*aStreamId*/, 
                                        const TRtpRecvHeader& /*aHeaderInfo*/, 
                                        const TDesC8& /*aPayloadData*/ ) 
	{
	
	}
void UT_CRtpAPI::NonRTPDataReceived( TUint /*aPort*/, 
									TBool /*aRTPport*/, 
									const TDesC8& /*aNonRTPData*/ )
	{
	
	}
void UT_CRtpAPI::SdesReceived( TRtpSSRC /*aSSRC*/, const TRtpSdesParams& /*aParams*/ ) 
	{
	
	}
void UT_CRtpAPI::ByeReceived( TRtpId /*aStreamId*/, 
							TRtpSSRC /*aSSRC*/, 
							const TDesC8& /*aReason*/ ) 
	{
	
	}
void UT_CRtpAPI::AppReceived( TRtpId /*aStreamId*/, 
							TRtpSSRC /*aSSRC*/, 
							const TRtcpApp& /*aApp*/ ) 
	{
	
	}
void UT_CRtpAPI::SrReceived( TRtpId /*aStreamId*/, 
						TRtpSSRC /*aSSRC*/, 
						const TTimeStamps& /*aTimeStamps*/ )
	{
	
	}
void UT_CRtpAPI::RrReceived( TRtpId /*aStreamId*/, 
							TRtpSSRC /*aSSRC*/ ) 
	{
	
	}

void UT_CRtpAPI::SRTPMasterKeyStaleEvent(const CSRTPSession&  /*aSession*/)
	{
	
	}
void UT_CRtpAPI::SRTPMasterKeyStaleEvent(const CSRTPStream&  /*aStream*/)
{
	
}

void UT_CRtpAPI::ReadyToSendRtpPacket( TRtpId /*aTranStreamId*/, TPtr8 &/*aPacket*/ )
    {
    // RTP Packet Callback received
    iIsCallBackReceived = ETrue;
    }


void UT_CRtpAPI::UT_CRtpAPI_RegisterRtpPostProcessingObserverL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    EUNIT_ASSERT(result==KErrNone);
    
    TRequestStatus status;
    result = iRtpAPI->StartConnection( status, KDefaultIap2 ); // KDefaultIap
    RTP_EUNIT_ASSERT_EQUALS ( result, KErrNone );
 // Wait until the connection is really up
    User::WaitForRequest( status );

    //create session
    TCreateSessionParams sessionParams;
    sessionParams.iPriority = TCreateSessionParams::EPriorityStandard;
    sessionParams.iSocketBufSize = KSocketBufSize;
    TUint port( 5000 );
    TBool enableRtcp( EFalse );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iRtcpTimeOut = 0;
    rtcpParams.iSessionBWidth = 32000;

    TRtpId error( KNullId );
    error = iRtpAPI->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    RTP_EUNIT_ASSERT_RTP_ID( error );
    
    TRcvStreamParams recvparams;
    recvparams.iPayloadType = 0;

    TRtpId error2( KNullId );
    TInt err( KErrNone );
    error2 = iRtpAPI->CreateReceiveStreamL( error /*session id*/, recvparams );
    RTP_EUNIT_ASSERT_RTP_ID ( error2 );
        
    
    err = iRtpAPI->RegisterRtpPostProcessingObserver( error, *this );
    EUNIT_ASSERT ( err == KErrNone )
    }


void UT_CRtpAPI::UT_CRtpAPI_SendRtpPacketWithCSRCL()
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    RTP_EUNIT_ASSERT_EQUALS(result, KErrNone);
    
    TRequestStatus status;
    result = iRtpAPI->StartConnection( status, KDefaultIap ); // KDefaultIap
    RTP_EUNIT_ASSERT_EQUALS ( result, KErrNone );
 // Wait until the connection is really up
    User::WaitForRequest( status );

    //create session
    TCreateSessionParams sessionParams;
    sessionParams.iPriority = TCreateSessionParams::EPriorityStandard;
    sessionParams.iSocketBufSize = KSocketBufSize;
    TUint port( 5000 );
    TBool enableRtcp( EFalse );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iRtcpTimeOut = 0;
    rtcpParams.iSessionBWidth = 32000;

    TRtpId error( KNullId );
    error = iRtpAPI->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    RTP_EUNIT_ASSERT_RTP_ID( error );
    
    TTranStreamParams transparams;
    transparams.iPayloadType = 0;
    TRtpSSRC ssrc( 0 );
    
    TInt err( KErrNone );
    err = iRtpAPI->RegisterRtpPostProcessingObserver( error, *this );
    EUNIT_ASSERT ( err == KErrNone )


    err = iRtpAPI->CreateTransmitStreamL( error /*session id*/, transparams, ssrc );
    RTP_EUNIT_ASSERT_RTP_ID ( err );
    
    
    TRtpSendHeader header; 
    header.iHeaderExtension = NULL;
    header.iMarker = 0;
    header.iPadding = 0;
    header.iPayloadType = 0;
    header.iTimestamp = 10;

    HBufC8* data = HBufC8::NewLC(KLength);
    data->Des().Copy(KHello);
    CleanupStack::Pop(data);

    TRtpSendPktParams *headerinfo = new(ELeave) TRtpSendPktParams(header);
    headerinfo->iTranStreamId = err;
    headerinfo->iPayloadData.Set(data->Des()); 
    
    const TUint KArrayGranularity = 15;
    
    CArrayFixFlat<TUint32>* fix;
    fix = new(ELeave) CArrayFixFlat<TUint32>(KArrayGranularity);
    
    fix->AppendL(123456);
    fix->AppendL(999999);
     
    TInt error2( KErrNone );
    iIsCallBackReceived = EFalse;
    // Try sending synchronously with Csrc
    error2 = iRtpAPI->SendRtpPacket(*headerinfo, fix->Array());
    RTP_EUNIT_ASSERT_EQUALS ( error2, KErrNone );
    
    CRtpSession* rtpSession = iRtpAPI->iManager->GetSession( err );
    User::WaitForRequest( rtpSession->iCommNet->iSender[ERTPPort]->iStatus );
    rtpSession->iCommNet->iSender[ERTPPort]->RunL();
    rtpSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
    EUNIT_ASSERT(iIsCallBackReceived == ETrue);
    
    iIsCallBackReceived = EFalse;
    // Try sending asynchronously with CSRC
    headerinfo->iStatus = &status;
    error2 = iRtpAPI->SendRtpPacket(*headerinfo, fix->Array());
    RTP_EUNIT_ASSERT_EQUALS ( error2, KErrNone );
    EUNIT_ASSERT( KRequestPending == status.Int() );
    User::WaitForRequest( rtpSession->iCommNet->iSender[ERTPPort]->iStatus );
    rtpSession->iCommNet->iSender[ERTPPort]->RunL();
    rtpSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
    User::WaitForRequest( status );
    EUNIT_ASSERT( KErrNone == status.Int() );
    // Is call back received check
    EUNIT_ASSERT(iIsCallBackReceived == ETrue);
    
    iIsCallBackReceived = EFalse;
    // Try sending asynchronously, specifying the sequence number with CSRC
	TRtpSequence seqNum = 42;
    headerinfo->iSequenceNum = &seqNum;
    error2 = iRtpAPI->SendRtpPacket(*headerinfo, fix->Array());
    RTP_EUNIT_ASSERT_EQUALS ( error2, KErrNone );
    EUNIT_ASSERT( KRequestPending == status.Int() );
    User::WaitForRequest( rtpSession->iCommNet->iSender[ERTPPort]->iStatus );
    rtpSession->iCommNet->iSender[ERTPPort]->RunL();
    rtpSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
    User::WaitForRequest( status );
    EUNIT_ASSERT( KErrNone == status.Int() );
    EUNIT_ASSERT(iIsCallBackReceived == ETrue);
    
    delete fix;
    delete headerinfo;
    delete data;
    }


void UT_CRtpAPI::UT_CRtpAPI_UnregisterRtpPostProcessingObserverL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    
    result = iRtpAPI->OpenL( params, NULL, NULL, NULL );
    EUNIT_ASSERT(result==KErrNone);
    
    TRequestStatus status;
    result = iRtpAPI->StartConnection( status, KDefaultIap2 ); // KDefaultIap
    RTP_EUNIT_ASSERT_EQUALS ( result, KErrNone );
 // Wait until the connection is really up
    User::WaitForRequest( status );

    //create session
    TCreateSessionParams sessionParams;
    sessionParams.iPriority = TCreateSessionParams::EPriorityStandard;
    sessionParams.iSocketBufSize = KSocketBufSize;
    TUint port( 5000 );
    TBool enableRtcp( EFalse );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iRtcpTimeOut = 0;
    rtcpParams.iSessionBWidth = 32000;

    TRtpId error( KNullId );
    error = iRtpAPI->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    RTP_EUNIT_ASSERT_RTP_ID( error );
    
    TRcvStreamParams recvparams;
    recvparams.iPayloadType = 0;

    TRtpId error2( KNullId );
    TInt err( KErrNone );
    error2 = iRtpAPI->CreateReceiveStreamL( error /*session id*/, recvparams );
    RTP_EUNIT_ASSERT_RTP_ID ( error2 );
        
    TTranStreamParams transparams;
    transparams.iPayloadType = 0;
    TRtpSSRC ssrc( 0 );
    
    err = iRtpAPI->RegisterRtpPostProcessingObserver( error, *this );
    EUNIT_ASSERT ( err == KErrNone )
    
    err = iRtpAPI->CreateTransmitStreamL( error /*session id*/, transparams, ssrc );
    RTP_EUNIT_ASSERT_RTP_ID ( err );
    
    
    TRtpSendHeader header; 
    header.iHeaderExtension = NULL;
    header.iMarker = 0;
    header.iPadding = 0;
    header.iPayloadType = 0;
    header.iTimestamp = 10;

    HBufC8* data = HBufC8::NewLC(KLength);
    data->Des().Copy(KHello);
    CleanupStack::Pop(data);

    TRtpSendPktParams *headerinfo = new(ELeave) TRtpSendPktParams(header);
    headerinfo->iTranStreamId = err;
    headerinfo->iPayloadData.Set(data->Des()); 
    
    const TUint KArrayGranularity = 15;
    
    CArrayFixFlat<TUint32>* fix;
    fix = new(ELeave) CArrayFixFlat<TUint32>(KArrayGranularity);
    
    fix->AppendL(123456);
    fix->AppendL(999999);
     
    TInt error3( KErrNone );
    iIsCallBackReceived = EFalse;
    // Try sending synchronously with Csrc
    error3 = iRtpAPI->SendRtpPacket(*headerinfo, fix->Array());
    RTP_EUNIT_ASSERT_EQUALS ( error3, KErrNone );
    
    CRtpSession* rtpSession = iRtpAPI->iManager->GetSession( err );
    User::WaitForRequest( rtpSession->iCommNet->iSender[ERTPPort]->iStatus );
    rtpSession->iCommNet->iSender[ERTPPort]->RunL();
    rtpSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
    EUNIT_ASSERT(iIsCallBackReceived == ETrue);
    
    // Unregister callback
    iRtpAPI->UnregisterRtpPostProcessingObserver(error);
    
    iIsCallBackReceived = EFalse;
    error3 = KErrNone;
    // Try sending synchronously with Csrc
    error3 = iRtpAPI->SendRtpPacket(*headerinfo, fix->Array());
    RTP_EUNIT_ASSERT_EQUALS ( error3, KErrNone );
    
    User::WaitForRequest( rtpSession->iCommNet->iSender[ERTPPort]->iStatus );
    rtpSession->iCommNet->iSender[ERTPPort]->RunL();
    rtpSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
    EUNIT_ASSERT(iIsCallBackReceived == EFalse);
    
    delete fix;
    delete headerinfo;
    delete data;
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CRtpAPI,
    "Test CRtpAPI.",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "CRtpAPI",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_NewLL, Teardown)

EUNIT_TEST(
    "OpenL - test ",
    "CRtpAPI",
    "OpenL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_OpenLL, Teardown)

EUNIT_TEST(
    "StartConnection - test ",
    "CRtpAPI",
    "StartConnection",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_StartConnectionL, Teardown)

EUNIT_TEST(
    "StartConnection - test ",
    "CRtpAPI",
    "StartConnection",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_StartConnection_OneL, Teardown)

EUNIT_TEST(
    "CancelStart - test ",
    "CRtpAPI",
    "CancelStart",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_CancelStartL, Teardown)

EUNIT_TEST(
    "Close - test ",
    "CRtpAPI",
    "Close",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_CloseL, Teardown)

EUNIT_TEST(
    "SetLocalSdes - test ",
    "CRtpAPI",
    "SetLocalSdes",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_SetLocalSdesL, Teardown)

EUNIT_TEST(
    "GetLocalIPAddressL - test ",
    "CRtpAPI",
    "GetLocalIPAddressL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_GetLocalIPAddressLL, Teardown)

EUNIT_TEST(
    "CreateSessionL - test ",
    "CRtpAPI",
    "CreateSessionL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_CreateSessionLL, Teardown)
    
 EUNIT_TEST(
    "CreateSRTPSessionL - test ",
    "CRtpAPI",
    "CreateSRTPSessionL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_CreateSessionL2L, Teardown) 

EUNIT_TEST(
    "StartSession - test ",
    "CRtpAPI",
    "StartSession",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_StartSessionL, Teardown)
    
EUNIT_TEST(
    "StartSRTPSession - test ",
    "CRtpAPI",
    "StartSRTPSession",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_TestStartSession2L, Teardown)    

EUNIT_TEST(
    "CloseSession - test ",
    "CRtpAPI",
    "CloseSession",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_CloseSessionL, Teardown)

EUNIT_TEST(
    "SetRemoteAddress - test ",
    "CRtpAPI",
    "SetRemoteAddress",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_SetRemoteAddressL, Teardown)

EUNIT_TEST(
    "SetRemoteRtcpAddress - test ",
    "CRtpAPI",
    "SetRemoteRtcpAddress",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_SetRemoteRtcpAddressL, Teardown)

EUNIT_TEST(
    "CreateReceiveStreamL - test ",
    "CRtpAPI",
    "CreateReceiveStreamL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_CreateReceiveStreamLL, Teardown)

EUNIT_TEST(
    "CreateTransmitStreamL - test ",
    "CRtpAPI",
    "CreateTransmitStreamL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_CreateTransmitStreamLL, Teardown)

EUNIT_TEST(
    "CreateTransmitStreamExtL - test ",
    "CRtpAPI",
    "CreateTransmitStreamExtL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_CreateTransmitStreamExtLL, Teardown)

EUNIT_TEST(
    "CloseStream - test ",
    "CRtpAPI",
    "CloseStream",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_CloseStreamL, Teardown)

EUNIT_TEST(
    "RegisterRtpObserver - test ",
    "CRtpAPI",
    "RegisterRtpObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_RegisterRtpObserverL, Teardown)

EUNIT_TEST(
    "UnregisterRtpObserver - test ",
    "CRtpAPI",
    "UnregisterRtpObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_UnregisterRtpObserverL, Teardown)

EUNIT_TEST(
    "SetNonRTPDataObserver - test ",
    "CRtpAPI",
    "SetNonRTPDataObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_SetNonRTPDataObserverL, Teardown)

EUNIT_TEST(
    "SendRtpPacket - test ",
    "CRtpAPI",
    "SendRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_SendRtpPacketL, Teardown)

EUNIT_TEST(
    "SendRtpPacket - test ",
    "CRtpAPI",
    "SendRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_SendRtpPacket_1L, Teardown)

EUNIT_TEST(
    "SendRtpPacket - test ",
    "CRtpAPI",
    "SendRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_SendRtpPacket_2L, Teardown)

EUNIT_TEST(
    "SendRtpPacket3L - test ",
    "CRtpAPI",
    "SendRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_SendRtpPacket_ThreeL, Teardown)

EUNIT_TEST(
    "SendDataL - test ",
    "CRtpAPI",
    "SendDataL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_SendDataLL, Teardown)

EUNIT_TEST(
    "CancelSend - test ",
    "CRtpAPI",
    "CancelSend",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_CancelSendL, Teardown)

EUNIT_TEST(
    "RegisterRtcpObserver - test ",
    "CRtpAPI",
    "RegisterRtcpObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_RegisterRtcpObserverL, Teardown)

EUNIT_TEST(
    "UnregisterRtcpObserver - test ",
    "CRtpAPI",
    "UnregisterRtcpObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_UnregisterRtcpObserverL, Teardown)

EUNIT_TEST(
    "SendRtcpByePacket - test ",
    "CRtpAPI",
    "SendRtcpByePacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_SendRtcpByePacketL, Teardown)

EUNIT_TEST(
    "SendRtcpAppPacket - test ",
    "CRtpAPI",
    "SendRtcpAppPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_SendRtcpAppPacketL, Teardown)

EUNIT_TEST(
    "SendRtcpRrPacket - test ",
    "CRtpAPI",
    "SendRtcpRrPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_SendRtcpRrPacketL, Teardown)

EUNIT_TEST(
    "SendRtcpSrPacket - test ",
    "CRtpAPI",
    "SendRtcpSrPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_SendRtcpSrPacketL, Teardown)

EUNIT_TEST(
    "SuspendRtcpSending - test ",
    "CRtpAPI",
    "SuspendRtcpSending",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_SuspendRtcpSendingL, Teardown)

EUNIT_TEST(
    "IsRtcpSendingSuspended - test ",
    "CRtpAPI",
    "IsRtcpSendingSuspended",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_IsRtcpSendingSuspendedL, Teardown)

EUNIT_TEST(
    "GetSessionId - test ",
    "CRtpAPI",
    "GetSessionId",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_GetSessionIdL, Teardown)

EUNIT_TEST(
    "GetRtpSocket - test ",
    "CRtpAPI",
    "GetRtpSocket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_GetRtpSocketL, Teardown)

EUNIT_TEST(
    "GetRtcpSocket - test ",
    "CRtpAPI",
    "GetRtcpSocket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_GetRtcpSocketL, Teardown)

EUNIT_TEST(
    "GetStreamStatistics - test ",
    "CRtpAPI",
    "GetStreamStatistics",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_GetStreamStatisticsL, Teardown)

EUNIT_TEST(
    "GetSamplingRate - test ",
    "CRtpAPI",
    "GetSamplingRate",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_GetSamplingRateL, Teardown)

EUNIT_TEST(
    "SetSamplingRate - test ",
    "CRtpAPI",
    "SetSamplingRate",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_SetSamplingRateL, Teardown)

EUNIT_TEST(
    "SetRtcpParameters - test ",
    "CRtpAPI",
    "SetRtcpParameters",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_SetRtcpParametersL, Teardown)

EUNIT_TEST(
    "CustomCommandSync - test ",
    "CRtpAPI",
    "CustomCommandSync",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_CustomCommandSyncL, Teardown)

EUNIT_TEST(
    "CustomCommandAsync - test ",
    "CRtpAPI",
    "CustomCommandAsync",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_CustomCommandAsyncL, Teardown)

EUNIT_TEST(
    "Version - test ",
    "CRtpAPI",
    "Version",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_VersionL, Teardown)
    
EUNIT_TEST(
    "RegisterRtpPostProcessingObserver - test ",
    "CRtpAPI",
    "RegisterRtpPostProcessingObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_RegisterRtpPostProcessingObserverL, Teardown)    


EUNIT_TEST(
    "SendRtpPacketWithCSRC - test ",
    "CRtpAPI",
    "SendRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_SendRtpPacketWithCSRCL, Teardown)   
    
EUNIT_TEST(
    "UnregisterRtpPostProcessingObserver - test ",
    "CRtpAPI",
    "UnregisterRtpPostProcessingObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpAPI_UnregisterRtpPostProcessingObserverL, Teardown)  

    
EUNIT_END_TEST_TABLE

//  END OF FILE
