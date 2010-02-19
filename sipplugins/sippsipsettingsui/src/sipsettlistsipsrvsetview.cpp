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
* Description:  The (common) controller for the proxy & registrar views
*
*/



// INCLUDE FILES
#include    <aknViewAppUi.h>
#include    <akntitle.h>
#include    <StringLoader.h>
#include    <hlplch.h>
#include    <featmgr.h>
#include    <gssipsettingspluginrsc.rsg> //GUI Resource
#include    "sipsettlistsipsrvsetview.h"
#include    "tsipsettingsdata.h"
#include    "sipsettlistsipsrvsetcontainer.h"
#include    "sipsettlistsipsrvsetmodel.h"
#include    "sipsettlistsipprofsetview.h"
#include    "viewid.h"
#include    "gssippluginlogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetView::CSIPSettListSIPSrvSetView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPSrvSetView::CSIPSettListSIPSrvSetView(
    TInt aTitleResourceID )
    : iView( NULL ),
      iTitleResourceID( aTitleResourceID )
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetView::CSIPSettListSIPSrvSetView" )
    }

// Destructor
CSIPSettListSIPSrvSetView::~CSIPSettListSIPSrvSetView()
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetView::~CSIPSettListSIPSrvSetView" )
    if ( iView )
        {
        AppUi()->RemoveFromViewStack( *this, iView );
        delete iView;
        }

    delete iModel;
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetView::ChangeSettingL
// Opens up the pop-up list for changing a setting
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPSrvSetView::ChangeSettingL( TInt aCommand )
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetView::ChangeSettingL" )
    iView->EditCurrentListItemL( aCommand );    
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetView::BackToSettingViewL
// Returns to profile setting view
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPSrvSetView::BackToSettingViewL()
    {  
    __GSLOGSTRING("CSIPSettListSIPSrvSetView::BackToSettingViewL" )
    iView->StoreSettingsL();    
    AppUi()->ActivateLocalViewL( iPrevViewId.iViewUid  );
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetView::HandleCommandL
// Handles the user interaction
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPSrvSetView::HandleCommandL( 
    TInt aCommand )
    {
    __GSLOGSTRING1("CSIPSettListSIPSrvSetView::HandleCommandL aCommand: %d", aCommand)
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
            BackToSettingViewL();
            break;             
        
        case EGSCmdUpdateList:
            // Nothing to update, just here to prevent infinite loop
            break;

        // "Exit"
        case EEikCmdExit:
        case EAknCmdExit:
        case EAknSoftkeyExit:
            {
            iView->StoreSettingsL();               
            // Propagate the exit message to appui
            AppUi()->ActivateLocalViewL( iPrevViewId.iViewUid  );
            iExit = ETrue;
            }            
            break;

        case EAknCmdHelp:
            {
            if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                HlpLauncher::LaunchHelpApplicationL(
                    iEikonEnv->WsSession(), AppUi()->AppHelpContextL() );
                }
            break;
            }

        default:
            AppUi()->HandleCommandL( aCommand );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetView::DoActivateL()
// Called upon the activation of the view. Creates the control
// container and model
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPSrvSetView::DoActivateL( 
    const TVwsViewId& aPrevViewId, 
    TUid /*aCustomMessageId*/, 
    const TDesC8& /*aCustomMessage*/ )
    {   
    __GSLOGSTRING("[GSSIPSettingsPlugin] CSIPSettListSIPSrvSetView::DoActivateL()" )
    __GSLOGSTRING1("[GSSIPSettingsPlugin] Previous vied id: %d", aPrevViewId.iViewUid.iUid)
 
    iPrevViewId = aPrevViewId;
    TGSSIPServerData* data = DataStruct();
    TInt resourceID = ResourceID();
    TBool locked = ProfileLocked();
    iModel = CSIPSettListSIPSrvSetModel::NewL( *data, ProfileType(), locked  );
    iView = CSIPSettListSIPSrvSetContainer::NewL(
        ClientRect(), this, *iModel, resourceID );
    
    AppUi()->AddToViewStackL( *this, iView );

    // Change the text on the title pane
    HBufC* titleBuf = StringLoader::LoadLC( iTitleResourceID );
    
    CEikStatusPane* sp = StatusPane();
    CAknTitlePane* title = static_cast<CAknTitlePane*>( 
        sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    title->SetTextL( *titleBuf );

    CleanupStack::PopAndDestroy( titleBuf );
    
    iExit = EFalse;
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetView::DoDeactivate()
// Called upon the deactivation of the view. Deletes the control 
// container and model
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPSrvSetView::DoDeactivate()
    {
    __GSLOGSTRING( "[GSSIPSettingsPlugin] CSIPSettListSIPProfSetView::DoDeactivate()" )
    AppUi()->RemoveFromViewStack( *this, iView );
    delete iView;
    iView = NULL;

    delete iModel;
    iModel = NULL;
    
    if ( iExit )
    	{
    	CSIPSettListSIPProfSetView* mainView = 
    	    static_cast<CSIPSettListSIPProfSetView*>( 
    	        AppUi()->View( iPrevViewId.iViewUid ) );
    	mainView->SaveAllSettings();
    	AppUi()->Exit();
    	}
    }
    
// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetView::DynInitMenuPaneL
// Initializes the menu dynamically before it is shown to the user
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPSrvSetView::DynInitMenuPaneL( 
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {  
    __GSLOGSTRING("CSIPSettListSIPSrvSetView::DynInitMenuPaneL" )
    // Delete Help item if feature is not supported
    if( aResourceId == R_GS_SIP_SERVER_SETTING_VIEW_MENU ) 
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
