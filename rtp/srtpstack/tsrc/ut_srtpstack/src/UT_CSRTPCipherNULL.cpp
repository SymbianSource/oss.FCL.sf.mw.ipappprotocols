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
#include "UT_CSRTPCipherNULL.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>

//  INTERNAL INCLUDES
#include "srtpcipher_null.h"
#include "srtpdef.h"

    // 128 bit plain text
	_LIT8(KRFC3686_TestPlainT128bits,  "53696E676C6520626C6F636B206D7367");
    // 128 bit key, test 1
	_LIT8(KRFC3686_TestKey128bits,     "AE6852F8121067CC4BF7A5765577F39E");
    // 128 bit IV, test 1
	_LIT8(KRFC3686_TestIV128bits,      "00000030000000000000000000000001");

// CONSTRUCTION
UT_CSRTPCipherNULL* UT_CSRTPCipherNULL::NewL()
    {
    UT_CSRTPCipherNULL* self = UT_CSRTPCipherNULL::NewLC(); 
    CleanupStack::Pop();

    return self;
    }

UT_CSRTPCipherNULL* UT_CSRTPCipherNULL::NewLC()
    {
    UT_CSRTPCipherNULL* self = new( ELeave ) UT_CSRTPCipherNULL();
    CleanupStack::PushL( self );

	self->ConstructL(); 

    return self;
    }

// Destructor (virtual by CBase)
UT_CSRTPCipherNULL::~UT_CSRTPCipherNULL()
    {
    }

// Default constructor
UT_CSRTPCipherNULL::UT_CSRTPCipherNULL()
    {
    }

// Second phase construct
void UT_CSRTPCipherNULL::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_CSRTPCipherNULL::SetupL(  )
    {
    iEncryptor = CSRTPCipherNULL::NewL();    

	iRFC3686_TestPlainT128bits = HBufC8::NewL(KRFC3686_TestPlainT128bits().Length());
	iRFC3686_TestKey128bits = HBufC8::NewL(KRFC3686_TestKey128bits().Length());
	iRFC3686_TestIV128bits = HBufC8::NewL(KRFC3686_TestIV128bits().Length());	

	*iRFC3686_TestPlainT128bits = KRFC3686_TestPlainT128bits;
	*iRFC3686_TestKey128bits = KRFC3686_TestKey128bits;
	*iRFC3686_TestIV128bits = KRFC3686_TestIV128bits;
	
	Hex(*iRFC3686_TestPlainT128bits);
	Hex(*iRFC3686_TestKey128bits);
	Hex(*iRFC3686_TestIV128bits);
    } 

void UT_CSRTPCipherNULL::Teardown(  )
    {
	delete iRFC3686_TestPlainT128bits;
	delete iRFC3686_TestKey128bits;
	delete iRFC3686_TestIV128bits;

	iRFC3686_TestPlainT128bits = NULL;
	iRFC3686_TestKey128bits = NULL;
	iRFC3686_TestIV128bits = NULL;

    delete iEncryptor;
    iEncryptor = NULL;
    }

void UT_CSRTPCipherNULL::UT_TransformL(  )
    {
 	HBufC8* result = iEncryptor->TransformL(*iRFC3686_TestKey128bits, 
 	                                         *iRFC3686_TestIV128bits,
 	                                         *iRFC3686_TestPlainT128bits);

 	CleanupStack::PushL(result); 	

    EUNIT_ASSERT( result->Compare(*iRFC3686_TestPlainT128bits) == 0);

 	CleanupStack::Pop(result); 	
 	delete result;    
    }

void UT_CSRTPCipherNULL::Hex(HBufC8& aString)
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
    UT_CSRTPCipherNULL,
    "CSRTPCipherNULL",
    "UNIT" )

EUNIT_TEST(
    "EncryptL encrypt data",
    "CSRTPCipherNULL",
    "TransformL",
    "FUNCTIONALITY",
    SetupL, UT_TransformL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE
