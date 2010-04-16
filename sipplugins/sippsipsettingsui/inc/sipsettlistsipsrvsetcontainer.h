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
* Description:  The (common) View part for the proxy & registrar views
*
*/


#ifndef SIP_SETT_LIST_SIP_SRV_SET_CONTAINER_H
#define SIP_SETT_LIST_SIP_SRV_SET_CONTAINER_H

//  INCLUDES

#include <coecntrl.h>
#include <aknlongtapdetector.h>
#include <aknstyluspopupmenu.h>
#include <eikmobs.h>
// FORWARD DECLARATIONS

class CSIPSettListSIPSrvSetItemList;
class CAknView;
class CSIPSettListSIPSrvSetModel;

// CLASS DECLARATION

/**
*  CSIPSettListSIPSrvSetContainer container class
*  @since 3.0
*  container class for SIP Server view
*/
class CSIPSettListSIPSrvSetContainer : public CCoeControl
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aRect Client rectangle                
        * @param aProvider Object provider for displaying scroll bar
        * @param aModel Reference to the Model                
        * @param aResourceID Resource ID of the item list
        */
        static CSIPSettListSIPSrvSetContainer* NewL(
            const TRect& aRect,             
            MObjectProvider* aProvider,
            CSIPSettListSIPSrvSetModel& aModel,
            TInt aResourceID );
        
        /**
        * Destructor.
        */
        virtual ~CSIPSettListSIPSrvSetContainer();

    public: // New functions
        
        /**
        * Handle pointer event
        */
        virtual void HandlePointerEventL( const TPointerEvent& aPointerEvent );
        
        /**
        * Opens the pop-up list to modify the selected list items
        * @param aCommand ID of the command to respond to
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
        CSIPSettListSIPSrvSetContainer();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aRect Client rectangle
        * @param aModel Reference to the (list box) model   
        * @param aResourceID Resource ID of the item list     
        */
        void ConstructL(
            const TRect& aRect,
            CSIPSettListSIPSrvSetModel& aModel,
            TInt aResourceID );
    
    private:
        /**
        * Required for help.
        * 
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;

    private:    // Data

        // The item list for changing the attributes of a profile
        CSIPSettListSIPSrvSetItemList* iItemList;
        
        // Type of server view.
        TBool iProxyServerView;     
         
    };

#endif      // SIP_SETT_LIST_SIP_SRV_SET_CONTAINER_H
            
// End of File
