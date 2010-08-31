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
* Description:  Container for SIPSettings view
*
*/


// INCLUDE FILES
#include    <aknlists.h>
#include    <cshelp/conset.hlp.hrh>
#include    <gsfwviewuids.h>
#include    <gssipsettingspluginrsc.rsg>
#include    <gssipsettingsplugin.mbg>
#include    <AknIconArray.h>
#include    <AknsUtils.h>     //for loading icons
#include    <akntitle.h>
#include    <eikspane.h>
#include     <aknnavi.h>
#include    <StringLoader.h>
#include    "sipsettingscontainer.h"
#include    "sipsettingsmodel.h"
#include    "sipsettingsplugin.h"
#include    "gssippluginlogger.h"

#include    "barsread.h"

// LOCAL CONSTANTS AND MACROS

const TInt KNumberOfControls = 1;
const TInt KControlListBox = 0;
const TInt KUnknownListBoxIndex = -1;
const TInt KGranularity = 1;


// MODULE DATA STRUCTURES

/**
 * Listbox item drawer for CSIPSettingsContainer.
 */
class CSIPSettingsContainer::CItemDrawer : 
        public CFormattedCellListBoxItemDrawer
    {
    public: // interface
        /**
         * Constructor.
         */
        inline CItemDrawer( MTextListBoxModel* aTextListBoxModel, 
                const CFont* aFont, 
                CFormattedCellListBoxData* aFormattedCellData );

        /**
         * Sets iMod to aModel.
         */
        inline void SetModel( CSIPSettingsModel& aModel );

    private: // From CListItemDrawer returns list item properties
        TListItemProperties Properties( TInt aItemIndex ) const;

    private: // data members
        // Ref: array of SIP Settings.
        CSIPSettingsModel* iMod;        
    };

/**
 * Listbox for CSIPSettingsContainer
 */
class CSIPSettingsContainer::CListBox : public CAknDoubleGraphicStyleListBox
    {
    public: // constructor
        inline CListBox();
        inline CItemDrawer* ItemDrawer() const;
        inline void SetIconArray( CArrayPtr<CGulIcon>* aIconArray );
        inline CArrayPtr<CGulIcon>* IconArray();
    public: // from CEikFormattedCellListBox
        void CreateItemDrawerL();

    };

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CSIPSettingsContainer::CItemDrawer::CItemDrawer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
inline
CSIPSettingsContainer::CItemDrawer::CItemDrawer
        ( MTextListBoxModel* aTextListBoxModel, 
        const CFont* aFont, 
        CFormattedCellListBoxData* aFormattedCellData ) :
    CFormattedCellListBoxItemDrawer(
        aTextListBoxModel, 
        aFont, 
        aFormattedCellData )
    {
    __GSLOGSTRING("CSIPSettingsContainer::CItemDrawer::CItemDrawer" )
    }

// -----------------------------------------------------------------------------
// CSIPSettingsContainer::CItemDrawer::SetModel
// Sets model for underlining default sip profile.
// -----------------------------------------------------------------------------
//
inline
void CSIPSettingsContainer::CItemDrawer::SetModel( CSIPSettingsModel& aModel )
    {
    iMod = &aModel;
    }

// -----------------------------------------------------------------------------
// CSIPSettingsContainer::CItemDrawer::Properties
// Gets list box item properties.
// -----------------------------------------------------------------------------
//
TListItemProperties CSIPSettingsContainer::CItemDrawer::Properties(
    TInt aItemIndex ) const
    {
    __GSLOGSTRING("CSIPSettingsContainer::CItemDrawer::Properties" )
    TListItemProperties result = 
        CFormattedCellListBoxItemDrawer::Properties( aItemIndex );
    
    if ( iMod && ( aItemIndex == iMod->DefaultProfileIndex() ) )
        {
        result.SetUnderlined(ETrue);        
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CSIPSettingsContainer::CListBox::CListBox
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
inline
CSIPSettingsContainer::CListBox::CListBox()
    {
    __GSLOGSTRING("CSIPSettingsContainer::CListBox::CListBox" )
    }

// -----------------------------------------------------------------------------
// CSIPSettingsContainer::CListBox::ItemDrawer
// Gets the object used by this list box view to draw its items.
// -----------------------------------------------------------------------------
//
inline
CSIPSettingsContainer::CItemDrawer* 
    CSIPSettingsContainer::CListBox::ItemDrawer(  ) const
    {
    return static_cast<CSIPSettingsContainer::CItemDrawer*>(
        CAknDoubleGraphicStyleListBox::ItemDrawer() );
    }

// -----------------------------------------------------------------------------
// CSIPSettingsContainer::CListBox::SetIconArray
// Sets the icon array used when drawing bitmaps.
// -----------------------------------------------------------------------------
//
inline
void CSIPSettingsContainer::CListBox::SetIconArray(
    CArrayPtr<CGulIcon>* aIconArray )
    {
    ItemDrawer()->ColumnData()->SetIconArray( aIconArray );
    }

// -----------------------------------------------------------------------------
// CSIPSettingsContainer::CListBox::IconArray
// Queries the icon array used when drawing bitmaps for the listbox items.
// -----------------------------------------------------------------------------
//
inline
CArrayPtr<CGulIcon>* CSIPSettingsContainer::CListBox::IconArray()
    {
    return ItemDrawer()->ColumnData()->IconArray();
    }

// -----------------------------------------------------------------------------
// CSIPSettingsContainer::CListBox::CreateItemDrawerL
// Chooses which itemdrawer to use.
// -----------------------------------------------------------------------------
//
void CSIPSettingsContainer::CListBox::CreateItemDrawerL()
    {
    __GSLOGSTRING("CSIPSettingsContainer::CListBox::CreateItemDrawerL" )
    CFormattedCellListBoxData* formattedData = CFormattedCellListBoxData::NewL();
    CleanupStack::PushL( formattedData );
    iItemDrawer = new(ELeave) CSIPSettingsContainer::CItemDrawer(
        Model(),
        iEikonEnv->NormalFont(),
        formattedData );
    CleanupStack::Pop( formattedData );
    }

// -----------------------------------------------------------------------------
// CSIPSettingsContainer::CSIPSettingsContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPSettingsContainer::CSIPSettingsContainer( CSIPSettingsPlugin* aObserver )
    : iObs( aObserver )
    {
    __GSLOGSTRING("CSIPSettingsContainer::CSIPSettingsContainer" )
    }

// -----------------------------------------------------------------------------
// CSIPSettingsContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIPSettingsContainer::ConstructL(
    const TRect& aRect, 
    TInt aActiveIndex,
    CSIPSettingsModel* aModel )
    {
    __GSLOGSTRING("CSIPSettingsContainer::ConstructL Start" )
    // Change the text on the title pane
    HBufC* titleBuf = StringLoader::LoadLC( R_QTN_APP_CAPTION_STRING );
    
    CEikStatusPane* sp = iAvkonAppUi->StatusPane();
    CAknTitlePane* title = static_cast<CAknTitlePane*>( 
        sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    title->SetTextL( *titleBuf );
    CleanupStack::PopAndDestroy( titleBuf );

       // Set navipane to default
    CAknNavigationControlContainer* naviContainer = 
      static_cast<CAknNavigationControlContainer*> 
          (sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi ) ) );
    naviContainer->PushDefaultL();

    CreateWindowL();
    // Initialize list box outlook    
    iListBox = new ( ELeave ) CListBox;
    iListBox->SetContainerWindowL( *this );
    iListBox->ConstructL( this, EAknListBoxSelectionList );
    
    iListBox->View()->ItemDrawer()->SetFlags( CTextListItemDrawer::ESingleClickEnabled );
    
    CAknIconArray* icons = new (ELeave) CAknIconArray( KGranularity );
    CleanupStack::PushL( icons );

    // locked icon
    CGulIcon* iconLocked = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(), 
        KAknsIIDQgnIndiSettProtectedAdd, 
        KGSDefaultSIPIconFileName,
        EMbmGssipsettingspluginQgn_prop_sip_locked, 
        EMbmGssipsettingspluginQgn_prop_sip_locked );
    
    CleanupStack::PushL( iconLocked );
    icons->AppendL( iconLocked );        
    CleanupStack::Pop( iconLocked );
    
    // empty icon
    CGulIcon* iconEmpty = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(), 
        KAknsIIDQgnIndiSettProtectedAdd, 
        KGSDefaultSIPIconFileName,
        EMbmGssipsettingspluginEmpty13x13, 
        EMbmGssipsettingspluginEmpty13x13 );
    
    CleanupStack::PushL( iconEmpty );
    icons->AppendL( iconEmpty );        
    CleanupStack::Pop( iconEmpty );
    
    iListBox->SetIconArray( icons );
    CleanupStack::Pop( icons );

    iListBox->ItemDrawer()->SetModel( *aModel );

    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );    
    
    // Set empty listbox's text.
    HBufC* text = iCoeEnv->AllocReadResourceLC( R_GS_SIP_NO_SIP_SETTINGS );
    iListBox->View()->SetListEmptyTextL( *text );
    CleanupStack::PopAndDestroy( text );
       
    // Set the model
    iListBox->SetListBoxObserver( iObs );
    iListBox->Model()->SetItemTextArray( aModel );
    // List box won't delete model, the Controller object will take care of it
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );

    // Set limits to the view & activate it
    SetRect( aRect );
    ActivateL();

    if ( aActiveIndex != KUnknownListBoxIndex && 
         aModel->NumOfProfiles() > 0 )
        {
        iListBox->SetCurrentItemIndex( aActiveIndex );
        }

    
    DrawNow();
    __GSLOGSTRING("CSIPSettingsContainer::ConstructL End" )
    }

// -----------------------------------------------------------------------------
// CSIPSettingsContainer::~CSIPSettingsContainer()
// Destructor
// -----------------------------------------------------------------------------
//
CSIPSettingsContainer::~CSIPSettingsContainer()
    {
    __GSLOGSTRING("CSIPSettingsContainer::~CSIPSettingsContainer" )
    delete iListBox;    
    }

// -----------------------------------------------------------------------------
// CSIPSettingsContainer::CurrentIndex()
// Returns the current active index from the list box
// -----------------------------------------------------------------------------
//
TInt CSIPSettingsContainer::CurrentIndex()
    {
    return iListBox->CurrentItemIndex();
    }

// -----------------------------------------------------------------------------
// CSIPSettingsContainer::ListItemModifiedL
// Selects the existing index and redraws the view
// -----------------------------------------------------------------------------
//      
void CSIPSettingsContainer::ListItemModifiedL( TInt aIndex )
    {    
    __GSLOGSTRING("CSIPSettingsContainer::ListItemModifiedL" )
    iListBox->SetCurrentItemIndex( aIndex );
    iListBox->HandleItemAdditionL();
    
    if ( iListBox->CurrentItemIndex() == KUnknownListBoxIndex )
        {
        if ( iListBox->Model()->ItemTextArray()->MdcaCount() > 0 )
            {
            iListBox->SetCurrentItemIndex( 
                iListBox->Model()->ItemTextArray()->MdcaCount() - 1 );
            }
        }

    iListBox->DrawNow();
    }

// -----------------------------------------------------------------------------
// CSIPSettingsContainer::ListItemDeletedL
// Selects the existing index and redraws the view
// -----------------------------------------------------------------------------
//      
void CSIPSettingsContainer::ListItemDeletedL()
    {    
    __GSLOGSTRING("CSIPSettingsContainer::ListItemDeletedL" )
    iListBox->HandleItemRemovalL();

    if ( iListBox->CurrentItemIndex() == KUnknownListBoxIndex )
        {
        if ( iListBox->Model()->ItemTextArray()->MdcaCount() > 0 )
            {
            iListBox->SetCurrentItemIndex( 
                iListBox->Model()->ItemTextArray()->MdcaCount() - 1 );
            }
        }
    
    iListBox->DrawNow();    
    }

// ---------------------------------------------------------------------------
// CSIPSettingsContainer::CurrentFeatureId()
//
// ---------------------------------------------------------------------------
//
TInt CSIPSettingsContainer::CurrentFeatureId( )
    {
    return CurrentIndex();
    }

// ---------------------------------------------------------------------------
// Sets observer for MSK label updations.
// ---------------------------------------------------------------------------
//
void CSIPSettingsContainer::SetMiddleSoftkeyObserver( 
    MGsFWMSKObserver* aObserver )
    {
    if ( !iMSKObserver && aObserver )
        {        
        iMSKObserver = aObserver;
        }
    }

// -----------------------------------------------------------------------------
// CSIPSettingsContainer::CountComponentControls()
// Returns the amount of controls on the view
// -----------------------------------------------------------------------------
//
TInt CSIPSettingsContainer::CountComponentControls() const
    {
    return KNumberOfControls;
    }

// -----------------------------------------------------------------------------
// CSIPSettingsContainer::ComponentControl
// Returns list box
// -----------------------------------------------------------------------------
//
CCoeControl* CSIPSettingsContainer::ComponentControl( 
    TInt aIndex ) const
    {
    __GSLOGSTRING("CSIPSettingsContainer::ComponentControl" )
    CCoeControl* cntrl = NULL;

    if ( aIndex == KControlListBox )
        {
        cntrl = iListBox;
        }
                                                                  
    return cntrl;
    }

// -----------------------------------------------------------------------------
// CSIPSettingsContainer::SizeChanged
// Sets new size for the list box
// -----------------------------------------------------------------------------
//
void CSIPSettingsContainer::SizeChanged()
    {
    iListBox->SetRect( Rect() );
    }

// -----------------------------------------------------------------------------
// CSIPSettingsContainer::OfferKeyEventL()
// Handles the key events, pushes them to list box
// -----------------------------------------------------------------------------
//
TKeyResponse CSIPSettingsContainer::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {   
    __GSLOGSTRING("CSIPSettingsContainer::OfferKeyEventL" )
    if ( aType == EEventKey && aKeyEvent.iCode == EKeyBackspace )
        {
         iObs->HandleCommandL( EGSCmdAppDelete );
         return EKeyWasConsumed;
           }
    
    // Pass the key event to list box
    return iListBox->OfferKeyEventL( aKeyEvent, aType );
    }
    
// -----------------------------------------------------------------------------
// CSIPSettingsContainer::FocusChanged()
// Responds to a change in focus
// -----------------------------------------------------------------------------
//
void CSIPSettingsContainer::FocusChanged( TDrawNow aDrawNow )
    {
    __GSLOGSTRING("CSIPSettingsContainer::FocusChanged" )
    CCoeControl::FocusChanged( aDrawNow );
    if( iListBox )
        {
        iListBox->SetFocus( IsFocused(), aDrawNow );
        }
    }

// ---------------------------------------------------------------------------
// CSIPSettingsContainer::HandleResourceChange
// Resource change handling
// ---------------------------------------------------------------------------
// 
void CSIPSettingsContainer::HandleResourceChange( TInt aType )
    {
    __GSLOGSTRING("CSIPSettingsContainer::HandleResourceChange" )
    //Handle layout orientation or skin change
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                                           mainPaneRect);
        SetRect( mainPaneRect );
        }
    CCoeControl::HandleResourceChange( aType );
    }

// ---------------------------------------------------------------------------
// CSIPSettingsContainer::GetHelpContext(TCoeHelpContext& aContext) const
// Gets Help 
//  
// ---------------------------------------------------------------------------
//
void CSIPSettingsContainer::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    __GSLOGSTRING("CSIPSettingsContainer::GetHelpContext" )
    aContext.iMajor = KUidGS;
    aContext.iContext = KSIP_HLP_MAIN;
    }  

// -----------------------------------------------------------------------------
// CSIPSettingsContainer::ProcessCommandL()
// Handle ProcessCommandL
// -----------------------------------------------------------------------------
//
void CSIPSettingsContainer::ProcessCommandL( TInt aCommand )
    {
    __GSLOGSTRING1("CSIPSettingsContainer::ProcessCommandL aCommand: %d", aCommand)
    switch( aCommand )    
        {
        case EGSCmdAppDelete:
            iObs->DeleteProfileL();
        break;
        
        case EGSCmdAppDefault:
            iObs->ChangeDefaultProfileL();                
        default:
        break;
        }
    }
void CSIPSettingsContainer::SetEmphasis( CCoeControl* /*aMenuControl*/, TBool /*aEmphasis*/ )
    {
     
    }

//  End of File  
