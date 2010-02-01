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
* Description:  The (common) controller for the proxy & registrar views
*
*/



#ifndef SIP_SETT_LIST_SIP_SRV_SET_VIEW_H
#define SIP_SETT_LIST_SIP_SRV_SET_VIEW_H

//  INCLUDES

#include <aknview.h>
#include <sipprofiletypeinfo.h>

// FORWARD DECLARATIONS
class CSIPSettListSIPSrvSetContainer;
class CSIPSettListSIPSrvSetModel;
class TSIPSettingsData;
class TGSSIPServerData;

// CLASS DECLARATION

/**
*  CSIPSettListSIPSrvSetView view class
*  @since 3.0
*  view class for SIP Server settings
*/
class CSIPSettListSIPSrvSetView : public CAknView
    {
    public:  // Constructors and destructor
        
        // No two phased construction, because this class will never
        // be directly instantiated!
        
        /**
        * Destructor.
        */
        virtual ~CSIPSettListSIPSrvSetView();
       
    protected:  // New functions        

        /**        
        * Returns used data structure
        * Deriving classes must implement this
        * @return Pointer to used server data structure
        */
        virtual TGSSIPServerData* DataStruct() = 0;

        /**        
        * Returns used item list resource id
        * Deriving classes must implement this
        * @return Resource ID
        */
        virtual TInt ResourceID() = 0;

        /**        
        * Returns used profile type
        * Deriving classes must implement this
        * @return type of selected profile
        */
        virtual TSIPProfileTypeInfo::TSIPProfileClass ProfileType() = 0;
        
        /**
        * Checks is currently shown profile locked
        * Deriving classes must implement this
        * @return ETrue if profile is locked
        */
        virtual TBool ProfileLocked() = 0;
        
        /**
        * Opens up the pop-up list for changing a setting
        * @param aCommand ID of the command to respond to
        */
        void ChangeSettingL( TInt aCommand );

        /**
        * Returns to profile setting view
        */
        void BackToSettingViewL();

    protected:  // Functions from base classes
                
        /**
        * From CAknView
        * Handles the user interaction
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

    protected:

        /**
        * C++ default constructor.
        * @param aTitleResourceID Title pane text's resource ID
        */
        CSIPSettListSIPSrvSetView( TInt aTitleResourceID );
           
    protected:  // Data
        
        // The view part of the profile list view
        CSIPSettListSIPSrvSetContainer* iView;

        // The model part of the profile list view
        CSIPSettListSIPSrvSetModel* iModel;

         // Title pane txt's resource id
        TInt iTitleResourceID;
        
        TVwsViewId iPrevViewId; // Previous view    
        
        TBool iExit;
        
    friend class UT_SIPSettListSIPRegSetView;
    };

#endif      // SIP_SETT_LIST_SIP_SRV_SET_VIEW_H
            
// End of File
