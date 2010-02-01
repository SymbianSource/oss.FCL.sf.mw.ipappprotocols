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
* Description:  The item list of the proxy & registrar views
*
*/



#ifndef SIP_SETT_LIST_SIP_SRV_SET_ITEM_LIST_H
#define SIP_SETT_LIST_SIP_SRV_SET_ITEM_LIST_H

//  INCLUDES

#include    <aknsettingitemlist.h>
#include    "tsipsettingsdata.h"

// FORWARD DECLARATIONS

class CSIPSettListSIPSrvSetModel;

// CLASS DECLARATION

/**
*  CSIPSettListSIPSrvSetItemList itemlist class
*  @since 3.0
*  itemlist class for SIP Server settings view
*/
class CSIPSettListSIPSrvSetItemList : public CAknSettingItemList
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aParent Parent control for the item list
        * @param aModel Model to store/fetch the profile attributes        
        * @param aResourceID Resource ID for the protocol
        */
        static CSIPSettListSIPSrvSetItemList* NewL( 
            CCoeControl* aParent, CSIPSettListSIPSrvSetModel* aModel,
            TInt aResourceID );
        
        /**
        * Destructor.
        */
        virtual ~CSIPSettListSIPSrvSetItemList();

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
        */
        void EditItemL( TInt aIndex, TBool aCalledFromMenu );

    private:
    
        /**
        * C++ default constructor.
        * @param aModel Pointer to the model   
        */
        CSIPSettListSIPSrvSetItemList( CSIPSettListSIPSrvSetModel* aModel );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aParent Parent control for the item list
        * @param aResourceID Resource ID for the protocol
        */
        void ConstructL( CCoeControl* aParent, TInt aResourceID );
        
        /**
        * Checks that does setting contains only spaces and 
        * removes them if needed.
        * @param aSettingText for setting text to be checked  
        */
        void CheckSettingForSpaces( TDes& aSettingText );

    protected:      // Data

        // The setting items, this class owns these
        CAknSettingItem* iAddress;
        CAknSettingItem* iRealm;
        CAknSettingItem* iUsername;
        CAknSettingItem* iPassword;
        CAknSettingItem* iLooseRouting;
        CAknSettingItem* iPort;
        CAknSettingItem* iTransportProtocol;
        
        // Model
        CSIPSettListSIPSrvSetModel* iModel;

        // Server data
        TGSSIPServerData* iServerData;
        
        // Server type.
        TInt iServerType;
        
        // Username. Special case for private user identity.
        TBuf<KMaxServerUsernameLength> iUName; 
        TBuf16<KMaxServerPortLength> iPortValue;
    };


#endif      // SIP_SETT_LIST_SIP_SRV_SET_ITEM_LIST_H
            
// End of File
