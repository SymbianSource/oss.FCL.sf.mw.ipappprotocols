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
#include "UT_CRtpManager.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "rtpmanager.h"


const TUint KIap1 = 0;
const TUint KIAPID = 6;
const TUint KIap2 = 1;
const TInt KLength = 20;
_LIT8(KHello, "hello...testing");

#define RTP_EUNIT_ASSERT_SPECIFIC_LEAVE( func, leaveCode ) \
TRAPD( leaveErr, func );\
if ( leaveErr == KErrNoMemory )\
    {\
    User::Leave( KErrNoMemory );\
    }\
else\
    {\
    EUNIT_ASSERT_EQUALS( leaveErr, leaveCode );\
    }

#define RTP_EUNIT_ASSERT_EQUALS( func, val2 ) \
{TInt val1 = func;\
if ( val2 != KErrNoMemory && val1 == KErrNoMemory ){\
	User::Leave( KErrNoMemory );\
	}\
else{\
	EUNIT_ASSERT_EQUALS( val1, val2 );\
	}}
	   
// CONSTRUCTION
UT_CRtpManager* UT_CRtpManager::NewL()
    {
    UT_CRtpManager* self = UT_CRtpManager::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CRtpManager* UT_CRtpManager::NewLC()
    {
    UT_CRtpManager* self = new( ELeave ) UT_CRtpManager();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CRtpManager::~UT_CRtpManager()
    {
    }

// Default constructor
UT_CRtpManager::UT_CRtpManager()
    {
    }

// Second phase construct
void UT_CRtpManager::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

void UT_CRtpManager::ErrorNotify( TInt /*aErrCode*/ ) 
	{
	
	}

void UT_CRtpManager::SetupL(  )
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
   
      
	iRtpManager = CRtpManager::NewL( *this );
    }

void UT_CRtpManager::Teardown(  )
    {
	delete iRtpManager; iRtpManager=NULL;
	iSocket.Close(); 
	iConnection.Stop();
    iConnection.Close(); 
    iSocketServ.Close(); 
    }

void UT_CRtpManager::UT_CRtpManager_NewLL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_OpenLL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    TPtrC16 fileName( _L16( "rtppacket.dll" ) );
    const TDesC16* temp= &fileName;
    
    RTP_EUNIT_ASSERT_SPECIFIC_LEAVE(iRtpManager->
    		OpenL( params, temp, &iSocketServ, &iConnection ), KErrNotFound);
    }

void UT_CRtpManager::UT_CRtpManager_StartConnectionL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );
    
    
    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
    iRtpManager->Close();
    }

void UT_CRtpManager::UT_CRtpManager_StartConnection_OneL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );
    
    iSocketServ.iNextError = KErrNotReady;
    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNotReady);
    iSocketServ.iNextError = KErrNone;
    iRtpManager->Close();
    }

void UT_CRtpManager::UT_CRtpManager_StartConnection_TwoL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );
    iConnection.iNextError = KErrNotReady;
    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNotReady);
    iConnection.iNextError = KErrNone;
    iRtpManager->Close();
    }
void UT_CRtpManager::UT_CRtpManager_CancelStartL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_CloseL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_SetLocalSdesL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_GetLocalIPAddressL1L(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );

    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(0), KErrNone);
    iRtpManager->iIapId = 0;
    EUNIT_ASSERT_SPECIFIC_LEAVE( iRtpManager->GetLocalIPAddressL(), 
    				KErrNotSupported);
    iRtpManager->iIapId=KIAPID;				
    iRtpManager->Close();				
    
    }
    
 void UT_CRtpManager::UT_CRtpManager_GetLocalIPAddressL2L(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );

    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
    iRtpManager->iConnPtr=NULL;	
    iRtpManager->GetLocalIPAddressL();			
    iRtpManager->Close();
    }   

void UT_CRtpManager::UT_CRtpManager_GetIAPIdL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );

    
    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
    iRtpManager->iConnPtr=NULL;	
    TUint32 iapId;
    EUNIT_ASSERT(iRtpManager->GetIapId(iapId)== KErrNotReady);			
    iRtpManager->Close();
    }   
void UT_CRtpManager::UT_CRtpManager_RemoveRtpObjectLL(  )
	{
TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );

    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
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
    sessionId = iRtpManager->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    EUNIT_ASSERT( sessionId != KNullId );
	iRtpManager->RemoveRtpObject(sessionId, ESession);
	
	iRtpManager->CloseSession(sessionId);
    iRtpManager->Close();
	}


void UT_CRtpManager::UT_CRtpManager_CreateSessionLL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_StartSessionL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_CloseSessionL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );

    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
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
    sessionId = iRtpManager->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    EUNIT_ASSERT( sessionId != KNullId );
    //close
    iRtpManager->CloseSession(sessionId);
    iRtpManager->Close();
    }

void UT_CRtpManager::UT_CRtpManager_SetRemoteAddressL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_SetRemoteRtcpAddressL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_CreateStreamLL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_CreateReceiveStreamLL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_CreateTransmitStreamLL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_CreateTransmitStreamExtLL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_CloseStreamL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_RegisterRtpObserverL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_UnregisterRtpObserverL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_SetNonRTPDataObserverL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_SendRtpPacketL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_SendRtpPacket_1L(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_SendRtpPacket_2L(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_SendDataLL(  )
    {
     TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );

    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
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
    sessionId = iRtpManager->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    EUNIT_ASSERT( sessionId != KNullId );
    iRtpManager->iStandardRtp=EFalse;
    HBufC8* data = HBufC8::NewLC(KLength);
	data->Des().Copy(KHello);
	CleanupStack::Pop(data);
	TRequestStatus status(KErrNone);
    EUNIT_ASSERT_SPECIFIC_LEAVE(iRtpManager->SendDataL(sessionId,
                       ETrue,
                       *data,
                       status), KErrNotSupported);
                       
    //close
    delete data;
    iRtpManager->CloseSession(sessionId);
    iRtpManager->Close();
    }

void UT_CRtpManager::UT_CRtpManager_CancelSendL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );

    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
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
    sessionId = iRtpManager->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    EUNIT_ASSERT( sessionId != KNullId );
    iRtpManager->iStandardRtp=ETrue;
    
    iRtpManager->CancelSend( sessionId );                   
    //close
    iRtpManager->CloseSession( sessionId );
    iRtpManager->Close();
    }

void UT_CRtpManager::UT_CRtpManager_RegisterRtcpObserverL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );

    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
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
    sessionId = iRtpManager->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    EUNIT_ASSERT( sessionId != KNullId );
    iRtpManager->iStandardRtp=ETrue;
    iRtpManager->RegisterRtcpObserver( sessionId, *this );
    iRtpManager->UnregisterRtcpObserver( sessionId );
    iRtpManager->CloseSession( sessionId );
    iRtpManager->Close();
    }

void UT_CRtpManager::UT_CRtpManager_UnregisterRtcpObserverL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_SendRtcpByePacketL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );

    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
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
    sessionId = iRtpManager->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    EUNIT_ASSERT( sessionId != KNullId );
    iRtpManager->iStandardRtp=ETrue;
    iRtpManager->RegisterRtcpObserver( sessionId, *this );
    iRtpManager->SendRtcpByePacketL( KIAPID, _L8( "ByePacket" )  );
    iRtpManager->UnregisterRtcpObserver( sessionId );
    iRtpManager->CloseSession( sessionId );
    iRtpManager->Close();
    }

void UT_CRtpManager::UT_CRtpManager_SendRtcpAppPacketL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );

    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
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
    sessionId = iRtpManager->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    EUNIT_ASSERT( sessionId != KNullId );
    iRtpManager->iStandardRtp=ETrue;
    TRtcpApp app;
    app.iSubType = 0;
    app.iName[0] = 0;
    app.iAppData[0] = 0;
    app.iAppDataLen = KMaxRtcpAppData + 1;
    iRtpManager->RegisterRtcpObserver( sessionId, *this );
    iRtpManager->SendRtcpAppPacketL( KIAPID, app  );
    iRtpManager->UnregisterRtcpObserver( sessionId );
    iRtpManager->CloseSession( sessionId );
    iRtpManager->Close();
    }

void UT_CRtpManager::UT_CRtpManager_SendRtcpRrPacketL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );

    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
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
    sessionId = iRtpManager->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    EUNIT_ASSERT( sessionId != KNullId );
    iRtpManager->iStandardRtp=ETrue;
    iRtpManager->RegisterRtcpObserver( sessionId, *this );
    iRtpManager->SendRtcpRrPacketL( KIAPID  );
    iRtpManager->UnregisterRtcpObserver( sessionId );
    iRtpManager->CloseSession( sessionId );
    iRtpManager->Close();
    }

void UT_CRtpManager::UT_CRtpManager_SendRtcpSrPacketL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );

    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
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
    sessionId = iRtpManager->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    EUNIT_ASSERT( sessionId != KNullId );
    iRtpManager->iStandardRtp=ETrue;
    iRtpManager->RegisterRtcpObserver( sessionId, *this );
    iRtpManager->SendRtcpByePacketL( KIAPID, _L8( "SrPacket" )  );
    iRtpManager->UnregisterRtcpObserver( sessionId );
    iRtpManager->CloseSession( sessionId );
    iRtpManager->Close();
    }

void UT_CRtpManager::UT_CRtpManager_SuspendRtcpSendingL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );

    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
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
    
    
    iRtpManager->iStandardRtp=ETrue;
    iRtpManager->SuspendRtcpSending( sessionId, EFalse );
    iRtpManager->CloseSession( sessionId );
    iRtpManager->Close();
    }

void UT_CRtpManager::UT_CRtpManager_IsRtcpSendingSuspendedL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );

    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
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
    
    
    iRtpManager->iStandardRtp=ETrue;
    TBool autosending;
    iRtpManager->IsRtcpSendingSuspended( sessionId, autosending );
    iRtpManager->CloseSession( sessionId );
    iRtpManager->Close();
    }

void UT_CRtpManager::UT_CRtpManager_GetSessionIdL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );

    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
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
    sessionId = iRtpManager->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    EUNIT_ASSERT( sessionId != KNullId );
    iRtpManager->GetSessionId(sessionId);
    //close
    iRtpManager->CloseSession(sessionId);
    iRtpManager->Close();
    }

void UT_CRtpManager::UT_CRtpManager_GetRtpSocketL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );

    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
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
    sessionId = iRtpManager->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    EUNIT_ASSERT( sessionId != KNullId );
    iRtpManager->GetRtpSocket( sessionId );
    //close
    iRtpManager->CloseSession(sessionId);
    iRtpManager->Close();
    }

void UT_CRtpManager::UT_CRtpManager_GetRtcpSocketL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );

    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
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
    sessionId = iRtpManager->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    EUNIT_ASSERT( sessionId != KNullId );
    iRtpManager->GetRtcpSocket( sessionId );
    //close
    iRtpManager->CloseSession(sessionId);
    iRtpManager->Close();
    }

void UT_CRtpManager::UT_CRtpManager_GetStreamStatisticsL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );

    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
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
    sessionId = iRtpManager->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    EUNIT_ASSERT( sessionId != KNullId );
    TRtpPeerStat state;
    iRtpManager->GetStreamStatistics( sessionId, state );
    //close
    iRtpManager->CloseSession(sessionId);
    iRtpManager->Close();
    }

void UT_CRtpManager::UT_CRtpManager_GetSamplingRateL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );

    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
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
    sessionId = iRtpManager->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    EUNIT_ASSERT( sessionId != KNullId );
    TRtpPayloadType payloadType( 2 ); 
    iRtpManager->GetSamplingRate( payloadType );
    //close
    iRtpManager->CloseSession(sessionId);
    iRtpManager->Close();
    }

void UT_CRtpManager::UT_CRtpManager_SetSamplingRateL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpManager::UT_CRtpManager_SetRtcpParametersL(  )
    {
    TInt result(KErrNone);
    TRtpSdesParams sdesparams;
    iRtpManager->OpenL( sdesparams, NULL, &iSocketServ, &iConnection );

    RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
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
    sessionId = iRtpManager->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    EUNIT_ASSERT( sessionId != KNullId );
    TRtcpParams params;
    iRtpManager->SetRtcpParameters( sessionId, params );
    //close
    iRtpManager->CloseSession(sessionId);
    iRtpManager->Close();
    }
    
void UT_CRtpManager::UT_CRtpManager_AddStreamToSessionL()
	{
	EUNIT_ASSERT(iRtpManager->AddStreamToSession(0, 0)==KErrNotFound);
	TInt result(KErrNone);
    TRtpSdesParams params;
    iRtpManager->OpenL( params, NULL, &iSocketServ, &iConnection );

	TCreateSessionParams sessionParams;
    sessionParams.iPriority = TCreateSessionParams::EPriorityStandard;
    sessionParams.iSocketBufSize = KSocketBufSize;
    TUint port( 5000 );
    TBool enableRtcp( EFalse );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iRtcpTimeOut = 0;
    rtcpParams.iSessionBWidth = 32000;
	RTP_EUNIT_ASSERT_EQUALS(iRtpManager->StartConnection(KIAPID), KErrNone);
   
    TRtpId sessionId( KNullId );
    sessionId = iRtpManager->CreateSessionL( sessionParams,
                                    port,
                                    enableRtcp,
                                    &rtcpParams );
    EUNIT_ASSERT( sessionId != KNullId );
    iRtpManager->AddStreamToSession(sessionId, 1);
    //close
    iRtpManager->CloseSession(sessionId);
    iRtpManager->Close();	
	}


void UT_CRtpManager::SdesReceived( TRtpSSRC /*aSSRC*/,
                           const TRtpSdesParams& /*aParams*/ )
	{
	}
    
void UT_CRtpManager::ByeReceived( TRtpId /*aStreamId*/, TRtpSSRC /*aSSRC*/,
                  const TDesC8& /*aReason*/ )
	{
	}
void UT_CRtpManager::AppReceived( TRtpId /*aStreamId*/, TRtpSSRC /*aSSRC*/,
                  const TRtcpApp& /*aApp*/ )
	{
	}
void UT_CRtpManager::SrReceived( TRtpId /*aStreamId*/, TRtpSSRC /*aSSRC*/,
                 const TTimeStamps& /*aTimeStamps*/ )
	{
	}
void UT_CRtpManager::RrReceived( TRtpId /*aStreamId*/, TRtpSSRC /*aSSRC */)    
	{
	}
//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CRtpManager,
    "CRtpManager",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "CRtpManager",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_NewLL, Teardown)

EUNIT_TEST(
    "OpenL - test ",
    "CRtpManager",
    "OpenL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_OpenLL, Teardown)

EUNIT_TEST(
    "StartConnection - test ",
    "CRtpManager",
    "StartConnection",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_StartConnectionL, Teardown)

EUNIT_TEST(
    "StartConnection1 - test ",
    "CRtpManager",
    "StartConnection 1",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_StartConnection_OneL, Teardown)


EUNIT_TEST(
    "StartConnection2 - test ",
    "CRtpManager",
    "StartConnection2",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_StartConnection_TwoL, Teardown)

EUNIT_TEST(
    "CancelStart - test ",
    "CRtpManager",
    "CancelStart",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_CancelStartL, Teardown)

EUNIT_TEST(
    "Close - test ",
    "CRtpManager",
    "Close",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_CloseL, Teardown)

EUNIT_TEST(
    "SetLocalSdes - test ",
    "CRtpManager",
    "SetLocalSdes",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_SetLocalSdesL, Teardown)

EUNIT_TEST(
    "GetLocalIPAddr1 - test ",
    "CRtpManager",
    "GetLocalIPAddressL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_GetLocalIPAddressL1L, Teardown)
    
EUNIT_TEST(
    "GetLocalIPAddr2 - test ",
    "CRtpManager",
    "GetLocalIPAddressL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_GetLocalIPAddressL2L, Teardown)    

EUNIT_TEST(
    "CreateSessionL - test ",
    "CRtpManager",
    "CreateSessionL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_CreateSessionLL, Teardown)

EUNIT_TEST(
    "StartSession - test ",
    "CRtpManager",
    "StartSession",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_StartSessionL, Teardown)

EUNIT_TEST(
    "CloseSession - test ",
    "CRtpManager",
    "CloseSession",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_CloseSessionL, Teardown)

EUNIT_TEST(
    "SetRemoteAddress - test ",
    "CRtpManager",
    "SetRemoteAddress",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_SetRemoteAddressL, Teardown)

EUNIT_TEST(
    "SetRemoteRtcpAddress - test ",
    "CRtpManager",
    "SetRemoteRtcpAddress",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_SetRemoteRtcpAddressL, Teardown)

EUNIT_TEST(
    "CreateStreamL - test ",
    "CRtpManager",
    "CreateStreamL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_CreateStreamLL, Teardown)

EUNIT_TEST(
    "CreateReceiveStreamL - test ",
    "CRtpManager",
    "CreateReceiveStreamL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_CreateReceiveStreamLL, Teardown)

EUNIT_TEST(
    "CreateTransmitStreamL - test ",
    "CRtpManager",
    "CreateTransmitStreamL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_CreateTransmitStreamLL, Teardown)

EUNIT_TEST(
    "CreateTransmitStreamExtL - test ",
    "CRtpManager",
    "CreateTransmitStreamExtL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_CreateTransmitStreamExtLL, Teardown)

EUNIT_TEST(
    "CloseStream - test ",
    "CRtpManager",
    "CloseStream",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_CloseStreamL, Teardown)

EUNIT_TEST(
    "RegisterRtpObserver - test ",
    "CRtpManager",
    "RegisterRtpObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_RegisterRtpObserverL, Teardown)

EUNIT_TEST(
    "UnregisterRtpObserver - test ",
    "CRtpManager",
    "UnregisterRtpObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_UnregisterRtpObserverL, Teardown)

EUNIT_TEST(
    "SetNonRTPDataObserver - test ",
    "CRtpManager",
    "SetNonRTPDataObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_SetNonRTPDataObserverL, Teardown)

EUNIT_TEST(
    "SendRtpPacket - test ",
    "CRtpManager",
    "SendRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_SendRtpPacketL, Teardown)

EUNIT_TEST(
    "SendRtpPacket - test ",
    "CRtpManager",
    "SendRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_SendRtpPacket_1L, Teardown)

EUNIT_TEST(
    "SendRtpPacket - test ",
    "CRtpManager",
    "SendRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_SendRtpPacket_2L, Teardown)

EUNIT_TEST(
    "SendDataL - test ",
    "CRtpManager",
    "SendDataL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_SendDataLL, Teardown)

EUNIT_TEST(
    "CancelSend - test ",
    "CRtpManager",
    "CancelSend",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_CancelSendL, Teardown)

EUNIT_TEST(
    "RegisterRtcpObserver - test ",
    "CRtpManager",
    "RegisterRtcpObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_RegisterRtcpObserverL, Teardown)

EUNIT_TEST(
    "UnregisterRtcpObserver - test ",
    "CRtpManager",
    "UnregisterRtcpObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_UnregisterRtcpObserverL, Teardown)

EUNIT_TEST(
    "SendRtcpByePacket - test ",
    "CRtpManager",
    "SendRtcpByePacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_SendRtcpByePacketL, Teardown)

EUNIT_TEST(
    "SendRtcpAppPacket - test ",
    "CRtpManager",
    "SendRtcpAppPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_SendRtcpAppPacketL, Teardown)

EUNIT_TEST(
    "SendRtcpRrPacket - test ",
    "CRtpManager",
    "SendRtcpRrPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_SendRtcpRrPacketL, Teardown)

EUNIT_TEST(
    "SendRtcpSrPacket - test ",
    "CRtpManager",
    "SendRtcpSrPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_SendRtcpSrPacketL, Teardown)

EUNIT_TEST(
    "SuspendRtcpSending - test ",
    "CRtpManager",
    "SuspendRtcpSending",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_SuspendRtcpSendingL, Teardown)

EUNIT_TEST(
    "IsRtcpSendingSuspended - test ",
    "CRtpManager",
    "IsRtcpSendingSuspended",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_IsRtcpSendingSuspendedL, Teardown)

EUNIT_TEST(
    "GetSessionId - test ",
    "CRtpManager",
    "GetSessionId",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_GetSessionIdL, Teardown)

EUNIT_TEST(
    "GetRtpSocket - test ",
    "CRtpManager",
    "GetRtpSocket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_GetRtpSocketL, Teardown)

EUNIT_TEST(
    "GetRtcpSocket - test ",
    "CRtpManager",
    "GetRtcpSocket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_GetRtcpSocketL, Teardown)

EUNIT_TEST(
    "GetStreamStatistics - test ",
    "CRtpManager",
    "GetStreamStatistics",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_GetStreamStatisticsL, Teardown)

EUNIT_TEST(
    "GetSamplingRate - test ",
    "CRtpManager",
    "GetSamplingRate",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_GetSamplingRateL, Teardown)

EUNIT_TEST(
    "SetSamplingRate - test ",
    "CRtpManager",
    "SetSamplingRate",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_SetSamplingRateL, Teardown)

EUNIT_TEST(
    "SetRtcpParameters - test ",
    "CRtpManager",
    "SetRtcpParameters",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_SetRtcpParametersL, Teardown)

EUNIT_TEST(
    "RemoveRtpObjectL ",
    "CRtpManager",
    "RemoveRtpObjectL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_RemoveRtpObjectLL, Teardown)

EUNIT_TEST(
    "GetIAPIdL ",
    "CRtpManager",
    "GetIAPIdL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_GetIAPIdL, Teardown)
EUNIT_TEST(
    "AddStreamToSession",
    "CRtpManager",
    "AddStreamToSession",
    "FUNCTIONALITY",
    SetupL, UT_CRtpManager_AddStreamToSessionL, Teardown)
EUNIT_END_TEST_TABLE

//  END OF FILE
