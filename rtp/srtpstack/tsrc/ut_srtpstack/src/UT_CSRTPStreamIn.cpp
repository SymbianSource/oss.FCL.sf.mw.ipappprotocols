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
#include "UT_CSRTPStreamIn.h"
#include "srtputils.h"

// CONSTANTS
const TInt KRtpVersion = 2;    // Current protocol version          
const TUint KMinRtpHeaderLen( 12 );
const TUint16 KTestSeqNr17 = 17171;
const TUint16 KTestSeqNr18 = 17172;
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
										 
_LIT8(KRFC3711_RtcpEncrKey128bits,   	"4C1AA45A81F73D61C800BBB00FBB1EAA");  
_LIT8(KRFC3711_RtcpSaltKey128bits,   	"9581C7AD87B3E530BF3E4454A8B3");  
_LIT8(KRFC3711_RtcpAuthKey128bits,   	"8D54534FEB49AE8E7993A6BD0B844FC323A93DFD");  

_LIT8(KDecryptedPayload,                "2EDE3286C6E33E68C6BB93350620A1245EC0004F");  

_LIT8(KTestM_without_ROC,               "123456789ABCDE");  
_LIT8(KRefM_with_ROC,                   "123456789ABCDEFFFFFFFF");  

_LIT8(KTestAuthTag,                     "D07919ABC9F235CB4516");  
_LIT8(KDummyRTCPPacket, "dummyrtcp");
_LIT8(KRTCPPacket, "80C90001DEADBEEF9E30159059A23363F0FBDB62DC85AF52875C944495C5B00280000001ABCDEF1234567890ABCDEF1234567890DFC98F01DE16DA30F20D");

_LIT8(KSRTCPPacket2, "80C90001DEADBEEF2BC095F51495D87D35B816BE569A6A1BA6989D5A20B541D680000002ABCDEF1234567890ABCDEF12345678905140BA93E8CDAC0C89D5");


//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include "srtpcryptohandlersrtp.h"
#include "srtpcryptohandlersrtcp.h"
#include "srtpmastersalt.h"
#include "srtpmasterkey.h"
#include "srtpstreamin.h"
#include "srtpsession.h"
#include "srtputils.h"



//  INTERNAL INCLUDES
#include "srtpstreamin.h"

// CONSTRUCTION
UT_CSRTPStreamIn* UT_CSRTPStreamIn::NewL()
    {
    UT_CSRTPStreamIn* self = UT_CSRTPStreamIn::NewLC(); 
    CleanupStack::Pop();

    return self;
    }

UT_CSRTPStreamIn* UT_CSRTPStreamIn::NewLC()
    {
    UT_CSRTPStreamIn* self = new( ELeave ) UT_CSRTPStreamIn();
    CleanupStack::PushL( self );

	self->ConstructL(); 

    return self;
    }

// Destructor (virtual by CBase)
UT_CSRTPStreamIn::~UT_CSRTPStreamIn()
    {
    }

// Default constructor
UT_CSRTPStreamIn::UT_CSRTPStreamIn() 
    {
    }

UT_CSRTPStreamIn::TPayloadDetails::TPayloadDetails() :
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
        
void UT_CSRTPStreamIn::SRTPMasterKeyStaleEvent(const CSRTPStream& /*aStream*/ )
    {
    }
void UT_CSRTPStreamIn::SRTPMasterKeyStaleEvent(const CSRTPSession& /*aSession*/ )
    {
    }

// Second phase construct
void UT_CSRTPStreamIn::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void UT_CSRTPStreamIn::Hex(HBufC8& aString)
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
// UT_CSRTPStreamIn::BuildSrtpPacketL()
// 
// ---------------------------------------------------------------------------
//
HBufC8* UT_CSRTPStreamIn::BuildPacketL(TBool aSrtp, TPayloadDetails& details)
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
        ptr = dataP + payloadStartOffset + details.payloadLen + padCount + details.MKILen;    
    	if (details.authTagLen==14)
    		{
    		details.authTagLen=10;
    		TSRTPUtils::Write32( ptr,0);
  			ptr+=4;  	
    		}
        //auth tag
        
        if (details.authTagLen > 0)
            {
            Mem::Copy( ptr, details.authTagData, details.authTagLen );        
            }            
        }    
    
    return packet;
    }



void UT_CSRTPStreamIn::SetupL(  )
    {

    iDecryptedPayload = HBufC8::NewL(KDecryptedPayload().Length());
	*iDecryptedPayload = KDecryptedPayload;
	Hex(*iDecryptedPayload);
	iDecryptedPayload->Des().SetLength(20);

    iRFC3711_SessionEncrKey128bits = HBufC8::NewL(KRFC3711_SessionEncrKey128bits().Length());
	*iRFC3711_SessionEncrKey128bits = KRFC3711_SessionEncrKey128bits;
	Hex(*iRFC3711_SessionEncrKey128bits);

    iRFC3711_SessionSaltKey128bits = HBufC8::NewL(KRFC3711_SessionSaltKey128bits().Length());
	*iRFC3711_SessionSaltKey128bits = KRFC3711_SessionSaltKey128bits;
	Hex(*iRFC3711_SessionSaltKey128bits);

    iRFC3711_SessionAuthKey128bits = HBufC8::NewL(KRFC3711_SessionAuthKey128bits().Length());
	*iRFC3711_SessionAuthKey128bits = KRFC3711_SessionAuthKey128bits;
	Hex(*iRFC3711_SessionAuthKey128bits);
	
    iRFC3711_RtcpEncrKey128bits = HBufC8::NewL(KRFC3711_RtcpEncrKey128bits().Length());
	*iRFC3711_RtcpEncrKey128bits = KRFC3711_RtcpEncrKey128bits;
	Hex(*iRFC3711_RtcpEncrKey128bits);

    iRFC3711_RtcpSaltKey128bits = HBufC8::NewL(KRFC3711_RtcpSaltKey128bits().Length());
	*iRFC3711_RtcpSaltKey128bits = KRFC3711_RtcpSaltKey128bits;
	Hex(*iRFC3711_RtcpSaltKey128bits);

    iRFC3711_RtcpAuthKey128bits = HBufC8::NewL(KRFC3711_RtcpAuthKey128bits().Length());
	*iRFC3711_RtcpAuthKey128bits = KRFC3711_RtcpAuthKey128bits;
	Hex(*iRFC3711_RtcpAuthKey128bits);
	
	
	    
	iTestPayload160Bits = HBufC8::NewL(KTestPayload160Bits().Length());
	*iTestPayload160Bits = KTestPayload160Bits;	
	Hex(*iTestPayload160Bits);	
    

    // our test MKI is 16 bit long:
    // ABCDEF1234567890ABCDEF1234567890

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
    iStreamInLateBD = CSRTPStreamIn::NewL(*iSRTPSession);
	iCryptoHandlerSRTP = iStreamIn->iHandlerRTP;
	iCryptoHandlerSRTCP =iStreamIn->iHandlerRTCP;
	iCryptoHandlerSRTP2 = iStreamInLateBD->iHandlerRTP;
	iCryptoHandlerSRTCP2 =iStreamInLateBD->iHandlerRTCP;
    } 


void UT_CSRTPStreamIn::Teardown(  )
    {
    delete iDecryptedPayload;
    delete iTestPayload160Bits;
	
	iDecryptedPayload = NULL;
    iTestPayload160Bits = NULL;



    delete iStreamIn;
    delete iStreamInLateBD;
    delete iSRTPSession;    
	
	iStreamIn = NULL;
    iStreamInLateBD = NULL;
    iSRTPSession = NULL; 

   
    delete iRFC3711_SessionEncrKey128bits;
    delete iRFC3711_SessionSaltKey128bits;
    delete iRFC3711_SessionAuthKey128bits;    
    delete iRFC3711_RtcpEncrKey128bits;
	delete iRFC3711_RtcpSaltKey128bits;
	delete iRFC3711_RtcpAuthKey128bits;
	
	iRFC3711_SessionEncrKey128bits = NULL;
    iRFC3711_SessionSaltKey128bits = NULL;
    iRFC3711_SessionAuthKey128bits = NULL;    
    iRFC3711_RtcpEncrKey128bits = NULL;
	iRFC3711_RtcpSaltKey128bits = NULL;
	iRFC3711_RtcpAuthKey128bits = NULL;
    }

void UT_CSRTPStreamIn::UT_CSRTPStreamIn_NewL_OneL(  )
    {        
    CSRTPStreamIn* tempStreamIn = CSRTPStreamIn::NewL(*iSRTPSession, (TUint)1);
    EUNIT_ASSERT(tempStreamIn->SSRC()== 1);
    delete tempStreamIn;
    }

void UT_CSRTPStreamIn::UT_CSRTPStreamIn_NewL_TwoL(  )
    {
    HBufC8* masterKey = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = KRFC3711_TestMasterKey128bits;

    
    HBufC8* masterSalt =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFC3711_TestMasterSalt112bits;

	
	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KRFC3711_TestMasterSalt112bits;
	Hex(*mki);
	TSrtpCryptoParams params;
	CSRTPMasterKey* masterkey = CSRTPMasterKey::NewLC( *masterKey, *mki );
    CSRTPMasterSalt* mastersalt = CSRTPMasterSalt::NewLC( *masterSalt );
    CSRTPCryptoContext* context = CSRTPCryptoContext::NewL(masterkey, mastersalt, params );
    CleanupStack::Pop( mastersalt );
    CleanupStack::Pop( masterkey );
    CleanupStack::PopAndDestroy( mki );
    CleanupStack::PopAndDestroy( masterSalt );
    CleanupStack::PopAndDestroy( masterKey ); 
    CleanupStack::PushL( context );
    CSRTPSession* srtpSession = CSRTPSession::NewL( iDestination, context, *this );     
    CleanupStack::Pop( context ); 
    CleanupStack::PushL( srtpSession );                      
    CSRTPStreamIn* tempStreamIn =CSRTPStreamIn::NewL( *srtpSession,
                                      (TUint)1,
                                      *this );
    EUNIT_ASSERT(tempStreamIn->SSRC()== 1);
    delete tempStreamIn;    
     CleanupStack::PopAndDestroy( srtpSession );                   
    }

void UT_CSRTPStreamIn::UT_CSRTPStreamIn_NewL_ThreeL(  )
    {
    HBufC8* masterKey = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = KRFC3711_TestMasterKey128bits;

    
    HBufC8* masterSalt =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFC3711_TestMasterSalt112bits;

	
	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KRFC3711_TestMasterSalt112bits;
	Hex(*mki);
	TSrtpCryptoParams params;
	CSRTPMasterKey* masterkey = CSRTPMasterKey::NewLC( *masterKey, *mki );
    CSRTPMasterSalt* mastersalt = CSRTPMasterSalt::NewLC( *masterSalt );
    CSRTPCryptoContext* context = CSRTPCryptoContext::NewL(masterkey, mastersalt, params );
    
    CleanupStack::Pop( mastersalt );
    CleanupStack::Pop( masterkey );
    CleanupStack::PopAndDestroy( mki );
    CleanupStack::PopAndDestroy( masterSalt );
    CleanupStack::PopAndDestroy( masterKey ); 
    CleanupStack::PushL( context );
    CSRTPSession* srtpSession = CSRTPSession::NewL( iDestination, context, *this ); 
    CleanupStack::Pop( context );
    CleanupStack::PushL( srtpSession );
    CSRTPStreamIn* tempStreamIn =CSRTPStreamIn::NewL( *srtpSession);
    
    EUNIT_ASSERT(tempStreamIn->GetCryptoContext().Valid()==ETrue); 
    delete tempStreamIn;   
    CleanupStack::PopAndDestroy( srtpSession );                   
    }
void UT_CSRTPStreamIn::UT_CSRTPStreamIn_NewL_FourL(  )
    {
     HBufC8* masterKey = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = KRFC3711_TestMasterKey128bits;

    
    HBufC8* masterSalt =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFC3711_TestMasterSalt112bits;

	
	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KRFC3711_TestMasterSalt112bits;
	Hex(*mki);
	TSrtpCryptoParams params;
	CSRTPMasterKey* masterkey = CSRTPMasterKey::NewLC( *masterKey, *mki );
    CSRTPMasterSalt* mastersalt = CSRTPMasterSalt::NewLC( *masterSalt );
    CSRTPCryptoContext* context = CSRTPCryptoContext::NewL(masterkey, mastersalt, params );
    
    CleanupStack::Pop( mastersalt );
    CleanupStack::Pop( masterkey );
    CleanupStack::PopAndDestroy( mki );
    CleanupStack::PopAndDestroy( masterSalt );
    CleanupStack::PopAndDestroy( masterKey ); 
    CleanupStack::PushL( context );
    CSRTPSession* srtpSession = CSRTPSession::NewL( iDestination, context, *this ); 
    CleanupStack::Pop( context );
    CleanupStack::PushL( srtpSession );
    CSRTPStreamIn* tempStreamIn =CSRTPStreamIn::NewL( *srtpSession, *this );
    
    EUNIT_ASSERT(tempStreamIn->GetCryptoContext().Valid()==ETrue); 
    delete tempStreamIn;  
    CleanupStack::PopAndDestroy( srtpSession );
    }
    
void UT_CSRTPStreamIn::UT_CSRTPStreamIn_NewL_FiveL(  )
    {
     HBufC8* masterKey = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = KRFC3711_TestMasterKey128bits;

    
    HBufC8* masterSalt =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFC3711_TestMasterSalt112bits;

	
	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KRFC3711_TestMasterSalt112bits;
	Hex(*mki);
	TSrtpCryptoParams params;
	CSRTPMasterKey* masterkey = CSRTPMasterKey::NewLC( *masterKey, *mki );
    CSRTPMasterSalt* mastersalt = CSRTPMasterSalt::NewLC( *masterSalt );
    CSRTPCryptoContext* context = CSRTPCryptoContext::NewL(masterkey, mastersalt, params );
    
    CleanupStack::Pop( mastersalt );
    CleanupStack::Pop( masterkey );
    CleanupStack::PopAndDestroy( mki );
    CleanupStack::PopAndDestroy( masterSalt );
    CleanupStack::PopAndDestroy( masterKey ); 
    CleanupStack::PushL( context );
    CSRTPSession* srtpSession = CSRTPSession::NewL( iDestination); 
    CleanupStack::PushL( srtpSession );
    CSRTPStreamIn* tempStreamIn =CSRTPStreamIn::NewL( *srtpSession, context, *this );
    CleanupStack::Pop( srtpSession );
    CleanupStack::Pop( context );
    
    
    EUNIT_ASSERT(tempStreamIn->GetCryptoContext().Valid()==ETrue); 
    delete tempStreamIn;  
    delete srtpSession;                       
    }
   
void UT_CSRTPStreamIn::UT_CSRTPStreamIn_SetCryptoInL(  )
	{
	 HBufC8* masterKey = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = KRFC3711_TestMasterKey128bits;

    
    HBufC8* masterSalt =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFC3711_TestMasterSalt112bits;

	
	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KRFC3711_TestMasterSalt112bits;
	Hex(*mki);
	TSrtpCryptoParams params;
	CSRTPMasterKey* masterkey = CSRTPMasterKey::NewLC( *masterKey, *mki );
    CSRTPMasterSalt* mastersalt = CSRTPMasterSalt::NewLC( *masterSalt );
    CSRTPCryptoContext* context = CSRTPCryptoContext::NewL(masterkey, mastersalt, params );
    
    CleanupStack::Pop( mastersalt );
    CleanupStack::Pop( masterkey );
    CleanupStack::PopAndDestroy( mki );
    CleanupStack::PopAndDestroy( masterSalt );
    CleanupStack::PopAndDestroy( masterKey ); 
    CleanupStack::PushL( context );
    CSRTPSession* srtpSession = CSRTPSession::NewL( iDestination); 
    CleanupStack::PushL( srtpSession ); 
    CSRTPStreamIn* tempStreamIn =CSRTPStreamIn::NewL( *srtpSession, *this );
    CleanupStack::PushL( tempStreamIn );
    CSRTPCryptoContext* context2 =NULL;
    CSRTPCryptoContext* context3 =context;
    EUNIT_ASSERT_SPECIFIC_LEAVE(tempStreamIn->SetCryptoInL(context2), KErrArgument);
    tempStreamIn->SetCryptoInL(context);
    CleanupStack::Pop( tempStreamIn );
    CleanupStack::Pop( srtpSession );
    CleanupStack::Pop( context );
    CleanupStack::PushL( srtpSession );
    CleanupStack::PushL( tempStreamIn );
    EUNIT_ASSERT(tempStreamIn->GetCryptoContext().Valid()==ETrue); 
    //iRekey is True
    tempStreamIn->iRekey=ETrue;
    tempStreamIn->SetCryptoInL(context);
    
    //with ssrc
    tempStreamIn->SetSSRC(1);
     tempStreamIn->SetCryptoInL(context3);
     
     CleanupStack::PopAndDestroy( tempStreamIn );
     CleanupStack::PopAndDestroy( srtpSession );                      
	}

void UT_CSRTPStreamIn::UT_CSRTPStreamIn_UnprotectSrtpLL(  )
    {
    
    TInt authTagLenInBytes = (iContext->CryptoParams().iSrtpAuthTagLen)/8;
        
    TBuf8<32> payload;
    TBuf8<32> authTag;        
    TInt ssrc = 500;

    
    for (TInt i=0; i<20; i++)
        {
        payload.Append(0xAA);            
        }        

	payload.SetLength(20);
	
    authTag.Append(0x6F);
    authTag.Append(0x35);
    authTag.Append(0x39);
    authTag.Append(0x16);
    authTag.Append(0x8C);
    authTag.Append(0xBB);
    authTag.Append(0xAD);
    authTag.Append(0xF5);
    authTag.Append(0xFD);
    authTag.Append(0x4B);

	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	TInt mkiLength = mki->Length();
	
    const TUint8* payloadPtr = payload.PtrZ();
    const TUint8* mkiPtr =mki->Ptr();// mki->Des().PtrZ();
    const TUint8* authTagPtr = authTag.PtrZ();
    const TUint8* decryptedPayloadPtr = iDecryptedPayload->Des().PtrZ();
    
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
    
      
    HBufC8* srtpPacket = BuildPacketL(ETrue, details);
    CleanupStack::PushL(srtpPacket);
    
    details.payloadData = decryptedPayloadPtr;
    
    HBufC8* refRtpPacket = BuildPacketL(EFalse, details);
    CleanupStack::PushL(refRtpPacket);
 
 	
    HBufC8* rtpPacket = iStreamIn->UnprotectSrtpL(*srtpPacket);
    CleanupStack::PushL(rtpPacket);
    
    EUNIT_ASSERT( iCryptoHandlerSRTP->iSessionEncrKey->Compare(
                                    *iRFC3711_SessionEncrKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTP->iSessionSaltKey->Compare(
                                    *iRFC3711_SessionSaltKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTP->iSessionAuthKey->Compare(
                                    *iRFC3711_SessionAuthKey128bits) == 0);    
    EUNIT_ASSERT( refRtpPacket->Compare(*rtpPacket) == 0);        
    
    EUNIT_ASSERT(iStreamIn->iCurrentRTPState==
    				MSRTPStreamInContext::ESRTPStreamInNormal);
    CleanupStack::Pop(3);
    delete rtpPacket;
    delete refRtpPacket;
    delete srtpPacket;
    CleanupStack::PopAndDestroy(mki);
    
    }
    
void UT_CSRTPStreamIn::UT_CSRTPStreamIn_UnprotectSrtpL2L(  )
    {
    //Test Late Binding state
    TInt authTagLenInBytes = iContext->CryptoParams().iSrtpAuthTagLen/8;
        
    TBuf8<32> payload;
    TBuf8<32> authTag;        
    TInt ssrc = 500;

    for (TInt i=0; i<20; i++)
        {
        payload.Append(0xAA);            
        }        

	payload.SetLength(20);
	
    authTag.Append(0x6F);
    authTag.Append(0x35);
    authTag.Append(0x39);
    authTag.Append(0x16);
    authTag.Append(0x8C);
    authTag.Append(0xBB);
    authTag.Append(0xAD);
    authTag.Append(0xF5);
    authTag.Append(0xFD);
    authTag.Append(0x4B);

	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	TInt mkiLength = mki->Length();
	
    const TUint8* payloadPtr = payload.PtrZ();
    const TUint8* mkiPtr =mki->Ptr();// mki->Des().PtrZ();
    const TUint8* authTagPtr = authTag.PtrZ();
    const TUint8* decryptedPayloadPtr = iDecryptedPayload->Des().PtrZ();
    
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
    
      
    HBufC8* srtpPacket = BuildPacketL(ETrue, details);
    CleanupStack::PushL(srtpPacket);
    
    details.payloadData = decryptedPayloadPtr;
    
    HBufC8* refRtpPacket = BuildPacketL(EFalse, details);
    CleanupStack::PushL(refRtpPacket);
 
 	
    HBufC8* rtpPacket = iStreamInLateBD->UnprotectSrtpL(*srtpPacket);
    CleanupStack::PushL(rtpPacket);
    
    EUNIT_ASSERT( iCryptoHandlerSRTP2->iSessionEncrKey->Compare(
                                    *iRFC3711_SessionEncrKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTP2->iSessionSaltKey->Compare(
                                    *iRFC3711_SessionSaltKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTP2->iSessionAuthKey->Compare(
                                    *iRFC3711_SessionAuthKey128bits) == 0);    
   
    EUNIT_ASSERT(iStreamInLateBD->iCurrentRTPState==
    				MSRTPStreamInContext::ESRTPStreamInNormal);
    CleanupStack::Pop(3);
    delete rtpPacket;
    delete refRtpPacket;
    delete srtpPacket;
    CleanupStack::PopAndDestroy(mki);
    }    
 
void UT_CSRTPStreamIn::UT_CSRTPStreamIn_UnprotectSrtpL3L(  )
    {
    //Test RCC mode
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
	params.iSrtpAuthAlg=EAuthRCCm1;
	params.iSrtpAuthTagLen=112;
	CSRTPMasterKey* masterkey = CSRTPMasterKey::NewLC( *masterKey, *mki1 );
    CSRTPMasterSalt* mastersalt = CSRTPMasterSalt::NewLC( *masterSalt );
    CSRTPCryptoContext* context = CSRTPCryptoContext::NewL(masterkey, mastersalt, params );
    
    CleanupStack::Pop( mastersalt );
    CleanupStack::Pop( masterkey );
    CleanupStack::PopAndDestroy( mki1 );
    CleanupStack::PopAndDestroy( masterSalt );
    CleanupStack::PopAndDestroy( masterKey ); 
    CleanupStack::PushL( context );
    //update context
    iStreamIn->SetCryptoInL(context);
    CleanupStack::Pop( context );
    iStreamIn->SetROC(0);
    
    TInt authTagLenInBytes = (iContext->CryptoParams().iSrtpAuthTagLen)/8;
        
    TBuf8<32> payload;
    TBuf8<32> authTag;        
    TInt ssrc = 500;
    
    for (TInt i=0; i<20; i++)
        {
        payload.Append(0xAA);            
        }        

	payload.SetLength(20);
	
    authTag.Append(0x6F);
    authTag.Append(0x35);
    authTag.Append(0x39);
    authTag.Append(0x16);
    authTag.Append(0x8C);
    authTag.Append(0xBB);
    authTag.Append(0xAD);
    authTag.Append(0xF5);
    authTag.Append(0xFD);
    authTag.Append(0x4B);

	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	TInt mkiLength = mki->Length();
	
    const TUint8* payloadPtr = payload.PtrZ();
    const TUint8* mkiPtr =mki->Ptr();
    const TUint8* authTagPtr = authTag.PtrZ();
    const TUint8* decryptedPayloadPtr = iDecryptedPayload->Des().PtrZ();
    
    TPayloadDetails details;    

    details.seqNr       = KTestSeqNr17;
    details.padding     = 0;
    details.numCSRC     = 0;
    details.fHeaderExtension = EFalse;
    details.payloadLen  = 20;
    details.payloadData = payloadPtr;
    details.MKILen      = mkiLength;
    details.MKIData     = mkiPtr;
    details.authTagLen  = authTagLenInBytes+4;
    details.authTagData = authTagPtr;    
    
      
    HBufC8* srtpPacket = BuildPacketL(ETrue, details);
    CleanupStack::PushL(srtpPacket);
    
    details.payloadData = decryptedPayloadPtr;
    
    HBufC8* refRtpPacket = BuildPacketL(EFalse, details);
    CleanupStack::PushL(refRtpPacket);
 
 	
    HBufC8* rtpPacket = iStreamIn->UnprotectSrtpL(*srtpPacket);
    CleanupStack::PushL(rtpPacket);
    
    EUNIT_ASSERT( iCryptoHandlerSRTP->iSessionEncrKey->Compare(
                                    *iRFC3711_SessionEncrKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTP->iSessionSaltKey->Compare(
                                    *iRFC3711_SessionSaltKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTP->iSessionAuthKey->Compare(
                                    *iRFC3711_SessionAuthKey128bits) == 0);    
    EUNIT_ASSERT( refRtpPacket->Compare(*rtpPacket) == 0);        
    
    EUNIT_ASSERT(iStreamIn->iCurrentRTPState==
    				MSRTPStreamInContext::ESRTPStreamInNormal);
    CleanupStack::Pop(3);
    delete rtpPacket;
    delete refRtpPacket;
    delete srtpPacket;
    CleanupStack::PopAndDestroy(mki);
    }    
    

void UT_CSRTPStreamIn::UT_CSRTPStreamIn_UnprotectSrtcpLL(  )
    {
    HBufC8* encSrtcpPacket =HBufC8::NewLC(KRTCPPacket().Length());
	*encSrtcpPacket=KRTCPPacket;
    Hex(*encSrtcpPacket);
    TInt len= KRTCPPacket().Length();
	encSrtcpPacket->Des().SetLength(len/2);

    HBufC8* rtcpPacket =iStreamIn->UnprotectSrtcpL(*encSrtcpPacket);
    CleanupStack::PopAndDestroy(encSrtcpPacket);
    CleanupStack::PushL(rtcpPacket);
    EUNIT_ASSERT( iCryptoHandlerSRTCP->iSessionEncrKey->Compare(
                                    *iRFC3711_RtcpEncrKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTCP->iSessionSaltKey->Compare(
                                    *iRFC3711_RtcpSaltKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTCP->iSessionAuthKey->Compare(
                                    *iRFC3711_RtcpAuthKey128bits) == 0);   
    EUNIT_ASSERT(iStreamIn->iCurrentRTCPState==
    				MSRTPStreamInContext::ESRTPStreamInNormal);                                 
    CleanupStack::PopAndDestroy(rtcpPacket);                                
    
    }
    
void UT_CSRTPStreamIn::UT_CSRTPStreamIn_UnprotectSrtcpL2L(  )
    {
    HBufC8* encSrtcpPacket =HBufC8::NewLC(KRTCPPacket().Length());
	*encSrtcpPacket=KRTCPPacket;
    Hex(*encSrtcpPacket);
    TInt len= KRTCPPacket().Length();
	encSrtcpPacket->Des().SetLength(len/2);

    HBufC8* rtcpPacket =iStreamInLateBD->UnprotectSrtcpL(*encSrtcpPacket);
    CleanupStack::PopAndDestroy(encSrtcpPacket);
    CleanupStack::PushL(rtcpPacket);
    EUNIT_ASSERT( iCryptoHandlerSRTCP2->iSessionEncrKey->Compare(
                                    *iRFC3711_RtcpEncrKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTCP2->iSessionSaltKey->Compare(
                                    *iRFC3711_RtcpSaltKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTCP2->iSessionAuthKey->Compare(
                                    *iRFC3711_RtcpAuthKey128bits) == 0);   
    EUNIT_ASSERT(iStreamInLateBD->iCurrentRTCPState==
    				MSRTPStreamInContext::ESRTPStreamInNormal);                                 
    CleanupStack::PopAndDestroy(rtcpPacket);                                
    
    }    
void UT_CSRTPStreamIn::UT_CSRTPStreamIn_StateTestsL()
	{
	EUNIT_ASSERT(iStreamIn->iCurrentRTPState==
    				MSRTPStreamInContext::ESRTPStreamInUninitialized); 
	UT_CSRTPStreamIn_UnprotectSrtpLL(  );
	EUNIT_ASSERT(iStreamIn->iCurrentRTPState==
    				MSRTPStreamInContext::ESRTPStreamInNormal);
    EUNIT_ASSERT(iStreamIn->iCurrentRTCPState==
    				MSRTPStreamInContext::ESRTPStreamInUninitialized); 				
	
	UT_CSRTPStreamIn_UnprotectSrtcpLL(  );
	EUNIT_ASSERT(iStreamIn->iCurrentRTPState==
    				MSRTPStreamInContext::ESRTPStreamInNormal);
	//UT_CSRTPStreamIn_UnprotectSrtpLL(  );
	//change sequence number
	
	 TInt authTagLenInBytes = iContext->CryptoParams().iSrtpAuthTagLen/8;
        
    TBuf8<32> payload;
    TBuf8<32> authTag;        
    TInt ssrc = 500;
//        TUint8* ssrcPtr = static_cast<TUint8 *>(&ssrc);

    
    for (TInt i=0; i<20; i++)
        {
        payload.Append(0xAA);            
        }        

	payload.SetLength(20);
	//898C05B99F51A602AEF8495A2987EB45A0FB5995
    authTag.Append(0x89);
    authTag.Append(0x8C);
    authTag.Append(0x05);
    authTag.Append(0xB9);
    authTag.Append(0x9F);
    authTag.Append(0x51);
    authTag.Append(0xA6);
    authTag.Append(0x02);
    authTag.Append(0xAE);
    authTag.Append(0xF8);

	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	TInt mkiLength = mki->Length();
	
    const TUint8* payloadPtr = payload.PtrZ();
    const TUint8* mkiPtr =mki->Ptr();// mki->Des().PtrZ();
    const TUint8* authTagPtr = authTag.PtrZ();
    const TUint8* decryptedPayloadPtr = iDecryptedPayload->Des().PtrZ();
    
    TPayloadDetails details;    

    details.seqNr       = KTestSeqNr18;
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
    
      
    HBufC8* srtpPacket = BuildPacketL(ETrue, details);
    CleanupStack::PushL(srtpPacket);
    
    details.payloadData = decryptedPayloadPtr;
    
    HBufC8* refRtpPacket = BuildPacketL(EFalse, details);
    CleanupStack::PushL(refRtpPacket);
 
 	
    HBufC8* rtpPacket = iStreamIn->UnprotectSrtpL(*srtpPacket);
    CleanupStack::PushL(rtpPacket);
    
    EUNIT_ASSERT( iCryptoHandlerSRTP->iSessionEncrKey->Compare(
                                    *iRFC3711_SessionEncrKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTP->iSessionSaltKey->Compare(
                                    *iRFC3711_SessionSaltKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTP->iSessionAuthKey->Compare(
                                    *iRFC3711_SessionAuthKey128bits) == 0);    
        
    
    EUNIT_ASSERT(iStreamIn->iCurrentRTPState==
    				MSRTPStreamInContext::ESRTPStreamInNormal);
    CleanupStack::Pop(3);
    delete rtpPacket;
    delete refRtpPacket;
    delete srtpPacket;
    CleanupStack::PopAndDestroy(mki);
	EUNIT_ASSERT(iStreamIn->iCurrentRTCPState==
    				MSRTPStreamInContext::ESRTPStreamInNormal);  
    }
    
void UT_CSRTPStreamIn::UT_CSRTPStreamIn_StateTests2L()
	{
	EUNIT_ASSERT(iStreamIn->iCurrentRTPState==
    				MSRTPStreamInContext::ESRTPStreamInUninitialized); 
	
	
	UT_CSRTPStreamIn_UnprotectSrtpLL(  );
	
	EUNIT_ASSERT(iStreamIn->iCurrentRTPState==
    				MSRTPStreamInContext::ESRTPStreamInNormal);
    EUNIT_ASSERT(iStreamIn->iCurrentRTCPState==
    				MSRTPStreamInContext::ESRTPStreamInUninitialized); 				
	
	UT_CSRTPStreamIn_UnprotectSrtcpLL(  );
	EUNIT_ASSERT(iStreamIn->iCurrentRTCPState==
    				MSRTPStreamInContext::ESRTPStreamInNormal);	
    				
    HBufC8* encSrtcpPacket =HBufC8::NewLC(KSRTCPPacket2().Length());
	*encSrtcpPacket=KSRTCPPacket2();
    Hex(*encSrtcpPacket);
    TInt len= KSRTCPPacket2().Length();
	encSrtcpPacket->Des().SetLength(len/2);
	//The following will leave because of the authentication tag will not match
    HBufC8* rtcpPacket =iStreamIn->UnprotectSrtcpL(*encSrtcpPacket);
    CleanupStack::PopAndDestroy(encSrtcpPacket);
    CleanupStack::PushL(rtcpPacket);
                     
    CleanupStack::PopAndDestroy(rtcpPacket);                                
    EUNIT_ASSERT(iStreamIn->iCurrentRTCPState==
    				MSRTPStreamInContext::ESRTPStreamInNormal);					
	}
//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE( 
    UT_CSRTPStreamIn,
    "CSRTPStreamIn",
    "UNIT" )

EUNIT_TEST(
    "NewL1 - test ",
    "CSRTPStreamIn",
    "NewL1",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamIn_NewL_OneL, Teardown)

EUNIT_TEST(
    "NewL2 - test ",
    "CSRTPStreamIn",
    "NewL2",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamIn_NewL_TwoL, Teardown)

EUNIT_TEST(
    "NewL3 - test ",
    "CSRTPStreamIn",
    "NewL3",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamIn_NewL_ThreeL, Teardown)
    
EUNIT_TEST(
    "NewL4 - test ",
    "CSRTPStreamIn",
    "NewL4",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamIn_NewL_FourL, Teardown)    

EUNIT_TEST(
    "NewL5 - test ",
    "CSRTPStreamIn",
    "NewL5",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamIn_NewL_FiveL, Teardown)    
EUNIT_TEST(
    "UnprotectSrtpL - test ",
    "CSRTPStreamIn",
    "UnprotectSrtpL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamIn_UnprotectSrtpLL, Teardown)

EUNIT_TEST(
    "UnprotectSrtcpL - test ",
    "CSRTPStreamIn",
    "UnprotectSrtcpL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamIn_UnprotectSrtcpLL, Teardown)

EUNIT_TEST(
    "UnprotectSrtpLateBD - test ",
    "CSRTPStreamIn",
    "UnprotectSrtpL Late Binding",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamIn_UnprotectSrtpL2L, Teardown)

EUNIT_TEST(
    "UnprotectSrtpL3L-RCC mode",
    "CSRTPStreamIn",
    "UnprotectSrtpL RCC",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamIn_UnprotectSrtpL3L, Teardown)    

EUNIT_TEST(
    "UnprotectSrtcpLateBind - test ",
    "CSRTPStreamIn",
    "UnprotectSrtcpL Late Binding",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamIn_UnprotectSrtcpL2L, Teardown)
EUNIT_TEST(
    "State Tests ",
    "CSRTPStreamIn",
    "State Tests for SRTP and SRTCP",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamIn_StateTestsL, Teardown)
EUNIT_TEST(
    "State Tests2 ",
    "CSRTPStreamIn",
    "State Tests for SRTP and SRTCP Noraml State",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamIn_StateTests2L, Teardown)

EUNIT_TEST(
    "SetCryptoIn",
    "CSRTPStreamIn",
    "SetCryptoIn",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamIn_SetCryptoInL, Teardown)    
EUNIT_END_TEST_TABLE

//  END OF FILE
