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
* Description:  The item list of profile setting view
*
*/


#ifndef SIP_SETT_LIST_SIP_PROF_SET_ITEM_LIST_H
#define SIP_SETT_LIST_SIP_PROF_SET_ITEM_LIST_H

//  INCLUDES

#include    <aknsettingitemlist.h>
#include    "tsipsettingsdata.h"

// FORWARD DECLARATIONS

class CSIPSettListSIPProfSetModel;
class CAknTitlePane;

// CLASS DECLARATION

/**
*  CSIPSettListSIPProfSetItemList itemlist class
*  @since 3.0
*  itemlist class for SIP Profile settings view
*/
class CSIPSettListSIPProfSetItemList : public CAknSettingItemList
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aParent Parent control for the item list
        * @param aModel Model to store/fetch the profile attributes
        * @param aPane  Title pane to modify it, 
        *               when service provider name changes                
        */
        static CSIPSettListSIPProfSetItemList* NewL( 
            CCoeControl* aParent, CSIPSettListSIPProfSetModel* aModel,
            CAknTitlePane* aPane );
        
        /**
        * Destructor.
        */
        virtual ~CSIPSettListSIPProfSetItemList();

    public: // Functions from base classes

        /**
        * From CAknSettingItemList
        * Called upon creation
        * @param TInt ID of the item
        * @return Pointer to setting item
        */
        CAknSettingItem* CreateSettingItemL( TInt aSettingId ); 
        
        /**
        * From MEikListBoxObserver.
        * Handles the commands coming from the listbox
        * @param aListBox List box that initiated the event
        * @param aEventType Type of the event         
        */
        void HandleListBoxEventL( CEikListBox* aListBox,
                                  TListBoxEvent aEventType );   

        /**
        * From CAknSettingItemList
        * Launches the setting page for the current item
        * @param aIndex current item's (Visible) index 
        * @param aCalledFromMenu called from menu
        */
        void EditItemL( TInt aIndex, TBool aCalledFromMenu );

		/**
		 * From CAknSettingItemList
		 * Maps the current selected Access point index to its UID
		 * and stores the setting value
		 */
		void StoreSettingsL();

    private:
    
        /**
        * C++ default constructor.
        * @param aModel Pointer to the model
        * @param aPane  Title pane to modify it, 
        *               when service provider name changes        
        * @param aData  Reference to the data structure
        */
        CSIPSettListSIPProfSetItemList( 
            CSIPSettListSIPProfSetModel* aModel,
            CAknTitlePane* aPane,
            TSIPSettingsData& aData );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CCoeControl* aParent );
        
        /**
        * Changes current setting item setting to next one.
        * @param TInt ID of the item
        */
        void SetNextSettingItemL( TInt aSettingId ); 
        
    protected:      // Data

        // The setting items, this class owns these
        CAknSettingItem* iProviderName;
        CAknSettingItem* iServiceProfile;
        CAknSettingItem* iDefaultDest;
        CAknSettingItem* iDefaultAP;
        CAknSettingItem* iUsername;
        CAknSettingItem* iCompression;
        CAknSettingItem* iRegistration;
        CAknSettingItem* iSecurity;
        CAknSettingItem* iProxyServer;
        CAknSettingItem* iRegistrarServer;

        // Pointer to the data model
        CSIPSettListSIPProfSetModel* iModel;

        // Pointer to title pane
        CAknTitlePane* iPane;

        // Profile data
        TSIPSettingsData& iProfileData;

        // Flag to get the modifications to provider name
        // for updating the title pane
        TBool iModifyingProviderName;

		// An index for the destinations list
		TInt iDestIndex;

		// An index for the AP list
		TInt iAPIndex;
    };


#endif      // SIP_SETT_LIST_SIP_PROF_SET_ITEM_LIST_H
            
// End of File
