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
* Name          : CSIPCRRoutingTable.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/



#ifndef CSIPCRROUTINGTABLE_H
/**
* @internalComponent
*/
#define CSIPCRROUTINGTABLE_H

// INCLUDES
#include <e32base.h>
#include "CSIPCRRoutingEntry.h"

// FORWARD DECLARATIONS

// CLASS DEFINITIONS
/**
* Class implements a routing table for SIP client UIDs.
* There can be SIP clients serving other SIP clients.
* These associations are stored in this table. 
*
*  @lib siprsvsrv.exe
*/
class CSIPCRRoutingTable : public CBase
    {
    public: // Constructors and destructor

        /**
        * Static constructor.
        *
        * @return An initialized instance of this class.
        */
        static CSIPCRRoutingTable* NewL();

        /// Destructor.
        ~CSIPCRRoutingTable();

    public: // New functions

        /**
        * Adds a top-level client to the routing table.
        *
        * @param aUid the UID to be added
        */
        void AddL( const TUid& aUid );

        /**
        * Associates a UID with an existing routing table entry.
        * This may require moving the associated entry as
        * a child of an existing routing entry, 
        * if the associated entry is already in the routing table.
        *
        * @param aUid the UID of an existing routing table entry
        * @param aAssociatedUid a UID to be associated with the existing entry.
        */
        void AssociateL( const TUid& aUid, const TUid& aAssociatedUid );

        /**
        * Removes a UID from the routing table.
        * If this is a non-leaf UID, all the associated UIDs are also removed.
        * @param aUid the UID to be removed
        * @return KErrNone if the routing table entry has been 
        *         succesfully removed. Otherwise a system wide error code.
        */
        TInt Remove( const TUid& aUid );

        /**
        * Finds the next-hop entry associated with the given UID.
        *
        * @param aUid the UID of an existing leaf level routing table entry
        * @param aParentUid the UID of a parent or grand-parent 
        *        of the leaf-level UID
        * @param aNextHopUid on return contains the next-hop UID
        * @return ETrue if a match was found. Otherwise EFalse.
        */
        TBool FindMatch( const TUid& aClientUid, 
                         const TUid& aParentUid, 
                         TUid& aNextHopUid );
                         
        /**
        * Checks whether the client UID is present in the routing table. 
        * @param aUid the UID of the client
        * @return ETrue if a match was found. Otherwise EFalse.
        */
        TBool HasUid( const TUid& aClientUid );                         

    private: // Data
    
        RPointerArray<CSIPCRRoutingEntry> iTable;
        CSIPCRRoutingEntry*               iFindEntry;            

    private: // methods
    
        // default constructor
        CSIPCRRoutingTable();
        
        // Find requested entry index
        TInt FindEntryIndex(const TUid& aUid);

        /**
        * Finds the top-level entry associated with the given UID.
        *
        * @param aUid the UID of an existing routing table entry
        * @param aTopLevelUid on return contains the top-level UID
        * @return ETrue if a match was found. Otherwise EFalse.
        */
        TBool FindToplevelMatch( const TUid& aUid, TUid& aTopLevelUid );
        
        // construct the CSIPCRRoutingTable object
        void ConstructL();
        
        
    private: // For testing purposes

	    UNIT_TEST(CSIPCRRoutingTableTest)
    };

#endif // CSIPCRROUTINGTABLE_H

