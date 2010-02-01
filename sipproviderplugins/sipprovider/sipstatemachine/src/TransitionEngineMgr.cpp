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
// CTransitionEngineMgr implementation file.
// 
//

/**
 @file
 @internalComponent
*/

#include "TransitionEngineMgr.h"


EXPORT_C CSIPTransitionEngine* CTransitionEngineMgr::FindOrCreateL(TUid aAppUid, TUint32 &aProfileId)
/**	Finds an instance of CSIPTransitionEngine that pertains to the given application UID
and IAP (Extracted from Profile) or creates a new one if none found. IMPORTANT: When the returned instance
is no longer needed, the caller must call Detach(<the_instance_pointer>) on 'this'.

@param aAppUid the application UID
@param aProfileId 
@return a pointer to the CSIPTransitionEngine instance or NULL if allocation problems
@exception leaves with KErrNoMemory if memory allocation fails
*/	
	{
	CSIPTransitionEngineBundle* theBundle = NULL;
	for (int i = 0; i < iTEBundles.Count(); i++ )
		{
		if (iTEBundles[i]->GetUID() == aAppUid)
			{
			theBundle = iTEBundles[i];
			}
		}
			
	if (NULL == theBundle)
		{
		theBundle = CSIPTransitionEngineBundle::NewL(aAppUid);
		CleanupStack::PushL(theBundle);
		iTEBundles.AppendL(theBundle);
		CleanupStack::Pop();
		}
	return theBundle->FindOrCreateL(aProfileId);
	}
	

EXPORT_C void CTransitionEngineMgr::Detach(CSIPTransitionEngine* aTE)
//EXPORT_C void CTransitionEngineMgr::Detach(CSIPTransitionEngine* aTE, MSIPRegistrationClient* aRegClient)
/**	This method must be called when the instance of CSIPTransitionEngine previously
obtained with CTransitionEngineMgr::FindOrCreateL is no longer necessery. The
necessary cleanup will be performed here if there are no other clients using
the instance.

@param aTE the pointer the caller whishes to detach from.
*/	
	{
	TInt detached = -1;
	for (int i = 0; i < iTEBundles.Count(); i++ )
		{
		detached = iTEBundles[i]->Detach(aTE);
		if (detached == 0)
			{
			//found and no more users to the found TE.
			CSIPTransitionEngineBundle * bundle = iTEBundles[i];
			iTEBundles.Remove(i);
			delete bundle;
			break;			
			}
		else if (detached > 0)
	    	{
			//found but still users to the found TE.    		
	    	break;
	    	}
		}
		
    //make sure we actually found and detached from a TE.
	ASSERT(detached>=0);
	}
	
EXPORT_C TUint32 CTransitionEngineMgr::DefaultProfileId()
	{
	//any bundle can find the default profile Id
	//so send requiest to the first one
	return iTEBundles[0]->DefaultProfileId();
	}

CSIPTransitionEngineBundle* CSIPTransitionEngineBundle::NewL(TUid aAppUid)
	{
	CSIPTransitionEngineBundle* theBundle = new (ELeave) CSIPTransitionEngineBundle(aAppUid);
	CleanupStack::PushL(theBundle);
	theBundle->ConstructL();
	CleanupStack::Pop();
	return theBundle;
	}


CSIPTransitionEngine* CSIPTransitionEngineBundle::FindOrCreateL(TUint32 &aProfileId)
	{
	TUint32 iapId;
	CSIPTransitionEngine* theTE = NULL;
	CSIPProfile* profile = NULL;
	//Retrive Profile using ProfileRegistry
	//and extract Iap from Profile
	
	if (aProfileId == KSIPDefaultProfileId)
		{
		profile = iProfileRegistry->DefaultProfileL();
		}
	else
		{
		profile = iProfileRegistry->ProfileL(aProfileId);	
		}
	
	//the following check added because ProfileL not leaving on error
	if (profile != NULL)
		{
		profile->GetParameter(KSIPAccessPointId, iapId);
		delete profile;
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	
	for (int i = 0; i < iTEs.Count(); i++ )
		{
		if (iTEs[i]->IapId() == iapId)
			{
			theTE = iTEs[i];
			}
		}
		
	if (NULL == theTE)
		{
		theTE = CSIPTransitionEngine::NewL(*iSip, iapId);
		CleanupStack::PushL(theTE);
		iTEs.AppendL(theTE);
		CleanupStack::Pop();
		}
		
	theTE->Attach();
	return theTE;

	}
	
void CSIPTransitionEngineBundle::ConstructL()
	{
	__FLOG_1(_L("CSIPTransitionEngineBundle %08x:\tConstructL, expect 'ConstructL successful', otherwise the method left"), this);	
	__FLOG_1(_L("CSIPTransitionEngineBundle %08x:\tinstantiating CSIP, may leave..."), this);	
	iSip = CSIP::NewL( iAppUid, *this );
	__FLOG_1(_L("CSIPTransitionEngineBundle %08x:\tinstantiating CSIPProfileRegistry, may leave..."), this);
	iProfileRegistry = CSIPProfileRegistry::NewL(*iSip, *this);
	__FLOG_1(_L("CSIPTransitionEngineBundle %08x:\tConstructL successful"), this);
	}

//todo
TInt CSIPTransitionEngineBundle::Detach(CSIPTransitionEngine* aTE)
//TInt CSIPTransitionEngineBundle::Detach(CSIPTransitionEngine* aTE, MSIPRegistrationClient* aRegClient)
/**Called when the user of the previously obtained CSIPTransitionEngine
   no longer needs it.

@param aTE the pointer the caller whishes to detach from.
@return the number of TEs still held here or -1 if the requested TE not found.
If the number of TEs still held here is zero, the caller should destroy 'this'.
*/
	{
	TInt detached = -1;
	for (int i = 0; i < iTEs.Count(); i++ )
		{
		if (iTEs[i] == aTE)
			{
			detached = iTEs[i]->Detach();
			if (detached == 0)
				{
				CSIPTransitionEngine * tE = iTEs[i];
				iTEs.Remove(i);
				delete tE;
				}
			break;
			}
		}
	return detached >=0 ? iTEs.Count() : detached;
	}

CSIPTransitionEngineBundle::~CSIPTransitionEngineBundle()
	{
		// for safety try deleting TEs
	for(TInt count = 0; count < iTEs.Count(); count ++)
		{
		CSIPTransitionEngine * tE = iTEs[count];
		iTEs.Remove(count);
		delete tE;
		}
	iTEs.Close();
	
	if(iProfileRegistry != NULL)
		{
		delete iProfileRegistry;
		iProfileRegistry = NULL;
		}
		
	delete iSip;
	iSip = NULL;
	__FLOG_CLOSE;
	}
	 
	 
//From MSIPObserver
void CSIPTransitionEngineBundle::IncomingRequest( TUint32 /*aIapId*/,
							  CSIPServerTransaction* /*aTransaction*/ )
	{
	}

//From MSIPObserver
void CSIPTransitionEngineBundle::TimedOut( CSIPServerTransaction& /*aSIPServerTransaction*/ )
	{
	}

//From MSIPProfileRegistryObserver
void CSIPTransitionEngineBundle::ProfileRegistryEventOccurred(TUint32 /*aProfileId*/, TEvent /*aEvent*/)
	{
	}
void CSIPTransitionEngineBundle::ProfileRegistryErrorOccurred(TUint32 /*aProfileId*/, TInt /*aError*/)
	{
	}

TUint32 CSIPTransitionEngineBundle::DefaultProfileId()
	{
	TUint32 profileId = KSIPInvalidProfileId;
	CSIPProfile* profile = NULL;
	TRAPD(err, profile = iProfileRegistry->DefaultProfileL());
	if (err == KErrNone)
		{
		profile->GetParameter(KSIPProfileId, profileId);
		delete profile;
		}
	return profileId;	
	}
