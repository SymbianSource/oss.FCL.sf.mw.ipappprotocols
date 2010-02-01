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
* Description:  Text setting page with a validation of the entered text
*
*/



#ifndef SIP_SETT_LIST_SIP_SRV_TEXT_SET_PAGE_H
#define SIP_SETT_LIST_SIP_SRV_TEXT_SET_PAGE_H

//  INCLUDES

#include    <akntextsettingpage.h>
#include    <aknsettingpage.h>
#include    "tgssipvalidatorwrapper.h"

// CLASS DECLARATION

/**
*  CSIPSettListSIPSrvTextSetPage page class
*  @since 3.0
*  page class for SIP Server settings view
*/
class CSIPSettListSIPSrvTextSetPage : public CAknTextSettingPage
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @param aResourceID resource ID of the setting page
        * @param aCallBack Callback method that is used to validate input
        * @param aText Reference to text descriptor
        * @param aTextSettingPageFlags Flags (not used)
        */        
	    CSIPSettListSIPSrvTextSetPage(
            TInt aResourceID, TGSSIPValidatorWrapper aValidator, 
            TDes& aText, TInt aTextSettingPageFlags = 0 );

        /**
        * Destructor.
        */
        virtual ~CSIPSettListSIPSrvTextSetPage();

    public: // Functions from base classes
        
        /**
        * From CAknTextSettingPage
        * Called to validate input before closing the page
        * @param aAccept ETrue = User pressed OK, else EFalse
        * @return ETrue = delete the page
        */
        TBool OkToExitL( TBool aAccept );
    
    protected:  // New methods

        /**
        * Reads the error msg txt from resource file
        * @return buffer containing the text
        */
        HBufC* ErrorMsgTxtLC();

    private:    // Data
        
        // Callback method for validating the input
        TGSSIPValidatorWrapper iValidator;

        // Resource ID for displaying correct error msg
        TInt iID;
    };

     
#endif      // SIP_SETT_LIST_SIP_SRV_TEXT_SET_PAGE_H   
            
// End of File
