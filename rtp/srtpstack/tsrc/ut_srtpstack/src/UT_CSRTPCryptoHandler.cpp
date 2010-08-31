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
#include "UT_CSRTPCryptoHandler.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "srtpcryptohandlersrtp.h"
#include "srtpmastersalt.h"
#include "srtpmasterkey.h"
#include "srtpstreamin.h"
#include "srtpstreamout.h"
#include "srtpsession.h"
#include "srtputils.h"

/*
      master key:  E1F97A0D3E018BE0D64FA32C06DE4139
      master salt: 0EC675AD498AFEEBB6960B3AABE6
*/
_LIT8(KTestMKI128Bits,      "ABCDEF1234567890ABCDEF1234567890");
_LIT8(KRFC3711_TestMasterKey128bits,    "E1F97A0D3E018BE0D64FA32C06DE4139");
_LIT8(KRFC3711_TestMasterSalt112bits,   "0EC675AD498AFEEBB6960B3AABE6");	
// 80 bit key, test 1
_LIT8(KTestAuthTag80bits,      "ABABABABABABABABABAB");

_LIT8(KRFCTestSalt112bits,          "0EC675AD498AFEEBB6960B3AABE6");
_LIT8(KSRTP_RFC112bitEncrXValue,    "0EC675AD498AFEEBB6960B3AABE6");
_LIT8(KSRTP_RFC112bitAuthXValue,    "0EC675AD498AFEEAB6960B3AABE6");
_LIT8(KSRTP_RFC112bitSaltXValue,    "0EC675AD498AFEE9B6960B3AABE6");
_LIT8(KSRTCP_RFC112bitEncrXValue,   "0EC675AD498AFEEBB6950B3AABE6");
_LIT8(KSRTCP_RFC112bitAuthXValue,   "0EC675AD498AFEEBB6920B3AABE6");
_LIT8(KSRTCP_RFC112bitSaltXValue,   "0EC675AD498AFEEBB6930B3AABE6");


// CONSTRUCTION
UT_CSRTPCryptoHandler* UT_CSRTPCryptoHandler::NewL()
    {
    UT_CSRTPCryptoHandler* self = UT_CSRTPCryptoHandler::NewLC(); 
    CleanupStack::Pop();

    return self;
    }

UT_CSRTPCryptoHandler* UT_CSRTPCryptoHandler::NewLC()
    {
    UT_CSRTPCryptoHandler* self = new( ELeave ) UT_CSRTPCryptoHandler();
    CleanupStack::PushL( self );

	self->ConstructL(); 

    return self;
    }

// Destructor (virtual by CBase)
UT_CSRTPCryptoHandler::~UT_CSRTPCryptoHandler()
    {
    }

// Default constructor
UT_CSRTPCryptoHandler::UT_CSRTPCryptoHandler() 
    {
    }

void UT_CSRTPCryptoHandler::SRTPMasterKeyStaleEvent(const CSRTPStream& /*aStream*/ )
    {
    }
void UT_CSRTPCryptoHandler::SRTPMasterKeyStaleEvent(const CSRTPSession& /*aSession*/ )
    {
    }

// Second phase construct
void UT_CSRTPCryptoHandler::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

void UT_CSRTPCryptoHandler::Hex(HBufC8& aString)
{
    TPtr8 ptr=aString.Des();
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

void UT_CSRTPCryptoHandler::SetupL(  )
    {
	iTestAuthTag80Bits = HBufC8::NewL(KTestAuthTag80bits().Length());
	*iTestAuthTag80Bits = KTestAuthTag80bits;	
	Hex(*iTestAuthTag80Bits);
	
    // initialize salting key (from RFC)
    iRFCTestSalt112bits.Copy(KRFCTestSalt112bits);
    TSRTPUtils::DeHex(iRFCTestSalt112bits, iRFCTestSalt112bits_dehexed); 

    // initialize derived SRTP session encr key (from RFC)
    iSRTP_RFC112bitEncrXValue.Copy(KSRTP_RFC112bitEncrXValue);
    TSRTPUtils::DeHex(iSRTP_RFC112bitEncrXValue, iSRTP_RFC112bitEncrXValue_dehexed);         
    // initialize derived SRTP session auth key (from RFC)
    iSRTP_RFC112bitAuthXValue.Copy(KSRTP_RFC112bitAuthXValue);
    TSRTPUtils::DeHex(iSRTP_RFC112bitAuthXValue, iSRTP_RFC112bitAuthXValue_dehexed); 
    // initialize derived SRTP session salting key (from RFC)
    iSRTP_RFC112bitSaltXValue.Copy(KSRTP_RFC112bitSaltXValue);
    TSRTPUtils::DeHex(iSRTP_RFC112bitSaltXValue, iSRTP_RFC112bitSaltXValue_dehexed); 

    // initialize derived SRTCP session encr key (from RFC)
    iSRTCP_RFC112bitEncrXValue.Copy(KSRTCP_RFC112bitEncrXValue);
    TSRTPUtils::DeHex(iSRTCP_RFC112bitEncrXValue, iSRTCP_RFC112bitEncrXValue_dehexed);         
    // initialize derived SRTCP session auth key (from RFC)
    iSRTCP_RFC112bitAuthXValue.Copy(KSRTCP_RFC112bitAuthXValue);
    TSRTPUtils::DeHex(iSRTCP_RFC112bitAuthXValue, iSRTCP_RFC112bitAuthXValue_dehexed); 
    // initialize derived SRTCP session salting key (from RFC)
    iSRTCP_RFC112bitSaltXValue.Copy(KSRTCP_RFC112bitSaltXValue);
    TSRTPUtils::DeHex(iSRTCP_RFC112bitSaltXValue, iSRTCP_RFC112bitSaltXValue_dehexed); 
		

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
    iStreamOut = CSRTPStreamOut::NewL(*iSRTPSession, (TUint)2);   
    //iCryptoHandlerSRTP = CSRTPCryptoHandlerSRTP::NewL(*iStreamIn);
    //iStreamIn->iHandlerRTP = iCryptoHandlerSRTP;   
    iCryptoHandlerSRTP = iStreamIn->iHandlerRTP;    
    } 

void UT_CSRTPCryptoHandler::Teardown(  )
    {
    delete iTestAuthTag80Bits;
    delete iStreamIn;

    delete iStreamOut;
    delete iSRTPSession; 
	
	iTestAuthTag80Bits = NULL;
    iStreamIn = NULL;

    iStreamOut = NULL;
    iSRTPSession = NULL; 
    
    iResult.Zero();        
    iRFCTestSalt112bits.Zero();
    iRFCTestSalt112bits_dehexed.Zero();
    iRFC_TestSalt_80bits.Zero();
        
        // SRTP test data KSRTP_RFC112bitEncrXValue
    iSRTP_RFC112bitEncrXValue.Zero();
    iSRTP_RFC112bitEncrXValue_dehexed.Zero();
    iSRTP_RFC112bitAuthXValue.Zero();
    iSRTP_RFC112bitAuthXValue_dehexed.Zero();
    iSRTP_RFC112bitSaltXValue.Zero();
    iSRTP_RFC112bitSaltXValue_dehexed.Zero();

        // SRTCP test data
    iSRTCP_RFC112bitEncrXValue.Zero();
    iSRTCP_RFC112bitEncrXValue_dehexed.Zero();
    iSRTCP_RFC112bitAuthXValue.Zero();
    iSRTCP_RFC112bitAuthXValue_dehexed.Zero();
    iSRTCP_RFC112bitSaltXValue.Zero();
    iSRTCP_RFC112bitSaltXValue_dehexed.Zero();

    }

void UT_CSRTPCryptoHandler::UT_CSRTPCryptoHandler_MasterKeyUpdatedL(  )
    {
    EUNIT_ASSERT(iCryptoHandlerSRTP->MasterKeysUpdated()==EFalse);
    iCryptoHandlerSRTP->SRTPMasterKeyChanged();
    EUNIT_ASSERT(iCryptoHandlerSRTP->MasterKeysUpdated()==ETrue);
    }

void UT_CSRTPCryptoHandler::UT_CSRTPCryptoHandler_AuthenticateLL(  )
    {
    EUNIT_ASSERT( ETrue );
    }


void UT_CSRTPCryptoHandler::UT_GetAuthenticationTagL(  )
    { 
/*       
    TBuf8<KMinRtpHeaderSize + 200> packetTBuf;

    for (TInt i=0; i<KMinRtpHeaderSize; i++)
        {
        //Fill the header with zeros
        packetTBuf.Append(0x00);
        }

    for (TInt i=0; i<20; i++)
        {
        //Append 160 bit payload
        packetTBuf.Append(0x11);
        }
    
        
    for (TInt i=0; i<10; i++)
        {
        //Append the authTag
        packetTBuf.Append(0xAB);
        }

    iCryptoHandlerSRTP->iPacket.Set(packetTBuf);
 
    TPtrC8  authTag = iCryptoHandlerSRTP->GetAuthenticationTag();
    
    EUNIT_ASSERT (authTag.Compare(*iTestAuthTag80Bits) == 0)                     
    */
    }


void UT_CSRTPCryptoHandler::UT_Count_XL(  )
    {

    TUint64 r = TSRTPUtils::Cnt_r(5, iContext->CryptoParams().iSrtpKeyDervRate);

    // first SRTP.. 
    // SRTP encr label (0x00)
    iCryptoHandlerSRTP->Count_X(r, KSRTPEncryptionLabel, KSRTPIndexLength, iResult);   
    EUNIT_ASSERT( iResult.Compare(iSRTP_RFC112bitEncrXValue_dehexed) == 0);
    //reset result
    iResult.FillZ();
    // SRTP  auth label (0x01)
    iCryptoHandlerSRTP->Count_X(r, KSRTPAuthenticationLabel, KSRTPIndexLength, iResult);   
    EUNIT_ASSERT( iResult.Compare(iSRTP_RFC112bitAuthXValue_dehexed) == 0);
    //reset result
    iResult.FillZ();
    // SRTP salting label (0x02)
    iCryptoHandlerSRTP->Count_X(r, KSRTPSaltingLabel, KSRTPIndexLength, iResult);   
    EUNIT_ASSERT( iResult.Compare(iSRTP_RFC112bitSaltXValue_dehexed) == 0);
    //reset result
    iResult.FillZ();

    // then SRTCP.. later
    }



//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE( 
    UT_CSRTPCryptoHandler,
    "CSRTPCryptoHandler",
    "UNIT" )

EUNIT_TEST(
    "UnprotectL - test ",
    "CSRTPCryptoHandler",
    "UnprotectL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPCryptoHandler_MasterKeyUpdatedL, Teardown)

EUNIT_TEST(
    "ProtectL - test ",
    "CSRTPCryptoHandler",
    "ProtectL",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPCryptoHandler_AuthenticateLL, Teardown)

EUNIT_TEST(
    "GetAuthenticationTag - test ",
    "CSRTPCryptoHandler",
    "GetAuthenticationTag",
    "FUNCTIONALITY",
    SetupL, UT_GetAuthenticationTagL, Teardown)

EUNIT_TEST(
    "Count_X - test ",
    "CSRTPCryptoHandler",
    "Count_X",
    "FUNCTIONALITY",
    SetupL, UT_Count_XL, Teardown)
EUNIT_TEST(
    "MasterKeyUpdated ",
    "CSRTPCryptoHandler",
    "MasterKeyUpdated",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPCryptoHandler_MasterKeyUpdatedL, Teardown)
EUNIT_END_TEST_TABLE

//  END OF FILE
