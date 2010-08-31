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


#ifndef SIP_SETTINGS_CONTAINER_H
#define SIP_SETTINGS_CONTAINER_H

//  INCLUDES

#include     <coecntrl.h>
#include "MGsFWMSKLabelObserver.h"


#include <aknlongtapdetector.h>
#include <aknstyluspopupmenu.h>
#include <eikmobs.h>
// FORWARD DECLARATIONS

class CSIPSettingsModel;
class CSIPSettingsPlugin;
class UT_CSIPSettingsContainer;

// CLASS DECLARATION

/**
*  CSIPSettingsContainer container class
*  @since 3.1
*  container class for SIP Profiles view
*/
class CSIPSettingsContainer : public CCoeControl,
                              public MEikMenuObserver
    {
    public:  // Constructors and destructor
        
        /**
        * SetEmphasis
        */
         void SetEmphasis( CCoeControl* /*aMenuControl*/, TBool /*aEmphasis*/ );

         /**
         * Process command
         */
         void ProcessCommandL( TInt aCommandId );
         
        /**
        * C++ default constructor.        
        */
        CSIPSettingsContainer( CSIPSettingsPlugin* aObserver );                

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aRect Client rectangle
        * @param aActiveIndex Active index on the list
        * @param aModel Pointer to the (list box) model
        */
        void ConstructL( 
            const TRect& aRect, 
            TInt aActiveIndex,
            CSIPSettingsModel* aModel );            

        /**
        * Destructor.
        */
        virtual ~CSIPSettingsContainer();

    public: // New functions
        
        /**
        * Returns the currently selected list box item
        * @return Index of the selected item
        */
        TInt CurrentIndex();

        /**
        * Selects the existing index and redraws the view.
        * Called after updating/adding a profile
        * @param aIndex an index to the modified item
        */
        void ListItemModifiedL( TInt aIndex );
    
        /**
        * Selects the next index and redraws the view.
        * Called after deletion of a profile
        */
        void ListItemDeletedL();
        
        /**
        * Retrieves the currently selected listbox feature id
        * @return feature id.
        */
        TInt CurrentFeatureId( );

        /**
        * Defines observer for the middle softkey label changes.
        * Can only be set once. Further attempts are ignored.
        */
        virtual void SetMiddleSoftkeyObserver( MGsFWMSKObserver* aObserver );
    
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
        class CItemDrawer;
        class CListBox;
        friend class CListBox;

       /**
        * Required for help.
        * 
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;
        
        /**
        * For testing purposes
        */        
        friend class UT_CSIPSettingsContainer;  
        
    public: // Data
        // The list box for showing the profiles and the status of each profile
        CListBox* iListBox;
        

    protected: // Data
        TBool iLongPress;
        CSIPSettingsModel* iModel;//not needed?
        CSIPSettingsPlugin* iObs;
        /**
        * Middle softkey label observer.
        * Own.
        */
        MGsFWMSKObserver* iMSKObserver;  
        
    };

#endif      // SIP_SETTINGS_CONTAINER_H
            
// End of File
