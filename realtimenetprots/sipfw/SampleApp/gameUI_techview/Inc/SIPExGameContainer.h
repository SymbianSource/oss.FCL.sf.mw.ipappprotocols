
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



#ifndef __CSIPLINEGAMECONTAINER_H__
#define __CSIPLINEGAMECONTAINER_H__

// INCLUDES
#include    "SIPExStateViewNotifier.h"
#include    <eikbctrl.h>
#include    <coeview.h>

// FORWARD DECLARATIONS
class CEikTextListBox;
class CSIPExGameView;
class CEikLabel;
class CSIPExEngine;
class CEikGlobalTextEditor;

// CLASS DECLARATIONS
/**
* A container class for some associated view
*/
class CSIPExGameContainer 
:   public CEikBorderedControl,
    public MCoeView,
    public MSIPExStateViewNotifier
    {
    public: 

        /**
        * Static constructor for CSIPExGameContainer
        * @param aRect the rectangle this view will be drawn to
        * @param aGameData The reference to the application engine
        * @return An instance of CSIPExGameContainer. 
        *         The ownership is transferred.
        */
        static CSIPExGameContainer* NewL( const TRect& aRect, CSIPExEngine& aGameData );

        /**
        * Static constructor for CSIPExGameContainer. On return the instance is left to
        * the CleanupStack.
        * @param aRect the rectangle this view will be drawn to
        * @param aGameData The reference to the application engine
        * @return An instance of CSIPExGameContainer. 
        *         The ownership is transferred.
        */
        static CSIPExGameContainer* NewLC( const TRect& aRect, CSIPExEngine& aGameData );

        /**
        * Destructor
        */
        ~CSIPExGameContainer();

    private:

        /**
        * Static constructor for CSIPExGameContainer
        * @param aRect Frame rectangle for container.
        * @param aGameData The reference to the application engine
        */
        void ConstructL( const TRect& aRect, CSIPExEngine& aGameData );

        /**
        * C++ default constructor
        */
        CSIPExGameContainer();

    public: // from CCoeControl

        /**
        * From CCoeControl.
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
        * From CCoeControl
        */
        void Draw(const TRect& aRect) const;

        /**
        * From CCoeControl
        */
        void SizeChanged();

        /**
        * From CCoeControl
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    public: // from MCoeView
        /**
        * From MCoeView
        */
        virtual TVwsViewId ViewId() const;

        /**
        * From MCoeView
        */
        virtual void ViewActivatedL(const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,const TDesC8& aCustomMessage);
        
        /**
        * From MCoeView
        */
        virtual void ViewDeactivated();

    public: // From MStateViewNotifier

        /**
        * From MStateViewNotifier
        * Shows the text in the info editor (CEikGlobalTextEditor)
        * @param aText A 16-bit text to be set to the info view.
        */
        void ShowInfoL( const TDesC& aText );

        /**
        * From MStateViewNotifier
        * Shows the text in the info editor (CEikGlobalTextEditor)
        * @param aText A 8-bit text to be set to the info view.
        */
        void ShowInfoL( const TDesC8& aText );

        /**
        * From MStateViewNotifier
        * Shows the text in the status view editor (CEikLabel)
        * @param aText The text to be set to the status view.
        */
        void ShowStatusInfoL( const TDesC& aText );

    private: // New functions
        /**
        * Counts the rects for components.
        */
        void CountRects();

        /**
        * Creates the text editor and status label.
        */
        void CreateStateViewEditorL();

        /**
        * Creates the game view.
        */
        void CreateGameViewL();

    private: // Data

        // Component areas
        TRect iGameAreaRect;
        TRect iStatusLblRect;
        TRect iEditorRect;

        // Info text area. Area where all info texts can be drawn.
        CEikGlobalTextEditor*       iEditor;

        // Label where game state text can be set.
        CEikLabel*                  iStatusLbl;

        // The game board view.
        CSIPExGameView*             iGameView;

        // Not owned: The pointer to the game engine.
        CSIPExEngine*          iGameEngine;
    };

#endif // __CSIPLINEGAMECONTAINER_H__

// End of file
