
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
#include    "SIPExGameView.h"
#include    "SIPExGameEngine.h"
#include    "SIPExGameConstants.h"

#include    <e32svr.h>

// CONSTANTS
const TInt KLeft( 25 );
const TInt KRight( 213 );
const TInt KTop( 75 );


// -----------------------------------------------------------------------------
// CSIPExGameView::NewL
// Static constructor
// -----------------------------------------------------------------------------
//
CSIPExGameView* CSIPExGameView::NewL(
    const TRect& aRect, 
    const CCoeControl* aParent, 
    CSIPExEngine& aEngine )
    {
    CSIPExGameView* self = CSIPExGameView::NewLC( aRect, aParent, aEngine );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPExGameView::NewLC
// Static constructor. On return the instance is left to the CleanupStack.
// -----------------------------------------------------------------------------
//
CSIPExGameView* CSIPExGameView::NewLC(
    const TRect& aRect, 
    const CCoeControl* aParent, 
    CSIPExEngine& aEngine )
    {
    CSIPExGameView* self = new ( ELeave ) CSIPExGameView( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aParent);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPExGameView::~CSIPExGameView
// Destructor
// -----------------------------------------------------------------------------
//
CSIPExGameView::~CSIPExGameView()
    {
    // no implementation required
    }

// -----------------------------------------------------------------------------
// CSIPExGameView::ConstructL
// Symbian 2nd phase constructor that might leave.
// -----------------------------------------------------------------------------
//
void CSIPExGameView::ConstructL( const TRect& aRect, const CCoeControl* aParent )
    {
    iLabelSize = 20;
	iXOffset = 5;
	iYOffset = 5;
	iBoxW = 18;
	iMidW = 4;

    iRect = aRect;
    SetContainerWindowL( *aParent );
    SetRect( iRect );

    ActivateL();
    }

// -----------------------------------------------------------------------------
// CSIPExGameView::CSIPExGameView
// C++ default constructor
// -----------------------------------------------------------------------------
//
CSIPExGameView::CSIPExGameView( CSIPExEngine& aEngine )
: iEngine( &aEngine )
    {
    // no implementation required
    }

// -----------------------------------------------------------------------------
// CSIPExGameView::Draw
// From CCoeControl.
// -----------------------------------------------------------------------------
//
void CSIPExGameView::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();
    DrawGame( gc, iRect );
    }

// -----------------------------------------------------------------------------
// CSIPExGameView::DrawGame
// Draws the game view. Decides whether we should draw the board or cursor 
// or not
// -----------------------------------------------------------------------------
//
void CSIPExGameView::DrawGame( CWindowGc& aGc, const TRect& aRect ) const 
    {
    if( iEngine->DrawBoard() )
        {
		DrawBoard(aGc, aRect);

        if( iEngine->DrawCursor() )
            {
			DrawCursor(aGc, aRect);
            }
	    }    
    }

// -----------------------------------------------------------------------------
// CSIPExGameView::DrawBoard
// Draws the game board.
// -----------------------------------------------------------------------------
//
void CSIPExGameView::DrawBoard( CWindowGc& aGc, const TRect& aRect ) const 
    {
    CWindowGc& gc =aGc;

    gc.SetPenSize( TSize(1,1) );
    gc.SetPenStyle( CGraphicsContext::ESolidPen );
    gc.SetPenColor( TRgb(0,0,0) );
    gc.SetBrushColor( TRgb(0,0,255) );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );


	TPoint p1 = aRect.iTl;
	p1.iX = p1.iX  + iXOffset;
	p1.iY = p1.iY + iLabelSize + iYOffset;

	TPoint p2 = aRect.iBr;
	p2.iX = p1.iX + ( ( KBoxCountX * iBoxW ) + ( ( KBoxCountX + 1 ) * iMidW ) );
	p2.iY = p1.iY + ( ( KBoxCountY * iBoxW ) + ( ( KBoxCountY + 1 ) * iMidW ) );
	TRect box2 = TRect(p1, p2);
	gc.DrawRect( box2 );


	gc.SetPenColor( TRgb(0,0,0) );
    gc.SetBrushColor( TRgb(255,255,255) );
    gc.SetPenStyle( CGraphicsContext::ESolidPen );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );

	for (int i=0 ; i < KBoxCountX; i++) 
        {
	    for (int j=0 ; j < KBoxCountY; j++) 
            {
	        p1 = aRect.iTl;
	        p2 = aRect.iBr;

	        p1.iX = p1.iX + iXOffset + iMidW + (i*iMidW) + (i*iBoxW);
	        p1.iY = p1.iY + iLabelSize  + iYOffset + iMidW + (j*iMidW) + (j*iBoxW);

	        p2.iX = p1.iX + iBoxW;
	        p2.iY = p1.iY + iBoxW;

	        if ( iEngine->BoardValue( i, j ) == -1 )
                {
		        gc.SetBrushColor( TRgb(255,255,255) );
                }
	        else if ( iEngine->BoardValue( i, j ) == 1 )
                {
		        gc.SetBrushColor( TRgb(255,0,0) );
                }
	        else if ( iEngine->BoardValue( i, j ) == 2 )
                {
		        gc.SetBrushColor( TRgb(0,255,0) );
                }
            else { /* Nothing to do */ }

	        TRect rect( p1, p2 ); 
	        gc.DrawEllipse( rect );
	        }
        }
    }

// -----------------------------------------------------------------------------
// CSIPExGameView::DrawCursor
// Draws the cursor.
// -----------------------------------------------------------------------------
//
void CSIPExGameView::DrawCursor( CWindowGc& aGc, const TRect& aRect ) const
    {
    CWindowGc& gc =aGc;

	gc.SetPenColor(TRgb(0,0,0));
	gc.SetBrushColor(TRgb(255,0,0));
    gc.SetPenStyle(CGraphicsContext::ESolidPen);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);


	TInt i = iEngine->Cursor();

	TPoint p1 = aRect.iTl;
	TPoint p2 = aRect.iBr;

	p1.iX = p1.iX + iXOffset + iMidW + (i*iMidW) + (i*iBoxW);
	p1.iY = p1.iY + iLabelSize  + iYOffset - iBoxW;

	p2.iX = p1.iX + iBoxW;
	p2.iY = p1.iY + iBoxW;

	TRect rect( p1, p2 ); 
	gc.DrawEllipse( rect );
    }

// -----------------------------------------------------------------------------
// CSIPExGameView::OfferKeyEventL
// From CCoeControl.
// -----------------------------------------------------------------------------
//
TKeyResponse CSIPExGameView::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent, 
    TEventCode aType )
    {
    if( aType == EEventKeyUp )
        {
        switch( aKeyEvent.iScanCode )
            {
            case EStdKeyLeftArrow:
                iEngine->CursorLeft();
                DrawDeferred();
                return EKeyWasConsumed;
            case EStdKeyRightArrow:
                iEngine->CursorRight();
                DrawDeferred();
                return EKeyWasConsumed;
            case EStdKeyDownArrow:
            case EStdKeyEnter:
			case EStdKeyDevice7: // FLOWTHROUGH
                iEngine->CursorPressed();
                DrawDeferred();
                return EKeyWasConsumed;
            default:
                return EKeyWasNotConsumed;
            }
        }

    return EKeyWasNotConsumed;
    }

// -----------------------------------------------------------------------------
// CSIPExGameView::HandlePointerEventL
// From CCoeControl. Calculates the column of the game board where the pointer
// event occured and moves the cursor according that information. 
// -----------------------------------------------------------------------------
//
void CSIPExGameView::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    // Double-click
	if( aPointerEvent.iModifiers & EModifierDoubleClick )
		{
		iEngine->CursorPressed();
        DrawDeferred();
		return;
		}

    // Cursor position
    switch( aPointerEvent.iType )
        {
        case TPointerEvent::EButton1Up:
            {
            TInt block( iMidW + iBoxW );
            TPoint point = aPointerEvent.iPosition;
            TInt x( point.iX );
            if( x >= KLeft && x <= KRight && point.iY > KTop )
                {
                for( TInt i=0; i < KBoxCountX; i++ )
                    {
                    if( x < ( KLeft + block * ( i + 1 ) ) )
                        {
                        iEngine->MoveCursorL( i );
                        DrawDeferred();
                        return;
                        }
                    }
                }
            }
        break;

        default: 
            break;
        }
    }

// End of file
