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
// Name          : CSIPCRRoutingEntry.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



// INCLUDES
#include "CSIPCRRoutingEntry.h"

// ----------------------------------------------------------------------------
// CSIPCRRoutingEntry::NewL
// ----------------------------------------------------------------------------
//
CSIPCRRoutingEntry* CSIPCRRoutingEntry::NewL( const TUid& aUid )
    {
    CSIPCRRoutingEntry* self = new (ELeave) CSIPCRRoutingEntry(aUid);
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPCRRoutingEntry::CSIPCRRoutingEntry
// ----------------------------------------------------------------------------
//
CSIPCRRoutingEntry::CSIPCRRoutingEntry(const TUid& aUid) 
 : iReferenceCount(1),
   iUID(aUid)
    {
    }

// ----------------------------------------------------------------------------
// CSIPCRRoutingEntry::~CSIPCRRoutingEntry
// ----------------------------------------------------------------------------
//
CSIPCRRoutingEntry::~CSIPCRRoutingEntry()
    {
    iServedClients.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// CSIPCRRoutingEntry::ReferenceCount 
// ----------------------------------------------------------------------------
//
TInt& CSIPCRRoutingEntry::ReferenceCount()
    {
    return iReferenceCount;
    }

// ----------------------------------------------------------------------------
// CSIPCRRoutingEntry::UID 
// ----------------------------------------------------------------------------
//
const TUid& CSIPCRRoutingEntry::UID() const
    {
    return iUID;
    }
    
// ----------------------------------------------------------------------------
// CSIPCRRoutingEntry::SetUID
// ----------------------------------------------------------------------------
//
void CSIPCRRoutingEntry::SetUID( const TUid& aUid )
    {
    iUID = aUid;    
    }
    
// ----------------------------------------------------------------------------
// CSIPCRRoutingEntry::AddL
// ----------------------------------------------------------------------------
//
void CSIPCRRoutingEntry::AddL( const TUid& aUid )
    {
	if (FindEntry(aUid)) 
	    {	    
	    User::Leave (KErrAlreadyExists);    
	    }
	    	    
    	
    CSIPCRRoutingEntry* entry = CSIPCRRoutingEntry::NewL(aUid);    
    CleanupStack::PushL(entry);
    
    User::LeaveIfError(iServedClients.Append(entry));
    CleanupStack::Pop(entry);
    }

// ----------------------------------------------------------------------------
// CSIPCRRoutingEntry::AddL
// ----------------------------------------------------------------------------
//
void CSIPCRRoutingEntry::AddL( const CSIPCRRoutingEntry* aEntry )
    {        
	if (FindEntry(aEntry->UID())) 
	    {	    
	    User::Leave (KErrAlreadyExists);    
	    }
	    	        	   
    User::LeaveIfError(iServedClients.Append(aEntry));
    }

// ----------------------------------------------------------------------------
// CSIPCRRoutingEntry::Compare
// ----------------------------------------------------------------------------
//
TBool CSIPCRRoutingEntry::Compare(const CSIPCRRoutingEntry& aItem,
				const CSIPCRRoutingEntry& aItem2) 
	{
	return (aItem.iUID == aItem2.iUID);
	}

// ----------------------------------------------------------------------------
// CSIPCRRoutingEntry::FindEntry
// ----------------------------------------------------------------------------
//
CSIPCRRoutingEntry* CSIPCRRoutingEntry::FindEntry(const TUid& aUid)
	{
		
	TIdentityRelation<CSIPCRRoutingEntry> compareId(
	                                            CSIPCRRoutingEntry::Compare);
		
    // Setting temporarily this entry to be the searched one.
    TUid currentUid = iUID;
    iUID = aUid;
	
	TInt index = iServedClients.Find(this, compareId);
	
    // Setting the original uid back.
    iUID = currentUid;
	
	if (index == KErrNotFound)
	    {	    
	    return NULL;
	    }
	
	return (CSIPCRRoutingEntry*) iServedClients[index]; 
	}

// ----------------------------------------------------------------------------
// CSIPCRRoutingEntry::FindEntryIndex
// ----------------------------------------------------------------------------
//
TInt CSIPCRRoutingEntry::FindEntryIndex(const TUid& aUid)
	{
	TIdentityRelation<CSIPCRRoutingEntry> compareId(
	                                            CSIPCRRoutingEntry::Compare);
		
    // Setting temporarily this entry to be the searched one.
    TUid currentUid = iUID;
    iUID = aUid;
	
	TInt index = iServedClients.Find(this, compareId);
	
    // Setting the original uid back.     
    iUID = currentUid;
	
	return index;
	}

// ----------------------------------------------------------------------------
// CSIPCRRoutingEntry::FindEntryInTree
// ----------------------------------------------------------------------------
//
CSIPCRRoutingEntry* CSIPCRRoutingEntry::FindEntryInTree( const TUid& aUid)
    {
    // if this is the entry, return it..
    if (iUID == aUid)
        {        
        return this;
        }
    
    // search all levels in this routing tree..
    TInt count = iServedClients.Count();
    if (count>0)
        {
        CSIPCRRoutingEntry* entry = NULL;
        TInt index;
        for (index = 0; index < count; index++)
            {
            entry = iServedClients[index];            
            if (entry->UID() == aUid)
                {                
                return entry;                       
                }
            if (entry->HasClient(aUid))
                {                
                return (entry->FindEntryInTree(aUid));
                }
            }
        }
        
    return NULL;    
    }

// ----------------------------------------------------------------------------
// CSIPCRRoutingEntry::FindParent
// ----------------------------------------------------------------------------
//
CSIPCRRoutingEntry* CSIPCRRoutingEntry::FindParent( const TUid& aUid )
    {
    if (UID() == aUid)
        {                
        return NULL;           
        }            
    
    TInt count = iServedClients.Count();
    if (count>0)
        {
        CSIPCRRoutingEntry* entry = NULL;
        TInt index;
        for (index = 0; index < count; index++)
            {
            entry = iServedClients[index];
            if (entry->UID() == aUid)
                {                
                return this;
                }            
            if (entry->HasClient(aUid))
                {                
                return (entry->FindParent(aUid));
                }
            }
        }
        
    return NULL;    
    }

// ----------------------------------------------------------------------------
// CSIPCRRoutingEntry::HasClient
// ----------------------------------------------------------------------------
//
TBool CSIPCRRoutingEntry::HasClient( const TUid& aUid ) const
    {
    TInt count = iServedClients.Count();
    
    if (count>0)
        {
        CSIPCRRoutingEntry* entry = NULL;
        TInt index;
        for (index = 0; index < count; index++)
            {
            entry = iServedClients[index];
            if (entry->UID() == aUid)
                {                
                return ETrue;            
                }
            if (entry->HasClient(aUid))
                {                
                return ETrue;            
                }
            }
        }
        
    return EFalse;    
    }

// ----------------------------------------------------------------------------
// CSIPCRRoutingEntry::Remove
// ----------------------------------------------------------------------------
//
TInt CSIPCRRoutingEntry::Remove( const TUid& aUid )
    {
    // search the correct entry 
	TInt index = FindEntryIndex(aUid);
	
	if (index == KErrNotFound)
	    {	    
	    return KErrNotFound;
	    }
	
    CSIPCRRoutingEntry* entry = iServedClients[index];
    iServedClients.Remove(index);    
    delete entry;
    
	return KErrNone;	
    }

// ----------------------------------------------------------------------------
// CSIPCRRoutingEntry::Detach
// ----------------------------------------------------------------------------
//
CSIPCRRoutingEntry* CSIPCRRoutingEntry::Detach( const TUid& aUid )
    {
    // search the correct entry 
	TInt index = FindEntryIndex(aUid);
	
	if (index == KErrNotFound)
	    {	    
	    return NULL;
	    }
	
    CSIPCRRoutingEntry* entry = iServedClients[index];
    iServedClients.Remove(index);    

	return entry;	
    }
