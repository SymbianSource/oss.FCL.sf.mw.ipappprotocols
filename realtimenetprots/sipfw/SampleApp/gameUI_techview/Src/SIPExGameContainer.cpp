
// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



// INCLUDES
#include    "SIPExGameContainer.h"
#include    "SIPExGameView.h"
#include    "SIPExApp.h"
#include    "SIPExGameConstants.h"

#include    <sipex.rsg>
#include    <eiklabel.h>
#include    <eikgted.h>     // CGlobalTextEditor
#include    <eikenv.h>
#include    <txtglobl.h>    // CGlobalText

// CONSTANTS
const TInt KUndefinedControlId( -3000 );
const TInt KComponentCount( 3 );
const TUid KViewUid = { 0x00000001 };

// -----------------------------------------------------------------------------
// CSIPExGameContainer::NewL
// Static constructor
// -----------------------------------------------------------------------------
//
CSIPExGameContainer* CSIPExGameContainer::NewL(
    const TRect& aRect, 
    CSIPExEngine& aGameData )
    {
    CSIPExGameContainer* self = CSIPExGameContainer::NewLC( aRect, aGameData );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::NewLC
// Static constructor. On return the instance is left to the CleanupStack.
// -----------------------------------------------------------------------------
//
CSIPExGameContainer* CSIPExGameContainer::NewLC(
    const TRect& aRect, 
    CSIPExEngine& aGameData )
    {
    CSIPExGameContainer* self = new ( ELeave ) CSIPExGameContainer;
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aGameData );
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::~CSIPExGameContainer
// Destructor
// -----------------------------------------------------------------------------
//
CSIPExGameContainer::~CSIPExGameContainer()
    {
    delete iStatusLbl;
    delete iGameView;
    delete iEditor;
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::ConstructL
// Symbian 2nd phase constructor that might leave.
// -----------------------------------------------------------------------------
//
void CSIPExGameContainer::ConstructL(
    const TRect& aRect, 
    CSIPExEngine& aGameData )
    {
    iGameEngine = &aGameData;
    // Create a window for this application view
    CreateWindowL();

    // Set the windows size
    SetRect( aRect );
    // Indicate that the control is blank
    SetBlank();
    // Set the control's border
    SetBorder( TGulBorder::EFlatContainer );

    // Initialize controls
    CreateGameViewL();
    CreateStateViewEditorL();

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::CSIPExGameContainer
// C++ default constructor
// -----------------------------------------------------------------------------
//
CSIPExGameContainer::CSIPExGameContainer()
    {
    // no implementation required
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::CountRects
// Calculates rectangles for all different areas.
// -----------------------------------------------------------------------------
//
void CSIPExGameContainer::CountRects()
    {
    iGameAreaRect = TRect( TPoint( 20, 30 ), TSize( 220, 205 ) );
    iStatusLblRect = TRect( TPoint( 20, 5 ), TSize( 220, 25 ) );
    iEditorRect = TRect( TPoint( 265, 5 ), TSize( 230, 230 ) );
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::CreateStateViewEditorL
// Creates the state window and status label.
// -----------------------------------------------------------------------------
//
void CSIPExGameContainer::CreateStateViewEditorL()
    {
    iEditor = new (ELeave) CEikGlobalTextEditor();
    TInt flags( EEikEdwinNoAutoSelection |
                EEikEdwinJustAutoCurEnd |
                EEikEdwinLineCursor |
                EEikEdwinNoHorizScrolling );
    // The max number of characters in editor 1k
    iEditor->ConstructL( this, 7, 1024, flags, EGulFontControlAll, EGulAllFonts );
    iEditor->SetContainerWindowL( *this );
    iEditor->SetRect( iEditorRect );
    iEditor->ActivateL();
    iEditor->CreateScrollBarFrameL();
    iEditor->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

    iStatusLbl = new (ELeave) CEikLabel();
    iStatusLbl->SetContainerWindowL( *this );
    iStatusLbl->SetRect( iStatusLblRect );
    iStatusLbl->ActivateL();
    iStatusLbl->SetTextL( KNullDesC );
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::CreateGameViewL
// Creates the game board view.
// -----------------------------------------------------------------------------
//
void CSIPExGameContainer::CreateGameViewL()
    {
    iGameView = CSIPExGameView::NewL( iGameAreaRect, this, *iGameEngine );
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::CountComponentControls
// From CCoeControl.
// -----------------------------------------------------------------------------
//
TInt CSIPExGameContainer::CountComponentControls() const
    {
    return KComponentCount;
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::ComponentControl
// From CCoeControl.
// -----------------------------------------------------------------------------
//
CCoeControl* CSIPExGameContainer::ComponentControl( TInt aIndex ) const
    {
    switch( aIndex )
        {
        case 0:
            return iEditor;
        case 1:
            return iGameView;
        case 2:
            return iStatusLbl;

        default:
            {
            _LIT( KPanicCntx, "SIP Example" );
            __ASSERT_ALWAYS( 
                EFalse, User::Panic( KPanicCntx(), KUndefinedControlId ) );
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::Draw
// From CCoeControl.
// -----------------------------------------------------------------------------
//
void CSIPExGameContainer::Draw( const TRect& aRect ) const
    {
    CEikBorderedControl::Draw( aRect );
    CWindowGc& gc = SystemGc();

    // Ensure that the border is not overwritten
    // by future drawing operations
    gc.SetClippingRect( Border().InnerRect( Rect() ) );
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::SizeChanged
// From CCoeControl.
// -----------------------------------------------------------------------------
//
void CSIPExGameContainer::SizeChanged()
    {
    CountRects();

    if( iGameView )
        {
        iGameView->SetRect( iGameAreaRect );
        }
    if( iEditor )
        {
        iEditor->SetRect( iEditorRect );
        }
    if( iStatusLbl )
        {
        iStatusLbl->SetRect( iStatusLblRect );
        }
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::ShowInfoL
// From MSIPExStateViewNotifier.
// Appends the text to the globaltexteditor. After each line the line break is
// added.
// -----------------------------------------------------------------------------
//
void CSIPExGameContainer::ShowInfoL( const TDesC& aText )
    {
    CGlobalText* globalText = iEditor->GlobalText();
    globalText->InsertL( globalText->DocumentLength(), aText );
    globalText->InsertL( 
                    globalText->DocumentLength(), CEditableText::ELineBreak );

    TCharFormat charFormat;
    TCharFormatMask charMask;
    iEditor->ApplyCharFormatL( charFormat, charMask );

    iEditor->MoveCursorL( TCursorPosition::EFPageDown, EFalse );
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::ShowInfoL
// From MSIPExStateViewNotifier.
// Converts the 8-bit descriptor to 16-bit desc. and calls the overwritten
// ShowInfoL
// -----------------------------------------------------------------------------
//
void CSIPExGameContainer::ShowInfoL( const TDesC8& aText )
    {
    HBufC* buf = HBufC::NewLC( aText.Length() );
    buf->Des().Copy( aText );
    ShowInfoL( *buf );
    CleanupStack::PopAndDestroy( buf );
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::ShowStatusInfoL
// From MSIPExStateViewNotifier. Updates the game status label text.
// -----------------------------------------------------------------------------
//
void CSIPExGameContainer::ShowStatusInfoL( const TDesC& aText )
    {
    iStatusLbl->SetTextL( aText );
    DrawNow();
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::OfferKeyEventL
// From CCoeControl. Redirect the keyevent to the game view.
// -----------------------------------------------------------------------------
//
TKeyResponse CSIPExGameContainer::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent, 
    TEventCode aType )
    {
    return iGameView->OfferKeyEventL( aKeyEvent, aType );
    }

// -----------------------------------------------------------------------------
// CSIPProfileListContainer::ViewId
// From MCoeView.
// -----------------------------------------------------------------------------
//
TVwsViewId CSIPExGameContainer::ViewId() const
    {
    TVwsViewId id;
    id.iAppUid = KUidSIPExApp;
    id.iViewUid = KViewUid;
    return id;
    }

// -----------------------------------------------------------------------------
// CSIPProfileListContainer::ViewActivatedL
// From MCoeView.
// -----------------------------------------------------------------------------
//
void CSIPExGameContainer::ViewActivatedL(
    const TVwsViewId& /*aPrevViewId*/,
    TUid /*aCustomMessageId*/,const TDesC8& /*aCustomMessage*/)
    {
    // Not used. Implemented to get around the focus disapearing problem.
    }

// -----------------------------------------------------------------------------
// CSIPProfileListContainer::ViewDeactivated
// From MCoeView.
// -----------------------------------------------------------------------------
//
void CSIPExGameContainer::ViewDeactivated()
    {
    // Not used. Implemented to get around the focus disapearing problem.
    }

// End of file

