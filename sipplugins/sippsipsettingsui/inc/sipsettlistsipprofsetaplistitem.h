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
* Description:  The modified list item for the access points
*
*/


#ifndef SIP_SETT_LIST_SIP_PROF_SET_AP_LIST_ITEM_H
#define SIP_SETT_LIST_SIP_PROF_SET_AP_LIST_ITEM_H

//  INCLUDES

#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS

class CSIPSettListSIPProfSetModel;
class CAknTitlePane;

// CLASS DECLARATION

/**
*  CSIPSettListSIPProfSetAPListItem listitem class
*  @since 3.0
*  listitem class for SIP Profile settings Access Points
*/
class CSIPSettListSIPProfSetAPListItem : 
	public CAknEnumeratedTextPopupSettingItem
    {

    public:  // Constructors and destructor
        
        /**
        * Constructor. No two-phased construction due
        * the nature of this object
        * @param aResourceId Resource ID for the list item
        * @param aValue The currently selected item
        * @param aModel Model for fetching the AP list        
        */                
        CSIPSettListSIPProfSetAPListItem( TInt aResourceId, TInt& aIndex,
										CSIPSettListSIPProfSetModel* aModel );
        
        /**
        * Destructor.
        */
        virtual ~CSIPSettListSIPProfSetAPListItem();

    protected:  // New functions
        
        /**
        * Checks that selected AP exists on the list
         @param aArray Array containing all the APs
        * @return Boolean value indicating if the selected AP is found
        */
        TBool ValueMatches( CArrayPtr<CAknEnumeratedText>* aArray );

        /**
        * Initializes the enumeration list
        * @param aTextArray enumeration value/txt values are stored here
        * @param aNullTextArray created, but not used (made for framework)
        */
        void InitializeListL( CArrayPtr<CAknEnumeratedText>*& aTextArray,
                              CArrayPtr<HBufC>*& aNullTextArray );

    protected:  // Functions from base classes
        
        /**
        * From CAknSettingItem, dynamically initializes the list
        * after base construction
        */
        void CompleteConstructionL();

        /**
        * To set the list, if the AP is not selected 
        */
        void EditItemL( TBool aCalledFromMenu );

        /*
		* From MAknSettingPageObserver
		* @see MAknSettingPageObserver
		*/
        void HandleSettingPageEventL( 
            CAknSettingPage* aSettingPage, 
            TAknSettingPageEvent aEventType );
        
    private:

        /**
        * For deleting array in case of leave
        */
        static void ResetAndDestroy( TAny* aPointerArray );   

    private:    // Data

        // Model
        CSIPSettListSIPProfSetModel* iModel;        

        // Currently selected AP Index
        TInt& iEnumValue;
        
        // Backup value for AP index
        TInt iBackupValue;
    };

#endif      // SIP_SETT_LIST_SIP_PROF_SET_AP_LIST_ITEM_H
            
// End of File
