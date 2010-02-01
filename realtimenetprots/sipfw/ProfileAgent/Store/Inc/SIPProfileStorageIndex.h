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
* Name        : sipprofilestorageindex.h
* Part of     : SIP Profile Server
* Interface   : private
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILESTORAGEINDEX_H
#define CSIPPROFILESTORAGEINDEX_H

//  INCLUDES

#include <s32file.h>
#include <bamdesca.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class CRepository;

// CLASS DECLARATION

/**
*  The class that provides index for permanent storage of SIP profiles.
*  The class fetches profile id's from central repository.
*
*  @lib N/A
*/
class CSIPProfileServerCore;
class CSIPProfileStorageIndex: public CBase
	{
	public:

	    /**
        * Two-phased constructor.
		* @param aStore central repository
		* @return new instance
        */
		static CSIPProfileStorageIndex* NewL(CRepository& aStore);

	    /**
        * Destructor.
        */
		~CSIPProfileStorageIndex();

	    /**
        * Adds new profile identifier into index
		* @param aId profile identifier
		* @param aDefault ETrue, if id also default profile
        */
		void AddProfileL(TUint32 aId, TBool aDefault = EFalse);

	    /**
        * Removes profile identifier from index
		* @param aId profile identifier
        */
		void RemoveProfileL(TUint32 aId);

	    /**
        * Checks if profile exists in storage
		* @param aId stream identifier
		* @return ETrue if profile exists
        */
		TBool ProfileExists(TUint32 aId) const;

	    /**
        * Sets profile as default profile
		* @param aId SIP profile's stream id
        */
		void SetDefaultProfileL(TUint32 aProfileId);

	    /**
        * Gets default profile's stream identifier
		* @return default profile identifier, 0 if no profiles in storage
        */
		TUint32 DefaultProfile() const;

	    /**
        * Gets all stream identifiers (=profile identifiers)
		* @return array of stored stream identifiers
        */
		const RArray<TUint32>& Profiles() const;
				
		/**
        * Gets next free profile id.
		* @return the next free profile identifier
        */
		TUint32 GetNextProfileIdL();
		
		/**
        * Reset the profile Id Array i.e iProfileIdArray
        */
		void DeleteAllProfilesIds();
		
		/**
        * Reset the stream Id Array i.e iProfileStreamIdArray
        */
		void DeleteAllStreamIds();
		
		/**
		* Gets the permanent store object 
		*/
		void SetPermanentStoreObjectL(CPermanentFileStore const* aStore);
		
		/**
        * Retrieve profile indices from permanent storage
        */
		void RestoreL();
		
		/**
        * Gets all stream identifiers (=profile identifiers)
		* @return array of stored stream identifiers
        */
		const RArray<TStreamId>& Streams() const;
		
		/**
        * Checks if profile exists in storage
		* @param aId stream identifier
		* @return ETrue if profile exists
        */
		TBool ProfileExists(TStreamId aId) const;		
		
		/**
		* Set the server-core object
		*/
		void SetProfileServerCoreObject (CSIPProfileServerCore const* aCore);
		
		
	private:

	    /**
        * Default constructor.
        * @param aStore central repository
        */
		CSIPProfileStorageIndex(CRepository& aStore);

	    /**
        * Symbian 2nd phase constructor.
        */
		void ConstructL();
		

	    /**
        * Fetches and have a copy of profileIds form cenrep  
        */
		void LoadProfileIDsFromCenrepL();	
		
		/**
        * deserialize index from permanent storage
		* @param aStream stream to read index from
        */
		void InternalizeL(RReadStream& aStream);


	private: // Data

		RArray<TUint32> iProfileIdArray;
		CRepository&    iCenRepHandle;
		TUint32         iDefault;
		// To store profile stream IDs from sipprofils.dat file
		RArray<TStreamId> iProfileStreamIdArray;
		TStreamId iRoot;
		CStreamStore const* iStore; // not owned
		TStreamId iStreamDefault;
		CSIPProfileServerCore const* iCore; //not owned

    private: // For testing purposes
#ifdef CPPUNIT_TEST
		friend class CSIPProfileStorageIndexTest;
#endif
	};

#endif
