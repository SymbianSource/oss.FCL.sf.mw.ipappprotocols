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
* Description:  The control container (view) of the profile setting view
*
*/


// INCLUDE FILES
#include    <eikappui.h>
#include    <eikenv.h>
#include    <cshelp/conset.hlp.hrh>
#include    <gsfwviewuids.h>
#include    <aknsettingitemlist.h>
#include    "sipsettlistsipprofsetcontainer.h"
#include    "sipsettlistsipprofsetmodel.h"
#include    "sipsettlistsipprofsetview.h"
#include    "sipsettlistsipprofsetitemlist.h"
#include     "sipsettingsplugin.h"
#include    "gssippluginlogger.h"

// LOCAL CONSTANTS AND MACROS

const TInt KNumberOfControls = 1;
const TInt KControlItemList = 0;

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CSIPSettListSIPProfSetContainer::CSIPSettListSIPProfSetContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CSIPSettListSIPProfSetContainer::CSIPSettListSIPProfSetContainer()
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetContainer::CSIPSettListSIPProfSetContainer" )
    }

// ----------------------------------------------------------------------------
// CSIPSettListSIPProfSetContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetContainer::ConstructL(
    TInt aIndex,
    CAknTitlePane* aPane,
    const TRect& aRect,    
    CSIPSettListSIPProfSetModel& aModel )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetContainer::ConstructL" )
    CreateWindowL();

    // Initialize item list
    iItemList = CSIPSettListSIPProfSetItemList::NewL( this, &aModel, aPane );

    // Set limits to the view & activate it
    SetRect( aRect );
    ActivateL();
    CAknSettingItemArray * array = iItemList->SettingItemArray();
    TInt Uiindex = 0;
    for( TInt i = 0; i < array->Count(); i ++ )
        {
        if( aIndex == array->ItemIndexFromVisibleIndex(i) )
            {
            Uiindex = i;
            break;
            }
        }
    
    iItemList->ListBox()->SetCurrentItemIndex( Uiindex );    
    DrawNow();
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPProfSetContainer* CSIPSettListSIPProfSetContainer::NewL( 
    TInt aIndex,
    CAknTitlePane* aPane,
    MObjectProvider* aProvider,
    const TRect& aRect,        
    CSIPSettListSIPProfSetModel& aModel )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetContainer::NewL" )
    CSIPSettListSIPProfSetContainer* self = 
        new ( ELeave ) CSIPSettListSIPProfSetContainer();
    
    CleanupStack::PushL( self );
    self->SetMopParent( aProvider );
    self->ConstructL( aIndex, aPane, aRect, aModel );
    CleanupStack::Pop( self );

    return self;
    }
   
// Destructor
CSIPSettListSIPProfSetContainer::~CSIPSettListSIPProfSetContainer()
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetContainer::~CSIPSettListSIPProfSetContainer" )  
    delete iItemList;
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetContainer::EditCurrentListItemL
// Edits the current list item by simulating the enter key press
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetContainer::EditCurrentListItemL( TInt aCommand )
    { 
    __GSLOGSTRING("CSIPSettListSIPProfSetContainer::EditCurrentListItemL" ) 
    const TInt index = iItemList->ListBox()->CurrentItemIndex();
    if ( aCommand == EGSMSKCmdAppChange &&
        ( index == EGSItemServiceProfile ||
            index == EGSItemCompression ||
            index == EGSItemRegistration ||
            index == EGSItemSecurity ) )
        {
        iItemList->HandleListBoxEventL( iItemList->ListBox(),
            MEikListBoxObserver::EEventEnterKeyPressed );
        }
    else
        {
        iItemList->EditItemL( iItemList->ListBox()->CurrentItemIndex(), ETrue );
        }
        
    TKeyEvent event;
    event.iCode = EStdKeyEnter;
    event.iScanCode = EKeyEnter;
    OfferKeyEventL( event, EEventKey );                    
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetContainer::StoreSettingsL()
// Stores settings to actual variables from item list
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetContainer::StoreSettingsL()
    {
    iItemList->StoreSettingsL();
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetContainer::CountComponentControls
// Returns the amount of controls on the view
// -----------------------------------------------------------------------------
//
TInt CSIPSettListSIPProfSetContainer::CountComponentControls() const
    {
    return KNumberOfControls;
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetContainer::ComponentControl
// Returns list box
// -----------------------------------------------------------------------------
//
CCoeControl* CSIPSettListSIPProfSetContainer::ComponentControl( 
    TInt aIndex ) const
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetContainer::ComponentControl" ) 
    CCoeControl* cntrl = NULL;

    if ( aIndex == KControlItemList )
        {
        cntrl = iItemList->ListBox();
        }
                                                                  
    return cntrl;
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetContainer::SizeChanged
// Sets new size for the item list
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetContainer::SizeChanged()
    {
    iItemList->ListBox()->SetRect( Rect() );
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetContainer::OfferKeyEventL
// Handles the key events, pushes them to item list
// -----------------------------------------------------------------------------
//
TKeyResponse CSIPSettListSIPProfSetContainer::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetContainer::OfferKeyEventL" ) 
   
    return iItemList->OfferKeyEventL( aKeyEvent, aType );
    }
    
// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetContainer::FocusChanged()
// Responds to a change in focus
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetContainer::FocusChanged( TDrawNow aDrawNow )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetContainer::FocusChanged" ) 
    CCoeControl::FocusChanged( aDrawNow );
    if( iItemList )
        {
      iItemList->SetFocus( IsFocused(), aDrawNow );
        }
    }

// ---------------------------------------------------------------------------
// CSIPSettListSIPProfSetContainer::HandleResourceChange
// Resource change handling
// ---------------------------------------------------------------------------
// 
void CSIPSettListSIPProfSetContainer::HandleResourceChange( TInt aType )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetContainer::HandleResourceChange" ) 
    CCoeControl::HandleResourceChange( aType );
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                                           mainPaneRect);
        SetRect( mainPaneRect );
        }
    }

// ---------------------------------------------------------------------------
// CSIPSettListSIPProfSetContainer::GetHelpContext(TCoeHelpContext& aContext) const
// Gets Help 
//  
// ---------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetContainer::GetHelpContext(TCoeHelpContext& aContext) const
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetContainer::GetHelpContext" )
    aContext.iMajor = KUidGS;
    aContext.iContext = KSIP_HLP_PROF_PARAMS;
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetContainer::HandlePointerEventL()
// Responds to a Pointer Event.
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetContainer::HandlePointerEventL" )
     CCoeControl::HandlePointerEventL( aPointerEvent );
    }
//  End of File  
