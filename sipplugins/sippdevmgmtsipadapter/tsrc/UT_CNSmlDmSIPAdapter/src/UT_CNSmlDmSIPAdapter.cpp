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
#include "UT_CNSmlDmSIPAdapter.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/EUnitMacros.h>

//  INTERNAL INCLUDES

#include <utf.h>
#include <sipprofile.h>
#include <sipprofiletypeinfo.h>
#include <SIPManagedProfile.h>
#include <SIPManagedProfileRegistry.h>
#include "nsmldmsipadapter.h"
#include "nsmldmsipadapter.h"
#include "sipprofileregistryobserver.h"
#include <SysUtil.h>
#include <f32file.h>
#include <digia/eunit/EUnitDecorators.h>

#include "UT_StateQuery.h"
#include "UT_Implementation.h"
#include "SmlDmCallback_stub.h"


// TEST literals

_LIT8( KName, "Test settings" ); 
_LIT8( KProfileType, "IETF" ); 
_LIT8( KAddressOfRecord, "eunit@test" ); 
_LIT8( KOBHost, "eunittest.nokia.com" ); 
_LIT8( KOBPort, "80" ); 
_LIT8( KOBTransport, "TCP" ); 
_LIT8( KOBUsername, "eunittest" ); 
_LIT8( KOBPasswd, "eunit" ); 
_LIT8( KOBRealm, "eunit" ); 

_LIT8( KRSHost, "eunittest2.nokia.com" ); 
_LIT8( KRSPort, "60" ); 
_LIT8( KRSTransport, "UDP" ); 
_LIT8( KRSUsername, "eunittest2" ); 
_LIT8( KRSPrivateIdentity, "private2" ); 
_LIT8( KRSPasswd, "eunit2" ); 
_LIT8( KRSRealm, "eunit2" ); 

_LIT8( KDefault, "True" ); 
_LIT8( KLr, "False" ); 
_LIT8( KEnableSecurityNegotiation, "False" ); 
_LIT8( KEnableSignalCompression, "False" ); 
_LIT8( KEnableAutoRegistration, "False" ); 
_LIT8( KProfileLock, "True" );
_LIT8( KSIPSnap, "DestinationNetwork" );

_LIT8( KProfileType2, "ims" ); 
_LIT8( KOBTransport2, "UDP" );
_LIT8( KRSTransport2, "TCP" ); 
_LIT8( KDefault2, "True" ); 
_LIT8( KLr2, "True" ); 
_LIT8( KEnableSecurityNegotiation2, "True" ); 
_LIT8( KEnableSignalCompression2, "True" ); 
_LIT8( KEnableAutoRegistration2, "True" ); 
_LIT8( KProfileLock2, "False" ); 


// give existing AP reference here.
_LIT8( KConRef, "AP/APId001" );


// CONSTRUCTION
UT_CNSmlDmSIPAdapter* UT_CNSmlDmSIPAdapter::NewL( )
    {
    UT_CNSmlDmSIPAdapter* self = UT_CNSmlDmSIPAdapter::NewLC(); 
    CleanupStack::Pop();
    return self;
    }

UT_CNSmlDmSIPAdapter* UT_CNSmlDmSIPAdapter::NewLC()
    {
    UT_CNSmlDmSIPAdapter* self = new( ELeave ) UT_CNSmlDmSIPAdapter();
    CleanupStack::PushL( self ); 

    self->ConstructL(); 

    return self;
    }

// Destructor (virtual by CBase)
UT_CNSmlDmSIPAdapter::~UT_CNSmlDmSIPAdapter()
    {
    }

// Default constructor
UT_CNSmlDmSIPAdapter::UT_CNSmlDmSIPAdapter()
    {
    }

// Second phase construct
void UT_CNSmlDmSIPAdapter::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void UT_CNSmlDmSIPAdapter::SetupL(  )
    {
    TStates* states = new ( ELeave ) TStates;
    Dll::SetTls( static_cast<TAny*>( states ) );
    
    iDmCallback = CSmlDmCallback_stub::NewL();
    iNSmlDmSIPAdapter = CNSmlDmSIPAdapter::NewL( iDmCallback );
    }

void UT_CNSmlDmSIPAdapter::Setup2L(  )
    {
    TStates* states = new ( ELeave ) TStates;
    Dll::SetTls( static_cast<TAny*>( states ) );
    
    iDmCallback = CSmlDmCallback_stub::NewL();
    iNSmlDmSIPAdapter = CNSmlDmSIPAdapter::NewL( iDmCallback );
    
    // Make a profile that's type is IETF
    iNSmlDmSIPAdapter->AddNodeObjectL( _L8("SIP/EUnit"), KNullDesC8, 0 );
    iLuid1 = iDmCallback->iLUID->Des().AllocL();
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/Name"), *iLuid1, KName, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/ProfileType"), *iLuid1, KProfileType, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/AddressOfRecord"), *iLuid1, KAddressOfRecord, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Host"), *iLuid1, KOBHost, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Port"), *iLuid1, KOBPort, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Transport"), *iLuid1, KOBTransport, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Username"), *iLuid1, KOBUsername, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Passwd"), *iLuid1, KOBPasswd, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Realm"), *iLuid1, KOBRealm, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Host"), *iLuid1, KRSHost, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Port"), *iLuid1, KRSPort, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Transport"), *iLuid1, KRSTransport, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Username"), *iLuid1, KRSUsername, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Passwd"), *iLuid1, KRSPasswd, KNullDesC8, 0 );    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Realm"), *iLuid1, KRSRealm, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/Default"), *iLuid1, KDefault, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/LooseRouting"), *iLuid1, KLr, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/EnableSecurityNegotiation"), *iLuid1, KEnableSecurityNegotiation, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/EnableSignalCompression"), *iLuid1, KEnableSignalCompression, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/EnableAutoRegistration"), *iLuid1, KEnableAutoRegistration, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/PrefConRef"), *iLuid1, KConRef, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/ProfileLock"), *iLuid1, KProfileLock, KNullDesC8, 0 );
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/DestinationNetwork"), *iLuid1, KSIPSnap, KNullDesC8, 0 );
    }

void UT_CNSmlDmSIPAdapter::Teardown(  )
    {
    delete iLuid1;
    iLuid1 = NULL;
    
    delete iNSmlDmSIPAdapter;
    iNSmlDmSIPAdapter = NULL;
    
    delete iDmCallback;
    iDmCallback = NULL;
    
    TStates* states = static_cast< TStates* > ( Dll::Tls() );
    states->Clean();
    delete states;
    states = NULL;
    Dll::FreeTls();
    }

void UT_CNSmlDmSIPAdapter::Teardown2(  )
    {
    TBuf16< 8 > status;
    
    // Deleting the SIP profile (from phone) created by the test
    if ( iLuid1 )
        {
        iNSmlDmSIPAdapter->DeleteObjectL( _L8("SIP/EUnit"), *iLuid1, 0 );
        }
    
    delete iLuid1;
    iLuid1 = NULL;
    
    delete iNSmlDmSIPAdapter;
    iNSmlDmSIPAdapter = NULL;
    
    delete iDmCallback;
    iDmCallback = NULL;
    
    TStates* states = static_cast< TStates* > ( Dll::Tls() );
    states->Clean();
    delete states;
    states = NULL;
    Dll::FreeTls();
    }

void UT_CNSmlDmSIPAdapter::UT_CNSmlDmSIPAdapter_DDFVersionLL(  )
    {
    _LIT8( KDDFVersion, "1.0" );
    CBufBase *version = CBufFlat::NewL( 3 );
    CleanupStack::PushL( version );
    iNSmlDmSIPAdapter->DDFVersionL( *version );
    HBufC8* resultHBufC = HBufC8::NewLC( version->Size() );
    resultHBufC->Des().Format( version->Ptr(0) );
    
    EUNIT_ASSERT_EQUALS( KDDFVersion(), resultHBufC->Des() );
    EUNIT_PRINT( resultHBufC->Des() );
    CleanupStack::PopAndDestroy( 2 ); //version, resultHBufC  
     EUNIT_ASSERT( ETrue );
    }

void UT_CNSmlDmSIPAdapter::UT_CNSmlDmSIPAdapter_AddNodeObjectLL(  )
    {
    TBuf16< 8 > status;
    
    iNSmlDmSIPAdapter->AddNodeObjectL( _L8("SIP/EUnit"), KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    iLuid1 = iDmCallback->iLUID->Des().AllocL();
    
    // Test already existing profile
    iNSmlDmSIPAdapter->AddNodeObjectL( _L8("SIP/EUnit"), *iLuid1, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EAlreadyExists == iDmCallback->iStatus );

    }

void UT_CNSmlDmSIPAdapter::UT_CNSmlDmSIPAdapter_UpdateLeafObjectLL(  )
    {
    TBuf16< 8 > status;
    
    iNSmlDmSIPAdapter->AddNodeObjectL( _L8("SIP/EUnit"), KNullDesC8, 0 );
    iLuid1 = iDmCallback->iLUID->Des().AllocL();
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/Name"), *iLuid1, KName, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/ProfileType"), *iLuid1, KProfileType, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/AddressOfRecord"), *iLuid1, KAddressOfRecord, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Host"), *iLuid1, KOBHost, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Port"), *iLuid1, KOBPort, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Transport"), *iLuid1, KOBTransport, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Username"), *iLuid1, KOBUsername, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Passwd"), *iLuid1, KOBPasswd, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Realm"), *iLuid1, KOBRealm, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Host"), *iLuid1, KRSHost, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Port"), *iLuid1, KRSPort, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Transport"), *iLuid1, KRSTransport, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    
    // This "EUnit" profile's type is IETF so must use Username instead of Private Identity
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/RegistrarServer/PrivateIdentity"), *iLuid1, KRSPrivateIdentity, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EError == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Username"), *iLuid1, KRSUsername, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Passwd"), *iLuid1, KRSPasswd, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Realm"), *iLuid1, KRSRealm, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/Default"), *iLuid1, KDefault, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/LooseRouting"), *iLuid1, KLr, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/EnableSecurityNegotiation"), *iLuid1, KEnableSecurityNegotiation, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/EnableSignalCompression"), *iLuid1, KEnableSignalCompression, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/EnableAutoRegistration"), *iLuid1, KEnableAutoRegistration, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );

    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/PrefConRef"), *iLuid1, KConRef, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/ProfileLock"), *iLuid1, KProfileLock, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/DestinationNetwork"), *iLuid1, KSIPSnap, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    // Testing ims type profile - so must use Private Identity instead of Username 
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/RegistrarServer/PrivateIdentity"), *iLuid1, KRSPrivateIdentity, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EError == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/ProfileType"), *iLuid1, KProfileType2, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/RegistrarServer/PrivateIdentity"), *iLuid1, KRSPrivateIdentity, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    
    // Update some values.

    // In theory this should return 200 but SIP does not have 'other' plug-in so 
    // EInvalidObject is returned.
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/ProfileType"), *iLuid1, _L8( "OTHER" ), KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EInvalidObject == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Port"), *iLuid1, KNullDesC8, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Transport"), *iLuid1, _L8( "other" ), KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EError == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Host"), *iLuid1, KNullDesC8, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Port"), *iLuid1, KNullDesC8, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Transport"), *iLuid1, _L8("other"), KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EError == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Host"), *iLuid1, KNullDesC8, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );

    
    // Test invalid settings.
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/NotExistingProfile/ProfileLock"), _L8( "12345678989" ), KProfileLock, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::ENotFound == iDmCallback->iStatus );

    // Overly long name.
    _LIT8( KTooLongName, "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901");
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/Name"), *iLuid1, KTooLongName, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EInvalidObject == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/ProfileType"), *iLuid1, _L8( "Else" ), KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EInvalidObject == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/Default"), *iLuid1, _L8( "Yeees" ), KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EInvalidObject == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/LooseRouting"), *iLuid1, _L8( "Yeees" ), KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EInvalidObject == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/EnableSecurityNegotiation"), *iLuid1, _L8( "Yeees" ), KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EInvalidObject == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/EnableSignalCompression"), *iLuid1, _L8( "Yeees" ), KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EInvalidObject == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/EnableAutoRegistration"), *iLuid1, _L8( "Yes" ), KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EInvalidObject == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/ProfileLock"), *iLuid1, _L8( "Yes" ), KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EInvalidObject == iDmCallback->iStatus );
    }

void UT_CNSmlDmSIPAdapter::UT_CNSmlDmSIPAdapter_FetchLeafObjectLL(  )
    {
    TBuf8 <256> results;
    TBuf16< 8 > status;
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/Name"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KName );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/ProfileType"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KProfileType );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/AddressOfRecord"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KAddressOfRecord );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Host"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KOBHost );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Port"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KOBPort );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Transport"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KOBTransport );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Username"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KOBUsername );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Realm"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KOBRealm );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Host"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KRSHost );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Port"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KRSPort );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Transport"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KRSTransport );
    
    // Profile type is IETF so username is set
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Username"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KRSUsername );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Realm"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KRSRealm );

#ifndef __WINS__
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/Default"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KDefault );
#endif // __WINS__
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/LooseRouting"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KLr );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/EnableSecurityNegotiation"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KEnableSecurityNegotiation );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/EnableSignalCompression"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KEnableSignalCompression );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/EnableAutoRegistration"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KEnableAutoRegistration );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/PrefConRef"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KConRef );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/ProfileLock"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KProfileLock );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/DestinationNetwork"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == _L8("BearerManagementSNAP/2") );
    
    // Test fetching private identity from ims type profile
    
    iNSmlDmSIPAdapter->AddNodeObjectL( _L8("SIP/EUnitIms"), KNullDesC8, 0 );
    TPtr8 tmp = iDmCallback->iLUID->Des();
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnitIms/ProfileType"), tmp, KProfileType2, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );

    EUNIT_DISABLE_ALLOC_DECORATOR;
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnitIms/RegistrarServer/Host"), tmp, KRSHost, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnitIms/RegistrarServer/PrivateIdentity"), tmp, KRSPrivateIdentity, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    EUNIT_ENABLE_ALLOC_DECORATOR;
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnitIms/RegistrarServer/PrivateIdentity"), tmp, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KRSPrivateIdentity );
    
    iNSmlDmSIPAdapter->FetchLeafObjectSizeL( _L8("SIP/EUnit/ProfileLock"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == _L8("4") );
    
    // Delete ims profile from phone's SIP profiles
    iNSmlDmSIPAdapter->DeleteObjectL( _L8("SIP/EUnitIms"), tmp, 0 );
    }
    
void UT_CNSmlDmSIPAdapter::UT_CNSmlDmSIPAdapter_UpdateLeafObjectOtherLL(  )
    {
    TBuf16< 8 > status;
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/ProfileType"), *iLuid1, KProfileType2, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );

    EUNIT_DISABLE_ALLOC_DECORATOR;
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Username"), *iLuid1, KNullDesC8, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Transport"), *iLuid1, KOBTransport2, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Transport"), *iLuid1, KRSTransport2, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    EUNIT_ENABLE_ALLOC_DECORATOR;
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/EnableSecurityNegotiation"), *iLuid1, KEnableSecurityNegotiation2, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/EnableSignalCompression"), *iLuid1, KEnableSignalCompression2, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/EnableAutoRegistration"), *iLuid1, KEnableAutoRegistration2, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/Default"), *iLuid1, KDefault2, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    EUNIT_DISABLE_ALLOC_DECORATOR;
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/LooseRouting"), *iLuid1, KLr2, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    EUNIT_ENABLE_ALLOC_DECORATOR;
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( _L8("SIP/EUnit/ProfileLock"), *iLuid1, KProfileLock2, KNullDesC8, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    }
    
    
void UT_CNSmlDmSIPAdapter::UT_CNSmlDmSIPAdapter_FetchLeafObjectOtherLL(  )
    {
    TBuf8 <256> results;
    
    UT_CNSmlDmSIPAdapter_UpdateLeafObjectOtherLL();
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/ProfileType"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KProfileType2 );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/OutboundProxy/Transport"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KOBTransport2 );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/RegistrarServer/Transport"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KRSTransport2 );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/EnableSecurityNegotiation"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KEnableSecurityNegotiation2 );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/EnableSignalCompression"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KEnableSignalCompression2 );

    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/EnableAutoRegistration"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KEnableAutoRegistration2 );

#ifndef __WINS__
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/Default"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KDefault2 );
#endif // __WINS__
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/LooseRouting"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KLr2 );
    
    iNSmlDmSIPAdapter->FetchLeafObjectL( _L8("SIP/EUnit/ProfileLock"), *iLuid1, KNullDesC8, 0, 0 );
    results.Copy( iDmCallback->iResult );
    EUNIT_ASSERT( results == KProfileLock2 );
    }

void UT_CNSmlDmSIPAdapter::UT_CNSmlDmSIPAdapter_ChildURIListLL(  )
    {
    TBuf8 <600> resultFromCallback;
    TBuf16< 8 > statusFromCallback;
    
    TInt result( 0 );
    TInt status( 0 );
    CArrayFix<TSmlDmMappingInfo>* array = new ( ELeave ) CArrayFixFlat<TSmlDmMappingInfo>( 1 );
    CleanupStack::PushL( array );
    
    iNSmlDmSIPAdapter->ChildURIListL( _L8("SIP/EUnit"), *iLuid1, *array, result, status );
    
    statusFromCallback.Num( iDmCallback->iStatus );
    EUNIT_PRINT( statusFromCallback );    
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    resultFromCallback.Copy( iDmCallback->iResult );
    EUNIT_PRINT( resultFromCallback );
    EUNIT_ASSERT( _L8("SIPId/Name/ProfileType/AddressOfRecord/OutboundProxy/RegistrarServer/Default/LooseRouting/EnableSecurityNegotiation/EnableSignalCompression/EnableAutoRegistration/PrefConRef/ProfileLock/DestinationNetwork/SignalingQoS/") == iDmCallback->iResult );
    
    // Gives list of SIP profiles - in this test there is only one profile
    // Somehow this doesn't work in emulator - works in HW though

#ifndef __WINS__ 
    iNSmlDmSIPAdapter->ChildURIListL( _L8("SIP"), KNullDesC8, *array, result, status );
    
    statusFromCallback.Num( iDmCallback->iStatus );
    EUNIT_PRINT( statusFromCallback );    
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    
    resultFromCallback.Copy( iDmCallback->iResult );
    EUNIT_PRINT( resultFromCallback );
    
    EUNIT_ASSERT( *iLuid1 == iDmCallback->iResult.Left( 15 ) );
#endif // __WINS__

    array->Reset();
    CleanupStack::PopAndDestroy( array );
    }

void UT_CNSmlDmSIPAdapter::UT_CNSmlDmSIPAdapter_DeleteObjectLL(  )
    {
    TBuf16< 8 > status;
    
    iNSmlDmSIPAdapter->DeleteObjectL( _L8("SIP/EUnit"), *iLuid1, 0 );
    status.Num( iDmCallback->iStatus );
    EUNIT_PRINT( status );
    EUNIT_ASSERT( MSmlDmAdapter::EOk == iDmCallback->iStatus );
    }

void UT_CNSmlDmSIPAdapter::UT_CNSmlDmSIPAdapter_DesToIntL(  )
    {
    EUNIT_ASSERT_EQUALS( iNSmlDmSIPAdapter->DesToInt( _L8("0")), 0 );
    EUNIT_ASSERT_EQUALS( iNSmlDmSIPAdapter->DesToInt( _L8("9")), 9 );
    EUNIT_ASSERT_EQUALS( iNSmlDmSIPAdapter->DesToInt( _L8("-1")), -1 );
    }


void UT_CNSmlDmSIPAdapter::UT_CNSmlDmSIPAdapter_RemoveLastURISegL(  )
    {
    // Test empty uri
    EUNIT_ASSERT_EQUALS_DESC( iNSmlDmSIPAdapter->RemoveLastURISeg( _L8("")), _L8(""), NULL );
    
    // Test real URI
    EUNIT_ASSERT_EQUALS_DESC( iNSmlDmSIPAdapter->RemoveLastURISeg( _L8("./SIP/OutboundProxy/Host")), _L8("./SIP/OutboundProxy"), NULL );
    }
    
void UT_CNSmlDmSIPAdapter::UT_CNSmlDmSIPAdapter_NumOfURISegsL(  )
    {
    // Test empty URI
    EUNIT_ASSERT_EQUALS( iNSmlDmSIPAdapter->NumOfURISegs( _L8("")), 1 );
    
    // Test real URI
    EUNIT_ASSERT_EQUALS( iNSmlDmSIPAdapter->NumOfURISegs( _L8("./SIP/OutboundProxy/Host")), 4 );
    
    // Test big URI
    EUNIT_ASSERT_EQUALS( iNSmlDmSIPAdapter->NumOfURISegs( _L8("./SIP/OutboundPrsdglsadflkashofhewfoihawoiufhsdoxy/Hdlgkhaslkjbogoersogerpogsjeposgrgpsoejpsojieojieojivoismmiovgpmvsmgpesgoiergopveoimmoipgoadgasvinhuwavnhuievineihvhniavivvwaiwahvgivgiohgemgsoimpgeomriojgdghslkdgjoiwehfopihnbjiohbifpabsdfipoihost")), 4 );
    }  
    
void UT_CNSmlDmSIPAdapter::UT_CNSmlDmSIPAdapter_LastURISegL(  )
    {
    EUNIT_ASSERT_EQUALS_DESC( iNSmlDmSIPAdapter->LastURISeg( _L8("")), _L8(""), NULL );
    EUNIT_ASSERT_EQUALS_DESC( iNSmlDmSIPAdapter->LastURISeg( _L8("./SIP/OutboundProxy/Host")), _L8("Host"), NULL );
    }

void UT_CNSmlDmSIPAdapter::UT_CNSmlDmSIPAdapter_OtherFunctionsLL(  )
    {
    TInt nullValue( 0 );
    
    TImplementation object;
    iNSmlDmSIPAdapter->DDFStructureL( object );
    
    iNSmlDmSIPAdapter->ExecuteCommandL( KNullDesC8, KNullDesC8, KNullDesC8,
        KNullDesC8, nullValue );
    EUNIT_ASSERT( MSmlDmAdapter::EError == iDmCallback->iStatus );
    
    RWriteStream* stream = NULL;
    iNSmlDmSIPAdapter->ExecuteCommandL( KNullDesC8, KNullDesC8, stream,
            KNullDesC8, nullValue );
    EUNIT_ASSERT( MSmlDmAdapter::EError == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->CopyCommandL( KNullDesC8, KNullDesC8, KNullDesC8, KNullDesC8,
        KNullDesC8, nullValue );
    EUNIT_ASSERT( MSmlDmAdapter::EError == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->StartAtomicL();

    iNSmlDmSIPAdapter->CommitAtomicL();

    iNSmlDmSIPAdapter->RollbackAtomicL();
    
    TBool support = iNSmlDmSIPAdapter->StreamingSupport( nullValue );
    
    if ( support )
        {
        EUNIT_ASSERT( EFalse );
        }
    
    iNSmlDmSIPAdapter->StreamCommittedL();
    
    
    iNSmlDmSIPAdapter->CompleteOutstandingCmdsL();
    EUNIT_DISABLE_ALLOC_DECORATOR;
    iNSmlDmSIPAdapter->LoadProfilesL();
    EUNIT_ENABLE_ALLOC_DECORATOR;
    iNSmlDmSIPAdapter->iModifications.Insert( 0, iNSmlDmSIPAdapter->iModifications.Count() );
    iNSmlDmSIPAdapter->CompleteOutstandingCmdsL();
    
    
    iNSmlDmSIPAdapter->UpdateLeafObjectL( KNullDesC8, KNullDesC8, stream,
            KNullDesC8, nullValue );
    EUNIT_ASSERT( MSmlDmAdapter::EError == iDmCallback->iStatus );
    
    iNSmlDmSIPAdapter->ProfileRegistryEventOccurred( nullValue,
        MSIPProfileRegistryObserver::EProfileCreated );
    
    iNSmlDmSIPAdapter->ProfileRegistryErrorOccurred( nullValue, nullValue );
    }

void UT_CNSmlDmSIPAdapter::ProfileRegistryEventOccurred(TUint32 /*aProfileId*/, TEvent /*aEvent*/)
    {
    }

void UT_CNSmlDmSIPAdapter::ProfileRegistryErrorOccurred(TUint32 /*aProfileId*/, TInt /*aError*/)
    {
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE( 
    UT_CNSmlDmSIPAdapter,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "DDFVersionL - test ",
    "CNSmlDmSIPAdapter",
    "DDFVersionL",
    "FUNCTIONALITY",
    SetupL, UT_CNSmlDmSIPAdapter_DDFVersionLL, Teardown)

EUNIT_NOT_DECORATED_TEST(
    "AddNodeObjectL - test ",
    "CNSmlDmSIPAdapter",
    "AddNodeObjectL",
    "FUNCTIONALITY",
    SetupL, UT_CNSmlDmSIPAdapter_AddNodeObjectLL, Teardown2)

EUNIT_NOT_DECORATED_TEST(
    "UpdateLeafObjectL - test ",
    "CNSmlDmSIPAdapter",
    "UpdateLeafObjectL",
    "FUNCTIONALITY",
    SetupL, UT_CNSmlDmSIPAdapter_UpdateLeafObjectLL, Teardown2)    

EUNIT_TEST(
    "FetchLeafObjectL - test ",
    "CNSmlDmSIPAdapter",
    "FetchLeafObjectL",
    "FUNCTIONALITY",
    Setup2L, UT_CNSmlDmSIPAdapter_FetchLeafObjectLL, Teardown2) 

EUNIT_TEST(
    "UpdateLeafObjectOtherL - test ",
    "CNSmlDmSIPAdapter",
    "UpdateLeafObjectOtherL",
    "FUNCTIONALITY",
    Setup2L, UT_CNSmlDmSIPAdapter_UpdateLeafObjectOtherLL, Teardown2) 

EUNIT_TEST(
    "FetchLeafObjectOtherL - test ",
    "CNSmlDmSIPAdapter",
    "FetchLeafObjectOtherL",
    "FUNCTIONALITY",
    Setup2L, UT_CNSmlDmSIPAdapter_FetchLeafObjectOtherLL, Teardown2)

EUNIT_TEST(
    "ChildURIListL - test ",
    "CNSmlDmSIPAdapter",
    "ChildURIListL",
    "FUNCTIONALITY",
    Setup2L, UT_CNSmlDmSIPAdapter_ChildURIListLL, Teardown2)

EUNIT_TEST(
    "DesToInt - test ",
    "CNSmlDmSIPAdapter",
    "DesToInt",
    "FUNCTIONALITY",
    SetupL, UT_CNSmlDmSIPAdapter_DesToIntL, Teardown)

EUNIT_TEST(
    "RemoveLastURISegL - test ",
    "CNSmlDmSIPAdapter",
    "RemoveLastURISeg",
    "FUNCTIONALITY",
    SetupL, UT_CNSmlDmSIPAdapter_RemoveLastURISegL, Teardown)
    
EUNIT_TEST(
    "NumOfURISegsL - test ",
    "CNSmlDmSIPAdapter",
    "NumOfURISegs",
    "FUNCTIONALITY",
    SetupL, UT_CNSmlDmSIPAdapter_NumOfURISegsL, Teardown)

EUNIT_TEST(
    "LastURISegL - test ",
    "CNSmlDmSIPAdapter",
    "LastURISeg",
    "FUNCTIONALITY",
    SetupL, UT_CNSmlDmSIPAdapter_LastURISegL, Teardown)

EUNIT_TEST(
    "OtherFunctionsL - test ",
    "CNSmlDmSIPAdapter",
    "OtherFunctions",
    "FUNCTIONALITY",
    SetupL, UT_CNSmlDmSIPAdapter_OtherFunctionsLL, Teardown)

EUNIT_TEST(
    "DeleteObjectL - test ",
    "CNSmlDmSIPAdapter",
    "DeleteObjectL",
    "FUNCTIONALITY",
    Setup2L, UT_CNSmlDmSIPAdapter_DeleteObjectLL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE

