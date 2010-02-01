/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Name        : ProfileUtil.h
* Part of     : SIP Profile Client / Settings User Interface
* Interface   : 
* Utility class for manipulating profiles and their attributes
* Version     : 1.0
*
*/




#ifndef PROFILEUTIL_H
#define PROFILEUTIL_H

// INCLUDES

#include "sipprofile.h"

// FORWARD DECLARATIONS

class TSIPProfileData;
class TSIPServerData;
class CSIPManagedProfile;

// CLASS DECLARATION

/**
*  Utility class for manipulating profiles and their attributes.
*  Contains only static methods and can never be instantiatewd
*
*  @lib SIPSettingsUI.app
*  @since 1.0
*/
class ProfileUtil
    {    
    public: // New functions
        
        /**
        * Copies profile parameters from temporary data structure
        * to profile
        * @since 1.0
        * @param aProfile     Profile where the attributes are copied to
        * @param aProfileData Temporary data structure       
        */
        static void CopyProfileAttributesL( 
            CSIPManagedProfile* aProfile, 
            const TSIPProfileData& aProfileData );
        
        /**
        * Copies proxy server parameters from temporary data structure
        * to profile
        * @since 1.0
        * @param aProfile     Profile where the attributes are copied to
        * @param aServer      Proxy server/registrar server
        * @param aProfileData Temporary data structure       
        */
        static void CopyServerAttributesL(
            CSIPManagedProfile* aProfile,
            const TUint32 aServer,
            const TSIPServerData& aProfileData );

        /**
        * Copies profiles attributes to temporary data structure
        * @since 1.0
        * @param aProfile      Profile where the attributes are copied from
        * @param aProfileData  Temporary data structure where attribs
        *                      are copied to
        */
        static void CopyToTempStruct(
            CSIPManagedProfile* aProfile,
            TSIPProfileData& aProfileData );

        /**
        * Copies server attributes to temporary data structure
        * @since 1.0
        * @param aProfile      Profile where the attributes are copied from
        * @param aServer       Proxy server/registrar server
        * @param aProfileData  Temporary data structure where attribs
        *                      are copied to
        */
        static void CopyServerSettingsToTempStructL(
            CSIPManagedProfile* aProfile,
            const TUint32 aServer,
            TSIPServerData& aProfileData );                
                                                 
        /**
        * Compares the temporary data structure with given profile
        * @since 1.0
        * @param aProfile     Profile
        * @param aProfileData Temporary data structure
        * @return ETrue if the profiles differ
        */
        static TBool ProfileModificationsL(
            CSIPManagedProfile* aProfile,
            const TSIPProfileData& aProfileData );

        /**
        * Compares the temporary data structure with given profile
        * @since 1.0
        * @param aProfile    Profile
        * @param aServer     Proxy server/registrar server
        * @param aData       Temporary data structure       
        * @return ETrue if the attributes differ
        */
        static TBool ServerModificationsL(
            CSIPManagedProfile* aProfile,
            const TUint32 aServer,
            const TSIPServerData& aData );
        
        /**
        * Copies profile information from one profile to another
        * @since 1.0
        * @param aOrigProfile   Attributes from here 
        * @param aCopiedProfile Attributes stored here               
        */
        static void CopyProfileL(
            CSIPManagedProfile* aOrigProfile,
            CSIPManagedProfile* aCopiedProfile );

        /**
        * Copies and sets server URI from given parameters
        * Used by CopyServerAttributesL()
        * @since 1.0
        */
        static void CopyServerURIL(
            CSIPManagedProfile* aProfile,
            const TUint32 aServer,
            const TSIPServerData& aProfileData );


    private:

        /**
        * C++ default constructor.
        * This class can never be instantiated and thus
        * the constructor is declared as private.
        */
        ProfileUtil();    
    };

#endif      // PROFILEUTIL_H   
            
// End of File
