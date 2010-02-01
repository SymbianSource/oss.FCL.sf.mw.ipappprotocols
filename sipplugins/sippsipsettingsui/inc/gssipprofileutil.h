/*
* Copyright (c) 2003 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility class for manipulating profiles and their attributes
*
*/



#ifndef GS_SIP_PPROFILE_UTIL_H
#define GS_SIP_PPROFILE_UTIL_H

// INCLUDES

#include <sipprofile.h>

// FORWARD DECLARATIONS

class TSIPSettingsData;
class TGSSIPServerData;
class CSIPManagedProfile;
class UT_GSSIPProfileUtil;

// CLASS DECLARATION

/**
*  GSSIPProfileUtil utility class
*  @since 3.0
*  Utility class for manipulating profiles and their attributes.
*/
class GSSIPProfileUtil
    {    
    public: // New functions
        
        /**
        * Copies profile parameters from temporary data structure
        * to profile
        * @param aProfile     Profile where the attributes are copied to
        * @param aProfileData Temporary data structure       
        */
        static void CopyProfileAttributesL( 
            CSIPManagedProfile* aProfile, 
            const TSIPSettingsData& aProfileData );
        
        /**
        * Copies proxy server parameters from temporary data structure
        * to profile
        * @param aProfile     Profile where the attributes are copied to
        * @param aServer      Proxy server/registrar server
        * @param aProfileData Temporary data structure       
        */
        static void CopyServerAttributesL(
            CSIPManagedProfile* aProfile,
            TUint32 aServer,
            const TGSSIPServerData& aProfileData );

        /**
        * Copies profiles attributes to temporary data structure
        * @param aProfile      Profile where the attributes are copied from
        * @param aProfileData  Temporary data structure where attribs
        *                      are copied to
        */
        static void CopyToTempStructL(
            CSIPManagedProfile* aProfile,
            TSIPSettingsData& aProfileData );

        /**
        * Copies server attributes to temporary data structure
        * @param aProfile      Profile where the attributes are copied from
        * @param aServer       Proxy server/registrar server
        * @param aProfileData  Temporary data structure where attribs
        *                      are copied to
        */
        static void CopyServerSettingsToTempStructL(
            CSIPManagedProfile* aProfile,
            TUint32 aServer,
            TGSSIPServerData& aProfileData );                
                                                 
        /**
        * Compares the temporary data structure with given profile
        * @param aProfile     Profile
        * @param aProfileData Temporary data structure
        * @return ETrue if the profiles differ
        */
        static TBool ProfileModificationsL(
            CSIPManagedProfile* aProfile,
            const TSIPSettingsData& aProfileData );

        /**
        * Compares the temporary data structure with given profile
        * @param aProfile    Profile
        * @param aServer     Proxy server/registrar server
        * @param aData       Temporary data structure       
        * @return ETrue if the attributes differ
        */
        static TBool ServerModificationsL(
            CSIPManagedProfile* aProfile,
            TUint32 aServer,
            const TGSSIPServerData& aData );
        
        /**
        * Copies profile information from one profile to another
        * @param aOrigProfile   Attributes from here 
        * @param aCopiedProfile Attributes stored here               
        */
        static void CopyProfileL(
            CSIPManagedProfile* aOrigProfile,
            CSIPManagedProfile* aCopiedProfile );

        /**
        * Copies and sets server URI from given parameters
        * Used by CopyServerAttributesL()
        */
        static void CopyServerURIL(
            CSIPManagedProfile* aProfile,
            TUint32 aServer,
            const TGSSIPServerData& aProfileData );

        /**
        * Parses uri path.
        * Used by CopyServerURIL()
        */
        static void ParseUriPathL(
            CSIPManagedProfile* aProfile,
            TUint32 aServer,
            const TGSSIPServerData& aProfileData,
            HBufC8* aFinalPath );
                        
       	/**
        * Checks if sip or sips scheme included.
        * @param aAddr address to be checked               
        * @param aHostBrackets when ETrue, host brackets are checked
        */
        static TBool SchemeIncluded( const TDesC8& aAddr, 
        							TBool aHostBrackets=EFalse );
        
		/**
		* GSSIPProfileUtil::BracketsIncluded
		* Check if brackets included correctly in IPv6 reference.
        * @return ETrue if brackets included, 
        *		  EFalse if brackets not included
        *		  -1 if brackets included incorrectly
		*/
		static TInt BracketsIncluded( TPtrC8 aHost );
        
        /**
        * Adds brackets into IPv6 address, if needed
        * @param aHost address to be checked               
        */
       	static void AddBrackets( TPtr8 aHost );
       
		/**
        * Removes brackets from IPv6 address, if needed
        * @param aHost address to be checked               
        */
       	static void RemoveBrackets( TPtr8 aHost );
       	
       	/**
       	* Check if reference is valid URI
       	* @param aValue reference to a value checked if valid URI
       	*/
       	static TInt URIValid( const TDesC8& aValue );

        /**
        * Replace %20 to ' '
        * @param aValue reference to token
        */
        static void ReplaceEscapedSpace( HBufC8* aValue );
    private:

        /**
        * C++ default constructor.
        * This class can never be instantiated and thus
        * the constructor is declared as private.
        */
        GSSIPProfileUtil();    

	/**
	* For testing purposes
	*/        
    friend class UT_GSSIPProfileUtil;
    };

#endif      // GS_SIP_PPROFILE_UTIL_H   
            
// End of File
