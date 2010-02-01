
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



#ifndef CSIPLINEAPPUI_H
#define CSIPLINEAPPUI_H

//  INCLUDES

#include    "SIPExGameObserver.h"
#include    "SIPExGameConstants.h"
#include    "SIPExGameEngine.h"
#include    <eikappui.h>

// FORWARD DECLARATIONS
class CSIPExGameContainer;
class CSIPExEngine;

// CLASS DECLARATION
/**
* Application user interface class.
* User interface part of the CKON application framework 
* for the SIPEx game.
*/
class CSIPExAppUi 
:   public CEikAppUi, 
    public MSIPExGameObserver     
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor. This needs to be public due to the way 
        * the framework constructs the AppUi.
        */
        CSIPExAppUi();

        /**
        * 2nd phase constructor. This needs to be public due to the way 
        * the framework constructs the AppUi 
        */
        void ConstructL();
        
        /**
        * Destructor.
        */
        virtual ~CSIPExAppUi();

    public: // Functions from base classes

        /**
        * From CEikAppUi handles the user interaction.
        * @param aCommand Id of the initiated command        
        */
        void HandleCommandL( TInt aCommand );         

        // From MSIPExGameObserver
        void GameStateChanged( CSIPExEngine::TEngineState aState );
        TBool AcceptInvitationL( const TDesC& aFrom );
        void  IMReceivedL( const TDesC8& aFrom, const TDesC8& aMsg );

    private: // New functions
    
        /**
        * Handles the toolbar items states (dim/undim).
        */
        void HandleToolbar();
        
    private:    // Data
        // Owned: Pointer to the game view container
        CSIPExGameContainer*    iGameContainer;
        // Not owned: pointer to the application engine.
        CSIPExEngine*           iEngine;
        // The address field. The app remembers the last address.
        TBuf< KMaxSIPAddressLength >   iAddress;
        // The state of the game.
        CSIPExEngine::TEngineState            iState;
    };

#endif      // CSIPLINEAPPUI_H
            
// End of File
