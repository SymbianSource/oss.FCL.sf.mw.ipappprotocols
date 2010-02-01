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
// SIP Tier Manager  selector class.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_SIP_TIER_MANAGER_SELECTOR_H
#define SYMBIAN_SIP_TIER_MANAGER_SELECTOR_H

#include <comms-infras/ss_tiermanager.h>
#include <comms-infras/ss_connselect.h>
#include <comms-infras/ss_platsec_apiext.h>
#include "siptiermanager.h"
#include "sipmcpr.h"
#include "sipmcprfactory.h"
#include <comms-infras/ss_metaconnprov.h> //for XMetaConnectionFactoryQuesry
#include <comms-infras/ss_mcprnodemessages.h>
#include <comms-infras/ss_metaconnprov_internal.h>

#include "sipprovisioninfo.h"

namespace CommsDat
{
	class CMDBSession;
	class CCDTierRecord;
}

using namespace ESock;

//
//CSipProviderSelector
class CSipMetaCprSelector : public CBase, public ESock::MProviderSelector
/** 
SIP connection selector.
Selector object for the SIP layer.

@internalComponent
@prototype
*/
	{
public:
	static ESock::MProviderSelector* NewL(const Meta::SMetaData& aSelectionPreferences);
	void SetTierManager(CSipTierManager* aTierManager);	
	void SetMcpr(CMetaConnectionProviderBase &aMcpr);
	
protected:	
	explicit CSipMetaCprSelector();
	virtual ~CSipMetaCprSelector();	
	virtual void SelectL(ESock::ISelectionNotify& aSelectionNotify);
	ESock::CMetaConnectionProviderBase* FindOrCreateProviderL(TUint aAccessPoint, TUint32 aProfileId, TUid aAppUid);	
	void ConstructL(const Meta::SMetaData& aSelectionPreferences);
	virtual void Cancel();	
	
protected:
	ESock::TSelectionPrefs iSelectionPrefs;
	CommsDat::CMDBSession* iDbs;
	ESock::CMetaConnectionFactoryContainer& iMetaContainer; 
	CSipTierManager* iTierManager;
	CommsDat::CCDTierRecord* iTierRecord;
	
private : 
	TSipMcprProvisionInfo* iProvisionInfo;
	};



class XSIPFindTheSamePrefsCPRQuery : public TMetaConnectionFactoryQuery
/**A query that finds the first SIPCPR created against both
the same application UID and the same profile.

@internalComponent
*/
	{
public:
	XSIPFindTheSamePrefsCPRQuery(TUid aAppUid, TUint32 aProfileId, TProviderInfo aProviderInfo, TUid aTierImplUid) 
	: TMetaConnectionFactoryQuery(aProviderInfo,aTierImplUid),
		iAppUid( aAppUid ),
	 	iProfileId( aProfileId )
		{
		}

protected:
	TUid 	iAppUid;
	TUint32 iProfileId;
	

public:
	virtual TMatchResult Match( Factories::TFactoryObjectInfo& aConnectionInfo );
	};
	
	
#endif //SYMBIAN_SIP_TIER_MANAGER_SELECTOR_H
