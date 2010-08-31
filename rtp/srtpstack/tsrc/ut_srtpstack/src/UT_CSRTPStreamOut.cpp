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
#include "UT_CSRTPStreamOut.h"

//  INTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>

//  EXTERNAL INCLUDES
#include "srtpcryptohandlersrtp.h"
#include "srtpcryptohandlersrtcp.h"
#include "srtpmastersalt.h"
#include "srtpmasterkey.h"
#include "srtpstreamout.h"
#include "srtpsession.h"
#include "srtputils.h"


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

_LIT8(KRFC3711_RtcpEncrKey128bits,   	"4C1AA45A81F73D61C800BBB00FBB1EAA");  
_LIT8(KRFC3711_RtcpSaltKey128bits,   	"9581C7AD87B3E530BF3E4454A8B3");  
_LIT8(KRFC3711_RtcpAuthKey128bits,   	"8D54534FEB49AE8E7993A6BD0B844FC323A93DFD");  


_LIT8(KTestM_without_ROC,               "123456789ABCDE");  
_LIT8(KRefM_with_ROC,                   "123456789ABCDEFFFFFFFF");  

_LIT8(KTestAuthTag,                     "D07919ABC9F235CB4516");  
_LIT8(KDummyRTCPPacket, "dummyrtcp");
_LIT8(KRTCPPacket, "80C80006DEADBEEFC7B1ACD423FE000000E12F18000000010000020081CA0015DEADBEEF010A52747055492075736572020455736572030D7573657240686F6D652E6E6574040B2B313233343536373839300504486F6D65060B527470206578616D706C65070641637469766508010000000000");

// CONSTRUCTION
UT_CSRTPStreamOut* UT_CSRTPStreamOut::NewL()
    {
    UT_CSRTPStreamOut* self = UT_CSRTPStreamOut::NewLC(); 
    CleanupStack::Pop();

    return self;
    }

UT_CSRTPStreamOut* UT_CSRTPStreamOut::NewLC()
    {
    UT_CSRTPStreamOut* self = new( ELeave ) UT_CSRTPStreamOut();
    CleanupStack::PushL( self );

	self->ConstructL(); 

    return self;
    }

// Destructor (virtual by CBase)
UT_CSRTPStreamOut::~UT_CSRTPStreamOut()
    {
    }

// Default constructor
UT_CSRTPStreamOut::UT_CSRTPStreamOut() 
    {
    }

// Second phase construct
void UT_CSRTPStreamOut::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

UT_CSRTPStreamOut::TPayloadDetails::TPayloadDetails() :
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
        
void UT_CSRTPStreamOut::SRTPMasterKeyStaleEvent(const CSRTPStream& /*aStream*/ )
    {
    }
void UT_CSRTPStreamOut::SRTPMasterKeyStaleEvent(const CSRTPSession& /*aSession*/ )
    {
    }

void UT_CSRTPStreamOut::Hex(HBufC8& aString)
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
// UT_CSRTPStreamOut::BuildSrtpPacketL()
// 
// ---------------------------------------------------------------------------
//
HBufC8* UT_CSRTPStreamOut::BuildPacketL(TBool aSrtp, TPayloadDetails& details)
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



void UT_CSRTPStreamOut::SetupL(  )
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
    
	/*iTestMKI128Bits = HBufC8::NewL(KTestMKI128Bits().Length());
	*iTestMKI128Bits = KTestMKI128Bits;	
	Hex(*iTestMKI128Bits);	
*/


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
    iStreamOut = CSRTPStreamOut::NewL(*iSRTPSession, (TUint)1);
    iCryptoHandlerSRTP = iStreamOut->iHandlerRTP;
    iCryptoHandlerSRTCP =iStreamOut->iHandlerRTCP;
    } 

void UT_CSRTPStreamOut::Teardown(  )
    {
    delete iDecryptedPayload;
    delete iTestPayload160Bits;
	
	iDecryptedPayload = NULL;
    iTestPayload160Bits = NULL;
    //delete iTestMKI128Bits;

    delete iStreamOut;
    delete iSRTPSession;    
	
	iStreamOut = NULL;
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

void UT_CSRTPStreamOut::UT_CSRTPStreamOut_NewL_OneL(  )
    {
    CSRTPStreamOut* tempStreamOut = CSRTPStreamOut::NewL(*iSRTPSession, (TUint)1);
    EUNIT_ASSERT(tempStreamOut->SSRC()== 1);
    delete tempStreamOut;
    }

void UT_CSRTPStreamOut::UT_CSRTPStreamOut_NewL_TwoL(  )
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
    CSRTPSession* srtpSession = CSRTPSession::NewL( iDestination );       
    CleanupStack::PushL( srtpSession );                          
    CSRTPStreamOut* tempStreamOut =CSRTPStreamOut::NewL( *srtpSession,
                                      (TUint)1,
                                      context,
                                      *this );
    CleanupStack::Pop( srtpSession );
    CleanupStack::Pop( context );
    
    EUNIT_ASSERT(tempStreamOut->SSRC()== 1);
    
    EUNIT_ASSERT(tempStreamOut->GetCryptoContext().Valid()==ETrue); 
    delete tempStreamOut;    
    delete srtpSession;                     
    }

void UT_CSRTPStreamOut::UT_CSRTPStreamOut_NewL_ThreeL(  )
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
    CSRTPSession* srtpSession = CSRTPSession::NewL( iDestination );       
    CleanupStack::PushL( srtpSession );                           
    CSRTPStreamOut* tempStreamOut =CSRTPStreamOut::NewL( *srtpSession,
                                      (TUint)1,
                                      *this );
    CleanupStack::PushL( tempStreamOut ); 
    
    EUNIT_ASSERT(tempStreamOut->SSRC()== 1);
    tempStreamOut->SetCryptoOutL(context);
    CleanupStack::Pop( tempStreamOut );
    CleanupStack::Pop( srtpSession );
    CleanupStack::Pop( context );
    EUNIT_ASSERT(tempStreamOut->GetCryptoContext().Valid()==ETrue); 
    delete tempStreamOut;    
    delete srtpSession;                     
    }
    
void UT_CSRTPStreamOut::UT_CSRTPStreamOut_SetCryptoOutLL(  )
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
    CSRTPSession* srtpSession = CSRTPSession::NewL( iDestination );       
    CleanupStack::PushL( srtpSession );
                              
    CSRTPStreamOut* tempStreamOut =CSRTPStreamOut::NewL( *srtpSession,
                                      (TUint)1,
                                      *this );
    CleanupStack::PushL( tempStreamOut );
    
    EUNIT_ASSERT(tempStreamOut->SSRC()== 1);
    CSRTPCryptoContext* context2=NULL;
    EUNIT_ASSERT_SPECIFIC_LEAVE(tempStreamOut->SetCryptoOutL(context2), KErrArgument);
    tempStreamOut->SetCryptoOutL(context);
    CleanupStack::Pop( tempStreamOut );
    CleanupStack::Pop( srtpSession );
    CleanupStack::Pop( context );
    CleanupStack::PushL( srtpSession );
    CleanupStack::PushL( tempStreamOut );
    tempStreamOut->iRekey=ETrue;
    tempStreamOut->SetCryptoOutL(context);
    EUNIT_ASSERT(tempStreamOut->GetCryptoContext().Valid()==ETrue); 
    CleanupStack::PopAndDestroy( tempStreamOut );
    CleanupStack::PopAndDestroy( srtpSession );                     
    }
        
void UT_CSRTPStreamOut::UT_CSRTPStreamOut_ProtectRtpLL(  )
    {
        //TInt mkiLength = (KTestMKI128Bits().Length())/2;
        TInt authTagLenInBytes = iContext->CryptoParams().iSrtpAuthTagLen/8;
 
        TBuf8<32> encryptedPayload;
        TBuf8<32> authTag;        
        TInt ssrc = 500;
//        TUint8* ssrcPtr = static_cast<TUint8 *>(&ssrc);
        

        for (TInt i=0; i<20; i++)
            {
            encryptedPayload.Append(0xAA);            
            }        
		encryptedPayload.SetLength(20);
		
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
        const TUint8* encryptedPayloadPtr = encryptedPayload.PtrZ();
        const TUint8* mkiPtr = mki->Ptr();
        const TUint8* authTagPtr = authTag.PtrZ();
        const TUint8* decryptedPayloadPtr = iDecryptedPayload->Ptr();
        
        TPayloadDetails details;    

        details.seqNr       = KTestSeqNr17;
//        details.ssrc        = ssrcPtr;
        details.padding     = 0;
        details.numCSRC     = 0;
        details.fHeaderExtension = EFalse;
        details.payloadLen  = 20;
        details.payloadData = decryptedPayloadPtr;
        details.MKILen      = mkiLength;
        details.MKIData     = mkiPtr;
        details.authTagLen  = authTagLenInBytes;
        details.authTagData = authTagPtr;            
      
    HBufC8* rtpPacket = BuildPacketL(EFalse, details);
    CleanupStack::PushL(rtpPacket);
    
    details.payloadData = encryptedPayloadPtr;
    
    HBufC8* refSrtpPacket = BuildPacketL(ETrue, details);
    CleanupStack::PushL(refSrtpPacket);
    
    // test protecting    
    HBufC8* srtpPacket = iStreamOut->ProtectRtpL(*rtpPacket);
//    HBufC8* srtpPacket = iCryptoHandlerSRTP->DoProtectL(*rtpPacket);
    CleanupStack::PushL(srtpPacket);
    
    TBuf8<65> copyRefSrtpPacket;
    copyRefSrtpPacket.SetLength(65);
    copyRefSrtpPacket.Copy(*refSrtpPacket);

    TBuf8<60> copysrtpPacket;
    copysrtpPacket.SetLength(59);
    copysrtpPacket.Copy(*srtpPacket);
    
    iCryptoHandlerSRTP = iStreamOut->iHandlerRTP;
    iCryptoHandlerSRTCP =iStreamOut->iHandlerRTCP;
                           
    EUNIT_ASSERT( iCryptoHandlerSRTP->iSessionEncrKey->Compare(
                                    *iRFC3711_SessionEncrKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTP->iSessionSaltKey->Compare(
                                    *iRFC3711_SessionSaltKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTP->iSessionAuthKey->Compare(
                                    *iRFC3711_SessionAuthKey128bits) == 0);    
                                            
    EUNIT_ASSERT( refSrtpPacket->Compare(*srtpPacket) == 0);        
	EUNIT_ASSERT(iStreamOut->iCurrentRTPState==
    				MSRTPStreamOutContext::ESRTPStreamOutNormal);
    CleanupStack::PopAndDestroy(srtpPacket);
    CleanupStack::PopAndDestroy(refSrtpPacket);
    CleanupStack::PopAndDestroy(rtpPacket); 
    CleanupStack::PopAndDestroy(mki);   
    }

void UT_CSRTPStreamOut::UT_CSRTPStreamOut_ProtectRtpL2L(  )
    {
    //update cryptohandler the param  
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
    iStreamOut->SetCryptoOutL(context);
    CleanupStack::Pop( context );
    iStreamOut->SetROC(0);
    TInt authTagLenInBytes = iContext->CryptoParams().iSrtpAuthTagLen/8;

    TBuf8<32> encryptedPayload;
    TBuf8<32> authTag; 
    TInt ssrc = 500;
    //TUint8* ssrcPtr = (ssrc);
    

    for (TInt i=0; i<20; i++)
        {
        encryptedPayload.Append(0xAA);            
        }        
	encryptedPayload.SetLength(20);


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
    const TUint8* encryptedPayloadPtr = encryptedPayload.PtrZ();
    const TUint8* mkiPtr = mki->Ptr();
    const TUint8* authTagPtr = authTag.PtrZ();
    const TUint8* decryptedPayloadPtr = iDecryptedPayload->Ptr();
    
    TPayloadDetails details;    

    details.seqNr       = KTestSeqNr17;
	details.ssrc        = 1;
    details.padding     = 0;
    details.numCSRC     = 0;
    details.fHeaderExtension = EFalse;
    details.payloadLen  = 20;
    details.payloadData = decryptedPayloadPtr;
    details.MKILen      = mkiLength;
    details.MKIData     = mkiPtr;
    details.authTagLen  = authTagLenInBytes+4;
    details.authTagData = authTagPtr;            
  
    HBufC8* rtpPacket = BuildPacketL(EFalse, details);
    CleanupStack::PushL(rtpPacket);
    
    details.payloadData = encryptedPayloadPtr;
    details.authTagLen  = authTagLenInBytes+4;
    
    
    HBufC8* refSrtpPacket = BuildPacketL(ETrue, details);
    CleanupStack::PushL(refSrtpPacket);
    
    // test protecting    
    HBufC8* srtpPacket = iStreamOut->ProtectRtpL(*rtpPacket);

    CleanupStack::PushL(srtpPacket);
    
    iCryptoHandlerSRTP = iStreamOut->iHandlerRTP;
    iCryptoHandlerSRTCP =iStreamOut->iHandlerRTCP;
                           
    EUNIT_ASSERT( iCryptoHandlerSRTP->iSessionEncrKey->Compare(
                                    *iRFC3711_SessionEncrKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTP->iSessionSaltKey->Compare(
                                    *iRFC3711_SessionSaltKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTP->iSessionAuthKey->Compare(
                                    *iRFC3711_SessionAuthKey128bits) == 0);    
                                            
    EUNIT_ASSERT( refSrtpPacket->Compare(*srtpPacket) == 0);        
	EUNIT_ASSERT(iStreamOut->iCurrentRTPState==
    				MSRTPStreamOutContext::ESRTPStreamOutNormal);
    CleanupStack::PopAndDestroy(srtpPacket);
    CleanupStack::PopAndDestroy(refSrtpPacket);
    CleanupStack::PopAndDestroy(rtpPacket); 
    CleanupStack::PopAndDestroy(mki);   
    }



void UT_CSRTPStreamOut::UT_CSRTPStreamOut_ProtectRtcpLL(  )
    {
    
    HBufC8* srtcpPacket =HBufC8::NewLC(KRTCPPacket().Length());
	*srtcpPacket=KRTCPPacket;
    Hex(*srtcpPacket);
    TInt len= KRTCPPacket().Length();
	srtcpPacket->Des().SetLength(len/2);
	
	
    HBufC8* decrypedRtcpPacket =iStreamOut->ProtectRtcpL(*srtcpPacket);
    CleanupStack::PopAndDestroy(srtcpPacket);
    CleanupStack::PushL(decrypedRtcpPacket);
    iCryptoHandlerSRTP = iStreamOut->iHandlerRTP;
    iCryptoHandlerSRTCP =iStreamOut->iHandlerRTCP;
    
    EUNIT_ASSERT( iCryptoHandlerSRTCP->iSessionEncrKey->Compare(
                                    *iRFC3711_RtcpEncrKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTCP->iSessionSaltKey->Compare(
                                    *iRFC3711_RtcpSaltKey128bits) == 0);
    EUNIT_ASSERT( iCryptoHandlerSRTCP->iSessionAuthKey->Compare(
									*iRFC3711_RtcpAuthKey128bits) == 0);    
	CleanupStack::PopAndDestroy(decrypedRtcpPacket);  
    EUNIT_ASSERT(iStreamOut->iCurrentRTCPState==
    				MSRTPStreamOutContext::ESRTPStreamOutNormal);      
    }
void UT_CSRTPStreamOut::UT_CSRTPStreamOut_StateTestsL()
	{
	EUNIT_ASSERT(iStreamOut->iCurrentRTPState==
    				MSRTPStreamOutContext::ESRTPStreamOutUninitialized); 
	UT_CSRTPStreamOut_ProtectRtpLL(  );
	EUNIT_ASSERT(iStreamOut->iCurrentRTPState==
    				MSRTPStreamOutContext::ESRTPStreamOutNormal);
    EUNIT_ASSERT(iStreamOut->iCurrentRTCPState==
    				MSRTPStreamOutContext::ESRTPStreamOutUninitialized); 				
	UT_CSRTPStreamOut_ProtectRtcpLL(  );
	EUNIT_ASSERT(iStreamOut->iCurrentRTPState==
    				MSRTPStreamOutContext::ESRTPStreamOutNormal);
	UT_CSRTPStreamOut_ProtectRtpLL(  );
	EUNIT_ASSERT(iStreamOut->iCurrentRTCPState==
    				MSRTPStreamOutContext::ESRTPStreamOutNormal);  
    UT_CSRTPStreamOut_ProtectRtcpLL(  );			 
	}
//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE( 
    UT_CSRTPStreamOut,
    "CSRTPStreamOut",
    "UNIT" )

EUNIT_TEST(
    "NewL1 - test ",
    "CSRTPStreamOut",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamOut_NewL_OneL, Teardown)

EUNIT_TEST(
    "NewL2 - test ",
    "CSRTPStreamOut",
    "NewL2",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamOut_NewL_TwoL, Teardown)
EUNIT_TEST(
    "NewL3 - test ",
    "CSRTPStreamOut",
    "NewL3",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamOut_NewL_ThreeL, Teardown)

EUNIT_TEST(
    "ProtectRtpL - test ",
    "CSRTPStreamOut",
    "ProtectRtpL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamOut_ProtectRtpLL, Teardown)
    
EUNIT_TEST(
    "ProtectRtp2L - test ",
    "CSRTPStreamOut",
    "ProtectRtp2L",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamOut_ProtectRtpL2L, Teardown)    

EUNIT_TEST(
    "ProtectRtcpL - test ",
    "CSRTPStreamOut",
    "ProtectRtcpL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamOut_ProtectRtcpLL, Teardown)

EUNIT_TEST(
    "Stream Out State Tests ",
    "CSRTPStreamOut",
    "State Tests for SRTP and SRTCP",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamOut_StateTestsL, Teardown)
EUNIT_TEST(
    "SetCryptoOutL",
    "CSRTPStreamOut",
    "SetCryptoOutL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPStreamOut_SetCryptoOutLL, Teardown)    
    
EUNIT_END_TEST_TABLE

//  END OF FILE
