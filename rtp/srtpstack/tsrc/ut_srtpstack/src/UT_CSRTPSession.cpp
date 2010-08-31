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
#include "UT_CSRTPSession.h"


//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include "srtpcryptohandlersrtp.h"
#include "srtpcryptohandlersrtcp.h"
#include "srtpmastersalt.h"
#include "srtpmasterkey.h"
#include "srtpstreamin.h"
#include "srtpstreamout.h"
#include "srtpsession.h"
#include "srtputils.h"

_LIT8(KRFC3711_TestMasterKey128bits,    "E1F97A0D3E018BE0D64FA32C06DE4139");
_LIT8(KRFC3711_TestMasterSalt112bits,   "0EC675AD498AFEEBB6960B3AABE6");	
_LIT8(KTestMKI128Bits,      			"ABCDEF1234567890ABCDEF1234567890");

#define RTP_EUNIT_ASSERT_SPECIFIC_LEAVE( func, val ) \
{\
TRAPD( rtpUnitTestRetVal, func );\
if ( val != KErrNoMemory && rtpUnitTestRetVal == KErrNoMemory ){\
	User::Leave( KErrNoMemory );\
	}\
else{\
	EUNIT_ASSERT_EQUALS( rtpUnitTestRetVal, val );\
	}\
}


//  INTERNAL INCLUDES
#include "srtpsession.h"
// CONSTANTS
_LIT8(KDummyRTCPPacket, "dummyrtcp");
// CONSTRUCTION
UT_CSRTPSession* UT_CSRTPSession::NewL()
    {
    UT_CSRTPSession* self = UT_CSRTPSession::NewLC(); 
    CleanupStack::Pop();

    return self;
    }

UT_CSRTPSession* UT_CSRTPSession::NewLC()
    {
    UT_CSRTPSession* self = new( ELeave ) UT_CSRTPSession();
    CleanupStack::PushL( self );

	self->ConstructL(); 

    return self;
    }

// Destructor (virtual by CBase)
UT_CSRTPSession::~UT_CSRTPSession()
    {
    }

// Default constructor
UT_CSRTPSession::UT_CSRTPSession()
    {
    }

// Second phase construct
void UT_CSRTPSession::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }
void UT_CSRTPSession::SRTPMasterKeyStaleEvent(const CSRTPStream& /*aStream*/ )
    {
    }
void UT_CSRTPSession::SRTPMasterKeyStaleEvent(const CSRTPSession& /*aSession*/ )
    {
    }


void UT_CSRTPSession::Hex(HBufC8& aString)
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
//  METHODS


void UT_CSRTPSession::SetupL(  )
    {
    HBufC8* masterKey = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = KRFC3711_TestMasterKey128bits;

    
    
    HBufC8* masterSalt =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFC3711_TestMasterSalt112bits;

	
	iMasterKey = CSRTPMasterKey::NewL( *masterKey, _L8("") );
    iMasterSalt = CSRTPMasterSalt::NewL( *masterSalt );
    
    TSrtpCryptoParams params;
    
    iContext = CSRTPCryptoContext::NewL(iMasterKey, iMasterSalt, params );
    
    CleanupStack::PopAndDestroy( masterSalt );
    CleanupStack::PopAndDestroy( masterKey ); 
    iDestination.Input(_L("127.0.0.1"));
    } 

void UT_CSRTPSession::Teardown(  )
    {
    delete iContext;
    iContext=NULL;
    }

void UT_CSRTPSession::UT_CSRTPSession_NewL_OneL(  )
    {
    CSRTPSession* srtpSession = CSRTPSession::NewL( iDestination); 
    EUNIT_ASSERT(srtpSession);
    delete srtpSession;
    }

void UT_CSRTPSession::UT_CSRTPSession_NewL_TwoL(  )
    {
                            
    CSRTPSession* srtpSession = CSRTPSession::NewL(iDestination,
    												iContext,
    												*this); 
    iContext=NULL; // Ownership was transferred
    
    EUNIT_ASSERT(srtpSession);	
    
    delete srtpSession;       
         
    }

void UT_CSRTPSession::UT_CSRTPSession_ConstructLL(  )
    {         
    delete iContext; iContext=0;
    CSRTPSession* srtpSession;
    RTP_EUNIT_ASSERT_SPECIFIC_LEAVE( srtpSession= CSRTPSession::NewLC(iDestination,
    												iContext,
    												*this), 
    							KErrArgument); 
    						
    }

void UT_CSRTPSession::UT_CSRTPSession_RemoveStreamL(  )
    {
    
    CSRTPSession* srtpSession = CSRTPSession::NewLC(iDestination,
    												iContext,
    												*this); 
    iContext=NULL; // Ownership was transferred
    
    //automatically added stream in the constructor												
    CSRTPStreamIn* tempStreamIn = CSRTPStreamIn::NewL(*srtpSession, (TUint)1);
    CleanupStack::PushL( tempStreamIn );
    
	EUNIT_ASSERT(srtpSession->iStreamList.First());
	srtpSession->RemoveStreamFromList(tempStreamIn);
	// Will not found stream so just not remove it
	srtpSession->RemoveStreamFromList(tempStreamIn);


    CleanupStack::PopAndDestroy( tempStreamIn );
    CleanupStack::PopAndDestroy( srtpSession );
   
    }

void UT_CSRTPSession::UT_CSRTPSession_GetCryptoContextL(  )
    {
 
    CSRTPSession* srtpSession = CSRTPSession::NewLC(iDestination,
    												iContext,
    												*this); 
    iContext=NULL; // Ownership was transferred
    
    EUNIT_ASSERT(srtpSession->iSesssionCrypto);
    CleanupStack::PopAndDestroy( srtpSession );												
    }
    
void UT_CSRTPSession::UT_CSRTPSession_UpdateCryptoContextLL()
	{
	//Test case 1 :set crypto context in session
	HBufC8* masterKey = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = KRFC3711_TestMasterKey128bits;
    
    HBufC8* masterSalt =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFC3711_TestMasterSalt112bits;

	
	CSRTPMasterKey* mKey = CSRTPMasterKey::NewLC( *masterKey, _L8("") );
    CSRTPMasterSalt* mSalt = CSRTPMasterSalt::NewLC( *masterSalt );
    
    TSrtpCryptoParams params;
    
    CSRTPCryptoContext* context = CSRTPCryptoContext::NewL(mKey, mSalt, params );
    CleanupStack::Pop( mSalt );
    CleanupStack::Pop( mKey );
    CleanupStack::PopAndDestroy( masterSalt );
    CleanupStack::PopAndDestroy( masterKey ); 
    CleanupStack::PushL( context );
    CSRTPSession* srtpSession = CSRTPSession::NewL(iDestination,
    												iContext,
    												*this); 
	iContext=NULL; // Ownership was transferred
	
	CleanupStack::PushL( srtpSession );
	srtpSession->SetCryptoContextL(context);
	CleanupStack::Pop( srtpSession );
	CleanupStack::Pop( context );
	CleanupStack::PushL( srtpSession );
	
	//Test cases 2: Create stream and then set the crypto context again
	CSRTPStreamIn* streamIn =CSRTPStreamIn::NewL( *srtpSession,
                                      (TUint)1,
                                      *this );
    CleanupStack::PushL( streamIn );
    
	CSRTPStreamOut* streamOut = CSRTPStreamOut::NewL(*srtpSession, (TUint)1);
	CleanupStack::PushL( streamOut );
	HBufC8* masterKey2 = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey2 );
    *masterKey2 = KRFC3711_TestMasterKey128bits;
    
    HBufC8* masterSalt2 =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt2);
	*masterSalt2 = KRFC3711_TestMasterSalt112bits;

	
	CSRTPMasterKey* mKey2 = CSRTPMasterKey::NewLC( *masterKey2, _L8("") );
    CSRTPMasterSalt* mSalt2 = CSRTPMasterSalt::NewLC( *masterSalt2 );
    
    //TSrtpCryptoParams params;
    
    CSRTPCryptoContext* context2 = CSRTPCryptoContext::NewL(mKey2, mSalt2, params );
    CleanupStack::Pop( mSalt2 );
    CleanupStack::Pop( mKey2 );
    CleanupStack::PopAndDestroy( masterSalt2 );
    CleanupStack::PopAndDestroy( masterKey2 ); 
    CleanupStack::PushL( context2 );
	srtpSession->SetCryptoContextL(context2);
	CleanupStack::Pop( context2 );
	
	// Test case 4: create stream with specdific context
	HBufC8* masterKey3 = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey3 );
    *masterKey3 = KRFC3711_TestMasterKey128bits;
    
    HBufC8* masterSalt3 =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt3);
	*masterSalt3 = KRFC3711_TestMasterSalt112bits;

	
	CSRTPMasterKey* mKey3 = CSRTPMasterKey::NewLC( *masterKey3, _L8("") );
    CSRTPMasterSalt* mSalt3 = CSRTPMasterSalt::NewLC( *masterSalt3 );
    
    //TSrtpCryptoParams params;
	CSRTPCryptoContext* context3 = CSRTPCryptoContext::NewL(mKey3, mSalt3, params );
	CleanupStack::Pop( mSalt3 );
    CleanupStack::Pop( mKey3 );
    CleanupStack::PopAndDestroy( masterSalt3 );
    CleanupStack::PopAndDestroy( masterKey3 ); 
    CleanupStack::PushL( context3 );
	HBufC8* masterKey4 = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey4 );
    *masterKey4 = KRFC3711_TestMasterKey128bits;
    
    HBufC8* masterSalt4 =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt4);
	*masterSalt4 = KRFC3711_TestMasterSalt112bits;

	
	CSRTPMasterKey* mKey4 = CSRTPMasterKey::NewLC( *masterKey4, _L8("") );
    CSRTPMasterSalt* mSalt4 = CSRTPMasterSalt::NewLC( *masterSalt4 );
    
    //TSrtpCryptoParams params;
	CSRTPCryptoContext* context4 = CSRTPCryptoContext::NewL(mKey4, mSalt4, params );
	CleanupStack::Pop( mSalt4 );
    CleanupStack::Pop( mKey4 );
    CleanupStack::PopAndDestroy( masterSalt4 );
    CleanupStack::PopAndDestroy( masterKey4); 
    CleanupStack::PushL( context4 );
	CSRTPStreamIn* streamIn2 =CSRTPStreamIn::NewL( *srtpSession,
                                      context3,
                                      *this );
    CleanupStack::Pop( context4 );
    CleanupStack::Pop( context3 );
    CleanupStack::PushL( streamIn2 );
    CleanupStack::PushL( context4 );
	CSRTPStreamOut* streamOut2 = CSRTPStreamOut::NewL(*srtpSession, (TUint)2, context4, *this);
	CleanupStack::Pop( context4 );
	CleanupStack::PushL( streamOut2 );
	
	HBufC8* masterKey5 = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey5 );
    *masterKey5 = KRFC3711_TestMasterKey128bits;
    
    HBufC8* masterSalt5 =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt5);
	*masterSalt5 = KRFC3711_TestMasterSalt112bits;

	
	CSRTPMasterKey* mKey5 = CSRTPMasterKey::NewLC( *masterKey5, _L8("") );
    CSRTPMasterSalt* mSalt5 = CSRTPMasterSalt::NewLC( *masterSalt5 );
    
    //TSrtpCryptoParams params;
	CSRTPCryptoContext* context5 = CSRTPCryptoContext::NewL(mKey5, mSalt5, params );
	CleanupStack::Pop( mSalt5 );
    CleanupStack::Pop( mKey5 );
    CleanupStack::PopAndDestroy( masterSalt5 );
    CleanupStack::PopAndDestroy( masterKey5 ); 
    CleanupStack::PushL( context5 );
    srtpSession->SetCryptoContextL(context5);
	CleanupStack::Pop( context5 );
	
	//Test case 5: Set the context=NULL			
	CSRTPCryptoContext* context6=NULL;
	EUNIT_ASSERT_SPECIFIC_LEAVE(srtpSession->SetCryptoContextL(context6), KErrArgument);
	
	CleanupStack::PopAndDestroy( streamOut2 );
	CleanupStack::PopAndDestroy( streamIn2 );
	CleanupStack::PopAndDestroy( streamOut );
	CleanupStack::PopAndDestroy( streamIn );
	CleanupStack::PopAndDestroy( srtpSession );
	}
    
void UT_CSRTPSession::UT_CSRTPSession_RemoteAddrL()
	{
	CSRTPSession* srtpSession = CSRTPSession::NewL(iDestination,
    												iContext,
    												*this); 
	iContext=NULL; // Ownership was transferred
	
	TInetAddr tempaddr =srtpSession->RemoteAddr();
	EUNIT_ASSERT(tempaddr==iDestination);
	delete srtpSession;
	}
	
void UT_CSRTPSession::UT_CSRTPSession_StreamLL()
	{
    CSRTPSession* srtpSession = CSRTPSession::NewLC(iDestination,
    												iContext,
    												*this); 
    iContext=NULL; // Ownership was transferred
    
    //automatically added stream in the constructor												
    CSRTPStreamIn* tempStreamIn = CSRTPStreamIn::NewL(*srtpSession, (TUint)1);
    CleanupStack::PushL( tempStreamIn );
    
	CSRTPStream* stream= &(srtpSession->StreamL(1, ETrue));
	EUNIT_ASSERT(stream!=NULL);
	EUNIT_ASSERT_SPECIFIC_LEAVE(srtpSession->StreamL(2, ETrue), KErrNotFound);

    CleanupStack::PopAndDestroy( tempStreamIn );
    CleanupStack::PopAndDestroy( srtpSession );
	
	}
void UT_CSRTPSession::UT_CSRTPSession_ProtectRTPLL()
	{

    CSRTPSession* srtpSession = CSRTPSession::NewLC(iDestination,
    												iContext,
    												*this); 
    iContext=NULL; // Ownership was transferred
    
    //automatically added stream in the constructor												
    CSRTPStreamOut* tempStreamOut = CSRTPStreamOut::NewL(*srtpSession, (TUint)1);
    CleanupStack::PushL( tempStreamOut );
    //will probably panic becasuse packet is not built properly
	RTP_EUNIT_ASSERT_SPECIFIC_LEAVE( srtpSession->ProtectRTPL(1, KDummyRTCPPacket()), 
							KErrCorrupt)
				

    CleanupStack::PopAndDestroy( tempStreamOut );
    CleanupStack::PopAndDestroy( srtpSession );	
	}

void UT_CSRTPSession::UT_CSRTPSession_UnprotectRTPLL()
	{

    CSRTPSession* srtpSession = CSRTPSession::NewLC(iDestination,
    												iContext,
    												*this); 
    iContext=NULL; // Ownership was transferred
    
    //automatically added stream in the constructor												
    CSRTPStreamIn* tempStreamIn = CSRTPStreamIn::NewL(*srtpSession, (TUint)1);
    CleanupStack::PushL( tempStreamIn );
    //will probably panic
	
	RTP_EUNIT_ASSERT_SPECIFIC_LEAVE( srtpSession->UnprotectRTPL(1, KDummyRTCPPacket()), 
							KErrCorrupt)
	

    CleanupStack::PopAndDestroy( tempStreamIn );
    CleanupStack::PopAndDestroy( srtpSession );		
	}
void UT_CSRTPSession::UT_CSRTPSession_UnprotectRTPL2L()
	{
    CSRTPSession* srtpSession = CSRTPSession::NewLC(iDestination,
    												iContext,
    												*this); 

    iContext=NULL; // Ownership was transferred
    
	CSRTPStreamIn* tempStreamIn2 = CSRTPStreamIn::NewL(*srtpSession, (TUint)0);	
	CleanupStack::PushL( tempStreamIn2 );
	//will probably panic
	    //will probably panic becasuse packet is not built properly
	RTP_EUNIT_ASSERT_SPECIFIC_LEAVE( srtpSession->UnprotectRTPL(2, KDummyRTCPPacket()), 
							KErrCorrupt)
	
	

    CleanupStack::PopAndDestroy( tempStreamIn2 );
    CleanupStack::PopAndDestroy( srtpSession );			
	}	
void UT_CSRTPSession::UT_CSRTPSession_ProtectRTCPLL()
	{

    CSRTPSession* srtpSession = CSRTPSession::NewLC(iDestination,
    												iContext,
    												*this); 
    iContext=NULL; // Ownership was transferred
    
    //automatically added stream in the constructor												
    CSRTPStreamOut* tempStreamOut = CSRTPStreamOut::NewL(*srtpSession, (TUint)1);
    CleanupStack::PushL( tempStreamOut );
    //will probably panic becasuse packet is not built properly
	RTP_EUNIT_ASSERT_SPECIFIC_LEAVE( srtpSession->ProtectRTCPL(1, KDummyRTCPPacket()), 
							KErrCorrupt)
				

    CleanupStack::PopAndDestroy( tempStreamOut );
    CleanupStack::PopAndDestroy( srtpSession );	
	}

void UT_CSRTPSession::UT_CSRTPSession_UnprotectRTCPLL()
	{

    CSRTPSession* srtpSession = CSRTPSession::NewLC(iDestination,
    												iContext,
    												*this); 
    iContext=NULL; // Ownership was transferred
    
    //automatically added stream in the constructor												
    CSRTPStreamIn* tempStreamIn = CSRTPStreamIn::NewL(*srtpSession, (TUint)1);
    CleanupStack::PushL( tempStreamIn );
    //will probably panic
	
	RTP_EUNIT_ASSERT_SPECIFIC_LEAVE( srtpSession->UnprotectRTCPL(1, KDummyRTCPPacket()), 
							KErrCorrupt)
	

    CleanupStack::PopAndDestroy( tempStreamIn );
    CleanupStack::PopAndDestroy( srtpSession );			
	}	

void UT_CSRTPSession::UT_CSRTPSession_UnprotectRTCPL2L()
	{
    CSRTPSession* srtpSession = CSRTPSession::NewLC(iDestination,
    												iContext,
    												*this); 
    iContext=NULL; // Ownership was transferred
    
	CSRTPStreamIn* tempStreamIn2 = CSRTPStreamIn::NewL(*srtpSession, (TUint)0);	
	CleanupStack::PushL( tempStreamIn2 );
	//will probably panic
	    //will probably panic becasuse packet is not built properly
	RTP_EUNIT_ASSERT_SPECIFIC_LEAVE( srtpSession->UnprotectRTCPL(2, KDummyRTCPPacket()), 
							KErrCorrupt)
	
	

    CleanupStack::PopAndDestroy( tempStreamIn2 );
    CleanupStack::PopAndDestroy( srtpSession );				
	}		
	
void UT_CSRTPSession::UT_CSRTPSession_FindStreamL()
	{
    CSRTPSession* srtpSession = CSRTPSession::NewLC(iDestination,
    												iContext,
    												*this); 
    iContext=NULL; // Ownership was transferred
    
    //automatically added stream in the constructor												
    CSRTPStreamIn* tempStreamIn = CSRTPStreamIn::NewL(*srtpSession, (TUint)1);
    CleanupStack::PushL( tempStreamIn );
    
	EUNIT_ASSERT(srtpSession->FindStream(1, ETrue));
	EUNIT_ASSERT(srtpSession->FindStream(22, ETrue)==EFalse);
	
    CleanupStack::PopAndDestroy( tempStreamIn );
    CleanupStack::PopAndDestroy( srtpSession );		
	}
	
void UT_CSRTPSession::FindLateBindingStreamAndUnprotectRTPLL()
	{
    CSRTPSession* srtpSession = CSRTPSession::NewLC(iDestination,
    												iContext,
    												*this); 
    iContext=NULL; // Ownership was transferred
    
    EUNIT_ASSERT_SPECIFIC_LEAVE(srtpSession->
    	FindLateBindingStreamAndUnprotectRTPL(0,KDummyRTCPPacket()), KErrCorrupt );
    
    CSRTPStreamIn* tempStreamIn = 
    		CSRTPStreamIn::NewL(*srtpSession, (TUint)1);	
    CleanupStack::PushL( tempStreamIn );
    tempStreamIn->ChangeRTPState(MSRTPStreamInContext::ESRTPStreamInNormal);
    
    EUNIT_ASSERT_SPECIFIC_LEAVE(srtpSession->
    	FindLateBindingStreamAndUnprotectRTPL(1,KDummyRTCPPacket()), KErrCorrupt );
    		
    CleanupStack::PopAndDestroy( tempStreamIn );
    CleanupStack::PopAndDestroy( srtpSession );		
	}	
	
void UT_CSRTPSession::FindLateBindingStreamAndUnprotectRTCPLL()
	{
    CSRTPSession* srtpSession = CSRTPSession::NewLC(iDestination,
    												iContext,
    												*this); 
    iContext=NULL; // Ownership was transferred
    
    EUNIT_ASSERT_SPECIFIC_LEAVE(srtpSession->
    	FindLateBindingStreamAndUnprotectRTCPL(0,KDummyRTCPPacket()), KErrCorrupt );
    
    CSRTPStreamIn* tempStreamIn = 
    		CSRTPStreamIn::NewL(*srtpSession, (TUint)1);	
    CleanupStack::PushL( tempStreamIn );
    
    tempStreamIn->ChangeRTCPState(MSRTPStreamInContext::ESRTPStreamInNormal);
    
    EUNIT_ASSERT_SPECIFIC_LEAVE(srtpSession->
    	FindLateBindingStreamAndUnprotectRTCPL(1,KDummyRTCPPacket()), KErrCorrupt );
    
    //Test: 2
	tempStreamIn->ChangeRTCPState(MSRTPStreamInContext::ESRTPStreamInLateBinding);
	tempStreamIn->ChangeRTPState(MSRTPStreamInContext::ESRTPStreamInNormal);
    
    RTP_EUNIT_ASSERT_SPECIFIC_LEAVE(srtpSession->
    	FindLateBindingStreamAndUnprotectRTCPL(2,KDummyRTCPPacket()), KErrCorrupt );
    	
    	
	//Test 2a    	
	tempStreamIn->ChangeRTCPState(MSRTPStreamInContext::ESRTPStreamInLateBinding);
	tempStreamIn->ChangeRTPState(MSRTPStreamInContext::ESRTPStreamInNormal);
    
    RTP_EUNIT_ASSERT_SPECIFIC_LEAVE(srtpSession->
    	FindLateBindingStreamAndUnprotectRTCPL(1,KDummyRTCPPacket()), KErrCorrupt );
    
    CleanupStack::PopAndDestroy( tempStreamIn );
    	
	
	//Test 3: Check where there is no InSTream and RTCP Packet is received
	
	CSRTPStreamOut* streamOut = CSRTPStreamOut::NewL(*srtpSession, (TUint)1);
	CleanupStack::PushL( streamOut );
    
    RTP_EUNIT_ASSERT_SPECIFIC_LEAVE(srtpSession->
    	FindLateBindingStreamAndUnprotectRTCPL(1,KDummyRTCPPacket()), KErrCorrupt );

    CleanupStack::PopAndDestroy( streamOut );		
    CleanupStack::PopAndDestroy( srtpSession );	
	}
	
void UT_CSRTPSession::UT_CSRTPSession_RemoveAllStreamL()	
	{
	CSRTPSession* srtpSession = CSRTPSession::NewL(iDestination,
    												iContext,
    												*this); 
    iContext=NULL; // Ownership was transferred
    
    srtpSession->RemoveAllStream( );
    delete srtpSession;												
	}

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE( 
    UT_CSRTPSession,
    "CSRTPSession Class",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "CSRTPSession",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPSession_NewL_OneL, Teardown)

EUNIT_TEST(
    "NewL - test ",
    "CSRTPSession",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPSession_NewL_TwoL, Teardown)
EUNIT_TEST(
    "ConstructL ",
    "CSRTPSession",
    "ConstructL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPSession_ConstructLL, Teardown)

EUNIT_TEST(
    "RemoveStream - test ",
    "CSRTPSession",
    "RemoveStream",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPSession_RemoveStreamL, Teardown)

EUNIT_TEST(
    "GetCryptoContext - test ",
    "CSRTPSession",
    "GetCryptoContext",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPSession_GetCryptoContextL, Teardown)

EUNIT_TEST(
    "StreamLL ",
    "CSRTPSession",
    "StreamLL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPSession_StreamLL, Teardown)

EUNIT_TEST(
    "ProtectRTPLL ",
    "CSRTPSession",
    "ProtectRTPLL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPSession_ProtectRTPLL, Teardown)

EUNIT_TEST(
    "unintial UnprotectRTPL",
    "CSRTPSession",
    "UnprotectRTPLL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPSession_UnprotectRTPLL, Teardown)

EUNIT_TEST(
    "latebinding UnprotectRTPL ",
    "CSRTPSession",
    "UnprotectRTPL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPSession_UnprotectRTPL2L, Teardown)
    
EUNIT_TEST(
    "ProtectRTCPLL ",
    "CSRTPSession",
    "ProtectRTCPLL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPSession_ProtectRTCPLL, Teardown)

EUNIT_TEST(
    "UnprotectRTCPL",
    "CSRTPSession",
    "UnprotectRTCPLL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPSession_UnprotectRTCPLL, Teardown)


EUNIT_TEST(
    "FindStreamL - test ",
    "CSRTPSession",
    "FindStreamL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPSession_FindStreamL, Teardown)

EUNIT_TEST(
    "FindLateBindingStreamL - test ",
    "CSRTPSession",
    "FindStreamL",
    "FUNCTIONALITY",
    SetupL, FindLateBindingStreamAndUnprotectRTPLL, Teardown)

EUNIT_TEST(
    "UpdateCryptoContext",
    "CSRTPSession",
    "UpdateCryptoContext",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPSession_UpdateCryptoContextLL, Teardown)
    
EUNIT_TEST(
    "RemoteAddr",
    "CSRTPSession",
    "RemoteAddr",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPSession_RemoteAddrL, Teardown)    

EUNIT_TEST(
    "UnprotectRTCPL2L",
    "CSRTPSession",
    "UnprotectRTCPL2L",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPSession_UnprotectRTCPL2L, Teardown)    
    
EUNIT_TEST(
    "FindLBUnprotectRTCPLL",
    "CSRTPSession",
    "FindLBUnprotectRTCPLL",
    "FUNCTIONALITY",
    SetupL, FindLateBindingStreamAndUnprotectRTCPLL, Teardown)        

EUNIT_TEST(
    "RemoveAllStreamL",
    "CSRTPSession",
    "RemoveAllStreamL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPSession_RemoveAllStreamL, Teardown)        

EUNIT_TEST(
    "UpdateCryptoContextLL",
    "CSRTPSession",
    "UpdateCryptoContextLL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPSession_UpdateCryptoContextLL, Teardown)        


EUNIT_END_TEST_TABLE

//  END OF FILE
