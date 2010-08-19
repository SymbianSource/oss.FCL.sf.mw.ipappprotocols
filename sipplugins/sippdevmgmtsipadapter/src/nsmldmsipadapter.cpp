/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Device Management SIP Adapter
*
*/


// INCLUDE FILES
#include <utf.h>
#include <ecom/implementationproxy.h> // For TImplementationProxy definition
#include <sipprofile.h>
#include <sipprofiletypeinfo.h>
#include <sipstrings.h>
#include <sipstrconsts.h>
#include <f32file.h>
#include <sysutil.h>
#include <escapeutils.h>

#include <sipmanagedprofile.h>
#include <sipprofileregistryobserver.h>
#include <sipmanagedprofileregistry.h>
#include <sipaddress.h>
#include <nsmldmiapmatcher.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>

#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include <cmmanagerdef.h>

#include "nsmldmsipadapter.h"
#include "smldmsipadapterconstants.h"
#include "smldmsipadapterdebug.h"

// CONSTANTS
_LIT8( KEscaped, "%" );

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter* CNSmlDmSIPAdapter::NewL
//
// ---------------------------------------------------------------------------
//
CNSmlDmSIPAdapter* CNSmlDmSIPAdapter::NewL(
    MSmlDmCallback* aDmCallback )
    {
    DBG_PRINT("CNSmlDmSIPAdapter::NewL(): begin");
    CNSmlDmSIPAdapter* self = NewLC( aDmCallback );
    CleanupStack::Pop( self );
    DBG_PRINT("CNSmlDmSIPAdapter::NewL(): end");
    return self;
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter* CNSmlDmSIPAdapter::NewLC
//
// ---------------------------------------------------------------------------
//
CNSmlDmSIPAdapter* CNSmlDmSIPAdapter::NewLC(
    MSmlDmCallback* aDmCallback )
    {
    DBG_PRINT("CNSmlDmSIPAdapter::NewLC(): begin");
    CNSmlDmSIPAdapter* self = new (ELeave) CNSmlDmSIPAdapter( aDmCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    self->iDmCallback = aDmCallback;
    DBG_PRINT("CNSmlDmSIPAdapter::NewLC(): end");
    return self;
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::ConstructL
//
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::ConstructL()
    {
    DBG_PRINT("CNSmlDmSIPAdapter::ConstructL(): begin");
    
    // Open sip strings for methods that use it.
    SIPStrings::OpenL();
    DBG_PRINT("CNSmlDmSIPAdapter::ConstructL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::~CNSmlDmSIPAdapter
//
// ---------------------------------------------------------------------------
//
CNSmlDmSIPAdapter::~CNSmlDmSIPAdapter()
    {
    DBG_PRINT("CNSmlDmSIPAdapter::~CNSmlDmSIPAdapter(): begin");
    if ( iProfiles )
        {
        iProfiles->ResetAndDestroy();
        delete iProfiles;
        }

    if ( iSIPManagedProfileRegistry )
        {
        delete iSIPManagedProfileRegistry;
        }

    if ( iResults )
        {
        delete iResults;
        }
    iModifications.Reset();
    SIPStrings::Close();
    DBG_PRINT("CNSmlDmSIPAdapter::~CNSmlDmSIPAdapter(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::CNSmlDmSIPAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CNSmlDmSIPAdapter::CNSmlDmSIPAdapter( TAny* aEcomArguments ) : CSmlDmAdapter( 
    aEcomArguments )
    {
    DBG_PRINT("CNSmlDmSIPAdapter::CNSmlDmSIPAdapter(aEcomArguments): begin");
    DBG_PRINT("CNSmlDmSIPAdapter::CNSmlDmSIPAdapter(aEcomArguments): end"); 
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::LoadProfilesL
// Loads all profile information to memory.
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::LoadProfilesL()
    {
    DBG_PRINT("CNSmlDmSIPAdapter::LoadProfilesL(): begin");

    // Create needed instances.
    iSIPManagedProfileRegistry  = CSIPManagedProfileRegistry::NewL( *this );

    RArray<TSIPProfileTypeInfo> profileTypes;
    CleanupClosePushL( profileTypes ); // CS:1

     // Get the supported profile types
    iSIPManagedProfileRegistry->SupportedProfileTypesL( profileTypes );

    // Create the profile pointer array.
    iProfiles = new ( ELeave ) CArrayPtrFlat<CSIPManagedProfile>( 5 );    

    RPointerArray<CSIPProfile> profilePointerArray;
    CleanupResetAndDestroyPushL( profilePointerArray ); // CS:2

    // Get all profiles based on profile types.
    const TInt profileTypecount = profileTypes.Count();
    for ( TInt i = 0; i < profileTypecount; i++ )
        {
        iSIPManagedProfileRegistry->ProfilesL( 
            profileTypes[i], profilePointerArray );
        const TInt profileCount = profilePointerArray.Count();
        for ( TInt j = 0; j < profileCount; j++ )
            {
            CSIPManagedProfile* profile =
                static_cast<CSIPManagedProfile*>( profilePointerArray[j] );
            iProfiles->AppendL( profile );
            }
        profilePointerArray.Reset();
        }
    // &profilePointerArray, &profileTypes
    CleanupStack::PopAndDestroy( 2, &profileTypes ); // CS:0
    DBG_PRINT("CNSmlDmSIPAdapter::LoadProfilesL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::DDFVersionL
// Inserts DDF version of the adapter to aDDFVersion
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::DDFVersionL(
    CBufBase& aDDFVersion )
    {
    DBG_PRINT("CNSmlDmSIPAdapter::DDFVersionL(TDes& aDDFVersion): begin");
    aDDFVersion.InsertL( 0, KNSmlDMSIPDDFVersion );
    DBG_PRINT("CNSmlDmSIPAdapter::DDFVersionL(TDes& aDDFVersion): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::DDFStructureL
// Builds the DDF structure of adapter
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
    {
    DBG_PRINT("CNSmlDmSIPAdapter::DDFStructureL(): begin");

    TSmlDmAccessTypes accessTypesGet;
    accessTypesGet.SetGet();

    TSmlDmAccessTypes accessTypesGetAdd;
    accessTypesGetAdd.SetGet();
    accessTypesGetAdd.SetAdd();

    TSmlDmAccessTypes accessTypesGetReplaceAdd;
    accessTypesGetReplaceAdd.SetGet();
    accessTypesGetReplaceAdd.SetReplace();
    accessTypesGetReplaceAdd.SetAdd();

    TSmlDmAccessTypes accessTypesReplaceAdd;
    accessTypesReplaceAdd.SetReplace();
    accessTypesReplaceAdd.SetAdd();

    TSmlDmAccessTypes accessTypesAll;
    accessTypesAll.SetGet();
    accessTypesAll.SetDelete();
    accessTypesAll.SetAdd();
    accessTypesAll.SetReplace();

    // SIP
    MSmlDmDDFObject& sip = aDDF.AddChildObjectL( KNSmlDMSIPNodeName );
    sip.SetAccessTypesL( accessTypesGet );
    sip.SetOccurenceL( MSmlDmDDFObject::EOne );
    sip.SetScopeL( MSmlDmDDFObject::EPermanent );
    sip.SetDFFormatL( MSmlDmDDFObject::ENode );
    sip.SetDescriptionL( KNSmlDMSIPNodeNameExp );

    // dynamic settings ID node
    MSmlDmDDFObject& idNode = sip.AddChildObjectGroupL();
    idNode.SetAccessTypesL( accessTypesAll );
    idNode.SetOccurenceL( MSmlDmDDFObject::EZeroOrMore );
    idNode.SetScopeL( MSmlDmDDFObject::EDynamic );
    idNode.SetDFFormatL( MSmlDmDDFObject::ENode );
    idNode.SetDescriptionL( KNSmlDMSIPDynamicNodeExp );

    // SipID
    MSmlDmDDFObject& sipID = idNode.AddChildObjectL( KNSmlDMSIPSipID );
    sipID.SetAccessTypesL( accessTypesGet );
    sipID.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    sipID.SetScopeL( MSmlDmDDFObject::EDynamic );
    sipID.SetDFFormatL( MSmlDmDDFObject::EChr );
    sipID.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    sipID.SetDescriptionL( KNSmlDMSIPSipIDExp );

    // Name
    MSmlDmDDFObject& name = idNode.AddChildObjectL( KNSmlDMSIPName );
    name.SetAccessTypesL( accessTypesGetReplaceAdd );
    name.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    name.SetScopeL( MSmlDmDDFObject::EDynamic );
    name.SetDFFormatL( MSmlDmDDFObject::EChr );
    name.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    name.SetDescriptionL( KNSmlDMSIPNameExp );

    // ProfileType
    MSmlDmDDFObject& profType = idNode.AddChildObjectL( 
        KNSmlDMSIPProfileType );
    profType.SetAccessTypesL( accessTypesGetReplaceAdd );
    profType.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    profType.SetScopeL( MSmlDmDDFObject::EDynamic );
    profType.SetDFFormatL( MSmlDmDDFObject::EChr );
    profType.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    profType.SetDescriptionL( KNSmlDMSIPProfileTypeExp );

    // Address Of Record
    MSmlDmDDFObject& aor = idNode.AddChildObjectL( KNSmlDMSIPAOR );
    aor.SetAccessTypesL( accessTypesGetReplaceAdd );
    aor.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    aor.SetScopeL( MSmlDmDDFObject::EDynamic );
    aor.SetDFFormatL( MSmlDmDDFObject::EChr );
    aor.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    aor.SetDescriptionL( KNSmlDMSIPAORExp );

    // OutboundProxy Node
    MSmlDmDDFObject& obProxy = idNode.AddChildObjectL( 
        KNSmlDMSIPOutboundProxy );
    obProxy.SetAccessTypesL( accessTypesGetAdd );
    obProxy.SetOccurenceL( MSmlDmDDFObject::EOne );
    obProxy.SetScopeL( MSmlDmDDFObject::EDynamic );
    obProxy.SetDFFormatL( MSmlDmDDFObject::ENode );
    obProxy.SetDescriptionL( KNSmlDMSIPOutboundProxyExp );

    // OutboundProxy/Host
    MSmlDmDDFObject& obHost = obProxy.AddChildObjectL( KNSmlDMSIPHost );
    obHost.SetAccessTypesL( accessTypesGetReplaceAdd );
    obHost.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    obHost.SetScopeL( MSmlDmDDFObject::EDynamic );
    obHost.SetDFFormatL( MSmlDmDDFObject::EChr );
    obHost.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    obHost.SetDescriptionL( KNSmlDMSIPHostExp );

    // OutboundProxy/Port
    MSmlDmDDFObject& obPort = obProxy.AddChildObjectL( KNSmlDMSIPPort );
    obPort.SetAccessTypesL( accessTypesGetReplaceAdd );
    obPort.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    obPort.SetScopeL( MSmlDmDDFObject::EDynamic );
    obPort.SetDFFormatL( MSmlDmDDFObject::EInt );
    obPort.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    obPort.SetDescriptionL( KNSmlDMSIPPortExp );

    // OutboundProxy/Transport
    MSmlDmDDFObject& obTransport = obProxy.AddChildObjectL( 
        KNSmlDMSIPTransport );
    obTransport.SetAccessTypesL( accessTypesGetReplaceAdd );
    obTransport.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    obTransport.SetScopeL( MSmlDmDDFObject::EDynamic );
    obTransport.SetDFFormatL( MSmlDmDDFObject::EChr );
    obTransport.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    obTransport.SetDescriptionL( KNSmlDMSIPTransportExp );

    // OutboundProxy/Username
    MSmlDmDDFObject& obUsername = obProxy.AddChildObjectL( 
        KNSmlDMSIPUsername );
    obUsername.SetAccessTypesL( accessTypesGetReplaceAdd );
    obUsername.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    obUsername.SetScopeL( MSmlDmDDFObject::EDynamic );
    obUsername.SetDFFormatL( MSmlDmDDFObject::EChr );
    obUsername.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    obUsername.SetDescriptionL( KNSmlDMSIPUsernameExp );

    // OutboundProxy/Passwd
    MSmlDmDDFObject& obPasswd = obProxy.AddChildObjectL( KNSmlDMSIPPasswd );
    obPasswd.SetAccessTypesL( accessTypesReplaceAdd );
    obPasswd.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    obPasswd.SetScopeL( MSmlDmDDFObject::EDynamic );
    obPasswd.SetDFFormatL( MSmlDmDDFObject::EChr );
    obPasswd.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    obPasswd.SetDescriptionL( KNSmlDMSIPPasswdExp );

    // OutboundProxy/Realm
    MSmlDmDDFObject& obRealm = obProxy.AddChildObjectL( KNSmlDMSIPRealm );
    obRealm.SetAccessTypesL( accessTypesGetReplaceAdd );
    obRealm.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    obRealm.SetScopeL( MSmlDmDDFObject::EDynamic );
    obRealm.SetDFFormatL( MSmlDmDDFObject::EChr );
    obRealm.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    obRealm.SetDescriptionL( KNSmlDMSIPRealmExp );

    // RegistrarServer Node
    MSmlDmDDFObject& regProxy = idNode.AddChildObjectL( 
        KNSmlDMSIPRegistrarServer );
    regProxy.SetAccessTypesL( accessTypesGetAdd );
    regProxy.SetOccurenceL( MSmlDmDDFObject::EOne );
    regProxy.SetScopeL( MSmlDmDDFObject::EDynamic );
    regProxy.SetDFFormatL( MSmlDmDDFObject::ENode );
    regProxy.SetDescriptionL( KNSmlDMSIPRegistrarServerExp );

    // RegistrarServer/Host
    MSmlDmDDFObject& regHost = regProxy.AddChildObjectL( KNSmlDMSIPHost );
    regHost.SetAccessTypesL( accessTypesGetReplaceAdd );
    regHost.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    regHost.SetScopeL( MSmlDmDDFObject::EDynamic );
    regHost.SetDFFormatL( MSmlDmDDFObject::EChr );
    regHost.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    regHost.SetDescriptionL( KNSmlDMSIPHostExp );

     // RegistrarServer/Port
    MSmlDmDDFObject& regPort = regProxy.AddChildObjectL( KNSmlDMSIPPort );
    regPort.SetAccessTypesL( accessTypesGetReplaceAdd );
    regPort.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    regPort.SetScopeL( MSmlDmDDFObject::EDynamic );
    regPort.SetDFFormatL( MSmlDmDDFObject::EInt );
    regPort.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    regPort.SetDescriptionL( KNSmlDMSIPPortExp );

    // RegistrarServer/Transport
    MSmlDmDDFObject& regTransport = regProxy.AddChildObjectL( 
        KNSmlDMSIPTransport );
    regTransport.SetAccessTypesL( accessTypesGetReplaceAdd );
    regTransport.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    regTransport.SetScopeL( MSmlDmDDFObject::EDynamic );
    regTransport.SetDFFormatL( MSmlDmDDFObject::EChr );
    regTransport.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    regTransport.SetDescriptionL( KNSmlDMSIPTransportExp );

    // RegistrarServer/Username
    MSmlDmDDFObject& regUsername = regProxy.AddChildObjectL( 
        KNSmlDMSIPUsername );
    regUsername.SetAccessTypesL( accessTypesGetReplaceAdd );
    regUsername.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    regUsername.SetScopeL( MSmlDmDDFObject::EDynamic );
    regUsername.SetDFFormatL( MSmlDmDDFObject::EChr );
    regUsername.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    regUsername.SetDescriptionL( KNSmlDMSIPUsernameExp );

    // RegistrarServer/PrivateIdentity
    MSmlDmDDFObject& regPrivateIdentity = regProxy.AddChildObjectL( 
        KNSmlDMSIPPrivateIdentity );
    regPrivateIdentity.SetAccessTypesL( accessTypesGetReplaceAdd );
    regPrivateIdentity.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    regPrivateIdentity.SetScopeL( MSmlDmDDFObject::EDynamic );
    regPrivateIdentity.SetDFFormatL( MSmlDmDDFObject::EChr );
    regPrivateIdentity.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    regPrivateIdentity.SetDescriptionL( KNSmlDMSIPPrivateIdentityExp );

    // RegistrarServer/Passwd
    MSmlDmDDFObject& regPasswd = regProxy.AddChildObjectL( KNSmlDMSIPPasswd );
    regPasswd.SetAccessTypesL( accessTypesReplaceAdd );
    regPasswd.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    regPasswd.SetScopeL( MSmlDmDDFObject::EDynamic );
    regPasswd.SetDFFormatL( MSmlDmDDFObject::EChr );
    regPasswd.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    regPasswd.SetDescriptionL( KNSmlDMSIPPasswdExp );

    // RegistrarServer/Realm
    MSmlDmDDFObject& regRealm = regProxy.AddChildObjectL( KNSmlDMSIPRealm );
    regRealm.SetAccessTypesL( accessTypesGetReplaceAdd );
    regRealm.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    regRealm.SetScopeL( MSmlDmDDFObject::EDynamic );
    regRealm.SetDFFormatL( MSmlDmDDFObject::EChr );
    regRealm.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    regRealm.SetDescriptionL( KNSmlDMSIPRealmExp );

    // Default
    MSmlDmDDFObject& def = idNode.AddChildObjectL( KNSmlDMSIPDefault );
    def.SetAccessTypesL( accessTypesGetReplaceAdd );
    def.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    def.SetScopeL( MSmlDmDDFObject::EDynamic );
    def.SetDFFormatL( MSmlDmDDFObject::EBool );
    def.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    def.SetDescriptionL( KNSmlDMSIPDefaultExp );

    // LooseRouting
    MSmlDmDDFObject& lr = idNode.AddChildObjectL( KNSmlDMSIPLooseRouting );
    lr.SetAccessTypesL( accessTypesGetReplaceAdd );
    lr.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    lr.SetScopeL( MSmlDmDDFObject::EDynamic );
    lr.SetDFFormatL( MSmlDmDDFObject::EBool );
    lr.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    lr.SetDescriptionL( KNSmlDMSIPLooseRoutingExp );

    // EnableSecurityNegotiation
    MSmlDmDDFObject& enSecNeg = idNode.AddChildObjectL( 
        KNSmlDMSIPEnableSecurityNegotiation );
    enSecNeg.SetAccessTypesL( accessTypesGetReplaceAdd );
    enSecNeg.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    enSecNeg.SetScopeL( MSmlDmDDFObject::EDynamic );
    enSecNeg.SetDFFormatL( MSmlDmDDFObject::EBool );
    enSecNeg.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    enSecNeg.SetDescriptionL( KNSmlDMSIPEnableSecurityNegotiationExp );

    // EnableSigComp
    MSmlDmDDFObject& enSigComp = idNode.AddChildObjectL( 
        KNSmlDMSIPEnableSigComp );
    enSigComp.SetAccessTypesL( accessTypesGetReplaceAdd );
    enSigComp.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    enSigComp.SetScopeL( MSmlDmDDFObject::EDynamic );
    enSigComp.SetDFFormatL( MSmlDmDDFObject::EBool );
    enSigComp.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    enSigComp.SetDescriptionL( KNSmlDMSIPEnableSigCompExp );

    // EnableAutoRegistration
    MSmlDmDDFObject& enAutoReg = idNode.AddChildObjectL( 
        KNSmlDMSIPEnableAutoRegistration );
    enAutoReg.SetAccessTypesL( accessTypesGetReplaceAdd );
    enAutoReg.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    enAutoReg.SetScopeL( MSmlDmDDFObject::EDynamic );
    enAutoReg.SetDFFormatL( MSmlDmDDFObject::EBool );
    enAutoReg.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    enAutoReg.SetDescriptionL( KNSmlDMSIPEnableAutoRegistrationExp );

    // PrefConRef
    MSmlDmDDFObject& toNapId = idNode.AddChildObjectL( KNSmlDMSIPConRef );
    toNapId.SetAccessTypesL( accessTypesGetReplaceAdd );
    toNapId.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    toNapId.SetScopeL( MSmlDmDDFObject::EDynamic );
    toNapId.SetDFFormatL( MSmlDmDDFObject::EChr );
    toNapId.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    toNapId.SetDescriptionL( KNSmlDMSIPConRefExp );

    // ProfileLock
    MSmlDmDDFObject& profileLock = idNode.AddChildObjectL( 
        KNSmlDMSIPProfileLock );
    profileLock.SetAccessTypesL( accessTypesGetReplaceAdd );
    profileLock.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    profileLock.SetScopeL( MSmlDmDDFObject::EDynamic );
    profileLock.SetDFFormatL( MSmlDmDDFObject::EBool );
    profileLock.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    profileLock.SetDescriptionL( KNSmlDMSIPProfileLockExp );

    // DestinationNetwork
    MSmlDmDDFObject& toSnapId = idNode.AddChildObjectL( KNSmlDMSIPSnap );
    toSnapId.SetAccessTypesL( accessTypesGetReplaceAdd );
    toSnapId.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    toSnapId.SetScopeL( MSmlDmDDFObject::EDynamic );
    toSnapId.SetDFFormatL( MSmlDmDDFObject::EChr );
    toSnapId.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    toSnapId.SetDescriptionL( KNSmlDMSIPSnapExp );

    // SignalingQoS
    MSmlDmDDFObject& sigQos = idNode.AddChildObjectL( KNSmlDMSIPSigQos );
    sigQos.SetAccessTypesL( accessTypesGetReplaceAdd );
    sigQos.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    sigQos.SetScopeL( MSmlDmDDFObject::EDynamic );
    sigQos.SetDFFormatL( MSmlDmDDFObject::EInt );
    sigQos.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    sigQos.SetDescriptionL( KNSmlDMSIPSigQosExp );
    
    //Bearer ID
    MSmlDmDDFObject& bearerid = idNode.AddChildObjectL( KNSmlDMSIPBearerID );
    bearerid.SetAccessTypesL( accessTypesGetReplaceAdd );
    bearerid.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    bearerid.SetScopeL( MSmlDmDDFObject::EDynamic );
    bearerid.SetDFFormatL( MSmlDmDDFObject::EInt );
    bearerid.AddDFTypeMimeTypeL( KNSmlDMSIPTextPlain );
    bearerid.SetDescriptionL( KNSmlDMSIPBearerIDExp );
    

    DBG_PRINT("CNSmlDmSIPAdapter::DDFStructureL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::UpdateLeafObjectL
// Updates value of a leaf object
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::UpdateLeafObjectL( 
    const TDesC8& aURI, 
    const TDesC8& aLUID, 
    const TDesC8& aObject, 
    const TDesC8& /*aType*/, 
    const TInt aStatusRef )
    {
    DBG_PRINT("CNSmlDmSIPAdapter::UpdateLeafObjectL(): begin");

    // Load profile information if not loaded yet
    if ( !iProfiles )
        {
        LoadProfilesL();    
        }

    CSmlDmAdapter::TError status = EOk;
    TPtrC8 lastUriSeg = LastURISeg( aURI );
    TPtrC8 uriTmp = RemoveLastURISeg( aURI );
    TPtrC8 secondLastUriSeg = LastURISeg( uriTmp );
    TInt err ( 0 );

    // Check that luid mapping exist 
    if ( !aLUID.Length() > 0 )
        {
        iDmCallback->SetStatusL( aStatusRef,CSmlDmAdapter::EError );
        DBG_PRINT("CNSmlDmSIPAdapter::UpdateLeafObjectL(): no LUID, end");
        return;
        }

    // Find profile from array.
    TInt profileID = FindHexProfile ( aLUID );
    if ( KErrNotFound == profileID )
        {
        status = CSmlDmAdapter::ENotFound;
        iDmCallback->SetStatusL( aStatusRef, status );
        DBG_PRINT("CNSmlDmSIPAdapter::UpdateLeafObjectL(): ENotFound end");
        return;
        }

    // Check that profile going to be modified is not in use
    TBool used ( iSIPManagedProfileRegistry->IsInUseL ( 
        *iProfiles->At ( profileID )));
    if ( used )
        {
        status = CSmlDmAdapter::EObjectInUse;
        iDmCallback->SetStatusL( aStatusRef, status );
        DBG_PRINT("CNSmlDmSIPAdapter::UpdateLeafObjectL(): EObjectInUse end");
        return;
        }

    // Check that profile is not allready to be saved.
    if ( KErrNotFound == iModifications.Find( profileID ) )
        {
        iModifications.Insert( profileID, iModifications.Count() );
        }

    // ==============================
    // SIP/x/ node
    // ==============================
    //

    // SIP/x/Name    
    if ( KNSmlDMSIPName() == lastUriSeg )  
        {
        if ( aObject.Length() < KMaxProviderNameLength )
            {
            err = iProfiles->At( profileID )->SetParameter( 
                KSIPProviderName, aObject );
            CheckDuplicateNameL( *iProfiles->At( profileID ), profileID );
            }
        else
            {
            status = CSmlDmAdapter::EInvalidObject;
            }
        }

    // SIP/x/ProfileType      
    if ( KNSmlDMSIPProfileType() == lastUriSeg )  
        {
        RArray<TSIPProfileTypeInfo> profileTypes;
        CleanupClosePushL( profileTypes ); // CS:1

         // Get the supported profile types
        iSIPManagedProfileRegistry->SupportedProfileTypesL( profileTypes );

        TSIPProfileTypeInfo type;
        if ( KPTypeIMS() == aObject )
            {
            type.iSIPProfileClass = TSIPProfileTypeInfo::EIms;
            type.iSIPProfileName = KPTypeIMS;
            if ( KErrNotFound != profileTypes.Find( type ) )
                {
                iProfiles->At( profileID )->SetType( type ); 
                }
            else
                {
                status = CSmlDmAdapter::EInvalidObject;
                }
            }
        else if ( KPTypeIETF() == aObject )
            {
            type.iSIPProfileClass = TSIPProfileTypeInfo::EInternet;
            type.iSIPProfileName = KPTypeIETF;
            if ( KErrNotFound != profileTypes.Find( type ) )
                {
                iProfiles->At( profileID )->SetType( type ); 
                }
            else
                {
                status = CSmlDmAdapter::EInvalidObject;
                }
            }
        else if ( KPTypeOther() == aObject )
            {
            type.iSIPProfileClass = TSIPProfileTypeInfo::EOther;
            type.iSIPProfileName = KPTypeOther;
            if ( KErrNotFound != profileTypes.Find( type ) )
                {
                iProfiles->At( profileID )->SetType( type ); 
                }
            else
                {
                status = CSmlDmAdapter::EInvalidObject;
                }
            }
        else
            {
            status = CSmlDmAdapter::EInvalidObject;
            } 
        CleanupStack::PopAndDestroy( &profileTypes ); // CS:0
        }

    // SIP/x/AddressOfRecord      
    if ( KNSmlDMSIPAOR() == lastUriSeg )  
        {
        // Supports one AOR for profile.

        if ( KErrNotFound == aObject.Find( KEscaped() ) )
            {
            HBufC8* encoded = EscapeUtils::EscapeEncodeL( aObject,
                EscapeUtils::EEscapeNormal );
            err = iProfiles->At( profileID )->SetParameter( KSIPUserAor,
                *encoded );
            delete encoded;
            }
        else
            {
            err = iProfiles->At( profileID )->SetParameter( 
                KSIPUserAor, aObject );
            }
        }

    // SIP/x/Default
    // Only setting as True allowed. SIP stack sets other profiles as False.
    if ( KNSmlDMSIPDefault() == lastUriSeg )  
        {
        if ( KNSmlDMSIPValueTrue() == aObject )
            {
            err = iProfiles->At( profileID )->SetParameter( 
                KSIPDefaultProfile, ETrue );
            }
        else
            {
            status = CSmlDmAdapter::EInvalidObject;    
            } 
        }

    // SIP/x/LooseRouting
    if ( KNSmlDMSIPLooseRouting() == lastUriSeg )
        {
        // get OutboundProxy SIP uri.
        const TDesC8* uri = 0;
        iProfiles->At( profileID )->GetParameter( KSIPOutboundProxy, 
            KSIPServerAddress, uri );
        TUriParser8 parser;
        err = parser.Parse( *uri );

        TBuf8<KUriPathMaxLength> tempPathLr;

        CUri8* tempUri = CUri8::NewLC( parser );
        tempPathLr.Copy( tempUri->Uri().Extract( EUriPath ) );
        tempPathLr.LowerCase();


        if ( KNSmlDMSIPValueTrue() == aObject )
            {
            if ( KErrNotFound == tempPathLr.Find( KSIPlr ) )
                {
                tempPathLr.Append( KSIPlr );
                tempUri->SetComponentL( tempPathLr, EUriPath );
                }
            }

        else if ( KNSmlDMSIPValueFalse() == aObject )
            {
            TInt pos = tempPathLr.Find( KSIPlr );
            if ( KErrNotFound != pos )
                {
                tempPathLr.Delete( pos, KSIPlr().Length() );
                tempUri->SetComponentL( tempPathLr, EUriPath );
                }
            }
        else
            {
            status = CSmlDmAdapter::EInvalidObject;
            }

        // Set tempUri back to original sipURI if it validates.
        if ( status == CSmlDmAdapter::EOk )
            {
            // Set outboundproxy parameters according to sipURI.
            err = iProfiles->At( profileID )->SetParameter( KSIPOutboundProxy,
                KSIPServerAddress, tempUri->Uri().UriDes() );
            }
        else
            {
            status = CSmlDmAdapter::EInvalidObject;
            }
        CleanupStack::PopAndDestroy( tempUri );
        }

    // SIP/x/EnableSecurityNegotiation
    if ( KNSmlDMSIPEnableSecurityNegotiation() == lastUriSeg )
        {
        if ( KNSmlDMSIPValueTrue() == aObject )
            {
            err = iProfiles->At( profileID )->SetParameter( 
                KSIPSecurityNegotiation, ETrue );
            }
        else if ( KNSmlDMSIPValueFalse() == aObject )
            {
            err = iProfiles->At( profileID )->SetParameter( 
                KSIPSecurityNegotiation, EFalse );
            }
        else
            {
            status = CSmlDmAdapter::EInvalidObject;
            } 
        }

    // SIP/x/EnableSigComp
    if ( KNSmlDMSIPEnableSigComp() == lastUriSeg )
        {
        if ( aObject == KNSmlDMSIPValueTrue )
            {
            err = iProfiles->At( profileID )->SetParameter( 
                KSIPSigComp, ETrue );
            }
        else if ( aObject == KNSmlDMSIPValueFalse )
            {
            err = iProfiles->At( profileID )->SetParameter( 
                KSIPSigComp, EFalse );
            }
        else
            {
            status = CSmlDmAdapter::EInvalidObject;
            } 
        }

    // SIP/x/EnableAutoRegistration
    if ( KNSmlDMSIPEnableAutoRegistration() == lastUriSeg )
        {
        if ( KNSmlDMSIPValueTrue() == aObject )
            {
            err = iProfiles->At( profileID )->SetParameter( 
                KSIPAutoRegistration, ETrue );
            }
        else if ( KNSmlDMSIPValueFalse() == aObject )
            {
            err = iProfiles->At( profileID )->SetParameter( 
                KSIPAutoRegistration, EFalse );
            }
        else
            {
            status = CSmlDmAdapter::EInvalidObject;
            } 
        }

    // SIP/x/PrefConRef
    if ( KNSmlDMSIPConRef() == lastUriSeg )
        {
        TInt lIAPId = SetConRefL( aObject, profileID ); // IAPid from CommsDB.
        if ( lIAPId != KErrNotFound )
            {
            status = CSmlDmAdapter::EOk;       
            }
        else
            {
            status = CSmlDmAdapter::EInvalidObject;
            }
        } 

    // SIP/x/ProfileLock
    if ( KNSmlDMSIPProfileLock() == lastUriSeg )
        {
        if ( KNSmlDMSIPValueTrue() == aObject )
            {
            err = iProfiles->At( profileID )->SetParameter( 
                KSIPProfileLocked, ETrue );
            }
        else if ( KNSmlDMSIPValueFalse() == aObject )
            {
            err = iProfiles->At( profileID )->SetParameter( 
                KSIPProfileLocked, EFalse );
            }
        else
            {
            status = CSmlDmAdapter::EInvalidObject;
            } 
        }

    // SIP/x/DestinationNetwork
    if ( KNSmlDMSIPSnap() == lastUriSeg )
        {
        TUint32 snapId = SnapIdL( aObject );
        if ( snapId )
            {
            err = iProfiles->At( profileID )->SetParameter( 
                KSIPSnapId, snapId );
            }
        else
            {
            status = CSmlDmAdapter::EInvalidObject;
            }
        }
    // SIP/x/SignalingQoS
    if ( KNSmlDMSIPSigQos() == lastUriSeg )
        {
        TUint32 sigQos = DesToInt( aObject );
        if ( sigQos <= KMaxSigQosValue )
            {
            // We need to do bitshifting on the IP TOS, because it's the
            // upper 6 bits that are set and settings provide us the IP TOS as
            // the lower 6 bits. The lower 2 bits are reserver for explicit
            // congestion notification.
            // See also more from:
            // Symbian DevLib / in_sock.h Global variables / KSoIpTOS
            sigQos <<= 2;
            err = iProfiles->At( profileID )->SetParameter( 
                KSIPSoIpTOS, sigQos );
            }
        else
            {
            status = CSmlDmAdapter::EInvalidObject;
            }
        }

    // SIP/x/BearerId
    if ( KNSmlDMSIPBearerID() == lastUriSeg )
        {
        TUint32 bearerid = DesToInt( aObject );
        if ( bearerid )
            {
            err = iProfiles->At( profileID )->SetParameter( 
                    KBearerType, bearerid );
            }
        else
            {
            status = CSmlDmAdapter::EInvalidObject;
            }
        }
    
    
    // ==============================
    // OutboundProxy settings node
    // ==============================
    //

    // SIP/x/OutboundProxy
    if ( KNSmlDMSIPOutboundProxy() == secondLastUriSeg )  
        {
        status = UpdateOutboundProxyNodeL( profileID, lastUriSeg, aObject );
        }

    // ==============================
    // RegistrarServer settings node
    // ==============================
    //
    
    // SIP/x/RegistrarServer
    if ( KNSmlDMSIPRegistrarServer() == secondLastUriSeg )  
        {
        status = UpdateRegistrarServerNodeL( profileID, lastUriSeg, aObject );
        }

    if ( err )
        {
        status = CSmlDmAdapter::EError;
        }

    iDmCallback->SetStatusL( aStatusRef, status );
    DBG_PRINT("CNSmlDmSIPAdapter::UpdateLeafObjectL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::DeleteObjectL
// Deletes SIP profiles.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::DeleteObjectL( 
    const TDesC8& aURI, 
    const TDesC8& aLUID, 
    const TInt aStatusRef )
    {
    DBG_PRINT("CNSmlDmSIPAdapter::DeleteObjectL(): begin");

     // Load profile information if not loaded yet
    if ( !iProfiles )
        {
        LoadProfilesL();
        }

    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
    TBuf8 <KNSmlSIPMaxUriLength> uri = aURI;

    // Copy LUID as last segment if exists.
    if ( 0 != aLUID.Length() )
        {
        uri.Copy( aLUID );
        }
    else 
        {
        retValue = CSmlDmAdapter::ENotFound;
        }
        
    if ( aLUID.Length() > 0 && NumOfURISegs( aURI ) == 2 )
        {
        TInt profileID = FindHexProfile( uri );

        if ( KErrNotFound == profileID )
            {
            retValue = CSmlDmAdapter::ENotFound;
            iDmCallback->SetStatusL( aStatusRef, retValue );
            DBG_PRINT("CNSmlDmSIPAdapter::DeleteObjectL(): ENotFound end");
            return;
            }

        // Delete allowed only if profile is not registered.
        TBool registered( EFalse );
        iProfiles->At( profileID )->GetParameter( 
            KSIPProfileRegistered, registered );

        if ( !registered )
            {
            // Delete profile from permanent store.
            iSIPManagedProfileRegistry->DestroyL( 
                *iProfiles->At( profileID ) );

            // Delete profile also from memory and compress array.
            delete iProfiles->At( profileID );
            iProfiles->Delete( profileID );
            iProfiles->Compress();
            retValue = CSmlDmAdapter::EOk;
            }
        else
            {
            retValue = CSmlDmAdapter::EObjectInUse;
            }
        }
    else
        {
        retValue = CSmlDmAdapter::EInvalidObject;
        }
    iDmCallback->SetStatusL( aStatusRef, retValue );
    DBG_PRINT("CNSmlDmSIPAdapter::DeleteObjectL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::FetchLeafObjectL
// Fetches the values of leaf objects.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::FetchLeafObjectL( 
    const TDesC8& aURI, 
    const TDesC8& aLUID, 
    const TDesC8& aType, 
    const TInt aResultsRef, 
    const TInt aStatusRef )
    {
    DBG_PRINT("CNSmlDmSIPAdapter::FetchLeafObjectL(): begin");

    // Load profile information if not loaded yet.
    if ( !iProfiles )
        {
        LoadProfilesL();
        }

    CBufBase *result = CBufFlat::NewL( KDefaultResultSize );
    CleanupStack::PushL( result );
    CSmlDmAdapter::TError status = FetchObjectL( aURI, aLUID, *result );

    if ( status == CSmlDmAdapter::EOk )
        {
        iDmCallback->SetResultsL( aResultsRef, *result, aType );
        }
    iDmCallback->SetStatusL( aStatusRef, status );

    CleanupStack::PopAndDestroy( result );
    DBG_PRINT("CNSmlDmSIPAdapter::FetchLeafObjectL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::FetchObjectL
// Fetches the values of leaf objects.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CSmlDmAdapter::TError CNSmlDmSIPAdapter::FetchObjectL( 
    const TDesC8& aURI, 
    const TDesC8& aLUID,
    CBufBase& aResult )
    {
    DBG_PRINT("CNSmlDmSIPAdapter::FetchObjectL(): begin");
    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;

    // Check that luid mapping exist 
    if ( !aLUID.Length() > 0 )
        {
        DBG_PRINT("CNSmlDmSIPAdapter::FetchObjectL: ENotFound end");
        return CSmlDmAdapter::ENotFound;
        }

    TBuf8<KNSmlSIPMaxResultLength> segmentResult;
    TInt err ( 0 );

    const TDesC8* paramResult = 0;
    TBool boolVal ( EFalse );

    TPtrC8 lastUriSeg = LastURISeg( aURI );
    TPtrC8 uriTmp = RemoveLastURISeg( aURI );
    TPtrC8 secondLastUriSeg = LastURISeg( uriTmp );

    // Find profile from iProfiles. If not found, set error.
    TInt profileID = FindHexProfile ( aLUID );
    if ( KErrNotFound == profileID )
        {
        retValue = CSmlDmAdapter::ENotFound;
        DBG_PRINT("CNSmlDmSIPAdapter::FetchObjectL: ENotFound end");
        return retValue;
        }

    // SIP/x/SIPId
    if ( KNSmlDMSIPSipID() == lastUriSeg ) 
        {
        TUint32 val( 0 );
        err = iProfiles->At( profileID )->GetParameter( KSIPProfileId, val );
        segmentResult.Num( val );
        }

    // SIP/x/Provider
    if ( KNSmlDMSIPName() == lastUriSeg )
        {
        err = iProfiles->At( profileID )->
            GetParameter( KSIPProviderName, paramResult );
        segmentResult.Copy( *paramResult );
        }

    // SIP/x/Type
    if ( KNSmlDMSIPProfileType() == lastUriSeg )
        {
        TSIPProfileTypeInfo type;
        type = iProfiles->At( profileID )->Type();
        if ( type.iSIPProfileClass == TSIPProfileTypeInfo::EIms )
            {
            segmentResult.Copy( KPTypeIMS );
            }
        if ( type.iSIPProfileClass == TSIPProfileTypeInfo::EInternet )
            {
            segmentResult.Copy( KPTypeIETF );
            }
        if ( type.iSIPProfileClass == TSIPProfileTypeInfo::EOther )
            {
            segmentResult.Copy( KPTypeOther );
            }
        }

    // SIP/x/ProfileLock
    if ( KNSmlDMSIPProfileLock() == lastUriSeg )
        {
        TBool boolVal( EFalse );
        err = iProfiles->At( profileID )->GetParameter( 
            KSIPProfileLocked, boolVal );
        if ( boolVal )
            {
            segmentResult.Copy( KNSmlDMSIPValueTrue );
            }
        else
            {
            segmentResult.Copy( KNSmlDMSIPValueFalse );
            } 
        }

    // SIP/x/AddressOfRecord
    if ( KNSmlDMSIPAOR() == lastUriSeg ) 
        {
        // Supports one AOR for profile.
        err = iProfiles->At( profileID )->GetParameter( 
            KSIPUserAor, paramResult );
        segmentResult.Copy( *paramResult );
        }

    // SIP/x/OutboundProxy
    if ( KNSmlDMSIPOutboundProxy() == secondLastUriSeg )  
        {
        retValue = FetchOutboundProxyNodeL( profileID, lastUriSeg, 
            segmentResult );
        } 

    // SIP/x/RegistrarServer
    if ( KNSmlDMSIPRegistrarServer() == secondLastUriSeg )
        {
        retValue = FetchRegistrarServerNodeL( profileID, lastUriSeg, 
            segmentResult );
        }  

    // SIP/x/Default
    if ( KNSmlDMSIPDefault() == lastUriSeg )
        {
        err = iProfiles->At( profileID )->GetParameter( 
            KSIPDefaultProfile, boolVal );
        if ( boolVal )
            {
            segmentResult.Copy( KNSmlDMSIPValueTrue );
            }
        else
            {
            segmentResult.Copy( KNSmlDMSIPValueFalse );
            }
        }

    // SIP/x/LooseRouting
    if ( KNSmlDMSIPLooseRouting() == lastUriSeg )
        {
        // Get OutboundProxy SIP uri.
        const TDesC8* uri = 0;
        err = iProfiles->At( profileID )->GetParameter( 
            KSIPOutboundProxy, KSIPServerAddress, uri );

        CSIPAddress* sipURI = 0;
        if ( 0 < uri->Length() )
            {
            sipURI = CSIPAddress::DecodeL( *uri );
            }
        // The whole SIP URI is empty so return ENotFound.
        else
            {
            retValue = CSmlDmAdapter::ENotFound;
            DBG_PRINT("CNSmlDmSIPAdapter::FetchObjectL: No SIP URI end");
            return retValue;
            }

        CleanupStack::PushL( sipURI );
        TBuf8<KUriPathMaxLength> tempPathLr;
        tempPathLr.Copy( sipURI->Uri8().Uri().Extract( EUriPath ) );
        tempPathLr.LowerCase();

        if ( KErrNotFound != sipURI->Uri8().Uri().Extract( EUriPath ).Find( 
            KSIPlr ) )
            {
            segmentResult.Copy( KNSmlDMSIPValueTrue );
            }
        else
            {
            segmentResult.Copy( KNSmlDMSIPValueFalse );
            }
        CleanupStack::PopAndDestroy( sipURI );
        }

    // SIP/x/EnableSecurityNegotiation
    if ( KNSmlDMSIPEnableSecurityNegotiation() == lastUriSeg )
        {
        err = iProfiles->At( profileID )->GetParameter( 
            KSIPSecurityNegotiation, boolVal );
        if ( boolVal )
            {
            segmentResult.Copy( KNSmlDMSIPValueTrue );
            }
        else
            {
            segmentResult.Copy( KNSmlDMSIPValueFalse );
            }
        }

    // SIP/x/EnableSigComp
    if ( KNSmlDMSIPEnableSigComp() == lastUriSeg )
        {
        err = iProfiles->At( profileID )->GetParameter( 
            KSIPSigComp, boolVal );
        if ( boolVal )
            {  
            segmentResult.Copy( KNSmlDMSIPValueTrue );
            }
        else
            {
            segmentResult.Copy( KNSmlDMSIPValueFalse );
            }
        }

    // SIP/x/AutoRegistration
    if ( KNSmlDMSIPEnableAutoRegistration() == lastUriSeg )
        {
        err = iProfiles->At( profileID )->GetParameter( 
            KSIPAutoRegistration, boolVal );
        if ( boolVal )
            {  
            segmentResult.Copy( KNSmlDMSIPValueTrue );
            }
        else
            {
            segmentResult.Copy( KNSmlDMSIPValueFalse );
            }
        }

    // SIP/x/ConRef
    if ( KNSmlDMSIPConRef() == lastUriSeg ) 
        {
        TBool conref = GetConRefL( aResult, profileID );
        if ( !conref )
            {
            aResult.Reset();
            retValue = CSmlDmAdapter::ENotFound;
            DBG_PRINT("CNSmlDmSIPAdapter::FetchObjectL(): \
PrefConRef not found end");
            return retValue;
            }
        }

    // SIP/x/DestinationNetwork
    if ( KNSmlDMSIPSnap() == lastUriSeg )
        {
        TUint32 snapId( KErrNone );
        err = iProfiles->At( profileID )->GetParameter( 
            KSIPSnapId, snapId );
        if ( snapId )
            {
            TBuf8<KNSmlSIPMaxUriLength> snapUri;
            retValue = GetSnapUriL( snapUri, snapId );
            if ( CSmlDmAdapter::EOk == retValue )
                {
                segmentResult.Copy( snapUri );
                }
            else if ( CSmlDmAdapter::ENotFound == retValue )
                {
                DBG_PRINT( "CNSmlDmSIPAdapter::FetchObjectL(): \
SNAP not found end" );
                return retValue;
                }
            }
        }

    // SIP/x/SignalingQoS
    if ( KNSmlDMSIPSigQos() == lastUriSeg )
        {
        TUint32 sigQos;
        err = iProfiles->At( profileID )->GetParameter(
            KSIPSoIpTOS, sigQos );
        // We need to do bitshifting on the IP TOS, because it's the upper 6
        // bits that are set and settings provide us the IP TOS as the lower 6
        // bits. The lower 2 bits are reserver for explicit congestion
        // notification.
        // See also more from:
        // Symbian Developer Library / in_sock.h Global variables / KSoIpTOS
        sigQos >>= 2;
        segmentResult.Num( sigQos );
        }
    
    // SIP/x/BearerId
       if ( KNSmlDMSIPBearerID() == lastUriSeg )
           {
           TUint32 bearerid;
           err = iProfiles->At( profileID )->GetParameter(
                   KBearerType, bearerid );
           segmentResult.Num( bearerid );
           }
    

    // Set error if fetch failed.
    if ( -1 > err )
        {
        retValue = CSmlDmAdapter::EError;
        }

    if ( EOk == retValue )
        {
        aResult.InsertL( aResult.Size(), segmentResult );
        }

    DBG_PRINT("CNSmlDmSIPAdapter::FetchObjectL(): end");
    return retValue;
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::FindHexProfile
// Parses profileID from aURI and returns it in TInt
// Returns: TInt: profile location in profile array
// ---------------------------------------------------------------------------
//
TInt CNSmlDmSIPAdapter::FindHexProfile( 
    const TDesC8& aURI )
    {
    TInt id (0);
    id = aURI.Find( KSIPHexPrefix  );
    if ( id != KErrNotFound )
        {
        TLex8 lexer ( aURI.Mid( id + KSIPPrefix().Length() + 2, 8 ) );
        TRadix radix = EHex;
        TUint32 tempVal( 0 );
        if ( KErrNone == lexer.Val ( tempVal, radix )  )
       	   {
       	   for ( TInt counter = 0 ; counter < iProfiles->Count() ; counter++ )
                {
	            TUint32 val ( 0 );	
	            iProfiles->At( counter )->GetParameter( KSIPProfileId, val );
	            if ( tempVal == val )
	                {
	                return counter;
	                }
	            }
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::ChildURIListL
// Asks for the list of children objects of the node
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void  CNSmlDmSIPAdapter::ChildURIListL( 
    const TDesC8& aURI,
    const TDesC8& aLUID, 
    const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
    const TInt aResultsRef, 
    const TInt aStatusRef ) 
    {
    DBG_PRINT( "CNSmlDmSIPAdapter::ChildURIListL(): start" );

    // Load profile information if not loaded yet.
    if ( !iProfiles )
        {
        LoadProfilesL();
        }

    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;

    CBufBase *currentURISegmentList = CBufFlat::NewL( KDefaultResultSize );
    CleanupStack::PushL( currentURISegmentList );

    TBuf8 <KNSmlSIPMaxUriLength> uri = aURI;

    // Use LUID mapping if exist.
    if ( 0 < aLUID.Length() )
        {
        uri.Copy( aLUID );
        } 

    // Get last URI segment. 
    TBuf8< KSmlMaxURISegLen > segment = LastURISeg( aURI );
    TBuf8< KSmlMaxURISegLen > segmentName;

    TInt howManyObjects = iProfiles->Count();

    if ( KNSmlDMSIPNodeName() == segment ) // ./SIP
        {
        for ( TInt counter = 0; counter < howManyObjects ; counter++ )
            {
            TBool notInList = ETrue;
            TInt currentline = 0;
            TUint32 ddId ( 0 );
            iProfiles->At( counter )->GetParameter( KSIPProfileId, ddId );
            while ( notInList && 
                currentline < aPreviousURISegmentList.Count() )
                {
                TInt profileLocation = FindHexProfile( 
                    aPreviousURISegmentList.At( currentline ).iURISegLUID );
                TUint32 profileID ( 0 );
                if ( profileLocation != KErrNotFound )
                    {
                    iProfiles->At( profileLocation )->GetParameter( 
                        KSIPProfileId, profileID );
                    }
                if ( profileID == ddId )
                    {
                	notInList = EFalse;
                	break;
                	}
                currentline++;
                }
            if ( notInList )
                {
                // SIP/x
                TUint32 val ( 0 );
                iProfiles->At( counter )->GetParameter( KSIPProfileId, val );
                segmentName.Copy( KSIPPrefix );
                segmentName.Append( KHexPrefix );
                segmentName.AppendNumFixedWidthUC( val, EHex, 8 );
                currentURISegmentList->InsertL( currentURISegmentList->
                    Size(), segmentName );
                currentURISegmentList->InsertL( currentURISegmentList->
                    Size(), KNSmlDMSIPSeparator8 );

                // Set LUID mapping to DM module.
                TBuf8<KMaxMappingLength> mappingInfo;
                mappingInfo.Copy ( KSIPPrefix );
                mappingInfo.Append ( KHexPrefix );
                mappingInfo.AppendNumFixedWidthUC( val, EHex, 8 );
                segmentName.Copy( KNSmlDMSIPNodeName );
                segmentName.Append ( KNSmlDMSIPSeparator8 );
                segmentName.Append ( KSIPPrefix );
                segmentName.Append( KHexPrefix );
                segmentName.AppendNumFixedWidthUC( val, EHex, 8 );
                iDmCallback->SetMappingL( segmentName, mappingInfo );
                }
            else
                { // Add those in PreviousList which are on database.
                currentURISegmentList->InsertL( currentURISegmentList->Size(),
                    aPreviousURISegmentList.At(currentline).iURISeg );
                currentURISegmentList->InsertL( currentURISegmentList->Size(),
                    KNSmlDMSIPSeparator8 );
                }
            }
        retValue = CSmlDmAdapter::EOk;
        iDmCallback->SetStatusL( aStatusRef, retValue );
        iDmCallback->SetResultsL( aResultsRef, *currentURISegmentList, 
            KNullDesC8 );
        CleanupStack::PopAndDestroy( currentURISegmentList ); 
        currentURISegmentList = NULL;
        DBG_PRINT("CNSmlDmSIPAdapter::ChildURIListL(): end");
        return;
        }

    // Find profile from array. Return if not found.
    TInt profileID = FindHexProfile ( uri );
    if ( KErrNotFound == profileID )
        {
        retValue = CSmlDmAdapter::ENotFound;
        CleanupStack::PopAndDestroy( currentURISegmentList );
        iDmCallback->SetStatusL( aStatusRef, retValue );
        DBG_PRINT("CNSmlDmSIPAdapter::ChildURIListL(): end");
        return;
        }

    // SIP/SIPId
    if ( 2 == NumOfURISegs( aURI ) )
        {
        segmentName.Copy( KNSmlDMSIPSipID );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPProfileType );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPAOR );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPOutboundProxy );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPRegistrarServer );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPDefault );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPLooseRouting );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPEnableSecurityNegotiation );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );
        
        segmentName.Copy( KNSmlDMSIPEnableSigComp );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPEnableAutoRegistration );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPConRef );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );
        
        segmentName.Copy( KNSmlDMSIPProfileLock );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPSnap );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPSigQos );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );
        
        segmentName.Copy( KNSmlDMSIPBearerID );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );
        }

    // SIP/x/OutboundProxy
    else if ( KNSmlDMSIPOutboundProxy() == LastURISeg( aURI ) ) 
        {
        segmentName.Copy( KNSmlDMSIPHost );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPPort );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPTransport );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPUsername );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPPasswd );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPRealm );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );
        }
    
    // SIP/x/RegistrarServer
    else if ( KNSmlDMSIPRegistrarServer() == LastURISeg( aURI ) )
        { 
        segmentName.Copy( KNSmlDMSIPHost );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 ); 

        segmentName.Copy( KNSmlDMSIPPort );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPTransport );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPUsername );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPPrivateIdentity );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPPasswd );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );

        segmentName.Copy( KNSmlDMSIPRealm );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMSIPSeparator8 );
        }
    else 
        {
        // If none of asked nodes found return error.
        retValue = CSmlDmAdapter::ENotFound;
        }
   
    iDmCallback->SetStatusL( aStatusRef, retValue );
    iDmCallback->SetResultsL( aResultsRef, *currentURISegmentList, 
        KNullDesC8 );
    CleanupStack::PopAndDestroy( currentURISegmentList );
    DBG_PRINT("CNSmlDmSIPAdapter::ChildURIListL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::CheckDuplicateNameL
// Checks if duplicate named sip profiles. Renames if same.
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::CheckDuplicateNameL( CSIPManagedProfile& aProfile, 
    const TInt& aProfileId )
    {
    DBG_PRINT("CNSmlDmSIPAdapter::CheckDuplicateNameL(): begin");
    const TDesC8* name;
    aProfile.GetParameter( KSIPProviderName, name );
    HBufC8* newName = HBufC8::NewLC( KMaxProviderNameLength );
    newName->Des().Copy( *name );

    TInt count( iProfiles->Count() );

    for ( TInt n = 0; n < count; n++ )
        {
        // Skip profile currently being checked.
        if ( aProfileId != n )
            {
            const TDesC8* existingName;
            iProfiles->At( n )->GetParameter( KSIPProviderName, 
                existingName );
            if ( existingName->Compare( *name )  == 0 )
                {
                TBool isUnique( EFalse );
                for ( TInt i = 1; !isUnique; i++ )
                    {
                    TBool found( EFalse );
                    newName->Des().Zero();
                    newName->Des().Append( *name );
                    newName->Des().Append( KOpenParenthesis() );
                    newName->Des().AppendNum( i );
                    newName->Des().Append( KClosedParenthesis() );
                    for ( TInt m = 0; m < count; m++ )
                        {
                        iProfiles->At( m )->GetParameter( 
                            KSIPProviderName, existingName );
                        if ( existingName->Compare( newName->Des() ) == 0 )
                            {
                            found = ETrue;
                            }
                        }
                    if ( !found )
                        {
                        isUnique = ETrue;
                        }
                    }
                }
            }
        }

    aProfile.SetParameter( KSIPProviderName, newName->Des() );
    CleanupStack::PopAndDestroy( newName );
    DBG_PRINT("CNSmlDmSIPAdapter::CheckDuplicateNameL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::AddNodeObjectL
// Adds a new SIP profile
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::AddNodeObjectL( 
    const TDesC8& aURI, 
    const TDesC8& aParentLUID, 
    const TInt aStatusRef )
    {
    DBG_PRINT("CNSmlDmSIPAdapter::AddNodeObjectL(): begin");
    CSmlDmAdapter::TError status = EOk;

    // Load profile information if not loaded yet.
    if ( !iProfiles )
        {
        LoadProfilesL();
        }

    // Find profile from array, profile deleted from elsewhere if not found.
    if ( aParentLUID.Length() > 0 )
        {
        TInt profileID = FindHexProfile ( aParentLUID );
        if ( KErrNotFound == profileID )
            {
            status = CSmlDmAdapter::ENotFound;
            }
        else
            {
            status = CSmlDmAdapter::EAlreadyExists;
            }
        }

    if ( NumOfURISegs( aURI ) == 2 
        && ( status == EOk || status == ENotFound ) )
        {
        status = EOk;
        // Check OOD before saving new profile.
        RFs fs;
        User::LeaveIfError( fs.Connect() );
        CleanupClosePushL( fs );

        // SIP settings are stored in one file
        if ( SysUtil::FFSSpaceBelowCriticalLevelL( 
            &fs, KNSmlDMFatMinimumFileSize ) )
            {
            // Prevent writing in OOD.
            CleanupStack::PopAndDestroy( &fs );
            iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EDiskFull );
            DBG_PRINT("CNSmlDmSIPAdapter::AddNodeObjectL(): DISK FULL end");
            return;
            }
        CleanupStack::PopAndDestroy( &fs );
        TSIPProfileTypeInfo type;
        type.iSIPProfileClass = TSIPProfileTypeInfo::EInternet;
        type.iSIPProfileName = KPTypeIETF;

        // Need to create a new profile.
        CSIPManagedProfile* profile = iSIPManagedProfileRegistry->CreateL( 
            type );
        CleanupStack::PushL( profile );
        if ( LastURISeg( aURI ).Length() < KMaxProviderNameLength )
            {
            profile->SetParameter( KSIPProviderName, LastURISeg( aURI ) );
            }

        // Check for name duplicates, this profile has not yet been added to
        // iProfiles.
        CheckDuplicateNameL( *profile, KErrNotFound );

        // Default value for Signaling QoS is 40.
        TUint32 sigQos = 40;
        // We need to do bitshifting on the IP TOS, because it's the upper 6
        // bits that are set and settings provide us the IP TOS as the lower 6
        // bits.
        // The lower 2 bits are reserver for explicit congestion notification.
        // See also more from:
        // Symbian Developer Library / in_sock.h Global variables / KSoIpTOS
        sigQos <<= 2;
        profile->SetParameter( KSIPSoIpTOS, sigQos );

        // Add profile to iProfiles and save it also to permanent storage.
        iSIPManagedProfileRegistry->SaveL( *profile );
        iProfiles->AppendL( profile );

        // Set LUID mapping to DM module.
        TUint32 val ( 0 );
        profile->GetParameter( KSIPProfileId, val );
        TBuf8<KSmlMaxURISegLen> mappingInfo;
        mappingInfo.Copy ( KSIPPrefix );
        mappingInfo.Append ( KHexPrefix );
        mappingInfo.AppendNumFixedWidthUC( val, EHex, 8 );
        CleanupStack::Pop( profile ); 
        iDmCallback->SetMappingL( aURI, mappingInfo );
        }

    // Allow add for existing OutboundProxy and RegistrarServer nodes.
    else if ( aParentLUID.Length() > 0 && NumOfURISegs( aURI ) == 3 
        && ( LastURISeg( aURI ) == KNSmlDMSIPOutboundProxy 
        || LastURISeg( aURI ) == KNSmlDMSIPRegistrarServer ) )
        {
        status = CSmlDmAdapter::EOk;
        }

    iDmCallback->SetStatusL( aStatusRef, status );
    DBG_PRINT("CNSmlDmSIPAdapter::AddNodeObjectL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::DesToInt
// Converts a 8 bit descriptor to int.
// Returns: TInt: 
// ---------------------------------------------------------------------------
//
TInt CNSmlDmSIPAdapter::DesToInt( const TDesC8& aDes )
    {
    TLex8 lex( aDes );
    TInt value = 0;
    lex.Val( value );
    return value;
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::RemoveLastURISeg
// returns parent uri, i.e. removes last uri segment
// Returns: TPtrC16: pointer to aURI before last / mark
// ---------------------------------------------------------------------------
//
TPtrC8 CNSmlDmSIPAdapter::RemoveLastURISeg( const TDesC8& aURI )
    {
    TInt i ( 0 );
    // Check that aURI exists.
    if ( 0 < aURI.Length() )
        {
        for ( i = aURI.Length() - 1; i >= 0; i-- )
            {
            if ( KDMSIPSeparator == aURI[i] )
                {
                break;
                }
            }
        }
		
	if(i < 0)
		return aURI.Left(i+1);
	else	
		return aURI.Left( i );
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::NumOfURISegs
// For getting the number of uri segs
// Returns: TInt: number of uri segments
// ---------------------------------------------------------------------------
//
TInt CNSmlDmSIPAdapter::NumOfURISegs( const TDesC8& aURI )
    {
    TInt numOfURISegs ( 1 );
    for ( TInt i = 0; i < aURI.Length(); i++ )
        {
        if ( KDMSIPSeparator == aURI[i] )
            {
            numOfURISegs++;
            }
        }
    return numOfURISegs;
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::LastURISeg
// Returns only the last uri segment
// Returns: TPtrC16: pointer to last uri segment
// ---------------------------------------------------------------------------
//
TPtrC8 CNSmlDmSIPAdapter::LastURISeg( const TDesC8& aURI )
    {
    TInt i ( 0 );
    // Check that aURI exists
    if ( 0 < aURI.Length() )
        {
        for ( i = aURI.Length() - 1; i >= 0; i-- )
            {
            if ( KDMSIPSeparator == aURI[i] )
                {
                break;
                }
            }
        }
    if ( 0 == i  )
        {
        return aURI;
        }
    else
        {
        return aURI.Mid( i+1 );
        }
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::RemoveDotSlash
// return uri without dot and slash in start
// Returns: TPtrC8: pointer to last uri segment
// ---------------------------------------------------------------------------
//
TPtrC8 CNSmlDmSIPAdapter::RemoveDotSlash( const TDesC8& aURI )
    {
    if ( 0 == aURI.Find( KNSmlSIPUriDotSlash ) )
        {
        return aURI.Right( aURI.Length()-KNSmlSIPUriDotSlash().Length() );
        }
    else
        {
        return aURI;
        }
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::GetConRefL
// Get URI for given accesspoint ID.
// Returns: TBool: True if connection reference found
// ---------------------------------------------------------------------------
//
TBool CNSmlDmSIPAdapter::GetConRefL( CBufBase& aObject, 
    const TInt& aProfileid )
    {
    DBG_PRINT("CNSmlDmSIPAdapter::GetConRefL() : begin");
    TBool found = EFalse;

    CNSmlDMIAPMatcher* iapmatch = CNSmlDMIAPMatcher::NewL( iDmCallback );
    CleanupStack::PushL( iapmatch );

    // Get current access point ID.
    TUint32 profIAPid;
    iProfiles->At( aProfileid )->GetParameter( 
            KSIPAccessPointId, profIAPid );
        
    HBufC8* uri8 = iapmatch->URIFromIAPIdL( profIAPid );
    CleanupStack::PushL( uri8 );
    if ( uri8 )
        {
        aObject.InsertL( aObject.Size(),uri8->Des() );
        found = ETrue;
        }
    CleanupStack::PopAndDestroy( uri8 );
    CleanupStack::PopAndDestroy( iapmatch );
    DBG_PRINT("CNSmlDmSIPAdapter::GetConRefL() : end");
    return found;
    }
    
// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::SetConRefL
// Set conref value.
// Status : Draft/Proposal/Approved
// ---------------------------------------------------------------------------
//
TInt CNSmlDmSIPAdapter::SetConRefL( const TDesC8& aObject, 
    const TInt& aProfileid )
    {
    DBG_PRINT("CNSmlDmSIPAdapter::SetConRefL() : begin");
    CNSmlDMIAPMatcher* iapmatch = CNSmlDMIAPMatcher::NewL( iDmCallback );
    CleanupStack::PushL( iapmatch );
    TInt lIAPid = iapmatch->IAPIdFromURIL( aObject );
    if ( lIAPid != KErrNotFound )
        {
        iProfiles->At( aProfileid )->SetParameter( 
            KSIPAccessPointId, ( TUint32 )lIAPid );
        }
    CleanupStack::PopAndDestroy( iapmatch );
    DBG_PRINT("CNSmlDmSIPAdapter::SetConRefL() : end");
    return lIAPid;
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::ExecuteCommandL
// (Not supported, see smldmadapter.h)
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::ExecuteCommandL( 
    const TDesC8& /*aURI*/, 
    const TDesC8& /*aLUID*/, 
    const TDesC8& /*aArgument*/, 
    const TDesC8& /*aType*/, 
    const TInt aStatusRef )
    {
    DBG_PRINT("CNSmlDmSIPAdapter::ExecuteCommandL(): begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    DBG_PRINT("CNSmlDmSIPAdapter::ExecuteCommandL(): end");
    }

// ---------------------------------------------------------------------------
//  CNSmlDmSIPAdapter::ExecuteCommandL
// (Not supported, see smldmadapter.h)
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::ExecuteCommandL( 
    const TDesC8& /*aURI*/, 
    const TDesC8& /*aParentLUID*/, 
    RWriteStream*& /*aStream*/, 
    const TDesC8& /*aType*/, 
    const TInt aStatusRef )
    {
    DBG_PRINT("CNSmlDmSIPAdapter::ExecuteCommandL(): stream: begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    DBG_PRINT("CNSmlDmSIPAdapter::ExecuteCommandL(): stream: end");
    }

// ---------------------------------------------------------------------------
//  CNSmlDmSIPAdapter::CopyCommandL
// (Not supported, see smldmadapter.h)
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::CopyCommandL( 
    const TDesC8& /*aTargetURI*/, 
    const TDesC8& /*aTargetLUID*/, 
    const TDesC8& /*aSourceURI*/, 
    const TDesC8& /*aSourceLUID*/, 
    const TDesC8& /*aType*/, 
    TInt aStatusRef )
    {
    DBG_PRINT("CNSmlDmSIPAdapter::CopyCommandL(): begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    DBG_PRINT("CNSmlDmSIPAdapter::CopyCommandL(): end");
    }

// ---------------------------------------------------------------------------
//  CNSmlDmSIPAdapter::StartAtomicL
// (Not supported, see smldmadapter.h)
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::StartAtomicL()
    {
    DBG_PRINT("CNSmlDmSIPAdapter::StartAtomicL(): begin");
    DBG_PRINT("CNSmlDmSIPAdapter::StartAtomicL(): end");
    }

// ---------------------------------------------------------------------------
//  CNSmlDmSIPAdapter::CommitAtomicL
// (Not supported, see smldmadapter.h)
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::CommitAtomicL()
    {
    DBG_PRINT("CNSmlDmSIPAdapter::CommitAtomicL(): begin");
    DBG_PRINT("CNSmlDmSIPAdapter::CommitAtomicL(): end");
    }

// ---------------------------------------------------------------------------
//  CNSmlDmSIPAdapter::RollbackAtomicL
// (Not supported, see smldmadapter.h)
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::RollbackAtomicL()
    {
    DBG_PRINT("CNSmlDmSIPAdapter::RollbackAtomicL(): begin");
    DBG_PRINT("CNSmlDmSIPAdapter::RollbackAtomicL(): end");
    }

// ---------------------------------------------------------------------------
//  CNSmlDmSIPAdapter::StreamingSupport
// (Not supported, see smldmadapter.h)
// ---------------------------------------------------------------------------
//
TBool CNSmlDmSIPAdapter::StreamingSupport( TInt& /*aItemSize*/ )
    {
    DBG_PRINT("CNSmlDmSIPAdapter::StreamingSupport(): begin");
    DBG_PRINT("CNSmlDmSIPAdapter::StreamingSupport(): end");
    return EFalse;
    }

// ---------------------------------------------------------------------------
//  CNSmlDmSIPAdapter::StreamCommittedL
// (Not supported, see smldmadapter.h)
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::StreamCommittedL()
    {
    DBG_PRINT("CNSmlDmSIPAdapter::StreamCommittedL(): begin");
    DBG_PRINT("CNSmlDmSIPAdapter::StreamCommittedL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::CompleteOutstandingCmdsL
// Saves modified profiles back to permanent store
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::CompleteOutstandingCmdsL()
    {
    DBG_PRINT("CNSmlDmSIPAdapter::CompleteOutstandingCmdsL(): begin");

    // Check if profiles exist and some modifications are done.
    // Save only profiles that have changed.
    if ( iProfiles )
        {
        for (TInt i = 0 ; i < iModifications.Count() ; i++ )
            {
            TUint32 tIAPId( 0 ); 
            iProfiles->At( iModifications[ i ] )->
                GetParameter( KSIPAccessPointId, tIAPId );

            TBool autoreg( EFalse );
            iProfiles->At( iModifications[ i ] )->
                GetParameter( KSIPAutoRegistration, autoreg );

            // Save changes to permanent storage.
            iSIPManagedProfileRegistry->SaveL( 
                *iProfiles->At( iModifications[ i ] ) );
            }
        iModifications.Reset();
        }
    DBG_PRINT("CNSmlDmSIPAdapter::CompleteOutstandingCmdsL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::FetchLeafObjectSizeL
// Fetches size of a leaf object
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::FetchLeafObjectSizeL( 
    const TDesC8& aURI, 
    const TDesC8& aLUID, 
    const TDesC8& aType, 
    const TInt aResultsRef, 
    const TInt aStatusRef )
    {
    DBG_PRINT("CNSmlDmSIPAdapter::FetchLeafObjectSizeL(): begin");

    CBufBase *object = CBufFlat::NewL( 1 );
    CleanupStack::PushL( object );
    CSmlDmAdapter::TError retValue = FetchObjectL( aURI, aLUID, *object );

    TInt objSizeInBytes = object->Size();
    TBuf8<16> stringObjSizeInBytes;
    stringObjSizeInBytes.Num( objSizeInBytes );
    object->Reset();
    object->InsertL( 0, stringObjSizeInBytes );

    iDmCallback->SetStatusL( aStatusRef, retValue );
    iDmCallback->SetResultsL( aResultsRef, *object, aType );
    CleanupStack::PopAndDestroy( object );
    DBG_PRINT("CNSmlDmSIPAdapter::FetchLeafObjectSizeL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::UpdateLeafObjectL
// (Not supported, see smldmadapter.h)
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::UpdateLeafObjectL( 
    const TDesC8& /*aURI*/, 
    const TDesC8& /*aLUID*/, 
    RWriteStream*& /*aStream*/, 
    const TDesC8& /*aType*/, 
    const TInt aStatusRef )
    {
    DBG_PRINT("CNSmlDmSIPAdapter::UpdateLeafObjectL(): stream: begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    DBG_PRINT("CNSmlDmSIPAdapter::UpdateLeafObjectL(): stream: end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::ProfileRegistryEventOccurred 
// for MSIPProfileRegistryObserver
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::ProfileRegistryEventOccurred(
    TUint32 /*aProfileId*/, 
    TEvent /*aEvent*/)
    {
    }
    
// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::ProfileRegistryEventOccurred 
// for MSIPProfileRegistryObserver
// ---------------------------------------------------------------------------
//
void CNSmlDmSIPAdapter::ProfileRegistryErrorOccurred(
    TUint32 /*aProfileId*/, 
    TInt /*aError*/)
    {
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::SnapIdL
// Gets SNAP ID based on URI.
// ---------------------------------------------------------------------------
//
TUint32 CNSmlDmSIPAdapter::SnapIdL( const TDesC8& aUri )
    {
    DBG_PRINT( "CNSmlDmSIPAdapter::SnapIdL - begin" );
    TUint32 snapId( KErrNone );

    HBufC8* luid = HBufC8::NewLC( KNSmlSIPMaxUriLength ); // CS:1
    luid->Des().Copy( iDmCallback->GetLuidAllocL( aUri )->Des() );

    if ( luid->Des().Length() )
        {
        snapId = DesToInt( LastURISeg( luid->Des() ) );
        }
    CleanupStack::PopAndDestroy( luid ); // CS:0
    DBG_PRINT( "CNSmlDmSIPAdapter::SnapIdL - end" );
    return snapId;
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::GetSnapUriL
// Set SNAP URI based on SNAP ID.
// ---------------------------------------------------------------------------
//
MSmlDmAdapter::TError CNSmlDmSIPAdapter::GetSnapUriL( TDes8& aObject, 
    TUint32 aSnapId )
    {
    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );

    DBG_PRINT( "CNSmlDmSIPAdapter::GetSnapUriL - begin" );

    CBufBase* result = CBufFlat::NewL( 1 );
    CleanupStack::PushL( result ); // CS:1
    CSmlDmAdapter::TError status;

    // Get URIs to all SNAP settings.
    iDmCallback->FetchLinkL( KNSmlDMSIPSnapUriPrefix(), *result, status );

    if ( CSmlDmAdapter::EOk != status )
        {
        CleanupStack::PopAndDestroy( result );
        DBG_PRINT( "CNSmlDmSIPAdapter::GetSnapUriL - Fetch SNAP error end" );
        return status;
        }

    result->Compress();
    HBufC8* childList = HBufC8::NewLC( result->Size() ); // CS:2
    TInt uriSegCount( 0 );

    if ( result->Size() )
        {
        childList->Des().Copy( result->Ptr( 0 ) );
        uriSegCount = NumOfURISegs( childList->Des() );
        }

    // Get LUIDs for URIs.
    for ( TInt index = 0; index < uriSegCount; index++ )
        {
        HBufC8* uri = HBufC8::NewLC( KNSmlSIPMaxUriLength ); // CS:3
        HBufC8* luid = HBufC8::NewLC( KNSmlSIPMaxUriLength ); // CS:4

        uri->Des().Copy( KNSmlDMSIPSnapUriPrefix() );
        uri->Des().Append( KNSmlDMSIPSeparator8() );
        uri->Des().Append( LastURISeg( childList->Des() ) );
        luid->Des().Copy( iDmCallback->GetLuidAllocL( uri->Des() )->Des() );
        if ( luid->Des().Length() && 
            aSnapId == DesToInt( LastURISeg( luid->Des() ) ) )
            {
            aObject.Copy( uri->Des() );
            CleanupStack::PopAndDestroy( luid ); // CS:3
            CleanupStack::PopAndDestroy( uri );  // CS:2
            break;
            }
        childList->Des().Copy( RemoveLastURISeg( childList->Des() ) );
        CleanupStack::PopAndDestroy( luid ); // CS:3
        CleanupStack::PopAndDestroy( uri );  // CS:2
        }
    CleanupStack::PopAndDestroy( childList );  // CS:1
    CleanupStack::PopAndDestroy( result );     // CS:0
    DBG_PRINT( "CNSmlDmSIPAdapter::GetSnapUriL - end" );
    return status;
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::UpdateOutboundProxyNodeL
// Updater method for OutboundProxy node.
// ---------------------------------------------------------------------------
//
MSmlDmAdapter::TError CNSmlDmSIPAdapter::UpdateOutboundProxyNodeL( 
    TUint32 aProfileId, const TDesC8& aUri, const TDesC8& aObject )
    {
    DBG_PRINT( "CNSmlDmSIPAdapter::UpdateOutboundProxyNodeL - begin" );
    MSmlDmAdapter::TError status = EOk;

    // Get OutboundProxy SIP URI.
    const TDesC8* uri = 0;
    iProfiles->At( aProfileId )->GetParameter( KSIPOutboundProxy, 
        KSIPServerAddress, uri );
    TUriParser8 parser;
    TInt err = parser.Parse( *uri );
    if ( KErrNone != err )
        {
        DBG_PRINT( "CNSmlDmSIPAdapter::UpdateOutboundProxyNodeL - URI parse \
 error end" );
        return CSmlDmAdapter::EError;
        }
    CUri8* tempUri = CUri8::NewLC( parser );

    // Start of outboundproxy parameters.

    // SIP/x/OutboundProxy/Host
    // When host is cleared, whole proxy address needs to be cleared.
    if ( KNSmlDMSIPHost() == aUri )
        {
        if ( aObject == KNullDesC8 )
            {
            tempUri->RemoveComponentL( EUriScheme );
            tempUri->RemoveComponentL( EUriUserinfo );
            tempUri->RemoveComponentL( EUriHost );
            tempUri->RemoveComponentL( EUriPort );
            tempUri->RemoveComponentL( EUriPath );
            tempUri->RemoveComponentL( EUriQuery );
            tempUri->RemoveComponentL( EUriFragment );
            }
        else
            {
            HBufC8* scheme = HBufC8::NewLC( KMaxSchemeLength );
            if ( 0 == aObject.Find( KNSmlDMSIPSIPPrefix() ) )
                {
                scheme->Des().Copy( KNSmlDMSIPSip() );
                }
            else if ( 0 == aObject.Find( KNSmlDMSIPSIPSPrefix() ) )
                {
                scheme->Des().Copy( KNSmlDMSIPSips() );
                }
            if ( scheme->Des().Length() > 0 )
                {
                tempUri->SetComponentL( scheme->Des(), EUriScheme );
                if ( aObject.Length() > scheme->Des().Length() )
                    {
                    tempUri->SetComponentL( aObject.Mid( scheme->
                        Des().Length() + 1 ), EUriHost );
                    }
                else
                    {
                    // No host entered at all, remove host component.
                    tempUri->RemoveComponentL( EUriHost );
                    }
                }
            else
                {
                scheme->Des().Copy( tempUri->Uri().Extract( 
                    EUriScheme ) );
                if ( 0 == scheme->Des().Length() )
                    {
                    scheme->Des().Copy( KNSmlDMSIPSip() );
                    }
                tempUri->SetComponentL( scheme->Des(), EUriScheme );
                // Set host component, no scheme in parameter.
                tempUri->SetComponentL( aObject, EUriHost );
                }
            CleanupStack::PopAndDestroy( scheme );
            }
        }

    // SIP/x/OutboundProxy/Port
    if ( KNSmlDMSIPPort() == aUri )
        {
        if ( aObject == KNullDesC8 )
            {
            tempUri->RemoveComponentL( EUriPort );
            }
        else
            {
            tempUri->SetComponentL( aObject, EUriPort );
            }
        }

    // SIP/x/OutboundProxy/Transport
    if ( KNSmlDMSIPTransport() == aUri )
        {
        TBuf8<KUriPathMaxLength> tempPathTp;
        tempPathTp.Copy( tempUri->Uri().Extract( EUriPath ) );
        tempPathTp.LowerCase();

        // SIP/x/OutboundProxy/Transport
        if ( KNSmlDMSIPTransportTCP() == aObject )
            {
            TInt pos = tempPathTp.Find( KSIPTransportPrefix );
            if ( KErrNotFound != pos )
                {
                tempPathTp.Delete( pos, ( tempPathTp.Length() - pos ) );
                tempPathTp.Append( KSIPTransportTCP );
                }
            else
                {
                tempPathTp.Append( KSIPTransportTCP );
                }
            tempUri->SetComponentL( tempPathTp, EUriPath );
            }

        // SIP/x/OutboundProxy/Transport
        else if ( KNSmlDMSIPTransportUDP() == aObject )
            {
            TInt pos = tempPathTp.Find( KSIPTransportPrefix );
            if ( KErrNotFound != pos )
                {
                tempPathTp.Delete( pos, ( tempPathTp.Length() - pos ) );
                tempPathTp.Append( KSIPTransportUDP );
                }
            else
                {
                tempPathTp.Append( KSIPTransportUDP );
                }
            tempUri->SetComponentL( tempPathTp, EUriPath );
            }

        // SIP/x/OutboundProxy/Transport
        else if ( KNSmlDMSIPTransportAuto() == aObject )
            {
            TInt pos = tempPathTp.Find( KSIPTransportPrefix );
            if ( KErrNotFound != pos )
                {
                tempPathTp.Delete( pos, ( tempPathTp.Length() - pos ) );
                }
            tempUri->SetComponentL( tempPathTp, EUriPath );
            }
        else
            {
            status = CSmlDmAdapter::EInvalidObject;
            } 
        }

    // SIP/x/OutboundProxy/Username 
    if ( KNSmlDMSIPUsername() == aUri )
        {
        TSIPProfileTypeInfo type;
        type = iProfiles->At( aProfileId )->Type();

        // Set Username if profile type is IETF.
        if ( type.iSIPProfileClass == TSIPProfileTypeInfo::EInternet )
            {
            if ( KErrNotFound == aObject.Find( KEscaped() ) )
                {
                HBufC8* encoded = EscapeUtils::EscapeEncodeL( aObject,
                    EscapeUtils::EEscapeNormal );
                err = iProfiles->At( aProfileId )->SetParameter(
                    KSIPOutboundProxy, KSIPDigestUserName, *encoded );
                delete encoded;
                }
            else
                {
                err = iProfiles->At( aProfileId )->SetParameter(
                    KSIPOutboundProxy, KSIPDigestUserName, aObject );
                }
            }

        // Set PrivateIdentity if profiletype is IMS or other.
        else if ( type.iSIPProfileClass == TSIPProfileTypeInfo::EIms ||
             type.iSIPProfileClass == TSIPProfileTypeInfo::EOther )
            {
            if ( KErrNotFound == aObject.Find( KEscaped() ) )
                {
                HBufC8* encoded = EscapeUtils::EscapeEncodeL( aObject,
                    EscapeUtils::EEscapeNormal );
                err = iProfiles->At( aProfileId )->SetParameter( 
                    KSIPPrivateIdentity, *encoded );
                delete encoded;
                }
            else
                {
                err = iProfiles->At( aProfileId )->SetParameter( 
                    KSIPPrivateIdentity, aObject );
                }
            }

        else 
            {
            status = CSmlDmAdapter::EInvalidObject;
            }
        }

    // SIP/x/OutboundProxy/Passwd
    if ( KNSmlDMSIPPasswd() == aUri )
        {
        err = iProfiles->At( aProfileId )->SetParameter( 
            KSIPOutboundProxy, KSIPDigestPassword, aObject );
        }

    // SIP/x/OutboundProxy/Realm
    if ( KNSmlDMSIPRealm() == aUri )
        {
        err = iProfiles->At( aProfileId )->SetParameter( 
            KSIPOutboundProxy, KSIPDigestRealm, aObject );
        }

    // Set tempUri back to original sipURI.
    if ( status == CSmlDmAdapter::EOk )
        {
        err = iProfiles->At( aProfileId )->SetParameter( 
            KSIPOutboundProxy, KSIPServerAddress, 
            tempUri->Uri().UriDes() );
        }
    else
        {
        status = CSmlDmAdapter::EError;
        }
    CleanupStack::PopAndDestroy( tempUri );

    if ( KErrNone != err )
        {
        status = CSmlDmAdapter::EError;
        }

    DBG_PRINT( "CNSmlDmSIPAdapter::UpdateOutboundProxyNodeL - end" );
    return status;
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::UpdateRegistrarServerNodeL
// Updater method for RegistrarServer node.
// ---------------------------------------------------------------------------
//
MSmlDmAdapter::TError CNSmlDmSIPAdapter::UpdateRegistrarServerNodeL( 
    TUint32 aProfileId, const TDesC8& aUri, const TDesC8& aObject )
    {
    DBG_PRINT( "CNSmlDmSIPAdapter::UpdateRegistrarServerNodeL - begin" );
    MSmlDmAdapter::TError status = EOk;

    // Get RegistrarServer SIP URI.
    const TDesC8* uri = 0;
    iProfiles->At( aProfileId )->GetParameter ( KSIPRegistrar, 
        KSIPServerAddress, uri );
    TUriParser8 parser;
    TInt err = parser.Parse( *uri );
    if ( KErrNone != err )
        {
    DBG_PRINT( "CNSmlDmSIPAdapter::UpdateRegistrarServerNodeL - URI \
parse error end" );
        return CSmlDmAdapter::EError;
        }
    CUri8* tempUri = CUri8::NewLC( parser );

    // SIP/x/RegistrarServer/Host
    if ( KNSmlDMSIPHost() == aUri )
        {
        if ( aObject == KNullDesC8 )
            {
            tempUri->RemoveComponentL( EUriHost );
            }
        else
            {
            HBufC8* scheme = HBufC8::NewLC( KMaxSchemeLength );
            if ( 0 == aObject.Find( KNSmlDMSIPSIPPrefix() ) )
                {
                scheme->Des().Copy( KNSmlDMSIPSip() );
                }
            else if ( 0 == aObject.Find( KNSmlDMSIPSIPSPrefix() ) )
                {
                scheme->Des().Copy( KNSmlDMSIPSips() );
                }
            if ( scheme->Des().Length() > 0 )
                {
                // Set scheme if exists in host parameter.
                tempUri->SetComponentL( scheme->Des(), EUriScheme );
                if ( aObject.Length() > scheme->Des().Length() )
                    {
                    tempUri->SetComponentL( aObject.Mid( scheme->
                        Des().Length() + 1 ), EUriHost );
                    }
                else
                    {
                    // No host entered at all, remove host component.
                    tempUri->RemoveComponentL( EUriHost );
                    }
                }
            else
                {
                scheme->Des().Copy( tempUri->Uri().Extract( 
                    EUriScheme ) );
                if ( 0 == scheme->Des().Length() )
                    {
                    scheme->Des().Copy( KNSmlDMSIPSip() );
                    }
                tempUri->SetComponentL( scheme->Des(), EUriScheme );
                // Set host component, no scheme in parameter.
                tempUri->SetComponentL( aObject, EUriHost );
                }   
            CleanupStack::PopAndDestroy( scheme );
            }
        }

    // SIP/x/RegistrarServer/Port
    if ( KNSmlDMSIPPort() == aUri )
        {
        if ( aObject == KNullDesC8 )
            {
            tempUri->RemoveComponentL( EUriPort );
            }
        else
            {
            tempUri->SetComponentL( aObject, EUriPort );
            }
        }

    // SIP/x/RegistrarServer/Transport       
    if ( KNSmlDMSIPTransport() == aUri )
        {
        TBuf8<KUriPathMaxLength> tempPathReg;
        tempPathReg.Copy( tempUri->Uri().Extract( EUriPath ) );
        tempPathReg.LowerCase();
        
        if ( KNSmlDMSIPTransportTCP() == aObject )
            {
            TInt pos = tempPathReg.Find( KSIPTransportPrefix );
            if ( KErrNotFound != pos )
                {
                tempPathReg.Delete( pos, ( tempPathReg.Length() - pos ) );
                tempPathReg.Append( KSIPTransportTCP );
                }
            else
                {
                tempPathReg.Append( KSIPTransportTCP );
                }
            tempUri->SetComponentL( tempPathReg, EUriPath );
            }

        else if ( KNSmlDMSIPTransportUDP() == aObject )
            {
            TInt pos = tempPathReg.Find( KSIPTransportPrefix );
            if ( KErrNotFound != pos  )
                {
                tempPathReg.Delete( pos, ( tempPathReg.Length() - pos ) );
                tempPathReg.Append( KSIPTransportUDP );
                }
            else
                {
                tempPathReg.Append( KSIPTransportUDP );
                }
            tempUri->SetComponentL( tempPathReg, EUriPath );
            }
        
        else if ( KNSmlDMSIPTransportAuto() == aObject )
            {
            TInt pos = tempPathReg.Find( KSIPTransportPrefix );
            if ( KErrNotFound != pos )
                {
                tempPathReg.Delete( pos, ( tempPathReg.Length() - pos ) );
                }
            tempUri->SetComponentL( tempPathReg, EUriPath );
            }

        else
            {
            status = CSmlDmAdapter::EInvalidObject;
            } 
        }

    // SIP/x/RegistrarServer/Username 
    if ( KNSmlDMSIPUsername() == aUri )
        {
        TSIPProfileTypeInfo type;
        type = iProfiles->At( aProfileId )->Type();
        if ( type.iSIPProfileClass == TSIPProfileTypeInfo::EInternet )
            {
            if ( KErrNotFound == aObject.Find( KEscaped() ) )
                {
                HBufC8* encoded = EscapeUtils::EscapeEncodeL( aObject,
                    EscapeUtils::EEscapeNormal );
                err = iProfiles->At( aProfileId )->SetParameter(
                    KSIPRegistrar, KSIPDigestUserName, *encoded );
                delete encoded;
                }
            else
                {
                err = iProfiles->At( aProfileId )->SetParameter(
                    KSIPRegistrar, KSIPDigestUserName, aObject );
                }
            }
        }

    // SIP/x/RegistrarServer/PrivateIdentity
    if ( KNSmlDMSIPPrivateIdentity() == aUri )
        {
        TSIPProfileTypeInfo type;
        type = iProfiles->At( aProfileId )->Type();

        // Set PrivateIdentity if profiletype is Ims or preIms.
        if ( type.iSIPProfileClass == TSIPProfileTypeInfo::EIms || 
             type.iSIPProfileClass == TSIPProfileTypeInfo::EOther )
            {
            err = iProfiles->At( aProfileId )->SetParameter( 
                KSIPPrivateIdentity, aObject );
            }
        else 
            {
            status = CSmlDmAdapter::EInvalidObject;
            }
        }

    // SIP/x/RegistrarServer/Passwd
    if ( KNSmlDMSIPPasswd() == aUri )
        {
        err = iProfiles->At( aProfileId )->SetParameter( KSIPRegistrar, 
            KSIPDigestPassword, aObject );
        }

    // SIP/x/RegistrarServer/Realm
    if ( KNSmlDMSIPRealm() == aUri )
        {
        err = iProfiles->At( aProfileId )->SetParameter( KSIPRegistrar,
            KSIPDigestRealm, aObject );
        }

    // Set tempUri back to original sipURI.
    // Allow emptying realm and passwd.
    if ( status == CSmlDmAdapter::EOk )
        {
        err = iProfiles->At( aProfileId )->SetParameter( KSIPRegistrar,
            KSIPServerAddress, tempUri->Uri().UriDes() );
        }
    else
        {
        status = CSmlDmAdapter::EError;
        }
    CleanupStack::PopAndDestroy( tempUri );

    if ( KErrNone != err )
        {
        status = CSmlDmAdapter::EError;
        }

    DBG_PRINT( "CNSmlDmSIPAdapter::UpdateRegistrarServerNodeL - end" );
    return status;
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::FetchOutboundProxyNodeL
// Fetcher method for OutboundProxy node.
// ---------------------------------------------------------------------------
//
MSmlDmAdapter::TError CNSmlDmSIPAdapter::FetchOutboundProxyNodeL( 
    TUint32 aProfileId, const TDesC8& aUri, TDes8& aSegmentResult )
    {
    DBG_PRINT( "CNSmlDmSIPAdapter::FetchOutboundProxyNodeL - begin" );
    MSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    // Get OutboundProxy SIP URI.
    const TDesC8* uri = 0;
    TInt err = iProfiles->At( aProfileId )->GetParameter( 
        KSIPOutboundProxy, KSIPServerAddress, uri );

    CSIPAddress* sipURI = 0;
    if ( 0 < uri->Length() )
        {
        sipURI = CSIPAddress::DecodeL( *uri );
        }
    // The whole SIP URI is empty so return ENotFound.
    else
        {
        status = CSmlDmAdapter::ENotFound;
        DBG_PRINT( "CNSmlDmSIPAdapter::FetchOutboundProxyNodeL - Host not \
found end" );
        return status;
        }
    CleanupStack::PushL( sipURI );

    // Start of outboundproxy parameters
    // SIP//OutboundProxy/Host
    if ( KNSmlDMSIPHost() == aUri )
        {
        aSegmentResult.Copy( sipURI->Uri8().Uri().Extract( EUriHost ) );
        }

    // SIP//OutboundProxy/Port
    if ( KNSmlDMSIPPort() == aUri )
        {
        // Get port if exists.
        if ( sipURI->Uri8().Uri().IsPresent( EUriPort ) )
            {
            aSegmentResult.Copy( sipURI->Uri8().Uri().Extract( EUriPort ) );
            }
        else
            {
            aSegmentResult.Num( KDefaultPortNumber );
            }
        }

    // SIP//OutboundProxy/Transport
    if ( KNSmlDMSIPTransport() == aUri )
        {
        TBuf8<KUriPathMaxLength> tempPathOb;
        tempPathOb.Copy( sipURI->Uri8().Uri().Extract( EUriPath ) );
        tempPathOb.LowerCase();

        // Return default transport if none is found.
        aSegmentResult.Copy( KNSmlDMSIPTransportAuto );

        TInt pos = tempPathOb.Find( KSIPTransportTCP );
        if ( KErrNotFound != pos  )
            {
            aSegmentResult.Copy( KNSmlDMSIPTransportTCP );
            }
        pos = tempPathOb.Find( KSIPTransportUDP );
        if ( KErrNotFound != pos  )
            {
            aSegmentResult.Copy( KNSmlDMSIPTransportUDP );
            } 
        }        

    // SIP/x/OutboundProxy/Username
    if ( KNSmlDMSIPUsername() == aUri )
        {
        TSIPProfileTypeInfo type;
        type = iProfiles->At( aProfileId )->Type();
        if ( type.iSIPProfileClass == TSIPProfileTypeInfo::EInternet )
            {
            const TDesC8* paramResult = NULL;
            err = iProfiles->At( aProfileId )->GetParameter( 
                KSIPOutboundProxy, KSIPDigestUserName, paramResult );
            aSegmentResult.Copy( *paramResult );
            }
        else if ( type.iSIPProfileClass == TSIPProfileTypeInfo::EIms || 
             type.iSIPProfileClass == TSIPProfileTypeInfo::EOther )
            {
            const TDesC8* paramResult = NULL;
            iProfiles->At( aProfileId )->
                GetParameter( KSIPPrivateIdentity, paramResult );
            aSegmentResult.Copy( *paramResult );
            }
        else
            {
            aSegmentResult.Copy( KNullDesC );
            }
        }

    // SIP/x/OutboundProxy/Realm
    if ( KNSmlDMSIPRealm() == aUri )
        {
        const TDesC8* paramResult = NULL;
        err = iProfiles->At( aProfileId )->GetParameter( KSIPOutboundProxy,
            KSIPDigestRealm, paramResult );
        aSegmentResult.Copy( *paramResult );
        }
    CleanupStack::PopAndDestroy( sipURI );

    if ( KErrNone != err )
        {
        status = CSmlDmAdapter::EError;
        }

    DBG_PRINT( "CNSmlDmSIPAdapter::FetchOutboundProxyNodeL - end" );
    return status;
    }

// ---------------------------------------------------------------------------
// CNSmlDmSIPAdapter::FetchRegistrarServerNodeL
// Fetcher method for RegistrarServer node.
// ---------------------------------------------------------------------------
//
MSmlDmAdapter::TError CNSmlDmSIPAdapter::FetchRegistrarServerNodeL( 
    TUint32 aProfileId, const TDesC8& aUri, TDes8& aSegmentResult )
    {
    DBG_PRINT( "CNSmlDmSIPAdapter::FetchRegistrarServerNodeL - begin" );
    MSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    // Start of RegistrarServer parameters.
    // Get Registrar SIP URI.
    const TDesC8* uri = 0;
    TInt err = iProfiles->At( aProfileId )->GetParameter( KSIPRegistrar, 
        KSIPServerAddress, uri );

    CSIPAddress* sipURI = 0;   
    if ( 0 < uri->Length() )
        {
        sipURI = CSIPAddress::DecodeL( *uri );
        }
    // The whole SIP URI is empty so return ENotFound.
    else
        {
        status = CSmlDmAdapter::ENotFound;
        DBG_PRINT( "CNSmlDmSIPAdapter::FetchRegistrarServerNodeL - Host not \
found end" );
        return status;
        }

    CleanupStack::PushL( sipURI );

    // SIP/x/RegistrarServer/Host
    if ( KNSmlDMSIPHost() == aUri )
        {
        aSegmentResult.Copy( sipURI->Uri8().Uri().Extract( EUriHost ) );
        }

    // SIP/x/RegistrarServer/Port
    if ( KNSmlDMSIPPort() == aUri )
        {
        // Get port if exists.
        if ( sipURI->Uri8().Uri().IsPresent( EUriPort ) )
            {
            aSegmentResult.Copy( sipURI->Uri8().Uri().Extract( 
                EUriPort ) );
            }
        else
            {
            aSegmentResult.Num ( KDefaultPortNumber );
            }
        }

    // SIP/x/RegistrarServer/Transport
    if ( KNSmlDMSIPTransport() == aUri )
        {
        TBuf8<KUriPathMaxLength> tempPathReg;
        tempPathReg.Copy( sipURI->Uri8().Uri().Extract( EUriPath ) );
        tempPathReg.LowerCase();

        // Return default transport if none is found.
        aSegmentResult.Copy( KNSmlDMSIPTransportAuto );

        TInt pos = tempPathReg.Find( KSIPTransportTCP );
        if ( KErrNotFound != pos )
            {
            aSegmentResult.Copy( KNSmlDMSIPTransportTCP );
            }
        pos = tempPathReg.Find( KSIPTransportUDP );
        if ( KErrNotFound != pos )
            {
            aSegmentResult.Copy( KNSmlDMSIPTransportUDP );
            }     
        }

    // SIP/x/RegistrarServer/Username
    if ( KNSmlDMSIPUsername() == aUri )
        {
        TSIPProfileTypeInfo type;
        type = iProfiles->At( aProfileId )->Type();
        if ( type.iSIPProfileClass == TSIPProfileTypeInfo::EInternet )
            {
            const TDesC8* paramResult = NULL;
            err = iProfiles->At( aProfileId )->GetParameter( 
                KSIPRegistrar, KSIPDigestUserName, paramResult );
            aSegmentResult.Copy( *paramResult );
            }
        else
            {
            aSegmentResult.Copy( KNullDesC );
            }
        }

    // PrivateIdentity used in IMS or PreIms.
    if ( KNSmlDMSIPPrivateIdentity() == aUri )
        {
        TSIPProfileTypeInfo type;
        type = iProfiles->At( aProfileId )->Type();
        if ( type.iSIPProfileClass == TSIPProfileTypeInfo::EIms || 
             type.iSIPProfileClass == TSIPProfileTypeInfo::EOther )
            {
            const TDesC8* paramResult = NULL;
            iProfiles->At( aProfileId )->
                GetParameter( KSIPPrivateIdentity, paramResult );
            aSegmentResult.Copy( *paramResult );
            }
        else
            {
            aSegmentResult.Copy( KNullDesC );
            }
        }

    // SIP/x/RegistrarServer/Realm
    if ( KNSmlDMSIPRealm() == aUri )
        {
        const TDesC8* paramResult = NULL;
        err = iProfiles->At( aProfileId )->GetParameter( 
            KSIPRegistrar, KSIPDigestRealm, paramResult );
        aSegmentResult.Copy( *paramResult );
        }
    CleanupStack::PopAndDestroy( sipURI );

    if ( KErrNone != err )
        {
        status = CSmlDmAdapter::EError;
        }

    DBG_PRINT( "CNSmlDmSIPAdapter::FetchRegistrarServerNodeL - end" );
    return status;
    }

// End of file
