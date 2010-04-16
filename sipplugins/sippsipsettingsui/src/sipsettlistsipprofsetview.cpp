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
* Description:  The setting view controller
*
*/


// INCLUDE FILES
#include    <gssipsettingspluginrsc.rsg> //GUI Resource
#include    <uikon.hrh>
#include    <aknViewAppUi.h>
#include     <aknnotewrappers.h> 
#include    <akntitle.h>
#include    <hlplch.h>
#include    <featmgr.h>
#include    <StringLoader.h>
#include    "sipsettlistsipprofsetview.h"
#include    "sipsettlistsipprofsetcontainer.h"
#include    "sipsettlistsipprofsetmodel.h"
#include    "viewid.h"
#include    "sipsettingsplugin.h"
#include    "gssippluginlogger.h"

// CONSTANTS
const TInt KStartIndex = 0;
const TInt KProxyServerIndex = 8;      
const TInt KRegistrarServerIndex = 9;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetView::CSIPSettListSIPProfSetView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPProfSetView::CSIPSettListSIPProfSetView( 
    MGSSIPProfileHandler* aHandler ) :
    iView( NULL ),
    iHandler( aHandler )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetView::CSIPSettListSIPProfSetView" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetView::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetView::ConstructL()
    {   
    __GSLOGSTRING("CSIPSettListSIPProfSetView::ConstructL" )
    BaseConstructL( R_GS_SIP_PROFILE_SETTING_VIEW );
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetView::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPProfSetView* CSIPSettListSIPProfSetView::NewLC(
    MGSSIPProfileHandler* aHandler )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetView::NewLC" )
    CSIPSettListSIPProfSetView* self = 
        new ( ELeave ) CSIPSettListSIPProfSetView( aHandler );
    
    CleanupStack::PushL( self );
    self->ConstructL();    

    return self;
    }

    
// Destructor
CSIPSettListSIPProfSetView::~CSIPSettListSIPProfSetView()
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetView::~CSIPSettListSIPProfSetView" )
    if ( iView )
        {
        AppUi()->RemoveFromViewStack( *this, iView );
        delete iView;
        }

    delete iModel;
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetView::Id
// Returns the UID of the view
// -----------------------------------------------------------------------------
//
TUid CSIPSettListSIPProfSetView::Id() const
    {    
    return KSettListSIPProfileSettingView;
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetView::BackToListViewL()
// Returns back to the list view
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetView::BackToListViewL()
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetView::BackToListViewL" )
    iView->StoreSettingsL();
    
    // Save profile.
    TRAPD( err, iModel->SaveProfileL() );
    
    if( err == KErrNone || err == KErrDiskFull )
        {
        AppUi()->ActivateLocalViewL( iPrevViewId.iViewUid );
        }
    else
        {
        User::LeaveIfError( err ); 
        }
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetView::SaveAllSettings
// Saves new settings
// -----------------------------------------------------------------------------
//
TInt CSIPSettListSIPProfSetView::SaveAllSettings()
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetView::SaveAllSettings" )
    TRAPD( err, iView->StoreSettingsL(); iModel->SaveProfileL() );
    return err;
    }         

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetView::ChangeSettingL
// Edits active item
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetView::ChangeSettingL( TInt aCommand )
    {
    iView->EditCurrentListItemL( aCommand );        
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetView::HandleCommandL
// Handles the user interaction
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetView::HandleCommandL( 
    TInt aCommand )
    {
    __GSLOGSTRING1("CSIPSettListSIPProfSetView::HandleCommandL aCommand: %d", aCommand)
    switch ( aCommand )
        {   
        // "Change" menu item ( same as "select" key press )
        case EGSMSKCmdAppChange:
        case EGSCmdAppChange:
            // Simulate key press
            ChangeSettingL( aCommand );
            break;

        // Saves settings (if modified) and returns to main view
        case EAknSoftkeyBack:
            BackToListViewL();
            break;

        case EGSCmdUpdateList:
            // Nothing to update, just here to prevent infinite loop
            break;
        
        // "Exit"
        case EEikCmdExit:
        case EAknCmdExit:
        case EAknSoftkeyExit:
            {
            TInt err = SaveAllSettings();
            if ( err == KErrNone || err == KErrDiskFull )
                {
                // Propagate the exit message to appui
                AppUi()->HandleCommandL( aCommand );
                }
            else
                {
                User::Leave( err );
                }
            }
            break;

        case EAknCmdHelp:
            {
            if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                HlpLauncher::LaunchHelpApplicationL(
                    iEikonEnv->WsSession(), AppUi()->AppHelpContextL() );
                }
            }
            break;

        default:
            AppUi()->HandleCommandL( aCommand );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetView::DoActivateL()
// Activates the view, creates View and Model
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetView::DoActivateL( 
    const TVwsViewId& aPrevViewId,
    TUid /*aCustomMessageId*/, 
    const TDesC8& /*aCustomMessage*/ )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetView::DoActivateL " )
    __GSLOGSTRING("[GSSIPSettingsPlugin] CSIPSettListSIPProfSetView::DoActivateL()" )
    __GSLOGSTRING1("[GSSIPSettingsPlugin] Previous vied id: %d", aPrevViewId.iViewUid.iUid)

    if ( aPrevViewId.iViewUid == KGSSIPSettingsPluginUID )
        {
        iPrevViewId = aPrevViewId;
        }

    CEikStatusPane* sp = StatusPane();
    CAknTitlePane* title = static_cast<CAknTitlePane*>( 
        sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    int index = KStartIndex;

    if ( aPrevViewId.iViewUid == KSettListSIPProxyServerView )
        {
        index = KProxyServerIndex;
        }
    else if ( aPrevViewId.iViewUid == KSettListSIPRegistrarServerView )
        {
        index = KRegistrarServerIndex;
        }
    else
        {
        // Coming from Profile List view
        }

    iModel = CSIPSettListSIPProfSetModel::NewL( iHandler );
    iView = CSIPSettListSIPProfSetContainer::NewL(
        index, title, this, ClientRect(), *iModel );
    AppUi()->AddToViewStackL( *this, iView );

    // Change the text on the title pane
    HBufC* titleBuf = iModel->ProviderNameLC();    
    title->SetTextL( *titleBuf );

    CleanupStack::PopAndDestroy( titleBuf );
    }
        
// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetView::DoDeactivate()
// Deactivates the view
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetView::DoDeactivate()
    {
    __GSLOGSTRING( "[GSSIPSettingsPlugin] CSIPSettListSIPProfSetView::DoDeactivate()" )
    AppUi()->RemoveFromViewStack( *this, iView );
    delete iView;
    iView = NULL;

    delete iModel;
    iModel = NULL;
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetView::DynInitMenuPaneL
// Initializes the menu dynamically before it is shown to the user
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetView::DynInitMenuPaneL( 
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {    
    __GSLOGSTRING("CSIPSettListSIPProfSetView::DynInitMenuPaneL" )      
    // Delete Help item if feature is not supported
    if( aResourceId == R_GS_SIP_PROFILE_SETTING_VIEW_MENU ) 
        {
        // iView might be NULL
        if( iView )
            {
            aMenuPane->SetItemDimmed( EGSCmdAppChange, ETrue );
            }
        if( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
            {
            aMenuPane->DeleteMenuItem( EAknCmdHelp );
            }
        }  
    }
        
//  End of File  
