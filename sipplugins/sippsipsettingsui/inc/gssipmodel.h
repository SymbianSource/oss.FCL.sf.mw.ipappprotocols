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
* Description:  SIP Settings model.
*
*/


#ifndef GS_SIP_MODEL_H
#define GS_SIP_MODEL_H

// INCLUDES

#include <e32base.h>
#include "tsipsettingsdata.h"
#include "mgssipprofilehandler.h"
#include <sipprofileregistryobserver.h>
#include <sipmanagedprofile.h>
#include <sipmanagedprofileregistry.h>

// FORWARD DECLARATIONS

class CGSPluginInterface;
class CSIPManagedProfileRegistry;
class MSIPProfileRegistryObserver;
class CGSSIPAppShutter; 

// CLASS DECLARATION
/**
*  CGSSIPModel model class
*  @since 3.0
*  model class for all SIP Settings
*/
class CGSSIPModel 
    : public CBase,
      public MSIPProfileRegistryObserver,
      public MGSSIPProfileHandler
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @return Pointer to created instance
        */
        static CGSSIPModel* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CGSSIPModel();
    
    public: // New functions
    
        /**
        * Creates the SIP profile engine       
        * @param aAppUi for CGSSIPPSettingsPlugin UI
        */
        void CreateEngineL( CGSPluginInterface* aAppUi );                        
        
        /**
        * Sets flag to quit the application after save has occured
        */
        void QuitAfterSave();
        
        /**
        * Returns pointer to the profile based on id
        * @param aId Profile id
        * @return Pointer to returned profile
        */
        CSIPManagedProfile* Profile( TUint aId );

        /**
        * Checks if profile can be deleted.
        * @param aIndex Index to the deleted profile
        * @param aIsUse ETrue if profile is in use
        * @param aDefault ETrue if profile is default profile 
        * @param aLocked ETrue if profile is locked 
        */
        void CheckProfileForDeleteL( 
            TInt aIndex, TBool& aIsUse, TBool& aDefault, TBool& aLocked );

        /**
        * Checks is profile in use.
        * @param aProfileId for SIP profile id
        * @return ETrue if profile is in use 
        */
        TBool CheckIsProfileInUseL( TUint32 aProfileId );

    public: // Functions from base classes

        /** 
        * from MSIPProfileRegistryObserver 
        * SIP profile information event
        * @param aProfileId is id for profile
        * @param aEvent type of information event
		*/
		void ProfileRegistryEventOccurred(
		    TUint32 aSIPProfileId, TEvent aEvent );
        
        /** 
        * SIP profile creation event
        * @param aSIPProfileId id of new SIP Profile
		*/
		void ProfileCreatedL( TUint32 aSIPProfileId );

		/** 
		* SIP profile information update event
		* @param aSIPProfileId id of updated SIP Profile
		*/
		void ProfileUpdatedL( TUint32 aSIPProfileId );

		/**
		* SIP profile registration status changed event
		* @param aSIPProfileId id of profile which registration 
		*        status has been changed.
		*/
		void ProfileRegistrationStatusChanged( TUint32 aSIPProfileId );

		/**		
        * SIP profile destruction event.
        * @param aSIPProfileId id of profile which was destroyed
		*/
		void ProfileDestroyed( TUint32 aSIPProfileId );
		
        /**
        * Added profile index in the profiles list
        * @return profile index
        */
        TInt AddedProfileIndex();

		/**
        * from MSIPProfileRegistryObserver
		* An asynchronous error has occurred related to SIP profile		
        * @param aSIPProfileId the id of failed profile 
		* @param aError a error code
		*/
		void ProfileRegistryErrorOccurred(				    
		    TUint32 aSIPProfileId,
		    TInt aError);

        /**
        * from MGSSIPProfileHandler
        * Sets the default profile        
        * @param aIndex Index on the array table of the profile that is going
        *               to be a new default profile        
        */
        void SetDefaultProfileL( TInt aIndex );

        /**
        * from MGSSIPProfileHandler
        * Gets the index of the default profile
        * @return KErrNotFound, if one does not exist, otherwise index to profile
        */
        TInt DefaultProfileIndex();

        /**
        * from MGSSIPProfileHandler
        * Returns pointer to the profile array, does not change ownership
        * @return Pointer to the profile array
        */
        CArrayPtr<CSIPManagedProfile>* ProfileArray();
        
        /**
        * from MGSSIPProfileHandler
        * Starts modifying a profile, copies profile's data to iProfileData
        * @param aIndex index on the array table        
        * @return Ok to move to the next view?
        */
        TBool EditProfileL( TInt aIndex );

        /**
        * from MGSSIPProfileHandler
        * Starts editing a profile that is based on existing one. 
        * @param aCopiedIndex index on the array table of the copied profile       
        */
        void CreateAndEditProfileL( TInt aCopiedIndex );
        
        /**
        * from MGSSIPProfileHandler
        * Deletes a profile from permanent storage
        */
        void DeleteProfileL( TInt aIndex );

        /**
        * from MGSSIPProfileHandler
        * Checks if there has been an error in registration
        * @param aIndex Index to the table
        * @return Error has happened in registration or then not
        */
        TBool ErrorInRegistration( TInt aIndex );
        
        /**
        * from MGSSIPProfileHandler
        * Returns pointer to profile data structure for modifying it directly
        * @return Pointer to iProfileData
        */
        TSIPSettingsData* ProfileData();
        
        /**
        * from MGSSIPProfileHandler
        * Saves profile's changes or returns EFalse if no changes are present.        
        * @return EFalse if no changes (not saved then), otherwise engine
        *         starts saving the modifications
        */
        TBool SaveProfileL();
        
        /**
        * from MGSSIPProfileHandler
        * Returns list of different profile service types
        * @return list of types
        */
        RArray<TSIPProfileTypeInfo>* TypeListL();

        /**
        * Checks that there are no duplicate profile names.
        * @param aProfileId for current profile to be checked.
        * @param aName for profile name to be checked.
        * @param aCreatedFromExistingProfile to inform if the profile
        * was created from existing profile and language specific
        * number conversions must be done.
        */
        void CheckDuplicateNameL( TInt32 aProfileId, TDes& aName,  
                           TBool aCreatedFromExistingProfile = EFalse);  

    protected:  // New functions
        
        /**
        * Reads the profile array from SIP Profile Client
        */        
        void ReadArrayFromEngineL();

        /**
        * Copies profile from CSIPManagedProfile to iProfileData for modifications
        * @param aIndex The index to the array to be copied
        */
        void CopyToProfileDataStructL( TInt aIndex );    

        /**
        * Fills the data structure with default values
        */
        void FillDataStructWithDefaultValsL();

        /**
        * Checks if user has made any modfications to the existing profile
        * @return ETrue if user has modified the profile
        */
        TBool CheckForModificationsL();

        /**
        * Copies data from iProfileData to the actual CSIPManagedProfile
        * @return Profile where the stuff was copied to..
        */
        CSIPManagedProfile* CopyDataToProfileL();

        /**        
        * Replaces one profile with another on the array; only cloned profiles
        * should be replaced
        * @param aIndex   Index on the table of the array that is replaced
        * @param aProfile The profile that replaces existing profile
        */
        void ReplaceProfileL( TInt aId, CSIPManagedProfile* aProfile );

        /**
        * Reads profile list from the engine; if reading fails,
        * keeps the old list safe and returns it
        */
        void ReadProfileListFromEngineSafeL();

        /**
        * Calls app ui to udpate the lists; might also trigger exit
        * @param aCmd Command that is passed to view
        */
        void UpdateListCmd( TInt aCmd = EGSCmdUpdateList );

    private:

        /**
        * C++ default constructor.
        */
        CGSSIPModel();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();            

        /**
        * For deleting RPointerArray in case of leave
        */
        static void ResetAndDestroy( TAny* aPointerArray );   
        
        /**
        * For deleting allocated RArray in case of leave
        * @param aArray An array to be destroyed.
        */
        static void CloseAndDestroy( TAny* aArray );     

    private:    // Data
    
        // The profile's data, which is manipulated
        TSIPSettingsData iProfileData;

        // Instance to the engine
        CSIPManagedProfileRegistry* iEngine;
        
        // Array of all the profiles
        CArrayPtrFlat<CSIPManagedProfile>* iProfiles;

        // Temporary pointer storage for a new profile before
        // placing it to the permanent storage
        CSIPManagedProfile* iNewProfile;
        
        // Flag indicating if one should quit after save
        TBool iQuitAfterSave;
        
        // For quitting app asynchronously after save
        CGSSIPAppShutter* iAppShutter;
        
        // Instance to plugin UI
        CGSPluginInterface* iAppUi;
        
        //Index of new profile in the list
        TInt iAddedProfileIndex;
    };

#endif      // GS_SIP_MODEL_H   
            
// End of File
