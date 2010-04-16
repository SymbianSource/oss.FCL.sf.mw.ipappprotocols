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
* Description:  The control container (view) of the proxy & registrar views
*
*/



// INCLUDE FILES

#include    <aknview.h>
#include    <cshelp/conset.hlp.hrh>
#include    <gsfwviewuids.h>
#include    <gssipsettingspluginrsc.rsg> //GUI Resource
#include    "sipsettlistsipsrvsetcontainer.h"
#include    "sipsettlistsipsrvsetmodel.h"
#include    "sipsettlistsipsrvsetitemlist.h"
#include     "sipsettingsplugin.h"
#include    "gssippluginlogger.h"

// LOCAL CONSTANTS AND MACROS

const TInt KNumberOfControls = 1;
const TInt KControlItemList = 0;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetContainer::CSIPSettListSIPSrvSetContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPSrvSetContainer::CSIPSettListSIPSrvSetContainer()
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetContainer::CSIPSettListSIPSrvSetContainer" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPSrvSetContainer::ConstructL(
    const TRect& aRect,
    CSIPSettListSIPSrvSetModel& aModel,
    TInt aResourceID )
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetContainer::ConstructL Start" )
    CreateWindowL();

    if ( aResourceID == R_SIP_PROXY_SETTING_VIEW_ITEM_LIST )
        {
        iProxyServerView = ETrue;
        }
    else
        {
        iProxyServerView = EFalse;
        }    
    // Initialize item list
    iItemList = CSIPSettListSIPSrvSetItemList::NewL( 
        this, &aModel, aResourceID );    

    // Set limits to the view & activate it
    SetRect( aRect );    
    ActivateL();
    __GSLOGSTRING("CSIPSettListSIPSrvSetContainer::ConstructL End" )  
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPSrvSetContainer* CSIPSettListSIPSrvSetContainer::NewL(
    const TRect& aRect,             
    MObjectProvider* aProvider,
    CSIPSettListSIPSrvSetModel& aModel,
    TInt aResourceID )
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetContainer::NewL" )
    CSIPSettListSIPSrvSetContainer* self = 
        new ( ELeave ) CSIPSettListSIPSrvSetContainer();
    
    CleanupStack::PushL( self );
    self->SetMopParent( aProvider );
    self->ConstructL( aRect, aModel, aResourceID );
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CSIPSettListSIPSrvSetContainer::~CSIPSettListSIPSrvSetContainer()
    {    
    __GSLOGSTRING("CSIPSettListSIPSrvSetContainer::~CSIPSettListSIPSrvSetContainer" )
    delete iItemList;
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetContainer::EditCurrentListItemL
// Opens up the pop-up for changing a setting
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPSrvSetContainer::EditCurrentListItemL( TInt aCommand )
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetContainer::EditCurrentListItemL" )
    const TInt index = iItemList->ListBox()->CurrentItemIndex();
    if ( aCommand == EGSMSKCmdAppChange && index == EGSServerItemRouting)
        {
        iItemList->HandleListBoxEventL( iItemList->ListBox(),
            MEikListBoxObserver::EEventEnterKeyPressed );
        }
    else
        {
        iItemList->EditItemL( iItemList->ListBox()->CurrentItemIndex(), ETrue );    
        }
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetContainer::StoreSettingsL
// Stores settings from item list to variables
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPSrvSetContainer::StoreSettingsL()
    {    
    iItemList->StoreSettingsL();    
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetContainer::CountComponentControls
// Returns the amount of controls on the view
// -----------------------------------------------------------------------------
//
TInt CSIPSettListSIPSrvSetContainer::CountComponentControls() const
    {
    return KNumberOfControls;
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetContainer::ComponentControl
// Returns list box
// -----------------------------------------------------------------------------
//
CCoeControl* CSIPSettListSIPSrvSetContainer::ComponentControl( 
    TInt aIndex ) const
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetContainer::ComponentControl" )
    CCoeControl* cntrl = NULL;

    if ( aIndex == KControlItemList )
        {
        cntrl = iItemList->ListBox();
        }
                                                                  
    return cntrl;
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetContainer::SizeChanged
// Sets new size for the item list
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPSrvSetContainer::SizeChanged()
    {
    iItemList->ListBox()->SetRect( Rect() );
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetContainer::OfferKeyEventL
// Handles the key events, pushes them to item list
// -----------------------------------------------------------------------------
//
TKeyResponse CSIPSettListSIPSrvSetContainer::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetContainer::OfferKeyEventL" )
    // Pass the key event to list box

    return iItemList->OfferKeyEventL( aKeyEvent, aType );    
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetContainer::FocusChanged()
// Responds to a change in focus
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPSrvSetContainer::FocusChanged( TDrawNow aDrawNow )
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetContainer::FocusChanged" ) 
    if( iItemList )
        {
        iItemList->SetFocus( IsFocused(), aDrawNow );
        }
    
     
        CCoeControl::FocusChanged( aDrawNow );
    
    }

// ---------------------------------------------------------------------------
// CSIPSettListSIPSrvSetContainer::HandleResourceChange
// Resource change handling
// ---------------------------------------------------------------------------
// 
void CSIPSettListSIPSrvSetContainer::HandleResourceChange( TInt aType )
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetContainer::HandleResourceChange" ) 
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
// CSIPSettListSIPSrvSetContainer::GetHelpContext(TCoeHelpContext& aContext) const
// Gets Help 
//  
// ---------------------------------------------------------------------------
//
void CSIPSettListSIPSrvSetContainer::GetHelpContext(TCoeHelpContext& aContext) const
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetContainer::GetHelpContext" ) 
    aContext.iMajor = KUidGS;
    if ( iProxyServerView )
        {
        aContext.iContext = KSIP_HLP_PROXY_PARAMS;    
        }
    else
        {
        aContext.iContext = KSIP_HLP_REGISTRAR_PARAM;
        }
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetContainer::HandlePointerEventL()
// Responds to a Pointer Event.
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPSrvSetContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetContainer::HandlePointerEventL" ) 
     CCoeControl::HandlePointerEventL( aPointerEvent );
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetContainer::IfPointerEvent()
// Check if it is Pointer Event.
// -----------------------------------------------------------------------------
//


//Third
//  End of File  
