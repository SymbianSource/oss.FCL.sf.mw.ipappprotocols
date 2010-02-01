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
* Description:  Mixin-class that provides functionality for manipulating 
*                profiles
*
*/



#ifndef M_GS_SIP_PROFILE_HANDLER_H
#define M_GS_SIP_PROFILE_HANDLER_H

// INCLUDES

#include "tsipsettingsdata.h"
#include <sipmanagedprofile.h>

// CLASS DECLARATION


/**
*  MGSSIPProfileHandler class
*  @since 3.0
*  Mixin class for profile manipulation
*  Handles profile manipulations 
*/
class MGSSIPProfileHandler
    {   
    public: // New functions
    
        /**
        * Setting the default profile is a special case of modifying a profile
        * attribute and it can be done directly here.
        * @param aIndex Index on the array table of the profile that is going
        *               to be a new default profile
        */
        virtual void SetDefaultProfileL( TInt aIndex ) = 0;

        /**
        * Gets the index of the default profile
        * @return KErrNotFound, if one does not exist, otherwise index to profile
        */
        virtual TInt DefaultProfileIndex() = 0;

        /**
        * Returns pointer to the profile array, does not change ownership
        * @return Pointer to the profile array
        */
        virtual CArrayPtr<CSIPManagedProfile>* ProfileArray() = 0;

        /**
        * Starts modifying a profile, copies profile's data to iProfileData
        * @param aIndex index on the array table
        * @return Ok to move to the next view?
        */
        virtual TBool EditProfileL( TInt aIndex ) = 0;

        /**
        * Starts editing a profile that is based on existing one. At the save
        * time a new profile is created and the information is copied there
        * @param aCopiedIndex index on the array table of the copied profile
        */
        virtual void CreateAndEditProfileL( TInt aCopiedIndex ) = 0;

        /**
        * Deletes a profile from permanent storage
        */
        virtual void DeleteProfileL( TInt aIndex ) = 0;
        
        /**
        * Checks if there has been an error in registration for some
        * particular probile
        * @param aIndex Index to the table
        * @return Error has happened in registration or then not
        */
        virtual TBool ErrorInRegistration( TInt aIndex ) = 0;

        /**
        * Returns pointer to profile data structure for modifying it directly
        * @return Pointer to iProfileData
        */
        virtual TSIPSettingsData* ProfileData() = 0;
      
        /**
        * Saves profile's changes or returns EFalse if no changes are present
        * and does not save anything then. Creates new profile if iProfileData's
        * iID member is -1
        * @return EFalse if no changes (not saved then), otherwise engine
        *         starts saving the modifications
        */
        virtual TBool SaveProfileL() = 0;
        
        /**
        * Returns list of different profile service types
        * @return list of types
        */
        virtual RArray<TSIPProfileTypeInfo>* TypeListL() = 0;

        /**
        * Checks that there are no duplicate profile names.
        * @param aProfileId for current profile to be checked.
        * @param aName for profile name to be checked.
        * @param aCreatedFromExistingProfile to inform if the profile
        * was created from existing profile and language specific
        * number conversions must be done.
        */
        virtual void CheckDuplicateNameL( TInt32 aProfileId, TDes& aName,
                                   TBool aCreatedFromExistingProfile = EFalse ) = 0;
    };

#endif      // M_GS_SIP_PROFILE_HANDLER_H   
            
// End of File
