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
#include "UT_CSRTPMasterKey.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "srtpmasterkey.h"
#include "srtputils.h"
#include "srtpdef.h"

_LIT8(KTestKey128bits,              "11223344556677889900AABBCCDDEEFF");
_LIT8(KTestMKI128Bits,      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
// CONSTRUCTION
UT_CSRTPMasterKey* UT_CSRTPMasterKey::NewL()
    {
    UT_CSRTPMasterKey* self = UT_CSRTPMasterKey::NewLC(); 
    CleanupStack::Pop();

    return self;
    }

UT_CSRTPMasterKey* UT_CSRTPMasterKey::NewLC()
    {
    UT_CSRTPMasterKey* self = new( ELeave ) UT_CSRTPMasterKey();
    CleanupStack::PushL( self );

	self->ConstructL(); 

    return self;
    }

// Destructor (virtual by CBase)
UT_CSRTPMasterKey::~UT_CSRTPMasterKey()
    {
    }
void UT_CSRTPMasterKey::Hex(HBufC8& aString)
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
// Default constructor
UT_CSRTPMasterKey::UT_CSRTPMasterKey() 
    {
    }

// Second phase construct
void UT_CSRTPMasterKey::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

void UT_CSRTPMasterKey::SetupL(  )
    {
    
    
    HBufC8* masterKey = HBufC8::NewL(KTestKey128bits().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = KTestKey128bits;

    
	
	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	
	iMasterKey = CSRTPMasterKey::NewL( *masterKey, *mki,
					KSRTPDefSessionEncryptionKeyLength, 
                    KSRTPDefSessionAuthenticationKeyLength/*, 
                    KSRTPDefaultKeyDerivationRate*/) ;
    

	iMasterKey2= CSRTPMasterKey::NewL( *masterKey, *mki,
						100, 
						KSRTPDefSessionAuthenticationKeyLength/*, 
                    	KSRTPDefaultKeyDerivationRate */),
   iMasterKey3= CSRTPMasterKey::NewL( *masterKey, *mki, 
   					KSRTPDefSessionEncryptionKeyLength,
   					100/*, 
                    KSRTPDefaultKeyDerivationRate*/ ),
   iMasterKey4= CSRTPMasterKey::NewL( *masterKey, *mki,
   					KSRTPDefSessionEncryptionKeyLength, 
                    KSRTPDefSessionAuthenticationKeyLength
                    /*22*/);
   iMasterKey5= CSRTPMasterKey::NewL( *masterKey, *mki);
    CleanupStack::PopAndDestroy( mki );
    
    CleanupStack::PopAndDestroy( masterKey ); 
 
        
    
    //    initialize key 
    iTestKey128bits.Copy(KTestKey128bits);
    TSRTPUtils::DeHex(iTestKey128bits, iTestKey128bits_dehexed); 
    } 

void UT_CSRTPMasterKey::Teardown(  )
    {
	delete iMasterKey;
	delete iMasterKey2;
	delete iMasterKey3;
	delete iMasterKey4;
	delete iMasterKey5;
	
    iMasterKey = NULL;
	iMasterKey2 = NULL;
	iMasterKey3 = NULL;
	iMasterKey4 = NULL;
	iMasterKey5 = NULL;
	
	iTestKey128bits.Zero();
    iTestKey128bits_dehexed.Zero();    
    }

void UT_CSRTPMasterKey::UT_CSRTPMasterKey_SRTPMasterKeyL(  )
    {
    
    const TBuf8<32> masterKey = iMasterKey->MasterKey();
    EUNIT_ASSERT( masterKey.Compare(iTestKey128bits_dehexed) == 0);          
    
    }
    
void UT_CSRTPMasterKey::UT_CSRTPMasterKey_ConstructLL(  )
    {
    
   	HBufC8* masterKey = HBufC8::NewL(KTestKey128bits().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = KTestKey128bits;
	Hex(*masterKey);
    
	
	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	
	CSRTPMasterKey* masterKeylength16 = CSRTPMasterKey::NewL( *masterKey, *mki,
					KSRTPDefSessionEncryptionKeyLength, 
                    KSRTPDefSessionAuthenticationKeyLength) ;
    CleanupStack::PopAndDestroy( mki );
    
    CleanupStack::PopAndDestroy( masterKey ); 
 	
    EUNIT_ASSERT( masterKeylength16->MasterKey()==iTestKey128bits_dehexed);   
 	delete masterKeylength16;       
    
    }    

void UT_CSRTPMasterKey::UT_CSRTPMasterKey_MasterKeyL(  )
    {   
    const TBuf8<32> masterKey = iMasterKey->MasterKey();
    EUNIT_ASSERT( masterKey.Compare(iTestKey128bits_dehexed) == 0);      
    }

void UT_CSRTPMasterKey::UT_CSRTPMasterKey_MKIL(  )
    {
    TBuf8<32>  MKI2;    
    for (TInt i = 0; i<16 ; i++)        
        {
    	MKI2.Append(0xAA);
        }
    EUNIT_ASSERT( MKI2.Compare(iMasterKey->MKI()) == 0);

    }

void UT_CSRTPMasterKey::UT_CSRTPMasterKey_EncrKeyLengthL(  )
    {
    EUNIT_ASSERT( iMasterKey->EncrKeyLength() == 128 );    
    }

void UT_CSRTPMasterKey::UT_CSRTPMasterKey_AuthKeyLengthL(  )
    {
    EUNIT_ASSERT( iMasterKey->AuthKeyLength() == 160 );    
    }


void UT_CSRTPMasterKey::UT_CSRTPMasterKey_ValidL(  )
    {
    //this one should pass
    EUNIT_ASSERT( iMasterKey->Valid());    
    //invalid bit length of the encryption key
    EUNIT_ASSERT(! iMasterKey2->Valid());    
    //invalid bit length of the authentication key
    EUNIT_ASSERT(! iMasterKey3->Valid());        
    //invalid key derivation rate
    EUNIT_ASSERT(iMasterKey4->Valid());    
    // test constructing with default values
    EUNIT_ASSERT(iMasterKey5->Valid());    
    }

void UT_CSRTPMasterKey::TestEqualL(  )
	{
	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	const CSRTPMasterKey& tempMasterKey=iMasterKey2->operator=(*iMasterKey2);
	EUNIT_ASSERT(tempMasterKey.MKI()==*mki);
	
	CleanupStack::PopAndDestroy(mki);
	}
//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE( 
    UT_CSRTPMasterKey,
    "CSRTPMasterKey",
    "UNIT" )

EUNIT_TEST(
    "CSRTPMasterKey - test ",
    "CSRTPMasterKey",
    "CSRTPMasterKey",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPMasterKey_SRTPMasterKeyL, Teardown)

EUNIT_TEST(
    "MasterKey - test ",
    "CSRTPMasterKey",
    "MasterKey",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPMasterKey_MasterKeyL, Teardown)

EUNIT_TEST(
    "MKI - test ",
    "CSRTPMasterKey",
    "MKI",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPMasterKey_MKIL, Teardown)

EUNIT_TEST(
    "EncrKeyLength - test ",
    "CSRTPMasterKey",
    "EncrKeyLength",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPMasterKey_EncrKeyLengthL, Teardown)

EUNIT_TEST(
    "AuthKeyLength - test ",
    "CSRTPMasterKey",
    "AuthKeyLength",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPMasterKey_AuthKeyLengthL, Teardown)



EUNIT_TEST(
    "Valid - test ",
    "CSRTPMasterKey",
    "Valid",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPMasterKey_ValidL, Teardown)
EUNIT_TEST(
    "Test = ",
    "CSRTPMasterKey",
    "Test Operator equal",
    "FUNCTIONALITY",
    SetupL, TestEqualL, Teardown)
EUNIT_TEST(
    "Test Construct",
    "CSRTPMasterKey",
    "Test contruct keys with different length",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPMasterKey_ConstructLL, Teardown)
 

EUNIT_END_TEST_TABLE

//  END OF FILE
