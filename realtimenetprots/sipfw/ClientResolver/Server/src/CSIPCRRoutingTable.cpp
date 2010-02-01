// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : CSIPCRRoutingTable.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



// INCLUDES
#include <e32base.h>
#include "CSIPCRRoutingTable.h"

// -----------------------------------------------------------------------------
// CSIPCRRoutingTable::CSIPCRRoutingTable
// -----------------------------------------------------------------------------
//
CSIPCRRoutingTable::CSIPCRRoutingTable()
    {
    }

// -----------------------------------------------------------------------------
// CSIPCRRoutingTable::NewL
// -----------------------------------------------------------------------------
//
CSIPCRRoutingTable* CSIPCRRoutingTable::NewL()
    {
    CSIPCRRoutingTable* self = new (ELeave) CSIPCRRoutingTable();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPCRRoutingTable::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPCRRoutingTable::ConstructL()
    {
    //create the entry object for find operations with dummy id 0
    iFindEntry = CSIPCRRoutingEntry::NewL(TUid::Uid(0));        
    }

// -----------------------------------------------------------------------------
// CSIPCRRoutingTable::~CSIPCRRoutingTable
// Destructor
// -----------------------------------------------------------------------------
//
CSIPCRRoutingTable::~CSIPCRRoutingTable()
    {
    iTable.ResetAndDestroy();
    delete iFindEntry;
    }

// -----------------------------------------------------------------------------
// CSIPCRRoutingTable::AddL
// -----------------------------------------------------------------------------
//
void CSIPCRRoutingTable::AddL( const TUid& aUid )
    {    
    // first check that this top level uid does not exist already,
    // if it does then we leave with error code KErrAlreadyExists 
	if (FindEntryIndex(aUid)!=KErrNotFound) 
	    {	    
	    User::Leave (KErrAlreadyExists);
	    }
	    
	TUid topLevelUid;
    if (FindToplevelMatch(aUid,topLevelUid))
        {
        // This entry is already in the tree, increase reference count
	    TInt topLevelIndex = FindEntryIndex(topLevelUid);
        CSIPCRRoutingEntry* topLevelEntry = iTable[topLevelIndex];        
        CSIPCRRoutingEntry* entry = topLevelEntry->FindEntryInTree(aUid);
        if (entry)
            {
            entry->ReferenceCount()++;
            }
        }
    else
        {
        // Add a new entry in the top level
        CSIPCRRoutingEntry* entry = CSIPCRRoutingEntry::NewL(aUid);
        CleanupStack::PushL(entry);
        iTable.AppendL(entry);
        CleanupStack::Pop(entry);
        }
    }

// -----------------------------------------------------------------------------
// CSIPCRRoutingTable::FindEntryIndex
// -----------------------------------------------------------------------------
//
TInt CSIPCRRoutingTable::FindEntryIndex(const TUid& aUid)
	{
	TIdentityRelation<CSIPCRRoutingEntry> compareId(CSIPCRRoutingEntry::Compare);
    iFindEntry->SetUID(aUid);
	return (iTable.Find(iFindEntry, compareId));
	}

// -----------------------------------------------------------------------------
// CSIPCRRoutingTable::AssociateL
// -----------------------------------------------------------------------------
//
void CSIPCRRoutingTable::AssociateL( const TUid& aUid, const TUid& aAssociatedUid )
    {
    TUid associatedUidTop;
    TUid parentUidTop;
    TInt index;
    CSIPCRRoutingEntry* topLevelEntry = NULL;
    CSIPCRRoutingEntry* associatedEntryObject = NULL;
    CSIPCRRoutingEntry* uidEntry = NULL;
    TBool alreadyExists = EFalse;
    
    // first check that the parent uid exists..
    // if it does not then we leave with error code KErrNotFound 
    if (!FindToplevelMatch(aUid, parentUidTop))
        {        
        User::Leave(KErrNotFound);
        }
    
    // first check if the associated uid exists
    // if it does then we may have to move it under the right entry
    alreadyExists = FindToplevelMatch(aAssociatedUid, associatedUidTop);        
    if (alreadyExists)    
        {
        CSIPCRRoutingEntry* currentParent = NULL; 
        
        // search the top level entry 
	    index = FindEntryIndex(associatedUidTop);	
        topLevelEntry = iTable[index];
        // get associated uid's current parent entry..

        currentParent = topLevelEntry->FindParent(aAssociatedUid);
        
        // if the current parent entry is the same 
        // as the one given, then dont do anything..        
        if (currentParent->UID() == aUid)
            {
            return; 
            }
        
        // we need to move the existing associated entry
        // to the new location, first remove the old
        associatedEntryObject = currentParent->Detach(aAssociatedUid);
        CleanupStack::PushL(associatedEntryObject);                
        }
        
    // search the parent top level entry 
	index = FindEntryIndex(parentUidTop);	
    topLevelEntry = iTable[index];        
                    
    //now get the uid entry if it is not at the top level
    if (topLevelEntry->UID() == aUid)
        {        
        uidEntry = topLevelEntry;
        }
    else
        {            
        uidEntry = topLevelEntry->FindEntryInTree(aUid);
        }
                    
    //Finally, add the associated entry
    if (associatedEntryObject)
        {
        // moving the already allocated entry object
        // to a new location
        uidEntry->AddL(associatedEntryObject);
        CleanupStack::Pop(associatedEntryObject);                    
        }
    else
        {                
        // first time inserting this uid..
        // no object created yet            
        uidEntry->AddL(aAssociatedUid);
        }
    }

// -----------------------------------------------------------------------------
// CSIPCRRoutingTable::Remove
// -----------------------------------------------------------------------------
//
TInt CSIPCRRoutingTable::Remove( const TUid& aUid )
    {
    TUid topLevelUid(TUid::Null());
    if (!FindToplevelMatch(aUid, topLevelUid))
        {
        return KErrNotFound;
        }

	TInt topLevelIndex = FindEntryIndex(topLevelUid);
    CSIPCRRoutingEntry* topLevelEntry = iTable[topLevelIndex];    
    
    if (topLevelEntry->UID() == aUid)
        {
        iTable.Remove(topLevelIndex);
        delete topLevelEntry;
        return KErrNone;
        }
        
    CSIPCRRoutingEntry* entry = topLevelEntry->FindEntryInTree(aUid);
    if (--entry->ReferenceCount() == 0)
        {
        CSIPCRRoutingEntry* parent = topLevelEntry->FindParent(aUid);
        return (parent->Remove(aUid));
        }
       
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSIPCRRoutingTable::FindMatch
// -----------------------------------------------------------------------------
//
TBool CSIPCRRoutingTable::FindMatch( const TUid& aClientUid, 
                         const TUid& aParentUid, 
                         TUid& aNextHopUid )
    {
    TUid clientUidTop;       
    TBool match = EFalse;
    TBool fContinue = ETrue;            
    
    // first check that the leaf uid exists..
    // if it does not then return false 
    if (!FindToplevelMatch(aClientUid, clientUidTop))
        {        
        return EFalse;
        }
        
    // search the top level entry 
	TInt index = FindEntryIndex(clientUidTop);	
    CSIPCRRoutingEntry* topLevelEntry = iTable[index];
    
    CSIPCRRoutingEntry*uidEntry = NULL;
    TUid uid = aClientUid;

    while (fContinue)
        {                        
        uidEntry = topLevelEntry->FindParent(uid);        
        if (!uidEntry)
            {
            // no match
            return EFalse;
            }             
                       
        if (uidEntry->UID() == aParentUid)
            {
            // match..
            match = ETrue;
            aNextHopUid = uid;
            fContinue = EFalse;
            }                                    
        else if (uidEntry->UID() == clientUidTop)
            {
            // we are at the top level.. 
            // no match
            match = EFalse;
            fContinue = EFalse;
            }
        else
            {            
            // no match yet..
            // continue climbing up
            uid = uidEntry->UID();
            }
        }  

    return match;
    }
    
// -----------------------------------------------------------------------------
// CSIPCRRoutingTable::HasUid
// -----------------------------------------------------------------------------
//    
TBool CSIPCRRoutingTable::HasUid( const TUid& aClientUid )
    {
    TBool found = EFalse;
    TInt count = iTable.Count();
    
    for ( TInt i=0; i < count && !found; i++ )
        {
        CSIPCRRoutingEntry* entry = iTable[i];
        if ( entry->UID() == aClientUid ||
             entry->HasClient( aClientUid ) )
            {
            found = ETrue;
            }
        }
    
    return found;
    }                       

// -----------------------------------------------------------------------------
// CSIPCRRoutingTable::FindToplevelMatch
// -----------------------------------------------------------------------------
//
TBool CSIPCRRoutingTable::FindToplevelMatch( const TUid& aUid, TUid& aTopLevelUid )
    {                 
    TInt count = iTable.Count();
    
    if (count > 0)
        {
        CSIPCRRoutingEntry* entry = NULL;
        TInt index;
        for (index = 0; index < count; index++)
            {
            entry = iTable[index];
            
            // if the requested uid is top level uid itself, return it.. 
            if (entry->UID() == aUid)
                {
                aTopLevelUid = entry->UID();
                return ETrue;
                }
            
            // or else check the clients..
            if (entry->HasClient(aUid))
                {
                aTopLevelUid = entry->UID();
                return ETrue;
                }
            }
        }
        
    return EFalse;       
    }
