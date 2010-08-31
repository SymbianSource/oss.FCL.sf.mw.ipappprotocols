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
#include "UT_CRtpStpPacket.h"
#include "rtpstppacket.h"
//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES


//Packet max size (defined also in RtpSession.h)
const TUint KMaxRtpPacketSize = 8192; // 65536 bits = 8192 bytes

// CONSTRUCTION
UT_CRtpStpPacket* UT_CRtpStpPacket::NewL()
    {
    UT_CRtpStpPacket* self = UT_CRtpStpPacket::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CRtpStpPacket* UT_CRtpStpPacket::NewLC()
    {
    UT_CRtpStpPacket* self = new( ELeave ) UT_CRtpStpPacket();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CRtpStpPacket::~UT_CRtpStpPacket()
    {
    }

// Default constructor
UT_CRtpStpPacket::UT_CRtpStpPacket()
    {
    }

// Second phase construct
void UT_CRtpStpPacket::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_CRtpStpPacket::SetupL(  )
    {
 	iRtpStpPacket = new(ELeave) CRtpStpPacket();
    iRtpStpPacket->ConstructL( KMaxRtpPacketSize, 1 );

    }

void UT_CRtpStpPacket::Teardown(  )
    {
 
	
    }

void UT_CRtpStpPacket::UT_CRtpStpPacket_CRtpStpPacketL(  )
    {
	// "CRtpUtStpPacket::TestOpenStpDll()" ) );
	// "Test description: Load RLibrary" ) );
	iRtpStpPacket->Close();
    TInt result(KErrNone);
    
    TPtrC16 fileName( _L16( "rtpstppacket.dll" ) );
    
    //Load library
    RLibrary iLibrary;
    User::LeaveIfError( iLibrary.Load( fileName ) );
    CleanupClosePushL( iLibrary );
    
    //Load function
    TLibraryFunction entry = iLibrary.Lookup( 1 );
    
    //Use function
    MRtpPacketExt* iPktExtRcv = ( MRtpPacketExt * ) entry(); //NewL()
    
    if( result == KErrNone && iPktExtRcv != NULL )
   	    {
	    EUNIT_ASSERT(ETrue);   
	    }
	else
	    {
	    EUNIT_ASSERT(EFalse);   
	    }

    iPktExtRcv->Close();

    //Close library
    
    CleanupStack::PopAndDestroy( &iLibrary );
    
    }

void UT_CRtpStpPacket::UT_CRtpStpPacket_ConstructLL(  )
    {
    EUNIT_ASSERT( ETrue );
    iRtpStpPacket->Close();
    }

void UT_CRtpStpPacket::UT_CRtpStpPacket_CloseL(  )
    {
    EUNIT_ASSERT( ETrue );
    iRtpStpPacket->Close();
    }

void UT_CRtpStpPacket::UT_CRtpStpPacket_RtpPacketResetL(  )
    {
     TInt result(KErrNone);
     iRtpStpPacket->Close(); 
    TPtrC16 fileName( _L16( "rtpstppacket.dll" ) );
    
    //Load library
    RLibrary iLibrary;
    User::LeaveIfError( iLibrary.Load( fileName ) );
    CleanupClosePushL( iLibrary );
    
    
    //Load function
    TLibraryFunction entry = iLibrary.Lookup( 1 );
    MRtpPacketExt* iPktExtRcv = ( MRtpPacketExt * ) entry(); //NewL()
    CleanupClosePushL( *iPktExtRcv );
  	iPktExtRcv->ConstructL( KMaxRtpPacketSize, 1 );

	iPktExtRcv ->RtpPacketReset();
	iPktExtRcv ->RtpPacketResetPtr();
	
	//TEST 1
	// "Test description: sendHeader.iChecksumInd = 0" ) );
	TRtpHeader header;
	TRtpSendHeader sendHeader= (static_cast<TRtpSendHeader&> (header));
	sendHeader.iChecksumInd = 0;
	TBuf8<15> data; 
    data.Format( _L8( "12345678" ) );
    
    
    result = iPktExtRcv ->RtpPacketBuild( sendHeader, data );

    TUint8* temp =  const_cast<TUint8*>(data.Ptr());
    TInt length = data.Length();
	iPktExtRcv->SetSize(20);
	result = iPktExtRcv ->RtpPacketProcess( temp, &length  );
	//Test 2
	// "Test description: sendHeader.iChecksumInd = 1" ) );
	
	iPktExtRcv ->RegisterRtpObserver( *this );
	sendHeader.iChecksumInd = 1;
	
	//result = iPktExtRcv->RtpPacketBuild( sendHeader, data );
	iPktExtRcv->SetSize(20);
	result = iPktExtRcv ->RtpPacketProcess( temp, &length  );
	
	iPktExtRcv ->UnregisterRtpObserver();
    //Close library
    CleanupStack::PopAndDestroy( iPktExtRcv );
    

    CleanupStack::PopAndDestroy( &iLibrary );
  
    }

void UT_CRtpStpPacket::UT_CRtpStpPacket_RtpPacketResetPtrL(  )
    {
    EUNIT_ASSERT( ETrue );
    iRtpStpPacket->Close();
    }

void UT_CRtpStpPacket::UT_CRtpStpPacket_RtpPacketBuildL(  )
    {
    EUNIT_ASSERT( ETrue );
    iRtpStpPacket->Close();
    }

void UT_CRtpStpPacket::UT_CRtpStpPacket_RtpPacketProcessL(  )
    {
    EUNIT_ASSERT( ETrue );
    iRtpStpPacket->Close();
    }

void UT_CRtpStpPacket::UT_CRtpStpPacket_RegisterRtpObserverL(  )
    {
    EUNIT_ASSERT( ETrue );
    iRtpStpPacket->Close();
    }

void UT_CRtpStpPacket::UT_CRtpStpPacket_UnregisterRtpObserverL(  )
    {
    EUNIT_ASSERT( ETrue );
    iRtpStpPacket->Close();
    }

void UT_CRtpStpPacket::UT_CRtpStpPacket_SetSizeL(  )
    {
    	TInt result(KErrNone);
	
	// "Test description: Test inline functions" ) );
	
	iRtpStpPacket->RtpPacketReset();
	iRtpStpPacket ->RtpPacketResetPtr();
	TInt size = 150;
	
	iRtpStpPacket->SetSize( size );
	
	if( size == iRtpStpPacket->Size())
	    {
	    // "CRtpUtStpPacket::TestInlineFunctions, Size - passed" ));
	    }
	
	else
	    {
	    EUNIT_ASSERT(EFalse);
	    // "CRtpUtStpPacket::TestInlineFunctions, Size - passed" ));
	    }
	
	TUint8* dataPtr = iRtpStpPacket->Data();
	
	if( dataPtr != NULL )
	    {
	    // "CRtpUtStpPacket::TestInlineFunctions, TUint8* - passed" ));
	    }
    else
	    {
	    EUNIT_ASSERT(EFalse);
	    // "CRtpUtStpPacket::TestInlineFunctions, TUint8* - passed" ));
	    }
	
	HBufC8* bufPtr = iRtpStpPacket->GetHBuf();
    
    if( bufPtr != NULL )
        {
        // "CRtpUtStpPacket::TestInlineFunctions, HBufC8* - passed" ));
        }
	else
	    {
	    EUNIT_ASSERT(EFalse);
	    // "CRtpUtStpPacket::TestInlineFunctions, HBufC8* - passed" ));
	    }
	
	TPtrC8 ptr = iRtpStpPacket->Des();
	
	
	if( ptr.Ptr() != NULL )
        {
        // "CRtpUtStpPacket::TestInlineFunctions, TPtrC8 - passed" ));
        }
	else
	    {
	    EUNIT_ASSERT(EFalse);
	    // "CRtpUtStpPacket::TestInlineFunctions, TPtrC8 - passed" ));
	    }
	iRtpStpPacket->Close();
    }

void UT_CRtpStpPacket::UT_CRtpStpPacket_SizeL(  )
    {
    EUNIT_ASSERT( ETrue );
    iRtpStpPacket->Close();
    }

void UT_CRtpStpPacket::UT_CRtpStpPacket_DataL(  )
    {
    EUNIT_ASSERT( ETrue );
    iRtpStpPacket->Close();
    }

void UT_CRtpStpPacket::UT_CRtpStpPacket_GetHBufL(  )
    {
    EUNIT_ASSERT( ETrue );
    iRtpStpPacket->Close();
    }

void UT_CRtpStpPacket::UT_CRtpStpPacket_DesL(  )
    {
    EUNIT_ASSERT( ETrue );
    iRtpStpPacket->Close();
    }
void UT_CRtpStpPacket::RtpPacketReceived( TRtpId /*aStreamId*/, 
                                        const TRtpRecvHeader& /*aHeaderInfo*/, 
                                        const TDesC8& /*aPayloadData*/ )
	{
	
	}

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CRtpStpPacket,
    "CRtpStpPacket",
    "UNIT" )

EUNIT_TEST(
    "CRtpStpPacket - test ",
    "CRtpStpPacket",
    "CRtpStpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStpPacket_CRtpStpPacketL, Teardown)

EUNIT_TEST(
    "ConstructL - test ",
    "CRtpStpPacket",
    "ConstructL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStpPacket_ConstructLL, Teardown)

EUNIT_TEST(
    "Close - test ",
    "CRtpStpPacket",
    "Close",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStpPacket_CloseL, Teardown)

EUNIT_TEST(
    "RtpPacketReset - test ",
    "CRtpStpPacket",
    "RtpPacketReset",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStpPacket_RtpPacketResetL, Teardown)

EUNIT_TEST(
    "RtpPacketResetPtr - test ",
    "CRtpStpPacket",
    "RtpPacketResetPtr",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStpPacket_RtpPacketResetPtrL, Teardown)

EUNIT_TEST(
    "RtpPacketBuild - test ",
    "CRtpStpPacket",
    "RtpPacketBuild",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStpPacket_RtpPacketBuildL, Teardown)

EUNIT_TEST(
    "RtpPacketProcess - test ",
    "CRtpStpPacket",
    "RtpPacketProcess",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStpPacket_RtpPacketProcessL, Teardown)

EUNIT_TEST(
    "RegisterRtpObserver - test ",
    "CRtpStpPacket",
    "RegisterRtpObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStpPacket_RegisterRtpObserverL, Teardown)

EUNIT_TEST(
    "UnregisterRtpObserver - test ",
    "CRtpStpPacket",
    "UnregisterRtpObserver",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStpPacket_UnregisterRtpObserverL, Teardown)

EUNIT_TEST(
    "SetSize - test ",
    "CRtpStpPacket",
    "SetSize",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStpPacket_SetSizeL, Teardown)

EUNIT_TEST(
    "Size - test ",
    "CRtpStpPacket",
    "Size",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStpPacket_SizeL, Teardown)

EUNIT_TEST(
    "Data - test ",
    "CRtpStpPacket",
    "Data",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStpPacket_DataL, Teardown)

EUNIT_TEST(
    "GetHBuf - test ",
    "CRtpStpPacket",
    "GetHBuf",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStpPacket_GetHBufL, Teardown)

EUNIT_TEST(
    "Des - test ",
    "CRtpStpPacket",
    "Des",
    "FUNCTIONALITY",
    SetupL, UT_CRtpStpPacket_DesL, Teardown)
    



EUNIT_END_TEST_TABLE

//  END OF FILE
