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
* Description:  SIP Settings view controller
*
*/
#ifndef SIP_SETT_INTEGER_SETPAGE_H
#define SIP_SETT_INTEGER_SETPAGE_H

#include    <akntextsettingpage.h>
#include    <aknsettingpage.h>
#include    "tgssipvalidatorwrapper.h"
#include    "tgssipserverdata.h"

// CLASS DECLARATION

/**
*  CSIPSettIntegerSetPage page class
*  
*  page class for SIP Server settings view
*/
class CSIPSettIntegerSetPage : public CAknIntegerSettingPage
    {
    public:  // Constructors and destructor
    	
        /**
        * C++ default constructor.
        * @param aResourceID resource ID of the setting page
        * @param aInteger , SetPage reference integer value;
        * @param aTextSettingPageFlags Flags (not used)
        */        
    	CSIPSettIntegerSetPage(
    	    TInt aResourceID, 
    	    TInt& aInteger, 
    	    TInt aTextSettingPageFlags );

        /**
        * Destructor.
        */
        virtual ~CSIPSettIntegerSetPage();

    public: // Functions from base classes
        
        /**
        * From CAknTextSettingPage
        * Called to validate input before closing the page
        * @param aAccept ETrue = User pressed OK, else EFalse
        * @return ETrue = delete the page
        */
        TBool OkToExitL( TBool aAccept );
        // Called to the Setpage initialize complete

    	void  DynamicInitL ();
  
    	

    private:    // Data
        TInt iValue;
    	TBuf<KMaxServerPortLength> iNULLString;
     
    };


#endif /* SIPSETTINTEGERSETPAGE_H_ */
