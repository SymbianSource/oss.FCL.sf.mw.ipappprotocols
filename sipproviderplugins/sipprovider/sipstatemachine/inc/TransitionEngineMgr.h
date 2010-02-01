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
// CTransitionEngineMgr definition file
// 
//

/**
 @file
 @internalComponent
*/

#ifndef TRANSITIONENGINEMGR_H
#define TRANSITIONENGINEMGR_H

#include "transitionengine.h"

class CSIPTransitionEngineBundle;
class CTransitionEngineMgr : public CBase
/**The class is responsible for managing CSIPTransitionEngine. This is
class is the only entity that can supply the objects.

@internalComponent
@released since v9.2
 */
	{  
public:
	IMPORT_C CSIPTransitionEngine* FindOrCreateL(TUid aAppUid, TUint32& aProfileId);
	IMPORT_C void Detach(CSIPTransitionEngine* aTE);
	IMPORT_C TUint32 DefaultProfileId();
	
private:
	RPointerArray<CSIPTransitionEngineBundle> iTEBundles;
	};
	
class CSIPTransitionEngineBundle : public CBase, public MSIPObserver,
								   public MSIPProfileRegistryObserver
/**This class is a container for CSIPTransitionEngine instances.
One instance of this class stores CSIPTransitionEngine instances
pertaining to a single CSIP (and hence to a single application).

@internalComponent
@released since v9.2
 */
	{  
public:
	static CSIPTransitionEngineBundle* NewL(TUid aAppUid);
	~CSIPTransitionEngineBundle();
	
	CSIPTransitionEngine* FindOrCreateL(TUint32& aProfileId);
	TInt Detach(CSIPTransitionEngine* aTE);
	TUint32 DefaultProfileId();
	// From MSIPObserver
	void IncomingRequest( TUint32 aIapId, CSIPServerTransaction* aTransaction );
	void TimedOut( CSIPServerTransaction& aSIPServerTransaction );
	//From MSIPProfileRegistryObserver
	void ProfileRegistryEventOccurred(TUint32 aProfileId, TEvent aEvent);
	void ProfileRegistryErrorOccurred(TUint32 aProfileId, TInt aError);
	TUid GetUID();
	
private:
	void ConstructL();
	CSIPTransitionEngineBundle(TUid aAppUid);
		 
	RPointerArray<CSIPTransitionEngine> iTEs;
	CSIP* iSip;
	CSIPProfileRegistry* iProfileRegistry;
	TUid iAppUid;
	__FLOG_DECLARATION_MEMBER;	
	};
	
inline CSIPTransitionEngineBundle::CSIPTransitionEngineBundle(TUid aAppUid)
:iAppUid(aAppUid)
    {
	__FLOG_OPEN(KSIPStateMachineLogTag1, KSIPStateMachineLogTag2);	
	__FLOG_1(_L("CSIPTransitionEngineBundle %08x:\tCSIPTransitionEngineBundle"), this);
    }


inline TUid CSIPTransitionEngineBundle::GetUID()
	{
	return iAppUid;
	}

#endif
