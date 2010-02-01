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
* Name        : sipprofilestorage.h
* Part of     : SIP Profile Server
* Interface   : private
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILESTORAGE_H
#define CSIPPROFILESTORAGE_H

//  INCLUDES
#include "SIPProfileStorageBase.h"

// CLASS DECLARATION
/**
*  The class that provides permanent storage for SIP profiles,
*  it does not have any cache.
*  The class enables adding, updating and deleting of a single profile.
*  The class enables retrieval of single profile and and array
*  containing all profiles.
*  The class also stores id of the default profile.
*  The class is responsible of generating unique profile identifiers.
*  The unique profile identifier equals permanent storage stream identifiers
*  @lib 
*/
class CSIPProfileStorage : public CSIPProfileStorageBase
	{
	public: // Constructors and destructor

	    /**
        * Two-phased constructor.
		* @return new instance
        */
		static CSIPProfileStorage* NewL(RFs& aFs);

	    /**
        * Destructor.
        */
		~CSIPProfileStorage();

    public: // From CSIPProfileStorageBase

	    /**
        * Adds profile into permanent storage 
		* @param aProfile SIP profile
        */
		void AddSIPProfileL(CSIPConcreteProfile& aProfile);

	    /**
        * Updates profile into permanent storage 
		* @param aProfile SIP profile
        */
		void UpdateSIPProfileL(CSIPConcreteProfile& aProfile);

	    /**
        * Updates default profile into permanent storage 
		* @param aStorageId identifier of the profile
        */
		void UpdateDefaultSIPProfileL(TUint32 aStorageId);

	    /**
        * Deletes profile from permanent storage 
		* @param aStorageId SIP profile identifier
        */
		void DeleteSIPProfileL(TUint32 aStorageId);

	private: // Constructors

		CSIPProfileStorage(RFs& aFs);

		void ConstructL();
		/**
        * Stores a CenRep key/value pair
		* @param aKey Key to be stored
		* @param aVal Value to be stored
        */
		template<typename T> void StoreKeyL(TUint32 aKey, const T& aVal);
		
		/**
        * Stores profile into permament storage(CenRep) 
		* @param aProfile SIP profile
        */
		void StoreProfileL(CSIPConcreteProfile& aProfile);
		
		/**
        * Store profile's arrays to permament storage(CenRep)
		* @param aProfile SIP profile
        */
		void StoreArraysL(CSIPConcreteProfile& aProfile);
		
		/**
        * Deletes array from permament storage(CenRep)
		* @param aStorageId identifier of the profile
		* @param aKey CenRep key of the array 
        */
		void DeleteArrayL(TUint32 aStorageId, TInt aKey);

	private: // For testing purposes
#ifdef CPPUNIT_TEST
		friend class CSIPProfileStorageTest;
#endif
	};

#endif // CSIPPROFILESTORAGE_H
