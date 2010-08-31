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
#include "UT_CSRTPCryptoContext.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "srtpcryptocontext.h"
#include "srtpcryptohandlersrtp.h"
#include "srtpcryptohandlersrtcp.h"
#include "srtpmastersalt.h"
#include "srtpmasterkey.h"
#include "srtpstreamin.h"
#include "srtpstreamout.h"
#include "srtpsession.h"

// 128 bit master key, test 1
_LIT8(K128bitMasterKey1,    "112233445566778899E6AABBCCDDEEFF");
// 128 bit master key, test 2
_LIT8(K128bitMasterKey2,       "FFEEDDCCBBAA11223344556677889900");
// 128 bit master key, test 3
_LIT8(K128bitMasterKey3,       "33333333333333333333333333333333");
// 112 bit master salt, test 1
_LIT8(K112bitMasterSalt1,      "0EC675AD498AFEEBB6960B3AABE6");
// 112 bit master salt, test 2
_LIT8(K112bitMasterSalt2,      "112233445566778899E6AABBCCDD");
// 112 bit master salt, test 3
_LIT8(K112bitMasterSalt3,      "3333333333333333333333333333");
// 128 bit MKI, test 1
_LIT8(K128bitMKITest1,         "11111111111111111111111111111111");
// 128 bit MKI, test 2
_LIT8(K128bitMKITest2,         "22222222222222222222222222222222");
// 128 bit MKI, test 3
_LIT8(K128bitMKITest3,         "33333333333333333333333333333333");

#define RTP_EUNIT_ASSERT_SPECIFIC_LEAVE( func, val ) \
{\
TRAPD( rtpUnitTestRetVal, func );\
if ( val != KErrNoMemory && rtpUnitTestRetVal == KErrNoMemory ){\
	User::Leave( KErrNoMemory );\
	}\
else{\
	EUNIT_ASSERT_EQUALS( rtpUnitTestRetVal, val );\
	}\
}

// CONSTRUCTION
UT_CSRTPCryptoContext* UT_CSRTPCryptoContext::NewL()
    {
    UT_CSRTPCryptoContext* self = UT_CSRTPCryptoContext::NewLC(); 
    CleanupStack::Pop();

    return self;
    }

UT_CSRTPCryptoContext* UT_CSRTPCryptoContext::NewLC()
    {
    UT_CSRTPCryptoContext* self = new( ELeave ) UT_CSRTPCryptoContext();
    CleanupStack::PushL( self );

	self->ConstructL(); 

    return self;
    }

// Destructor (virtual by CBase)
UT_CSRTPCryptoContext::~UT_CSRTPCryptoContext()
    {
    }

// Default constructor
UT_CSRTPCryptoContext::UT_CSRTPCryptoContext() 
    {
    }

// Second phase construct
void UT_CSRTPCryptoContext::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_CSRTPCryptoContext::SetupL(  )
    {

    HBufC8* masterKey1 = HBufC8::NewL(K128bitMasterKey1().Length());
    CleanupStack::PushL( masterKey1 );
    *masterKey1 = K128bitMasterKey1;
	
	HBufC8* masterKey2 = HBufC8::NewL(K128bitMasterKey2().Length());
    CleanupStack::PushL( masterKey2 );
    *masterKey2 = K128bitMasterKey2;
	
	
    
    HBufC8* masterSalt1 =HBufC8::NewL(K112bitMasterSalt1().Length());
    CleanupStack::PushL( masterSalt1);
	*masterSalt1 = K112bitMasterSalt1;

	
	
    HBufC8* masterSalt2 =HBufC8::NewL(K112bitMasterSalt2().Length());
    CleanupStack::PushL( masterSalt2);
	*masterSalt2 = K112bitMasterSalt2;

	

	HBufC8* mki =HBufC8::NewL(K128bitMKITest1().Length());
    CleanupStack::PushL( mki);
	*mki = K128bitMKITest1;
	Hex(*mki);

	HBufC8* mki2 =HBufC8::NewL(K128bitMKITest2().Length());
    CleanupStack::PushL( mki2);
	*mki2 = K128bitMKITest2;
	Hex(*mki2);
	
	iMasterKey = CSRTPMasterKey::NewL( *masterKey1, *mki,
									KSRTPDefSessionEncryptionKeyLength, 
        							KSRTPDefSessionAuthenticationKeyLength );
    iMasterSalt = CSRTPMasterSalt::NewL( *masterSalt1,KSRTPDefSessionSaltingKeyLength );
    
    iMasterKey2 = CSRTPMasterKey::NewL( *masterKey2, *mki2,
								        KSRTPDefSessionEncryptionKeyLength,
								        KSRTPDefSessionAuthenticationKeyLength  );
    iMasterSalt2 = CSRTPMasterSalt::NewL( *masterSalt2, KSRTPDefSessionSaltingKeyLength );
    
 
    
    
    
    
	TSrtpCryptoParams params;
    iContext = CSRTPCryptoContext::NewL(iMasterKey, iMasterSalt,params   );
								        
 	//iContext2 is valid context costructed with automatic values
 	
 	params.iSrtcpAuthTagLen=32;
   	iContext2= CSRTPCryptoContext::NewL(iMasterKey2,iMasterSalt2, params);
   	
 

	CleanupStack::PopAndDestroy( mki2 );            
    CleanupStack::PopAndDestroy( mki );
    CleanupStack::PopAndDestroy( masterSalt2 );
    CleanupStack::PopAndDestroy( masterSalt1 );
    CleanupStack::PopAndDestroy( masterKey2 ); 
    CleanupStack::PopAndDestroy( masterKey1 ); 
    iDestination.Input(_L("127.0.0.1"));
    
 
  
	i128bitMasterKey1  = HBufC8::NewL(K128bitMasterKey1().Length());
	i128bitMasterKey2  = HBufC8::NewL(K128bitMasterKey2().Length());
	i128bitMasterKey3  = HBufC8::NewL(K128bitMasterKey3().Length());
	i112bitMasterSalt1 = HBufC8::NewL(K112bitMasterSalt1().Length());
	i112bitMasterSalt2 = HBufC8::NewL(K112bitMasterSalt2().Length());
	i112bitMasterSalt3 = HBufC8::NewL(K112bitMasterSalt3().Length());
	//i128bitMKI1        = HBufC8::NewL(K128bitMKITest1().Length());
	//i128bitMKI2        = HBufC8::NewL(K128bitMKITest2().Length());
	//i128bitMKI3        = HBufC8::NewL(K128bitMKITest3().Length());
	
	*i128bitMasterKey1  = K128bitMasterKey1;
	*i128bitMasterKey2  = K128bitMasterKey2;
	*i128bitMasterKey3  = K128bitMasterKey3;
	*i112bitMasterSalt1 = K112bitMasterSalt1;
	*i112bitMasterSalt2 = K112bitMasterSalt2;
	*i112bitMasterSalt3 = K112bitMasterSalt3;
	//*i128bitMKI1        = K128bitMKITest1;
	//*i128bitMKI2        = K128bitMKITest2;
	//*i128bitMKI3        = K128bitMKITest3;
	
    Hex(*i128bitMasterKey1);
    Hex(*i128bitMasterKey2);
    Hex(*i128bitMasterKey3);
    Hex(*i112bitMasterSalt1);
    Hex(*i112bitMasterSalt2);
    Hex(*i112bitMasterSalt3);
	//Hex(*i128bitMKI1);
	//Hex(*i128bitMKI2);  
	//Hex(*i128bitMKI3);  
	
	// we need to create these in order to test adding CSRTPCryptoHandler
	// object to CSRTPCryptoContext lists
    iSRTPSession = CSRTPSession::NewL( iDestination, iContext, *this ); 
    iStreamIn = CSRTPStreamIn::NewL(*iSRTPSession, (TUint)1);
    iStreamOut = CSRTPStreamOut::NewL(*iSRTPSession, (TUint)2);   
    } 
    
        
void UT_CSRTPCryptoContext::Teardown(  )
    { 
    delete i128bitMasterKey1;
    delete i128bitMasterKey2;
    delete i128bitMasterKey3;
    delete i112bitMasterSalt1;
    delete i112bitMasterSalt2;
    delete i112bitMasterSalt3;

	delete iStreamIn;
    delete iStreamOut; 
    delete iSRTPSession; 
	
    delete iContext2;
	
	iStreamIn = NULL;
    iStreamOut = NULL; 
    iSRTPSession = NULL; 
	
    iContext2 = NULL;
    			
    }

void UT_CSRTPCryptoContext::UT_CSRTPCryptoContext_Test1L(  )
    {  
    HBufC8* masterKey3 = HBufC8::NewL(K128bitMasterKey3().Length());
    CleanupStack::PushL( masterKey3 );
    *masterKey3 = K128bitMasterKey3;
	
	HBufC8* masterSalt3 =HBufC8::NewL(K112bitMasterSalt3().Length());
    CleanupStack::PushL( masterSalt3);
	*masterSalt3 = K112bitMasterSalt3;

		
	HBufC8* mki3 =HBufC8::NewL(K128bitMKITest3().Length());
    CleanupStack::PushL( mki3);
	*mki3 = K128bitMKITest3;
	Hex(*mki3);
 

    TSrtpCryptoParams params;
    //test constructL  
    iMasterKey3 = CSRTPMasterKey::NewL( *masterKey3, *mki3,
    							    KSRTPDefSessionEncryptionKeyLength, 
    							    KSRTPDefSessionAuthenticationKeyLength );
    iMasterSalt3 = CSRTPMasterSalt::NewL( *masterSalt3, 1100 );
   
    CleanupStack::PopAndDestroy( mki3 );
	
    CleanupStack::PopAndDestroy( masterSalt3 );
    CleanupStack::PopAndDestroy( masterKey3 ); 
    
  
    params.iSrtcpAuthTagLen=80;
   	RTP_EUNIT_ASSERT_SPECIFIC_LEAVE( iContext3= CSRTPCryptoContext::NewL(iMasterKey3,iMasterSalt3, params ),
   		 KErrArgument );
   	
  
   	delete iContext3;	 
   	
    }

void UT_CSRTPCryptoContext::UT_CSRTPCryptoContext_Test2L(  )
	{
	HBufC8* masterKey3 = HBufC8::NewL(K128bitMasterKey3().Length());
    CleanupStack::PushL( masterKey3 );
    *masterKey3 = K128bitMasterKey3;
	
	HBufC8* masterSalt3 =HBufC8::NewL(K112bitMasterSalt3().Length());
    CleanupStack::PushL( masterSalt3);
	*masterSalt3 = K112bitMasterSalt3;

		
	HBufC8* mki3 =HBufC8::NewL(K128bitMKITest3().Length());
    CleanupStack::PushL( mki3);
	*mki3 = K128bitMKITest3;
	Hex(*mki3);
 

    TSrtpCryptoParams params;
   	iMasterKey3 = CSRTPMasterKey::NewL( *masterKey3, *mki3,
    							    KSRTPDefSessionEncryptionKeyLength, 
    							    KSRTPDefSessionAuthenticationKeyLength );
    iMasterSalt3 = CSRTPMasterSalt::NewL( *masterSalt3, 1100 );
   
    CleanupStack::PopAndDestroy( mki3 );
	CleanupStack::PopAndDestroy( masterSalt3 );
    CleanupStack::PopAndDestroy( masterKey3 ); 
    
    params.iSrtcpAuthTagLen=80;
    RTP_EUNIT_ASSERT_SPECIFIC_LEAVE( iContext3= CSRTPCryptoContext::NewL(iMasterKey3,iMasterSalt3, params ),
   		 KErrArgument );
   	
    delete iContext3;	 
   
	}
	
void UT_CSRTPCryptoContext::UT_CSRTPCryptoContext_Test3L(  )
	{
    HBufC8* masterKey = HBufC8::NewL(K128bitMasterKey1().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = K128bitMasterKey1;
    
    HBufC8* masterSalt =HBufC8::NewL(K112bitMasterSalt1().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = K112bitMasterSalt1;
	HBufC8* mki =HBufC8::NewL(K128bitMKITest1().Length());
    CleanupStack::PushL( mki);
	*mki = K128bitMKITest1;
	Hex(*mki);
	
	iMasterKey4 = CSRTPMasterKey::NewL( *masterKey, *mki,
									KSRTPDefSessionEncryptionKeyLength, 
        							KSRTPDefSessionAuthenticationKeyLength );
    iMasterSalt4 = CSRTPMasterSalt::NewL( *masterSalt,KSRTPDefSessionSaltingKeyLength );
   
    CleanupStack::PopAndDestroy( mki );
	
    CleanupStack::PopAndDestroy( masterSalt );
    CleanupStack::PopAndDestroy( masterKey );  
    
    
   
     
	 //invalid authalg
	 TSrtpCryptoParams params;
     params.iSrtpAuthAlg=TSRTPAuthAlg(10);   
    RTP_EUNIT_ASSERT_SPECIFIC_LEAVE(iContext4=CSRTPCryptoContext::NewL(iMasterKey4,iMasterSalt4, params),
     KErrArgument );
     
    
  
    }

void UT_CSRTPCryptoContext::UT_CSRTPCryptoContext_Test4L(  )
	{
	HBufC8* masterKey = HBufC8::NewL(K128bitMasterKey1().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = K128bitMasterKey1;
    
    HBufC8* masterSalt =HBufC8::NewL(K112bitMasterSalt1().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = K112bitMasterSalt1;
	HBufC8* mki =HBufC8::NewL(K128bitMKITest1().Length());
    CleanupStack::PushL( mki);
	*mki = K128bitMKITest1;
	Hex(*mki);
	
	iMasterKey5 = CSRTPMasterKey::NewL( *masterKey, *mki,
									KSRTPDefSessionEncryptionKeyLength, 
        							KSRTPDefSessionAuthenticationKeyLength);
    iMasterSalt5 = CSRTPMasterSalt::NewL( *masterSalt,KSRTPDefSessionSaltingKeyLength ); 
    
   	CleanupStack::PopAndDestroy( mki );
	CleanupStack::PopAndDestroy( masterSalt );
    CleanupStack::PopAndDestroy( masterKey );   
	
	
    
    
	TSrtpCryptoParams params;
	 //invalid auth tag length in context5
	  params.iSrtpAuthTagLen=20;
	 RTP_EUNIT_ASSERT_SPECIFIC_LEAVE( iContext5=CSRTPCryptoContext::NewL(iMasterKey5,iMasterSalt5,params ),
     KErrArgument );
     
 
	 }

void UT_CSRTPCryptoContext::UT_CSRTPCryptoContext_Test5L(  )
	{
	HBufC8* masterKey = HBufC8::NewL(K128bitMasterKey1().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = K128bitMasterKey1;
    
    HBufC8* masterSalt =HBufC8::NewL(K112bitMasterSalt1().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = K112bitMasterSalt1;
	HBufC8* mki =HBufC8::NewL(K128bitMKITest1().Length());
    CleanupStack::PushL( mki);
	*mki = K128bitMKITest1;
	Hex(*mki);
	iMasterKey6 = CSRTPMasterKey::NewL( *masterKey, *mki,
									KSRTPDefSessionEncryptionKeyLength, 
        							KSRTPDefSessionAuthenticationKeyLength );
    iMasterSalt6 = CSRTPMasterSalt::NewL( *masterSalt,KSRTPDefSessionSaltingKeyLength );
  
	TSrtpCryptoParams params;
	CleanupStack::PopAndDestroy( mki );
	CleanupStack::PopAndDestroy( masterSalt );
    CleanupStack::PopAndDestroy( masterKey );   
	
	
    
	//invalid prefix length in context6
	params.iPrefixLen=10;
	RTP_EUNIT_ASSERT_SPECIFIC_LEAVE( iContext6=CSRTPCryptoContext::NewL(iMasterKey6,iMasterSalt6, params )  ,
     KErrArgument ); 
     
	
     
	}
void UT_CSRTPCryptoContext::UT_MasterKey_Test1L(  )
    {    
    
    EUNIT_ASSERT( iContext->MasterKey().MasterKey().Compare(*i128bitMasterKey1) == 0 );
    }

void UT_CSRTPCryptoContext::UT_MasterSalt_Test1L(  )
    {    
    
    EUNIT_ASSERT( iContext->MasterSalt().MasterSalt().Compare(*i112bitMasterSalt1) == 0 );
    }

void UT_CSRTPCryptoContext::UT_AddCryptoChangeObserver_Test1L(  )
    {
    //iContext->AddCryptoChangeObserver(iCryptoHandlerSRTCP);
    //iContext.AddCryptoChangeObserver(iCryptoHandlerSRTCP);
    iStreamIn->CreateCryptoHandlerSRTPL();	
    iStreamIn->CreateCryptoHandlerSRTCPL();
    

    EUNIT_ASSERT( iContext->iHandlerList.IsEmpty() == EFalse );

    }


void UT_CSRTPCryptoContext::UT_SetMasterKey_Test1L(  )
    {
    
	HBufC8* masterKey3 = HBufC8::NewL(K128bitMasterKey3().Length());
    CleanupStack::PushL( masterKey3 );
    *masterKey3 = K128bitMasterKey3;
		
	
	HBufC8* mki3 =HBufC8::NewL(K128bitMKITest3().Length());
    CleanupStack::PushL( mki3);
	*mki3 = K128bitMKITest3;
	Hex(*mki3);
	    
    CSRTPMasterKey *masterKey = CSRTPMasterKey::NewL( *masterKey3, *mki3,
    							    KSRTPDefSessionEncryptionKeyLength, 80 );

   iContext->SetMasterKey(masterKey);
    EUNIT_ASSERT( iContext->MasterKey().MasterKey().Compare(*i128bitMasterKey3) == 0 );
    CleanupStack::PopAndDestroy(mki3);
    CleanupStack::PopAndDestroy(masterKey3);
    }

void UT_CSRTPCryptoContext::UT_SetMasterSalt_Test1L(  )
    {
    	
	HBufC8* masterSalt3 =HBufC8::NewL(K112bitMasterSalt3().Length());
    CleanupStack::PushL( masterSalt3);
	*masterSalt3 = K112bitMasterSalt3;
    CSRTPMasterSalt* masterSalt = CSRTPMasterSalt::NewL( *masterSalt3, 1100 );
    iContext->SetMasterSalt(masterSalt);
    
    EUNIT_ASSERT( iContext->MasterSalt().MasterSalt().Compare(*i112bitMasterSalt3) == 0 );
     
     CleanupStack::PopAndDestroy(masterSalt3);
    }

void UT_CSRTPCryptoContext::UT_ValidL(  )
    {
    //this one should pass
    EUNIT_ASSERT( iContext->Valid());    
    // test constructing with default values
    EUNIT_ASSERT(iContext2->Valid());    
    //invalid encoding algorithm
    }


 void UT_CSRTPCryptoContext::TestOperatorEqualL()
    {
  
    const CSRTPCryptoContext& tempContext2= iContext2->operator=(*iContext2);
    
    
   	HBufC8* masterKey2 = HBufC8::NewL(K128bitMasterKey2().Length());
    CleanupStack::PushL( masterKey2 );
    *masterKey2 = K128bitMasterKey2;
    Hex(*masterKey2);
    EUNIT_ASSERT(tempContext2.MasterKey().MasterKey()==*masterKey2);
    
    CleanupStack::PopAndDestroy(masterKey2);
    
    }
    
void UT_CSRTPCryptoContext::TestPrefixLenL()
    {
    EUNIT_ASSERT(iContext->CryptoParams().iPrefixLen== 0);
    }    
    
void UT_CSRTPCryptoContext::TestReplayProtectionL()
    {
    EUNIT_ASSERT(iContext->CryptoParams().iSrtpReplayProtection  == ETrue);
    }    
    
void UT_CSRTPCryptoContext::TestIsEqualL()
	{
	EUNIT_ASSERT(iContext->IsEqual(*iContext2)==EFalse);
	
	HBufC8* masterKey3 = HBufC8::NewL(K128bitMasterKey1().Length());
    CleanupStack::PushL( masterKey3 );
    *masterKey3 = K128bitMasterKey1;
	
	HBufC8* masterSalt3 =HBufC8::NewL(K112bitMasterSalt1().Length());
    CleanupStack::PushL( masterSalt3);
	*masterSalt3 = K112bitMasterSalt1;

		
	HBufC8* mki3 =HBufC8::NewL(K128bitMKITest1().Length());
    CleanupStack::PushL( mki3);
	*mki3 = K128bitMKITest1;
	Hex(*mki3);
 

    TSrtpCryptoParams params;
    //test constructL  
    iMasterKey3 = CSRTPMasterKey::NewL( *masterKey3, *mki3 );
    iMasterSalt3 = CSRTPMasterSalt::NewL( *masterSalt3);
    
    CleanupStack::PopAndDestroy( mki3 );
	
    CleanupStack::PopAndDestroy( masterSalt3 );
    CleanupStack::PopAndDestroy( masterKey3 ); 
    
 
    
   	iContext3= CSRTPCryptoContext::NewL(iMasterKey3,iMasterSalt3, params );
 
 
     
   	params.iSrtpAuthTagLen=112;
   	iContext3->UpdateCryptoParams(params );
   	EUNIT_ASSERT(iContext->IsEqual(*iContext3)==EFalse);
   
   	delete iContext3;
	iContext3 = NULL;
   	//test only param is different
   	HBufC8* masterKey1 = HBufC8::NewL(K128bitMasterKey1().Length());
    CleanupStack::PushL( masterKey1 );
    *masterKey1 = K128bitMasterKey1;
	
	
    HBufC8* masterSalt1 =HBufC8::NewL(K112bitMasterSalt1().Length());
    CleanupStack::PushL( masterSalt1);
	*masterSalt1 = K112bitMasterSalt1;

	HBufC8* mki =HBufC8::NewL(K128bitMKITest1().Length());
    CleanupStack::PushL( mki);
	*mki = K128bitMKITest1;
	Hex(*mki);

	iMasterKey = CSRTPMasterKey::NewL( *masterKey1, *mki,
									KSRTPDefSessionEncryptionKeyLength, 
        							KSRTPDefSessionAuthenticationKeyLength );
    iMasterSalt = CSRTPMasterSalt::NewL( *masterSalt1,KSRTPDefSessionSaltingKeyLength );
  
    CleanupStack::PopAndDestroy( mki );
    CleanupStack::PopAndDestroy( masterSalt1 );
    CleanupStack::PopAndDestroy( masterKey1 );
    TSrtpCryptoParams params2;
    params2.iSrtpAuthTagLen=32;
  
 
    CSRTPCryptoContext* context = CSRTPCryptoContext::NewL(iMasterKey, iMasterSalt,params2   );
    EUNIT_ASSERT(!iContext->IsEqual(*context));
    
 
    params2.iSrtpAuthTagLen=80;
    params2.iMasterKeysLifeTime = 0;
    context->UpdateCryptoParams(params2 );
    
    EUNIT_ASSERT(iContext->IsEqual(*context));
    delete context;
								        
	}
void UT_CSRTPCryptoContext::TestSetRCCm3SyncL()
	{
	TBool sync= ETrue;
	iContext->SetRCCm3Sync(sync);
	EUNIT_ASSERT(iContext->CryptoParams().iIsRCCm3Sync);
	}
	
void UT_CSRTPCryptoContext::TestIsValidL()
	{
	TSrtpCryptoParams params;
	//TEst cases 1 about Encryption method
	params.iSrtpEncAlg=TSRTPEncAlg(10);
	iContext->UpdateCryptoParams(params );
	EUNIT_ASSERT(!iContext->Valid());
	//Test case 2 : alg method
	params.iSrtpEncAlg=EEncAES_CM;
	params.iSrtpAuthAlg=TSRTPAuthAlg(10);
	iContext->UpdateCryptoParams(params);
	EUNIT_ASSERT(!iContext->Valid());
	EUNIT_ASSERT(!iContext->Valid());
	
	//Test case 3.:
	params.iSrtpAuthAlg=EAuthHMAC_SHA1;
	params.iSrtcpAuthAlg=EAuthHMAC_SHA1;
	params.iSrtcpAuthTagLen=0;
	iContext->UpdateCryptoParams(params );
	EUNIT_ASSERT(!iContext->Valid());
	//Test case 4:
	params.iSrtcpAuthTagLen=32;
	params.iSrtpAuthAlg=EAuthRCCm1;
	params.iSrtpAuthTagLen=32;
	iContext->UpdateCryptoParams(params );
	EUNIT_ASSERT(!iContext->Valid());
		
	//Test case 5:
	params.iSrtpAuthAlg=EAuthRCCm3;
	params.iSrtpAuthTagLen=0;
	iContext->UpdateCryptoParams(params );
	EUNIT_ASSERT(!iContext->Valid());	
	
	//Test case 6:
	params.iSrtpAuthAlg=EAuthNull;
	params.iSrtpAuthTagLen=10;
	iContext->UpdateCryptoParams(params );
	EUNIT_ASSERT(!iContext->Valid());	
	
	//Test case 7
	params.iSrtpAuthTagLen=0;
	params.iROCTransRate=0;
	iContext->UpdateCryptoParams(params );
	EUNIT_ASSERT(!iContext->Valid());	
	params.iROCTransRate=1;
	//make all the value to valid
	iContext->UpdateCryptoParams(params );
	EUNIT_ASSERT(iContext->Valid());	
	//test is master key valid 
	HBufC8* masterKey3 = HBufC8::NewL(K128bitMasterKey3().Length());
    CleanupStack::PushL( masterKey3 );
    *masterKey3 = K128bitMasterKey3;
	
	HBufC8* masterSalt3 =HBufC8::NewL(K112bitMasterSalt3().Length());
    CleanupStack::PushL( masterSalt3);
	*masterSalt3 = K112bitMasterSalt3;

		
	HBufC8* mki3 =HBufC8::NewL(K128bitMKITest3().Length());
    CleanupStack::PushL( mki3);
	*mki3 = K128bitMKITest3;
	Hex(*mki3);
 
    iMasterKey3 = CSRTPMasterKey::NewL( *masterKey3, *mki3,
    								KSRTPDefSessionAuthenticationKeyLength, 
    							    KSRTPDefSessionEncryptionKeyLength
    							    );
    iMasterSalt3 = CSRTPMasterSalt::NewL( *masterSalt3, 1100 );
   
    CleanupStack::PopAndDestroy( mki3 );
	
    CleanupStack::PopAndDestroy( masterSalt3 );
    CleanupStack::PopAndDestroy( masterKey3 ); 
  
   	RTP_EUNIT_ASSERT_SPECIFIC_LEAVE( iContext3= CSRTPCryptoContext::NewL(iMasterKey3,iMasterSalt3, params ),
   		 KErrArgument );
   
	}
	
void UT_CSRTPCryptoContext::SRTPMasterKeyStaleEvent(const CSRTPStream& /*aStream*/)
    {
    
    }
void UT_CSRTPCryptoContext::SRTPMasterKeyStaleEvent(const CSRTPSession& /*aSession*/)
    {
    }


void UT_CSRTPCryptoContext::Hex(HBufC8& aString)
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
    UT_CSRTPCryptoContext,
    "CSRTPCryptoContext",
    "UNIT" )

EUNIT_TEST(
    "CSRTPCryptoContext - 1 ",
    "CSRTPCryptoContext1",
    "CSRTPCryptoContext1",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPCryptoContext_Test1L, Teardown)

EUNIT_TEST(
    "CSRTPCryptoContext - 2 ",
    "CSRTPCryptoContext2",
    "CSRTPCryptoContext2",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPCryptoContext_Test2L, Teardown)
EUNIT_TEST(
    "CSRTPCryptoContext - 3 ",
    "CSRTPCryptoContext3",
    "CSRTPCryptoContext3",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPCryptoContext_Test3L, Teardown)

EUNIT_TEST(
    "CSRTPCryptoContext - 4 ",
    "CSRTPCryptoContext4",
    "CSRTPCryptoContext4",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPCryptoContext_Test4L, Teardown)
    
EUNIT_TEST(
    "CSRTPCryptoContext - 5 ",
    "CSRTPCryptoContext5",
    "CSRTPCryptoContext5",
    "FUNCTIONALITY",
    SetupL, UT_CSRTPCryptoContext_Test5L, Teardown)
EUNIT_TEST(
    "MasterKey - 1 ",
    "CSRTPCryptoContext",
    "MasterKey",
    "FUNCTIONALITY",
    SetupL, UT_MasterKey_Test1L, Teardown)

EUNIT_TEST(
    "MasterSalt - 1 ",
    "CSRTPCryptoContext",
    "MasterSalt",
    "FUNCTIONALITY",
    SetupL, UT_MasterSalt_Test1L, Teardown)

EUNIT_TEST(
    "AddCryptoChangeObserver - 1 ",
    "CSRTPCryptoContext",
    "AddCryptoChangeObserver",
    "FUNCTIONALITY",
    SetupL, UT_AddCryptoChangeObserver_Test1L, Teardown)

EUNIT_TEST(
    "SetMasterKey - 1 ",
    "CSRTPCryptoContext",
    "SetMasterKey",
    "FUNCTIONALITY",
    SetupL, UT_SetMasterKey_Test1L, Teardown)

EUNIT_TEST(
    "SetMasterSalt - 1 ",
    "CSRTPCryptoContext",
    "SetMasterSalt",
    "FUNCTIONALITY",
    SetupL, UT_SetMasterSalt_Test1L, Teardown)

EUNIT_TEST(
    "Valid - test ",
    "CSRTPCryptoContext",
    "Valid",
    "FUNCTIONALITY",
    SetupL, UT_ValidL, Teardown)

EUNIT_TEST(
    "Test = ",
    "CSRTPCryptoContext",
    "TestOperatorEqualL",
    "FUNCTIONALITY",
    SetupL, TestOperatorEqualL, Teardown)
    
 EUNIT_TEST(
    "PrefixLenL ",
    "CSRTPCryptoContext",
    "TestPrefixLenL",
    "FUNCTIONALITY",
    SetupL, TestPrefixLenL, Teardown)
    
EUNIT_TEST(
    "IsEqual ",
    "CSRTPCryptoContext",
    "IsEqual",
    "FUNCTIONALITY",
    SetupL, TestIsEqualL, Teardown)   

EUNIT_TEST(
    "SetRCCm3Sync",
    "CSRTPCryptoContext",
    "SetRCCm3Sync",
    "FUNCTIONALITY",
    SetupL, TestSetRCCm3SyncL, Teardown)   
 EUNIT_TEST(
    "TestIsValid",
    "CSRTPCryptoContext",
    "TestIsValid",
    "FUNCTIONALITY",
    SetupL, TestIsValidL, Teardown)  
    
EUNIT_END_TEST_TABLE

//  END OF FILE
