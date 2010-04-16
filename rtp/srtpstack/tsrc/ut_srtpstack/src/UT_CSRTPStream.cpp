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
#include "UT_CSRTPStream.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "srtpstream.h"
#include "srtpcryptohandlersrtp.h"
#include "srtpcryptohandlersrtcp.h"
#include "srtpmastersalt.h"
#include "srtpmasterkey.h"
#include "srtpstreamin.h"
#include "srtpstreamout.h"
#include "srtpsession.h"
#include "srtputils.h"
#include "srtpcryptoparams.h"

// CONSTANTS
const TInt KRtpVersion = 2;    // Current protocol version          
const TUint KMinRtpHeaderLen( 12 );
const TUint16 KTestSeqNr17 = 17171;
const TInt KLength = 50;

// 80 bit key, test 1
_LIT8(KTestPayload160Bits,  "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

// 80 bit key, test 1
//_LIT8(KTestMKI128Bits,      "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB");
_LIT8(KTestMKI128Bits,      "ABCDEF1234567890ABCDEF1234567890");

// 80 bit key, test 1
_LIT8(KTestAuthTag80Bits,   "CCCCCCCCCCCCCCCCCCCC");

_LIT8(KRFC3711_TestMasterKey128bits,    "E1F97A0D3E018BE0D64FA32C06DE4139");
_LIT8(KRFC3711_TestMasterSalt112bits,   "0EC675AD498AFEEBB6960B3AABE6");	
_LIT8(KRFC3711_SessionEncrKey128bits,   "C61E7A93744F39EE10734AFE3FF7A087");  
_LIT8(KRFC3711_SessionSaltKey128bits,   "30CBBC08863D8C85D49DB34A9AE1");  
_LIT8(KRFC3711_SessionAuthKey128bits,   "CEBE321F6FF7716B6FD4AB49AF256A156D38BAA4");  
_LIT8(KDecryptedPayload,                "2EDE3286C6E33E68C6BB93350620A1245EC0004F");  

_LIT8(KTestM_without_ROC,               "123456789ABCDE");  
_LIT8(KRefM_with_ROC,                   "123456789ABCDEFFFFFFFF");  

_LIT8(KTestAuthTag,                     "D07919ABC9F235CB4516");  
_LIT8(KDummyRTCPPacket, "dummyrtcp");


// CONSTRUCTION
UT_CSRTPStream* UT_CSRTPStream::NewL()
    {
    UT_CSRTPStream* self = UT_CSRTPStream::NewLC(); 
    CleanupStack::Pop();

    return self;
    }

UT_CSRTPStream* UT_CSRTPStream::NewLC()
    {
    UT_CSRTPStream* self = new( ELeave ) UT_CSRTPStream();
    CleanupStack::PushL( self );

	self->ConstructL(); 

    return self;
    }

// Destructor (virtual by CBase)
UT_CSRTPStream::~UT_CSRTPStream()
    {
    }

// Default constructor
UT_CSRTPStream::UT_CSRTPStream()
    {
    }
 
UT_CSRTPStream::TPayloadDetails::TPayloadDetails() :
        seqNr(),
//      ssrc(),
        padding(0),
        numCSRC(0),
        fHeaderExtension(EFalse),
        payloadLen(0),
        payloadData(NULL),
        MKILen(0),
        MKIData(NULL),
        authTagLen(0),
        authTagData(NULL)
            {            
            }    
            

// Second phase construct
void UT_CSRTPStream::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

void UT_CSRTPStream::Hex(HBufC8& aString)
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

void UT_CSRTPStream::SRTPMasterKeyStaleEvent(const CSRTPStream& /*aStream*/)
    {
    iReKeyCalled= ETrue;
    }
void UT_CSRTPStream::SRTPMasterKeyStaleEvent(const CSRTPSession& /*aSession*/ )
    {
    iReKeyCalled= ETrue;
    }


void UT_CSRTPStream::SetupL(  )
    {
 
   iDecryptedPayload = HBufC8::NewL(KDecryptedPayload().Length());
	*iDecryptedPayload = KDecryptedPayload;
	Hex(*iDecryptedPayload);

    iRFC3711_SessionEncrKey128bits = HBufC8::NewL(KRFC3711_SessionEncrKey128bits().Length());
	*iRFC3711_SessionEncrKey128bits = KRFC3711_SessionEncrKey128bits;
	Hex(*iRFC3711_SessionEncrKey128bits);

    iRFC3711_SessionSaltKey128bits = HBufC8::NewL(KRFC3711_SessionSaltKey128bits().Length());
	*iRFC3711_SessionSaltKey128bits = KRFC3711_SessionSaltKey128bits;
	Hex(*iRFC3711_SessionSaltKey128bits);

    iRFC3711_SessionAuthKey128bits = HBufC8::NewL(KRFC3711_SessionAuthKey128bits().Length());
	*iRFC3711_SessionAuthKey128bits = KRFC3711_SessionAuthKey128bits;
	Hex(*iRFC3711_SessionAuthKey128bits);
	
    
	iTestPayload160Bits = HBufC8::NewL(KTestPayload160Bits().Length());
	*iTestPayload160Bits = KTestPayload160Bits;	
	Hex(*iTestPayload160Bits);	
    
	iTestMKI128Bits = HBufC8::NewL(KTestMKI128Bits().Length());
	*iTestMKI128Bits = KTestMKI128Bits;	
	Hex(*iTestMKI128Bits);	

	iTestAuthTag80Bits = HBufC8::NewL(KTestAuthTag80Bits().Length());
	*iTestAuthTag80Bits = KTestAuthTag80Bits;	
	Hex(*iTestAuthTag80Bits);	
		


    // our test MKI is 16 bit long:
    // ABCDEF1234567890ABCDEF1234567890
   
	HBufC8* masterKey = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = KRFC3711_TestMasterKey128bits;
	
    
    HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KRFC3711_TestMasterSalt112bits;
	Hex(*mki);
	
    HBufC8* masterSalt =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFC3711_TestMasterSalt112bits;

	
	iMasterKey = CSRTPMasterKey::NewL( *masterKey, *mki );
    iMasterSalt = CSRTPMasterSalt::NewL( *masterSalt );
    
    TSrtpCryptoParams params;
    
    
    iContext = CSRTPCryptoContext::NewL(iMasterKey, iMasterSalt, params );
    
    CleanupStack::PopAndDestroy( masterSalt );
     CleanupStack::PopAndDestroy( mki ); 
    CleanupStack::PopAndDestroy( masterKey ); 
    iDestination.Input(_L("127.0.0.1"));
    iSRTPSession = CSRTPSession::NewL( iDestination, iContext, *this ); 
    iStreamIn = CSRTPStreamIn::NewL(*iSRTPSession, (TUint)1);
  	
    iCryptoHandlerSRTP = iStreamIn->iHandlerRTP;
    } 

void UT_CSRTPStream::Teardown(  )
    {
    delete iDecryptedPayload;
    delete iTestPayload160Bits;
    delete iTestMKI128Bits;
    delete iTestAuthTag80Bits;
	
	iDecryptedPayload = NULL;
    iTestPayload160Bits = NULL;
    iTestMKI128Bits = NULL;
    iTestAuthTag80Bits = NULL;

    delete iStreamIn;
    delete iSRTPSession;  

    iStreamIn = NULL;
    iSRTPSession = NULL;	

    delete iRFC3711_SessionEncrKey128bits;
    delete iRFC3711_SessionSaltKey128bits;
    delete iRFC3711_SessionAuthKey128bits;    
	
	iRFC3711_SessionEncrKey128bits = NULL;
    iRFC3711_SessionSaltKey128bits = NULL;
    iRFC3711_SessionAuthKey128bits = NULL;    

    }
    
void UT_CSRTPStream::UT_CSRTPStream_TestNewL(  )
    {
	HBufC8* masterKey = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = KRFC3711_TestMasterKey128bits;
	
    
    HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KRFC3711_TestMasterSalt112bits;
	Hex(*mki);
	
    HBufC8* masterSalt =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFC3711_TestMasterSalt112bits;

	
	CSRTPMasterKey* mKey = CSRTPMasterKey::NewLC( *masterKey, *mki );
    CSRTPMasterSalt* mSalt = CSRTPMasterSalt::NewLC( *masterSalt );
    
    TSrtpCryptoParams params;
    
    
    CSRTPCryptoContext* context = CSRTPCryptoContext::NewL(mKey, mSalt, params );
    CleanupStack::Pop( mSalt );
    CleanupStack::Pop( mKey );
    CleanupStack::PopAndDestroy( masterSalt );
     CleanupStack::PopAndDestroy( mki ); 
    CleanupStack::PopAndDestroy( masterKey ); 
    CleanupStack::PushL( context );
    
    CSRTPStreamIn* streamIn = CSRTPStreamIn::NewL(*iSRTPSession, context, *this);
    CleanupStack::Pop( context );
  	delete streamIn;    
    }

void UT_CSRTPStream::UT_CSRTPStream_IsContextSetL(  )    
	{
		HBufC8* masterKey = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = KRFC3711_TestMasterKey128bits;
	
    
    HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KRFC3711_TestMasterSalt112bits;
	Hex(*mki);
	
    HBufC8* masterSalt =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFC3711_TestMasterSalt112bits;

	
	CSRTPMasterKey* mKey = CSRTPMasterKey::NewLC( *masterKey, *mki );
    CSRTPMasterSalt* mSalt = CSRTPMasterSalt::NewLC( *masterSalt );
    
    TSrtpCryptoParams params;
    
    CSRTPCryptoContext* context = CSRTPCryptoContext::NewL(mKey, mSalt, params );
    CleanupStack::Pop( mSalt );
    CleanupStack::Pop( mKey );
    CleanupStack::PopAndDestroy( masterSalt );
     CleanupStack::PopAndDestroy( mki ); 
    CleanupStack::PopAndDestroy( masterKey ); 
    CleanupStack::PushL( context );
    
    CSRTPSession* srtpSession = CSRTPSession::NewLC( iDestination ); 
    CSRTPStreamIn* streamIn = CSRTPStreamIn::NewL(*iSRTPSession, context, *this);
    CleanupStack::Pop( srtpSession );
    CleanupStack::Pop( context );
    CleanupStack::PushL( srtpSession );
    CleanupStack::PushL( streamIn );
	CSRTPStreamIn* streamIn2 = CSRTPStreamIn::NewL(*srtpSession, *this);
	CleanupStack::PushL( streamIn2 );
	streamIn->IsContextSet();
  	streamIn2->IsContextSet();
  	CleanupStack::PopAndDestroy( streamIn2 );   
  	CleanupStack::PopAndDestroy( streamIn );   
  	CleanupStack::PopAndDestroy( srtpSession );   
  	}

void UT_CSRTPStream::UT_CSRTPStream_SetROCL(  )
    {
	//CSRTPStream* tempStream =&(CSRTPStream( *iSRTPSession,0));
    iStreamIn ->SetROC(22);
    EUNIT_ASSERT(iStreamIn ->ROC()== 22);
    
    }

void UT_CSRTPStream::UT_CSRTPStream_ROCL(  )
    {
    EUNIT_ASSERT( ETrue);
    }

void UT_CSRTPStream::UT_CSRTPStream_GetCryptoContextL(  )
    {
    EUNIT_ASSERT((iStreamIn->GetCryptoContext().CryptoParams() .iSrtpAuthTagLen)==
    80);
    }
void UT_CSRTPStream::UT_CSRTPStream_SetSSRCL(  )
    {
    CSRTPStreamIn* streamIn = CSRTPStreamIn::NewL(*iSRTPSession, (TUint)0);
    streamIn->SetSSRC(1);
    EUNIT_ASSERT(streamIn->SSRC()== 1);
    
    delete streamIn;
    }
 
 void UT_CSRTPStream::UT_CSRTPStream_ReKeyNeeded_OneL(  )
    {
    iReKeyCalled= EFalse;
    CSRTPStreamIn* streamIn = CSRTPStreamIn::NewL(*iSRTPSession, (TUint)0);
    streamIn->ReKeyNeeded();
    EUNIT_ASSERT(iReKeyCalled==ETrue);
    delete streamIn;
    }   

void UT_CSRTPStream::UT_CSRTPStream_ReKeyNeeded_TwoL(  )
    {
    iReKeyCalled= EFalse;
    HBufC8* masterKey = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = KRFC3711_TestMasterKey128bits;
	
    
    HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KRFC3711_TestMasterSalt112bits;
	Hex(*mki);
	
    HBufC8* masterSalt =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFC3711_TestMasterSalt112bits;

	
	CSRTPMasterKey* mKey = CSRTPMasterKey::NewLC( *masterKey, *mki );
    CSRTPMasterSalt* mSalt = CSRTPMasterSalt::NewLC( *masterSalt );
    
    TSrtpCryptoParams params;
    
    
    CSRTPCryptoContext* context = CSRTPCryptoContext::NewL(mKey, mSalt, params );
    CleanupStack::Pop( mSalt );
    CleanupStack::Pop( mKey );
    CleanupStack::PopAndDestroy( masterSalt );
     CleanupStack::PopAndDestroy( mki ); 
    CleanupStack::PopAndDestroy( masterKey ); 
    CleanupStack::PushL( context );
    CSRTPStreamOut* streamOut= CSRTPStreamOut::NewL(*iSRTPSession, (TUint)1, context, *this);
    CleanupStack::Pop( context );
    streamOut->ReKeyNeeded();
    EUNIT_ASSERT(iReKeyCalled== ETrue);
    delete streamOut;
    }       
//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE( 
    UT_CSRTPStream,
    "CSRTPStream test",
    "UNIT" )

EUNIT_TEST(
    "SetROC - test ",
    "CSRTPStream",
    "SetROC",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStream_SetROCL, Teardown)

EUNIT_TEST(
    "ROC - test ",
    "CSRTPStream",
    "ROC",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStream_ROCL, Teardown)

EUNIT_TEST(
    "GetCryptoContext - test ",
    "CSRTPStream",
    "GetCryptoContext",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStream_GetCryptoContextL, Teardown)
EUNIT_TEST(
    "SetSSRCL ",
    "CSRTPStream",
    "SetSSRCL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStream_SetSSRCL, Teardown)  
EUNIT_TEST(
    "ReKey1",
    "CSRTPStream",
    "ReKey",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStream_ReKeyNeeded_OneL, Teardown)    
EUNIT_TEST(
    "ReKey2 ",
    "CSRTPStream",
    "ReKey",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStream_ReKeyNeeded_TwoL, Teardown)    

EUNIT_TEST(
    "TestNewL",
    "Test Default Constructor",
    "Test Default Constructor",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStream_TestNewL, Teardown)   

EUNIT_TEST(
    "isContextSetL",
    "IsContextSet",
    "IsContextSet",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStream_IsContextSetL, Teardown)            


EUNIT_END_TEST_TABLE

//  END OF FILE
