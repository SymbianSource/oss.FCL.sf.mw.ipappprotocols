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
#include "UT_SRTPAuthentication_NULL.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "srtpauthentication_null.h"

// RFC 2202 test 1 160 bit key
_LIT8(KRFC2202_Test1_Key_160bits,     "0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B");
// RFC 2202 test 1 16 bit data
_LIT8(KRFC2202_Test1_Data_16bits,     "Hi There");
// RFC 2202 test 1 80 bit digest/tag
_LIT8(KRFC2202_Test1_Tag_80bits,      "B617318655057264E28B");


// CONSTRUCTION
UT_SRTPAuthentication_NULL* UT_SRTPAuthentication_NULL::NewL()
    {
    UT_SRTPAuthentication_NULL* self = UT_SRTPAuthentication_NULL::NewLC(); 
    CleanupStack::Pop();

    return self;
    }

UT_SRTPAuthentication_NULL* UT_SRTPAuthentication_NULL::NewLC()
    {
    UT_SRTPAuthentication_NULL* self = new( ELeave ) UT_SRTPAuthentication_NULL();
    CleanupStack::PushL( self );

	self->ConstructL(); 

    return self;
    }

// Destructor (virtual by CBase)
UT_SRTPAuthentication_NULL::~UT_SRTPAuthentication_NULL()
    {
    }

// Default constructor
UT_SRTPAuthentication_NULL::UT_SRTPAuthentication_NULL()
    {
    }

// Second phase construct
void UT_SRTPAuthentication_NULL::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_SRTPAuthentication_NULL::SetupL(  )
    {

    iAuthenticator = CSRTPAuthentication_NULL::NewL();    

    iRFC2202_Test1_Key_160bits = HBufC8::NewL(KRFC2202_Test1_Key_160bits().Length());
    iRFC2202_Test1_Data_16bits = HBufC8::NewL(KRFC2202_Test1_Data_16bits().Length());
    iRFC2202_Test1_Tag_80bits = HBufC8::NewL(KRFC2202_Test1_Tag_80bits().Length());

	*iRFC2202_Test1_Key_160bits = KRFC2202_Test1_Key_160bits;
	*iRFC2202_Test1_Data_16bits = KRFC2202_Test1_Data_16bits;
	*iRFC2202_Test1_Tag_80bits  = KRFC2202_Test1_Tag_80bits;

	Hex(*iRFC2202_Test1_Key_160bits);
//	Hex(*iRFC2202_Test1_Data_16bits);
	Hex(*iRFC2202_Test1_Tag_80bits);
    } 

void UT_SRTPAuthentication_NULL::Teardown(  )
    {
    delete iAuthenticator;
    iAuthenticator = NULL;
	
	delete iRFC2202_Test1_Key_160bits;
	delete iRFC2202_Test1_Data_16bits;
	delete iRFC2202_Test1_Tag_80bits;
	
    iRFC2202_Test1_Key_160bits = NULL;
	iRFC2202_Test1_Data_16bits = NULL;
	iRFC2202_Test1_Tag_80bits = NULL;
    }

void UT_SRTPAuthentication_NULL::UT_AuthenticateL_RFC2202_Test1_EightyL(  )
    {
 	HBufC8* result = iAuthenticator->AuthenticateL(80, *iRFC2202_Test1_Key_160bits, 
 	                                         *iRFC2202_Test1_Data_16bits,
 	                                         KNullDesC8);

 	delete result;        

    EUNIT_ASSERT( ETrue);

    }


void UT_SRTPAuthentication_NULL::Hex(HBufC8& aString)
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
    UT_SRTPAuthentication_NULL,
    "CSRTPAuthentication_NULL",
    "UNIT" )

EUNIT_TEST(
    "AuthenticateL - 1 80 bits",
    "SRTPAuthentication_NULL",
    "AuthenticateL",
    "FUNCTIONALITY",
    SetupL, UT_AuthenticateL_RFC2202_Test1_EightyL, Teardown)


EUNIT_END_TEST_TABLE

//  END OF FILE
