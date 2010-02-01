// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : sipprofilestorageindex
// Part of     : SIP Profile Server
// implementation
// Version     : 1.0
//



// INCLUDE FILES
#include "SIPProfileStorageIndex.h"
#include <centralrepository.h>
#include "SIPProfileStoragePrivateCRKeys.h"
#include "SipProfileLog.h"
#include "SipProfileServerCore.h"
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileStorageIndex::NewL
// -----------------------------------------------------------------------------
//
CSIPProfileStorageIndex*
CSIPProfileStorageIndex::NewL(CRepository& aStore)
    {
    CSIPProfileStorageIndex* self =
    	new (ELeave) CSIPProfileStorageIndex(aStore);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPProfileStorageIndex::CSIPProfileStorageIndex
// -----------------------------------------------------------------------------
//
CSIPProfileStorageIndex::CSIPProfileStorageIndex(CRepository& aStore)
    :iCenRepHandle(aStore),iDefault(0)    
	{
	}

// -----------------------------------------------------------------------------
// CSIPProfileStorageIndex::~CSIPProfileStorageIndex
// -----------------------------------------------------------------------------
//
CSIPProfileStorageIndex::~CSIPProfileStorageIndex()
	{
	iProfileIdArray.Close();
	iProfileStreamIdArray.Close();
	iCore = NULL;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStorageIndex::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorageIndex::ConstructL()
	{
	LoadProfileIDsFromCenrepL();
	}

// -----------------------------------------------------------------------------
// CSIPProfileStorageIndex::LoadProfileIDsFromCenrepL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorageIndex::LoadProfileIDsFromCenrepL()
	{
	PROFILE_DEBUG1("CSIPProfileStorageIndex::LoadProfileIDsFromCenrepL : Start")
    RArray<TUint32> foundIDs;
    TUint32 err = iCenRepHandle.FindL(KAllKeys, KNoMask, foundIDs); 
    CleanupClosePushL(foundIDs);
    PROFILE_DEBUG3("CSIPProfileStorageIndex::LoadProfileIDsFromCenrepL : Total Keys in CenRep : ",foundIDs.Count())
    if (err == KErrNone)
    	{
    	TInt arrCount = foundIDs.Count();

    	for (TInt i=0; i<arrCount; i++)
        	{
        	TUint32 cenRepKey = foundIDs[i];
        	TUint32 profileID = cenRepKey >> KProfileIdShift; 
               
        	if (cenRepKey != KSIPDefaultProfileID &&
        				iProfileIdArray.Find(profileID) == KErrNotFound)
				{
            	User::LeaveIfError(iProfileIdArray.Append(profileID));
            	}
        	}           
    	User::LeaveIfError(
    	iCenRepHandle.Get(KSIPDefaultProfileID, (TInt&)iDefault));    	
    	}
    CleanupStack::PopAndDestroy();
	PROFILE_DEBUG1("CSIPProfileStorageIndex::LoadProfileIDsFromCenrepL : End")
	}  

// -----------------------------------------------------------------------------
// CSIPProfileStorageIndex::AddProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorageIndex::AddProfileL(TUint32 aId, TBool aDefault)
	{
	PROFILE_DEBUG3("CSIPProfileStorageIndex::AddProfileL, Id : ",aId)	
	__ASSERT_ALWAYS(!ProfileExists(aId), User::Leave(KErrAlreadyExists));

	User::LeaveIfError(iProfileIdArray.Append(aId));
	
	if (aDefault || iDefault == 0)
		{
		User::LeaveIfError(iCenRepHandle.Set(KSIPDefaultProfileID, (TInt)aId));
		iDefault = aId;
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileStorageIndex::RemoveProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorageIndex::RemoveProfileL(TUint32 aId)
	{
	PROFILE_DEBUG3("CSIPProfileStorageIndex::RemoveProfileL, Id : ",aId)	
	TInt index = iProfileIdArray.Find(aId);
	__ASSERT_ALWAYS(index != KErrNotFound, User::Leave(KErrNotFound));

	iProfileIdArray.Remove(index);
	iProfileIdArray.Compress();

	if (iDefault == aId)
		{
		if (iProfileIdArray.Count() == 0)
			{
			iDefault = 0;
			}
		else
			{
			iDefault = iProfileIdArray[0];			
			}
		// Set Default Profile ID in to cenrep
		User::LeaveIfError(iCenRepHandle.Set(KSIPDefaultProfileID, (TInt)iDefault));
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileStorageIndex::Profiles
// -----------------------------------------------------------------------------
//
const RArray<TUint32>& CSIPProfileStorageIndex::Profiles() const
	{
	return iProfileIdArray;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStorageIndex::ProfileExists
// -----------------------------------------------------------------------------
//
TBool CSIPProfileStorageIndex::ProfileExists(TUint32 aId) const
	{
	return iProfileIdArray.Find(aId) != KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStorageIndex::SetDefaultProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorageIndex::SetDefaultProfileL(TUint32 aProfileId)
	{
	__ASSERT_ALWAYS(ProfileExists(aProfileId), User::Leave(KErrNotFound));
    User::LeaveIfError(iCenRepHandle.Set(KSIPDefaultProfileID, (TInt)aProfileId));
	iDefault = aProfileId;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStorageIndex::DefaultProfile
// -----------------------------------------------------------------------------
//
TUint32 CSIPProfileStorageIndex::DefaultProfile() const
	{
	return iDefault;
	}


// -----------------------------------------------------------------------------
// CSIPProfileStorageIndex::GetNextProfileIdL
// -----------------------------------------------------------------------------
//
TUint32 CSIPProfileStorageIndex::GetNextProfileIdL()
	{
	TUint32 profileID = iProfileIdArray.Count()+1;
	
	iProfileIdArray.SortUnsigned();
	for (TUint32 i = 0; i < iProfileIdArray.Count(); i++)
	    {
	    TRAPD(err, iCore->ProfileCacheItemL(i+1));
	    if (i+1 != iProfileIdArray[i] && KErrNotFound == err)
	        {
	        profileID = i+1;
	        break;
	        }
	    }
	    
    if (profileID > KMaxProfileId) 
        {
        User::Leave(KErrTooBig);
        }           
	return profileID;	
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileStorageIndex::DeleteAllProfilesIds
// -----------------------------------------------------------------------------
//	
void CSIPProfileStorageIndex::DeleteAllProfilesIds()
{
PROFILE_DEBUG1("CSIPProfileStorageIndex::DeleteAllProfilesIds")	
	for(TUint32 i=0; i<iProfileIdArray.Count();i++)
		{
		if (iDefault == iProfileIdArray[i])
			{
			iDefault = 0;
			break;
			}
		}
	
	iProfileIdArray.Reset();	
}

// -----------------------------------------------------------------------------
// CSIPProfileStorageIndex::DeleteAllStreamIds
// -----------------------------------------------------------------------------
//	
void CSIPProfileStorageIndex::DeleteAllStreamIds()
{
PROFILE_DEBUG1("CSIPProfileStorageIndex::DeleteAllStreamIds")	
	for(TInt i=0; i<iProfileStreamIdArray.Count();i++)
		{
		if (iDefault == iProfileStreamIdArray[i].Value())
			{
			iDefault = 0;
			break;
			}
		}
	iProfileStreamIdArray.Reset();	
}

// -----------------------------------------------------------------------------
// CSIPProfileStorageIndex::SetPermanentStoreObjectL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorageIndex::SetPermanentStoreObjectL(CPermanentFileStore const* aStore)
{
	PROFILE_DEBUG1("CSIPProfileStorageIndex::SetPermanentStoreObjectL")	
	iStore = aStore; 
	iRoot =  aStore->Root();	
	// Get the profiles stored in sipprofiles.dat file
	RestoreL();		
}

// -----------------------------------------------------------------------------
// CSIPProfileStorageIndex::RestoreL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorageIndex::RestoreL()
	{
	PROFILE_DEBUG1("CSIPProfileStorageIndex::RestoreL")	
	RStoreReadStream instream;
	instream.OpenLC(*iStore,iRoot);
	InternalizeL(instream);
	CleanupStack::PopAndDestroy();	
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileStorageIndex::InternalizeL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorageIndex::InternalizeL(RReadStream& aStream)
	{
	// Check on the default profile IDs
	iStreamDefault = aStream.ReadUint32L();
	TUint count = aStream.ReadUint32L();
	for (TUint i = 0;i < count; i++) 
		{
		TStreamId id; 
		id.InternalizeL(aStream);
		User::LeaveIfError(iProfileStreamIdArray.Append(id));
		}
	} 
	
// -----------------------------------------------------------------------------
// CSIPProfileStorageIndex::Streams
// -----------------------------------------------------------------------------
//
const RArray<TStreamId>& CSIPProfileStorageIndex::Streams() const
	{
	return iProfileStreamIdArray;
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileStorageIndex::ProfileExists
// -----------------------------------------------------------------------------
//
TBool CSIPProfileStorageIndex::ProfileExists(TStreamId aId) const
	{
	return iProfileStreamIdArray.Find(aId) != KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStorageIndex::SetProfileServerCoreObject
// -----------------------------------------------------------------------------
//
void CSIPProfileStorageIndex::SetProfileServerCoreObject (CSIPProfileServerCore const* aCore)
    {
    iCore = aCore;
    }
