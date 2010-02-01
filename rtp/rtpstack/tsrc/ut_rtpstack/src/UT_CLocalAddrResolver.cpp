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
#include "UT_CLocalAddrResolver.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "localaddrresolver.h"
_LIT(KAddr1, "172.21.11.40");
_LIT (KAddr2, "fe80::20b:dbff:fe3c:ca82");

// CONSTRUCTION
UT_CLocalAddrResolver* UT_CLocalAddrResolver::NewL()
    {
    UT_CLocalAddrResolver* self = UT_CLocalAddrResolver::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CLocalAddrResolver* UT_CLocalAddrResolver::NewLC()
    {
    UT_CLocalAddrResolver* self = new( ELeave ) UT_CLocalAddrResolver();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CLocalAddrResolver::~UT_CLocalAddrResolver()
    {
    }

// Default constructor
UT_CLocalAddrResolver::UT_CLocalAddrResolver()
    {
    }

// Second phase construct
void UT_CLocalAddrResolver::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_CLocalAddrResolver::SetupL(  )
    {
    TInt err( KErrCouldNotConnect );
    RSocketServ socketServ;  
    iSocketServ = socketServ;
    // Opens a session to the socket server
    // i.e. creates an IPC communication channel to the socket server
    if ( ( err = iSocketServ.Connect() ) != KErrNone )
    	{
    	//Show EUNIT log
    	EUNIT_PRINT(  _L("CRtpUtCLocalAddrResolver::SetupL()") );
    	EUNIT_PRINT(  _L("RSocketServ - Open Connection Error") );
    	}
	
    //Create a CLocalAddrResolver object
    iLocalAddrResolver = CLocalAddrResolver::NewL(iSocketServ/*, iConnection*/);
    
    EUNIT_ASSERT(err==KErrNone);
    }

void UT_CLocalAddrResolver::Teardown(  )
    {
	delete iLocalAddrResolver;
    iLocalAddrResolver = NULL;
   
    iConnection.Close();
    iSocketServ.Close();
    }

void UT_CLocalAddrResolver::UT_CLocalAddrResolver_NewLL(  )
    {
    EUNIT_ASSERT(ETrue);
	}

void UT_CLocalAddrResolver::UT_CLocalAddrResolver_NewLCL(  )
    {
    EUNIT_ASSERT(ETrue);
    }

void UT_CLocalAddrResolver::UT_CLocalAddrResolver_GetLocalAddrLL(  )
    {
    //Test Case	
	TInetAddr addr1;
	addr1.SetAddress(KInetAddrAny);
	addr1.SetPort(0); 
	
	TInetAddr addr2;
    addr2.SetAddress(KInetAddrAny);
	addr2.SetPort(5000); 
	
	iLocalAddrResolver->GetLocalAddrL(addr1, 12);
    
    TBuf16<16> aBuf, aBuf2;
    
    addr1.Output( aBuf ); 
    addr2.Output( aBuf2 ); 
    
    EUNIT_PRINT( _L( "CLocalAddrResolver::GetLocalAddrL %d"), addr1.Address() );
    EUNIT_PRINT( _L("TInetAddr addr1 %S"), &aBuf );
    EUNIT_PRINT( _L("TInetAddr addr2 %S"), &aBuf2 );

    }
void UT_CLocalAddrResolver::UT_CLocalAddrResolver_SetAddrL(  )
	{
	
    //Test Case	
	TInetAddr addr1;
	addr1.SetAddress(KInetAddrAny);
	addr1.SetPort(5050);

	TInetAddr addr2;
    addr2.SetAddress(KInetAddrAny);
	addr2.SetPort(5000); 
	
	//iLocalAddrResolver->
	iLocalAddrResolver->SetAddr(addr1, addr2);
	iLocalAddrResolver->GetLocalAddrL(addr1, 1);
	iLocalAddrResolver->CheckAndSetAddr( addr1, addr2, 4, 4 );
    //iLocalAddrResolver->iConnection;
	TBuf16<16> aBuf, aBuf2;
	addr1.Output( aBuf ); 
    addr2.Output( aBuf2 ); 
    
    EUNIT_PRINT( _L( "CLocalAddrResolver::SetAddr %d"), addr1.Address() );
    EUNIT_PRINT( _L("TInetAddr addr1 %S"), &aBuf );
    EUNIT_PRINT( _L("TInetAddr addr2 %S"), &aBuf2 );
	
	}
void UT_CLocalAddrResolver::UT_CLocalAddrResolver_CheckAndSetAddrL()
	{
	//Test Case	
	TInetAddr addr1;
	addr1.SetAddress(KInetAddrAny);
	addr1.SetPort(5050);

	TInetAddr addr2;
    addr2.Input(KAddr1);
	addr2.SetPort(5000); 
	



	
	//iLocalAddrResolver->
	iLocalAddrResolver->SetAddr(addr1, addr2);
	iLocalAddrResolver->GetLocalAddrL(addr1, 1);
	iLocalAddrResolver->CheckAndSetAddr( addr1, addr2, 4, 4 );
    //iLocalAddrResolver->iConnection;
	TBuf16<16> aBuf, aBuf2;
	addr1.Output( aBuf ); 
    addr2.Output( aBuf2 ); 
    EUNIT_ASSERT(addr1==addr2);
    
    addr2.ConvertToV4Mapped();
    iLocalAddrResolver->SetAddr(addr1, addr2);
	iLocalAddrResolver->GetLocalAddrL(addr1, 1);
	iLocalAddrResolver->CheckAndSetAddr( addr1, addr2, 4, 4 );
	EUNIT_ASSERT(addr1==addr2);
	
    EUNIT_PRINT( _L( "CLocalAddrResolver::SetAddr %d"), addr1.Address() );
    EUNIT_PRINT( _L("TInetAddr addr1 %S"), &aBuf );
    EUNIT_PRINT( _L("TInetAddr addr2 %S"), &aBuf2 );
	}

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CLocalAddrResolver,
    "TestLocalAddrresolver",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "CLocalAddrResolver",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CLocalAddrResolver_NewLL, Teardown)

EUNIT_TEST(
    "NewLC - test ",
    "CLocalAddrResolver",
    "NewLC",
    "FUNCTIONALITY",
    SetupL, UT_CLocalAddrResolver_NewLCL, Teardown)

EUNIT_TEST(
    "GetLocalAddrL - test ",
    "CLocalAddrResolver",
    "GetLocalAddrL",
    "FUNCTIONALITY",
    SetupL, UT_CLocalAddrResolver_GetLocalAddrLL, Teardown)
EUNIT_TEST(
    "SetAddrL - test ",
    "CLocalAddrResolver",
    "SetAddr",
    "FUNCTIONALITY",
    SetupL, UT_CLocalAddrResolver_SetAddrL, Teardown)
EUNIT_TEST(
    "CheckAndSetAddr - test ",
    "CLocalAddrResolver",
    "CheckAndSetAddr",
    "FUNCTIONALITY",
    SetupL, UT_CLocalAddrResolver_CheckAndSetAddrL, Teardown)    


EUNIT_END_TEST_TABLE

//  END OF FILE
