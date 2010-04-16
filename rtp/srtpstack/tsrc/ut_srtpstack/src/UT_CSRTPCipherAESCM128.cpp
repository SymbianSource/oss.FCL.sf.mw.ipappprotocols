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
#include "UT_CSRTPCipherAESCM128.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <e32debug.h>

//  INTERNAL INCLUDES
#include "srtpcipher_aescm128.h"
#include "srtpdef.h"

    // 128 bit key, test 1
	_LIT8(KRFC3686_TestKey128bits,     "AE6852F8121067CC4BF7A5765577F39E");
    // 128 bit key, test 2
	_LIT8(KRFC3686_TestKey_2_128bits,  "7E24067817FAE0D743D6CE1F32539163");
    // 128 bit IV, test 1
	_LIT8(KRFC3686_TestIV128bits,      "00000030000000000000000000000001");
    // 128 bit IV, test 2
	_LIT8(KRFC3686_TestIV_2_128bits,   "006CB6DBC0543B59DA48D90B00000001");
    // 128 bit plain text
	_LIT8(KRFC3686_TestPlainT128bits,  "53696E676C6520626C6F636B206D7367");
    // 128 bit cipher text
    _LIT8(KRFC3686_TestCipherT128bits, "E4095D4FB7A7B3792D6175A3261311B8");                                        
    // 104 bit plain text
	_LIT8(KRFC3686_TestPlainT104bits,  "53696E676C6520626C6F636B20");
    // 104 bit cipher text
    _LIT8(KRFC3686_TestCipherT104bits, "E4095D4FB7A7B3792D6175A326");                                        
    // 256 bit plain text
	_LIT8(KRFC3686_TestPlainT256bits, 
	"000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F");
    // 232 bit plain text
	_LIT8(KRFC3686_TestPlainT232bits, 
	"000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C");
    // 256 bit cipher text
    _LIT8(KRFC3686_TestCipherT256bits,
    "5104A106168A72D9790D41EE8EDAD388EB2E1EFC46DA57C8FCE630DF9141BE28");                                        
    // 232 bit cipher text
    _LIT8(KRFC3686_TestCipherT232bits,
    "5104A106168A72D9790D41EE8EDAD388EB2E1EFC46DA57C8FCE630DF91");                                        
    // 96 bit data, too short for key or IV
	_LIT8(KTooShortData96bits,      "F0F1F2F3F4F5F6F7F8F9FAFB");
    // 160 bit data, too long for key or IV
	_LIT8(KTooLongData160bits,      "F0F1F2F3F4F5F6F7F8F9FAFBFCFD9FAFBFCFD0FF");

#define RTP_EUNIT_ASSERT_EQUALS( val1, val2 ) \
if ( val2 != KErrNoMemory && val1 == KErrNoMemory ){\
	User::Leave( KErrNoMemory );\
	}\
else{\
	EUNIT_ASSERT_EQUALS( val1, val2 );\
	}
	
// CONSTRUCTION
UT_CSRTPCipherAESCM128* UT_CSRTPCipherAESCM128::NewL()
    {
    UT_CSRTPCipherAESCM128* self = UT_CSRTPCipherAESCM128::NewLC(); 
    CleanupStack::Pop();

    return self;
    }

UT_CSRTPCipherAESCM128* UT_CSRTPCipherAESCM128::NewLC()
    {
    UT_CSRTPCipherAESCM128* self = new( ELeave ) UT_CSRTPCipherAESCM128();
    CleanupStack::PushL( self );

	self->ConstructL(); 

    return self;
    }

// Destructor (virtual by CBase)
UT_CSRTPCipherAESCM128::~UT_CSRTPCipherAESCM128()
    {
    }

// Default constructor
UT_CSRTPCipherAESCM128::UT_CSRTPCipherAESCM128()
    {
    }

// Second phase construct
void UT_CSRTPCipherAESCM128::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_CSRTPCipherAESCM128::SetupL(  )
    {
    iEncryptor = CSRTPCipherAESCM128::NewL();    

	iRFC3686_TestKey128bits = HBufC8::NewL(KRFC3686_TestKey128bits().Length());
	iRFC3686_TestKey_2_128bits = HBufC8::NewL(KRFC3686_TestKey_2_128bits().Length());
	iRFC3686_TestIV128bits = HBufC8::NewL(KRFC3686_TestIV128bits().Length());
	iRFC3686_TestIV_2_128bits = HBufC8::NewL(KRFC3686_TestIV_2_128bits().Length());
	iRFC3686_TestPlainT104bits = HBufC8::NewL(KRFC3686_TestPlainT104bits().Length());
	iRFC3686_TestCipherT104bits = HBufC8::NewL(KRFC3686_TestCipherT104bits().Length());
	iRFC3686_TestPlainT128bits = HBufC8::NewL(KRFC3686_TestPlainT128bits().Length());
	iRFC3686_TestCipherT128bits = HBufC8::NewL(KRFC3686_TestCipherT128bits().Length());
	iRFC3686_TestPlainT256bits = HBufC8::NewL(KRFC3686_TestPlainT256bits().Length());
	iRFC3686_TestCipherT256bits = HBufC8::NewL(KRFC3686_TestCipherT256bits().Length());
	iRFC3686_TestPlainT232bits = HBufC8::NewL(KRFC3686_TestPlainT232bits().Length());
	iRFC3686_TestCipherT232bits = HBufC8::NewL(KRFC3686_TestCipherT232bits().Length());

	*iRFC3686_TestKey128bits = KRFC3686_TestKey128bits;
	*iRFC3686_TestKey_2_128bits = KRFC3686_TestKey_2_128bits;
	*iRFC3686_TestIV128bits = KRFC3686_TestIV128bits;
	*iRFC3686_TestIV_2_128bits = KRFC3686_TestIV_2_128bits;
	*iRFC3686_TestPlainT104bits = KRFC3686_TestPlainT104bits;
	*iRFC3686_TestCipherT104bits = KRFC3686_TestCipherT104bits;
	*iRFC3686_TestPlainT128bits = KRFC3686_TestPlainT128bits;
	*iRFC3686_TestCipherT128bits = KRFC3686_TestCipherT128bits;
	*iRFC3686_TestPlainT256bits = KRFC3686_TestPlainT256bits;
	*iRFC3686_TestCipherT256bits = KRFC3686_TestCipherT256bits;
	*iRFC3686_TestPlainT232bits = KRFC3686_TestPlainT232bits;
	*iRFC3686_TestCipherT232bits = KRFC3686_TestCipherT232bits;
	
	Hex(*iRFC3686_TestKey128bits);
	Hex(*iRFC3686_TestKey_2_128bits);
	Hex(*iRFC3686_TestIV128bits);
	Hex(*iRFC3686_TestIV_2_128bits);
	Hex(*iRFC3686_TestPlainT104bits);
	Hex(*iRFC3686_TestCipherT104bits);
	Hex(*iRFC3686_TestPlainT128bits);
	Hex(*iRFC3686_TestCipherT128bits);
	Hex(*iRFC3686_TestPlainT256bits);
	Hex(*iRFC3686_TestCipherT256bits);
	Hex(*iRFC3686_TestPlainT232bits);
	Hex(*iRFC3686_TestCipherT232bits);

    } 

void UT_CSRTPCipherAESCM128::Teardown(  )
    {
	delete iRFC3686_TestKey128bits;
	delete iRFC3686_TestKey_2_128bits;
	delete iRFC3686_TestIV128bits;
	delete iRFC3686_TestIV_2_128bits;
	delete iRFC3686_TestPlainT104bits;
	delete iRFC3686_TestCipherT104bits;
	delete iRFC3686_TestPlainT128bits;
	delete iRFC3686_TestCipherT128bits;
	delete iRFC3686_TestPlainT256bits;
	delete iRFC3686_TestCipherT256bits;
	delete iRFC3686_TestPlainT232bits;
	delete iRFC3686_TestCipherT232bits;

	iRFC3686_TestKey128bits = NULL;
	iRFC3686_TestKey_2_128bits = NULL;
	iRFC3686_TestIV128bits = NULL;
	iRFC3686_TestIV_2_128bits = NULL;
	iRFC3686_TestPlainT104bits = NULL;
	iRFC3686_TestCipherT104bits = NULL;
	iRFC3686_TestPlainT128bits = NULL;
	iRFC3686_TestCipherT128bits = NULL;
	iRFC3686_TestPlainT256bits = NULL;
	iRFC3686_TestCipherT256bits = NULL;
	iRFC3686_TestPlainT232bits = NULL;
	iRFC3686_TestCipherT232bits = NULL;

    delete iEncryptor;
    iEncryptor = NULL;
    }

void UT_CSRTPCipherAESCM128::UT_EncryptL_EncryptDataL(  )
    {
 	HBufC8* result = iEncryptor->TransformL(*iRFC3686_TestKey128bits, 
 	                                         *iRFC3686_TestIV128bits,
 	                                         *iRFC3686_TestPlainT128bits);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC3686_TestCipherT128bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }

void UT_CSRTPCipherAESCM128::UT_EncryptL_EncryptData2L(  )
    {
 	HBufC8* result = iEncryptor->TransformL(*iRFC3686_TestKey_2_128bits, 
 	                                         *iRFC3686_TestIV_2_128bits,
 	                                         *iRFC3686_TestPlainT256bits);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC3686_TestCipherT256bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;
    }

void UT_CSRTPCipherAESCM128::UT_EncryptL_EncryptData3L(  )
    {
 	HBufC8* result = iEncryptor->TransformL(*iRFC3686_TestKey_2_128bits, 
 	                                         *iRFC3686_TestIV_2_128bits,
 	                                         *iRFC3686_TestPlainT232bits);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC3686_TestCipherT232bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }

void UT_CSRTPCipherAESCM128::UT_EncryptL_EncryptData4L(  )
    {
 	HBufC8* result = iEncryptor->TransformL(*iRFC3686_TestKey128bits, 
 	                                         *iRFC3686_TestIV128bits,
 	                                         *iRFC3686_TestPlainT104bits);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC3686_TestCipherT104bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }

void UT_CSRTPCipherAESCM128::UT_EncryptL_EncryptDataKeyChangesL(  )
    {
    UT_EncryptL_EncryptDataL();
    UT_EncryptL_EncryptData2L();
    }

void UT_CSRTPCipherAESCM128::UT_EncryptL_DecryptDataL(  )
    {
 	HBufC8* result = iEncryptor->TransformL(*iRFC3686_TestKey128bits, 
 	                                         *iRFC3686_TestIV128bits,
 	                                         *iRFC3686_TestCipherT128bits);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC3686_TestPlainT128bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result; 
    }

void UT_CSRTPCipherAESCM128::UT_EncryptL_DecryptData2L(  )
    {
 	HBufC8* result = iEncryptor->TransformL(*iRFC3686_TestKey_2_128bits, 
 	                                         *iRFC3686_TestIV_2_128bits,
 	                                         *iRFC3686_TestCipherT256bits);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC3686_TestPlainT256bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }

void UT_CSRTPCipherAESCM128::UT_EncryptL_DecryptData3L(  )
    {
 	HBufC8* result = iEncryptor->TransformL(*iRFC3686_TestKey_2_128bits, 
 	                                         *iRFC3686_TestIV_2_128bits,
 	                                         *iRFC3686_TestCipherT232bits);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC3686_TestPlainT232bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }

void UT_CSRTPCipherAESCM128::UT_EncryptL_DecryptData4L(  )
    {
 	HBufC8* result = iEncryptor->TransformL(*iRFC3686_TestKey128bits, 
 	                                         *iRFC3686_TestIV128bits,
 	                                         *iRFC3686_TestCipherT104bits);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC3686_TestPlainT104bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }

void UT_CSRTPCipherAESCM128::UT_EncryptL_DecryptDataKeyChangesL(  )
    {
    UT_EncryptL_DecryptDataL();
    UT_EncryptL_DecryptData2L();
    }

void UT_CSRTPCipherAESCM128::UT_EncryptL_ErrorTest1L(  )
    {	
    TInt err = KErrNone;
    HBufC8* result = NULL;

    // too short data: 96 bits
	HBufC8* tooShortData = HBufC8::NewLC(KTooShortData96bits().Length());
	*tooShortData = KTooShortData96bits;
	Hex(*tooShortData);	
    	
    // too long data: 160 bits
	HBufC8* tooLongData = HBufC8::NewLC(KTooLongData160bits().Length());
	*tooLongData = KTooLongData160bits;
	Hex(*tooLongData);	

    // test too short IV: 96 bits
 	TRAP(err, result = iEncryptor->TransformL(*iRFC3686_TestKey128bits, 
 	                                         *tooShortData,
 	                                         *iRFC3686_TestPlainT128bits));
 	RTP_EUNIT_ASSERT_EQUALS(err, KErrArgument);
 	 	
    // too short key: 96 bits
 	TRAP(err, result = iEncryptor->TransformL(*tooShortData, 
 	                                         *iRFC3686_TestIV128bits,
 	                                         *iRFC3686_TestPlainT128bits));
 	RTP_EUNIT_ASSERT_EQUALS(err, KErrArgument);
 	
    // too long IV: 160 bits
 	TRAP(err, result = iEncryptor->TransformL(*iRFC3686_TestKey128bits, 
 	                                         *tooLongData,
 	                                         *iRFC3686_TestPlainT128bits));
 	RTP_EUNIT_ASSERT_EQUALS(err,KErrArgument);

    // too long key: 160 bits
 	TRAP(err, result = iEncryptor->TransformL(*tooLongData, 
 	                                         *iRFC3686_TestIV128bits,
 	                                         *iRFC3686_TestPlainT128bits));
 	RTP_EUNIT_ASSERT_EQUALS(err, KErrArgument);

    CleanupStack::PopAndDestroy(2);			
    }


void UT_CSRTPCipherAESCM128::UT_EncryptL_ErrorTest2L(  )
    {	
    TInt err = KErrNone;
    HBufC8* result = NULL;
    	
    // too long text: > 2^23 bits
	HBufC8* tooLongText = HBufC8::NewMaxLC(KSRTPMaxTextLength+2);	
	
    // test too long text: > 2^23 bits
 	TRAP(err, result = iEncryptor->TransformL(*iRFC3686_TestKey128bits, 
 	                                         *iRFC3686_TestIV128bits,
 	                                         *tooLongText));
 	RTP_EUNIT_ASSERT_EQUALS(err, KErrArgument);

    CleanupStack::PopAndDestroy(1);			
    }


void UT_CSRTPCipherAESCM128::Hex(HBufC8& aString)
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

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE( 
    UT_CSRTPCipherAESCM128,
    "CSRTPCipherAESCM128",
    "UNIT" )

EUNIT_TEST(
    "EncryptL encrypt data",
    "CSRTPCipherAESCM128",
    "EncryptL",
    "FUNCTIONALITY",
    SetupL, UT_EncryptL_EncryptDataL, Teardown)

EUNIT_TEST(
    "EncryptL encrypt 2 data",
    "CSRTPCipherAESCM128",
    "EncryptL",
    "FUNCTIONALITY",
    SetupL, UT_EncryptL_EncryptData2L, Teardown)

EUNIT_TEST(
    "EncryptL encrypt 3 data",
    "CSRTPCipherAESCM128",
    "EncryptL",
    "FUNCTIONALITY",
    SetupL, UT_EncryptL_EncryptData3L, Teardown)

EUNIT_TEST(
    "EncryptL encrypt 4 data",
    "CSRTPCipherAESCM128",
    "EncryptL",
    "FUNCTIONALITY",
    SetupL, UT_EncryptL_EncryptData4L, Teardown)

EUNIT_TEST(
    "EncryptL encrypt data, key changes",
    "CSRTPCipherAESCM128",
    "EncryptL",
    "FUNCTIONALITY",
    SetupL, UT_EncryptL_EncryptDataKeyChangesL, Teardown)
    
EUNIT_TEST(
    "EncryptL decrypt data",
    "CSRTPCipherAESCM128",
    "EncryptL",
    "FUNCTIONALITY",
    SetupL, UT_EncryptL_DecryptDataL, Teardown)

EUNIT_TEST(
    "EncryptL decrypt 2 data",
    "CSRTPCipherAESCM128",
    "EncryptL",
    "FUNCTIONALITY",
    SetupL, UT_EncryptL_DecryptData2L, Teardown)

EUNIT_TEST(
    "EncryptL decrypt 3 data",
    "CSRTPCipherAESCM128",
    "EncryptL",
    "FUNCTIONALITY",
    SetupL, UT_EncryptL_DecryptData3L, Teardown)

EUNIT_TEST(
    "EncryptL decrypt 4 data",
    "CSRTPCipherAESCM128",
    "EncryptL",
    "FUNCTIONALITY",
    SetupL, UT_EncryptL_DecryptData4L, Teardown)
    
EUNIT_TEST(
    "EncryptL decrypt data, key changes",
    "CSRTPCipherAESCM128",
    "EncryptL",
    "FUNCTIONALITY",
    SetupL, UT_EncryptL_DecryptDataKeyChangesL, Teardown)

EUNIT_TEST(
    "EncryptL error 1",
    "CSRTPCipherAESCM128",
    "EncryptL",
    "FUNCTIONALITY",
    SetupL, UT_EncryptL_ErrorTest1L, Teardown)

EUNIT_TEST(
    "EncryptL error 2",
    "CSRTPCipherAESCM128",
    "EncryptL",
    "FUNCTIONALITY",
    SetupL, UT_EncryptL_ErrorTest2L, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE
