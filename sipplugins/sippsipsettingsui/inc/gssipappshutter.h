/*
* Copyright (c) 2003 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Exits the application when activated
*
*/



#ifndef GS_SIP_APP_SHUTTER_H
#define GS_SIP_APP_SHUTTER_H

//  INCLUDES

#include <e32base.h>

// FORWARD DECLARATIONS

class CGSPluginInterface;


// CLASS DECLARATION

/**
*  CGSSIPAppShutter class
*  @since 3.0
*  Exits the General Settings application once activated
*/
class CGSSIPAppShutter : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aAppUi pointer to appui        
        */
        static CGSSIPAppShutter* NewL( CGSPluginInterface* aAppUi );
        
        /**
        * Destructor.
        */
        virtual ~CGSSIPAppShutter();

    public:     // New methods

        /**
        * Activates this active object
        */
        void RequestAppShutdown();

    protected:  // Functions from base classes
        
        /**
        * From CActive does the required cancellation operations (nothing)
        */
        void DoCancel();
	    
        /**
        * From CActive a request completion event occurs
        */
        void RunL();

    private:

        /**
        * C++ default constructor.
        * @param aAppUi pointer to appui
        */
        CGSSIPAppShutter( CGSPluginInterface* aAppUi );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data

        CGSPluginInterface* iAppUi;
    };

#endif      // GS_SIP_APP_SHUTTER_H
            
// End of File
