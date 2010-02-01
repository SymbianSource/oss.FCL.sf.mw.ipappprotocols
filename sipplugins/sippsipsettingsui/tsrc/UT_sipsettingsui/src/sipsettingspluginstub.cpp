/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

// INCLUDE FILES
#include    "sipsettingsplugin.h"

// Constants
//const TUid KGSSIPSettingsPluginUID = { 0x101FD6D0 }; //SIP Settings ECom plug-in uid
const TInt KGSPluginProviderInternal = 87329234;

TInt gEditProfileLInvokedTimes = 0;
TInt gDeleteProfileLInvokedTimes = 0;
TInt gChangeDefaultProfileLInvokedTimes = 0;
TInt gHandleCommandLInvokedTimes = 0;


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

    }

// ---------------------------------------------------------------------------
// CSIPSettingsPlugin::~CSIPSettingsPlugin()
// Destructor
//
// ---------------------------------------------------------------------------
//
CSIPSettingsPlugin::~CSIPSettingsPlugin()
    {
    }

// -----------------------------------------------------------------------------
// When this method is called, view checks based on highlight focus, if the MSK
// label is correct.
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::CheckMiddleSoftkeyLabelL()
    {
    }

// -----------------------------------------------------------------------------
// Remove unnecessary commands from Middle softkey.
// @flag = ETrue means presently MSK value is "Change" so we remove that
// @flag = EFalse means presently no MSK value is currently on
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::RemoveCommandFromMSK(const TBool /*flag*/ )
    {
    }

// ---------------------------------------------------------------------------
// Sets middle softkey label.
// ---------------------------------------------------------------------------
//
void CSIPSettingsPlugin::SetMiddleSoftKeyLabelL( 
    const TInt /*aResourceId*/, const TInt /*aCommandId*/ )
    {
    }

// ---------------------------------------------------------------------------
// CSIPSettingsPlugin::ConstructL(const TRect& aRect)
// Symbian OS two-phased constructor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CSIPSettingsPlugin::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::NewL
// Static constructor
// -----------------------------------------------------------------------------
//
CSIPSettingsPlugin* CSIPSettingsPlugin::NewL( TAny* /*aInitParams*/ )
    {
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
    }
    

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfListView::DoActivateL()
// Activates the view, creates View and Model
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::DoActivateL( 
    const TVwsViewId& /*aPrevViewId*/, 
    TUid /*aCustomMessageId*/, 
    const TDesC8& /*aCustomMessage*/ )
    {
    }
        
// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::DoDeactivate()
// Deactivates the view
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::DoDeactivate()
    {
    }
   
// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::HandleCommandL
// Handles the user interaction
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::HandleCommandL( TInt /*aCommand*/ )
    {
    gHandleCommandLInvokedTimes++;
    }
    
// ========================= From CGSPluginInterface ==================

// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::GetCaptionL
// 
// Method for getting caption of this plugin. This should be the
// localized name of the settings view to be shown in parent view.
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::GetCaptionL( TDes& /*aCaption*/ ) const
    {
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
CGulIcon* CSIPSettingsPlugin::CreateIconL( const TUid /*aIconType*/ )
    {
    //EMbm<Mbm_file_name><Bitmap_name>
    CGulIcon* icon = NULL;
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
    TListBoxEvent /*aEventType*/ )
    {
    }

// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::ChangeDefaultProfileL
// Shows the pop-up list for changing the default profile
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::ChangeDefaultProfileL()
    {
    gChangeDefaultProfileLInvokedTimes++;
    }

// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::CreateFromExistingProfileL
// Shows pop-up list for user to choose the cloned profile
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::CreateFromExisitingProfileL()
    {
    }

// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::CreatePopupListL
// Creates pop-up list for choosing the cloned profile
// -----------------------------------------------------------------------------
//
CAknPopupList* CSIPSettingsPlugin::CreatePopupListL(
    CEikFormattedCellListBox* /*aListBox*/ )
    {
    // Create and initialize the popup list
    CAknPopupList* popupList = NULL;

    return popupList;
    }

// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::CreateNewProfileL
// Creates a new profile and changes the view to another
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::CreateNewProfileL( 
    TInt /*aIndex*/ )
    {
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfListView::DeleteProfileL
// Confirms from the user and then deletes the profile
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::DeleteProfileL()
    {   
    gDeleteProfileLInvokedTimes++;
    }

// -----------------------------------------------------------------------------
// CSIPSettingsPlugin::EditProfileL
// Edits existing profile
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::EditProfileL()
    {
    gEditProfileLInvokedTimes++;
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfListView::DynInitMenuPaneL
// Initializes the menu dynamically before it is shown to the user
// -----------------------------------------------------------------------------
//
void CSIPSettingsPlugin::DynInitMenuPaneL( 
    TInt /*aResourceId*/,
    CEikMenuPane* /*aMenuPane*/ )
    {    
    }

//  End of File  

