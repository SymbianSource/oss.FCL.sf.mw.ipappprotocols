/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The item list of profile setting view
*
*/


// INCLUDE FILES

#include    "sipsettlistsipprofsetmodel.h"
#include    "sipsettlistsipprofsetitemlist.h"
#include    "sipsettlistsipprofsetaplistitem.h"
#include    "sipsettlistsipprofsetdestlistitem.h"
#include    "sipsettlistsipprofsettypelistitem.h"
#include    "sipsettlistsipprofsetprovlistitem.h"
#include    "sipsettlistsipsrvtextsetitem.h"
#include    "sipsettlistsipprxsetview.h"
#include    "sipsettlistsipregsetview.h"
#include    "sipserversettingvalidator.h"
#include    "viewid.h"
#include    <aknsettingitemlist.h>
#include    <gssipsettingspluginrsc.rsg> //GUI Resource
#include    <eikenv.h>
#include    <aknViewAppUi.h>
#include    <aknview.h>
#include    <eikenv.h>
#include    <akntitle.h> 
#include    <eikspane.h>
#include    <StringLoader.h>
#include    <aknnotewrappers.h>
#include    <eiklbi.h>
#include    "gssippluginlogger.h"

// LOCAL CONSTS

const TInt KProxyServerIndex = 8;
const TInt KRegistrarServerIndex = 9;
const TInt KInitialIndex = -1;
const TInt KServiceProfileIETF = 0;
const TInt KServiceProfile3GPP = 1;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetItemList::CSIPSettListSIPProfSetItemList
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPProfSetItemList::CSIPSettListSIPProfSetItemList(
    CSIPSettListSIPProfSetModel* aModel,
    CAknTitlePane* aPane,
    TSIPSettingsData& aData ) :
    iModel( aModel ),
    iPane( aPane ),
    iProfileData( aData ),
    iDestIndex( KInitialIndex ),
    iAPIndex( KInitialIndex )    
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetItemList::CSIPSettListSIPProfSetItemList" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetItemList::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetItemList::ConstructL( 
    CCoeControl* aParent )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetItemList::ConstructL" )
    SetContainerWindowL( *aParent );    
    ConstructFromResourceL( R_SIP_SETTING_VIEW_ITEM_LIST );       

    ListBox()->CreateScrollBarFrameL( ETrue );
    ListBox()->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    ListBox()->View()->ItemDrawer()->SetFlags( CTextListItemDrawer::ESingleClickEnabled );
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetItemList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPProfSetItemList* CSIPSettListSIPProfSetItemList::NewL(
    CCoeControl* aParent,
    CSIPSettListSIPProfSetModel* aModel,
    CAknTitlePane* aPane )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetItemList::NewL" )
    CSIPSettListSIPProfSetItemList* self = 
        new ( ELeave ) CSIPSettListSIPProfSetItemList( 
            aModel, aPane, aModel->Settings() );
    
    CleanupStack::PushL( self );
    self->ConstructL( aParent );
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CSIPSettListSIPProfSetItemList::~CSIPSettListSIPProfSetItemList()
    {  
    __GSLOGSTRING("CSIPSettListSIPProfSetItemList::~CSIPSettListSIPProfSetItemList" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetItemList::CreateSettingItemL
// Creates the items to the list
// -----------------------------------------------------------------------------
//
CAknSettingItem* CSIPSettListSIPProfSetItemList::CreateSettingItemL( 
    TInt aSettingId )
    { 
    __GSLOGSTRING("CSIPSettListSIPProfSetItemList::CreateSettingItemL" )
    CAknSettingItem* item = NULL;

    switch ( aSettingId )
        {
        case EGSItemProviderName:
            iProviderName = new ( ELeave ) 
                CSIPSettListSIPProfSetProvListItem( EGSItemProviderName, 
                                                    iProfileData.iProviderName,
                                                    iPane );
            item = iProviderName;            
            break;

        case EGSItemServiceProfile:            
            iServiceProfile = new ( ELeave )
                CSIPSettListSIPProfSetTypeListItem( EGSItemServiceProfile, 
                                             iProfileData.iServiceProfileIndex,
                                             iModel );            
            item = iServiceProfile;                      
            break;

        case EGSItemDefaultDest:            
            iDefaultDest = new ( ELeave )
                CSIPSettListSIPProfSetDestListItem( EGSItemDefaultDest, 
                                                    iDestIndex, 
                                                    iModel );
            item = iDefaultDest;            
            break;

        case EGSItemDefaultAP:            
            iDefaultAP = new ( ELeave )
                CSIPSettListSIPProfSetAPListItem( EGSItemDefaultAP, 
                                                    iAPIndex, 
                                                    iModel );
            if( iDestIndex != KUnknownAPUID )
                {
                iDefaultAP->SetHidden( ETrue ) ;
              
                }
            item = iDefaultAP;            
            break;

        case EGSItemUsername:
            iUsername = new ( ELeave ) CSIPSettListSIPSrvTextSetItem(
                EGSItemUsername, iProfileData.iUsername, 
                SIPServerSettingValidator::ValidateServerUsernameL );            
            item = iUsername;
            break;

        case EGSItemCompression:
            iCompression = new ( ELeave )
                CAknEnumeratedTextPopupSettingItem(
                    EGSItemCompression, iProfileData.iCompression );
            item = iCompression;
            break;

        case EGSItemRegistration:
            iRegistration = new ( ELeave )
                CAknEnumeratedTextPopupSettingItem(
                    EGSItemRegistration, iProfileData.iRegistrationMode );
            item = iRegistration;
            break;

        case EGSItemSecurity:
            iSecurity = new ( ELeave )
                CAknEnumeratedTextPopupSettingItem(
                    EGSItemSecurity, iProfileData.iSecurity );
            item = iSecurity;
            break;

        case EGSItemProxyServer:
            iProxyServer = new ( ELeave ) CAknBigSettingItemBase(
                EGSItemProxyServer );
            item = iProxyServer;
            break;

        case EGSItemRegistrarServer:
            iRegistrarServer = new ( ELeave ) CAknBigSettingItemBase(
                EGSItemRegistrarServer );
            item = iRegistrarServer;
            break;
               
        default:
            // Leave if no matching ID is found
            User::Leave( KErrNotSupported );
            break;
        }
    
    return item;
    }          

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetItemList::HandleListBoxEventL
// Manages list box events
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetItemList::HandleListBoxEventL( 
    CEikListBox* aListBox,
    TListBoxEvent aEventType )
    { 
    __GSLOGSTRING("CSIPSettListSIPProfSetItemList::HandleListBoxEventL Start" )
    if ( aEventType == EEventEnterKeyPressed ||
        aEventType == EEventItemSingleClicked ) 
        {
        // If currently shown profile is locked, show a note and return.
        TInt currentIndex = aListBox->CurrentItemIndex();
    currentIndex = SettingItemArray()->ItemIndexFromVisibleIndex( currentIndex );
        if ( iModel->ProfileLocked() &&
            currentIndex != KProxyServerIndex && 
            currentIndex != KRegistrarServerIndex )
            {
            HBufC* lockedText = 
                StringLoader::LoadLC( R_QTN_SIP_INFONOTE_PROFILE_LOCKED );
            CAknInformationNote* lockedNote = new (ELeave) CAknInformationNote();
            lockedNote->ExecuteLD( *lockedText );
            CleanupStack::PopAndDestroy( lockedText );
            return;
            }

        if ( currentIndex == KProxyServerIndex )
            {
            CAknViewAppUi* appUi = 
                static_cast<CAknViewAppUi*>( iEikonEnv->AppUi() );
            // Store settings to variables before changing view
            StoreSettingsL();
            appUi->ActivateLocalViewL( KSettListSIPProxyServerView );
            return;
            }
        else if ( currentIndex == KRegistrarServerIndex )
            {
            CAknViewAppUi* appUi = 
                static_cast<CAknViewAppUi*>( iEikonEnv->AppUi() );
            // Store settings to variables before changing view
            StoreSettingsL();
            appUi->ActivateLocalViewL( KSettListSIPRegistrarServerView );
            return;
            }
        else if ( currentIndex == EGSItemServiceProfile ||
            currentIndex == EGSItemCompression ||
            currentIndex == EGSItemSecurity )
            {
            SetNextSettingItemL( currentIndex );
            aListBox->ActivateL();    
            aListBox->HandleItemAdditionL();
            return;
            }
        else    
            {
            // Do nothing here, HandleListBoxEventL() is executed
            }
        }         
    CAknSettingItemList::HandleListBoxEventL( aListBox, aEventType ); 
    __GSLOGSTRING("CSIPSettListSIPProfSetItemList::HandleListBoxEventL End" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetItemList::EditItemL
// Manages list box events
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetItemList::EditItemL( 
    TInt /*aIndex*/, 
    TBool aCalledFromMenu )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetItemList::EditItemL Start" )
    TInt index = ListBox()->CurrentItemIndex(); 
    
    index = SettingItemArray()->ItemIndexFromVisibleIndex(index);
    // If currently shown profile is locked, show a note and return.
    if ( iModel->ProfileLocked() &&
        index != KProxyServerIndex && index != KRegistrarServerIndex )
        {
        HBufC* lockedText = 
            StringLoader::LoadLC( R_QTN_SIP_INFONOTE_PROFILE_LOCKED );
        CAknInformationNote* lockedNote = new (ELeave) CAknInformationNote();
        lockedNote->ExecuteLD( *lockedText );
        CleanupStack::PopAndDestroy( lockedText );
        return;
        }
    
    if ( index == KProxyServerIndex || index == KRegistrarServerIndex )
        {
        HandleListBoxEventL( ListBox(), EEventEnterKeyPressed );
        }
    else if ( index == EGSItemProviderName )
        {
        HBufC* defValue = StringLoader::LoadLC( R_QTN_DEF_PROFILE_NAME );
        HBufC* oldValue = HBufC::NewLC( KMaxSIPProviderNameLength );
        oldValue->Des().Copy( iProfileData.iProviderName );                
                
        TBool cancel( EFalse );
        while ( !cancel )
            {
            // Show setting page and get new profile name.
            CAknSettingItemList::EditItemL( index, aCalledFromMenu );    
            iProviderName->StoreL(); 
        
            AknTextUtils::StripCharacters( 
                iProfileData.iProviderName, KAknStripListControlChars );
            
            // Show Title info using new value
            CEikStatusPane* sp = iAvkonAppUi->StatusPane();
            CAknTitlePane* title = static_cast<CAknTitlePane*>( 
                sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
            title->SetTextL( *oldValue );
            
            HBufC* newName = HBufC::NewLC( KMaxSIPProviderNameLength );
            newName->Des().Copy( iProfileData.iProviderName );  
            
            // If both texts are same, assume that we pressed cancel.
            if ( !iProfileData.iProviderName.Compare( oldValue->Des() ) )
                {
                title->SetTextL( *newName );
                cancel = ETrue;
                }
            else
                {
                // Check that new name is unique. If not, show confirmation
                // query for further actions.
                if ( !iModel->CheckDuplicateNameL( iProfileData.iProviderName ) )
                    {
                    CAknQueryDialog* query = new( ELeave ) 
                        CAknQueryDialog( CAknQueryDialog::ENoTone );
                    query->PrepareLC( R_SIP_SERVICE_PROVIDER_CONF_QUERY );
                    HBufC* string = StringLoader::LoadL( 
                        R_QTN_SIP_ITEMS_WITH_SAME_NAME, newName->Des() );
                    CleanupStack::PushL( string );
                    query->SetPromptL( *string );
                    CleanupStack::PopAndDestroy( string );
                    
                    // Show confirmation query.
                    if ( query->RunLD() )
                        {
                        // Copy old text back to the editor.
                        iProfileData.iProviderName.Copy( oldValue->Des() );
                        }
                    else
                        {
                        iProfileData.iProviderName.Copy( oldValue->Des() );
                        cancel = ETrue;
                        }
                    
                    iProviderName->LoadL();
                    }
                else
                    {
                    title->SetTextL( *newName );
                    cancel = ETrue;
                    }   
                }
            CleanupStack::PopAndDestroy( newName );   
            }
   
        // If canceled, copy setting back to right place.
        if ( !iProfileData.iProviderName.Length() )
            {
            iProfileData.iProviderName.Copy( oldValue->Des() );
            iProviderName->LoadL(); 
            }
        
        CleanupStack::PopAndDestroy( oldValue );            
        CleanupStack::PopAndDestroy( defValue );
        iProviderName->LoadL();
        iProviderName->UpdateListBoxTextL();
        }
    // Remove nonallowed characters from setting data if needed.    
    else if ( index == EGSItemUsername )
        {
        CAknSettingItemList::EditItemL( index, aCalledFromMenu );    
        iUsername->StoreL(); 
        
        AknTextUtils::StripCharacters( 
            iProfileData.iUsername, KAknStripListControlChars );
        
        iUsername->LoadL();
        iUsername->UpdateListBoxTextL();
        }
    // Display information note if there are no destinations defined.    
    else if (index == EGSItemDefaultDest && (iModel->DestinationsCountL() <= 0))    
        {
        HBufC* noDestinationsText = 
            StringLoader::LoadLC( R_QTN_SIP_INFONOTE_NO_DESTINATIONS_DEFINED );
        CAknInformationNote* noDestinationsNote = 
            new (ELeave) CAknInformationNote();
        noDestinationsNote->ExecuteLD( *noDestinationsText );
        CleanupStack::PopAndDestroy( noDestinationsText );
        }
    else
        {
        CAknSettingItemList::EditItemL( index, aCalledFromMenu );    
        if( index == EGSItemDefaultDest && iDestIndex != KUnknownAPUID )
            
            {
            iDefaultAP->SetHidden(ETrue) ;
            HandleChangeInItemArrayOrVisibilityL();
            }
        }
    __GSLOGSTRING("CSIPSettListSIPProfSetItemList::EditItemL End" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetItemList::StoreSettingsL
// stores the settings of the items and maps the index of the access point 
// to the UID
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetItemList::StoreSettingsL()
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetItemList::StoreSettingsL Start" )
    CAknSettingItemList::StoreSettingsL();
    
     // map the index of selected destination/access point to its UID
     if ( iDestIndex >= 0 )
        {
        // if destination is set => AP not set
        iProfileData.iAPIndex = KDefaultIAP;
        iProfileData.iSNAPIndex = iModel->SNAPUID( iDestIndex );
        }
    else
        {
        iProfileData.iSNAPIndex = KDefaultIAP;
        if ( iAPIndex >= 0 )
            {
            iProfileData.iAPIndex = iModel->APUID( iAPIndex );
            }
        else
            {
            iProfileData.iAPIndex = KDefaultIAP;
            }
        }
        
    // map the index of selected service profile to its profile data.
    if( iProfileData.iServiceProfileIndex >= 0 )
        {
        iProfileData.iServiceProfile = iModel->ProfileTypeInfo( iProfileData.iServiceProfileIndex );
        }
    __GSLOGSTRING("CSIPSettListSIPProfSetItemList::StoreSettingsL End" )
    }
    
// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetItemList::SetNextSettingItemL
// Changes current setting item setting to next one.
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetItemList::SetNextSettingItemL( TInt aSettingId )
    {   
    __GSLOGSTRING("CSIPSettListSIPProfSetItemList::SetNextSettingItemL Start" )
    TSIPProfileTypeInfo::TSIPProfileClass prevClass = 
        TSIPProfileTypeInfo::EOther;
    TInt prevIndex = 0;
                  
    switch ( aSettingId )
        {
        case EGSItemServiceProfile: // Service profile setting.
            iServiceProfile->StoreL();
            
            prevClass = iModel->ProfileTypeInfo( 
                iProfileData.iServiceProfileIndex ).iSIPProfileClass;
            prevIndex = iProfileData.iServiceProfileIndex;
            
            if ( iProfileData.iServiceProfileIndex == KServiceProfileIETF )
                {
                iProfileData.iServiceProfileIndex = KServiceProfile3GPP;            
                }
            else 
                {
                iProfileData.iServiceProfileIndex = KServiceProfileIETF;
                }    
            // Get text for setting based on index value.    
            iProfileData.iServiceProfile = 
                iModel->ProfileTypeInfo( iProfileData.iServiceProfileIndex );
            if ( iProfileData.iServiceProfile.iSIPProfileClass != 
                prevClass )
                { // Check if next index is valid
                iServiceProfile->LoadL();
                iServiceProfile->UpdateListBoxTextL();
                }
            else
                { // Not valid, use the same index as before this function call
                iProfileData.iServiceProfileIndex = prevIndex;
                }
            break;
        case EGSItemCompression:    // Compression mode setting.
            iCompression->StoreL();            
            if ( iProfileData.iCompression == EGSComprNo )
                {
                iProfileData.iCompression = EGSComprYes;            
                }
            else 
                {
                iProfileData.iCompression = EGSComprNo;
                }    
            iCompression->LoadL();
            iCompression->UpdateListBoxTextL();
            break;
        case EGSItemSecurity:       // Security mode setting.
            iSecurity->StoreL();
            if ( iProfileData.iSecurity == EGSSecurityOff )
                {
                iProfileData.iSecurity = EGSSecurityOn;
                }
            else
                {
                iProfileData.iSecurity = EGSSecurityOff;
                }
            iSecurity->LoadL();
            iSecurity->UpdateListBoxTextL();        
            break;
        default:                    // Other setting items are not supported.
            User::Leave( KErrNotSupported );
            break;
        }
    __GSLOGSTRING("CSIPSettListSIPProfSetItemList::SetNextSettingItemL End" )
    }          
    
// End of file
