/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles SIP settings in provisioning.
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <CWPAdapter.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include <wpsipadapter.rsg>
#include <utf.h>           // Unicode conversion
#include <WPAdapterUtil.h>
#include <sipmanagedprofileregistry.h>
#include <sipmanagedprofile.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>

#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include <cmconnectionmethoddef.h>

#include "CSIPProfileRegistryObserver.h"
#include "CWPSIPAdapter.h"      // own header
#include "CWPSIPItem.h"
#include "wpsipadapterdebug.h"

// CONSTANTS
_LIT( KSIPAdapterName,      "WPSIPAdapter" );
_LIT( KSIPAppID,            "w9010" );
_LIT8( KSIPAppID8,          "w9010" );
_LIT8( KDestinationNwAppId, "w904C" ); // Destination Network settings APPID

// SIP profile parameters, see w9010.txt
_LIT( KParmPTYPE,      "PTYPE");
_LIT( KParmPUID,       "PUID");
_LIT( KParmSIGNALCMPR, "SIGNALCMPR" );
_LIT( KParmSECNEG,     "SECNEG" );
_LIT( KParmLR,         "LR" );
_LIT( KParmLOCK,       "LOCK" );
_LIT( KParmAUTOREG,    "AUTOREG" );
_LIT( KParmSIGQOS,     "SIGQOS" );
_LIT( KParmBearerID,   "PBEARERID" );

// For other purposes.
const TUint32 KNotSet = 0xffffffff;

// ---------------------------------------------------------------------------
// CWPSIPAdapter::CWPSIPAdapter
// 
// ---------------------------------------------------------------------------
//
CWPSIPAdapter::CWPSIPAdapter() : CWPAdapter()
    {
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::ConstructL
// 
// ---------------------------------------------------------------------------
//
void CWPSIPAdapter::ConstructL()
    {
    DBG_PRINT( "CWPSIPAdapter::ConstructL - begin" );
    TFileName fileName;
    Dll::FileName( fileName );
    iDefaultName = WPAdapterUtil::ReadHBufCL( fileName, KSIPAdapterName, 
        R_QTN_SM_DEFAULT_NAME_SIP );
    iTitle = WPAdapterUtil::ReadHBufCL( fileName, KSIPAdapterName, 
        R_QTN_SM_HEAD_SIP );
    iProviderIdDefault = WPAdapterUtil::ReadHBufCL( fileName, KSIPAdapterName,
        R_QTN_SM_PROVIDER );
    iSIPIdForDefault = KNotSet;
    DBG_PRINT( "CWPSIPAdapter::ConstructL - end" );
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::NewL
// 
// ---------------------------------------------------------------------------
//
CWPSIPAdapter* CWPSIPAdapter::NewL()
    {
    CWPSIPAdapter* self = new( ELeave ) CWPSIPAdapter; 
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::~CWPSIPAdapter
// 
// ---------------------------------------------------------------------------
//
CWPSIPAdapter::~CWPSIPAdapter()
    {
    DBG_PRINT( "CWPSIPAdapter::~CWPSIPAdapter - begin" );
    delete iTitle;
    delete iProviderIdDefault;
    delete iDefaultName;
    delete iCurrentSIPItem;
    iDatas.ResetAndDestroy();
    iDatas.Close();
    iSIPIdArray.Reset();
    iSIPIdArray.Close();
    DBG_PRINT( "CWPSIPAdapter::~CWPSIPAdapter - end" );
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::ContextExtension
//
// ---------------------------------------------------------------------------
//
TInt CWPSIPAdapter::ContextExtension( MWPContextExtension*& aExtension )
    {
    aExtension = this;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::SaveDataL
//
// ---------------------------------------------------------------------------
//
const TDesC8& CWPSIPAdapter::SaveDataL( TInt aIndex ) const
    {
    return iDatas[aIndex]->SaveData();
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::DeleteL
//
// ---------------------------------------------------------------------------
//
void CWPSIPAdapter::DeleteL( const TDesC8& aSaveData )
    {
    TUint tId;
    TLex8 lex( aSaveData );
    TInt err( KErrNone );            
    err = lex.Val( tId );
    if ( KErrNone == err )
        {
        CSIPProfileRegistryObserver* cSIPProfileRegistryObserver = 
            CSIPProfileRegistryObserver::NewLC();
        CSIPManagedProfileRegistry*  cSIPManagedProfileRegistry  = 
            CSIPManagedProfileRegistry::NewLC( *cSIPProfileRegistryObserver );
        CSIPProfile* cProfile = NULL;
        cProfile = cSIPManagedProfileRegistry->ProfileL( tId );
        CleanupStack::PushL( cProfile );
        cSIPManagedProfileRegistry->DestroyL( *cProfile );
        CleanupStack::PopAndDestroy( cProfile );
        cProfile = NULL;
        CleanupStack::PopAndDestroy( cSIPManagedProfileRegistry );
        CleanupStack::PopAndDestroy( cSIPProfileRegistryObserver );
        }// if
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::Uid
//
// ---------------------------------------------------------------------------
//
TUint32 CWPSIPAdapter::Uid() const
    {
    return iDtor_ID_Key.iUid; 
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::DetailsL
//
// ---------------------------------------------------------------------------
//
TInt CWPSIPAdapter::DetailsL( TInt /*aItem*/, MWPPairVisitor& /*aVisitor */ )
    {
    return KErrNotSupported;
    }  

// ---------------------------------------------------------------------------
// CWPSIPAdapter::SummaryCount
//
// ---------------------------------------------------------------------------
//
TInt CWPSIPAdapter::ItemCount() const
    {
    return iDatas.Count();
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::SummaryTitle
//
// ---------------------------------------------------------------------------
//
const TDesC16& CWPSIPAdapter::SummaryTitle( TInt /*aIndex*/ ) const
    {
    return *iTitle;
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::SummaryText
//
// ---------------------------------------------------------------------------
//
const TDesC16& CWPSIPAdapter::SummaryText( TInt aIndex ) const
    {
    // If there is no value for NAME parameter the default value is set here.
    if ( iDatas[aIndex]->GetName().Length() == 0 )
        {
        TRAP_IGNORE( iDatas[aIndex]->SetNameL( iDefaultName->Des() ) );
        }
    return iDatas[aIndex]->GetName();
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::SaveL
//
// ---------------------------------------------------------------------------
//
void CWPSIPAdapter::SaveL( TInt aIndex )
    {
    DBG_PRINT( "CWPSIPAdapter::SaveL - begin" );
    // Getting the SIP item (SIP profile data) at given index.
    CWPSIPItem* sipItem = iDatas[aIndex];

    if ( sipItem->GetNapDef() )
        {
        // Get WAP ID.
        TPckgBuf<TUint32> uid;
        uid.Copy( sipItem->GetNapDef()->Data( 0 ) );
        
        TUint32 wapId( uid() );
        // Get corresponding IAP ID.
        TUint32 iapId = IapIdFromWapIdL( wapId );
        sipItem->SetIAPId( iapId );
        }

    // If the PROVIDER-ID was not received the default value is set.
    if ( sipItem->GetProviderId().Length() == 0 )
        {
        HBufC8* tmpProviderId8 = HBufC8::NewLC( 
            iProviderIdDefault->Length() ); // CS:1
        TPtr8   ptrProviderId( tmpProviderId8->Des() );
        ptrProviderId.Copy( iProviderIdDefault->Des() );
        sipItem->SetProviderIdL( tmpProviderId8->Des() );
        CleanupStack::PopAndDestroy( tmpProviderId8 ); // CS:0
        }

    sipItem->StoreL();

    // Set WLAN availability state. Setting WLAN availability is done in
    // SavingFinalizedL().
    if ( sipItem->GetAutomaticRegistrationEnabled() )
        {
        iSIPIdArray.AppendL( sipItem->GetProfileId() );
        }
    DBG_PRINT( "CWPSIPAdapter::SaveL - end" );
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::CanSetAsDefault
//
// ---------------------------------------------------------------------------
//
TBool CWPSIPAdapter::CanSetAsDefault( TInt /* aIndex */ ) const
    {
    TBool canSetAsDefault( 0 );
    TRAP_IGNORE( canSetAsDefault = CheckSettingAsDefaultL() );
    return canSetAsDefault;
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::CheckSettingAsDefaultL
//
// ---------------------------------------------------------------------------
//
TBool CWPSIPAdapter::CheckSettingAsDefaultL() const
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

    // Create the profile pointer array.
    profiles = new ( ELeave ) CArrayPtrFlat<CSIPManagedProfile>( 5 );    
    TCleanupItem cleanupItem( CWPSIPItem::CleanupArrayItem, profiles );
    CleanupStack::PushL( cleanupItem ); // CS:4

    TInt i( 0 );
    RPointerArray<CSIPProfile> profilePointerArray;
    CleanupResetAndDestroyPushL( profilePointerArray ); // CS:5

    // Get all profiles based on profile types
    for ( i = 0; i < profileTypes.Count(); i++ )
        {        
        cSIPManagedProfileRegistry->ProfilesL( profileTypes[i], 
            profilePointerArray );
        TInt j( 0 );
        for ( j = 0; j < profilePointerArray.Count(); j++ )
            {
            CSIPManagedProfile* profile = 
                static_cast<CSIPManagedProfile*>( profilePointerArray[j] );
            profiles->AppendL( profile ); 
            }                    
        profilePointerArray.Reset();
        }
    TInt profileCount = profiles->Count();   

    // &profilePointerArray, profiles, &profileTypes,
    // cSIPManagedProfileRegistry, cSIPProfileRegistryObserver
    CleanupStack::PopAndDestroy( 5, cSIPProfileRegistryObserver ); // CS:0

    if ( profileCount > 1 )
        {
        // Adapter supports setting profile as default when there are at least
        // one profile stored before.
        return ETrue;
        }
    else
        {
        // First profile always gets default, no need to ask question in UI.
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::SetAsDefaultL
//
// ---------------------------------------------------------------------------
//
void CWPSIPAdapter::SetAsDefaultL( TInt /* aIndex */ )
    {
    // Set messages first profile as default
    iDatas[0]->SetAsDefaultL();
    iDatas[0]->StoreL();
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::VisitL
//
// ---------------------------------------------------------------------------
//
void CWPSIPAdapter::VisitL( CWPCharacteristic& aCharacteristic )
    {
    switch( aCharacteristic.Type() )
        {
        case KWPApplication:
            iCurrentCharacteristic = KWPApplication;
            iCurrentSIPItem = CWPSIPItem::NewL();
            aCharacteristic.AcceptL( *this );

            if ( iAppID == KSIPAppID )
                {
                User::LeaveIfError( iDatas.Append( iCurrentSIPItem ) );
                }
            else
                {
                delete iCurrentSIPItem;
                }
            iCurrentSIPItem = NULL;
            break;

        case KWPResource:
            iCurrentCharacteristic = KWPResource;
			if ( iCurrentSIPItem )
                {                
                aCharacteristic.AcceptL( *this );
                }
            break;

        case KWPNapDef:
            iCurrentCharacteristic = KWPNapDef;
			if ( iCurrentSIPItem )
                {                
                aCharacteristic.AcceptL( *this );
                }
            break;

        case KWPAppAddr:
            iCurrentCharacteristic = KWPAppAddr;
            if ( iCurrentSIPItem )
                {                
                aCharacteristic.AcceptL( *this );
                }
            break;

        case KWPPort:
            iCurrentCharacteristic = KWPPort;
            if ( iCurrentSIPItem )
                {                
                aCharacteristic.AcceptL( *this );
                }
            break;

        case KWPAppAuth:
            iCurrentCharacteristic = KWPAppAuth;
			if ( iCurrentSIPItem )
                {                
                aCharacteristic.AcceptL( *this );
                }
            break;

        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::VisitL
//
// ---------------------------------------------------------------------------
//
void CWPSIPAdapter::VisitL( CWPParameter& aParameter )
    {
    DBG_PRINT("CWPSIPAdapter::VisitL( parameter ) - begin");

    if ( !iCurrentSIPItem )
        {
        User::Leave( KErrGeneral );
        }
    // tmpValue is created for holding value converted from Unicode to UTF8.
    HBufC8* tmpValue;
    tmpValue = HBufC8::NewL( aParameter.Value().Length() );
    CleanupStack::PushL( tmpValue ); // CS:1
    TPtr8 ptrTmpValue( tmpValue->Des() );
    ptrTmpValue.Copy( aParameter.Value() );

    switch ( aParameter.ID() )
        {
        case EWPParameterAppID:
            {
		    iAppID.Set( aParameter.Value() );
            break;
            }
        // Here case 0 are handled the SIP parameters that are extensions to 
        // OMA Client Provisioning parameter set.
        case 0:
            {
            if ( aParameter.Name() == KParmPTYPE )
                {
                iCurrentSIPItem->SetProfileTypeL( tmpValue->Des() );
                }
            else if ( aParameter.Name() == KParmPUID && 
                iCurrentSIPItem->GetAOR().Length() == 0 )
                {
                iCurrentSIPItem->SetAORL( tmpValue->Des() );
                }
            else if ( aParameter.Name() == KParmSIGNALCMPR && 
                iCurrentSIPItem->GetSignalCompressionEnabled() == EFalse )
                {
                iCurrentSIPItem->SetSignalCompressionEnabled( ETrue );
                }
            else if ( aParameter.Name() == KParmSECNEG && 
                iCurrentSIPItem->GetSecurityNegotiationEnabled() == EFalse )
                {
                iCurrentSIPItem->SetSecurityNegotiationEnabled( ETrue );
                }
            // Extensions to OMA parameters in APPDDR characteristics    
            else if ( aParameter.Name() == KParmLR && iCurrentSIPItem->
                GetLooseRoutingForOutboundConnectionEnabled() == EFalse ) 
                {
                iCurrentSIPItem->SetLooseRoutingForOutboundConnectionEnabled( 
                    ETrue );
                }
            else if ( aParameter.Name() == KParmLOCK ) 
                {
                iCurrentSIPItem->SetProfileLockEnabled( ETrue );
                }
            else if ( aParameter.Name() == KParmAUTOREG && iCurrentSIPItem->
                GetAutomaticRegistrationEnabled() == EFalse ) 
                {
                iCurrentSIPItem->SetAutomaticRegistrationEnabled( ETrue );
                }
            else if ( aParameter.Name() == KParmSIGQOS )
                {
                TUint32 parmVal;
                TLex8 lex( tmpValue->Des() );
                TInt err( KErrNone );
                err = lex.Val( parmVal, EDecimal );
                if ( KErrNone == err )
                    {
                    iCurrentSIPItem->SetSigQos( parmVal );
                    }
                }
            
            else if(aParameter.Name() == KParmBearerID )
                {
                TInt bearerid;
                TLex8 lex( tmpValue->Des() );
                User::LeaveIfError( lex.Val( bearerid ) );
                iCurrentSIPItem->SetBearerID((TUint32) bearerid );
                }
            break;
            }
        case EWPParameterAppRef:
            {
            if ( iCurrentSIPItem->GetAppRef().Length() == 0 )
                {
                iCurrentSIPItem->SetAppRefL( tmpValue->Des() );
                }// if
            break;
            }
        case EWPParameterURI:
            {
            if ( iCurrentCharacteristic == KWPResource && 
                iCurrentSIPItem->GetRegistrarAddr().Length() == 0 )
                {
                iCurrentSIPItem->SetRegistrarAddrL( tmpValue->Des() );
                }// if
            break;
            }

        case EWPParameterToNapID:
            {
            if ( iCurrentCharacteristic == KWPApplication && 
                iCurrentSIPItem->GetNapDefName().Length() == 0 )
                {
                iCurrentSIPItem->SetNapDefNameL( tmpValue->Des() );
                }// if
            break;
            }
        case EWPParameterName:
            {
            if ( iCurrentSIPItem->GetName().Length() == 0 )
                {
                iCurrentSIPItem->SetNameL( aParameter.Value() );
                }
            break;
            }

        case EWPParameterProviderID:
            {
            if ( iCurrentSIPItem->GetProviderId().Length() == 0 )
                {
                iCurrentSIPItem->SetProviderIdL( tmpValue->Des() );
                }
        break;
            }

        case EWPParameterAProtocol:
            {
            if ( iCurrentSIPItem->GetProtocolForOutboundConnection().Length()
                == 0 )
                {
                iCurrentSIPItem->SetProtocolForOutboundConnectionL( 
                    tmpValue->Des() );
                }
 			break;
            }

        case EWPParameterAddr:
            {
            if ( iCurrentCharacteristic == KWPAppAddr && 
                iCurrentSIPItem->GetOutboundAddr().Length() == 0 )
                {
                iCurrentSIPItem->SetOutboundAddrL( tmpValue->Des() );
                }
			break;
            }

        case EWPParameterPortNbr:
            {
            if ( iCurrentCharacteristic == KWPPort && iCurrentSIPItem->
                GetPortNumberForOutboundConnection().Length() == 0 )
                {
                iCurrentSIPItem->SetPortNumberForOutboundConnectionL(
                    tmpValue->Des() );
                }
          break;
            }

        case EWPParameterAAuthType:
            {
            if ( iCurrentCharacteristic == KWPAppAuth && iCurrentSIPItem->
                GetAuthenticationTypeForOutbound().Length() == 0 )
                {
                iCurrentSIPItem->SetAuthenticationTypeForOutboundL( 
                    tmpValue->Des() );
                }
            else if ( iCurrentCharacteristic == KWPResource && 
                iCurrentSIPItem->GetAuthenticationTypeForRegistrar().Length()
                == 0 )
                {
                iCurrentSIPItem->SetAuthenticationTypeForRegistrarL( 
                    tmpValue->Des() );
                }
            break;
            }

        case EWPParameterAAuthName:
            {
            if ( iCurrentCharacteristic == KWPAppAuth &&
                iCurrentSIPItem->GetUsernameForOutbound().Length() == 0 )
                {
                iCurrentSIPItem->SetUsernameForOutboundL( tmpValue->Des() );
                }
            else if ( iCurrentCharacteristic == KWPResource && 
                iCurrentSIPItem->GetUsernameForRegistrar().Length() == 0 )
                {
                iCurrentSIPItem->SetUsernameForRegistrarL( tmpValue->Des() );
                }
            break;
            }

        case EWPParameterAAuthSecret:
            {
            if ( iCurrentCharacteristic == KWPAppAuth &&
                iCurrentSIPItem->GetPasswordForOutbound().Length() == 0 )
                {
                iCurrentSIPItem->SetPasswordForOutboundL(tmpValue->Des());
                }
            else if ( iCurrentCharacteristic == KWPResource && 
                iCurrentSIPItem->GetPasswordForRegistrar().Length() == 0 )
                {
                iCurrentSIPItem->SetPasswordForRegistrarL( tmpValue->Des() );
                }
            break;
            }

        case EWPParameterAAuthData:
            {
            if ( iCurrentCharacteristic == KWPAppAuth && 
                iCurrentSIPItem->GetRealmForOutbound().Length() == 0 )
                {
                iCurrentSIPItem->SetRealmForOutboundL(tmpValue->Des());
                }
            else if ( iCurrentCharacteristic == KWPResource && 
                iCurrentSIPItem->GetRealmForRegistrar().Length() == 0 )
                {
                iCurrentSIPItem->SetRealmForRegistrarL( tmpValue->Des() );
                }
            break;
            }

        case EWPParameterToAppRef:
            {
            if ( !iCurrentSIPItem->ToAppRef().Length() )
                {
                iCurrentSIPItem->SetToAppRefL( tmpValue->Des() );
                }
            break;
            }

        case EWPParameterNetwork:
            {
            if ( !iCurrentSIPItem->Network().Length() )
                {
                iCurrentSIPItem->SetNetworkL( aParameter.Value() );
                }
            break;
            }

        default:
            break;
        }
    CleanupStack::PopAndDestroy( tmpValue );  // CS:0
    DBG_PRINT("CWPSIPAdapter::VisitL( parameter ) - end");
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::VisitLinkL
//
// ---------------------------------------------------------------------------
//
void CWPSIPAdapter::VisitLinkL( CWPCharacteristic& aLink )
    {
    DBG_PRINT("CWPSIPAdapter::VisitLinkL - begin");
    switch( aLink.Type() )
        {
        case KWPNapDef:
            {            
            if ( !iCurrentSIPItem->GetNapDef() )
                {
                iCurrentSIPItem->SetNapDef( &aLink );
                }
            break;
            }
        default:
            break;
        }
    DBG_PRINT("CWPSIPAdapter::VisitLinkL - end");
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::GetSavingInfoL
//
// ---------------------------------------------------------------------------
//
void CWPSIPAdapter::GetSavingInfoL( TInt aIndex, 
                                    RPointerArray<HBufC8>& aSavingInfo )
    {
    // APPID into place [0].
    aSavingInfo.AppendL( KSIPAppID8().AllocL() );
    // APPREF into place [1].
    aSavingInfo.AppendL( iDatas[aIndex]->GetAppRef().AllocL() );
    // SIP Profile id into place [2].
    aSavingInfo.AppendL( iDatas[aIndex]->SaveData().AllocL() );
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::SavingFinalizedL()
//
// ---------------------------------------------------------------------------
//
void CWPSIPAdapter::SavingFinalizedL()
    {
    DBG_PRINT( "CWPSIPAdapter::SavingFinalizedL - begin" );
    CSIPProfileRegistryObserver* cSIPProfileRegistryObserver = 
        CSIPProfileRegistryObserver::NewLC();
    CSIPManagedProfileRegistry* cSIPManagedProfileRegistry  = 
        CSIPManagedProfileRegistry::NewLC( *cSIPProfileRegistryObserver );

    // Go through SIP profiles and set autoregistration if needed.
    // Autoregistrable profile IDs are in iSIPIdArray.
    for ( TInt sipIdCounter = 0; sipIdCounter < iSIPIdArray.Count(); 
        sipIdCounter++ )
        {
        CSIPProfile* cProfile = NULL;
        cProfile = cSIPManagedProfileRegistry->ProfileL( 
            iSIPIdArray[sipIdCounter] );
        CSIPManagedProfile* cSIPManagedProfile = 
            static_cast<CSIPManagedProfile*>( cProfile );
        cSIPManagedProfile->SetParameter( KSIPAutoRegistration, ETrue );
        cSIPManagedProfileRegistry->SaveL( *cSIPManagedProfile );
        }
    iSIPIdArray.Reset(); 
    CleanupStack::PopAndDestroy( cSIPManagedProfileRegistry );
    CleanupStack::PopAndDestroy( cSIPProfileRegistryObserver );
    DBG_PRINT( "CWPSIPAdapter::SavingFinalizedL - end" );
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::IapIdFromWapIdL
// Returns IAP ID from WAP ID.
// ---------------------------------------------------------------------------
//
TUint32 CWPSIPAdapter::IapIdFromWapIdL( TUint32 aWapId )
    {
    DBG_PRINT( "CWPSIPAdapter::IapIdFromWapIdL - begin" );
    RCmManagerExt cmManager;
    cmManager.OpenLC(); // CS:1
    RArray<TUint32> iapIds;

    TUint32 iapId( 0 );

    // First get all free IAP ID's.
    cmManager.ConnectionMethodL( iapIds );
    CleanupClosePushL( iapIds );  // CS:2

    // Then get IAP ID's from all destinations.
    RArray<TUint32> destIds;
    cmManager.AllDestinationsL( destIds );
    CleanupClosePushL( destIds );  // CS:3
    
    TInt destCount = destIds.Count();
    
    for ( TInt destIndex = 0; destIndex < destCount; destIndex++ )
        {
        RCmDestinationExt dest = cmManager.DestinationL( 
            destIds[destIndex] );
        
        CleanupClosePushL( dest ); // CS:4
        TInt cmCount = dest.ConnectionMethodCount();
        for ( TInt cmIndex = 0; cmIndex < cmCount; cmIndex++ )
            {
            TUint32 apId = dest.ConnectionMethodL( 
                cmIndex ).GetIntAttributeL( CMManager::ECmIapId );
            iapIds.AppendL( apId );
            }
        CleanupStack::PopAndDestroy( &dest ); // CS:3
        }
    // Finally, go through all connection methods and find correct IAP ID.
    const TInt cmCount = iapIds.Count();
    for ( TInt counter = 0; counter < cmCount; counter++ )
        {
        TUint32 id = cmManager.GetConnectionMethodInfoIntL( 
            iapIds[counter], CMManager::ECmWapId );

        if ( id == aWapId )
            {
            iapId = iapIds[counter];
            // No need to go through rest of IAPs.
            break;
            }
        }
    
    // PopAndDestroy destIds, iapIds, cmManager.
    CleanupStack::PopAndDestroy( 3, &cmManager ); // CS:0

    DBG_PRINT( "CWPSIPAdapter::IapIdFromWapIdL - end" );
    return iapId;
    }

// ---------------------------------------------------------------------------
// CWPSIPAdapter::SettingsSavedL
//
// ---------------------------------------------------------------------------
//
void CWPSIPAdapter::SettingsSavedL( const TDesC8& aAppIdOfSavingItem,
    const TDesC8& aAppRef, const TDesC8& aStorageIdValue )
    {
    DBG_PRINT( "CWPSIPAdapter::SettingsSavedL - begin" );
    if ( 0 == aAppIdOfSavingItem.Compare( KDestinationNwAppId() ) )
        {
        TInt snapId;
        TLex8 lex( aStorageIdValue );
        User::LeaveIfError( lex.Val( snapId ) );
        for ( TInt counter = 0; counter < iDatas.Count(); counter++ )
            {
            if ( 0 == iDatas[counter]->ToAppRef().Compare( aAppRef ) )
                {
                // If SIP settings are already saved, i.e. profile ID exists,
                // set SNAP ID to existing profile.
                if ( iDatas[counter]->GetProfileId() )
                    {
                    CSIPProfileRegistryObserver* observer = 
                        CSIPProfileRegistryObserver::NewLC(); // CS:1
                    CSIPManagedProfileRegistry* registry  = 
                        CSIPManagedProfileRegistry::NewLC( *observer );// CS:2
                    CSIPProfile* profile = NULL;
                    profile = registry->ProfileL( 
                        iDatas[counter]->GetProfileId() );
                    CSIPManagedProfile* managedProfile = 
                        static_cast<CSIPManagedProfile*>( profile );
                    managedProfile->SetParameter( 
                        KSIPSnapId, (TUint32)snapId );
                    registry->SaveL( *managedProfile );
                    CleanupStack::PopAndDestroy( 2, observer ); // CS:0
                    }
                else
                    {
                    iDatas[counter]->SetSnapId( snapId );
                    }
                }
            }
        }
    DBG_PRINT( "CWPSIPAdapter::SettingsSavedL - end" );
    }

//  End of File
