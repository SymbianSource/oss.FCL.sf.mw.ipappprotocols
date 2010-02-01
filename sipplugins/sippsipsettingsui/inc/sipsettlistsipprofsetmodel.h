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
* Description:  The model of profile setting view
*
*/


#ifndef SIP_SETT_LIST_SIP_PROF_SET_MODEL_H
#define SIP_SETT_LIST_SIP_PROF_SET_MODEL_H

//  INCLUDES

#include <e32base.h>
#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS

class TSIPSettingsData;
class CGSSIPModel;
class TSIPProfileTypeInfo;
class MGSSIPProfileHandler;

// CLASS DECLARATION

/**
*  CSIPSettListSIPProfSetModel model class
*  @since 3.0
*  model class for SIP Profile settings view
*/
class CSIPSettListSIPProfSetModel : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aHandler Profile handler for manipulating profiles
        */
        static CSIPSettListSIPProfSetModel* NewL( 
        	MGSSIPProfileHandler* aHandler );
        
        /**
        * Destructor.
        */
        virtual ~CSIPSettListSIPProfSetModel();

    public: // New functions

        /**
        * Fetches the settings of the modified profile
        * @return Reference to the settings data structure
        */        
        TSIPSettingsData& Settings();

        /**
        * Fetches the list of APs for the item list
        * @param aArray The array, where all the APs are placed
        * @param aIndex Currently selected AP
        */
        void GetAPListL( CArrayPtr<CAknEnumeratedText>* aArray,
                         TInt& aIndex );

        /**
        * Fetches the list of destinationss for the item list
        * @param aArray The array, where all the destinations are placed
        * @param aIndex Currently selected destination
        */
        void GetDestinationsListL( CArrayPtr<CAknEnumeratedText>* aArray,
                         TInt& aIndex );

        /**
        * Fetches the list of types for the item list
        * @param aArray The array, where all the types are
        * @param aIndex Currently selected AP
        */
        void GetTypeListL( CArrayPtr<CAknEnumeratedText>* aArray,
                           TInt& aIndex );

        /**
        * Returns the service provider name for title pane
        * @return Buffer containing the provider name
        */
        HBufC* ProviderNameLC();

        /**
        * Returns the service profile data struct with index.
        * @param aIndex A index to a type array.
        * @return Reference to the type info struct.
        */
        TSIPProfileTypeInfo& ProfileTypeInfo( TInt aIndex );

        /**
        * Stores the service profile setting, i.e. matches the chosen
        * index to the TSIPProfileTypeInfo struct accordingly
        */
        void StoreServiceProfileSetting();

        /**
        * Starts saving the settings of the modified profile
        */
        void SaveProfileL();

		/**
		* Get the UID of the access point based on the index
		* @param aIndex The index of selected access point
		* @return the UID of the access point
		*/
		TUint32 APUID( const TInt aIndex ) const;
		
		/**
		* Get the UID of the access point
		* @return the UID of the access point
		*/
		TUint32 APUID() const;
		
		/**
		* Get the UID of the snap based on the index
		* @param aIndex The index of selected destination
		* @return the UID of the snap
		*/
		TUint32 SNAPUID( const TInt aIndex ) const;
		
		/**
		* Checks that given name is not already exists. Provides 
		* new name if same name exists in the device.
		* @param aName for name to be checked
		* @return ETrue if duplicate names was found
		*/
		TBool CheckDuplicateNameL( TDes& aName);
		
		/**
        * Checks is currently shown profile locked.
        * @return ETrue if profile is locked
        */
        TBool ProfileLocked();
        
		/**
        * Checks if SNAP is defined and so AP setting is locked.
        * @return ETrue if AP setting is locked, otherwise EFalse
        */
        TBool APLocked();
        
		/**
        * SNAPid defined, lock AP setting.
        */
        void SetApLocked();
		
		/**
		* Checks if profile is registered.
        * @return ETrue if profile is registered otherwise EFalse
		*/
		TBool IsRegistered();
		
        /**
		* Remove defined AP UID .
		*/
        void CleanAPUid();
        
        /**
		* Gets the destinations count.
		* @return Count of destinations.
		*/
        TInt DestinationsCountL( );
		
    private:

        /**
        * C++ default constructor.
        * @param aHandler Profile handler for manipulating profiles
        */
        CSIPSettListSIPProfSetModel( MGSSIPProfileHandler* aHandler );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data

        // Pointer to profile handler
        MGSSIPProfileHandler* iHandler;

        // Pointer to profile data
        TSIPSettingsData* iProfileData;

        // Pointer to service profile array
        RArray<TSIPProfileTypeInfo>* iTypeInfo;       

		// Array stores the mapping between 
		// the APs index and the UID of the APs
		RArray<TUint32> iAPsUIDs;
		
		// Array stores the mapping between 
		// the SNAP's index and the UID of the SNAPs
		RArray<TUint32> iSNAPUIDs;
		
		// Profile locked info
        TBool iLocked; 

		// AP setting locked info
        TBool iAPLocked; 
        
        // Profile registration status
        TBool iRegistered;
    };

#endif      // SIP_SETT_LIST_SIP_PROF_SET_MODEL_H   
            
// End of File
