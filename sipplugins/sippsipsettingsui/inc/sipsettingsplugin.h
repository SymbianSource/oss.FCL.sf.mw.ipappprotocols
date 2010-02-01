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
* Description:  
*     SIPSettings plugin implementation
*
*/


#ifndef SIP_SETTINGS_PLUGIN_H
#define SIP_SETTINGS_PLUGIN_H

// System includes
#include     <eikclb.h>
#include     <ConeResLoader.h>
#include     <gsplugininterface.h>
#include     "MGsFWMSKLabelObserver.h"

// Classes referenced
class CSIPSettingsContainer;
class CAknNavigationDecorator;
class CAknViewAppUi;
class CEikFormattedCellListBox;
class CAknPopupList;
class CSIPSettingsModel;
class CGSSIPModel;

// Constants
const TUid KGSSIPSettingsPluginUID = { 0x101FD6D0 }; //SIP Settings ECom plug-in uid
//const   TUid    KUidGS = { 0x100058EC }; // GS uid, needed in help handling
_LIT( KSIPSettingsResourceFileName, "z:GSSIPSettingsPluginRsc.rsc" );
_LIT( KGSDefaultSIPIconFileName, "Z:\\resource\\apps\\GSSIPSettingsplugin.mbm");
_LIT( KSIPluginIconFileName, "\\resource\\apps\\GSSIPSettingsplugin.mif");



// CLASS DECLARATION

/**
*  CPacketDataPlugin view class.
*
* This class handles state and application logic of SIP settings.
*
*  
*/
class CSIPSettingsPlugin 
    : public CGSPluginInterface, 
      public MEikListBoxObserver,
      public MGsFWMSKObserver
      
    {
    public: // Constructors and destructor
        /**
        * Symbian OS two-phased constructor
        * @return 
        */
        static CSIPSettingsPlugin* NewL( TAny* aAppUi );
    
        /**
        * Destructor.
        */
        ~CSIPSettingsPlugin();

        /**
        * Checks if the MSK label needs to be adjusted.
        */
        void CheckMiddleSoftkeyLabelL();
        
        /**
        * Remove command and label from MSK.
        */
        void RemoveCommandFromMSK( const TBool flag);
      
        /**
        * Adds given resource text as MSK to CBA.
        * 
        * @param aResourceId middle softkey label.
        * @param aCommandId command that should be performed when MSK
        *        is pressed.
        */
        void SetMiddleSoftKeyLabelL( const TInt aResourceId,
            const TInt aCommandId );
        
    public: // From CAknView
        TUid Id() const;
        void HandleCommandL( TInt aCommand );

    protected: // From CAknView
        void HandleClientRectChange();
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );
        void DoDeactivate();

    protected:

        /**
        * C++ default constructor.
        */
        CSIPSettingsPlugin( );

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();
    
    public: // From CGSPluginInterface

        /**
        * Method for getting caption of this plugin. This should be the
        * localized name of the settings view to be shown in parent view.
        *
        * @param aCaption pointer to Caption variable
        */
        void GetCaptionL( TDes& aCaption ) const;
        
        /**
        * Method for reading the ID of the plugin provider category. See
        * TGSPluginProviderCategory. PluginProviderCategory can be used for
        * sorting plugins.
        *
        * Default value is EGSPluginProvider3rdParty. Override this function
        * to change the category.
        *
        * @return Plugin provider category ID defined by
        *         TGSPluginProviderCategory
        */
        TInt PluginProviderCategory() const;
        
        /**
        * @see CGSPluginInterface header file.
        */
        CGulIcon* CreateIconL( const TUid aIconType );
        
        /**
        * Shows the pop-up list for user to change the default profile
        */
        void ChangeDefaultProfileL();
        
        /**
        * Confirms from the user and then deletes the profile
        */
        void DeleteProfileL();

        /**
        * Starts editing an existing profile
        */
        void EditProfileL();  
    
    protected: // From MEikListBoxObserver
    
        /**
        * Handles listbox events.
        * @param aListBox Listbox where the event occurred.
        * @param aEventType Event type.
        * 
        */
        void HandleListBoxEventL( CEikListBox* aListBox, 
                                  TListBoxEvent aEventType );
    
    protected:  // New functions
       
        /**
        * Shows user a pop-up list, where s/he can choose the profile,
        * which parameters are copied to a new profile
        */
        void CreateFromExisitingProfileL();

        /**
        * Creates pop-up list for choosing the cloned profile
        * @param aListBox List box used in the pop-up list             
        * @return Initialized pop-up list and list box
        */
        CAknPopupList* CreatePopupListL( CEikFormattedCellListBox* aListBox );

        /**
        * Creates a new profile and moves to profile edit view (deactivates 
        * this view        
        * @param aIndex index to point the cloned profile
        */
        void CreateNewProfileL( TInt aIndex );

    private:  // Functions from base classes
        
        /**
        * From CAknView (MEikMenuObserver)
        * Dynamically initializes the contents of the menu list
        */
        void DynInitMenuPaneL( TInt aResourceId,
                               CEikMenuPane* aMenuPane );        

    private: // New
        void HandleListBoxSelectionL();
        void ShowSIPProfilesSettingPageL();
        
    protected: //Data        
        RConeResourceLoader iResourceLoader;    // resource loader

        //check if which command MSK is set during destruction
        TBool                       iMskCommandFlag;

        CSIPSettingsContainer* iContainer; // pointer to the container
        CAknNavigationDecorator* iNaviPaneContext; // navi pane
        
        CGSSIPModel* iHandler; // pointer to SIP profile handler
        CSIPSettingsModel* iModel;
        TInt iActiveIndex; // Active index number

        TInt iCurrentItem; // currently selected listbox item
        TInt iTopItemIndex; // first item in the listbox
        TBool iElaf;

        TVwsViewId iPrevViewId; // Previous view
        
    	TBool iListBoxEventPenDown; // Check the ListBoxEvent
    };

#endif
