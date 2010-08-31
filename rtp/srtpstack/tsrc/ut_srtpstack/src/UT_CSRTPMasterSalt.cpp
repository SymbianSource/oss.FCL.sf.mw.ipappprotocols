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
#include "UT_CSRTPMasterSalt.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "srtpmastersalt.h"
#include "srtputils.h"
#include "srtpdef.h"

_LIT8(KRFCTestSalt112bits,              "0EC675AD498AFEEBB6960B3AABE6");
_LIT8(KRFCTestSalt112bits2,              "0E0E0E0E0E0E0E0E0E0E0E0E");


// CONSTRUCTION
UT_CSRTPMasterSalt* UT_CSRTPMasterSalt::NewL()
    {
    UT_CSRTPMasterSalt* self = UT_CSRTPMasterSalt::NewLC(); 
    CleanupStack::Pop();

    return self;
    }

UT_CSRTPMasterSalt* UT_CSRTPMasterSalt::NewLC()
    {
    UT_CSRTPMasterSalt* self = new( ELeave ) UT_CSRTPMasterSalt();
    CleanupStack::PushL( self );

	self->ConstructL(); 

    return self;
    }

// Destructor (virtual by CBase)
UT_CSRTPMasterSalt::~UT_CSRTPMasterSalt()
    {
    }

// Default constructor
UT_CSRTPMasterSalt::UT_CSRTPMasterSalt() 
    {
    }

// Second phase construct
void UT_CSRTPMasterSalt::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }


//  METHODS

void UT_CSRTPMasterSalt::SetupL(  )
    {
    HBufC8* masterSalt =HBufC8::NewL(KRFCTestSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFCTestSalt112bits;
	
	
	HBufC8* masterSalt2 =HBufC8::NewL(KRFCTestSalt112bits2().Length());
    CleanupStack::PushL( masterSalt2);
	*masterSalt2 = KRFCTestSalt112bits2;
	
    iMasterSalt = CSRTPMasterSalt::NewL( *masterSalt,KSRTPDefSessionSaltingKeyLength );
    iMasterSalt2 = CSRTPMasterSalt::NewL( *masterSalt,100 );
    iMasterSalt3 = CSRTPMasterSalt::NewL( *masterSalt2, KSRTPDefSessionSaltingKeyLength);
    iMasterSalt4 = CSRTPMasterSalt::NewL( *masterSalt);
    CleanupStack::PopAndDestroy( masterSalt2 );
    CleanupStack::PopAndDestroy( masterSalt );
    
    } 

void UT_CSRTPMasterSalt::Teardown(  )
    {
	delete iMasterSalt;
	delete iMasterSalt2;
	delete iMasterSalt3;
	delete iMasterSalt4;
	
    iMasterSalt = NULL;
	iMasterSalt2 = NULL;
	iMasterSalt3 = NULL;
	iMasterSalt4 = NULL;
	
	iRFCTestSalt112bits.Zero();
    iRFCTestSalt112bits_dehexed.Zero();      
    }

void UT_CSRTPMasterSalt::Hex(HBufC8& aString)
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
void UT_CSRTPMasterSalt::UT_CSRTPMasterSalt_TSRTPMasterSaltL(  )
    {
    iSaltKey.Append(0x0E);
    iSaltKey.Append(0xC6);
    iSaltKey.Append(0x75);
    iSaltKey.Append(0xAD);
    iSaltKey.Append(0x49);
    iSaltKey.Append(0x8A);
    iSaltKey.Append(0xFE);
    iSaltKey.Append(0xEB);
    iSaltKey.Append(0xB6);
    iSaltKey.Append(0x96);
    iSaltKey.Append(0x0B);
    iSaltKey.Append(0x3A);
    iSaltKey.Append(0xAB);
    iSaltKey.Append(0xE6); 
    
    const TBuf8<32> salt = iMasterSalt->MasterSalt();      
    EUNIT_ASSERT( salt.Compare(iSaltKey) == 0);      
    
    EUNIT_ASSERT( iMasterSalt->SaltLength() == KSRTPDefaultMasterSaltLength );   
    iSaltKey.Zero(); 
    }

void UT_CSRTPMasterSalt::UT_CSRTPMasterSalt_MasterSaltL(  )
    {
//     initialize salting key (from RFC)
    iRFCTestSalt112bits.Copy(KRFCTestSalt112bits);
    TSRTPUtils::DeHex(iRFCTestSalt112bits, iRFCTestSalt112bits_dehexed); 
    
    const TBuf8<32> salt = iMasterSalt->MasterSalt();

    EUNIT_ASSERT( salt.Compare(iRFCTestSalt112bits_dehexed) == 0);      
      
//    EUNIT_ASSERT( salt.Compare(iSaltKey) == 0);      
    }

void UT_CSRTPMasterSalt::UT_CSRTPMasterSalt_SaltLengthL(  )
    {        
    EUNIT_ASSERT( iMasterSalt->SaltLength() == KSRTPDefaultMasterSaltLength );
    }

void UT_CSRTPMasterSalt::UT_CSRTPMasterSalt_ValidL(  )
    {            
    //this one should pass
    EUNIT_ASSERT( iMasterSalt->Valid());
    //invalid session salt length
    EUNIT_ASSERT( !iMasterSalt2->Valid());
    //invalid master salt length
    EUNIT_ASSERT( !iMasterSalt3->Valid());
    //test constructing with default values
    EUNIT_ASSERT( iMasterSalt4->Valid());
    }

void UT_CSRTPMasterSalt::TestEqualL(  )
	{
	HBufC8* masterSalt =HBufC8::NewL(KRFCTestSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFCTestSalt112bits;
	Hex(*masterSalt);
	const CSRTPMasterSalt& tempSalt=iMasterSalt2->operator=(*iMasterSalt2);

	EUNIT_ASSERT(tempSalt.MasterSalt() == *masterSalt);
	EUNIT_ASSERT(tempSalt.SaltLength()==100);
	CleanupStack::PopAndDestroy(masterSalt);
	}

void UT_CSRTPMasterSalt::UT_CSRTPMasterSalt_ConstructLL(  )
	{
	HBufC8* masterSalt =HBufC8::NewL(KRFCTestSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFCTestSalt112bits;
	Hex(*masterSalt);
	
	const CSRTPMasterSalt& masterSaltlength14 = iMasterSalt->operator=(*iMasterSalt);
 	
    EUNIT_ASSERT( masterSaltlength14.MasterSalt()== *masterSalt);  
    CleanupStack::PopAndDestroy(masterSalt);
 	 
 	
	}
//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE( 
    UT_CSRTPMasterSalt,
    "CSRTPMasterSalt",
    "UNIT" )

EUNIT_TEST(
    "TSRTPMasterSalt - test ",
    "CSRTPMasterSalt",
    "CSRTPMasterSalt",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPMasterSalt_TSRTPMasterSaltL, Teardown)

EUNIT_TEST(
    "MasterSalt - test ",
    "CSRTPMasterSalt",
    "MasterSalt",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPMasterSalt_MasterSaltL, Teardown)

EUNIT_TEST(
    "SaltLength - test ",
    "CSRTPMasterSalt",
    "SaltLength",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPMasterSalt_SaltLengthL, Teardown)

EUNIT_TEST(
    "Valid - test ",
    "CSRTPMasterSalt",
    "Valid",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPMasterSalt_ValidL, Teardown)

EUNIT_TEST(
    "Test = ",
    "CSRTPMasterSalt",
    "Test Operator equal",
    "FUNCTIONALITY",
    SetupL, TestEqualL, Teardown)

EUNIT_TEST(
    "Test Construct",
    "CSRTPMasterSalt",
    "Test contruct keys with different length",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPMasterSalt_ConstructLL, Teardown)
    
EUNIT_END_TEST_TABLE

//  END OF FILE
