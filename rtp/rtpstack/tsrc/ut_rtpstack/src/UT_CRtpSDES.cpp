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
#include "UT_CRtpSDES.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "rtpsdes.h"

// CONSTRUCTION
UT_CRtpSDES* UT_CRtpSDES::NewL()
    {
    UT_CRtpSDES* self = UT_CRtpSDES::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CRtpSDES* UT_CRtpSDES::NewLC()
    {
    UT_CRtpSDES* self = new( ELeave ) UT_CRtpSDES();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CRtpSDES::~UT_CRtpSDES()
    {
    }

// Default constructor
UT_CRtpSDES::UT_CRtpSDES()
    {
    }

// Second phase construct
void UT_CRtpSDES::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_CRtpSDES::SetupL(  )
    {

    }

void UT_CRtpSDES::Teardown(  )
    {
	//delete iRtpSdes;
    }

void UT_CRtpSDES::UT_CRtpSDES_NewLL(  )
    {
	TInt result( KErrNone );
    TInt err( KErrNone );
    CRtpSDES* temp;
    
    
    // "Test description: Construct a CRtpSDES object without parameters" ) );
    // "CRtpSDES::NewL()") );
    
    TRAP( err, temp = CRtpSDES::NewL() )
    if ( err != KErrNone )
        {
        __ASSERT_ALWAYS( err != KErrNoMemory, User::Leave( KErrNoMemory ) );
        // "Could not create CRtpSDES object" ) );
        // "CRtpSDES::NewL - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
    CleanupStack::PushL( temp );

    for ( TInt k = 0; k <= ERTCP_SDES_PRIV; k++ )
        {
        if ( temp->iSDESBuf[k] == NULL ||
             temp->iSDESItems[k] == NULL ||
             temp->iSDESItemsSize[k] != 0 )
            {
            result = KErrGeneral;
            }
        }
    if ( temp->iSndSSRCArray == NULL )
        {
        // "Wrong initial values" ) );
        // "CRtpSDES::NewL - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
        
    CleanupStack::PopAndDestroy( temp );    

    // "CRtpSDES::NewL - passed" ) );
    
    }

void UT_CRtpSDES::UT_CRtpSDES_NewL_OneL(  )
    {
    TInt result( KErrNone );
    TBuf8<16> cName;
    TBuf8<16> userName;
    TBuf8<16> email;
    TBuf8<16> phoneNumber;
    TBuf8<16> location;
    TBuf8<16> swToolName;
    TBuf8<16> noticeStatus;
    TBuf8<16> priv;
    
    TBuf8<16> tempCName;
    TBuf8<16> tempUserName;
    TBuf8<16> tempEmail;
    TBuf8<16> tempPhoneNumber;
    TBuf8<16> tempLocation;
    TBuf8<16> tempSwToolName;
    TBuf8<16> tempNoticeStatus;
    TBuf8<16> tempPriv;

    // "" ) );
    // "Test description: Construct a CRtpSDES object with an SDES parameter" ) );
    // "CRtpSDES::NewL( TRtpSdesParams )") );
    
    // Create our own SDES
    cName.Format( _L8( "foo_name" ) );
    userName.Format( _L8( "foo_uname" ) );
    email.Format( _L8( "foo_email" ) );
    phoneNumber.Format( _L8( "123456" ) );
    location.Format( _L8( "foo_loc" ) );
    swToolName.Format( _L8( "foo_tool" ) );
    noticeStatus.Format( _L8( "foo_status" ) );
    priv.Format( _L8( "foo_priv" ) );
    
    iSdesParams.iCName.Set( cName );
    iSdesParams.iUserName.Set( userName );
    iSdesParams.iEmail.Set( email );
    iSdesParams.iPhoneNumber.Set( phoneNumber );
    iSdesParams.iLocation.Set( location );
    iSdesParams.iSwToolName.Set( swToolName );
    iSdesParams.iNoticeStatus.Set( noticeStatus );
    iSdesParams.iPrivate.Set( priv );

    // Now construct the CRtpSDES object
    TRAPD( err, iRtpSdes = CRtpSDES::NewL( iSdesParams ) )
    if ( err != KErrNone )
        {
        __ASSERT_ALWAYS( err != KErrNoMemory, User::Leave( KErrNoMemory ) );
        // "Could not create CRtpSDES object" ) );
        // "CRtpSDES::NewL - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }

    // See if the initial values are OK
    if ( iRtpSdes->iSndSSRCArray == NULL )
        {
        // "iSndSSRCArray not NULL in CRtpSDES" ) );
        // "CRtpSDES::NewL - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }

    if ( iRtpSdes->iSDESItemsSize[ERTCP_SDES_CNAME] != cName.Length() ||
         iRtpSdes->iSDESItemsSize[ERTCP_SDES_NAME] != userName.Length() ||
         iRtpSdes->iSDESItemsSize[ERTCP_SDES_EMAIL] != email.Length() ||
         iRtpSdes->iSDESItemsSize[ERTCP_SDES_PHONE] != phoneNumber.Length() ||
         iRtpSdes->iSDESItemsSize[ERTCP_SDES_LOC] != location.Length() ||
         iRtpSdes->iSDESItemsSize[ERTCP_SDES_TOOL] != swToolName.Length() ||
         iRtpSdes->iSDESItemsSize[ERTCP_SDES_NOTE] != noticeStatus.Length() ||
         iRtpSdes->iSDESItemsSize[ERTCP_SDES_PRIV] != priv.Length() )
        {
        // "Bad iSDESItemsSize in CRtpSDES" ) );
        // "CRtpSDES::NewL - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }

    // Also check the SDES values
    tempCName.Format( _L8( iRtpSdes->iSDESItems[ERTCP_SDES_CNAME] ) );
    tempUserName.Format( _L8( iRtpSdes->iSDESItems[ERTCP_SDES_NAME] ) );
    tempEmail.Format( _L8( iRtpSdes->iSDESItems[ERTCP_SDES_EMAIL] ) );
    tempPhoneNumber.Format( _L8( iRtpSdes->iSDESItems[ERTCP_SDES_PHONE] ) );
    tempLocation.Format( _L8( iRtpSdes->iSDESItems[ERTCP_SDES_LOC] ) );
    tempSwToolName.Format( _L8( iRtpSdes->iSDESItems[ERTCP_SDES_TOOL] ) );
    tempNoticeStatus.Format( _L8( iRtpSdes->iSDESItems[ERTCP_SDES_NOTE] ) );
    tempPriv.Format( _L8( iRtpSdes->iSDESItems[ERTCP_SDES_PRIV] ) );

    if ( tempCName != cName ||
         tempUserName != userName ||
         tempEmail != email ||
         tempPhoneNumber != phoneNumber ||
         tempLocation != location ||
         tempSwToolName != swToolName ||
         tempNoticeStatus != noticeStatus ||
         tempPriv != priv )
        {
        // "SDES was not correctly copied" ) );
        // "CRtpSDES::NewL - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
	delete iRtpSdes;
    }

void UT_CRtpSDES::UT_CRtpSDES_GetSDESL(  )
    {
    TInt result( KErrNone );
    TInt err( KErrNone );
    CRtpSDES* temp;
    TRtpSdesParams tempSdesParams;
    
    TBuf8<16> cName;
    TBuf8<16> userName;
    TBuf8<16> email;
    TBuf8<16> phoneNumber;
    TBuf8<16> location;
    TBuf8<16> swToolName;
    TBuf8<16> noticeStatus;
    TBuf8<16> priv;

    TBuf8<16> tempCName;
    TBuf8<16> tempUserName;
    TBuf8<16> tempEmail;
    TBuf8<16> tempPhoneNumber;
    TBuf8<16> tempLocation;
    TBuf8<16> tempSwToolName;
    TBuf8<16> tempNoticeStatus;
    TBuf8<16> tempPriv;

    // "" ) );
    // "Test description: Create variables for GetSDES, run it" ) );
    // "                  and verify that the variables match." ) );
    // "CRtpSDES::GetSDES( TRtpSdesParams )") );

    // Create our own SDES
    cName.Format( _L8( "poo_name" ) );
    userName.Format( _L8( "poo_uname" ) );
    email.Format( _L8( "poo_email" ) );
    phoneNumber.Format( _L8( "123456" ) );
    location.Format( _L8( "poo_loc" ) );
    swToolName.Format( _L8( "poo_tool" ) );
    noticeStatus.Format( _L8( "poo_status" ) );
    priv.Format( _L8( "poo_priv" ) );
    
    iSdesParams.iCName.Set( cName );
    iSdesParams.iUserName.Set( userName );
    iSdesParams.iEmail.Set( email );
    iSdesParams.iPhoneNumber.Set( phoneNumber );
    iSdesParams.iLocation.Set( location );
    iSdesParams.iSwToolName.Set( swToolName );
    iSdesParams.iNoticeStatus.Set( noticeStatus );
    iSdesParams.iPrivate.Set( priv );

    // Create the SDES with iSdesParams
    TRAP( err, temp = CRtpSDES::NewL( iSdesParams ) )
    if ( err != KErrNone )
        {
        __ASSERT_ALWAYS( err != KErrNoMemory, User::Leave( KErrNoMemory ) );
        
        // "Could not create CRtpSDES object" ) );
        // "CRtpSDES::GetSDES - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
    CleanupStack::PushL( temp );

    temp->GetSDES( tempSdesParams );
    
    if ( tempSdesParams.iCName != cName ||
         tempSdesParams.iUserName != userName ||
         tempSdesParams.iEmail != email ||
         tempSdesParams.iPhoneNumber != phoneNumber ||
         tempSdesParams.iLocation != location ||
         tempSdesParams.iSwToolName != swToolName ||
         tempSdesParams.iNoticeStatus != noticeStatus ||
         tempSdesParams.iPrivate != priv )
        {
        // "SDES was not correctly copied" ) );
        // "CRtpSDES::GetSDES - FAILED" ) );
        EUNIT_ASSERT( EFalse );
        }
    
    CleanupStack::PopAndDestroy( temp );    

    }

void UT_CRtpSDES::UT_CRtpSDES_SetSDESL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpSDES::UT_CRtpSDES_AddSsrcToSdesLL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_CRtpSDES::UT_CRtpSDES_GetSdesSsrcL(  )
    {
       TInt result( KErrNone );
    CRtpSDES* rtpSdes( NULL );
    TInt err( KErrNone );

    // "" ) );
    // "Test description: Get SSRC that does not exist" ) );
    // "CRtpSDES::GetSdesSsrc( TRtpSSRC aSSRC )") );
    // "CRtpSDES::AddSsrcToSdesL( const TRtpSSRC aSSRC )") );

    TRAP( err, rtpSdes = CRtpSDES::NewL() )
    if ( err != KErrNone )
        {
        delete rtpSdes;
        __ASSERT_ALWAYS( err != KErrNoMemory, User::Leave( KErrNoMemory ) );
        // "CRtpSDES: ERROR, could not create CRtpSDES object" ) );
        EUNIT_ASSERT( EFalse );
        }
    CleanupStack::PushL( rtpSdes );
    TRtpSSRC ssrc( 0 );
    TBool gotSSRC( rtpSdes->GetSdesSsrc( ssrc ) ); 
    if ( gotSSRC )
        {
        // "ERROR, should not have found SSRC") );
        }

    ssrc = 1;
    rtpSdes->AddSsrcToSdesL( ssrc );
    if ( rtpSdes->iSndSSRCArray->Count() != 1 )
        {
        result = KErrGeneral;
        // "ERROR, incorrect number of SSRC:s in array (1)") );
        }

    ssrc = 1;
    rtpSdes->AddSsrcToSdesL( ssrc );
    if ( rtpSdes->iSndSSRCArray->Count() != 1 )
        {
        result = KErrGeneral;
        // "ERROR, incorrect number of SSRC:s in array (2)") );
        }

    ssrc = 2;
    rtpSdes->AddSsrcToSdesL( ssrc );
    if ( rtpSdes->iSndSSRCArray->Count() != 2 )
        {
        result = KErrGeneral;
        // "ERROR, incorrect number of SSRC:s in array (3)") );
        }

    gotSSRC = rtpSdes->GetSdesSsrc( 3 ); 
    if ( gotSSRC )
        {
        result = KErrGeneral;
        // "ERROR, should not have found SSRC 3") );
        }    

    gotSSRC = rtpSdes->GetSdesSsrc( 1 ); 
    if ( !gotSSRC )
        {
        result = KErrGeneral;
        // "ERROR, should have found SSRC 1") );
        }    

    if ( rtpSdes->iSndSSRCArray->Count() != 2 )
        {
        result = KErrGeneral;
        // "ERROR, incorrect number of SSRC:s in array (4)") );
        }

    if ( result == KErrNone )
        {
        // "CRtpSDES::GetSdesSsrc - passed" ) );
        // "CRtpSDES::AddSsrcToSdesL - passed" ) );
        }
    else
        {
        // "CRtpSDES::GetSdesSsrc - FAILED" ) );
        // "CRtpSDES::AddSsrcToSdesL - FAILED" ) );
        }
	
    CleanupStack::PopAndDestroy( rtpSdes );

    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CRtpSDES,
    "UT_CRtpSDES",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "CRtpSDES",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSDES_NewLL, Teardown)

EUNIT_TEST(
    "NewL - test ",
    "CRtpSDES",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSDES_NewL_OneL, Teardown)

EUNIT_TEST(
    "GetSDES - test ",
    "CRtpSDES",
    "GetSDES",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSDES_GetSDESL, Teardown)

EUNIT_TEST(
    "SetSDES - test ",
    "CRtpSDES",
    "SetSDES",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSDES_SetSDESL, Teardown)

EUNIT_TEST(
    "AddSsrcToSdesL - test ",
    "CRtpSDES",
    "AddSsrcToSdesL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSDES_AddSsrcToSdesLL, Teardown)

EUNIT_TEST(
    "GetSdesSsrc - test ",
    "CRtpSDES",
    "GetSdesSsrc",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSDES_GetSdesSsrcL, Teardown)


EUNIT_END_TEST_TABLE

//  END OF FILE
