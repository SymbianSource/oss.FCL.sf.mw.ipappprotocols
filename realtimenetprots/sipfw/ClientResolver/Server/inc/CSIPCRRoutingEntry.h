/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSIPCRRoutingEntry.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/



#ifndef CSIPCRROUTINGENTRY_H

/**
* @internalComponent
*/
#define CSIPCRROUTINGENTRY_H

// INCLUDES
#include <e32base.h>
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS

// CLASS DEFINITIONS
/**
* Class implements a routing table entry for SIP client UIDs.
*
*  @lib siprsvsrv.exe
*/
class CSIPCRRoutingEntry : public CBase
    {
    public: // Constructors and destructor

        /**
        * Static constructor.
        *
        * @param aUid the UID of the entry
        *
        * @return An initialized instance of this class.
        */
        static CSIPCRRoutingEntry* NewL( const TUid& aUid );

        /// Destructor.
        ~CSIPCRRoutingEntry();

    public: // New functions

        /**
        * Gets the reference count to this entry.
        * This means the count of times this entry has been 
        * added/associated to the routing table.
        * @return the reference count
        */
        TInt& ReferenceCount();

        /**
        * Gets the UID of this entry.
        * 
        * @return the UID of this entry
        */
        const TUid& UID() const;

        /**
        * Adds a client UID that will be served by this entry.
		*
        * @param aUid the UID to be added
		*/
        void AddL( const TUid& aUid );

        /**
        * Add an allocated uid entry object under this entry.
		*
        * @param aUid the entry object to be added
		*/
        void AddL( const CSIPCRRoutingEntry* aEntry );        

        /**
        * Removes the client UID from the list of served clients.
        * 
        * @param aUid the UID to be removed
        * @return KErrNone if the UID has been succesfully removed. 
        *         Otherwise a system wide error code.
        */
        TInt Remove( const TUid& aUid );

		/**
        * Compare two CSIPCRRoutingEntry items
		* @return ETrue, if UID id equals
        */
        static TBool Compare(const CSIPCRRoutingEntry& aItem,
				      const CSIPCRRoutingEntry& aItem2);
				      
        /**
        * Retrieves the requested CSIPCRRoutingEntry instance 
        * in the the whole CSIPCRRoutingEntry tree.
        * 
        * @param aUid of the requested CSIPCRRoutingEntry
        * @return requested CSIPCRRoutingEntry instance. Otherwise NULL.
        */
        CSIPCRRoutingEntry* FindEntryInTree( const TUid& aUid);
        
        /**
        * Retrieves the parent instance of the requested CSIPCRRoutingEntry.
        * 
        * @param aUid UID of the requested CSIPCRRoutingEntry
        * @return requested parent CSIPCRRoutingEntry instance. Otherwise NULL.
        */
        CSIPCRRoutingEntry* FindParent( const TUid& aUid ); 

        /**
        * Checks whether this routing table entry has the given UID
        * as a served client UID.
        * 
        * @param aUid the UID to be checked
        * @return ETrue if found. Otherwise EFalse.
        */
        TBool HasClient( const TUid& aUid ) const;

        /*
        Set UID (used in search operation)
        */
        void SetUID( const TUid& aUid );

        /*
        * Detach requested UID from array, and return the entry
        */
        CSIPCRRoutingEntry* Detach( const TUid& aUid );

    private: // methods
        /*
        default constructor
        */
        CSIPCRRoutingEntry(const TUid& aUid);
    
		/**
        * Find requested CSIPCRRoutingEntry instance, 
        * Note! This operation temporarily switches the uid
        * of current entry object to be the searched one
        * @param aUid of the requested CSIPCRRoutingEntry
        * @ return requested CSIPCRRoutingEntry instance, otherwise NULL
        */
        CSIPCRRoutingEntry* FindEntry(const TUid& aUid);

		/**
        * Find index of requested CSIPCRRoutingEntry instance, 
        * Note! This operation temporarily switches the uid
        * of current entry object to be the searched one
        * @param aUid of the requested CSIPCRRoutingEntry
        * @return RPointerArray index, otherwise KErrNotFound
        */
        TInt FindEntryIndex(const TUid& aUid);				      

    private: // Data
    
        TInt iReferenceCount;
        TUid iUID;
        RPointerArray<CSIPCRRoutingEntry> iServedClients;        
        
    private: // For testing purposes

	    UNIT_TEST(CSIPCRRoutingEntryTest)          
    };

#endif // CSIPCRROUTINGENTRY_H

