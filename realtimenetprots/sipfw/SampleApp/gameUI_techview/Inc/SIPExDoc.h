
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



#ifndef CSIPLINEDOC_H
#define CSIPLINEDOC_H

// INCLUDES

#include <eikdoc.h>
#include <e32base.h>


// FORWARD DECLARATIONS
class CEikApplication;
class CSIPExEngine;
class MSIPExGameObserver;

// CLASS DECLARATION
/**
* The document class.
* Document part of the CKON application framework for the SIPLine.
* Creates appui.
*/
class CSIPExDoc 
    : public CEikDocument
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aApp reference to the application class instance
        */
        static CSIPExDoc* NewL( CEikApplication& aApp );
        
        /**
        * Destructor.
        */
        virtual ~CSIPExDoc();
    
    public: // New functions
    
        /**
        * Creates the application engine.
        * @param aObserver A reference to the game observer.
        * @return A pointer to engine. The client gets ownership of 
        *         the engine.
        */
        CSIPExEngine* CreateEngineL( MSIPExGameObserver& aObserver );        
 
    public: // Functions from base classes

        /**
        * From CEikDocument
        * Creates instance of appui class
        * @return Pointer to the appui class instance
        */
        CEikAppUi* CreateAppUiL();
    
    private:

        /**
        * C++ default constructor.
        * @param aApp Reference to the application class
        */
        CSIPExDoc( CEikApplication& aApp );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();            


    private:    // Data
        // Owned: The application engine pointer
        CSIPExEngine*  iEngine;
    };

#endif      // CSIPLINEDOC_H   
            
// End of File
