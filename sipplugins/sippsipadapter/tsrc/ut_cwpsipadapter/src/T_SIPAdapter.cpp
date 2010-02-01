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
#include "T_SIPAdapter.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/EUnitMacros.h>
#include <f32file.h>
#include <CWPEngine.h>
#include <BADESCA.H>
#include <SIPManagedProfile.h>
#include <SIPManagedProfileRegistry.h>
#include <SIPProfile.h>
#include <MWPElement.h>
#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include <cmmanagerdef.h>
#include <cmconnectionmethodext.h>
// INTERNAL INCLUDES
#include "CSIPProfileRegistryObserver.h"
#include "wpsipadapterdebug.h"

#include <in_sock.h>

// ---------------
// Profile 1, IMS
// ---------------
_LIT8( KProviderId1,        "Provider" );
_LIT8( KPublicUsername1,    "sips:TestUser@sipserver" );
// Proxy settings
_LIT8( KProxyAddress1,      "sips:192.0.9.5:5060;transport=UDP" );
_LIT8( KPrivateIdentity1,   "testusername5" );
//_LIT8( KProxyPassword1,     "testpassword5" );
_LIT8( KProxyRealm1,        "realmi" );
// Registrar settings
_LIT8( KRegistrarAddr1,     "sips:sipserver:5060;transport=UDP" );
//_LIT8( KRegistrarPassword1, "testpassword5" );
_LIT8( KRegistrarRealm1,    "realmi" );

// ---------------
// Profile 2, IETF
// ---------------
_LIT8( KProviderId2,        "Sofia" );
_LIT8( KPublicUsername2,    "sip:theTester@10.36.2.82" );
// Proxy settings
_LIT8( KProxyAddress2,      "sip:10.36.2.82:5060;transport=TCP;lr" );
_LIT8( KProxyUsername2,     "SofiaUser1" );
//_LIT8( KProxyPassword2,     "SofiaAuth1" );
_LIT8( KProxyRealm2,        "10.36.2.82" );
// Registrar settings
_LIT8( KRegistrarAddr2,     "sip:10.36.2.82:5060;transport=TCP" );
_LIT8( KRegistrarUsername2, "SofiaUser2" );
//_LIT8( KRegistrarPassword2, "SofiaAuth2" );
_LIT8( KRegistrarRealm2,    "10.36.2.82" );

// ---------------
// Profile 3, IETF
// ---------------
_LIT8( KProviderId3,        "Provider(1)" );


// CONSTRUCTION
T_SIPAdapter* T_SIPAdapter::NewL()
    {
    T_SIPAdapter* self = T_SIPAdapter::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

T_SIPAdapter* T_SIPAdapter::NewLC()
    {
    T_SIPAdapter* self = new( ELeave ) T_SIPAdapter();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
T_SIPAdapter::~T_SIPAdapter()
    {
    iFs.Close();
    }

// Default constructor
T_SIPAdapter::T_SIPAdapter() : iProvData( NULL, 0 )
    {
    }

// Second phase construct
void T_SIPAdapter::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    User::LeaveIfError( iFs.Connect() );
    }

//  METHODS

// ---------------------------------------------------------------------------
// T_SIPAdapter::SetupL
// Empty.
// ---------------------------------------------------------------------------
//
void T_SIPAdapter::SetupL(  )
    {
    }

// ---------------------------------------------------------------------------
// T_SIPAdapter::Setup2L
// Deletes all SIP profiles, IAPs and SNAPs from phone to avoid name
// conflicts.
// ---------------------------------------------------------------------------
//
void T_SIPAdapter::Setup2L(  )
    {
    ResetSettingsL();
    }

// ---------------------------------------------------------------------------
// T_SIPAdapter::Teardown
// Deletes iProvisioningDoc if it has been instantiated.
// ---------------------------------------------------------------------------
//
void T_SIPAdapter::Teardown(  )
    {
    if ( iProvisioningDoc )
        {
        delete iProvisioningDoc;
        iProvisioningDoc = NULL;
        }
    }

// ---------------------------------------------------------------------------
// T_SIPAdapter::ResetSettingsL
// 1. Empties the SIP profile registry, i.e. deletes all SIP profiles.
// 2. Deletes all Destination Networks (SNAPs).
// 3. Deletes all IAPs.
// ---------------------------------------------------------------------------
//
void T_SIPAdapter::ResetSettingsL(  )
    {
    __UHEAP_MARK;

    // Delete all SIP profiles from phone.
    CSIPProfileRegistryObserver* observer = 
        CSIPProfileRegistryObserver::NewLC(); // CS:1
    CSIPManagedProfileRegistry* registry =
        CSIPManagedProfileRegistry::NewLC( *observer ); // CS:2
    RPointerArray<CSIPProfile> profiles;
    CleanupClosePushL( profiles ); // CS:3
    registry->ProfilesL( profiles );
    for ( TInt counter = 0; counter < profiles.Count(); counter++ )
        {
        CSIPManagedProfile* mprofile = 
            static_cast<CSIPManagedProfile*>( profiles[counter] );
        TUint32 intti;
        mprofile->GetParameter( KSIPAccessPointId, intti );
        registry->DestroyL( *profiles[counter] );
        profiles[counter] = NULL;
        }
    // &profiles, registry, observer
    CleanupStack::PopAndDestroy( 3, observer );

    // Delete all SNAPs.
    RCmManagerExt manager;
    manager.OpenL();
    CleanupClosePushL( manager );
    RArray<TUint32> snapIds;
    CleanupClosePushL( snapIds );
    manager.AllDestinationsL( snapIds );
    for ( TInt counter = 0; counter < snapIds.Count(); counter++ )
        {
        RCmDestinationExt dest = manager.DestinationL( snapIds[counter] );
        dest.DeleteLD();
        }
    // &snapIds, &manager
    CleanupStack::PopAndDestroy( 2, &manager );

    // Delete all IAPs.
    RCmManagerExt cmm;
    cmm.OpenL();
    CleanupClosePushL( cmm );
    RArray<TUint32> iapIds;
    CleanupClosePushL( iapIds );
    cmm.ConnectionMethodL( iapIds, EFalse, EFalse, EFalse );
    for ( TInt counter = 0; counter < iapIds.Count(); counter++ )
        {
        RCmConnectionMethodExt cm = 
            cmm.ConnectionMethodL( iapIds[counter] );
        cm.DeleteL();
        }
    // &iapIds, &manager
    CleanupStack::PopAndDestroy( 2, &cmm );

    __UHEAP_MARKEND;
    }

// ---------------------------------------------------------------------------
// T_SIPAdapter::StoreL
// Runs SaveDocumentL and checks that it doesn't leave.
// ---------------------------------------------------------------------------
//
void T_SIPAdapter::StoreL(  )
    {
    __UHEAP_MARK;
    _LIT( KFileName, "c:\\data\\sipadaptertest.wbxml" );
    EUNIT_ASSERT_NO_LEAVE( SaveDocumentL( KFileName() ) );
    __UHEAP_MARKEND;
    }

// ---------------------------------------------------------------------------
// T_SIPAdapter::SaveDocumentL
// Saves settings.
// ---------------------------------------------------------------------------
//
void T_SIPAdapter::SaveDocumentL( const TDesC16& aFilename )
    {
    __UHEAP_MARK;
    TInt fileSize ( 0 );
    TInt err( iFile.Open( iFs, aFilename, EFileShareReadersOnly ) );
    if ( KErrNone == err )
        {
        err = iFile.Size( fileSize );
        iFile.Close();
        }

    delete iProvisioningDoc;
    iProvisioningDoc = NULL;
    iProvisioningDoc = HBufC8::NewL( fileSize );
    iProvData.Set( iProvisioningDoc->Des() );

	User::LeaveIfError( iFile.Open( iFs, aFilename, EFileShareReadersOnly ) );
	User::LeaveIfError( iFile.Read( iProvData ) );
	iFile.Close();

    // The ProvisioningEngine 
    CWPEngine* engine = CWPEngine::NewL();
    CleanupStack::PushL( engine );
    engine->ImportDocumentL( iProvData );
    engine->PopulateL();

    // Save OMA message
    for ( TInt i = 0; i < engine->ItemCount(); i++ )
        {
        engine->SaveL( i );
        }
    CleanupStack::PopAndDestroy( engine );
    __UHEAP_MARKEND;
    }

// ---------------------------------------------------------------------------
// T_SIPAdapter::CheckSettingsL
// Checks saved settings.
// ---------------------------------------------------------------------------
//
void T_SIPAdapter::CheckSettingsL(  )
    {
    __UHEAP_MARK;

    CSIPProfileRegistryObserver* observer = 
        CSIPProfileRegistryObserver::NewLC(); // CS:1
    CSIPManagedProfileRegistry* registry =
        CSIPManagedProfileRegistry::NewLC( *observer ); // CS:2
    RPointerArray<CSIPProfile> profiles;
    CleanupClosePushL( profiles ); // CS:3
    registry->ProfilesL( profiles );

    EUNIT_ASSERT( 3 == profiles.Count() );

    TUint32 tuintValue( KErrNone );
    TBuf8<100> bufValue;
    TBool boolValue( EFalse );

    // First SIP profile.
    CSIPManagedProfile* profile = 
        static_cast<CSIPManagedProfile*>( profiles[0] );

    const TDesC8* provider1 = NULL;
    profile->GetParameter( KSIPProviderName, provider1 );
    bufValue.Copy( *provider1 );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( KProviderId1() == bufValue );

    TSIPProfileTypeInfo type1 = profile->Type();
    bufValue.Num( type1.iSIPProfileClass );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( TSIPProfileTypeInfo::EIms == type1.iSIPProfileClass );

    profile->GetParameter( KSIPAccessPointId, tuintValue );
    bufValue.Num( tuintValue );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( 1 < tuintValue );
    tuintValue = 0;

    const TDesC8* puid1 = NULL;
    profile->GetParameter( KSIPUserAor, puid1 );
    bufValue.Copy( *puid1 );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( KPublicUsername1() == bufValue );

    profile->GetParameter( KSIPSigComp, boolValue );
    bufValue.Num( boolValue );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( ETrue == boolValue );
    boolValue = EFalse;

    profile->GetParameter( KSIPSecurityNegotiation, boolValue );
    bufValue.Num( boolValue );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( ETrue == boolValue );
    boolValue = ETrue;

    profile->GetParameter( KSIPAutoRegistration, boolValue );
    bufValue.Num( boolValue );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( EFalse == boolValue );
    boolValue = EFalse;

    const TDesC8* proxy1 = NULL;
    profile->GetParameter( KSIPOutboundProxy, KSIPServerAddress, proxy1 );
    bufValue.Copy( *proxy1 );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( KProxyAddress1() == bufValue );

    const TDesC8* proxyUser1 = NULL;
    profile->GetParameter( KSIPPrivateIdentity, proxyUser1 );
    bufValue.Copy( *proxyUser1 );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( KPrivateIdentity1() == bufValue );

    const TDesC8* proxyPw1 = NULL;
    TInt error = profile->GetParameter( KSIPOutboundProxy, KSIPDigestPassword, proxyPw1 );
    
    // Passwords are changed to write only (in API) so can't read the contents.      
    EUNIT_ASSERT( KErrPermissionDenied == error );
    error = KErrNone;

    const TDesC8* proxyRealm1 = NULL;
    profile->GetParameter( KSIPOutboundProxy, KSIPDigestRealm, proxyRealm1 );
    bufValue.Copy( *proxyRealm1 );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( KProxyRealm1() == bufValue );

    const TDesC8* registrarUri1 = NULL;
    profile->GetParameter( KSIPRegistrar, KSIPServerAddress, registrarUri1 );
    bufValue.Copy( *registrarUri1 );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( KRegistrarAddr1() == bufValue );

    const TDesC8* registrarPw1 = NULL;
    error = profile->GetParameter( KSIPRegistrar, KSIPDigestPassword, registrarPw1 );
    EUNIT_ASSERT( KErrPermissionDenied == error );
    error = KErrNone;

    const TDesC8* registrarRealm1 = NULL;
    profile->GetParameter( KSIPRegistrar, KSIPDigestRealm, registrarRealm1 );
    bufValue.Copy( *registrarRealm1 );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( KRegistrarRealm1() == bufValue );

    profile->GetParameter( KSIPDefaultProfile, boolValue );
    bufValue.Num( boolValue );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( ETrue == boolValue );
    boolValue = EFalse;

    profile->GetParameter( KSIPProfileLocked, boolValue );
    bufValue.Num( boolValue );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( ETrue == boolValue );
    boolValue = ETrue;

    // Second SIP profile.
    profile = NULL;
    profile = static_cast<CSIPManagedProfile*>( profiles[1] );

    const TDesC8* provider2 = NULL;
    profile->GetParameter( KSIPProviderName, provider2 );
    bufValue.Copy( *provider2 );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( KProviderId2() == bufValue );

    TSIPProfileTypeInfo type2 = profile->Type();
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( TSIPProfileTypeInfo::EInternet == type2.iSIPProfileClass );

    profile->GetParameter( KSIPSnapId, tuintValue );
    bufValue.Num( tuintValue );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( 1 < tuintValue );
    tuintValue = 0;

    const TDesC8* puid2 = NULL;
    profile->GetParameter( KSIPUserAor, puid2 );
    bufValue.Copy( *puid2 );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( KPublicUsername2() == bufValue );

    profile->GetParameter( KSIPSigComp, boolValue );
    bufValue.Num( boolValue );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( EFalse == boolValue );
    boolValue = ETrue;

    profile->GetParameter( KSIPSecurityNegotiation, boolValue );
    bufValue.Num( boolValue );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( EFalse == boolValue );
    boolValue = ETrue;

    profile->GetParameter( KSIPAutoRegistration, boolValue );
    bufValue.Num( boolValue );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( EFalse == boolValue );
    boolValue = ETrue;

    const TDesC8* proxy2 = NULL;
    profile->GetParameter( KSIPOutboundProxy, KSIPServerAddress, proxy2 );
    bufValue.Copy( *proxy2 );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( KProxyAddress2() == bufValue );

    const TDesC8* proxyUsr2 = NULL;
    profile->GetParameter( KSIPOutboundProxy, KSIPDigestUserName, proxyUsr2 );
    bufValue.Copy( *proxyUsr2 );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( KProxyUsername2() == bufValue );

    const TDesC8* proxyPw2 = NULL;
    error = profile->GetParameter( KSIPOutboundProxy, KSIPDigestPassword, proxyPw2 );
    EUNIT_ASSERT( KErrPermissionDenied == error );
    error = KErrNone;

    const TDesC8* proxyRealm2 = NULL;
    profile->GetParameter( KSIPOutboundProxy, KSIPDigestRealm, proxyRealm2 );
    bufValue.Copy( *proxyRealm2 );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( KProxyRealm2() == bufValue );

    const TDesC8* registrarUri2 = NULL;
    profile->GetParameter( KSIPRegistrar, KSIPServerAddress, registrarUri2 );
    bufValue.Copy( *registrarUri2 );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( KRegistrarAddr2() == bufValue );

    const TDesC8* registrarUsr2 = NULL;
    profile->GetParameter( KSIPRegistrar, KSIPDigestUserName, registrarUsr2 );
    bufValue.Copy( *registrarUsr2 );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( KRegistrarUsername2() == bufValue );

    const TDesC8* registrarPw2 = NULL;
    error = profile->GetParameter( KSIPRegistrar, KSIPDigestPassword, registrarPw2 );
    EUNIT_ASSERT( KErrPermissionDenied == error );
    error = KErrNone;

    const TDesC8* registrarRealm2 = NULL;
    profile->GetParameter( KSIPRegistrar, KSIPDigestRealm, registrarRealm2 );
    bufValue.Copy( *registrarRealm2 );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( KRegistrarRealm2() == bufValue );

    profile->GetParameter( KSIPDefaultProfile, boolValue );
    bufValue.Num( boolValue );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( EFalse == boolValue );
    boolValue = ETrue;

    // Profile not locked, SIPAdapter did not handle the setting, setting not
    // stored, should return KErrNotFound.
    TInt err = profile->GetParameter( KSIPProfileLocked, boolValue );
    DBG_PRINT_2( "read KSIPProfileLocked from profile, error code %d", err );
    EUNIT_ASSERT( KErrNotFound == err );


    // Third SIP profile.
    profile = NULL;
    profile = static_cast<CSIPManagedProfile*>( profiles[2] );

    const TDesC8* provider3 = NULL;
    profile->GetParameter( KSIPProviderName, provider3 );
    bufValue.Copy( *provider3 );
    EUNIT_PRINT( bufValue );
    EUNIT_ASSERT( KProviderId3() == bufValue );


    profile = NULL;
    // &profiles, registry, observer
    CleanupStack::PopAndDestroy( 3, observer );

    __UHEAP_MARKEND;
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    T_SIPAdapter,
    "SIP adapter unit&module tests.",
    "MODULE" )

EUNIT_TEST(
    "Store profiles",
    "SIPAdapter",
    "VisitL, VisitLinkL, SaveL, setters and getters",
    "FUNCTIONALITY",
    Setup2L, StoreL, Teardown)

EUNIT_TEST(
    "Check updated settings",
    "T_SIPAdapter",
    "Direct SIP API usage to check settings",
    "FUNCTIONALITY",
    SetupL, CheckSettingsL, Teardown)

EUNIT_TEST(
    "Clean up stored settings",
    "T_SIPAdapter",
    "SIP & CMManager API usage",
    "FUNCTIONALITY",
    SetupL, ResetSettingsL, Teardown)


EUNIT_END_TEST_TABLE

//  End of file.
