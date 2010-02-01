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
* Description:  Setting provider list item, modifies title pane
*
*/



#ifndef SIP_SETT_LIST_SIP_PROF_SET_PROV_LIST_ITEM_H
#define SIP_SETT_LIST_SIP_PROF_SET_PROV_LIST_ITEM_H

//  INCLUDES

#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS

class CAknTitlePane;

// CLASS DECLARATION

/**
*  CSIPSettListSIPProfSetProvListItem listitem class
*  @since 3.0
*  listitem class for SIP Profile settings view
*/
class CSIPSettListSIPProfSetProvListItem : public CAknTextSettingItem
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor. No two-phased construction due
        * the nature of this object
        * @param aIdentifier ID of this instance
        * @param aText Default text
        * @param aPane Pointer to modified title pane
        */
        CSIPSettListSIPProfSetProvListItem( 
            TInt aIdentifier, TDes& aText, CAknTitlePane* aPane );

        /**
        * Destructor.
        */
        virtual ~CSIPSettListSIPProfSetProvListItem();    

    public: // Functions from base classes

        /**
        * From CAknSettingItem
        * @param aSettingPage Setting page that caused the item
        * @return aEventType the type of the event
        */
        void HandleSettingPageEventL( CAknSettingPage* aSettingPage,
                                      TAknSettingPageEvent aEventType );
          
    protected:  // Data
        
        CAknTitlePane* iPane;  
    };

#endif      // SIP_SETT_LIST_SIP_PROF_SET_PROV_LIST_ITEM_H
            
// End of File
