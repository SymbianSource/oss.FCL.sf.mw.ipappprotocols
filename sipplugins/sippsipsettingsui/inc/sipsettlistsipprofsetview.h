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
* Description:  The profile list view controller
*
*/


#ifndef SIP_SETT_LIST_SIP_PROF_SET_VIEW_H
#define SIP_SETT_LIST_SIP_PROF_SET_VIEW_H

//  INCLUDES

#include <aknview.h>
#include "mgssipprofilehandler.h"

// FORWARD DECLARATIONS

class CSIPSettListSIPProfSetContainer;
class CSIPSettListSIPProfSetModel;
class CAknPopupList;
class MGSSIPProfileHandler;
    
// CLASS DECLARATION

/**
*  CSIPSettListSIPProfSetView view class
*  @since 3.0
*  view class for SIP Profile settings
*/
class CSIPSettListSIPProfSetView : 
      public CAknView
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor. Instance is placed on the cleanup stack
        * @param aHandler Profile handler for model
        */
        static CSIPSettListSIPProfSetView* NewLC( 
            MGSSIPProfileHandler* aHandler );
        
        /**
        * Destructor.
        */
        virtual ~CSIPSettListSIPProfSetView();

    public: // Functions from base classes

        /**
        * From CAknView.
        * Returns the UID of the view
        * @return UID of the view
        */
        TUid Id() const;        
        
    public:  
     
        /**
        * Saves all settings
        */
        TInt SaveAllSettings();     
      
    protected:  // New functions
                
        /**
        * Saves new settings and returns back to list view
        */
        void BackToListViewL();
        
        /**
        * Opens the pop-up list on the selected item
        */
        void ChangeSettingL( TInt aCommand );

    private:  // Functions from base classes
                
        /**
        * From CAknView
        * Handles the user interaction
        * @param aCommand ID of the command to respond to
        */
        void HandleCommandL( TInt aCommand ); 

        /**
        * From CAknView
        * Called upon the activation of the view. Creates the control
        * container and model
        */
        void DoActivateL( 
            const TVwsViewId& aPrevViewId, 
            TUid aCustomMessageId, 
            const TDesC8& aCustomMessage );
        
        /**
        * From CAknView
        * Deletes the control container and model
        */
        void DoDeactivate();
        
        /**
        * From CAknView (MEikMenuObserver)
        * Dynamically initializes the contents of the menu list
        */
        void DynInitMenuPaneL( TInt aResourceId,
                               CEikMenuPane* aMenuPane );

    private:    

        /**
        * C++ default constructor.
        * @param aHandler Profile handler for model
        */
        CSIPSettListSIPProfSetView( MGSSIPProfileHandler* aHandler );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        
        // The view part of the profile setting view
        CSIPSettListSIPProfSetContainer* iView;

        // The model part of the profile setting view
        CSIPSettListSIPProfSetModel* iModel;

        // Profile handler
        MGSSIPProfileHandler* iHandler;
        
        // Previous view
        TVwsViewId iPrevViewId; 
        
    friend class UT_SIPSettListSIPProfSetView;
    friend class UT_SIPSettListSIPRegSetView;
    };

#endif      // SIP_SETT_LIST_SIP_PROF_SET_VIEW_H   
            
// End of File
