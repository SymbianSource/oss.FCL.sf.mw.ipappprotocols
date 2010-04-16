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
#include "UT_CSRTPCryptoHandlerSRTCP.h"
// CONSTANTS
const TInt KRtpVersion = 2;    // Current protocol version          
const TUint KMinRtpHeaderLen( 12 );
const TUint16 KTestSeqNr17 = 17171;
const TUint16 KTestSeqNr55 = 55;
const TUint16 KTestSeqNr17200 = 17200;
const TUint16 KTestSeqNr55536=55535;
const TUint16 KTestSeqNr65536=65535;

// 80 bit key, test 1
_LIT8(KTestPayload160Bits,  "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

// 80 bit key, test 1
//_LIT8(KTestMKI128Bits,      "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB");
_LIT8(KTestMKI128Bits,      "ABCDEF1234567890ABCDEF1234567890");

// 80 bit key, test 1
_LIT8(KTestAuthTag80Bits,   "CCCCCCCCCCCCCCCCCCCC");

_LIT8(KRFC3711_TestMasterKey128bits,    "E1F97A0D3E018BE0D64FA32C06DE4139");
_LIT8(KRFC3711_TestMasterSalt112bits,   "0EC675AD498AFEEBB6960B3AABE6");	
_LIT8(KRFC3711_SessionEncrKey128bits,   "4C1AA45A81F73D61C800BBB00FBB1EAA");  
_LIT8(KRFC3711_SessionSaltKey128bits,   "9581C7AD87B3E530BF3E4454A8B3");  
_LIT8(KRFC3711_SessionAuthKey128bits,   "8D54534FEB49AE8E7993A6BD0B844FC323A93DFD");  
_LIT8(KDecryptedPayload,                "2EDE3286C6E33E68C6BB93350620A1245EC0004F");  

_LIT8(KTestM_without_ROC,               "123456789ABCDE");  
_LIT8(KRefM_with_ROC,                   "123456789ABCDEFFFFFFFF");  

_LIT8(KTestAuthTag,                     "D07919ABC9F235CB4516");  
_LIT8(KSRTCPPacket, "80C90001DEADBEEF9E30159059A23363F0FBDB62DC85AF52875C944495C5B00280000001ABCDEF1234567890ABCDEF1234567890DFC98F01DE16DA30F20D");
_LIT8(KRTCPPacket, "80C80006DEADBEEFC7B1ACD423FE000000E12F18000000010000020081CA0015DEADBEEF010A52747055492075736572020455736572030D7573657240686F6D652E6E6574040B2B313233343536373839300504486F6D65060B527470206578616D706C65070641637469766508010000000000");
_LIT8(KSRTCPPacket1, "80C90001DEADBEEF9E30159059A23363F0FBDB62DC85AF52875C944495C5B00280000001ABCDEF1234567890ABCDEF1234567890DFC98F01DE16DA30F20D");
_LIT8(KSRTCPPacket2, "80C90001DEADBEEF2BC095F51495D87D35B816BE569A6A1BA6989D5A20B541D680000002ABCDEF1234567890ABCDEF12345678905140BA93E8CDAC0C89D5");


_LIT8(KSRTCPPacketWrongMKI, "80C90001DEADBEEF9E30159059A23363F0FBDB62DC85AF52875C944495C5B00280000001ABCDEF1234567890DFC98F01DE16DA30F20D");
_LIT8(KSRTCPPacketNoAuthTag, "80C90001DEADBEEF9E30159059A23363F0FBDB62DC85AF52875C944495C5B00280000001ABCDEF1234567890ABCDEF1234567890");
_LIT8(KSRTCPPacketWrongAuthTag, "80C90001DEADBEEF9E30159059A23363F0FBDB62DC85AF52875C944495C5B00280000001ABCDEF1234567890ABCDEF1234567890DFC98F01DE16DA30F20A");
_LIT8(KRTCPPacketIndexUpperLimit, 
"80C80006DEADBEEFC7B1ACD423FE000000E12F18000000010000020081CA0015DEADBEEF010A52747055492075736572020455736572030D7573657240686F6D652E6E6574040B2B313233343536373839300504486F6D65060B527470206578616D706C650706416374697665087FFFFFFE");

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

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "srtpcryptohandlersrtcp.h"
#include "srtpmastersalt.h"
#include "srtpmasterkey.h"
#include "srtpstreamin.h"
#include "srtpstreamout.h"
#include "srtpsession.h"
#include "srtputils.h"

// CONSTRUCTION
UT_CSRTPCryptoHandlerSRTCP* UT_CSRTPCryptoHandlerSRTCP::NewL()
    {
    UT_CSRTPCryptoHandlerSRTCP* self = UT_CSRTPCryptoHandlerSRTCP::NewLC(); 
    CleanupStack::Pop();

    return self;
    }

UT_CSRTPCryptoHandlerSRTCP* UT_CSRTPCryptoHandlerSRTCP::NewLC()
    {
    UT_CSRTPCryptoHandlerSRTCP* self = new( ELeave ) UT_CSRTPCryptoHandlerSRTCP();
    CleanupStack::PushL( self );

	self->ConstructL(); 

    return self;
    }

// Destructor (virtual by CBase)
UT_CSRTPCryptoHandlerSRTCP::~UT_CSRTPCryptoHandlerSRTCP()
    {
    }

// Default constructor
UT_CSRTPCryptoHandlerSRTCP::UT_CSRTPCryptoHandlerSRTCP()
    {
    }

// Second phase construct
void UT_CSRTPCryptoHandlerSRTCP::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

void UT_CSRTPCryptoHandlerSRTCP::SRTPMasterKeyStaleEvent(const CSRTPStream& /*aStream*/ )
    {
    iReKeyCalled= ETrue;
    }
void UT_CSRTPCryptoHandlerSRTCP::SRTPMasterKeyStaleEvent(const CSRTPSession& /*aSession*/ )
    {
    iReKeyCalled= ETrue;
    }
void UT_CSRTPCryptoHandlerSRTCP::Hex(HBufC8& aString)
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
  

void UT_CSRTPCryptoHandlerSRTCP::SetupL(  )
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
	
	iMasterKey = CSRTPMasterKey::NewL( *masterKey, *mki );
    iMasterSalt = CSRTPMasterSalt::NewL( *masterSalt );
    
    
    TSrtpCryptoParams params;
    iContext = CSRTPCryptoContext::NewL(iMasterKey, iMasterSalt, params );
    CleanupStack::PopAndDestroy( mki );
    CleanupStack::PopAndDestroy( masterSalt );
    CleanupStack::PopAndDestroy( masterKey ); 
    iDestination.Input(_L("127.0.0.1"));

    iSRTPSession = CSRTPSession::NewL( iDestination, iContext, *this ); 
    iStreamIn = CSRTPStreamIn::NewL(*iSRTPSession, 0xDEADBEEF);
  
    iCryptoHandlerSRTCP=  iStreamIn->iHandlerRTCP;   
    iReKeyCalled = EFalse; 
    } 

void UT_CSRTPCryptoHandlerSRTCP::Teardown(  )
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

void UT_CSRTPCryptoHandlerSRTCP::UT_CSRTPCryptoHandlerSRTCP_NewLL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CSRTPCryptoHandlerSRTCP::UT_CSRTPCryptoHandlerSRTCP_NewLCL(  )
    {
    EUNIT_ASSERT( ETrue );
    }



void UT_CSRTPCryptoHandlerSRTCP::UT_DeriveSessionKeysL_OneL()
	{
	HBufC8* encSrtcpPacket =HBufC8::NewLC(KSRTCPPacket().Length());
	*encSrtcpPacket=KSRTCPPacket;
    Hex(*encSrtcpPacket);
    TInt len= KSRTCPPacket().Length();
	encSrtcpPacket->Des().SetLength(len/2);
	
    iCryptoHandlerSRTCP->InitializeEncryptedPacketL(*encSrtcpPacket);
    iCryptoHandlerSRTCP->ReceiverPacketIndexL();
    iCryptoHandlerSRTCP->DeriveSessionKeysL();

    EUNIT_ASSERT( iCryptoHandlerSRTCP->iSessionEncrKey->Compare(
                                    *iRFC3711_SessionEncrKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTCP->iSessionSaltKey->Compare(
                                    *iRFC3711_SessionSaltKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTCP->iSessionAuthKey->Compare(
                                    *iRFC3711_SessionAuthKey128bits) == 0);
    CleanupStack::PopAndDestroy(encSrtcpPacket);  	
	}
         
 void UT_CSRTPCryptoHandlerSRTCP::UT_PerformAuthenticationLL()
 	{
 	HBufC8* encSrtcpPacket =HBufC8::NewLC(KSRTCPPacket().Length());
	*encSrtcpPacket=KSRTCPPacket;
    Hex(*encSrtcpPacket);
    TInt len= KSRTCPPacket().Length();
	encSrtcpPacket->Des().SetLength(len/2);
 	iCryptoHandlerSRTCP->InitializeEncryptedPacketL(*encSrtcpPacket);
 	iCryptoHandlerSRTCP->ReceiverPacketIndexL();
    iCryptoHandlerSRTCP->CheckMasterKeyIdentifierL();
    iCryptoHandlerSRTCP->DeriveSessionKeysL();
	
	// do the authentication (integrity check)
    iCryptoHandlerSRTCP->AuthenticateL();
 	CleanupStack::PopAndDestroy(encSrtcpPacket);     
 	}

void UT_CSRTPCryptoHandlerSRTCP::UT_AuthenticateLL( ) 
 	{
 	//false cases when auth tage =0 but using HMAC auth
 	HBufC8* encSrtcpPacket =HBufC8::NewLC(KSRTCPPacketNoAuthTag().Length());
	*encSrtcpPacket=KSRTCPPacketNoAuthTag;
    Hex(*encSrtcpPacket);
    TInt len= KSRTCPPacketNoAuthTag().Length();
	encSrtcpPacket->Des().SetLength(len/2);
	//update crypto handler in the session
	//Set AuthLen=0
	TSrtpCryptoParams crypto;
	crypto.iSrtcpAuthTagLen=0;
	iCryptoHandlerSRTCP->Context().UpdateCryptoParams(crypto);
 	iCryptoHandlerSRTCP->InitializeEncryptedPacketL(*encSrtcpPacket);
 	iCryptoHandlerSRTCP->ReceiverPacketIndexL();
    iCryptoHandlerSRTCP->CheckMasterKeyIdentifierL();
    iCryptoHandlerSRTCP->DeriveSessionKeysL();
	
	// do the authentication (integrity check)
    EUNIT_ASSERT_SPECIFIC_LEAVE(iCryptoHandlerSRTCP->AuthenticateL(), 
    													KErrNotFound);
 	CleanupStack::PopAndDestroy(encSrtcpPacket);     
 	
 	
 	}
 	
void UT_CSRTPCryptoHandlerSRTCP::UT_AuthenticateL2L( ) 
 	{
 	//false cases when auth tage is not equal 
 	HBufC8* encSrtcpPacket =HBufC8::NewLC(KSRTCPPacketWrongAuthTag().Length());
	*encSrtcpPacket=KSRTCPPacketWrongAuthTag;
    Hex(*encSrtcpPacket);
    TInt len= KSRTCPPacketWrongAuthTag().Length();
	encSrtcpPacket->Des().SetLength(len/2);
 	iCryptoHandlerSRTCP->InitializeEncryptedPacketL(*encSrtcpPacket);
 	iCryptoHandlerSRTCP->ReceiverPacketIndexL();
    iCryptoHandlerSRTCP->CheckMasterKeyIdentifierL();
    iCryptoHandlerSRTCP->DeriveSessionKeysL();
	
	// do the authentication (integrity check)
    RTP_EUNIT_ASSERT_SPECIFIC_LEAVE(iCryptoHandlerSRTCP->AuthenticateL(), 
    													KErrNotFound);
 	CleanupStack::PopAndDestroy(encSrtcpPacket);     
 	
 	
 	}
 	 	
  void UT_CSRTPCryptoHandlerSRTCP::UT_AddAuthTagLL(  )
 	{
 	//Set auth tag len=0 and MKI = NULL
 	
 			
   	HBufC8* masterKey = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = KRFC3711_TestMasterKey128bits;
	
    HBufC8* masterSalt =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFC3711_TestMasterSalt112bits;
	
	
	CSRTPMasterKey* master = CSRTPMasterKey::NewLC( *masterKey, KNullDesC8 );
    CSRTPMasterSalt* salt = CSRTPMasterSalt::NewLC( *masterSalt );
    TSrtpCryptoParams params;
    params.iSrtcpAuthAlg=EAuthNull;
    params.iSrtcpAuthTagLen=0;
    CSRTPCryptoContext* context = CSRTPCryptoContext::NewL(master, salt, params);
    
    CleanupStack::Pop( salt );
    CleanupStack::Pop( master ); 
    CleanupStack::PopAndDestroy( masterSalt );
    CleanupStack::PopAndDestroy( masterKey ); 
    iDestination.Input(_L("127.0.0.1"));

    CleanupStack::PushL( context );
    CSRTPSession* srtpSession = CSRTPSession::NewL( iDestination); 
    CleanupStack::PushL( srtpSession );
    CSRTPStreamIn* streamIn = CSRTPStreamIn::NewL(*srtpSession, 0xDEADBEEF, context, *this);
    CleanupStack::Pop( srtpSession );
    CleanupStack::Pop( context );
    CleanupStack::PushL( srtpSession );
    CleanupStack::PushL( streamIn );
    CSRTPCryptoHandlerSRTCP* cryptoSrtcp=  streamIn->iHandlerRTCP;
    
    HBufC8* rtcpPacket =HBufC8::NewLC(KRTCPPacket().Length());
	*rtcpPacket=KRTCPPacket;
    Hex(*rtcpPacket);
    TInt len= KRTCPPacket().Length();
	rtcpPacket->Des().SetLength(len/2);
    cryptoSrtcp->InitializePlainPacketL(*rtcpPacket);
    //for testing only , not for real cases packet created sequences
    TUint8* srtpPacketPtr = const_cast<TUint8*>(rtcpPacket->Des().Ptr());
    cryptoSrtcp->AddAuthTagToPacketL(srtpPacketPtr);
    EUNIT_ASSERT(rtcpPacket->Size()==(len/2));
    CleanupStack::PopAndDestroy(rtcpPacket);  
    CleanupStack::PopAndDestroy( streamIn );
    CleanupStack::PopAndDestroy( srtpSession );
 	}
 	
 	
 		
 void UT_CSRTPCryptoHandlerSRTCP::UT_CountSenderPacketIndexL1L(  )
 	{
 	// just a dummy packet index
    //iCryptoHandlerSRTCP->iReplayPktIndex = 5;
    HBufC8* rtcpPacket =HBufC8::NewLC(KRTCPPacket().Length());
	*rtcpPacket=KRTCPPacket;
    Hex(*rtcpPacket);
    TInt len= KRTCPPacket().Length();
	rtcpPacket->Des().SetLength(len/2);
    iCryptoHandlerSRTCP->InitializePlainPacketL(*rtcpPacket);
    iCryptoHandlerSRTCP->CountSenderPacketIndexL();
    //normal situation
    EUNIT_ASSERT(iCryptoHandlerSRTCP->iSRTCPIndex==0x80000001);
    EUNIT_ASSERT( !iReKeyCalled );
    EUNIT_ASSERT ( !iCryptoHandlerSRTCP->iReKey );
    
    //test Rekey
    iReKeyCalled = EFalse;
    
    iCryptoHandlerSRTCP->iNumPackets = 100;
    iCryptoHandlerSRTCP->iSRTCPMasterKeyLifeTime = 200;

    iCryptoHandlerSRTCP->IndexReKeyCheckL();
    EUNIT_ASSERT( iReKeyCalled );
    iCryptoHandlerSRTCP->iNumPackets = 200;
    iCryptoHandlerSRTCP->IndexReKeyCheckL();
    EUNIT_ASSERT ( iCryptoHandlerSRTCP->iReKey );
    
	CleanupStack::PopAndDestroy(rtcpPacket);
 	}
 	
 void UT_CSRTPCryptoHandlerSRTCP::UT_CountSenderPacketIndexL2L(  )
 	{
 	//Index Rekey Check when does not need Re-Key
 	// just a dummy packet index
    //iCryptoHandlerSRTCP->iReplayPktIndex = 5;
    iCryptoHandlerSRTCP->iSRTCPIndexEStripped= 0x7FFFFFFE;
    HBufC8* rtcpPacket =HBufC8::NewLC(KRTCPPacketIndexUpperLimit().Length());
	*rtcpPacket=KRTCPPacketIndexUpperLimit;
    Hex(*rtcpPacket);
    TInt len= KRTCPPacketIndexUpperLimit().Length();
	rtcpPacket->Des().SetLength(len/2);
    iCryptoHandlerSRTCP->InitializePlainPacketL(*rtcpPacket);
    iCryptoHandlerSRTCP->CountSenderPacketIndexL();
    //normal situation
    //7FFFFFFE -is the index we sent
    EUNIT_ASSERT(iCryptoHandlerSRTCP->iSRTCPIndex==0xFFFFFFFF);
    EUNIT_ASSERT(iReKeyCalled== ETrue);
	CleanupStack::PopAndDestroy(rtcpPacket);
 	} 	
 
 void UT_CSRTPCryptoHandlerSRTCP::UT_ReplayProtectionL1L( )   
 	{
 	HBufC8* encSrtcpPacket1 =HBufC8::NewLC(KSRTCPPacket1().Length());
	*encSrtcpPacket1=KSRTCPPacket1;
    Hex(*encSrtcpPacket1);
    TInt len= KSRTCPPacket1().Length();
	encSrtcpPacket1->Des().SetLength(len/2);
	

    // do the authentication (integrity check)
    iCryptoHandlerSRTCP->InitializeEncryptedPacketL(*encSrtcpPacket1);
 	iCryptoHandlerSRTCP->ReceiverPacketIndexL();
 	
             
    // derive session keys..
    iCryptoHandlerSRTCP->DeriveSessionKeysL();
    // do the replay protection 
    iCryptoHandlerSRTCP->ReplayProtectionL();
    
    iCryptoHandlerSRTCP->AuthenticateL();
    iCryptoHandlerSRTCP->AddReplayIndex();
    //EUNIT_ASSERT(iCryptoHandlerSRTP->iPktIndex == KTestSeqNr17); 
   	CleanupStack::PopAndDestroy(encSrtcpPacket1);  
    
    //Create the packet again so can check the Replay protection
            
	HBufC8* encSrtcpPacket2 =HBufC8::NewLC(KSRTCPPacket2().Length());
	*encSrtcpPacket2=KSRTCPPacket2;
    Hex(*encSrtcpPacket2);
    TInt len2= KSRTCPPacket2().Length();
	encSrtcpPacket2->Des().SetLength(len2/2);
    
    
        
    iCryptoHandlerSRTCP->InitializeEncryptedPacketL(*encSrtcpPacket2);
 
    iCryptoHandlerSRTCP->ReceiverPacketIndexL();
     
    iCryptoHandlerSRTCP->ReplayProtectionL();
    
    iCryptoHandlerSRTCP->AddReplayIndex(); 
    TUint64  temp= iCryptoHandlerSRTCP->iReplayPktIndex; 
    //EUNIT_ASSERT(iCryptoHandlerSRTCP->iReplayPktIndex == 2);
   	CleanupStack::PopAndDestroy(encSrtcpPacket2);  	
 	}
 	
 void UT_CSRTPCryptoHandlerSRTCP::UT_ReplayProtectionL2L( )
 	{
 	//Set stored PktIndx number to bigger than the on in the sequence number
    //but inside the Replay window and has not been seen
    iCryptoHandlerSRTCP->iReplayPktIndex= KTestSeqNr55;
    
    HBufC8* encSrtcpPacket1 =HBufC8::NewLC(KSRTCPPacket1().Length());
	*encSrtcpPacket1=KSRTCPPacket1;
    Hex(*encSrtcpPacket1);
    TInt len= KSRTCPPacket1().Length();
	encSrtcpPacket1->Des().SetLength(len/2);
        
    iCryptoHandlerSRTCP->InitializeEncryptedPacketL(*encSrtcpPacket1);
 	iCryptoHandlerSRTCP->ReceiverPacketIndexL();
    
             
    // derive session keys..
    iCryptoHandlerSRTCP->DeriveSessionKeysL();
    // do the replay protection 
    iCryptoHandlerSRTCP->ReplayProtectionL();
    
    iCryptoHandlerSRTCP->AddReplayIndex();
    EUNIT_ASSERT(iCryptoHandlerSRTCP->iReplayPktIndex == KTestSeqNr55); 
    //check also bitmap later
   	CleanupStack::PopAndDestroy(encSrtcpPacket1); 
 	}
 	
 void UT_CSRTPCryptoHandlerSRTCP::UT_ReplayProtectionL3L( )   
 	{
 	//Set stored PktIndx number to bigger than the on in the sequence number
    //but inside the Replay window and it has been seen
     	
 	HBufC8* encSrtcpPacket1 =HBufC8::NewLC(KSRTCPPacket1().Length());
	*encSrtcpPacket1=KSRTCPPacket1;
    Hex(*encSrtcpPacket1);
    TInt len= KSRTCPPacket1().Length();
	encSrtcpPacket1->Des().SetLength(len/2);
	

    // do the authentication (integrity check)
    iCryptoHandlerSRTCP->InitializeEncryptedPacketL(*encSrtcpPacket1);
 	iCryptoHandlerSRTCP->ReceiverPacketIndexL();
 	iCryptoHandlerSRTCP->CheckMasterKeyIdentifierL();
             
    // derive session keys..
    iCryptoHandlerSRTCP->DeriveSessionKeysL();
    // do the replay protection 
    iCryptoHandlerSRTCP->ReplayProtectionL();
    
    iCryptoHandlerSRTCP->AuthenticateL();
    iCryptoHandlerSRTCP->AddReplayIndex();
    //EUNIT_ASSERT(iCryptoHandlerSRTP->iPktIndex == KTestSeqNr17); 
   	CleanupStack::PopAndDestroy(encSrtcpPacket1);  
    
    //Create the packet again so can check the Replay protection
            
	HBufC8* encSrtcpPacket2 =HBufC8::NewLC(KSRTCPPacket2().Length());
	*encSrtcpPacket2=KSRTCPPacket2;
    Hex(*encSrtcpPacket2);
    TInt len2= KSRTCPPacket2().Length();
	encSrtcpPacket2->Des().SetLength(len2/2);
    
    
        
    iCryptoHandlerSRTCP->InitializeEncryptedPacketL(*encSrtcpPacket2);
 
    iCryptoHandlerSRTCP->ReceiverPacketIndexL();
     
    iCryptoHandlerSRTCP->ReplayProtectionL();
    
    iCryptoHandlerSRTCP->AddReplayIndex(); 
    
   	CleanupStack::PopAndDestroy(encSrtcpPacket2);  	

	HBufC8* encSrtcpPacket3 =HBufC8::NewLC(KSRTCPPacket1().Length());
	*encSrtcpPacket3=KSRTCPPacket1;
    Hex(*encSrtcpPacket3);
    len= KSRTCPPacket1().Length();
	encSrtcpPacket1->Des().SetLength(len/2);
	

    // do the authentication (integrity check)
    iCryptoHandlerSRTCP->InitializeEncryptedPacketL(*encSrtcpPacket3);
 	iCryptoHandlerSRTCP->ReceiverPacketIndexL();
             
    // do the replay protection 
    EUNIT_ASSERT_SPECIFIC_LEAVE( iCryptoHandlerSRTCP->ReplayProtectionL(),
     KErrArgument );
    
   	CleanupStack::PopAndDestroy(encSrtcpPacket3);  
       	
     	}
 void UT_CSRTPCryptoHandlerSRTCP::UT_ReplayProtectionL4L( )  
 	{
 	//Set stored PktIndx number to bigger than the one in the sequence number
    //and outside the Replay window
    iCryptoHandlerSRTCP->iReplayPktIndex= KTestSeqNr65536;
    HBufC8* encSrtcpPacket1 =HBufC8::NewLC(KSRTCPPacket1().Length());
	*encSrtcpPacket1=KSRTCPPacket1;
    Hex(*encSrtcpPacket1);
    TInt len= KSRTCPPacket1().Length();
	encSrtcpPacket1->Des().SetLength(len/2);
        
    iCryptoHandlerSRTCP->InitializeEncryptedPacketL(*encSrtcpPacket1);
 	iCryptoHandlerSRTCP->ReceiverPacketIndexL();
             
    
    // do the replay protection 
    EUNIT_ASSERT_SPECIFIC_LEAVE( 
    	iCryptoHandlerSRTCP->ReplayProtectionL(), KErrArgument );
    
   	CleanupStack::PopAndDestroy(encSrtcpPacket1); 
 	}
 	
 void UT_CSRTPCryptoHandlerSRTCP::UT_CheckMasterKeyIdentifierLL( ) 
 	{
	HBufC8* encSrtcpPacket =HBufC8::NewLC(KSRTCPPacketWrongMKI().Length());
	*encSrtcpPacket=KSRTCPPacketWrongMKI;
    Hex(*encSrtcpPacket);
    TInt len= KSRTCPPacketWrongMKI().Length();
	encSrtcpPacket->Des().SetLength(len/2);
 	iCryptoHandlerSRTCP->InitializeEncryptedPacketL(*encSrtcpPacket);
 	iCryptoHandlerSRTCP->ReceiverPacketIndexL();
   	//giving packet different MKI from the one in the master key
    EUNIT_ASSERT_SPECIFIC_LEAVE(iCryptoHandlerSRTCP->CheckMasterKeyIdentifierL(),
    							KErrNotFound);
   
 	CleanupStack::PopAndDestroy(encSrtcpPacket);      	
 	}


//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE( 
    UT_CSRTPCryptoHandlerSRTCP,
    "CSRTPCryptoHandlerSRTCP",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "CSRTPCryptoHandlerSRTCP",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPCryptoHandlerSRTCP_NewLL, Teardown)

EUNIT_TEST(
    "NewLC - test ",
    "CSRTPCryptoHandlerSRTCP",
    "NewLC",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPCryptoHandlerSRTCP_NewLCL, Teardown)

EUNIT_TEST(
    "DeriveKeys1L - test ",
    "CSRTPCryptoHandlerSRTCP",
    "DeriveSessionKeys",
    "FUNCTIONALITY",
    SetupL, UT_DeriveSessionKeysL_OneL, Teardown)
    
EUNIT_TEST(
    "Authentication1",
    "CSRTPCryptoHandlerSRTCP",
    "PerformAuthentication",
    "FUNCTIONALITY",
    SetupL, UT_PerformAuthenticationLL, Teardown)           
EUNIT_TEST(
    "Authentication2",
    "CSRTPCryptoHandlerSRTCP",
    "UT_AuthenticateLL",
    "FUNCTIONALITY",
    SetupL, UT_AuthenticateLL, Teardown)    
    
EUNIT_TEST(
    "Authentication3",
    "CSRTPCryptoHandlerSRTCP",
    "UT_AuthenticateL2L",
    "FUNCTIONALITY",
    SetupL, UT_AuthenticateL2L, Teardown) 
EUNIT_TEST(
    "AddAuth",
    "CSRTPCryptoHandlerSRTCP",
    "AddAuth",
    "FUNCTIONALITY",
    SetupL, UT_AddAuthTagLL, Teardown)     
    
    
EUNIT_TEST(
    "CntSndPktIndex1",
    "CSRTPCryptoHandlerSRTCP",
    "CountSenderPacketIndexL",
    "FUNCTIONALITY",
    SetupL, UT_CountSenderPacketIndexL1L, Teardown) 
    
EUNIT_TEST(
    "CntSndPktIndex2",
    "CSRTPCryptoHandlerSRTCP",
    "CountSenderPacketIndex2L",
    "FUNCTIONALITY",
    SetupL, UT_CountSenderPacketIndexL2L, Teardown)     
          
EUNIT_TEST(
    "RProtection1",
    "CSRTPCryptoHandlerSRTCP",
    "ReplayProtectionL1",
    "FUNCTIONALITY",
    SetupL, UT_ReplayProtectionL1L, Teardown)  
         
 EUNIT_TEST(
    "RProtection2",
    "CSRTPCryptoHandlerSRTCP",
    "ReplayProtectionL2",
    "FUNCTIONALITY",
    SetupL, UT_ReplayProtectionL2L, Teardown)   
EUNIT_TEST(
    "RProtection3",
    "CSRTPCryptoHandlerSRTCP",
    "ReplayProtectionL3",
    "FUNCTIONALITY",
    SetupL, UT_ReplayProtectionL3L, Teardown)  
EUNIT_TEST(
    "RProtection4",
    "CSRTPCryptoHandlerSRTCP",
    "ReplayProtectionL4",
    "FUNCTIONALITY",
    SetupL, UT_ReplayProtectionL4L, Teardown) 
EUNIT_TEST(
    "CheckMKI",
    "CSRTPCryptoHandlerSRTCP",
    "CheckMKI",
    "FUNCTIONALITY",
    SetupL, UT_CheckMasterKeyIdentifierLL, Teardown)          

EUNIT_END_TEST_TABLE

//  END OF FILE
