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
#include "UT_CSRTPCryptoHandlerSRTP.h"
#include "srtppacket.h"
#include "srtppacketrtp.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>

// CONSTANTS
const TInt KRtpVersion = 2;    // Current protocol version          
const TUint KMinRtpHeaderLen( 12 );
const TUint16 KTestSeqNr17 = 17171;
const TUint16 KTestSeqNr18 = 17172;
const TUint16 KTestSeqNr17200 = 17200;
const TUint16 KTestSeqNr55536=55535;
const TUint16 KTestSeqNr65536=65535;

// 80 bit key, test 1
_LIT8(KTestPayload160Bits,  "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

// 80 bit key, test 1
_LIT8(KTestMKI128Bits2,      "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB");
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
#include "srtpcryptohandlersrtp.h"
#include "srtpmastersalt.h"
#include "srtpmasterkey.h"
#include "srtpstreamin.h"
#include "srtpstreamout.h"
#include "srtpsession.h"
#include "srtputils.h"


UT_CSRTPCryptoHandlerSRTP::TPayloadDetails::TPayloadDetails() :
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
        

// CONSTRUCTION
UT_CSRTPCryptoHandlerSRTP* UT_CSRTPCryptoHandlerSRTP::NewL()
    {
    UT_CSRTPCryptoHandlerSRTP* self = UT_CSRTPCryptoHandlerSRTP::NewLC(); 
    CleanupStack::Pop();

    return self;
    }

UT_CSRTPCryptoHandlerSRTP* UT_CSRTPCryptoHandlerSRTP::NewLC()
    {
    UT_CSRTPCryptoHandlerSRTP* self = new( ELeave ) UT_CSRTPCryptoHandlerSRTP();
    CleanupStack::PushL( self );

	self->ConstructL(); 

    return self;
    }

// Destructor (virtual by CBase)
UT_CSRTPCryptoHandlerSRTP::~UT_CSRTPCryptoHandlerSRTP()
    {
    }

// Default constructor
UT_CSRTPCryptoHandlerSRTP::UT_CSRTPCryptoHandlerSRTP() 
    {

    }

// Second phase construct
void UT_CSRTPCryptoHandlerSRTP::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void UT_CSRTPCryptoHandlerSRTP::SRTPMasterKeyStaleEvent(const CSRTPStream& /*aStream*/ )
    {
    iReKeyCalled= ETrue;
    }
void UT_CSRTPCryptoHandlerSRTP::SRTPMasterKeyStaleEvent(const CSRTPSession& /*aSession*/ )
    {
    iReKeyCalled= ETrue;
    }



void UT_CSRTPCryptoHandlerSRTP::SetupL(  )
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
    iStreamIn = CSRTPStreamIn::NewL(*iSRTPSession, (TUint)1);

    iCryptoHandlerSRTP=  iStreamIn->iHandlerRTP;    
    iReKeyCalled = EFalse;
    } 



void UT_CSRTPCryptoHandlerSRTP::Teardown(  )
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


void UT_CSRTPCryptoHandlerSRTP::Hex(HBufC8& aString)
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

// ---------------------------------------------------------------------------
// UT_CSRTPCryptoHandlerSRTP::BuildSrtpPacketL()
// 
// ---------------------------------------------------------------------------
//
HBufC8* UT_CSRTPCryptoHandlerSRTP::BuildPacketL(TBool aSrtp, TPayloadDetails& details)
    {
    TUint packetSize = KMinRtpHeaderLen + details.payloadLen;
    TUint payloadStartOffset = KMinRtpHeaderLen;
    TUint8 padCount = 0;
    
    packetSize += details.numCSRC*4;
    payloadStartOffset += details.numCSRC*4;          
    
    if (details.fHeaderExtension) 
        {
        packetSize += 4;        
        payloadStartOffset += 4;
        }

    if (details.padding) 
        {
        padCount = 3;
        }
        
    packetSize += padCount;            
        
    if (aSrtp)
        {
        packetSize += details.MKILen;                    
        packetSize += details.authTagLen;                        
        }
        
    
    HBufC8* packet = HBufC8::NewMaxL(packetSize);
    TUint8* dataP = const_cast<TUint8*>( packet->Des().Ptr() );    
    TUint8* ptr = dataP;
    Mem::FillZ( ptr, packetSize);
    
    // padding (1 bit)
    ptr[0] |= static_cast<TUint8>( details.padding << 5 );
        
    // header extension (1 bit)
    if ( details.fHeaderExtension )
        {
        ptr[0] |= ( 1 << 4 );
        }
    
    if (details.numCSRC)
        {
        // CC
        ptr[0] |= details.numCSRC;
        }
       
    //sequence number
    ptr+=2;
    TSRTPUtils::Write16( ptr, details.seqNr );    
    
    //ssrc   
    /*
    ptr+=6;
    if (details.ssrc)
        {
        Mem::Copy( ptr, details.ssrc, 4 );        
        }
   */
    //payload
    ptr = dataP + payloadStartOffset;
    if (details.payloadData)
        {
        Mem::Copy( ptr, details.payloadData, details.payloadLen );        
        }

    if (aSrtp)
        {
        //MKI
        ptr = dataP + payloadStartOffset + details.payloadLen + padCount;
        if (details.MKILen > 0)
            {
            Mem::Copy( ptr, details.MKIData, details.MKILen );        
            }
    
        //auth tag
        ptr = dataP + payloadStartOffset + details.payloadLen + padCount + details.MKILen;
        if (details.authTagLen > 0)
            {
            Mem::Copy( ptr, details.authTagData, details.authTagLen );        
            }            
        }    
    
    return packet;
    }



void UT_CSRTPCryptoHandlerSRTP::UT_CSRTPCryptoHandlerSRTP_NewLL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CSRTPCryptoHandlerSRTP::UT_CSRTPCryptoHandlerSRTP_NewLCL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CSRTPCryptoHandlerSRTP::UT_InitializeEncryptedPacketLL(  )
    {
    //Create the packet firest 
     TInt authTagLenInBytes = iContext->CryptoParams().iSrtpAuthTagLen/8;
        
    TBuf8<32> payload;
    TBuf8<32> authTag;        
    TInt ssrc = 500;
//        TUint8* ssrcPtr = static_cast<TUint8 *>(&ssrc);

    
    for (TInt i=0; i<20; i++)
        {
        payload.Append(0xAA);            
        }        

    authTag.Append(0x79);
    authTag.Append(0xD0);
    authTag.Append(0xAB);
    authTag.Append(0x19);
    authTag.Append(0xF2);
    authTag.Append(0xC9);
    authTag.Append(0xCB);
    authTag.Append(0x35);
    authTag.Append(0x16);
    authTag.Append(0x45);

	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	TInt mkiLength = mki->Length();
	
    const TUint8* payloadPtr = payload.PtrZ();
    const TUint8* mkiPtr = mki->Des().PtrZ();
    const TUint8* authTagPtr = authTag.PtrZ();
    const TUint8* decryptedPayloadPtr = iDecryptedPayload->Ptr();
    
    TPayloadDetails details;    

    details.seqNr       = KTestSeqNr17;
//        details.ssrc        = ssrcPtr;
    details.padding     = 0;
    details.numCSRC     = 0;
    details.fHeaderExtension = EFalse;
    details.payloadLen  = 0;
    details.payloadData = NULL;
    details.MKILen      = mkiLength;
    details.MKIData     = mkiPtr;
    details.authTagLen  = authTagLenInBytes;
    details.authTagData = authTagPtr;    
  
      
    HBufC8* packet = BuildPacketL(ETrue, details);
    CleanupStack::PushL(packet);
    TInt packetLength = packet->Length();
        
    iCryptoHandlerSRTP->InitializeEncryptedPacketL(*packet);
    
    EUNIT_ASSERT( iCryptoHandlerSRTP->iCurrentPacket->PayloadLength()== 0);
   
   
    CleanupStack::PopAndDestroy(packet);  
    CleanupStack::PopAndDestroy(mki);                                   
    }



void UT_CSRTPCryptoHandlerSRTP::UT_DeriveSessionKeysL_OneL(  )
    {
    //Create the packet firest 
     TInt authTagLenInBytes = iContext->CryptoParams().iSrtpAuthTagLen/8;
        
    TBuf8<32> payload;
    TBuf8<32> authTag;        
    TInt ssrc = 500;
//        TUint8* ssrcPtr = static_cast<TUint8 *>(&ssrc);

    
    for (TInt i=0; i<20; i++)
        {
        payload.Append(0xAA);            
        }        

    authTag.Append(0x79);
    authTag.Append(0xD0);
    authTag.Append(0xAB);
    authTag.Append(0x19);
    authTag.Append(0xF2);
    authTag.Append(0xC9);
    authTag.Append(0xCB);
    authTag.Append(0x35);
    authTag.Append(0x16);
    authTag.Append(0x45);

	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	TInt mkiLength = mki->Length();
	
    const TUint8* payloadPtr = payload.PtrZ();
    const TUint8* mkiPtr = mki->Des().PtrZ();
    const TUint8* authTagPtr = authTag.PtrZ();
    const TUint8* decryptedPayloadPtr = iDecryptedPayload->Ptr();
    
    TPayloadDetails details;    

    details.seqNr       = KTestSeqNr17;
//        details.ssrc        = ssrcPtr;
    details.padding     = 0;
    details.numCSRC     = 0;
    details.fHeaderExtension = EFalse;
    details.payloadLen  = 20;
    details.payloadData = payloadPtr;
    details.MKILen      = mkiLength;
    details.MKIData     = mkiPtr;
    details.authTagLen  = authTagLenInBytes;
    details.authTagData = authTagPtr;    
    
      

      
    HBufC8* packet = BuildPacketL(ETrue, details);
    CleanupStack::PushL(packet);
    TInt packetLength = packet->Length();
        
    iCryptoHandlerSRTP->InitializeEncryptedPacketL(*packet);
    
    
   
    // just a dummy packet index
    iCryptoHandlerSRTP->iPktIndex = 5;

    iCryptoHandlerSRTP->DeriveSessionKeysL();

    EUNIT_ASSERT( iCryptoHandlerSRTP->iSessionEncrKey->Compare(
                                    *iRFC3711_SessionEncrKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTP->iSessionSaltKey->Compare(
                                    *iRFC3711_SessionSaltKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTP->iSessionAuthKey->Compare(
                                    *iRFC3711_SessionAuthKey128bits) == 0);
   CleanupStack::PopAndDestroy(packet);  
    CleanupStack::PopAndDestroy(mki);                                   
    }


void UT_CSRTPCryptoHandlerSRTP::UT_UpdateROCL(  )
    {
    //Create the packet firest 
     TInt authTagLenInBytes = iContext->CryptoParams().iSrtpAuthTagLen/8;
        
    TBuf8<32> payload;
    TBuf8<32> authTag;        
    TInt ssrc = 500;
//        TUint8* ssrcPtr = static_cast<TUint8 *>(&ssrc);

    
    for (TInt i=0; i<20; i++)
        {
        payload.Append(0xAA);            
        }        

    authTag.Append(0x79);
    authTag.Append(0xD0);
    authTag.Append(0xAB);
    authTag.Append(0x19);
    authTag.Append(0xF2);
    authTag.Append(0xC9);
    authTag.Append(0xCB);
    authTag.Append(0x35);
    authTag.Append(0x16);
    authTag.Append(0x45);

	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	TInt mkiLength = mki->Length();
	
    const TUint8* payloadPtr = payload.PtrZ();
    const TUint8* mkiPtr = mki->Des().PtrZ();
    const TUint8* authTagPtr = authTag.PtrZ();
    const TUint8* decryptedPayloadPtr = iDecryptedPayload->Ptr();
    
    TPayloadDetails details;    

    details.seqNr       = 0;//KTestSeqNr17;
//        details.ssrc        = ssrcPtr;
    details.padding     = 0;
    details.numCSRC     = 0;
    details.fHeaderExtension = EFalse;
    details.payloadLen  = 20;
    details.payloadData = payloadPtr;
    details.MKILen      = mkiLength;
    details.MKIData     = mkiPtr;
    details.authTagLen  = authTagLenInBytes;
    details.authTagData = authTagPtr;    
    
      

      
    HBufC8* packet = BuildPacketL(ETrue, details);
    CleanupStack::PushL(packet);
    TInt packetLength = packet->Length();
    //iCryptoHandlerSRTP->iCurrentSeq = 0;    
    iCryptoHandlerSRTP->InitializeEncryptedPacketL(*packet);
     // cond. update of ROC in sender side
    iCryptoHandlerSRTP->UpdateROC();
    // ROC should not wrap when SEQ == 0 !!!
    EUNIT_ASSERT( iCryptoHandlerSRTP->ROC() == 0);
	CleanupStack::PopAndDestroy(packet);
	
	
	
    // set SEQ == 65534
    //iCryptoHandlerSRTP->iCurrentSeq = 0x10000 - 2;
    details.seqNr       = 65534;
    
    // ROC should not wrap when SEQ == 65534 !!!    
    HBufC8* packet1 = BuildPacketL(ETrue, details);
    CleanupStack::PushL(packet);
    packetLength = packet->Length();
    //iCryptoHandlerSRTP->iCurrentSeq = 0;    
    iCryptoHandlerSRTP->InitializeEncryptedPacketL(*packet);
     // cond. update of ROC in sender side
    iCryptoHandlerSRTP->UpdateROC();
    // ROC should not wrap when SEQ == 0 !!!
    EUNIT_ASSERT( iCryptoHandlerSRTP->ROC() == 0);
	CleanupStack::PopAndDestroy(packet1);


    // set SEQ == 65535
    //iCryptoHandlerSRTP->iCurrentSeq = 0x10000 - 1;
    details.seqNr       = 65535;
	HBufC8* packet2 = BuildPacketL(ETrue, details);
    CleanupStack::PushL(packet);
    packetLength = packet->Length();
    //iCryptoHandlerSRTP->iCurrentSeq = 0;    
    iCryptoHandlerSRTP->InitializeEncryptedPacketL(*packet);
     // cond. update of ROC in sender side
    iCryptoHandlerSRTP->UpdateROC();
    // ROC should be updated now, as SEQ wraps !!!    
    EUNIT_ASSERT( iCryptoHandlerSRTP->ROC() == 1);
    CleanupStack::PopAndDestroy(packet2);  
    CleanupStack::PopAndDestroy(mki);      
    }

void UT_CSRTPCryptoHandlerSRTP::CountSenderPacketIndexL(  )
    {
    iReKeyCalled= EFalse;
    //Create the packet firest 
     TInt authTagLenInBytes = iContext->CryptoParams().iSrtpAuthTagLen/8;
        
    TBuf8<32> payload;
    TBuf8<32> authTag;        
    TInt ssrc = 500;
//        TUint8* ssrcPtr = static_cast<TUint8 *>(&ssrc);

    
    for (TInt i=0; i<20; i++)
        {
        payload.Append(0xAA);            
        }        

    authTag.Append(0x79);
    authTag.Append(0xD0);
    authTag.Append(0xAB);
    authTag.Append(0x19);
    authTag.Append(0xF2);
    authTag.Append(0xC9);
    authTag.Append(0xCB);
    authTag.Append(0x35);
    authTag.Append(0x16);
    authTag.Append(0x45);

	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	TInt mkiLength = mki->Length();
	
    const TUint8* payloadPtr = payload.PtrZ();
    const TUint8* mkiPtr = mki->Des().PtrZ();
    const TUint8* authTagPtr = authTag.PtrZ();
    const TUint8* decryptedPayloadPtr = iDecryptedPayload->Ptr();
    
    TPayloadDetails details;    

    details.seqNr       = 0x10000-1;
    details.padding     = 0;
    details.numCSRC     = 0;
    details.fHeaderExtension = EFalse;
    details.payloadLen  = 20;
    details.payloadData = payloadPtr;
    details.MKILen      = mkiLength;
    details.MKIData     = mkiPtr;
    details.authTagLen  = authTagLenInBytes;
    details.authTagData = authTagPtr;   
      
    HBufC8* packet = BuildPacketL(ETrue, details);
    CleanupStack::PushL(packet);
    TInt packetLength = packet->Length();
    
    iCryptoHandlerSRTP->InitializeEncryptedPacketL(*packet);
    iCryptoHandlerSRTP->SetROC(0x100000000-2);
    iCryptoHandlerSRTP->CountSenderPacketIndexL();
    
    EUNIT_ASSERT( iCryptoHandlerSRTP->iReKey );
    //test Rekey
    iReKeyCalled = EFalse;
    iCryptoHandlerSRTP->SetROC( 0 );
    iCryptoHandlerSRTP->iNumPackets = 100;
    CSRTPCryptoHandlerSRTP* srtphandler = static_cast <CSRTPCryptoHandlerSRTP*> (iCryptoHandlerSRTP);
    srtphandler->iSRTPMasterKeyLifeTime = 200;

    iCryptoHandlerSRTP->ReKeyCheck();
    EUNIT_ASSERT(iReKeyCalled== ETrue);
    iCryptoHandlerSRTP->iNumPackets = 200;
    iCryptoHandlerSRTP->ReKeyCheck();
    EUNIT_ASSERT ( iCryptoHandlerSRTP->iReKey );
    
	CleanupStack::PopAndDestroy(packet);
	CleanupStack::PopAndDestroy(mki);      
    }

void UT_CSRTPCryptoHandlerSRTP::ReplayProtectionL1L(  )
    {
        //Create the packet firest 
    TInt authTagLenInBytes = iContext->CryptoParams().iSrtpAuthTagLen/8;
    TBuf8<32> payload;
    TBuf8<32> authTag;        
    TInt ssrc = 500;

    for (TInt i=0; i<20; i++)
        {
        payload.Append(0xAA);            
        }        

    authTag.Append(0x79);
    authTag.Append(0xD0);
    authTag.Append(0xAB);
    authTag.Append(0x19);
    authTag.Append(0xF2);
    authTag.Append(0xC9);
    authTag.Append(0xCB);
    authTag.Append(0x35);
    authTag.Append(0x16);
    authTag.Append(0x45);

	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	TInt mkiLength = mki->Length();
	
    const TUint8* payloadPtr = payload.PtrZ();
    const TUint8* mkiPtr = mki->Des().PtrZ();
    const TUint8* authTagPtr = authTag.PtrZ();
    const TUint8* decryptedPayloadPtr = iDecryptedPayload->Ptr();
    
    TPayloadDetails details;    

    details.seqNr       = KTestSeqNr17;
    details.padding     = 0;
    details.numCSRC     = 0;
    details.fHeaderExtension = EFalse;
    details.payloadLen  = 20;
    details.payloadData = payloadPtr;
    details.MKILen      = mkiLength;
    details.MKIData     = mkiPtr;
    details.authTagLen  = authTagLenInBytes;
    details.authTagData = authTagPtr;    
    
    HBufC8* packet = BuildPacketL(ETrue, details);
    CleanupStack::PushL(packet);
    TInt packetLength = packet->Length();
        
    iCryptoHandlerSRTP->InitializeEncryptedPacketL(*packet);
 	iCryptoHandlerSRTP->SetReceiver_s_l();
    iCryptoHandlerSRTP->CountReceiverPacketIndexL();
             
    // derive session keys..
    iCryptoHandlerSRTP->DeriveSessionKeysL();
    // do the replay protection 
    iCryptoHandlerSRTP->ReplayProtectionL();
    iCryptoHandlerSRTP->Update_s_l_and_RocL();
    iCryptoHandlerSRTP->AddReplayIndex();
    EUNIT_ASSERT(iCryptoHandlerSRTP->iPktIndex == KTestSeqNr17); 
   	CleanupStack::PopAndDestroy(packet);  
    
    //Create the packet again so can check the Replay protection
   	details.seqNr       = KTestSeqNr18;             
	HBufC8* packet2 = BuildPacketL(ETrue, details);
    CleanupStack::PushL(packet2);
    
        
    iCryptoHandlerSRTP->InitializeEncryptedPacketL(*packet2);
 
    iCryptoHandlerSRTP->SetReceiver_s_l();
       
    
    iCryptoHandlerSRTP->CountReceiverPacketIndexL();
    iCryptoHandlerSRTP->ReplayProtectionL();
    
    
    iCryptoHandlerSRTP->Update_s_l_and_RocL();
    iCryptoHandlerSRTP->AddReplayIndex(); 
    EUNIT_ASSERT(iCryptoHandlerSRTP->iPktIndex == KTestSeqNr18);
   	CleanupStack::PopAndDestroy(packet);  
    CleanupStack::PopAndDestroy(mki);          
    }
    
void UT_CSRTPCryptoHandlerSRTP::ReplayProtectionL2L(  )
    {
    //Set stored PktIndx number to bigger than the on in the sequence number
    //but inside the Replay window and has not been seen
    iCryptoHandlerSRTP->iPktIndex= KTestSeqNr18;
    //Create the packet firest 
    TInt authTagLenInBytes = iContext->CryptoParams().iSrtpAuthTagLen/8;
    TBuf8<32> payload;
    TBuf8<32> authTag;        
    TInt ssrc = 500;

    for (TInt i=0; i<20; i++)
        {
        payload.Append(0xAA);            
        }        

    authTag.Append(0x79);
    authTag.Append(0xD0);
    authTag.Append(0xAB);
    authTag.Append(0x19);
    authTag.Append(0xF2);
    authTag.Append(0xC9);
    authTag.Append(0xCB);
    authTag.Append(0x35);
    authTag.Append(0x16);
    authTag.Append(0x45);

	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	TInt mkiLength = mki->Length();
	
    const TUint8* payloadPtr = payload.PtrZ();
    const TUint8* mkiPtr = mki->Des().PtrZ();
    const TUint8* authTagPtr = authTag.PtrZ();
    const TUint8* decryptedPayloadPtr = iDecryptedPayload->Ptr();
    
    TPayloadDetails details;    

    details.seqNr       = KTestSeqNr17;
    details.padding     = 0;
    details.numCSRC     = 0;
    details.fHeaderExtension = EFalse;
    details.payloadLen  = 20;
    details.payloadData = payloadPtr;
    details.MKILen      = mkiLength;
    details.MKIData     = mkiPtr;
    details.authTagLen  = authTagLenInBytes;
    details.authTagData = authTagPtr;    
    
    HBufC8* packet = BuildPacketL(ETrue, details);
    CleanupStack::PushL(packet);
    TInt packetLength = packet->Length();
        
    iCryptoHandlerSRTP->InitializeEncryptedPacketL(*packet);
 	iCryptoHandlerSRTP->SetReceiver_s_l();
    iCryptoHandlerSRTP->CountReceiverPacketIndexL();
             
    // derive session keys..
    iCryptoHandlerSRTP->DeriveSessionKeysL();
    // do the replay protection 
    iCryptoHandlerSRTP->ReplayProtectionL();
    iCryptoHandlerSRTP->Update_s_l_and_RocL();
    iCryptoHandlerSRTP->AddReplayIndex();
    EUNIT_ASSERT(iCryptoHandlerSRTP->iPktIndex == KTestSeqNr18); 
    //check also bitmap later
   	CleanupStack::PopAndDestroy(packet); 
   	CleanupStack::PopAndDestroy(mki);         
    
    }  
    
void UT_CSRTPCryptoHandlerSRTP::ReplayProtectionL3L(  )
    {
    //Set stored PktIndx number to bigger than the on in the sequence number
    //but inside the Replay window and it has been seen
      //Create the packet firest 
    TInt authTagLenInBytes = iContext->CryptoParams().iSrtpAuthTagLen/8;
    TBuf8<32> payload;
    TBuf8<32> authTag;        
    TInt ssrc = 500;

    for (TInt i=0; i<20; i++)
        {
        payload.Append(0xAA);            
        }        

    authTag.Append(0x79);
    authTag.Append(0xD0);
    authTag.Append(0xAB);
    authTag.Append(0x19);
    authTag.Append(0xF2);
    authTag.Append(0xC9);
    authTag.Append(0xCB);
    authTag.Append(0x35);
    authTag.Append(0x16);
    authTag.Append(0x45);

	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	TInt mkiLength = mki->Length();
	
    const TUint8* payloadPtr = payload.PtrZ();
    const TUint8* mkiPtr = mki->Des().PtrZ();
    const TUint8* authTagPtr = authTag.PtrZ();
    const TUint8* decryptedPayloadPtr = iDecryptedPayload->Ptr();
    
    TPayloadDetails details;    

    details.seqNr       = KTestSeqNr17;
    details.padding     = 0;
    details.numCSRC     = 0;
    details.fHeaderExtension = EFalse;
    details.payloadLen  = 20;
    details.payloadData = payloadPtr;
    details.MKILen      = mkiLength;
    details.MKIData     = mkiPtr;
    details.authTagLen  = authTagLenInBytes;
    details.authTagData = authTagPtr;    
    
    HBufC8* packet = BuildPacketL(ETrue, details);
    CleanupStack::PushL(packet);
    TInt packetLength = packet->Length();
        
    iCryptoHandlerSRTP->InitializeEncryptedPacketL(*packet);
 	iCryptoHandlerSRTP->SetReceiver_s_l();
    iCryptoHandlerSRTP->CountReceiverPacketIndexL();
    iCryptoHandlerSRTP->DeriveSessionKeysL();
    iCryptoHandlerSRTP->ReplayProtectionL();
    iCryptoHandlerSRTP->Update_s_l_and_RocL();
    iCryptoHandlerSRTP->AddReplayIndex();
    EUNIT_ASSERT(iCryptoHandlerSRTP->iPktIndex == KTestSeqNr17); 
   	CleanupStack::PopAndDestroy(packet);  
    
    //Create the packet again so can check the Replay protection
   	details.seqNr       = KTestSeqNr18;             
	HBufC8* packet2 = BuildPacketL(ETrue, details);
    CleanupStack::PushL(packet2);
    
        
    iCryptoHandlerSRTP->InitializeEncryptedPacketL(*packet2);
 
    iCryptoHandlerSRTP->SetReceiver_s_l();
       
    
    iCryptoHandlerSRTP->CountReceiverPacketIndexL();
    iCryptoHandlerSRTP->ReplayProtectionL();
    
    
    iCryptoHandlerSRTP->Update_s_l_and_RocL();
    iCryptoHandlerSRTP->AddReplayIndex(); 
    EUNIT_ASSERT(iCryptoHandlerSRTP->iPktIndex == KTestSeqNr18);
   	CleanupStack::PopAndDestroy(packet2);  
   	
   	//Create the packet again so can check the Replay protection
   	details.seqNr       = KTestSeqNr17;             
	HBufC8* packet3 = BuildPacketL(ETrue, details);
    CleanupStack::PushL(packet3);
    
        
    iCryptoHandlerSRTP->InitializeEncryptedPacketL(*packet3);
 
    iCryptoHandlerSRTP->SetReceiver_s_l();
       
    
    iCryptoHandlerSRTP->CountReceiverPacketIndexL();
    EUNIT_ASSERT_SPECIFIC_LEAVE( 
    	iCryptoHandlerSRTP->ReplayProtectionL(), KErrArgument );
    
   	CleanupStack::PopAndDestroy(packet3);  
    CleanupStack::PopAndDestroy(mki);             
    }  
    
void UT_CSRTPCryptoHandlerSRTP::ReplayProtectionL4L(  )
    {
   //Set stored PktIndx number to bigger than the one in the sequence number
    //and outside the Replay window
     
    iCryptoHandlerSRTP->iPktIndex= KTestSeqNr65536;
    //Create the packet firest 
    TInt authTagLenInBytes = iContext->CryptoParams().iSrtpAuthTagLen/8;
    TBuf8<32> payload;
    TBuf8<32> authTag;        
    TInt ssrc = 500;

    for (TInt i=0; i<20; i++)
        {
        payload.Append(0xAA);            
        }        

    authTag.Append(0x79);
    authTag.Append(0xD0);
    authTag.Append(0xAB);
    authTag.Append(0x19);
    authTag.Append(0xF2);
    authTag.Append(0xC9);
    authTag.Append(0xCB);
    authTag.Append(0x35);
    authTag.Append(0x16);
    authTag.Append(0x45);

	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	TInt mkiLength = mki->Length();
	
    const TUint8* payloadPtr = payload.PtrZ();
    const TUint8* mkiPtr = mki->Des().PtrZ();
    const TUint8* authTagPtr = authTag.PtrZ();
    const TUint8* decryptedPayloadPtr = iDecryptedPayload->Ptr();
    
    TPayloadDetails details;    

    details.seqNr       = KTestSeqNr17;
    details.padding     = 0;
    details.numCSRC     = 0;
    details.fHeaderExtension = EFalse;
    details.payloadLen  = 20;
    details.payloadData = payloadPtr;
    details.MKILen      = mkiLength;
    details.MKIData     = mkiPtr;
    details.authTagLen  = authTagLenInBytes;
    details.authTagData = authTagPtr;    
    
    HBufC8* packet = BuildPacketL(ETrue, details);
    CleanupStack::PushL(packet);
    TInt packetLength = packet->Length();
        
    iCryptoHandlerSRTP->InitializeEncryptedPacketL(*packet);
 	iCryptoHandlerSRTP->SetReceiver_s_l();
    iCryptoHandlerSRTP->CountReceiverPacketIndexL();
             
    // derive session keys..
    iCryptoHandlerSRTP->DeriveSessionKeysL();
    // do the replay protection 
    EUNIT_ASSERT_SPECIFIC_LEAVE( 
    	iCryptoHandlerSRTP->ReplayProtectionL(), KErrArgument );
    
    
    
   	CleanupStack::PopAndDestroy(packet); 
   	CleanupStack::PopAndDestroy(mki);      
    }      
    
void UT_CSRTPCryptoHandlerSRTP::TestPacketHeaderAndPayloadL(  )
    {
    //Create the packet first 
     TInt authTagLenInBytes = iContext->CryptoParams().iSrtpAuthTagLen/8;
        
    TBuf8<32> payload;
    TBuf8<32> authTag;        
    TInt ssrc = 500;

    for (TInt i=0; i<20; i++)
        {
        payload.Append(0xAA);            
        }        

    authTag.Append(0x79);
    authTag.Append(0xD0);
    authTag.Append(0xAB);
    authTag.Append(0x19);
    authTag.Append(0xF2);
    authTag.Append(0xC9);
    authTag.Append(0xCB);
    authTag.Append(0x35);
    authTag.Append(0x16);
    authTag.Append(0x45);

	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	TInt mkiLength = mki->Length();
	
    const TUint8* payloadPtr = payload.PtrZ();
    const TUint8* mkiPtr = mki->Des().PtrZ();
    const TUint8* authTagPtr = authTag.PtrZ();
    const TUint8* decryptedPayloadPtr = iDecryptedPayload->Ptr();
    
    TPayloadDetails details;    

    details.seqNr       = KTestSeqNr17;

    details.padding     = 0;
    details.numCSRC     = 0;
    details.fHeaderExtension = EFalse;
    details.payloadLen  = 20;
    details.payloadData = payloadPtr;
    details.MKILen      = mkiLength;
    details.MKIData     = mkiPtr;
    details.authTagLen  = authTagLenInBytes;
    details.authTagData = authTagPtr;    
    
      

      
    HBufC8* packet = BuildPacketL(ETrue, details);
    CleanupStack::PushL(packet);
    TInt packetLength = packet->Length();
        
    iCryptoHandlerSRTP->InitializeEncryptedPacketL(*packet);
   
    details.payloadData = decryptedPayloadPtr;
 
    TPtrC8 nonDecryptedPayload =iCryptoHandlerSRTP->iCurrentPacket->HeaderAndPayload();
	TInt size = nonDecryptedPayload.Size();
	EUNIT_ASSERT(nonDecryptedPayload.Size()==32);
	
	
   	CleanupStack::PopAndDestroy(packet);  
    CleanupStack::PopAndDestroy(mki);          
    
    }    

void UT_CSRTPCryptoHandlerSRTP::TestAuthenticationL1L(  )
	{

	//create normal packets with tag
	TInt authTagLenInBytes = iContext->CryptoParams().iSrtpAuthTagLen/8;
        
    TBuf8<32> payload;
    TBuf8<32> authTag;        
    TInt ssrc = 500;
//        TUint8* ssrcPtr = static_cast<TUint8 *>(&ssrc);

    
    for (TInt i=0; i<20; i++)
        {
        payload.Append(0xAA);            
        }        
	//make the caculate one different from the one here
	//so it can go into different branch in the code
    authTag.Append(0xAA);
    authTag.Append(0xD0);
    authTag.Append(0xAB);
    authTag.Append(0x19);
    authTag.Append(0xF2);
    authTag.Append(0xC9);
    authTag.Append(0xCB);
    authTag.Append(0x35);
    authTag.Append(0x16);
    authTag.Append(0x45);

	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	TInt mkiLength = mki->Length();
	
    const TUint8* payloadPtr = payload.PtrZ();
    const TUint8* mkiPtr = mki->Des().PtrZ();
    const TUint8* authTagPtr = authTag.PtrZ();
    const TUint8* decryptedPayloadPtr = iDecryptedPayload->Ptr();
    
    TPayloadDetails details;    

    details.seqNr       = KTestSeqNr17;
//        details.ssrc        = ssrcPtr;
    details.padding     = 0;
    details.numCSRC     = 0;
    details.fHeaderExtension = EFalse;
    details.payloadLen  = 20;
    details.payloadData = payloadPtr;
    details.MKILen      = mkiLength;
    details.MKIData     = mkiPtr;
    details.authTagLen  = authTagLenInBytes;
    details.authTagData = authTagPtr;    
    
      

      
    HBufC8* packet = BuildPacketL(ETrue, details);
    CleanupStack::PushL(packet);
    TInt packetLength = packet->Length();
        
    
    iCryptoHandlerSRTP->InitializeEncryptedPacketL(*packet);
 	iCryptoHandlerSRTP->SetReceiver_s_l();
    iCryptoHandlerSRTP->CountReceiverPacketIndexL();
             
    // derive session keys..
    iCryptoHandlerSRTP->DeriveSessionKeysL();
    
    RTP_EUNIT_ASSERT_SPECIFIC_LEAVE(iCryptoHandlerSRTP->AuthenticateL(), KErrNotFound);
    //cover the false branch 
    	//update crypto param to NULL
	HBufC8* masterKey = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = KRFC3711_TestMasterKey128bits;

    
    HBufC8* masterSalt =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFC3711_TestMasterSalt112bits;

	
	HBufC8* mki1 =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki1);
	*mki1 = KTestMKI128Bits;
	Hex(*mki1);
	TSrtpCryptoParams params;
	//modify crypto param	
	
	params.iSrtpAuthAlg=EAuthNull;
	params.iSrtpAuthTagLen=0;
	CSRTPMasterKey* masterkey = CSRTPMasterKey::NewLC( *masterKey, *mki1 );
    CSRTPMasterSalt* mastersalt = CSRTPMasterSalt::NewLC( *masterSalt );
    CSRTPCryptoContext* context = CSRTPCryptoContext::NewL(masterkey, mastersalt, params );
    
    CleanupStack::Pop( mastersalt );
    CleanupStack::Pop( masterkey );
    CleanupStack::PopAndDestroy( mki1 );
    CleanupStack::PopAndDestroy( masterSalt );
    CleanupStack::PopAndDestroy( masterKey ); 
    CleanupStack::PushL( context );
    iStreamIn->SetCryptoInL(context);
    CleanupStack::Pop( context );
    iStreamIn->SetROC(0);
	iCryptoHandlerSRTP->InitializeEncryptedPacketL(*packet);
 	iCryptoHandlerSRTP->SetReceiver_s_l();
    iCryptoHandlerSRTP->CountReceiverPacketIndexL();
    
	iCryptoHandlerSRTP->AuthenticateL();
	CleanupStack::Pop(packet);  
	delete packet; 
	CleanupStack::Pop(mki);  
    delete mki;               

	}

void UT_CSRTPCryptoHandlerSRTP::TestUpdate_s_l_and_RocLL()
    {
    //Create the packet firest 
     TInt authTagLenInBytes = iContext->CryptoParams().iSrtpAuthTagLen/8;
        
    TBuf8<32> payload;
    TBuf8<32> authTag;        
    TInt ssrc = 500;
//        TUint8* ssrcPtr = static_cast<TUint8 *>(&ssrc);

    
    for (TInt i=0; i<20; i++)
        {
        payload.Append(0xAA);            
        }        

    authTag.Append(0x79);
    authTag.Append(0xD0);
    authTag.Append(0xAB);
    authTag.Append(0x19);
    authTag.Append(0xF2);
    authTag.Append(0xC9);
    authTag.Append(0xCB);
    authTag.Append(0x35);
    authTag.Append(0x16);
    authTag.Append(0x45);

	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	TInt mkiLength = mki->Length();
	
    const TUint8* payloadPtr = payload.PtrZ();
    const TUint8* mkiPtr = mki->Des().PtrZ();
    const TUint8* authTagPtr = authTag.PtrZ();
    const TUint8* decryptedPayloadPtr = iDecryptedPayload->Ptr();
    
    TPayloadDetails details;    

    details.seqNr       = 0x10000-1;
    details.padding     = 0;
    details.numCSRC     = 0;
    details.fHeaderExtension = EFalse;
    details.payloadLen  = 20;
    details.payloadData = payloadPtr;
    details.MKILen      = mkiLength;
    details.MKIData     = mkiPtr;
    details.authTagLen  = authTagLenInBytes;
    details.authTagData = authTagPtr;   
      
    HBufC8* packet = BuildPacketL(ETrue, details);
    CleanupStack::PushL(packet);
    TInt packetLength = packet->Length();
    
    iCryptoHandlerSRTP->InitializeEncryptedPacketL(*packet);
    iCryptoHandlerSRTP->SetROC(0);
    iCryptoHandlerSRTP->iV=3;
    //Go to more branch
    iCryptoHandlerSRTP->Update_s_l_and_RocL();
    
    //Go to more branch
    iCryptoHandlerSRTP->iV=1;
    iCryptoHandlerSRTP->Update_s_l_and_RocL();
	CleanupStack::PopAndDestroy(packet);

    CleanupStack::PopAndDestroy(mki);      
    }
    
void UT_CSRTPCryptoHandlerSRTP::TestCountReceiverPacketIndexL()
    {
    //Create the packet firest 
     TInt authTagLenInBytes = iContext->CryptoParams().iSrtpAuthTagLen/8;
        
    TBuf8<32> payload;
    TBuf8<32> authTag;        
    TInt ssrc = 500;

    
    for (TInt i=0; i<20; i++)
        {
        payload.Append(0xAA);            
        }        

    authTag.Append(0x79);
    authTag.Append(0xD0);
    authTag.Append(0xAB);
    authTag.Append(0x19);
    authTag.Append(0xF2);
    authTag.Append(0xC9);
    authTag.Append(0xCB);
    authTag.Append(0x35);
    authTag.Append(0x16);
    authTag.Append(0x45);

	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	TInt mkiLength = mki->Length();
	
    const TUint8* payloadPtr = payload.PtrZ();
    const TUint8* mkiPtr = mki->Des().PtrZ();
    const TUint8* authTagPtr = authTag.PtrZ();
    const TUint8* decryptedPayloadPtr = iDecryptedPayload->Ptr();
    
    TPayloadDetails details;    

    details.seqNr       = 11;
    details.padding     = 0;
    details.numCSRC     = 0;
    details.fHeaderExtension = EFalse;
    details.payloadLen  = 20;
    details.payloadData = payloadPtr;
    details.MKILen      = mkiLength;
    details.MKIData     = mkiPtr;
    details.authTagLen  = authTagLenInBytes;
    details.authTagData = authTagPtr;   
      
    HBufC8* packet = BuildPacketL(ETrue, details);
    CleanupStack::PushL(packet);
    TInt packetLength = packet->Length();
    
    iCryptoHandlerSRTP->InitializeEncryptedPacketL(*packet);
    iCryptoHandlerSRTP->SetROC(0);
    iCryptoHandlerSRTP->iS_L=32800;
    //Go to more branch
    iCryptoHandlerSRTP->CountReceiverPacketIndexL();
    iCryptoHandlerSRTP->iS_L=32769;
    //Go to more branch
    iCryptoHandlerSRTP->CountReceiverPacketIndexL();
    
	CleanupStack::PopAndDestroy(packet);

    CleanupStack::PopAndDestroy(mki);      
    }    
    
void UT_CSRTPCryptoHandlerSRTP::TestCheckMasterKeyIdentifierL()
    {
    //Create the packet firest 
     TInt authTagLenInBytes = iContext->CryptoParams().iSrtpAuthTagLen/8;
        
    TBuf8<32> payload;
    TBuf8<32> authTag;        
    TInt ssrc = 500;
    for (TInt i=0; i<20; i++)
        {
        payload.Append(0xAA);            
        }        

    authTag.Append(0x79);
    authTag.Append(0xD0);
    authTag.Append(0xAB);
    authTag.Append(0x19);
    authTag.Append(0xF2);
    authTag.Append(0xC9);
    authTag.Append(0xCB);
    authTag.Append(0x35);
    authTag.Append(0x16);
    authTag.Append(0x45);

	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits2().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits2;
	Hex(*mki);
	TInt mkiLength = mki->Length();
	
    const TUint8* payloadPtr = payload.PtrZ();
    const TUint8* mkiPtr = mki->Des().PtrZ();
    const TUint8* authTagPtr = authTag.PtrZ();
    const TUint8* decryptedPayloadPtr = iDecryptedPayload->Ptr();
    
    TPayloadDetails details;    

    details.seqNr       = 11;
    details.padding     = 0;
    details.numCSRC     = 0;
    details.fHeaderExtension = EFalse;
    details.payloadLen  = 20;
    details.payloadData = payloadPtr;
    details.MKILen      = mkiLength;
    details.MKIData     = mkiPtr;
    details.authTagLen  = authTagLenInBytes;
    details.authTagData = authTagPtr;   
      
    HBufC8* packet = BuildPacketL(ETrue, details);
    CleanupStack::PushL(packet);
    TInt packetLength = packet->Length();
    
    iCryptoHandlerSRTP->InitializeEncryptedPacketL(*packet);
    iCryptoHandlerSRTP->SetROC(0);
    EUNIT_ASSERT_SPECIFIC_LEAVE(iCryptoHandlerSRTP->CheckMasterKeyIdentifierL(), KErrNotFound);
	CleanupStack::PopAndDestroy(packet);

    CleanupStack::PopAndDestroy(mki);      
    }    
 
void UT_CSRTPCryptoHandlerSRTP::TestCountEncryptedPacketSizeL()
    {
    HBufC8* masterKey = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = KRFC3711_TestMasterKey128bits;

    
    HBufC8* masterSalt =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFC3711_TestMasterSalt112bits;

	
	HBufC8* mki1 =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki1);
	*mki1 = KTestMKI128Bits;
	Hex(*mki1);
	TSrtpCryptoParams params;
	//modify crypto param	
	
	params.iSrtpAuthAlg=EAuthRCCm1;
	params.iSrtpAuthTagLen=112;
	params.iROCTransRate=3;
	CSRTPMasterKey* masterkey = CSRTPMasterKey::NewLC( *masterKey, *mki1 );
    CSRTPMasterSalt* mastersalt = CSRTPMasterSalt::NewLC( *masterSalt );
    CSRTPCryptoContext* context = CSRTPCryptoContext::NewL(masterkey, mastersalt, params );
    
    CleanupStack::Pop( mastersalt );
    CleanupStack::Pop( masterkey );
    CleanupStack::PopAndDestroy( mki1 );
    CleanupStack::PopAndDestroy( masterSalt );
    CleanupStack::PopAndDestroy( masterKey ); 
    CleanupStack::PushL( context );
    iStreamIn->SetCryptoInL(context);
    CleanupStack::Pop( context );
    
    //Create the packet firest 
     TInt authTagLenInBytes = iContext->CryptoParams().iSrtpAuthTagLen/8;
        
    TBuf8<32> payload;
    TBuf8<32> authTag;        
    TInt ssrc = 500;
    for (TInt i=0; i<20; i++)
        {
        payload.Append(0xAA);            
        }        

    authTag.Append(0x79);
    authTag.Append(0xD0);
    authTag.Append(0xAB);
    authTag.Append(0x19);
    authTag.Append(0xF2);
    authTag.Append(0xC9);
    authTag.Append(0xCB);
    authTag.Append(0x35);
    authTag.Append(0x16);
    authTag.Append(0x45);

	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits2().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits2;
	Hex(*mki);
	TInt mkiLength = mki->Length();
	
    const TUint8* payloadPtr = payload.PtrZ();
    const TUint8* mkiPtr = mki->Des().PtrZ();
    const TUint8* authTagPtr = authTag.PtrZ();
    const TUint8* decryptedPayloadPtr = iDecryptedPayload->Ptr();
    
    TPayloadDetails details;    

    details.seqNr       = 11;
    details.padding     = 0;
    details.numCSRC     = 0;
    details.fHeaderExtension = EFalse;
    details.payloadLen  = 20;
    details.payloadData = payloadPtr;
    details.MKILen      = mkiLength;
    details.MKIData     = mkiPtr;
    details.authTagLen  = authTagLenInBytes;
    details.authTagData = authTagPtr;   
      
    HBufC8* packet = BuildPacketL(EFalse, details);
    CleanupStack::PushL(packet);
    TInt packetLength = packet->Length();
    
    iCryptoHandlerSRTP->InitializePlainPacketL(*packet);
    iCryptoHandlerSRTP->SetROC(0);
    //check the size
    CSRTPPacket* packet1= iCryptoHandlerSRTP->iCurrentPacket;
    CSRTPPacketRTP* packet2 = static_cast <CSRTPPacketRTP*> (packet1);
    TInt tempSize1 = packet2->CountEncryptedPacketSizeL();
    
    EUNIT_ASSERT(tempSize1== 12 /*RTP header*/ + 20 /*payload*/ + 16 /*MKI*/);
    
	CleanupStack::PopAndDestroy(packet);

    CleanupStack::PopAndDestroy(mki);      
    }    
    
void UT_CSRTPCryptoHandlerSRTP::TestCountEncryptedPacketSize2L()
    {
    //Set Alg to NULL
    //update crypto param to NULL
	HBufC8* masterKey = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = KRFC3711_TestMasterKey128bits;

    
    HBufC8* masterSalt =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFC3711_TestMasterSalt112bits;

	
	HBufC8* mki1 =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki1);
	*mki1 = KTestMKI128Bits;
	Hex(*mki1);
	TSrtpCryptoParams params;
	//modify crypto param	
	
	params.iSrtpAuthAlg=EAuthNull;
	params.iSrtpAuthTagLen=0;
	CSRTPMasterKey* masterkey = CSRTPMasterKey::NewLC( *masterKey, *mki1 );
    CSRTPMasterSalt* mastersalt = CSRTPMasterSalt::NewLC( *masterSalt );
    CSRTPCryptoContext* context = CSRTPCryptoContext::NewL(masterkey, mastersalt, params );
    
    CleanupStack::Pop( mastersalt );
    CleanupStack::Pop( masterkey );
    CleanupStack::PopAndDestroy( mki1 );
    CleanupStack::PopAndDestroy( masterSalt );
    CleanupStack::PopAndDestroy( masterKey ); 
    CleanupStack::PushL( context );
    iStreamIn->SetCryptoInL(context);
    CleanupStack::Pop( context );
      //Create the packet firest 
     TInt authTagLenInBytes = iContext->CryptoParams().iSrtpAuthTagLen/8;
        
    TBuf8<32> payload;
    TBuf8<32> authTag;        
    TInt ssrc = 500;
    for (TInt i=0; i<20; i++)
        {
        payload.Append(0xAA);            
        }        

    authTag.Append(0x79);
    authTag.Append(0xD0);
    authTag.Append(0xAB);
    authTag.Append(0x19);
    authTag.Append(0xF2);
    authTag.Append(0xC9);
    authTag.Append(0xCB);
    authTag.Append(0x35);
    authTag.Append(0x16);
    authTag.Append(0x45);

	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits2().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits2;
	Hex(*mki);
	TInt mkiLength = mki->Length();
	
    const TUint8* payloadPtr = payload.PtrZ();
    const TUint8* mkiPtr = mki->Des().PtrZ();
    const TUint8* authTagPtr = authTag.PtrZ();
    const TUint8* decryptedPayloadPtr = iDecryptedPayload->Ptr();
    
    TPayloadDetails details;    

    details.seqNr       = 11;
    details.padding     = 0;
    details.numCSRC     = 0;
    details.fHeaderExtension = EFalse;
    details.payloadLen  = 20;
    details.payloadData = payloadPtr;
    details.MKILen      = mkiLength;
    details.MKIData     = mkiPtr;
    details.authTagLen  = authTagLenInBytes;
    details.authTagData = authTagPtr;   
      
    HBufC8* packet = BuildPacketL(EFalse, details);
    CleanupStack::PushL(packet);
    TInt packetLength = packet->Length();
    
    iCryptoHandlerSRTP->InitializePlainPacketL(*packet);
    iStreamIn->SetROC(0);
    CSRTPPacket* packet1= iCryptoHandlerSRTP->iCurrentPacket;
    CSRTPPacketRTP* packet2 = static_cast <CSRTPPacketRTP*> (packet1);
    TInt tempSize1 = packet2->CountEncryptedPacketSizeL();
	EUNIT_ASSERT(tempSize1==12+20+16);
	CleanupStack::PopAndDestroy(packet);
    CleanupStack::PopAndDestroy(mki);          
    }
    

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE( 
    UT_CSRTPCryptoHandlerSRTP,
    "CSRTPCryptoHandlerSRTP",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "CSRTPCryptoHandlerSRTP",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPCryptoHandlerSRTP_NewLL, Teardown)

EUNIT_TEST(
    "NewLC - test ",
    "CSRTPCryptoHandlerSRTP",
    "NewLC",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPCryptoHandlerSRTP_NewLCL, Teardown)




EUNIT_TEST(
    "DeriveSessionKeysL - 1 ",
    "CSRTPCryptoHandlerSRTP",
    "DeriveSessionKeysL",
    "FUNCTIONALITY",
    SetupL, UT_DeriveSessionKeysL_OneL, Teardown)

EUNIT_TEST(
    "UpdateROC - 1 ",
    "CSRTPCryptoHandlerSRTP",
    "UpdateROC",
    "FUNCTIONALITY",
    SetupL, UT_UpdateROCL, Teardown)

EUNIT_TEST(
    "CountSenderPacketIndexL ",
    "CSRTPCryptoHandlerSRTP",
    "CountSenderPacketIndexL",
    "FUNCTIONALITY",
    SetupL, CountSenderPacketIndexL, Teardown)

EUNIT_TEST(
    "ReplayProtection1 ",
    "CSRTPCryptoHandlerSRTP",
    "ReplayProtection Normal",
    "FUNCTIONALITY",
    SetupL, ReplayProtectionL1L, Teardown)   
    
EUNIT_TEST(
    "ReplayProtection2 ",
    "CSRTPCryptoHandlerSRTP",
    "ReplayProtection Normal",
    "FUNCTIONALITY",
    SetupL, ReplayProtectionL2L, Teardown)    
    
 EUNIT_TEST(
    "ReplayProtection3 ",
    "CSRTPCryptoHandlerSRTP",
    "In Replay Window",
    "FUNCTIONALITY",
    SetupL, ReplayProtectionL3L, Teardown)    
    
 EUNIT_TEST(
    "ReplayProtection4",
    "CSRTPCryptoHandlerSRTP",
    "In Replay Window",
    "FUNCTIONALITY",
    SetupL, ReplayProtectionL4L, Teardown)    
    
 EUNIT_TEST(
    "PacketHeaderAndPayloadL ",
    "CSRTPCryptoHandlerSRTP",
    "TestPacketHeaderAndPayloadL for CSrtpPacket class",
    "FUNCTIONALITY",
    SetupL, TestPacketHeaderAndPayloadL, Teardown)    

EUNIT_TEST(
    "TestAuthenticationL1L",
    "CSRTPCryptoHandlerSRTP",
    "TestAuthentication gets more coverate",
    "FUNCTIONALITY",
    SetupL, TestAuthenticationL1L, Teardown)        

EUNIT_TEST(
    "Update_s_l_and_Roc",
    "CSRTPCryptoHandlerSRTP",
    "s_l_and_Roc",
    "FUNCTIONALITY",
    SetupL, TestUpdate_s_l_and_RocLL, Teardown)        

EUNIT_TEST(
    "CountReceiverPacketIndex",
    "CSRTPCryptoHandlerSRTP",
    "CountReceiverPacketIndex",
    "FUNCTIONALITY",
    SetupL, TestCountReceiverPacketIndexL, Teardown)        

EUNIT_TEST(
    "CheckMasterKeyIdentifierL",
    "CSRTPCryptoHandlerSRTP",
    "CheckMasterKeyIdentifierL",
    "FUNCTIONALITY",
    SetupL, TestCheckMasterKeyIdentifierL, Teardown)   
EUNIT_TEST(
    "CountEncryptedPacketSize",
    "CSRTPCryptoHandlerSRTP",
    "CountEncryptedPacketSize",
    "FUNCTIONALITY",
    SetupL, TestCountEncryptedPacketSizeL, Teardown)       

EUNIT_TEST(
    "CountEncryptedPktSize2",
    "CSRTPCryptoHandlerSRTP",
    "CountEncryptedPacketSize2",
    "FUNCTIONALITY",
    SetupL, TestCountEncryptedPacketSize2L, Teardown)       

EUNIT_TEST(
    "InitializeEncryptedPacket",
    "CSRTPCryptoHandlerSRTP",
    "InitializeEncryptedPacket",
    "FUNCTIONALITY",
    SetupL, UT_InitializeEncryptedPacketLL, Teardown)   

EUNIT_END_TEST_TABLE

//  END OF FILE
