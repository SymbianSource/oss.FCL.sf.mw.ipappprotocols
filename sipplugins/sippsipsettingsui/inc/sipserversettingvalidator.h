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
* Description:  Validation methods for checking username etc.
*
*/


#ifndef SIP_SERVER_SETTING_VALIDATOR_H
#define SIP_SERVER_SETTING_VALIDATOR_H

//  INCLUDES

#include <e32std.h>

/**
*  SIPServerSettingValidator utility class
*  @since 3.0
*  Utility class for validating attributes
*/
class SIPServerSettingValidator
    {
    public: // New functions
        
        /**
        * Used to validate proxy/registrar server username
        * @param aTxt Pointer to descriptor containing the name
        * @return ETrue OK, EFalse invalid characters used
        */
        static TBool ValidateServerUsernameL( TAny* aDes );

        /**
        * Used to validate proxy/registrar server address
        * @param aTxt Pointer to descriptor containing the name
        * @return ETrue OK, EFalse invalid characters used
        */
        static TBool ValidateServerAddressL( TAny* aDes );

    private:

        /**
        * C++ default constructor.
        */
        SIPServerSettingValidator();        
    };

#endif      // SIP_SERVER_SETTING_VALIDATOR_H
            
// End of File
