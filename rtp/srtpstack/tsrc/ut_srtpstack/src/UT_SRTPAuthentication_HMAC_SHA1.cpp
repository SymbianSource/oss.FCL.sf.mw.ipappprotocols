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
#include "UT_SRTPAuthentication_HMAC_SHA1.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include "hash.h"
#include "srtputils.h"

//  INTERNAL INCLUDES
#include "srtpauthentication_hmac_sha1.h"

// RFC 2202 test 1 160 bit key
_LIT8(KRFC2202_Test1_Key_160bits,     "0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B");
// RFC 2202 test 1 16 bit data
_LIT8(KRFC2202_Test1_Data_16bits,     "Hi There");
// RFC 2202 test 1 160 bit digest/tag, not used here
_LIT8(KRFC2202_Test1_Tag_160bits,     "B617318655057264E28BC0B6FB378C8EF146BE00");
// RFC 2202 test 1 80 bit digest/tag
_LIT8(KRFC2202_Test1_Tag_80bits,      "B617318655057264E28B");
// RFC 2202 test 1 32 bit digest/tag
_LIT8(KRFC2202_Test1_Tag_32bits,      "B6173186");

/*test_case =     1
key =           0x0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b
key_len =       20
data =          "Hi There"
data_len =      8
digest =        0xb617318655057264e28bc0b6fb378c8ef146be00


*/
// RFC 2202 test 2 key
_LIT8(KRFC2202_Test2_Key,             "Jefe");
// RFC 2202 test 2 16 bit data
_LIT8(KRFC2202_Test2_Data_28bits,     "what do ya want for nothing?");
// RFC 2202 test 2 160 bit digest/tag, not used
_LIT8(KRFC2202_Test2_Tag_160bits,     "EFFCDF6AE5EB2FA2D27416D5F184DF9C259A7C79");
// RFC 2202 test 2 80 bit digest/tag
_LIT8(KRFC2202_Test2_Tag_80bits,      "EFFCDF6AE5EB2FA2D274");
// RFC 2202 test 2 32 bit digest/tag
_LIT8(KRFC2202_Test2_Tag_32bits,      "EFFCDF6AE5");
/*
test_case =     2
key =           "Jefe"
key_len =       4
data =          "what do ya want for nothing?"
data_len =      28
digest =        0xeffcdf6ae5eb2fa2d27416d5f184df9c259a7c79


*/


// RFC 2202 test 3 160 bit key
_LIT8(KRFC2202_Test3_Key_160bits,     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
// RFC 2202 test 3 400 bit data
_LIT8(KRFC2202_Test3_Data_400bits,
"DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD");
// RFC 2202 test 3 160 bit digest/tag, not used
_LIT8(KRFC2202_Test3_Tag_160bits,     "125D7342B9AC11CD91A39AF48AA17B4F63F175D3");
// RFC 2202 test 3 80 bit digest/tag
_LIT8(KRFC2202_Test3_Tag_80bits,      "125D7342B9AC11CD91A3");
// RFC 2202 test 3 32 bit digest/tag
_LIT8(KRFC2202_Test3_Tag_32bits,      "125D7342");

// RFC 2202 test 4 200 bit key
_LIT8(KRFC2202_Test4_Key_200bits,     "0102030405060708090A0B0C0D0E0F10111213141516171819");
// RFC 2202 test 4 400 bit data
_LIT8(KRFC2202_Test4_Data_400bits,
"CDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCD");
// RFC 2202 test 4 160 bit digest/tag, not used
_LIT8(KRFC2202_Test4_Tag_160bits,     "4C9007F4026250C6BC8414F9BF50C86C2D7235DA");
// RFC 2202 test 4 80 bit digest/tag
_LIT8(KRFC2202_Test4_Tag_80bits,      "4C9007F4026250C6BC84");
// RFC 2202 test 4 32 bit digest/tag
_LIT8(KRFC2202_Test4_Tag_32bits,      "4C9007F4");

// RFC 2202 test 5 160 bit key
_LIT8(KRFC2202_Test5_Key_160bits,     "0C0C0C0C0C0C0C0C0C0C0C0C0C0C0C0C0C0C0C0C");
// RFC 2202 test 5 data
_LIT8(KRFC2202_Test5_Data        ,    "Test With Truncation");
// RFC 2202 test 5 160 bit digest/tag, not used
_LIT8(KRFC2202_Test5_Tag_160bits,     "4C1A03424B55E07FE7F27BE1D58BB9324A9A5A04");
// RFC 2202 test 5 80 bit digest/tag
_LIT8(KRFC2202_Test5_Tag_80bits,      "4C1A03424B55E07FE7F2");
// RFC 2202 test 5 32 bit digest/tag
_LIT8(KRFC2202_Test5_Tag_32bits,      "4C1A0342");


// RFC 2202 test 6 640 bit key
_LIT8(KRFC2202_Test6_Key_640bits,
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
// RFC 2202 test 6 54 bit data
_LIT8(KRFC2202_Test6_Data_54bits,     "Test Using Larger Than Block-Size Key - Hash Key First");
// RFC 2202 test 6 160 bit digest/tag, not used
_LIT8(KRFC2202_Test6_Tag_160bits,     "AA4AE5E15272D00E95705637CE8A3B55ED402112");
// RFC 2202 test 6 80 bit digest/tag
_LIT8(KRFC2202_Test6_Tag_80bits,      "AA4AE5E15272D00E9570");
// RFC 2202 test 6 32 bit digest/tag
_LIT8(KRFC2202_Test6_Tag_32bits,      "AA4AE5E1");

// RFC 2202 test 7 640 bit key
_LIT8(KRFC2202_Test7_Key_640bits,
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
// RFC 2202 test 7 73 bit data
_LIT8(KRFC2202_Test7_Data_73bits,     "Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data");
// RFC 2202 test 7 160 bit digest/tag
_LIT8(KRFC2202_Test7_Tag_160bits,     "E8E99D0F45237D786D6BBAA7965C7808BBFF1A91");
// RFC 2202 test 7 80 bit digest/tag
_LIT8(KRFC2202_Test7_Tag_80bits,      "E8E99D0F45237D786D6B");
// RFC 2202 test 7 32 bit digest/tag
_LIT8(KRFC2202_Test7_Tag_32bits,      "E8E99D0F");
                                       
//OpenSrc test cases

_LIT8(KTest8_Key_160bits,"CEBE321F6FF7716B6FD4AB49AF256A156D38BAA4");

//_LIT8(KTest8_Data_168bits,     "800FBDC200000001DEADBEEF38D740EBF094E5E050");
_LIT8(KTest8_Data_168bits,     "8001CDDD00000001DEADBEEFF9206D8823BACDD");
_LIT8(KTest8_Data2_32bits,		"00000000");
_LIT8(KTest8_Tag_80bits,     "F21BB78B7A6916E88138");

// CONSTRUCTION
UT_SRTPAuthentication_HMAC_SHA1* UT_SRTPAuthentication_HMAC_SHA1::NewL()
    {
    UT_SRTPAuthentication_HMAC_SHA1* self = UT_SRTPAuthentication_HMAC_SHA1::NewLC(); 
    CleanupStack::Pop();

    return self;
    }

UT_SRTPAuthentication_HMAC_SHA1* UT_SRTPAuthentication_HMAC_SHA1::NewLC()
    {
    UT_SRTPAuthentication_HMAC_SHA1* self = new( ELeave ) UT_SRTPAuthentication_HMAC_SHA1();
    CleanupStack::PushL( self );

	self->ConstructL(); 

    return self;
    }

// Destructor (virtual by CBase)
UT_SRTPAuthentication_HMAC_SHA1::~UT_SRTPAuthentication_HMAC_SHA1()
    {
    }

// Default constructor
UT_SRTPAuthentication_HMAC_SHA1::UT_SRTPAuthentication_HMAC_SHA1()
    {
    }

// Second phase construct
void UT_SRTPAuthentication_HMAC_SHA1::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_SRTPAuthentication_HMAC_SHA1::SetupL(  )
    {

    iAuthenticator = CSRTPAuthentication_HMAC_SHA1::NewL();    

    iRFC2202_Test1_Key_160bits = HBufC8::NewL(KRFC2202_Test1_Key_160bits().Length());
    iRFC2202_Test1_Data_16bits = HBufC8::NewL(KRFC2202_Test1_Data_16bits().Length());
    iRFC2202_Test1_Tag_80bits = HBufC8::NewL(KRFC2202_Test1_Tag_80bits().Length());
    iRFC2202_Test1_Tag_32bits = HBufC8::NewL(KRFC2202_Test1_Tag_32bits().Length());

    iRFC2202_Test2_Key         = HBufC8::NewL(KRFC2202_Test2_Key().Length());
    iRFC2202_Test2_Data_28bits = HBufC8::NewL(KRFC2202_Test2_Data_28bits().Length());
    iRFC2202_Test2_Tag_80bits = HBufC8::NewL(KRFC2202_Test2_Tag_80bits().Length());
    iRFC2202_Test2_Tag_32bits = HBufC8::NewL(KRFC2202_Test2_Tag_32bits().Length());

    iRFC2202_Test3_Key_160bits = HBufC8::NewL(KRFC2202_Test3_Key_160bits().Length());
    iRFC2202_Test3_Data_400bits = HBufC8::NewL(KRFC2202_Test3_Data_400bits().Length());
    iRFC2202_Test3_Tag_80bits = HBufC8::NewL(KRFC2202_Test3_Tag_80bits().Length());
    iRFC2202_Test3_Tag_32bits = HBufC8::NewL(KRFC2202_Test3_Tag_32bits().Length());

    iRFC2202_Test4_Key_200bits = HBufC8::NewL(KRFC2202_Test4_Key_200bits().Length());
    iRFC2202_Test4_Data_400bits = HBufC8::NewL(KRFC2202_Test4_Data_400bits().Length());
    iRFC2202_Test4_Tag_80bits = HBufC8::NewL(KRFC2202_Test4_Tag_80bits().Length());
    iRFC2202_Test4_Tag_32bits = HBufC8::NewL(KRFC2202_Test4_Tag_32bits().Length());

    iRFC2202_Test5_Key_160bits = HBufC8::NewL(KRFC2202_Test5_Key_160bits().Length());
    iRFC2202_Test5_Data        = HBufC8::NewL(KRFC2202_Test5_Data().Length());
    iRFC2202_Test5_Tag_80bits = HBufC8::NewL(KRFC2202_Test5_Tag_80bits().Length());
    iRFC2202_Test5_Tag_32bits = HBufC8::NewL(KRFC2202_Test5_Tag_32bits().Length());

    iRFC2202_Test6_Key_640bits = HBufC8::NewL(KRFC2202_Test6_Key_640bits().Length());
    iRFC2202_Test6_Data_54bits = HBufC8::NewL(KRFC2202_Test6_Data_54bits().Length());
    iRFC2202_Test6_Tag_80bits = HBufC8::NewL(KRFC2202_Test6_Tag_80bits().Length());
    iRFC2202_Test6_Tag_32bits = HBufC8::NewL(KRFC2202_Test6_Tag_32bits().Length());

    iRFC2202_Test7_Key_640bits = HBufC8::NewL(KRFC2202_Test7_Key_640bits().Length());
    iRFC2202_Test7_Data_73bits = HBufC8::NewL(KRFC2202_Test7_Data_73bits().Length());
    iRFC2202_Test7_Tag_80bits = HBufC8::NewL(KRFC2202_Test7_Tag_80bits().Length());
    iRFC2202_Test7_Tag_32bits = HBufC8::NewL(KRFC2202_Test7_Tag_32bits().Length());
    iTest8_Key_160bits= HBufC8::NewL(KTest8_Key_160bits().Length());
    iTest8_Data_168bits= HBufC8::NewL(KTest8_Data_168bits().Length());
    iTest8_Data2_32bits= HBufC8::NewL(KTest8_Data2_32bits().Length());
    iTest8_Tag_80bits= HBufC8::NewL(KTest8_Tag_80bits().Length());

	*iRFC2202_Test1_Key_160bits = KRFC2202_Test1_Key_160bits;
	*iRFC2202_Test1_Data_16bits = KRFC2202_Test1_Data_16bits;
	*iRFC2202_Test1_Tag_80bits  = KRFC2202_Test1_Tag_80bits;
	*iRFC2202_Test1_Tag_32bits  = KRFC2202_Test1_Tag_32bits;

	*iRFC2202_Test2_Key         = KRFC2202_Test2_Key;
	*iRFC2202_Test2_Data_28bits = KRFC2202_Test2_Data_28bits;
	*iRFC2202_Test2_Tag_80bits  = KRFC2202_Test2_Tag_80bits;
	*iRFC2202_Test2_Tag_32bits  = KRFC2202_Test2_Tag_32bits;

	*iRFC2202_Test3_Key_160bits = KRFC2202_Test3_Key_160bits;
	*iRFC2202_Test3_Data_400bits = KRFC2202_Test3_Data_400bits;
	*iRFC2202_Test3_Tag_80bits  = KRFC2202_Test3_Tag_80bits;
	*iRFC2202_Test3_Tag_32bits  = KRFC2202_Test3_Tag_32bits;

    *iRFC2202_Test4_Key_200bits = KRFC2202_Test4_Key_200bits;
    *iRFC2202_Test4_Data_400bits = KRFC2202_Test4_Data_400bits;
    *iRFC2202_Test4_Tag_80bits  = KRFC2202_Test4_Tag_80bits;
    *iRFC2202_Test4_Tag_32bits  = KRFC2202_Test4_Tag_32bits;

    *iRFC2202_Test5_Key_160bits = KRFC2202_Test5_Key_160bits;
    *iRFC2202_Test5_Data        = KRFC2202_Test5_Data;
    *iRFC2202_Test5_Tag_80bits  = KRFC2202_Test5_Tag_80bits;
    *iRFC2202_Test5_Tag_32bits  = KRFC2202_Test5_Tag_32bits;

	*iRFC2202_Test6_Key_640bits = KRFC2202_Test6_Key_640bits;
	*iRFC2202_Test6_Data_54bits = KRFC2202_Test6_Data_54bits;
	*iRFC2202_Test6_Tag_80bits  = KRFC2202_Test6_Tag_80bits;
	*iRFC2202_Test6_Tag_32bits  = KRFC2202_Test6_Tag_32bits;

	*iRFC2202_Test7_Key_640bits = KRFC2202_Test7_Key_640bits;
	*iRFC2202_Test7_Data_73bits = KRFC2202_Test7_Data_73bits;
	*iRFC2202_Test7_Tag_80bits  = KRFC2202_Test7_Tag_80bits;
	*iRFC2202_Test7_Tag_32bits = KRFC2202_Test7_Tag_32bits;
	*iTest8_Key_160bits= KTest8_Key_160bits;
    *iTest8_Data_168bits= KTest8_Data_168bits;
    *iTest8_Data2_32bits= KTest8_Data2_32bits;
    *iTest8_Tag_80bits= KTest8_Tag_80bits;

	Hex(*iRFC2202_Test1_Key_160bits);
//	Hex(*iRFC2202_Test1_Data_16bits);
	Hex(*iRFC2202_Test1_Tag_80bits);
	Hex(*iRFC2202_Test1_Tag_32bits);

//	Hex(*iRFC2202_Test2_Key);
//	Hex(*iRFC2202_Test2_Data_28bits);
	Hex(*iRFC2202_Test2_Tag_80bits);
	Hex(*iRFC2202_Test2_Tag_32bits);
	iRFC2202_Test2_Tag_32bits->Des().SetLength(32/8);

	Hex(*iRFC2202_Test3_Key_160bits);
	Hex(*iRFC2202_Test3_Data_400bits);
	Hex(*iRFC2202_Test3_Tag_80bits);
	Hex(*iRFC2202_Test3_Tag_32bits);

	Hex(*iRFC2202_Test4_Key_200bits);
	Hex(*iRFC2202_Test4_Data_400bits);
	Hex(*iRFC2202_Test4_Tag_80bits);
	Hex(*iRFC2202_Test4_Tag_32bits);

	Hex(*iRFC2202_Test5_Key_160bits);
//	Hex(*iRFC2202_Test5_Data);
	Hex(*iRFC2202_Test5_Tag_80bits);
	Hex(*iRFC2202_Test5_Tag_32bits);

	Hex(*iRFC2202_Test6_Key_640bits);
//	Hex(*iRFC2202_Test6_Data_54bits);
	Hex(*iRFC2202_Test6_Tag_80bits);
	Hex(*iRFC2202_Test6_Tag_32bits);

	Hex(*iRFC2202_Test7_Key_640bits);
//	Hex(*iRFC2202_Test7_Data_73bits);
	Hex(*iRFC2202_Test7_Tag_80bits);
	Hex(*iRFC2202_Test7_Tag_32bits);
	
	Hex(*iTest8_Key_160bits);
	Hex(*iTest8_Data_168bits);
	
	Hex(*iTest8_Data2_32bits);

	Hex(*iTest8_Tag_80bits);
	iTest8_Tag_80bits->Des().SetLength(10);
    } 

void UT_SRTPAuthentication_HMAC_SHA1::Teardown(  )
    {
    delete iAuthenticator;
    iAuthenticator = NULL;
	
	delete iRFC2202_Test1_Key_160bits;
	delete iRFC2202_Test1_Data_16bits;
	delete iRFC2202_Test1_Tag_80bits;
	delete iRFC2202_Test1_Tag_32bits;
	
	iRFC2202_Test1_Key_160bits = NULL;
	iRFC2202_Test1_Data_16bits = NULL;
	iRFC2202_Test1_Tag_80bits = NULL;
	iRFC2202_Test1_Tag_32bits = NULL;

	delete iRFC2202_Test2_Key;
	delete iRFC2202_Test2_Data_28bits;
	delete iRFC2202_Test2_Tag_80bits;
	delete iRFC2202_Test2_Tag_32bits;
	
    iRFC2202_Test2_Key = NULL;
	iRFC2202_Test2_Data_28bits = NULL;
	iRFC2202_Test2_Tag_80bits = NULL;
	iRFC2202_Test2_Tag_32bits = NULL;

	delete iRFC2202_Test3_Key_160bits;
	delete iRFC2202_Test3_Data_400bits;
	delete iRFC2202_Test3_Tag_80bits;
	delete iRFC2202_Test3_Tag_32bits;
	
    iRFC2202_Test3_Key_160bits = NULL;
	iRFC2202_Test3_Data_400bits = NULL;
	iRFC2202_Test3_Tag_80bits = NULL;
	iRFC2202_Test3_Tag_32bits = NULL;

    delete iRFC2202_Test4_Key_200bits;
    delete iRFC2202_Test4_Data_400bits;
    delete iRFC2202_Test4_Tag_80bits;
    delete iRFC2202_Test4_Tag_32bits;
	
	iRFC2202_Test4_Key_200bits = NULL;
    iRFC2202_Test4_Data_400bits = NULL;
    iRFC2202_Test4_Tag_80bits = NULL;
    iRFC2202_Test4_Tag_32bits = NULL;

    delete iRFC2202_Test5_Key_160bits;
    delete iRFC2202_Test5_Data;
    delete iRFC2202_Test5_Tag_80bits;
    delete iRFC2202_Test5_Tag_32bits;

	iRFC2202_Test5_Key_160bits = NULL;
    iRFC2202_Test5_Data = NULL;
    iRFC2202_Test5_Tag_80bits = NULL;
    iRFC2202_Test5_Tag_32bits = NULL;
	
	delete iRFC2202_Test6_Key_640bits;
	delete iRFC2202_Test6_Data_54bits;
	delete iRFC2202_Test6_Tag_80bits;
	delete iRFC2202_Test6_Tag_32bits;
	
    iRFC2202_Test6_Key_640bits = NULL;
	iRFC2202_Test6_Data_54bits = NULL;
	iRFC2202_Test6_Tag_80bits = NULL;
	iRFC2202_Test6_Tag_32bits = NULL;

	delete iRFC2202_Test7_Key_640bits;
	delete iRFC2202_Test7_Data_73bits;
	delete iRFC2202_Test7_Tag_80bits;
	delete iRFC2202_Test7_Tag_32bits;
	delete iTest8_Key_160bits;
	delete iTest8_Data_168bits;
	delete iTest8_Data2_32bits;
	delete iTest8_Tag_80bits;
	
    iRFC2202_Test7_Key_640bits = NULL;
	iRFC2202_Test7_Data_73bits = NULL;
	iRFC2202_Test7_Tag_80bits = NULL;
	iRFC2202_Test7_Tag_32bits = NULL;
	iTest8_Key_160bits = NULL;
	iTest8_Data_168bits = NULL;
	iTest8_Data2_32bits = NULL;
	iTest8_Tag_80bits = NULL;
    }


void UT_SRTPAuthentication_HMAC_SHA1::UT_AuthenticateL_RFC2202_Test1_EightyL(  )
    {
 	HBufC8* result = iAuthenticator->AuthenticateL(80, *iRFC2202_Test1_Key_160bits, 
 	                                         *iRFC2202_Test1_Data_16bits,
 	                                         KNullDesC8);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC2202_Test1_Tag_80bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;     	 	 	
    }

void UT_SRTPAuthentication_HMAC_SHA1::UT_AuthenticateL_RFC2202_Test1_ThirtyTwoL(  )
    {
 	HBufC8* result = iAuthenticator->AuthenticateL(32, *iRFC2202_Test1_Key_160bits, 
 	                                         *iRFC2202_Test1_Data_16bits,
 	                                         KNullDesC8);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC2202_Test1_Tag_32bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }


void UT_SRTPAuthentication_HMAC_SHA1::UT_AuthenticateL_RFC2202_Test2_EightyL(  )
    {
 	HBufC8* result = iAuthenticator->AuthenticateL(80, *iRFC2202_Test2_Key, 
 	                                         *iRFC2202_Test2_Data_28bits,
 	                                         KNullDesC8);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC2202_Test2_Tag_80bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }

void UT_SRTPAuthentication_HMAC_SHA1::UT_AuthenticateL_RFC2202_Test2_ThirtyTwoL(  )
    {
 	HBufC8* result = iAuthenticator->AuthenticateL(32, *iRFC2202_Test2_Key, 
 	                                         *iRFC2202_Test2_Data_28bits,
 	                                         KNullDesC8);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC2202_Test2_Tag_32bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }


void UT_SRTPAuthentication_HMAC_SHA1::UT_AuthenticateL_RFC2202_Test3_EightyL(  )
    {
 	HBufC8* result = iAuthenticator->AuthenticateL(80, *iRFC2202_Test3_Key_160bits, 
 	                                         *iRFC2202_Test3_Data_400bits,
 	                                         KNullDesC8);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC2202_Test3_Tag_80bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }

void UT_SRTPAuthentication_HMAC_SHA1::UT_AuthenticateL_RFC2202_Test3_ThirtyTwoL(  )
    {
 	HBufC8* result = iAuthenticator->AuthenticateL(32, *iRFC2202_Test3_Key_160bits, 
 	                                         *iRFC2202_Test3_Data_400bits,
 	                                         KNullDesC8);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC2202_Test3_Tag_32bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }

void UT_SRTPAuthentication_HMAC_SHA1::UT_AuthenticateL_RFC2202_Test4_EightyL(  )
    {
 	HBufC8* result = iAuthenticator->AuthenticateL(80, *iRFC2202_Test4_Key_200bits, 
 	                                         *iRFC2202_Test4_Data_400bits,
 	                                         KNullDesC8);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC2202_Test4_Tag_80bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }

void UT_SRTPAuthentication_HMAC_SHA1::UT_AuthenticateL_RFC2202_Test4_ThirtyTwoL(  )
    {
 	HBufC8* result = iAuthenticator->AuthenticateL(32, *iRFC2202_Test4_Key_200bits, 
 	                                         *iRFC2202_Test4_Data_400bits,
 	                                         KNullDesC8);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC2202_Test4_Tag_32bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }

void UT_SRTPAuthentication_HMAC_SHA1::UT_AuthenticateL_RFC2202_Test5_EightyL(  )
    {
 	HBufC8* result = iAuthenticator->AuthenticateL(80, *iRFC2202_Test5_Key_160bits, 
 	                                         *iRFC2202_Test5_Data,
 	                                         KNullDesC8);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC2202_Test5_Tag_80bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }

void UT_SRTPAuthentication_HMAC_SHA1::UT_AuthenticateL_RFC2202_Test5_ThirtyTwoL(  )
    {
 	HBufC8* result = iAuthenticator->AuthenticateL(32, *iRFC2202_Test5_Key_160bits, 
 	                                         *iRFC2202_Test5_Data,
 	                                         KNullDesC8);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC2202_Test5_Tag_32bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }

void UT_SRTPAuthentication_HMAC_SHA1::UT_AuthenticateL_RFC2202_Test6_EightyL(  )
    {
 	HBufC8* result = iAuthenticator->AuthenticateL(80, *iRFC2202_Test6_Key_640bits, 
 	                                         *iRFC2202_Test6_Data_54bits,
 	                                         KNullDesC8);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC2202_Test6_Tag_80bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }

void UT_SRTPAuthentication_HMAC_SHA1::UT_AuthenticateL_RFC2202_Test6_ThirtyTwoL(  )
    {
 	HBufC8* result = iAuthenticator->AuthenticateL(32, *iRFC2202_Test6_Key_640bits, 
 	                                         *iRFC2202_Test6_Data_54bits,
 	                                         KNullDesC8);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC2202_Test6_Tag_32bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }

void UT_SRTPAuthentication_HMAC_SHA1::UT_AuthenticateL_RFC2202_Test7_EightyL(  )
    {
 	HBufC8* result = iAuthenticator->AuthenticateL(80, *iRFC2202_Test7_Key_640bits, 
 	                                         *iRFC2202_Test7_Data_73bits,
 	                                         KNullDesC8);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC2202_Test7_Tag_80bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }

void UT_SRTPAuthentication_HMAC_SHA1::UT_AuthenticateL_RFC2202_Test7_ThirtyTwoL(  )
    {
 	HBufC8* result = iAuthenticator->AuthenticateL(32, *iRFC2202_Test7_Key_640bits, 
 	                                         *iRFC2202_Test7_Data_73bits,
 	                                         KNullDesC8);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC2202_Test7_Tag_32bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }


void UT_SRTPAuthentication_HMAC_SHA1::UT_AuthenticateL_Test8_ThirtyTwoL(  )
    {
    TBuf8<20> buf; 
    TBuf8<19> buf2;
    TBuf8<4> buf3;
    TUint8* temp = const_cast<TUint8*>( iTest8_Data_168bits->Des().Ptr() );
    
    TInt len = iTest8_Data_168bits->Length();
    TInt size = iTest8_Data_168bits->Size();
   
   	TUint32 roc=0;
    //fromhere read data/////////////////////////////////////////
    //"8001CDDD00000001DEADBEEFF9206D8823BACDD"
 	buf2.Append(0x80);
 	buf2.Append(0x01);
 
 	buf2.Append(0xCD);
 	buf2.Append(0xDD);
 	
 	buf2.Append(0x00);
 	buf2.Append(0x00);
 
 	buf2.Append(0x00);
 	buf2.Append(0x01);
 	
 	buf2.Append(0xDE);
 	buf2.Append(0xAD);
 	buf2.Append(0xBE);
 	buf2.Append(0xEF);
 	
 	buf2.Append(0x92);
	buf2.Append(0x06);
	buf2.Append(0xD8);
	buf2.Append(0x82);
	buf2.Append(0x3B);
	buf2.Append(0xAC);

	buf2.Append(0xDD);
	
	buf2.SetLength(19);
	buf3.Append(0x00);
	buf3.Append(0x00);
	buf3.Append(0x00);
	buf3.Append(0x00);
	buf3.SetLength(4);
    //end of writing data

    
 	HBufC8* result = iAuthenticator->AuthenticateL(80, *iTest8_Key_160bits, 
 	                                         buf2,
 	 											buf3);
 	CleanupStack::PushL(result);  											
	/*CSHA1* sha1= CSHA1::NewL();
	sha1->Reset();
	
	CHMAC* hmac= CHMAC::NewL(*iTest8_Key_160bits, sha1);
	
	hmac->Update(buf2); 
	
	buf.Copy(hmac->Final(buf3)); 
 	HBufC8* result = HBufC8::NewL(20);  
	CleanupStack::PushL(result); 	
	TPtr8 ptrOutputBuff = result->Des();	
 	*result = buf;
 	 
    ptrOutputBuff.SetLength(80/8);
    EUNIT_ASSERT( result->Compare(*iTest8_Tag_80bits) == 0);
	
	delete hmac;
*/
	EUNIT_ASSERT( result->Compare(*iTest8_Tag_80bits) == 0);
	buf.Zero();
	buf2.Zero();
	buf3.Zero();
 	CleanupStack::PopAndDestroy(result); 	
 	}

void UT_SRTPAuthentication_HMAC_SHA1::UT_AuthenticateL_Test_KeyChangedL(  )
    {
    UT_AuthenticateL_RFC2202_Test1_EightyL();
    UT_AuthenticateL_RFC2202_Test1_ThirtyTwoL();
    }

void UT_SRTPAuthentication_HMAC_SHA1::Hex(HBufC8& aString)
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
    UT_SRTPAuthentication_HMAC_SHA1,
    "CSRTPAuthentication_HMAC_SHA1",
    "UNIT" )

EUNIT_TEST(
    "AuthenticateL - 1 80 bits",
    "SRTPAuthentication_HMAC_SHA1",
    "AuthenticateL",
    "FUNCTIONALITY",
    SetupL, UT_AuthenticateL_RFC2202_Test1_EightyL, Teardown)

EUNIT_TEST(
    "AuthenticateL - 1 32 bits",
    "SRTPAuthentication_HMAC_SHA1",
    "AuthenticateL",
    "FUNCTIONALITY",
    SetupL, UT_AuthenticateL_RFC2202_Test1_ThirtyTwoL, Teardown)

EUNIT_TEST(
    "AuthenticateL - 2 80 bits",
    "SRTPAuthentication_HMAC_SHA1",
    "AuthenticateL",
    "FUNCTIONALITY",
    SetupL, UT_AuthenticateL_RFC2202_Test2_EightyL, Teardown)

EUNIT_TEST(
    "AuthenticateL - 2 32 bits",
    "SRTPAuthentication_HMAC_SHA1",
    "AuthenticateL",
    "FUNCTIONALITY",
    SetupL, UT_AuthenticateL_RFC2202_Test2_ThirtyTwoL, Teardown)

EUNIT_TEST(
    "AuthenticateL - 3 80 bits",
    "SRTPAuthentication_HMAC_SHA1",
    "AuthenticateL",
    "FUNCTIONALITY",
    SetupL, UT_AuthenticateL_RFC2202_Test3_EightyL, Teardown)

EUNIT_TEST(
    "AuthenticateL - 3 32 bits",
    "SRTPAuthentication_HMAC_SHA1",
    "AuthenticateL",
    "FUNCTIONALITY",
    SetupL, UT_AuthenticateL_RFC2202_Test3_ThirtyTwoL, Teardown)

EUNIT_TEST(
    "AuthenticateL - 4 80 bits",
    "SRTPAuthentication_HMAC_SHA1",
    "AuthenticateL",
    "FUNCTIONALITY",
    SetupL, UT_AuthenticateL_RFC2202_Test4_EightyL, Teardown)

EUNIT_TEST(
    "AuthenticateL - 4 32 bits",
    "SRTPAuthentication_HMAC_SHA1",
    "AuthenticateL",
    "FUNCTIONALITY",
    SetupL, UT_AuthenticateL_RFC2202_Test4_ThirtyTwoL, Teardown)

EUNIT_TEST(
    "AuthenticateL - 5 80 bits",
    "SRTPAuthentication_HMAC_SHA1",
    "AuthenticateL",
    "FUNCTIONALITY",
    SetupL, UT_AuthenticateL_RFC2202_Test5_EightyL, Teardown)

EUNIT_TEST(
    "AuthenticateL - 5 32 bits",
    "SRTPAuthentication_HMAC_SHA1",
    "AuthenticateL",
    "FUNCTIONALITY",
    SetupL, UT_AuthenticateL_RFC2202_Test5_ThirtyTwoL, Teardown)

EUNIT_TEST(
    "AuthenticateL - 6 80 bits",
    "SRTPAuthentication_HMAC_SHA1",
    "AuthenticateL",
    "FUNCTIONALITY",
    SetupL, UT_AuthenticateL_RFC2202_Test6_EightyL, Teardown)

EUNIT_TEST(
    "AuthenticateL - 6 32 bits",
    "SRTPAuthentication_HMAC_SHA1",
    "AuthenticateL",
    "FUNCTIONALITY",
    SetupL, UT_AuthenticateL_RFC2202_Test6_ThirtyTwoL, Teardown)

EUNIT_TEST(
    "AuthenticateL - 7 80 bits",
    "SRTPAuthentication_HMAC_SHA1",
    "AuthenticateL",
    "FUNCTIONALITY",
    SetupL, UT_AuthenticateL_RFC2202_Test7_EightyL, Teardown)

EUNIT_TEST(
    "AuthenticateL - 7 32 bits",
    "SRTPAuthentication_HMAC_SHA1",
    "AuthenticateL",
    "FUNCTIONALITY",
    SetupL, UT_AuthenticateL_RFC2202_Test7_ThirtyTwoL, Teardown)
EUNIT_TEST(
    "AuthenticateL - RealPacket",
    "SRTPAuthentication_HMAC_SHA1",
    "AuthenticateL with Real Packet",
    "FUNCTIONALITY",
    SetupL, UT_AuthenticateL_Test8_ThirtyTwoL, Teardown)
    
EUNIT_TEST(
    "AuthenticateL - KeyChanged",
    "SRTPAuthentication_HMAC_SHA1",
    "AuthenticateL",
    "FUNCTIONALITY",
    SetupL, UT_AuthenticateL_Test_KeyChangedL, Teardown)
    
EUNIT_END_TEST_TABLE

//  END OF FILE
