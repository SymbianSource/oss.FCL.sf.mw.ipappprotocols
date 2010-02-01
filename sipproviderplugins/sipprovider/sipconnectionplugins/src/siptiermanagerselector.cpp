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
// This is part of an ECOM plug-in
// SIP TierManagerSelector class used to Find or Create SIP MCPR
// 
//

/**
 @file
 @internalComponent
*/

#include "siptiermanagerselector.h"
#include "siptiermanagerfactory.h"
#include <comms-infras/ss_log.h>
#include <ss_glob.h>
#include <comms-infras/ss_metaconnprov.h>
#include <commsdattypesv1_1.h> 
#include <es_connpref.h>	
#include <commdbconnpref.h>	
#include <comms-infras/ss_tiermanagerutils.h>
#include <sipconnpref.h>
#include <commsdattypesv1_1_partner.h>


#ifdef __CFLOG_ACTIVE
#define KSipTierMgrTag KESockMetaConnectionTag
_LIT8(KSipTierMgrSubTag, "siptiermanager");
#endif

using namespace ESock;
using namespace CommsDat;

enum TSipSelectorPanic
	{
	EExpectedAccessPointAwareSystem = 0,     //
	EExpectedDifferentSelectionPreferences = 1,
	ESipMcprNotFound = 2,
    };


//This fn performs a pre-selection decision to match a proper selector.
MProviderSelector* CSipMetaCprSelector::NewL(const Meta::SMetaData& aSelectionPreferences)
	{
	CSipMetaCprSelector* self = new (ELeave) CSipMetaCprSelector();
	CleanupStack::PushL(self);
	self->ConstructL(aSelectionPreferences);
	CleanupStack::Pop();
	return self;
	}

void CSipMetaCprSelector::ConstructL(const Meta::SMetaData& aSelectionPreferences)
	{		
	ASSERT(aSelectionPreferences.IsTypeOf(TSelectionPrefs::TypeId()));	
	iDbs = CMDBSession::NewLC(KCDVersion1_2);
	CleanupStack::Pop(iDbs);
	iTierRecord = TierManagerUtils::LoadTierRecordL(TUid::Uid(CSipTierManagerFactory::iUid),*iDbs);
	iSelectionPrefs.Copy(aSelectionPreferences);
	}
	
CSipMetaCprSelector::CSipMetaCprSelector()
:	iMetaContainer(*(SockManGlobals::Get()->iMetaConnectionFactories))

	{
	__CFLOG_VAR((KSipTierMgrTag, KSipTierMgrSubTag, _L8("CSipMetaCprSelector [this=%08x]:\tCSipMetaCprSelector"), this));
	}

CSipMetaCprSelector::~CSipMetaCprSelector()
	{
	__CFLOG_VAR((KSipTierMgrTag, KSipTierMgrSubTag, _L8("CSipMetaCprSelector [this=%08x]:\t ~CSipMetaCprSelector"), this));
	Cancel();
	delete iProvisionInfo;
	iProvisionInfo = NULL;
	delete iDbs;
	}
	
void CSipMetaCprSelector::Cancel()
	{	
	}

void CSipMetaCprSelector::SelectL(ISelectionNotify& aSelectionNotify)
	{
	// In the case of SIP the selector selects the MCPR and sets the 
	// appUid, profileId and ptr to CTransitionEngineMgr which form the 
	// provisioning info that'll be passd to the CPR by the MCPR
	//
	__CFLOG_VAR((KSipTierMgrTag, KSipTierMgrSubTag, _L8("CSipMetaCprSelector %08x:\tSelectL()"), this));
	
	ASSERT(iDbs);
	ASSERT(iTierRecord);	
	
	TSecureId sid;
	ASubSessionPlatsecApiExt platsecext(iSelectionPrefs.SubSessionUniqueId());
	platsecext.SecureId(sid);
	TUid appUid = sid.operator TUid();		
	TUint32 profileId = KSIPDefaultProfileId;
	TUint32 defaultAccessPoint = iTierRecord->iDefaultAccessPoint;
	
	if (iSelectionPrefs.IsEmpty())
		{
		ASSERT(iSelectionPrefs.Flags()==0);
		__CFLOG_VAR((KSipTierMgrTag, KSipTierMgrSubTag, _L8("CSipMetaCprSelector %08x::\tSelectL() Using Default AP:%d"),this,defaultAccessPoint));				
		aSelectionNotify.SelectComplete(this,FindOrCreateProviderL(defaultAccessPoint, profileId, appUid));				
		aSelectionNotify.SelectComplete(this,NULL);
		return;
		}		
	
	const TConnPref& prefs = iSelectionPrefs.Prefs();
	
	if (prefs.ExtensionId() == TConnPref::EConnPrefSip)
		{		
		TSipConnPref sipPref = TSipConnPref::Cast(prefs);
 		TUint32 profileId = sipPref.GetProfileId();		
		CMetaConnectionProviderBase* mcpr = FindOrCreateProviderL(defaultAccessPoint,profileId,appUid);
		__ASSERT_ALWAYS(mcpr!=NULL, User::Leave(KErrNotFound));		
		
		aSelectionNotify.SelectComplete(this, mcpr);	
		aSelectionNotify.SelectComplete(this,NULL);		
		}
	}
		

CMetaConnectionProviderBase* CSipMetaCprSelector::FindOrCreateProviderL(TUint aAccessPoint, TUint32 aProfileId, TUid aAppUid)
	{
	__CFLOG_VAR((KSipTierMgrTag, KSipTierMgrSubTag, _L8("CSipMetaCprSelector %08x:\tFindOrCreateProviderL()"), this));
	
	//Find MCPR factory	
	TUid mCprUid = TierManagerUtils::ReadMCprUidL(aAccessPoint, *iDbs);
	
	CMetaConnectionProviderFactoryBase* factory = static_cast<CMetaConnectionProviderFactoryBase*>(iMetaContainer.FindOrCreateFactoryL(mCprUid));

	//Create the provider	
	TUid tierId = TUid::Uid(iTierRecord->iRecordTag);	
	// Adding the tier implementation id
	TUid tierImplId = TUid::Uid(iTierRecord->iTierImplUid);
	// Send Transition Engine Mgr, AppUid and ProfileID in provision to CPR
	iProvisionInfo = new (ELeave) TSipMcprProvisionInfo(); 
	iProvisionInfo->iProfileId = aProfileId;
	iProvisionInfo->iAppUid = aAppUid;
	iProvisionInfo->iTransitionEngineMgr = iTierManager->TransitionEngineManager();	
	
	TProviderInfo providerInfo(tierId,aAccessPoint,reinterpret_cast<TAny*>(iProvisionInfo));
	XSIPFindTheSamePrefsCPRQuery cprQuery(aAppUid, aProfileId, providerInfo, tierImplId);
	return static_cast<CMetaConnectionProviderBase*>(factory->FindOrCreateObjectL(cprQuery));
	}

void CSipMetaCprSelector::SetTierManager(CSipTierManager* aTierManager)
	{
	iTierManager = aTierManager;
	}




//-=========================================================
// XSIPFindTheSamePrefsCPRQuery
//-=========================================================
    
Factories::MFactoryQuery::TMatchResult XSIPFindTheSamePrefsCPRQuery::Match( Factories::TFactoryObjectInfo& aProviderInfo )
	{
	
	CSipMetaConnectionProvider* prov = static_cast<CSipMetaConnectionProvider*>(aProviderInfo.iInfo.iFactoryObject);
	
	// match the profile Ids and App Uids. 
	// a match for the both the profileId and appUid indicates that the same app has started the same profile again
	// it can share the MCPR (and the CPR, since MCPR : CPR is a 1:1 ralation )
	
	if (prov->AppUid() != iAppUid)
		{
		//uids dont match. They are different apps and cannot share an MCPR
		return EContinue;
		}
	if (prov->ProfileId() == iProfileId)
		{
		//the profile id also matches. Hence the same MCPR
		return EMatch;
		}
		
	return EContinue;
	}
	
