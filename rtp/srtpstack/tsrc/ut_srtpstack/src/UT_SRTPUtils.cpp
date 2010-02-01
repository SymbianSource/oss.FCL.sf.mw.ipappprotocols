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
#include "UT_SRTPUtils.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "srtputils.h"
#include "srtpdef.h"

_LIT8(KRFCTestSalt112bits,              "0EC675AD498AFEEBB6960B3AABE6");
_LIT8(KSRTP_RFC112bitSessionEncrKey,    "0EC675AD498AFEEBB6960B3AABE6");
_LIT8(KSRTP_RFC112bitSessionAuthKey,    "0EC675AD498AFEEAB6960B3AABE6");
_LIT8(KSRTP_RFC112bitSessionSaltKey,    "0EC675AD498AFEE9B6960B3AABE6");
_LIT8(KSRTCP_RFC112bitSessionEncrKey,   "0EC675AD498AFEEBB6950B3AABE6");
_LIT8(KSRTCP_RFC112bitSessionAuthKey,   "0EC675AD498AFEEBB6920B3AABE6");
_LIT8(KSRTCP_RFC112bitSessionSaltKey,   "0EC675AD498AFEEBB6930B3AABE6");
_LIT8(KRFC3711_PaddedSaltX128bits,      "0EC675AD498AFEE9B6960B3AABE60000");
_LIT8(KTest128BitIV,                    "0EC675ADB67501164969F4C554190000");



// CONSTRUCTION
UT_SRTPUtils* UT_SRTPUtils::NewL()
    {
    UT_SRTPUtils* self = UT_SRTPUtils::NewLC(); 
    CleanupStack::Pop();

    return self;
    }

UT_SRTPUtils* UT_SRTPUtils::NewLC()
    {
    UT_SRTPUtils* self = new( ELeave ) UT_SRTPUtils();
    CleanupStack::PushL( self );

	self->ConstructL(); 

    return self;
    }

// Destructor (virtual by CBase)
UT_SRTPUtils::~UT_SRTPUtils()
    {
    }

// Default constructor
UT_SRTPUtils::UT_SRTPUtils()
    {
    }

// Second phase construct
void UT_SRTPUtils::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_SRTPUtils::SetupL(  )
    {    
    // initialize 128 bit padded salt X value
    iRFC3711_PaddedSaltX128bits.Copy(KRFC3711_PaddedSaltX128bits);
    TSRTPUtils::DeHex(iRFC3711_PaddedSaltX128bits, iRFC3711_PaddedSaltX128bits_dehexed); 
       
    // test 128 bit IV (based on RFC session salt value)
    iTest128BitIV.Copy(KTest128BitIV);
    TSRTPUtils::DeHex(iTest128BitIV, iTest128BitIV_dehexed);     
       
    // initialize salting key (from RFC)
    iRFCTestSalt112bits.Copy(KRFCTestSalt112bits);
    TSRTPUtils::DeHex(iRFCTestSalt112bits, iRFCTestSalt112bits_dehexed); 

    // initialize derived SRTP session encr key (from RFC)
    iSRTP_RFC112bitSessionEncrKey.Copy(KSRTP_RFC112bitSessionEncrKey);
    TSRTPUtils::DeHex(iSRTP_RFC112bitSessionEncrKey, iSRTP_RFC112bitSessionEncrKey_dehexed);         
    // initialize derived SRTP session auth key (from RFC)
    iSRTP_RFC112bitSessionAuthKey.Copy(KSRTP_RFC112bitSessionAuthKey);
    TSRTPUtils::DeHex(iSRTP_RFC112bitSessionAuthKey, iSRTP_RFC112bitSessionAuthKey_dehexed); 
    // initialize derived SRTP session salting key (from RFC)
    iSRTP_RFC112bitSessionSaltKey.Copy(KSRTP_RFC112bitSessionSaltKey);
    TSRTPUtils::DeHex(iSRTP_RFC112bitSessionSaltKey, iSRTP_RFC112bitSessionSaltKey_dehexed); 

    // initialize derived SRTCP session encr key (from RFC)
    iSRTCP_RFC112bitSessionEncrKey.Copy(KSRTCP_RFC112bitSessionEncrKey);
    TSRTPUtils::DeHex(iSRTCP_RFC112bitSessionEncrKey, iSRTCP_RFC112bitSessionEncrKey_dehexed);         
    // initialize derived SRTCP session auth key (from RFC)
    iSRTCP_RFC112bitSessionAuthKey.Copy(KSRTCP_RFC112bitSessionAuthKey);
    TSRTPUtils::DeHex(iSRTCP_RFC112bitSessionAuthKey, iSRTCP_RFC112bitSessionAuthKey_dehexed); 
    // initialize derived SRTCP session salting key (from RFC)
    iSRTCP_RFC112bitSessionSaltKey.Copy(KSRTCP_RFC112bitSessionSaltKey);
    TSRTPUtils::DeHex(iSRTCP_RFC112bitSessionSaltKey, iSRTCP_RFC112bitSessionSaltKey_dehexed); 

    } 

void UT_SRTPUtils::Teardown(  )
    {
    iResult.Zero();        
    iRFCTestSalt112bits.Zero();
    iRFCTestSalt112bits_dehexed.Zero();
    iRFC_TestSalt_80bits.Zero();        
        
    iRFC3711_PaddedSaltX128bits.Zero();
    iRFC3711_PaddedSaltX128bits_dehexed.Zero();
    iTest128BitIV.Zero();
    iTest128BitIV_dehexed.Zero();
                
        // SRTP test data
    iSRTP_RFC112bitSessionEncrKey.Zero();
    iSRTP_RFC112bitSessionEncrKey_dehexed.Zero();
    iSRTP_RFC112bitSessionAuthKey.Zero();
    iSRTP_RFC112bitSessionAuthKey_dehexed.Zero();
    iSRTP_RFC112bitSessionSaltKey.Zero();
    iSRTP_RFC112bitSessionSaltKey_dehexed.Zero();
    
    
     iSRTCP_RFC112bitSessionEncrKey.Zero();
     iSRTCP_RFC112bitSessionEncrKey_dehexed.Zero();
     iSRTCP_RFC112bitSessionAuthKey.Zero();
     iSRTCP_RFC112bitSessionAuthKey_dehexed.Zero();
     iSRTCP_RFC112bitSessionSaltKey.Zero();
     iSRTCP_RFC112bitSessionSaltKey_dehexed.Zero();
    }


void UT_SRTPUtils::UT_GetIVFirstTermL(  )
    {

    TBuf8<32> result;    
    TSRTPUtils::GetIVFirstTerm(result, iSRTP_RFC112bitSessionSaltKey_dehexed);
    
    EUNIT_ASSERT( result.Compare(iRFC3711_PaddedSaltX128bits_dehexed) == 0);
    }


void UT_SRTPUtils::UT_GetIVSecondTermL(  )
    {
    TBuf8<32> result;    
    TBuf8<16> paddedSSRC;

    paddedSSRC.Append(0x00);
    paddedSSRC.Append(0x00);
    paddedSSRC.Append(0x00);
    paddedSSRC.Append(0x00);
    paddedSSRC.Append(0xFF);
    paddedSSRC.Append(0xFF);
    paddedSSRC.Append(0xFF);
    paddedSSRC.Append(0xFF);
    paddedSSRC.Append(0x00);
    paddedSSRC.Append(0x00);
    paddedSSRC.Append(0x00);
    paddedSSRC.Append(0x00);
    paddedSSRC.Append(0x00);
    paddedSSRC.Append(0x00);
    paddedSSRC.Append(0x00);
    paddedSSRC.Append(0x00);

    TSRTPUtils::GetIVSecondTerm(result, 0xFFFFFFFF);
        
    EUNIT_ASSERT( result.Compare(paddedSSRC) == 0);
    }

void UT_SRTPUtils::UT_GetIVThirdTermL(  )
    {
    TBuf8<32> result;    
    TBuf8<16> paddedPacketIndex;

    paddedPacketIndex.Append(0x00);
    paddedPacketIndex.Append(0x00);
    paddedPacketIndex.Append(0x00);
    paddedPacketIndex.Append(0x00);
    paddedPacketIndex.Append(0x00);
    paddedPacketIndex.Append(0x00);
    paddedPacketIndex.Append(0x00);
    paddedPacketIndex.Append(0x00);
    paddedPacketIndex.Append(0xFF);
    paddedPacketIndex.Append(0xFF);
    paddedPacketIndex.Append(0xFF);
    paddedPacketIndex.Append(0xFF);
    paddedPacketIndex.Append(0xFF);
    paddedPacketIndex.Append(0xFF);
    paddedPacketIndex.Append(0x00);
    paddedPacketIndex.Append(0x00);

    TSRTPUtils::GetIVThirdTerm(result, 0xFFFFFFFFFFFF);
        
    EUNIT_ASSERT( result.Compare(paddedPacketIndex) == 0);
    }


void UT_SRTPUtils::UT_CountIVL(  )
    {
    TBuf8<32> result;    
    
           
    TInt ret = TSRTPUtils::CountIV(result, iSRTP_RFC112bitSessionSaltKey_dehexed,
                       0xFFFFFFFF,
                       0xFFFFFFFFFFFF);

    EUNIT_ASSERT( ret == KErrArgument);
                       
    result.SetLength(16);   

    ret = TSRTPUtils::CountIV(result, iSRTP_RFC112bitSessionSaltKey_dehexed,
                       0xFFFFFFFF,
                       0xFFFFFFFFFFFF);

    EUNIT_ASSERT( ret == KErrNone);

    EUNIT_ASSERT( result.Compare(iTest128BitIV_dehexed) == 0);
    
    
    }


void UT_SRTPUtils::UT_SRTPUtils_Cnt_rL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_SRTPUtils::UT_SRTPUtils_Cnt_key_idL(  )
    {
    EUNIT_ASSERT( TSRTPUtils::Cnt_key_id(KSRTPEncryptionLabel,0,     KSRTPIndexLength) == 0);
    EUNIT_ASSERT( TSRTPUtils::Cnt_key_id(KSRTPAuthenticationLabel,0, KSRTPIndexLength) == 0x01000000000000);
    EUNIT_ASSERT( TSRTPUtils::Cnt_key_id(KSRTCPEncryptionLabel,0, KSRTCPIndexLength) == 0x0300000000);
    EUNIT_ASSERT( TSRTPUtils::Cnt_key_id(KSRTCPAuthenticationLabel,0, KSRTCPIndexLength) == 0x0400000000);
    
    }

void UT_SRTPUtils::UT_SRTPUtils_Cnt_xL(  )
    {

    // first SRTP.. 
    // RFC test salt (112 bit) and SRTP encr label (0x00)
    TSRTPUtils::Cnt_x(KSRTP_56bit_encr_key_id, iResult, iRFCTestSalt112bits_dehexed);   
    EUNIT_ASSERT( iResult.Compare(iSRTP_RFC112bitSessionEncrKey_dehexed) == 0);
    //reset result
    iResult.FillZ();
    // RFC test salt (112 bit) and SRTP  auth label (0x01)
    TSRTPUtils::Cnt_x(KSRTP_56bit_auth_key_id, iResult, iRFCTestSalt112bits_dehexed);   
    EUNIT_ASSERT( iResult.Compare(iSRTP_RFC112bitSessionAuthKey_dehexed) == 0);
    //reset result
    iResult.FillZ();
    // RFC test salt (112 bit) and SRTP salting label (0x02)
    TSRTPUtils::Cnt_x(KSRTP_56bit_salt_key_id, iResult, iRFCTestSalt112bits_dehexed);   
    EUNIT_ASSERT( iResult.Compare(iSRTP_RFC112bitSessionSaltKey_dehexed) == 0);
    //reset result
    iResult.FillZ();

    // then SRTCP..
    // RFC test salt (112 bit) and SRTCP encr label (0x03)
    TSRTPUtils::Cnt_x(KSRTCP_39bit_encr_key_id, iResult, iRFCTestSalt112bits_dehexed);   
    EUNIT_ASSERT( iResult.Compare(iSRTCP_RFC112bitSessionEncrKey_dehexed) == 0);
    //reset result
    iResult.FillZ();
    // RFC test salt (112 bit) and SRTCP auth label (0x04)
    TSRTPUtils::Cnt_x(KSRTCP_39bit_auth_key_id, iResult, iRFCTestSalt112bits_dehexed);   
    EUNIT_ASSERT( iResult.Compare(iSRTCP_RFC112bitSessionAuthKey_dehexed) == 0);
    //reset result
    iResult.FillZ();
    // RFC test salt (112 bit) and SRTCP salting label (0x05)
    TSRTPUtils::Cnt_x(KSRTCP_39bit_salt_key_id, iResult, iRFCTestSalt112bits_dehexed);   
    EUNIT_ASSERT( iResult.Compare(iSRTCP_RFC112bitSessionSaltKey_dehexed) == 0);

    }

void UT_SRTPUtils::UT_SRTPUtils_SrtpDIVL(  )
    {
    TInt a = 9;
    TInt b = 3;
    EUNIT_ASSERT( TSRTPUtils::SrtpDIV(a,b) == 3);
    
    b=0;
    EUNIT_ASSERT( TSRTPUtils::SrtpDIV(a,b) == 0);
    }

void UT_SRTPUtils::UT_SRTPUtils_DeHexL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_SRTPUtils::UT_SRTPUtils_HexValL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE( 
    UT_SRTPUtils,
    "SRTPUtils",
    "UNIT" )

EUNIT_TEST(
    "Cnt_r - test ",
    "SRTPUtils",
    "Cnt_r",
    "FUNCTIONALITY",
    SetupL, UT_SRTPUtils_Cnt_rL, Teardown)

EUNIT_TEST(
    "Cnt_key_id - test ",
    "SRTPUtils",
    "Cnt_key_id",
    "FUNCTIONALITY",
    SetupL, UT_SRTPUtils_Cnt_key_idL, Teardown)

EUNIT_TEST(
    "Cnt_x - test ",
    "SRTPUtils",
    "Cnt_x",
    "FUNCTIONALITY",
    SetupL, UT_SRTPUtils_Cnt_xL, Teardown)

EUNIT_TEST(
    "SrtpDIV - test ",
    "SRTPUtils",
    "SrtpDIV",
    "FUNCTIONALITY",
    SetupL, UT_SRTPUtils_SrtpDIVL, Teardown)

EUNIT_TEST(
    "DeHex - test ",
    "SRTPUtils",
    "DeHex",
    "FUNCTIONALITY",
    SetupL, UT_SRTPUtils_DeHexL, Teardown)

EUNIT_TEST(
    "HexVal - test ",
    "SRTPUtils",
    "HexVal",
    "FUNCTIONALITY",
    SetupL, UT_SRTPUtils_HexValL, Teardown)

EUNIT_TEST(
    "GetIVFirstTerm - test ",
    "SRTPUtils",
    "GetIVFirstTerm",
    "FUNCTIONALITY",
    SetupL, UT_GetIVFirstTermL, Teardown)

EUNIT_TEST(
    "GetIVSecondTerm - test ",
    "SRTPUtils",
    "GetIVSecondTerm",
    "FUNCTIONALITY",
    SetupL, UT_GetIVSecondTermL, Teardown)

EUNIT_TEST(
    "GetIVThirdTerm - test ",
    "SRTPUtils",
    "GetIVThirdTerm",
    "FUNCTIONALITY",
    SetupL, UT_GetIVThirdTermL, Teardown)

EUNIT_TEST(
    "CountIV - test ",
    "SRTPUtils",
    "CountIV",
    "FUNCTIONALITY",
    SetupL, UT_CountIVL, Teardown)


EUNIT_END_TEST_TABLE

//  END OF FILE

/*    
    iSalt.Append(0x0E);
    iSalt.Append(0xC6);
    iSalt.Append(0x75);
    iSalt.Append(0xAD);
    iSalt.Append(0x49);
    iSalt.Append(0x8A);
    iSalt.Append(0xFE);
    iSalt.Append(0xEB);
    iSalt.Append(0xB6);
    iSalt.Append(0x96);
    iSalt.Append(0x0B);
    iSalt.Append(0x3A);
    iSalt.Append(0xAB);
    iSalt.Append(0xE6); */


/*
    TBuf8<320> salt;
    _LIT8(KFormatHex,"[%d %d %d %d %d %d %d %d %d %d %d %d %d %d] ");

    salt.Format(KFormatHex, 0x0E,0xC6,0x75,0xAD,0x49,0x8A, 
                            0xFE,0xEB,0xB6,0x96,0x0B,0x3A,
                            0xAB,0xE6);  */                             
                               

//    _LIT8(KTestMasterSalt,"0EC675AD498AFEEBB6960B3AABE6");
//    _LIT8(KTestSaltResult,"0EC675AD498AFEE9B6960B3AABE6");
//    TBuf8<32> salt(KTestMasterSalt);        
    

/*
TBuf8<256> tgt;
...
_LIT8(KFormat1,"[%b %c %d %o %u %x]");
tgt.Format(KFormat1,65,65,65,65,65,65);//generates:
... //[1000001 A 65 101 65 41]
...      */
   
/*    

TBuf8<16> tgt(Kabc);   */  

