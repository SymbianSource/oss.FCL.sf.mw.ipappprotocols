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
* Description:  The modified list item for the type selection
*
*/


#ifndef SIP_SETT_LIST_SIP_PROF_SET_TYPE_LIST_ITEM_H
#define SIP_SETT_LIST_SIP_PROF_SET_TYPE_LIST_ITEM_H

//  INCLUDES

#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS

class CSIPSettListSIPProfSetModel;

// CLASS DECLARATION

/**
*  CSIPSettListSIPProfSetTypeListItem listitem class
*  @since 3.0
*  listitem class for SIP Profile settings view
*/
class CSIPSettListSIPProfSetTypeListItem : 
	public CAknEnumeratedTextPopupSettingItem
    {

    public:  // Constructors and destructor
        
        /**
        * Constructor. No two-phased construction due
        * the nature of this object
        * @param aResourceId Resource ID for the list item
        * @param aValue The currently selected item
        * @param aModel Model for fetching the types
        */                
        CSIPSettListSIPProfSetTypeListItem( TInt aResourceId, TInt& aValue,
                                   CSIPSettListSIPProfSetModel* aModel );
        
        /**
        * Destructor.
        */
        virtual ~CSIPSettListSIPProfSetTypeListItem();

    protected:  // New functions
                
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

    private:

        /**
        * For deleting array in case of leave
        */
        static void ResetAndDestroy( TAny* aPointerArray );   

    private:    // Data

        // Model
        CSIPSettListSIPProfSetModel* iModel;        

        // Currently selected index
        TInt& iEnumValue;        
    };

#endif      // SIP_SETT_LIST_SIP_PROF_SET_TYPE_LIST_ITEM_H
            
// End of File
