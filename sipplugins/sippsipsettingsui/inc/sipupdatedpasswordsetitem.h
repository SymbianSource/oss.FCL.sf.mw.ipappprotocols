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
* Description:  For updating the list box every time
*
*/


#ifndef SIP_UPDATED_PASSWORD_SET_ITEM_H
#define SIP_UPDATED_PASSWORD_SET_ITEM_H

//  INCLUDES

#include <aknsettingitemlist.h>

// CLASS DECLARATION

/**
*  CSIPUpdatedPasswordSetItem class
*  @since 3.0
*  Updates list box every time, behaves like a password setting item
*/
class CSIPUpdatedPasswordSetItem : public CAknPasswordSettingItem
    {
    public:

        /**
        * Constructor, passes the parameters to CAknPasswordSettingItem's
        * constructor
        */
        CSIPUpdatedPasswordSetItem( 
            TInt aIdentifier, 
            enum TAknPasswordSettingItemMode aMode, 
            TDes& aPassword );    

        /**
        * Destructor
        */
        virtual ~CSIPUpdatedPasswordSetItem();

        /**
        * Updates the list box always after displaying popup list
        */
        void EditItemL( TBool aCalledFromMenu );
    
    private:
    
        // Ref to the password setting item.
        TDes& iPassword;
    };

#endif      // SIP_UPDATED_PASSWORD_SET_ITEM_H   
            
// End of File
