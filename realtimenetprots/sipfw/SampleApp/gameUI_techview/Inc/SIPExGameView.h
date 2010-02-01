
/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CSIPLINEGAMEVIEW_H__
#define __CSIPLINEGAMEVIEW_H__

#include <coecntrl.h>

// FORWARD DECLARATIONS
class CSIPExEngine;

// CLASS DECLARATIONS
/**
* The class implements the the game board drawing.
*/
class CSIPExGameView : public CCoeControl
    {
    public: 

        /**
        * Static constructor
        * @param aRect the rectangle this view will be drawn to
        * @param aParent The pointer to the parent container component.
        * @param aEngine The reference to the application engine.
        */
        static CSIPExGameView* NewL(const TRect& aRect, const CCoeControl* aParent, CSIPExEngine& aEngine);

        /**
        * Static constructor. On return the instance is left the CleanupStack.
        * @param aRect the rectangle this view will be drawn to
        * @param aParent The pointer to the parent container component.
        * @param aEngine The reference to the application engine.
        */
        static CSIPExGameView* NewLC(const TRect& aRect, const CCoeControl* aParent, CSIPExEngine& aEngine);

        /**
        * Destructor
        */
        ~CSIPExGameView();

    private:

        /**
        * Symbian 2nd phase constructor
        * @param aRect The rectangle this view will be drawn to
        * @param aParent The pointer to the parent container component.
        */
        void ConstructL(const TRect& aRect, const CCoeControl* aParent);

        /**
        * C++ default constructor
        * @param aEngine The reference to the application engine.
        */
        CSIPExGameView( CSIPExEngine& aEngine );

        /**
        * Drawing game. Decides if the cursor or board will be drawn.
        * @param aGc    The system's graphics context.
        * @param aRect  The rectangle where to draw.
        */
        void DrawGame(CWindowGc& aGc, const TRect& aRect) const;

        /**
        * Drawing the game board
        * @param aGc    The system's graphics context.
        * @param aRect  The rectangle where to draw.
        */
        void DrawBoard( CWindowGc& aGc, const TRect& aRect ) const;

        /**
        * Drawing the game cursor.
        * @param aGc    The system's graphics context.
        * @param aRect  The rectangle where to draw.
        */
        void DrawCursor(CWindowGc& aGc, const TRect& aRect) const;

    public: // from CoeControl

        /**
        * From CCoeControl
        */
        void Draw(const TRect& aRect) const;

        /**
        * From CCoeControl
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * From CCoeControl
        */
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);

    private: // Data members

        // Game data
        CSIPExEngine* iEngine;

       	TInt iLabelSize;
        TInt iXOffset;
        TInt iYOffset;
        TInt iBoxW;
        TInt iMidW;
        TRect iRect;
    };

#endif // __CSIPLINEGAMEVIEW_H__

// End of file
