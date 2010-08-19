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
* Description:  Settings item for SIP settings, handles also the saving of
*                settings item. 
*
*/


// INCLUDE FILES
#include <wpsipadapter.rsg>
#include <charconv.h>
#include <sipprofile.h>
#include <sipprofiletypeinfo.h>
#include <sipmanagedprofileregistry.h>
#include <sipprofileregistry.h>
#include <sipmanagedprofile.h>
#include <in_sock.h>                // TInetAddr, TSockAddr
#include <sip.h>
#include <WPAdapterUtil.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>

#include "CWPSIPItem.h"
#include "CSIPProfileRegistryObserver.h"
#include "CWPSIPAdapter.h"
#include "wpsipadapterdebug.h"

#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include <cmmanagerdef.h>
#include <escapeutils.h>

// CONSTANTS
const TInt KMaxCharsInTUint32     = 10;
const TInt KMaxProviderNameLength = 100;
_LIT8( KPTypeIMS,           "IMS" ); // w9010.txt
_LIT8( KImsPlugInName,      "ims" ); // SIP Profile Plugin name
_LIT8( KPTypeNokiapreR5Ims, "NokiapreR5Ims" ); // for backward compatibility
_LIT8( KPTypeIETF,          "IETF" ); // w9010.txt
_LIT8( KColon,              ":" );
_LIT8( KTransport,          ";transport=" ); // IETF RFC 3261
_LIT8( KSip,                "sip:" ); // IETF RFC 3261
_LIT8( KLr,                 ";lr" ); // IETF RFC 3261
_LIT8( KOpenParenthesis,    "(" );
_LIT8( KClosedParenthesis,  ")" );
_LIT8( KSips,               "sips:" ); // IETF RFC 3261
_LIT8( KEscaped,            "%" );

// ---------------------------------------------------------------------------
// CWPSIPItem::NewL
//
// ---------------------------------------------------------------------------
//
CWPSIPItem* CWPSIPItem::NewL()
    {
    CWPSIPItem* self = new ( ELeave ) CWPSIPItem();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::CWPSIPItem
//
// ---------------------------------------------------------------------------
//
CWPSIPItem::CWPSIPItem()
    {
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::~CWPSIPItem
//
// ---------------------------------------------------------------------------
//
CWPSIPItem::~CWPSIPItem()
    {
    DBG_PRINT( "CWPSIPItem::~CWPSIPItem - begin" );
    delete iProviderId;
    delete iName;
    delete iAppRef;
    delete iAOR;
    delete iProtocolForOutboundConnection;
    delete iNapDefName;
    delete iOutboundAddr;
    delete iPortNumberForOutboundConnection;
    delete iAuthenticationTypeForOutbound;
    delete iUsernameForOutbound;
    delete iPasswordForOutbound;
    delete iRealmForOutbound;
    delete iRegistrarAddr;
    delete iAuthenticationTypeForRegistrar;
    delete iUsernameForRegistrar;
    delete iPasswordForRegistrar;
    delete iRealmForRegistrar;
    delete iSavedProfileId;
    delete iToAppRef;
    delete iNetwork;
    DBG_PRINT( "CWPSIPItem::~CWPSIPItem - end" );
    }

// ---------------------------------------------------------------------------
// CWPSIPSAPItem::ConstructL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::ConstructL()
    { 
    DBG_PRINT( "CWPSIPItem::ConstructL - begin" );
    iProviderId                               = HBufC8::NewL( 0 );
    iName                                     = HBufC::NewL( 0 );
    iAppRef                                   = HBufC8::NewL( 0 );
    iAOR                                      = HBufC8::NewL( 0 );
    iProtocolForOutboundConnection            = HBufC8::NewL( 0 );
    iNapDefName                               = HBufC8::NewL( 0 );
    iSignalCompressionEnabled                 = EFalse;
    iSecurityNegotiationEnabled               = EFalse;
    iLooseRoutingForOutboundConnectionEnabled = EFalse;
    iOutboundAddr                             = HBufC8::NewL( 0 );
    iPortNumberForOutboundConnection          = HBufC8::NewL( 0 );
    iAuthenticationTypeForOutbound            = HBufC8::NewL( 0 );
    iUsernameForOutbound                      = HBufC8::NewL( 0 );
    iPasswordForOutbound                      = HBufC8::NewL( 0 );
    iRealmForOutbound                         = HBufC8::NewL( 0 );
    iRegistrarAddr                            = HBufC8::NewL( 0 );
    iAuthenticationTypeForRegistrar           = HBufC8::NewL( 0 );
    iUsernameForRegistrar                     = HBufC8::NewL( 0 );
    iPasswordForRegistrar                     = HBufC8::NewL( 0 );
    iRealmForRegistrar                        = HBufC8::NewL( 0 );
    iNapDef                                   = NULL;
    iSetAsDefault                             = EFalse;
    iSavedProfileId                           = HBufC8::NewL( 0 );
    iProfileId                                = 0;
    iProfileLockEnabled                       = EFalse;
    iToAppRef                                 = HBufC8::NewL( 0 );
    iNetwork                                  = HBufC::NewL( 0 );
    iSnapId                                   = KErrNotFound;
    iBearerId                                 = 0;
    SetSigQos( 40 );
    DBG_PRINT( "CWPSIPItem::ConstructL - end" );
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetProfileTypeL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetProfileTypeL( const TDesC8& aPTYPE )
    {
    HBufC8* tmpProfileType = aPTYPE.AllocL();
    CleanupStack::PushL( tmpProfileType ); // CS:1
    // Map the profile type name used in message to SIP stack internal type.
    if ( ( ( tmpProfileType->Compare( KPTypeIMS() ) ) == 0 ) ||
        ( tmpProfileType->Compare( KPTypeNokiapreR5Ims ) == 0 ) )
        {
        iSIPProfileTypeInfo.iSIPProfileClass = TSIPProfileTypeInfo::EIms;
        iSIPProfileTypeInfo.iSIPProfileName.Insert( 0, KImsPlugInName() );
        }
    else if ( ( tmpProfileType->Compare( KPTypeIETF() ) ) == 0 )
        {
        iSIPProfileTypeInfo.iSIPProfileClass = TSIPProfileTypeInfo::EInternet;
        iSIPProfileTypeInfo.iSIPProfileName.Insert( 0,tmpProfileType->Des() );
        }
    else
        {
        iSIPProfileTypeInfo.iSIPProfileClass = TSIPProfileTypeInfo::EOther;
        iSIPProfileTypeInfo.iSIPProfileName.Insert( 0,tmpProfileType->Des() );
        }
    CleanupStack::PopAndDestroy( tmpProfileType ); // CS:0
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetAppRefL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetAppRefL( const TDesC8& aAPPREF )
    {
    HBufC8* temp = aAPPREF.AllocL();
    delete iAppRef;
    iAppRef = NULL;
    iAppRef = temp;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetAORL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetAORL( const TDesC8& aPUID )
    {
    HBufC8* temp( NULL );
    
    if ( KErrNotFound == aPUID.Find( KEscaped() ) )
        {
        temp = EscapeUtils::EscapeEncodeL( aPUID,
            EscapeUtils::EEscapeNormal );
        }
    else
        {
        temp = aPUID.AllocL();
        }
    
    delete iAOR;
    iAOR = NULL;
    
    // If the PUID starts with sip: or sips:
    if ( KErrNone == temp->Find( KSip() ) ||
         KErrNone == temp->Find( KSips() ) )
        {
        iAOR = temp;
        }
    else
        {
        CleanupStack::PushL( temp ); // CS: 1
        
        iAOR = HBufC8::NewL( KSip().Length() + temp->Length() );
        iAOR->Des().Copy( KSip() );
        iAOR->Des().Append( *temp );
        
        CleanupStack::PopAndDestroy( temp );  // CS: 0
        }
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetNameL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetNameL( const TDesC& aNAME )
    {
    HBufC* temp = aNAME.AllocL();
    delete iName;
    iName = NULL;
    iName = temp;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetProviderIdL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetProviderIdL( const TDesC8& aPROVIDERID )
    {
    HBufC8* temp = aPROVIDERID.AllocL();
    delete iProviderId;
    iProviderId = NULL;
    iProviderId = temp;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetProtocolForOutboundConnectionL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetProtocolForOutboundConnectionL( const TDesC8& aAPROTOCOL )
    {
    HBufC8* temp = aAPROTOCOL.AllocL();
    delete iProtocolForOutboundConnection;
    iProtocolForOutboundConnection = NULL;
    iProtocolForOutboundConnection = temp;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetNapDefNameL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetNapDefNameL( const TDesC8& aTONAPID )
    {
    HBufC8* temp = aTONAPID.AllocL();
    delete iNapDefName;
    iNapDefName = NULL;
    iNapDefName = temp;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetSignalCompressionEnabled
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetSignalCompressionEnabled( TBool aEnabled )
    {
    iSignalCompressionEnabled = aEnabled;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetSecurityNegotiationEnabled
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetSecurityNegotiationEnabled( TBool aEnabled )
    {
    iSecurityNegotiationEnabled = aEnabled;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetLooseRoutingForOutboundConnectionEnabled
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetLooseRoutingForOutboundConnectionEnabled( TBool aEnabled )
    {
    iLooseRoutingForOutboundConnectionEnabled = aEnabled;
    }
    
// ---------------------------------------------------------------------------
// CWPSIPItem::SetProfileLockEnabled
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetProfileLockEnabled( TBool aEnabled )
    {
    iProfileLockEnabled = aEnabled;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetOutboundAddrL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetOutboundAddrL( const TDesC8& aOutboundAddr )
    {
    // If the parameter is empty, do not add prefix.
    if ( !aOutboundAddr.Length() )
        {
        return;
        }

    delete iOutboundAddr;
    iOutboundAddr = NULL;
    // Check if the address starts with sip: or sips:
    if ( KErrNone == aOutboundAddr.Find( KSip() ) ||
         KErrNone == aOutboundAddr.Find( KSips() ) )
        {
        iOutboundAddr = aOutboundAddr.AllocL();
        }
    else
        {
        iOutboundAddr = HBufC8::NewL( KSip().Length() + 
            aOutboundAddr.Length() );
        iOutboundAddr->Des().Copy( KSip() );
        iOutboundAddr->Des().Append( aOutboundAddr );
        }
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetPortNumberForOutboundConnectionL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetPortNumberForOutboundConnectionL( const TDesC8& aPORT )
    {
    HBufC8* temp = aPORT.AllocL();
    delete iPortNumberForOutboundConnection;
    iPortNumberForOutboundConnection = NULL;
    iPortNumberForOutboundConnection = temp;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetAuthenticationTypeForOutboundL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetAuthenticationTypeForOutboundL( const TDesC8& aAAUTHTYPE )
    {
    HBufC8* temp = aAAUTHTYPE.AllocL();
    delete iAuthenticationTypeForOutbound;
    iAuthenticationTypeForOutbound = NULL;
    iAuthenticationTypeForOutbound = temp;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetUsernameForOutboundL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetUsernameForOutboundL( const TDesC8& aAAUTHNAME )
    {
    HBufC8* temp( NULL );
    
    if ( KErrNotFound == aAAUTHNAME.Find( KEscaped() ) )
        {
        temp = EscapeUtils::EscapeEncodeL( aAAUTHNAME,
            EscapeUtils::EEscapeNormal );
        }
    else
        {
        temp = aAAUTHNAME.AllocL();
        }
    
    delete iUsernameForOutbound;
    iUsernameForOutbound = NULL;
    iUsernameForOutbound = temp;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetPasswordForOutboundL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetPasswordForOutboundL( const TDesC8& aAAUTHSECRET )
    {
    HBufC8* temp = aAAUTHSECRET.AllocL();
    delete iPasswordForOutbound;
    iPasswordForOutbound = NULL;
    iPasswordForOutbound = temp;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetRealmForOutboundL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetRealmForOutboundL( const TDesC8& aAAUTHDATA )
    {
    HBufC8* temp = aAAUTHDATA.AllocL();
    delete iRealmForOutbound;
    iRealmForOutbound = NULL;
    iRealmForOutbound = temp;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetRegistrarAddrL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetRegistrarAddrL( const TDesC8& aRegistrarAddr )
    {
    // If the paremeter is empty, do not add prefix.
    if ( !aRegistrarAddr.Length() )
        {
        return;
        }

    delete iRegistrarAddr;
    iRegistrarAddr = NULL;
    // Check if the address starts with sip: or sips:
    if ( KErrNone == aRegistrarAddr.Find( KSip() ) ||
         KErrNone == aRegistrarAddr.Find( KSips() ) )
        {
        iRegistrarAddr = aRegistrarAddr.AllocL();
        }
    else
        {
        iRegistrarAddr = HBufC8::NewL( KSip().Length() + 
            aRegistrarAddr.Length() );
        iRegistrarAddr->Des().Copy( KSip() );
        iRegistrarAddr->Des().Append( aRegistrarAddr );
        }
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetAuthenticationTypeForRegistrarL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetAuthenticationTypeForRegistrarL( const TDesC8& 
    aAAUTHTYPE )
    {
    HBufC8* temp = aAAUTHTYPE.AllocL();
    delete iAuthenticationTypeForRegistrar;
    iAuthenticationTypeForRegistrar = NULL;
    iAuthenticationTypeForRegistrar = temp;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetUsernameForRegistrarL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetUsernameForRegistrarL( const TDesC8& aAAUTHNAME )
    {
    HBufC8* temp( NULL );
    
    if ( KErrNotFound == aAAUTHNAME.Find( KEscaped() ) )
        {
        temp = EscapeUtils::EscapeEncodeL( aAAUTHNAME,
            EscapeUtils::EEscapeNormal );
        }
    else
        {
        temp = aAAUTHNAME.AllocL();
        }
    
    delete iUsernameForRegistrar;
    iUsernameForRegistrar = NULL;
    iUsernameForRegistrar = temp;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetPasswordForRegistrarL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetPasswordForRegistrarL( const TDesC8& aAAUTHSECRET )
    {
    HBufC8* temp = aAAUTHSECRET.AllocL();
    delete iPasswordForRegistrar;
    iPasswordForRegistrar = NULL;
    iPasswordForRegistrar = temp;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetRealmForRegistrarL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetRealmForRegistrarL( const TDesC8& aAAUTHDATA )
    {
    HBufC8* temp = aAAUTHDATA.AllocL();
    delete iRealmForRegistrar;
    iRealmForRegistrar = NULL;
    iRealmForRegistrar = temp;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetNapDef
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetNapDef( CWPCharacteristic* aNapDef )
    {
    iNapDef = aNapDef;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetAsDefaultL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetAsDefaultL()
    {
    iSetAsDefault = ETrue;
    }

// ---------------------------------------------------------------------------
// CWPSIPSAPItem::SetIAPId
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetIAPId( TUint32 aUid )
    {
    iIAPId = aUid;
    }

// ---------------------------------------------------------------------------
// CWPSIPSAPItem::SetAutomaticRegistrationEnabled
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetAutomaticRegistrationEnabled( 
    TBool aAutomaticRegistrationEnabled )
    {
    iAutomaticRegistrationEnabled = aAutomaticRegistrationEnabled;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetAOR
//
// ---------------------------------------------------------------------------
//
TDesC8& CWPSIPItem::GetAOR()
    {
    return *iAOR;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetProviderId
//
// ---------------------------------------------------------------------------
//
TDesC8& CWPSIPItem::GetProviderId()
    {
    return *iProviderId;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetName
//
// ---------------------------------------------------------------------------
//
TDesC& CWPSIPItem::GetName()
    {
    return *iName;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetProtocolForOutboundConnection
//
// ---------------------------------------------------------------------------
//
TDesC8& CWPSIPItem::GetProtocolForOutboundConnection()
    {
    return *iProtocolForOutboundConnection;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetNapDefName
//
// ---------------------------------------------------------------------------
//
TDesC8& CWPSIPItem::GetNapDefName()
    {
    return *iNapDefName;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetSignalCompressionEnabled
//
// ---------------------------------------------------------------------------
//
TBool CWPSIPItem::GetSignalCompressionEnabled()
    {
    return iSignalCompressionEnabled;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetSecurityNegotiationEnabled
//
// ---------------------------------------------------------------------------
//
TBool CWPSIPItem::GetSecurityNegotiationEnabled()
    {
    return iSecurityNegotiationEnabled;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetLooseRoutingForOutboundConnectionEnabled
//
// ---------------------------------------------------------------------------
//
TBool CWPSIPItem::GetLooseRoutingForOutboundConnectionEnabled()
    {
    return iLooseRoutingForOutboundConnectionEnabled;
    }
    
// ---------------------------------------------------------------------------
// CWPSIPItem::GetAutomaticRegistrationEnabled
//
// ---------------------------------------------------------------------------
//
TBool CWPSIPItem::GetAutomaticRegistrationEnabled()
    {
    return iAutomaticRegistrationEnabled;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetProfileLockEnabled
//
// ---------------------------------------------------------------------------
//
TBool CWPSIPItem::GetProfileLockEnabled()
    {
    return iProfileLockEnabled;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetOutboundAddr
//
// ---------------------------------------------------------------------------
//
TDesC8& CWPSIPItem::GetOutboundAddr()
    {
    return *iOutboundAddr;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetPortNumberForOutboundConnection
//
// ---------------------------------------------------------------------------
//
TDesC8& CWPSIPItem::GetPortNumberForOutboundConnection()
    {
    return *iPortNumberForOutboundConnection;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetAuthenticationTypeForOutbound
//
// ---------------------------------------------------------------------------
//
TDesC8& CWPSIPItem::GetAuthenticationTypeForOutbound()
    {
    return *iAuthenticationTypeForOutbound;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetUsernameForOutbound
//
// ---------------------------------------------------------------------------
//
TDesC8& CWPSIPItem::GetUsernameForOutbound()
    {
    return *iUsernameForOutbound;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetPasswordForOutbound
//
// ---------------------------------------------------------------------------
//
TDesC8& CWPSIPItem::GetPasswordForOutbound()
    {
    return *iPasswordForOutbound;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetRealmForOutbound
//
// ---------------------------------------------------------------------------
//
TDesC8& CWPSIPItem::GetRealmForOutbound()
    {
    return *iRealmForOutbound;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetRegistrarAddr
//
// ---------------------------------------------------------------------------
//
TDesC8& CWPSIPItem::GetRegistrarAddr()
    {
    return *iRegistrarAddr;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetAuthenticationTypeForRegistrar
//
// ---------------------------------------------------------------------------
//
TDesC8& CWPSIPItem::GetAuthenticationTypeForRegistrar()
    {
    return *iAuthenticationTypeForRegistrar;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetUsernameForRegistrar
//
// ---------------------------------------------------------------------------
//
TDesC8& CWPSIPItem::GetUsernameForRegistrar()
    {
    return *iUsernameForRegistrar;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetPasswordForRegistrar
//
// ---------------------------------------------------------------------------
//
TDesC8& CWPSIPItem::GetPasswordForRegistrar()
    {
    return *iPasswordForRegistrar;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetRealmForRegistrar
//
// ---------------------------------------------------------------------------
//
TDesC8& CWPSIPItem::GetRealmForRegistrar()
    {
    return *iRealmForRegistrar;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetNapDef
//
// ---------------------------------------------------------------------------
//
CWPCharacteristic* CWPSIPItem::GetNapDef()
    {
    return iNapDef;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetProfileId
//
// ---------------------------------------------------------------------------
//
TUint32 CWPSIPItem::GetProfileId()
    {
    return iProfileId;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::GetRefId
//
// ---------------------------------------------------------------------------
//
TDesC8& CWPSIPItem::GetAppRef()
    {
    return *iAppRef;
    }
    
// ---------------------------------------------------------------------------
// CWPSIPSAPItem::GetIAPId
//
// ---------------------------------------------------------------------------
//
TUint32 CWPSIPItem::GetIAPId()
    {
    return iIAPId;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::StoreL()
// Saves the SIP settings by using SIP Profile API and SIP API. 
// ---------------------------------------------------------------------------
//
TUint32 CWPSIPItem::StoreL()
    {
    // __UHEAP_MARK; // For testing.
    DBG_PRINT( "CWPSIPItem::StoreL - begin" );

    CSIPProfile* tmpProfile = NULL;
    CSIPProfileRegistryObserver* cSIPProfileRegistryObserver = 
        CSIPProfileRegistryObserver::NewLC(); // CS:1
    CSIPManagedProfileRegistry* cSIPManagedProfileRegistry = 
        CSIPManagedProfileRegistry::NewLC( *cSIPProfileRegistryObserver );
        // CS:2

    // Preparing persistent managed profile for saving the SIP settings of the
    CSIPManagedProfile* cSIPManagedProfile = cSIPManagedProfileRegistry->
        CreateL( iSIPProfileTypeInfo ); 
           
    // Load existing profile if we are trying to set as default    
    if ( iProfileId && iSetAsDefault )
        {
        // Delete current profilecontent and load already saved profile.
        delete cSIPManagedProfile;
        tmpProfile = cSIPManagedProfileRegistry->ProfileL( iProfileId );
        cSIPManagedProfile = static_cast<CSIPManagedProfile*> ( tmpProfile );
        }
    CleanupStack::PushL( cSIPManagedProfile );// CS:3 

    // Skip setting providername if we are setting profile as default,
    // no need to check duplicate providers again.
    if ( !iSetAsDefault )
        {
        // Provider.
        cSIPManagedProfile->SetParameter( KSIPProviderName, 
            iProviderId->Des() );

        // For adding numbering to providername shown in settings UI.
        CheckDuplicateNameL( *cSIPManagedProfile, KErrNotFound );
        }

    if ( iNetwork->Des().Length() )
        {
        iSnapId = NetworkIdL( iNetwork->Des() );
        if ( KErrNotFound < iSnapId )
            {
            // Set SNAP ID.
            TInt err = cSIPManagedProfile->SetParameter( 
                KSIPSnapId, (TUint32)iSnapId );
            }
        else
            {
            // Set IAP ID.
            cSIPManagedProfile->SetParameter( KSIPAccessPointId, iIAPId );
            }
        }
    else
        {
        if ( KErrNotFound < iSnapId )
            {
            // Set SNAP ID.
            TInt err = cSIPManagedProfile->SetParameter( KSIPSnapId, 
                (TUint32)iSnapId );
            }
        else
            {
            // Set IAP ID.
            cSIPManagedProfile->SetParameter( KSIPAccessPointId, iIAPId );
            }
        }

    // Setting SIP Address-Of-Record/Public User Identity. There is only one 
    // AOR via OMA Client Provisioning.
    cSIPManagedProfile->SetParameter( KSIPUserAor, iAOR->Des() );

    // Set outbound proxy URI only if address part is present.
    if ( iOutboundAddr->Length() )
        {
        // Outbound proxy SIP-URI. The size of the final SIP-URI is calculated
        // with the maximum possible size. sip:host:port;transport
        HBufC8* bufURI = HBufC8::NewLC( 
            iOutboundAddr->Length()                    +
            KColon().Length()                          +
            iPortNumberForOutboundConnection->Length() +
            KTransport().Length()                      +
            iProtocolForOutboundConnection->Length()   +
            KLr().Length()
            ); // CS:4

        TPtr8 ptrBufURI( bufURI->Des() );

        // Outbound/P-CSCF proxy address;domain name or IP address.
        ptrBufURI.Copy( iOutboundAddr->Des() ); 
        // If the port number was received,
        // it is appended into the Request-URI.
        if ( iPortNumberForOutboundConnection->Length() > 0 ) // :port
            {
            ptrBufURI.Append( KColon );
            ptrBufURI.Append( iPortNumberForOutboundConnection->Des() );
            }
        // If the transport protocol was received, it is appended into the 
        // Request-URI.
        if ( iProtocolForOutboundConnection->Length() > 0 )
            {
            ptrBufURI.Append( KTransport );
            ptrBufURI.Append( iProtocolForOutboundConnection->Des() );
            }

        // Check if the loose routing parameter ";lr" should be
        // placed into the Request-URI.
        if ( iLooseRoutingForOutboundConnectionEnabled )
            {
            ptrBufURI.Append( KLr );
            }
        // Set outbound proxy/P-CSCF address.
        cSIPManagedProfile->SetParameter( 
            KSIPOutboundProxy, KSIPServerAddress, bufURI->Des() );
        CleanupStack::PopAndDestroy( bufURI ); // CS:3
        bufURI = NULL;
        }

    // Set signal compression enabled value.
    cSIPManagedProfile->SetParameter( KSIPSigComp, 
        iSignalCompressionEnabled );

    // Set security negotiation enabled value.
    cSIPManagedProfile->SetParameter( KSIPSecurityNegotiation,
        iSecurityNegotiationEnabled );
 
     if ( iSIPProfileTypeInfo.iSIPProfileClass == TSIPProfileTypeInfo::EOther
        || iSIPProfileTypeInfo.iSIPProfileClass == TSIPProfileTypeInfo::EIms )
        {
        cSIPManagedProfile->SetParameter( KSIPPrivateIdentity, 
            iUsernameForOutbound->Des() );
        }
    else
        {
        // Set outbound proxy username. Digest username.
        cSIPManagedProfile->SetParameter( KSIPOutboundProxy, 
            KSIPDigestUserName, iUsernameForOutbound->Des() );
        }

    // Set outbound proxy password. Digest password.
    cSIPManagedProfile->SetParameter( KSIPOutboundProxy, KSIPDigestPassword,
        iPasswordForOutbound->Des() );
 
    // Set realm for outbound.
    cSIPManagedProfile->SetParameter( KSIPOutboundProxy, KSIPDigestRealm,
        iRealmForOutbound->Des() );

    // Registrar SIP-URI.
    cSIPManagedProfile->SetParameter( KSIPRegistrar, KSIPServerAddress, 
        iRegistrarAddr->Des() );

    if ( iSIPProfileTypeInfo.iSIPProfileClass == TSIPProfileTypeInfo::EOther
        || iSIPProfileTypeInfo.iSIPProfileClass == TSIPProfileTypeInfo::EIms )
        {
        cSIPManagedProfile->SetParameter( KSIPPrivateIdentity, 
            iUsernameForRegistrar->Des() );
        }
    else
        {
        // Set registrar username. Digest username.
        cSIPManagedProfile->SetParameter( KSIPRegistrar, KSIPDigestUserName,
            iUsernameForRegistrar->Des() );
        }

    // Set registrar password. Digest password.
    cSIPManagedProfile->SetParameter( KSIPRegistrar, KSIPDigestPassword,
        iPasswordForRegistrar->Des() );

    // Set realm for registrar.
    cSIPManagedProfile->SetParameter( KSIPRegistrar, KSIPDigestRealm,
        iRealmForRegistrar->Des() );

    // Default parameter
    if ( iSetAsDefault )   
        {
        cSIPManagedProfile->SetParameter( KSIPDefaultProfile, ETrue );
        cSIPManagedProfile->SetParameter( KSIPProfileId, iProfileId );
        }

    // Profile lock parameter
    if ( iProfileLockEnabled )
        {
        cSIPManagedProfile->SetParameter( KSIPProfileLocked, ETrue );
        }

    // Signalling QoS parameter.
    cSIPManagedProfile->SetParameter( KSIPSoIpTOS, iSigQos );
    
    cSIPManagedProfile->SetParameter( KBearerType, iBearerId );
    
    // Stores SIP profile to permanent storage
    cSIPManagedProfileRegistry->SaveL( *cSIPManagedProfile );

    // get saved profile id
    cSIPManagedProfile->GetParameter( KSIPProfileId, iProfileId );
    CleanupStack::PopAndDestroy( cSIPManagedProfile );    
    CleanupStack::PopAndDestroy( cSIPManagedProfileRegistry );
    CleanupStack::PopAndDestroy( cSIPProfileRegistryObserver );
    cSIPManagedProfile = NULL;

    // for testing
    //__UHEAP_MARKEND;

    HBufC8* tmpId = HBufC8::NewL( KMaxCharsInTUint32 );
    _LIT8( KFormatTxt,"%u" );
    tmpId->Des().Format( KFormatTxt, iProfileId );
    delete iSavedProfileId;
    iSavedProfileId = NULL;
    iSetAsDefault = EFalse;
    iSavedProfileId = tmpId;
    tmpId = NULL;
    DBG_PRINT( "CWPSIPItem::StoreL - end" );
    return iProfileId;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SaveData
//
// ---------------------------------------------------------------------------
//
const TDesC8& CWPSIPItem::SaveData()
    {
    return *iSavedProfileId;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::CheckDuplicateNameL
// Checks if duplicate named sip profiles. Renames if same.
// ---------------------------------------------------------------------------
//
void CWPSIPItem::CheckDuplicateNameL( CSIPManagedProfile& aProfile, 
    const TInt& aProfileId )
    {
    // Array of pointers to all loaded SIP profiles.
    CArrayPtrFlat<CSIPManagedProfile>* profiles;
    CSIPProfileRegistryObserver* cSIPProfileRegistryObserver = 
        CSIPProfileRegistryObserver::NewLC(); // CS:1
    CSIPManagedProfileRegistry*  cSIPManagedProfileRegistry = 
        CSIPManagedProfileRegistry::NewLC( *cSIPProfileRegistryObserver );
        // CS:2

    RArray<TSIPProfileTypeInfo> profileTypes;
    CleanupClosePushL( profileTypes ); // CS:3

     // Get the supported profile types.
    cSIPManagedProfileRegistry->SupportedProfileTypesL( profileTypes );

    // Create profile pointer array.
    profiles = new ( ELeave ) CArrayPtrFlat<CSIPManagedProfile>( 5 );
    TCleanupItem cleanupItem( CWPSIPItem::CleanupArrayItem, profiles );
    CleanupStack::PushL( cleanupItem ); // CS:4

    RPointerArray<CSIPProfile> profilePointerArray;
    CleanupResetAndDestroyPushL( profilePointerArray ); // CS:5

    // Get all profiles based on profile types.
    const TInt profileTypeCount = profileTypes.Count();
    for ( TInt i = 0; i < profileTypeCount; i++ )
        {        
        cSIPManagedProfileRegistry->ProfilesL( profileTypes[i], 
            profilePointerArray );
        const TInt profileCount = profilePointerArray.Count();
        for ( TInt j = 0; j < profileCount; j++ )
            {
            CSIPManagedProfile* profile = 
                static_cast<CSIPManagedProfile*>( profilePointerArray[j] );
            profiles->AppendL( profile );
            }                    
        profilePointerArray.Reset();
        }
    CleanupStack::PopAndDestroy( &profilePointerArray ); // CS:4

    // Go through loaded profiles and check for name duplicates.
    const TDesC8* name;
    aProfile.GetParameter( KSIPProviderName, name );
    HBufC8* newName = HBufC8::NewLC( KMaxProviderNameLength ); // CS:5
    newName->Des().Copy( *name );
    const TInt count( profiles->Count() );
    for ( TInt n = 0; n < count; n++ )
        {
        // Skip profile currently being checked.
        if ( aProfileId != n )
            {
            const TDesC8* existingName;
            profiles->At( n )->GetParameter( KSIPProviderName, existingName );
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
                        profiles->At( m )->GetParameter( KSIPProviderName, 
                            existingName );
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
    // newName, profiles, &profileTypes, cSIPManagedProfileRegistry
    CleanupStack::PopAndDestroy( 5, cSIPProfileRegistryObserver ); // CS:0
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::CleanupArrayItem
// Cleans up an array.
// ---------------------------------------------------------------------------
//
void CWPSIPItem::CleanupArrayItem( TAny* aArray )
    {
    CArrayPtrFlat<CSIPManagedProfile>* tempArray = 
        static_cast<CArrayPtrFlat<CSIPManagedProfile>*>( aArray );
    if ( tempArray )
        {
        tempArray->ResetAndDestroy();
        }
    delete tempArray;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetToAppRefL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetToAppRefL( const TDesC8& aToAppRef )
    {
    delete iToAppRef;
    iToAppRef = NULL;
    iToAppRef = aToAppRef.AllocL();
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::ToAppRef
//
// ---------------------------------------------------------------------------
//
const TDesC8& CWPSIPItem::ToAppRef() const
    {
    return *iToAppRef;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetNetworkL
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetNetworkL( const TDesC& aNetwork )
    {
    delete iNetwork;
    iNetwork = NULL;
    iNetwork = aNetwork.AllocL();
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::Network
//
// ---------------------------------------------------------------------------
//
const TDesC& CWPSIPItem::Network() const
    {
    return *iNetwork;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetSnapId
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetSnapId( TInt aSnapId )
    {
    iSnapId = aSnapId;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::NetworkIdL
//
// ---------------------------------------------------------------------------
//
TInt CWPSIPItem::NetworkIdL( const TDesC& aName )
    {
    DBG_PRINT( "CWPSIPItem::NetworkIdL - begin" );
    TInt networkId( KErrNotFound );
    RCmManagerExt cmManager;
    cmManager.OpenL();
    CleanupClosePushL( cmManager ); // CS:1
    RArray<TUint32> destinations;
    CleanupClosePushL( destinations ); // CS:2
    cmManager.AllDestinationsL( destinations );
    const TInt destCount = destinations.Count();
    for ( TInt counter = 0; counter < destCount; counter++ )
        {
        RCmDestinationExt destination = cmManager.DestinationL( 
            destinations[counter] );
        CleanupClosePushL( destination ); // CS:3
        HBufC* destName = destination.NameLC(); // CS:4
        if ( 0 == aName.Compare( destName->Des() ) )
            {
            networkId = destinations[counter];
            CleanupStack::PopAndDestroy( 2, &destination ); // CS:2
            break;
            }
        CleanupStack::PopAndDestroy( 2, &destination ); // CS:2
        }
    CleanupStack::PopAndDestroy( 2, &cmManager ); // CS:0
    destinations.Close();
    DBG_PRINT( "CWPSIPItem::NetworkIdL - end" );
    return networkId;
    }

// ---------------------------------------------------------------------------
// CWPSIPItem::SetSigQos
//
// ---------------------------------------------------------------------------
//
void CWPSIPItem::SetSigQos( TUint32 aSigQos )
    {
    // We need to do bitshifting on the IP TOS, because it's the upper 6 bits
    // that are set and settings provide us the IP TOS as the lower 6 bits.
    // The lower 2 bits are reserver for explicit congestion notification.
    // See also more from:
    // Symbian Developer Library / in_sock.h Global variables / KSoIpTOS
    const TInt ipTosShift( 2 );
    const TUint32 tosBits( aSigQos << ipTosShift );
    iSigQos = tosBits;
    }


void CWPSIPItem::SetBearerID(TUint32 aBearerId)
{
iBearerId = aBearerId;
}

TUint32 CWPSIPItem:: GetBearerID()
{
return iBearerId; 
}
// End of file.
