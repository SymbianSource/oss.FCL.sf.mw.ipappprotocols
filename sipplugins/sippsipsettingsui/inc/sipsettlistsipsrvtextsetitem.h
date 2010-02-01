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
* Description:  Text setting item with a validation of the entered text
*
*/



#ifndef SIP_SETT_LIST_SIP_SRV_TEXT_SET_ITEM_H
#define SIP_SETT_LIST_SIP_SRV_TEXT_SET_ITEM_H

//  INCLUDES

#include <aknsettingitemlist.h>


// CLASS DECLARATION

/**
*  CSIPSettListSIPSrvTextSetItem item class
*  @since 3.0
*  item class for SIP Server settings view
*/
class CSIPSettListSIPSrvTextSetItem : public CAknTextSettingItem
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor. No two-phased construction due
        * the nature of this object
        * @param aIdentifier ID of this instance
        * @param aText Default text
        * @param aFunction Valitdation function
        */
        CSIPSettListSIPSrvTextSetItem( 
            TInt aIdentifier, TDes& aText, TBool (*aFunctionL)( TAny* aPtr ) );

        /**
        * Destructor.
        */
        virtual ~CSIPSettListSIPSrvTextSetItem();    

    public: // Functions from base classes

        /**
        * From CAknSettingItem
        * Called when user wants to edit the text
        * @param aCalledFromMenu Called from menu or not (doesn't matter, tho)
        */
        void EditItemL( TBool aCalledFromMenu );
          
    protected:  // Data
                
        TBool (*iFunctionL)( TAny* aPtr );
    };

#endif      // SIP_SETT_LIST_SIP_SRV_TEXT_SET_ITEM_H
            
// End of File
