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
* Name        : sipprofilestoragebase.h
* Part of     : SIP Profile Server
* Interface   : private
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef SIPPROFILESTORAGEBASE_H
#define SIPPROFILESTORAGEBASE_H

//  INCLUDES

#include <s32file.h>
#include <bamdesca.h>
#include <badesca.h>
#include <sipprofiletypeinfo.h>

// FORWARD DECLARATIONS

class CSIPProfileStorageIndex;
class CSIPConcreteProfile;
class CRepository;

// CLASS DECLARATION

/**
*  The class that provides permanent storage for SIP profiles,
*  it does not have any cache.
*  The class enables adding, updating and deleting of a single profile.
*  The class enables retrieval of single profile and and array
*  containing all profiles.
*  The class also stores id of the default profile.
*  The class is responsible of generating unique profile identifiers.
*  This is abstract base class.
*  @lib 
*/
class CSIPProfileStorageBase : public CBase
	{
    public: // Enumerations

        /** Variable types */
        enum TVariableType
            {
            EInteger,
            EBoolean,
            EDescriptor,
            EDescArray,
            EIntArray,
            EBoolArray,
            EDescArrayArray
            };	
	
	public:

	    /**
        * Destructor.
        */
		virtual ~CSIPProfileStorageBase();

	    /**
        * Adds profile into permanent storage 
		* @param aProfile SIP profile
        */
		virtual void AddSIPProfileL(CSIPConcreteProfile& aProfile) = 0;

	    /**
        * Updates profile into permanent storage 
		* @param aProfile SIP profile
        */
		virtual void UpdateSIPProfileL(CSIPConcreteProfile& aProfile) = 0;

	    /**
        * Updates default profile into permanent storage 
		* @param aStorageId identifier of the profile
        */
		virtual void UpdateDefaultSIPProfileL(TUint32 aStorageId) = 0;

	    /**
        * Deletes profile from permanent storage 
		* @param aStorageId SIP profile identifier
        */
		virtual void DeleteSIPProfileL(TUint32 aStorageId) = 0;

	    /**
        * Gets all profiles 
        * @param FromStore specifies whether to get profiles from fileStore or Cenrep
		* @return SIP profiles, ownership transferred
        */
		RPointerArray<CSIPConcreteProfile>* SIPProfilesL(TBool FromStore=EFalse);

	    /**
        * Gets the identifier of the default profile 
		* @return SIP default profile identifier
        */
		TUint32 DefaultProfile();

	    /**
        * Deletes all profiles picked from cenrep
        */
		static void DeleteAllProfilesL();
		
		/**
        * Deletes all profiles fetched from sipprofiles.dat file
        */
		void DeleteAllStreamProfilesL();
		
		/**
		* Get the ProfileStorageIndex Object
		*/
		CSIPProfileStorageIndex* GetProfileStorageIndexObject();
		

	protected: // Constructors
	
	    /**
        * Default constructor
        */
		CSIPProfileStorageBase(RFs& aFs);

	    /**
        * Symbian 2nd phase constructor
        */
		void BaseConstructL();

    protected: // New functions

	    /**
        * Compacts the permanent storage.
		* This is needed when storage is modified either by
		* update or delete operations.
        */
		void Compact();

	    /**
        * Provides roll back for permanent storage in case of failure
        * Ownership of aStore is not transferred
		* @param aStore permanent store
        */
		static void CrashRevert(TAny* aStore);

	protected: // Data

		CSIPProfileStorageIndex*	iProfileIndex;		
		HBufC8*						iEmpty;
        // Central Repository for SIP profiles
        CRepository*                iSipCenRep;
        RFs&						iFs;
        CPermanentFileStore*		iStore;		

    private: // New functions
 
        static void ResetAndDestroyProfile(TAny* aHeaderArray);
    
	    /**
        * Get all values for a profile from central repository.
        * @param aProfileId The id of wanted profile.
        */		
		CSIPConcreteProfile* SIPProfileL(TUint32 aProfileId);
		
		/**
		* Gets the profile parameters from file store
		*/
		CSIPConcreteProfile* SIPProfileL(TStreamId aStreamId);

        /*
        * Gets the variable type of given key.
        * @param aProfileKey Key from CenRep.
        */
        TVariableType VariableTypeL( TUint32 aProfileKey);
        /*
        * Fills profile's arrays with values read from cenrep.
        * @param aProfile Profile that will get the array.
        * @param aValueArray Array to be populated.
        * @param aIndex Current index of read keys array.
        * @param aKeysArray Array of read keys.
        * @param aVarID Variable id of array.
        */
        void FillValueArrayL( CSIPConcreteProfile& aProfile,
                              CDesC8ArrayFlat* aValueArray,
                              TInt& aIndex,
                              RArray<TUint32>& aKeysArray,
                              TUint32 aVarID );
        
        /*
        * Sets the read value from cenrep to the right variable
        * of the profile.
        * @param aProfileKey Read key from CenRep.
        * @param aProfile Profile that will get the variable.
        * @param aVarType Variable type of the read key.
        */
        void SetVariableL(  TUint32 aProfileKey, 
                            CSIPConcreteProfile& aProfile, 
                            TVariableType aVarType );		
        
        /*
        * Sets the read and filled array from cenrep to the
        * right variable array of the profile.
        * @param aIndex Current index of read keys array. 
        * @param aKeysArray Array of read keys.
        * @param aProfile Profile that will get the array.
        */        
        void SetArrayVariableL( 
                            TInt& aIndex, 
                            RArray<TUint32>& aKeysArray,
                            CSIPConcreteProfile& aProfile );

        /*
        * Sets the read key/value pairs read from cenrep to
        * the right extension variable of the profile.
        * @param aServer Server type
        * @param aProfile Profile that will get the array.
        * @param aIndex Current index of read keys array. 
        * @param aKeysArray Array of read keys.
        */
        void SetServerExtParametersL(  
                            TUint32 aServer, 
                            CSIPConcreteProfile& aProfile,
                            TInt& aIndex, 
                            RArray<TUint32>& aKeysArray );
                            
        /*
        * Initializes to read the profiles form sipprofiles.dat file
        */
        void InitializeFileStoreL();
    private:
       TSIPProfileTypeInfo iProfileInfo; 
    private: // For testing purposes
#ifdef CPPUNIT_TEST
		friend class CSIPProfileStorageTest;
#endif
	};

#endif
