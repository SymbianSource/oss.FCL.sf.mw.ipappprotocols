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
#include "UT_CSRTPKeyDerivation_AESCM128.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "srtpkeyderivation_aescm128.h"

	_LIT8(KRFC3711_Test1Key128bits,         "2B7E151628AED2A6ABF7158809CF4F3C");
	_LIT8(KRFC3711_Test2Key128bits,         "E1F97A0D3E018BE0D64FA32C06DE4139");
	_LIT8(KRFC3711_TestIV128bits,           "F0F1F2F3F4F5F6F7F8F9FAFBFCFD0000");
    _LIT8(KRFC3711_TestKeystream128bits,    "E03EAD0935C95E80E166B16DD92B4EB4");  

	_LIT8(KRFC3711_SessionEncrIV128bits,    "0EC675AD498AFEEBB6960B3AABE60000");
	_LIT8(KRFC3711_SessionSaltIV128bits,    "0EC675AD498AFEE9B6960B3AABE60000");
	_LIT8(KRFC3711_SessionAuthIV128bits,    "0EC675AD498AFEEAB6960B3AABE60000");
	
    _LIT8(KRFC3711_SessionEncrKey128bits,   "C61E7A93744F39EE10734AFE3FF7A087");  
    _LIT8(KRFC3711_SessionSaltKey128bits,   "30CBBC08863D8C85D49DB34A9AE17AC6");  
    _LIT8(KRFC3711_SessionAuthKey128bits,   "CEBE321F6FF7716B6FD4AB49AF256A15");  

	_LIT8(KTooShortData112bits,             "F0F1F2F3F4F5F6F7F8F9FAFBFCFD");
	_LIT8(KTooLongData160bits,              "F0F1F2F3F4F5F6F7F8F9FAFBFCFD9FAFBFCFD0FF");

#define RTP_EUNIT_ASSERT_EQUALS( val1, val2 ) \
if ( val2 != KErrNoMemory && val1 == KErrNoMemory ){\
	User::Leave( KErrNoMemory );\
	}\
else{\
	EUNIT_ASSERT_EQUALS( val1, val2 );\
	}
	
// CONSTRUCTION
UT_CSRTPKeyDerivation_AESCM128* UT_CSRTPKeyDerivation_AESCM128::NewL()
    {
    UT_CSRTPKeyDerivation_AESCM128* self = UT_CSRTPKeyDerivation_AESCM128::NewLC(); 
    CleanupStack::Pop();

    return self;
    }

UT_CSRTPKeyDerivation_AESCM128* UT_CSRTPKeyDerivation_AESCM128::NewLC()
    {
    UT_CSRTPKeyDerivation_AESCM128* self = new( ELeave ) UT_CSRTPKeyDerivation_AESCM128();
    CleanupStack::PushL( self );

	self->ConstructL(); 

    return self;
    }

// Destructor (virtual by CBase)
UT_CSRTPKeyDerivation_AESCM128::~UT_CSRTPKeyDerivation_AESCM128()
    {
    }

// Default constructor
UT_CSRTPKeyDerivation_AESCM128::UT_CSRTPKeyDerivation_AESCM128()
    {
    }

// Second phase construct
void UT_CSRTPKeyDerivation_AESCM128::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_CSRTPKeyDerivation_AESCM128::SetupL(  )
    {
    iEncryptor = CSRTPKeyDerivation_AESCM128::NewL();    

	iRFC3711_Test1Key128bits = HBufC8::NewL(KRFC3711_Test1Key128bits().Length());
	iRFC3711_Test2Key128bits = HBufC8::NewL(KRFC3711_Test2Key128bits().Length());
	iRFC3711_TestIV128bits = HBufC8::NewL(KRFC3711_TestIV128bits().Length());
	iRFC3711_TestKeystream128bits = HBufC8::NewL(KRFC3711_TestKeystream128bits().Length());

    iRFC3711_SessionEncrIV128bits = HBufC8::NewL(KRFC3711_SessionEncrIV128bits().Length());
    iRFC3711_SessionSaltIV128bits = HBufC8::NewL(KRFC3711_SessionSaltIV128bits().Length());
    iRFC3711_SessionAuthIV128bits = HBufC8::NewL(KRFC3711_SessionAuthIV128bits().Length());

    iRFC3711_SessionEncrKey128bits = HBufC8::NewL(KRFC3711_SessionEncrKey128bits().Length());
    iRFC3711_SessionSaltKey128bits = HBufC8::NewL(KRFC3711_SessionSaltKey128bits().Length());
    iRFC3711_SessionAuthKey128bits = HBufC8::NewL(KRFC3711_SessionAuthKey128bits().Length());


	*iRFC3711_Test1Key128bits = KRFC3711_Test1Key128bits;
	*iRFC3711_Test2Key128bits = KRFC3711_Test2Key128bits;
	*iRFC3711_TestIV128bits = KRFC3711_TestIV128bits;
	*iRFC3711_TestKeystream128bits = KRFC3711_TestKeystream128bits;
		
	*iRFC3711_SessionEncrIV128bits = KRFC3711_SessionEncrIV128bits;
	*iRFC3711_SessionSaltIV128bits = KRFC3711_SessionSaltIV128bits;
	*iRFC3711_SessionAuthIV128bits = KRFC3711_SessionAuthIV128bits;

	*iRFC3711_SessionEncrKey128bits = KRFC3711_SessionEncrKey128bits;
	*iRFC3711_SessionSaltKey128bits = KRFC3711_SessionSaltKey128bits;
	*iRFC3711_SessionAuthKey128bits = KRFC3711_SessionAuthKey128bits;

	
	Hex(*iRFC3711_Test1Key128bits);
	Hex(*iRFC3711_Test2Key128bits);
	Hex(*iRFC3711_TestIV128bits);
	Hex(*iRFC3711_TestKeystream128bits);

	Hex(*iRFC3711_SessionEncrIV128bits);
	Hex(*iRFC3711_SessionSaltIV128bits);
	Hex(*iRFC3711_SessionAuthIV128bits);

	Hex(*iRFC3711_SessionEncrKey128bits);
	Hex(*iRFC3711_SessionSaltKey128bits);
	Hex(*iRFC3711_SessionAuthKey128bits);
    } 

void UT_CSRTPKeyDerivation_AESCM128::Teardown(  )
    {

	delete iRFC3711_Test1Key128bits;
	delete iRFC3711_Test2Key128bits;
	delete iRFC3711_TestIV128bits;
	delete iRFC3711_TestKeystream128bits;

    delete iRFC3711_SessionEncrIV128bits;
    delete iRFC3711_SessionSaltIV128bits;
    delete iRFC3711_SessionAuthIV128bits;

    delete iRFC3711_SessionEncrKey128bits;
    delete iRFC3711_SessionSaltKey128bits;
    delete iRFC3711_SessionAuthKey128bits;

	iRFC3711_Test1Key128bits = NULL;
	iRFC3711_Test2Key128bits = NULL;
	iRFC3711_TestIV128bits = NULL;
	iRFC3711_TestKeystream128bits = NULL;

    iRFC3711_SessionEncrIV128bits = NULL;
    iRFC3711_SessionSaltIV128bits = NULL;
    iRFC3711_SessionAuthIV128bits = NULL;

    iRFC3711_SessionEncrKey128bits = NULL;
    iRFC3711_SessionSaltKey128bits = NULL;
    iRFC3711_SessionAuthKey128bits = NULL;

    delete iEncryptor;
    iEncryptor = NULL;

    }


void UT_CSRTPKeyDerivation_AESCM128::UT_RFC3711_AESCM_TestL()
    {
 	HBufC8* result = iEncryptor->PRF_128BitL(128, *iRFC3711_Test1Key128bits, 
 	                                         *iRFC3711_TestIV128bits);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC3711_TestKeystream128bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }

void UT_CSRTPKeyDerivation_AESCM128::UT_RFC3711_Session_Encr_Key_TestL()
    {
 	HBufC8* result = iEncryptor->PRF_128BitL(128, *iRFC3711_Test2Key128bits, 
 	                                         *iRFC3711_SessionEncrIV128bits);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC3711_SessionEncrKey128bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    
    }

void UT_CSRTPKeyDerivation_AESCM128::UT_RFC3711_Session_Salt_Key_TestL()
    {
 	HBufC8* result = iEncryptor->PRF_128BitL(128, *iRFC3711_Test2Key128bits, 
 	                                         *iRFC3711_SessionSaltIV128bits);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC3711_SessionSaltKey128bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }

void UT_CSRTPKeyDerivation_AESCM128::UT_RFC3711_Session_Auth_Key_TestL()
    {
 	HBufC8* result = iEncryptor->PRF_128BitL(128, *iRFC3711_Test2Key128bits, 
 	                                         *iRFC3711_SessionAuthIV128bits);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC3711_SessionAuthKey128bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }


void UT_CSRTPKeyDerivation_AESCM128::UT_ErronousInput_OneL()
    {    
        
    TInt err = KErrNone;
    HBufC8* result = NULL;

    // too short data: 112 bits
	HBufC8* tooShortData = HBufC8::NewLC(KTooShortData112bits().Length());
	*tooShortData = KTooShortData112bits;
	Hex(*tooShortData);	
    	    	
    // too long data: 160 bits
	HBufC8* tooLongData = HBufC8::NewLC(KTooLongData160bits().Length());
	*tooLongData = KTooLongData160bits;
	Hex(*tooLongData);	

    // test too short IV: 112 bits
 	result = iEncryptor->PRF_128BitL(128, *iRFC3711_Test1Key128bits, 
 	                                         *tooShortData);
 	CleanupStack::PushL(result); 	
    EUNIT_ASSERT( result->Compare(*iRFC3711_TestKeystream128bits) == 0);
 	 	
    // too short key: 112 bits
 	TRAP(err, result = iEncryptor->PRF_128BitL(128, *tooShortData, 
 	                                         *iRFC3711_TestIV128bits));
 	RTP_EUNIT_ASSERT_EQUALS(err, KErrArgument);
 	
    // too long IV: 160 bits
 	TRAP(err, result = iEncryptor->PRF_128BitL(128, *iRFC3711_Test1Key128bits, 
 	                                         *tooLongData));
 	RTP_EUNIT_ASSERT_EQUALS(err, KErrArgument);

    // too long key: 160 bits
 	TRAP(err, result = iEncryptor->PRF_128BitL(128, *tooLongData, 
 	                                         *iRFC3711_TestIV128bits));
 	RTP_EUNIT_ASSERT_EQUALS(err, KErrArgument);

    CleanupStack::PopAndDestroy(3);
    }


void UT_CSRTPKeyDerivation_AESCM128::Hex(HBufC8& aString)
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
    UT_CSRTPKeyDerivation_AESCM128,
    "CSRTPKeyDerivation_AESCM128",
    "UNIT" )

EUNIT_TEST(
    "PRF_128L - AESCTR",
    "CSRTPKeyDerivation_AESCM128",
    "PRF_128L",
    "FUNCTIONALITY",
    SetupL, UT_RFC3711_AESCM_TestL, Teardown)

EUNIT_TEST(
    "PRF_128L - EncrKey test",
    "CSRTPKeyDerivation_AESCM128",
    "PRF_128L",
    "FUNCTIONALITY",
    SetupL, UT_RFC3711_Session_Encr_Key_TestL, Teardown)

EUNIT_TEST(
    "PRF_128L - SaltKey test",
    "CSRTPKeyDerivation_AESCM128",
    "PRF_128L",
    "FUNCTIONALITY",
    SetupL, UT_RFC3711_Session_Salt_Key_TestL, Teardown)

EUNIT_TEST(
    "PRF_128L - AuthKey test",
    "CSRTPKeyDerivation_AESCM128",
    "PRF_128L",
    "FUNCTIONALITY",
    SetupL, UT_RFC3711_Session_Auth_Key_TestL, Teardown)

EUNIT_TEST(
    "PRF_128L ErrorInput",
    "CSRTPKeyDerivation_AESCM128",
    "PRF_128L",
    "FUNCTIONALITY",
    SetupL, UT_ErronousInput_OneL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE

/*
void UT_CSRTPKeyDerivation_AESCM128::UT_ErronousInput_OneL()
    {
    TInt err = KErrNone;
    HBufC8* result = NULL;
    
    // too short data: 96 bits
	HBufC8* tooShortData = HBufC8::NewL(KTooShortData96bits().Length());
	*tooShortData = KTooShortData96bits;
    CleanupStack::PushL(tooShortData);	
	   
    // too long data: 160 bits
	HBufC8* tooLongData = HBufC8::NewL(KTooLongData160bits().Length());
	*tooLongData = KTooLongData160bits;
    CleanupStack::PushL(tooLongData);	
	
    // test too short IV: 96 bits
 	TRAP(err, result = iEncryptor->PRF_128L(*iRFC3711_Test1Key128bits, 
 	                                         *tooShortData));
 	EUNIT_ASSERT(err == KErrArgument);

    // too short key: 96 bits
 	TRAP(err, result = iEncryptor->PRF_128L(*tooShortData, 
 	                                         *iRFC3711_TestIV128bits));
 	EUNIT_ASSERT(err == KErrArgument);

    // too long IV: 160 bits
 	TRAP(err, result = iEncryptor->PRF_128L(*iRFC3711_Test1Key128bits, 
 	                                         *tooLongData));
 	EUNIT_ASSERT(err == KErrArgument);

    // too long key: 160 bits
 	TRAP(err, result = iEncryptor->PRF_128L(*tooLongData, 
 	                                         *iRFC3711_TestIV128bits));
 	EUNIT_ASSERT(err == KErrArgument);

 	CleanupStack::Pop(2); 	
 	delete tooShortData;
 	delete tooLongData;
    }

*/