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
* Description:  SIP Settings view controller
*
*/


// INCLUDE FILES
#include    <uikon.hrh>
#include    <aknradiobuttonsettingpage.h>
#include    <aknnotewrappers.h>
#include    <StringLoader.h>
#include    <hlplch.h>
#include    <featmgr.h>
#include     <bautils.h>
#include     <gsfwviewuids.h>     //for KGSConPluginUid
#include    <gssipsettingspluginrsc.rsg> //GUI Resource
#include    <gssipsettingsplugin.mbg>
#include    "sipsettingsplugin.h"
#include    "sipsettingsmodel.h"
#include    "sipsettingscontainer.h"
#include     "gssipmodel.h"
#include    "sipsettlistsipprofsetview.h"
#include    "sipsettlistsipprxsetview.h"
#include    "sipsettlistsipregsetview.h"
#include    "gssipsettingsplugin.hrh"
#include    "viewid.h"
#include    "gsprivatepluginproviderids.h"
#include    "gssippluginlogger.h"

#include "gssipsettingspluginrsc.rsg"

#include <aknappui.h>
// LOCAL CONSTANTS AND MACROS
// Middle Softkey control ID.
const TInt KGSMSKControlID = 3;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::CSIPSettingsPlugin
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPSettingsPlugin::CSIPSettingsPlugin() 
    : iResourceLoader( *iCoeEnv ), iMskCommandFlag(ETrue)
    {
    __GSLOGSTRING( "[GSSIPSettingsPlugin] CSIPSettingsPlugin::CSIPSettingsPlugin()" )
    }

// ---------------------------------------------------------------------------
// CSIPSettingsPlugin::~CSIPSettingsPlugin()
// Destructor
//
// ---------------------------------------------------------------------------
//
CSIPSettingsPlugin::~CSIPSettingsPlugin()
    {
    __GSLOGSTRING( "[GSSIPSettingsPlugin] CSIPSettingsPlugin::~CSIPSettingsPlugin()" )
    
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer=NULL;
        }
    iResourceLoader.Close();
    
    delete iModel;
    delete iHandler;
    }

// -----------------------------------------------------------------------------
// When this method is called, view checks based on highlight focus, if the MSK
// label is correct.
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::CheckMiddleSoftkeyLabelL()
    {
    __GSLOGSTRING("CSIPSettingsPlugin::CheckMiddleSoftkeyLabelL " )
    const TInt currentFeatureId = iContainer->CurrentFeatureId();
    // First remove any previous commands.
    RemoveCommandFromMSK( iMskCommandFlag );
    if ( currentFeatureId >= 0 )
        {
         //Set middle softkey as Change. 
        SetMiddleSoftKeyLabelL( R_PROFILES_MSK_EDIT,
                                EGSMSKCmdAppChange );
        // This way we set which command is set for MSK
        iMskCommandFlag = ETrue;
        }
     else
        {
        }
    }

// -----------------------------------------------------------------------------
// Remove unnecessary commands from Middle softkey.
// @flag = ETrue means presently MSK value is "Change" so we remove that
// @flag = EFalse means presently no MSK value is currently on
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::RemoveCommandFromMSK(const TBool flag )
    {
    __GSLOGSTRING("CSIPSettingsPlugin::RemoveCommandFromMSK " )
    CEikButtonGroupContainer* cbaGroup = Cba();
    if ( cbaGroup )
        {
        if ( flag )
            {
                cbaGroup->RemoveCommandFromStack( 
                                KGSMSKControlID, EGSMSKCmdAppChange );
            }
        else
            {
            }
        }
    }

// ---------------------------------------------------------------------------
// Sets middle softkey label.
// ---------------------------------------------------------------------------
//
void CSIPSettingsPlugin::SetMiddleSoftKeyLabelL( 
    const TInt aResourceId, const TInt aCommandId )
    {
    __GSLOGSTRING("CSIPSettingsPlugin::SetMiddleSoftKeyLabelL" )
    CEikButtonGroupContainer* cbaGroup = Cba();
    if ( cbaGroup )
        {
        HBufC* middleSKText = StringLoader::LoadLC( aResourceId );
        TPtr mskPtr = middleSKText->Des();
        cbaGroup->AddCommandToStackL( 
            KGSMSKControlID, 
            aCommandId, 
            mskPtr );
        CleanupStack::PopAndDestroy( middleSKText );
        }
    }

// ---------------------------------------------------------------------------
// CSIPSettingsPlugin::ConstructL(const TRect& aRect)
// Symbian OS two-phased constructor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CSIPSettingsPlugin::ConstructL()
    {
    __GSLOGSTRING( "[GSSIPSettingsPlugin] CSIPSettingsPlugin::ConstructL()" )
    __GSLOGSTRING( "[GSSIPSettingsPlugin] Loading resource from : z:GSSIPSettingsPluginRsc.rsc" )
    
    
    // Find the resource file
    TParse parse;
    parse.Set( KSIPSettingsResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );
    TFileName fileName( parse.FullName() );

    // Get language of resource file
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );

    // Open resource file
    iResourceLoader.OpenL( fileName );
    
    iHandler = CGSSIPModel::NewL();
    iHandler->CreateEngineL( this );
    iModel = CSIPSettingsModel::NewL( iHandler );
    
    // Construct views
    CAknView* view;

    view = CSIPSettListSIPProfSetView::NewLC( iHandler );
    AppUi()->AddViewL( view );    // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( view );    
    __GSLOGSTRING( "[GSSIPSettingsPlugin] CSIPSettingsPlugin::ConstructL: View created" )
    
    view = CSIPSettListSIPPrxSetView::NewLC( iHandler );
    AppUi()->AddViewL( view );    // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( view );    
    __GSLOGSTRING( 
        "[GSSIPSettingsPlugin] CSIPSettingsPlugin::ConstructL: SIP prx srv View created" )
    
    view = CSIPSettListSIPRegSetView::NewLC( iHandler );
    AppUi()->AddViewL( view );    // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( view );    
    __GSLOGSTRING(
        "[GSSIPSettingsPlugin] CSIPSettingsPlugin::ConstructL: SIP reg srv View created" )
        
    iListBoxEventPenDown = EFalse;
    BaseConstructL( R_GS_SIP_PROFILE_LIST_VIEW );       
    }

// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::NewL
// Static constructor
// -----------------------------------------------------------------------------
//
CSIPSettingsPlugin* CSIPSettingsPlugin::NewL( TAny* /*aInitParams*/ )
    {
    __GSLOGSTRING( "[GSSIPSettingsPlugin] CSIPSettingsPlugin::NewL()" )

    CSIPSettingsPlugin* self = 
        new ( ELeave ) CSIPSettingsPlugin( );
    
    CleanupStack::PushL( self );
    self->ConstructL();    
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::Id
//
// Returns the UID of the view
// -----------------------------------------------------------------------------
//
TUid CSIPSettingsPlugin::Id() const
    {    
    return KGSSIPSettingsPluginUID;
    }

// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::HandleClientRectChange
//
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::HandleClientRectChange()
    {
    __GSLOGSTRING("[GSSIPSettingsPlugin] CSIPSettingsPlugin::HandleClientRectChange()" )
    if ( iContainer && iContainer->iListBox )
        {
        iContainer->SetRect( ClientRect() );
        }
    }
    

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfListView::DoActivateL()
// Activates the view, creates View and Model
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::DoActivateL( 
    const TVwsViewId& aPrevViewId, 
    TUid /*aCustomMessageId*/, 
    const TDesC8& /*aCustomMessage*/ )
    {
    __GSLOGSTRING( "[GSSIPSettingsPlugin] CSIPSettingsPlugin::DoActivateL()" )
    __GSLOGSTRING1( "[GSSIPSettingsPlugin] Previous vied id: %d" , aPrevViewId.iViewUid.iUid )
    iPrevViewId = aPrevViewId;

    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer=NULL;
        }

    iContainer = new (ELeave) CSIPSettingsContainer( this );    
    __ASSERT_DEBUG(
        iContainer, 
        User::Panic(_L( "CSIPSettingsPlugin::iContainer == NULL" ), 0 ) );
    iContainer->SetMopParent( this );

    TRAPD( error, iContainer->ConstructL( ClientRect(), iActiveIndex, iModel ) );

    if ( error && iContainer )
        {
        delete iContainer;
        iContainer = NULL;
        User::Leave( error );
        }

    iContainer->SetMiddleSoftkeyObserver( this );
    AppUi()->AddToViewStackL( *this, iContainer );
       // This way we set which command is set for MSK
    iMskCommandFlag = ETrue;
    CheckMiddleSoftkeyLabelL();
    
    __GSLOGSTRING("[GSSIPSettingsPlugin] CSIPSettingsPlugin::DoActivateL done" )
    }
        
// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::DoDeactivate()
// Deactivates the view
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::DoDeactivate()
    {
    __GSLOGSTRING( "[GSSIPSettingsPlugin] CSIPSettingsPlugin::DoDeactivate()" )
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }
   
// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::HandleCommandL
// Handles the user interaction
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::HandleCommandL( TInt aCommand )
    {
    __GSLOGSTRING1("CSIPSettingsPlugin::HandleCommandL aCommand: %d", aCommand)
    TBool exitNow( ETrue ); 
    TVwsViewId id;
    AppUi()->GetActiveViewId( id ); // Current view id.
    
    switch ( aCommand )
        {   
        // "Add new..Use existing profile" menu item
        case EGSCmdAppNewExist:
            CreateFromExisitingProfileL();
            break;
        
        // "Add new..Use default profile" menu item
        case EGSCmdAppNewDefault:
            CreateNewProfileL( KUnknownProfileIndex );
            break;

        // "Edit" menu item / Select pressed
        case EGSMSKCmdAppChange:
        case EGSCmdAppEdit:
            EditProfileL();
            break;

        // "Delete" menu item / 'c' pressed
        case EGSCmdAppDelete:
            DeleteProfileL();
            break;

        // "Default profile" menu item
        case EGSCmdAppDefault:
            ChangeDefaultProfileL();
            break;

        // Updates the list as registration status has changed
        // or a new list item is added
        case EGSCmdUpdateList:
            if ( iContainer )
                {
                iContainer->ListItemModifiedL( iHandler->AddedProfileIndex() );
                CheckMiddleSoftkeyLabelL();
                }
            break;

        // Called when a profile is deleted
        case EGSCmdProfileDeleted:
            if ( iContainer )
                {
                iContainer->ListItemDeletedL();
                CheckMiddleSoftkeyLabelL();          
                }
            break;
        
        // Activate 'Connection' or 'Administrative Settings' view
        case EAknSoftkeyBack:
            iActiveIndex = 0;
#if defined (RD_CONTROL_PANEL)                
            AppUi()->ActivateLocalViewL( KGSAdminPluginUid );
#else           
            AppUi()->ActivateLocalViewL( KGSConPluginUid );
#endif  // RD_CONTROL_PANEL             
            break;
            
        case EAknCmdHelp:
            {
            if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                HlpLauncher::LaunchHelpApplicationL(
                    iEikonEnv->WsSession(), AppUi()->AppHelpContextL() );
                }
            break;
            }
            
        case EAknCmdExit:
            iActiveIndex = 0;
            iHandler->QuitAfterSave();
            if ( ( id.iViewUid == KSettListSIPProfileSettingView || 
                id.iViewUid == KSettListSIPProxyServerView ||
                id.iViewUid == KSettListSIPRegistrarServerView ) &&
                iHandler->SaveProfileL() )
                {
                exitNow = EFalse;
                }
            
            // Exit?
            if ( exitNow )
                {
                AppUi()->HandleCommandL( aCommand );                
                }
            break;
        

        case EGSCmdAppShutterExit:
        default:
            AppUi()->HandleCommandL( aCommand );
            break;
        }
    }
    
// ========================= From CGSPluginInterface ==================

// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::GetCaptionL
// 
// Method for getting caption of this plugin. This should be the
// localized name of the settings view to be shown in parent view.
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::GetCaptionL( TDes& aCaption ) const
    {
    __GSLOGSTRING("CSIPSettingsPlugin::GetCaptionL " )
    HBufC* result = StringLoader::LoadL( R_GS_SIPSETTINGS_PLUGIN_CAPTION );
    aCaption.Copy( *result );
    delete result;
    }

// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::PluginProviderCategory
// Method for reading the ID of the plugin provider category.
// -----------------------------------------------------------------------------
//
TInt CSIPSettingsPlugin::PluginProviderCategory() const
    {
    return KGSPluginProviderInternal;
    }
    
// ---------------------------------------------------------------------------
// CSIPSettingsPlugin::CreateIconL
//
// Return the icon, if has one.
// ---------------------------------------------------------------------------
//
CGulIcon* CSIPSettingsPlugin::CreateIconL( const TUid aIconType )
    {
    __GSLOGSTRING( "[CSIPSettingsPlugin] CreateIconL()|->" )
    //EMbm<Mbm_file_name><Bitmap_name>
    CGulIcon* icon;

    if( aIconType == KGSIconTypeLbxItem )
        {
        icon = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(),
        KAknsIIDQgnPropSetConnSip,
        KGSDefaultSIPIconFileName,
        EMbmGssipsettingspluginQgn_prop_set_conn_sip,
        EMbmGssipsettingspluginQgn_prop_set_conn_sip_mask );
        }
     else
        {
        icon = CGSPluginInterface::CreateIconL( aIconType );
        }

    __GSLOGSTRING( "[CSIPSettingsPlugin] CreateIconL()-|" )
    return icon;
    }

// ========================= MEikMenuObserver =================================

// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::HandleListBoxEventL
// Handles the list box events
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::HandleListBoxEventL( 
    CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType )
    {
    __GSLOGSTRING("CSIPSettingsPlugin::HandleListBoxEventL " )
    __GSLOGSTRING1("CSIPSettingsPlugin::HandleListBoxEventL aEventType: %d", aEventType)
    if( EEventItemSingleClicked == aEventType ||
    		  EEventEnterKeyPressed == aEventType )
	    {
    	    EditProfileL();
	    }
    }

// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::ChangeDefaultProfileL
// Shows the pop-up list for changing the default profile
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::ChangeDefaultProfileL()
    {
    __GSLOGSTRING("CSIPSettingsPlugin::ChangeDefaultProfileL Start " )
    TInt selectedIndex = iModel->DefaultProfileIndex();  
    TInt oldProfileIndex = selectedIndex;  
    
    // Get the array of the profile names, ownership changes
    CDesCArray* array = iModel->ListOfProfileNamesL();    
    CleanupStack::PushL( array );
    
    // Create and display the pop-up list
    CAknRadioButtonSettingPage* defaultPopUp = 
        new ( ELeave ) CAknRadioButtonSettingPage( 
            R_SIP_PROFILE_LIST_VIEW_DEFAULT_SETTING_PAGE,
            selectedIndex,
            array );
    if ( defaultPopUp->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
        {
        if ( selectedIndex != oldProfileIndex )
            {
            // User has changed the default profile, set new setting
            // to persistent storage
            iModel->SetDefaultProfileL( selectedIndex );
            }
        }

    CleanupStack::PopAndDestroy( array );  // array
    __GSLOGSTRING("CSIPSettingsPlugin::ChangeDefaultProfileL End" )
    }

// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::CreateFromExistingProfileL
// Shows pop-up list for user to choose the cloned profile
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::CreateFromExisitingProfileL()
    {
    __GSLOGSTRING("CSIPSettingsPlugin::CreateFromExisitingProfileL Start " )
    // Creates list box for the pop-up list
    CEikFormattedCellListBox* listBox = new ( ELeave ) 
        CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL( listBox );

    // Create pop-up list
    CAknPopupList* popupList = CreatePopupListL( listBox );

    // Show the pop-up list
    TBool profileSelected = popupList->ExecuteLD();
    TInt index = listBox->CurrentItemIndex();

    CleanupStack::PopAndDestroy( listBox );  

    if ( profileSelected )
        {
        CreateNewProfileL( index );
        }
    __GSLOGSTRING("CSIPSettingsPlugin::CreateFromExisitingProfileL End" )
    }

// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::CreatePopupListL
// Creates pop-up list for choosing the cloned profile
// -----------------------------------------------------------------------------
//
CAknPopupList* CSIPSettingsPlugin::CreatePopupListL(
    CEikFormattedCellListBox* aListBox )
    {
    __GSLOGSTRING("CSIPSettingsPlugin::CreatePopupListL" )
    // Create and initialize the popup list
    CAknPopupList* popupList = CAknPopupList::NewL(
        aListBox, R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
        AknPopupLayouts::EPopupSNotePopupWindow );
    CleanupStack::PushL( popupList );

    // Set the title of the pop-up list
    HBufC* titleBuf = StringLoader::LoadLC( 
        R_PROFILE_LIST_VIEW_NEW_PROFILE_LIST_TITLE );
    popupList->SetTitleL( *titleBuf );
    CleanupStack::PopAndDestroy( titleBuf );

    // Initialize list box
    aListBox->ConstructL( popupList,
                          EAknListBoxSelectionList );
    aListBox->CreateScrollBarFrameL( ETrue );
    aListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    aListBox->Model()->SetItemTextArray( iModel->ListOfProfileNamesL() );

    CleanupStack::Pop( popupList ); 

    return popupList;
    }

// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::CreateNewProfileL
// Creates a new profile and changes the view to another
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::CreateNewProfileL( 
    TInt aIndex )
    {
    __GSLOGSTRING("CSIPSettingsPlugin::CreateNewProfileL" )
    iActiveIndex = iContainer->CurrentIndex();
    iModel->CreateNewProfileL( aIndex );
    AppUi()->ActivateLocalViewL( KSettListSIPProfileSettingView );
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfListView::DeleteProfileL
// Confirms from the user and then deletes the profile
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::DeleteProfileL()
    {  
    __GSLOGSTRING("CSIPSettingsPlugin::DeleteProfileL Start" )
    TUint count = iModel->NumOfProfiles();
    if ( count == 0 )
        {
        // No profiles to be deleted, so return
        return;
        }
    
    TBool isUse( EFalse );
    TBool isDefault( EFalse );
    TBool isLocked( EFalse );
    iModel->CheckProfileForDeleteL( iContainer->CurrentIndex(), isUse, 
                                isDefault, isLocked );

    // Don't allow delete profile which is in use.    
    if ( isUse )
        {
        HBufC* txtErr = StringLoader::LoadLC( 
            R_QTN_SIP_ERROR_UNABLE_TO_DELETE );
        CAknErrorNote* note = new ( ELeave ) CAknErrorNote( ETrue );
        note->ExecuteLD( txtErr->Des() );
        CleanupStack::PopAndDestroy( txtErr );
        }
    // Don't allow delete default profile until it's last one.    
    else if ( isDefault && count > 1 )
        {
        HBufC* txtErr = StringLoader::LoadLC(
            R_QTN_SIP_ERROR_UNABLE_TO_DELETE_DEFAULT );
        CAknErrorNote* note = new ( ELeave ) CAknErrorNote( ETrue );
        note->ExecuteLD( txtErr->Des() );
        CleanupStack::PopAndDestroy( txtErr );
        }
    // Don't allow to delete profile which is locked
    else if ( isLocked )
        {
        HBufC* txtErr = StringLoader::LoadLC(
            R_QTN_SIP_ERROR_UNABLE_TO_DEL_LOCKED );
        CAknErrorNote* note = new ( ELeave ) CAknErrorNote( ETrue );
        note->ExecuteLD( txtErr->Des() );
        CleanupStack::PopAndDestroy( txtErr );
        }
    else
        {
        // Load and initialize the dialog text
        HBufC* txtBuf = iModel->ProfileNameLC( iContainer->CurrentIndex() );    
        HBufC* dlgTxt = StringLoader::LoadLC( 
            R_QTN_CONF_PROFILE_DELETE_LABEL, *txtBuf );       

        CAknQueryDialog* dlg = CAknQueryDialog::NewL();
        CleanupStack::PushL( dlg );
        dlg->SetPromptL( *dlgTxt );
        CleanupStack::Pop( dlg );
        
        if ( dlg->ExecuteLD( R_SIP_DELETE_CONFIRMATION_DLG ) )
            {
            iModel->DeleteProfileL( iContainer->CurrentIndex() );    
            if ( iActiveIndex && iActiveIndex == iContainer->CurrentIndex())    
                {
                iActiveIndex--;
                }
            }

        CleanupStack::PopAndDestroy( dlgTxt );      
        CleanupStack::PopAndDestroy( txtBuf );   
        }  
    __GSLOGSTRING("CSIPSettingsPlugin::DeleteProfileL End" )
    }

// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::EditProfileL
// Edits existing profile
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::EditProfileL()
    {
    __GSLOGSTRING("CSIPSettingsPlugin::EditProfileL" )
    if ( iContainer->CurrentIndex() < 0 )
        {
        return;
        }
        
    TBool moveNxtView = iModel->EditProfileL( iContainer->CurrentIndex() );
    
    if ( moveNxtView )
        {
        iActiveIndex = iContainer->CurrentIndex();
        AppUi()->ActivateLocalViewL( KSettListSIPProfileSettingView );  
        }
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfListView::DynInitMenuPaneL
// Initializes the menu dynamically before it is shown to the user
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::DynInitMenuPaneL( 
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {    
    __GSLOGSTRING("CSIPSettingsPlugin::DynInitMenuPaneL" )
    // Delete Help item if feature is not supported
    if( aResourceId == R_GS_SIP_PROFILE_LIST_VIEW_MENU && 
		!FeatureManager::FeatureSupported( KFeatureIdHelp ) ) 
        {

            aMenuPane->DeleteMenuItem( EAknCmdHelp );
        }    
        
    if ( aResourceId == R_GS_SIP_PROFILE_LIST_VIEW_MENU && 
         iModel->NumOfProfiles() == 0 )
        {
        // No profiles exist, time to dim some menu items
        aMenuPane->SetItemDimmed( EGSCmdAppEdit, ETrue );
        aMenuPane->SetItemDimmed( EGSCmdAppDelete, ETrue );
        aMenuPane->SetItemDimmed( EGSCmdAppDefault, ETrue );        
        }
    else if ( aResourceId == R_GS_SIP_PROFILE_LIST_ADD_NEW_MENU && 
              iModel->NumOfProfiles() == 0 )
        {
        // No profiles exist, time to dim some menu items
        aMenuPane->SetItemDimmed( EGSCmdAppNewExist, ETrue );        
        }
    else
        {
        // Do nothing
        }
    }

//  End of File  
