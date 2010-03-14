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


#ifndef SIP_SETT_LIST_SIP_PROF_SET_CONTAINER_H
#define SIP_SETT_LIST_SIP_PROF_SET_CONTAINER_H

//  INCLUDES

#include <coecntrl.h>

#include <aknlongtapdetector.h>
#include <aknstyluspopupmenu.h>
#include <eikmobs.h>
// FORWARD DECLARATIONS

class CSIPSettListSIPProfSetItemList;
class CSIPSettListSIPProfSetModel;
class CAknView;
class CAknTitlePane;

// CLASS DECLARATION

/**
*  CSIPSettListSIPProfSetContainer container class
*  @since 3.0
*  container class for SIP Profile settings view
*/
class CSIPSettListSIPProfSetContainer : public CCoeControl
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aIndex activated index on the item list
        * @param aPane The title pane
        * @param aProvider Pointer to the provider
        * @param aRect Client rectangle                
        * @param aModel Reference to the Model  
        */
        static CSIPSettListSIPProfSetContainer* NewL( 
            TInt aIndex,
            CAknTitlePane* aPane,
            MObjectProvider* aProvider,
            const TRect& aRect,                        
            CSIPSettListSIPProfSetModel& aModel );
        
        /**
        * Destructor.
        */
        virtual ~CSIPSettListSIPProfSetContainer();
        
        /**
        * Handle pointer event
        */
         virtual void HandlePointerEventL( const TPointerEvent& aPointerEvent );
         
    
    public: // New functions
        
        /**
        * Opens the pop-up list to modify the selected list items
        */
        void EditCurrentListItemL( TInt aCommand );   

        /**
        * Stores settings from item list to variables
        */
        void StoreSettingsL();

    protected:  // Functions from base classes
                
        /**
         * From CoeControl returns the amount of controls
         */
        TInt CountComponentControls() const;

        /**
         * From CCoeControl returns controls for CONE actions
         */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
         * From CCoeControl changes the size of the list box
         */
        void SizeChanged();

        /**
         * From CCoeControl, receives key events and passes them
         * to list box
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );
                                     
        /**
         * From CCoeControl, responds to a change in focus
         */
        void FocusChanged( TDrawNow aDrawNow );
        
        /**
         * From CoeControl, resource change handling
         */
        void HandleResourceChange( TInt aType );

    private:

        /**
        * C++ default constructor.
         */
        CSIPSettListSIPProfSetContainer();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aIndex activated index on the item list
        * @param aPane The title pane
        * @param aRect Client rectangle
        * @param aModel Reference to the (list box) model        
        */
        void ConstructL( 
            TInt aIndex,
            CAknTitlePane* aPane, 
            const TRect& aRect,             
            CSIPSettListSIPProfSetModel& aModel );
    
    private:
        /**
        * Required for help.
        * 
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;
            
    private:    // Data
        
        // The item list for changing the attributes of a profile
        CSIPSettListSIPProfSetItemList* iItemList;

    /**
    * For testing purposes
    */        
    friend class UT_SIPSettListSIPProfSetContainer;
    };

#endif      // SIP_SETT_LIST_SIP_PROF_SET_CONTAINER_H
            
// End of File
