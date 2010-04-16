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
// NETMCPR.CPP
// This is part of an ECOM plug-in
// SIP Meta Conneciton Provider class definition
// 
//

/**
 @file
 @internalComponent
*/

#include "sipmcpr.h"
#include <comms-infras/ss_nodemessages_selector.h>
#include <comms-infras/ss_corepractivities.h>
#include <elements/mm_states.h>

#ifdef __CFLOG_ACTIVE
#define KSipMCprTag KESockMetaConnectionTag
_LIT8(KSipMCprSubTag, "sipmcpr");
#endif

using namespace ESock;
using namespace NetStateMachine;
using namespace Messages;
using namespace MeshMachine;
using namespace MCprActivities;
using namespace PRActivities;
using namespace CoreActivities;


/*
The problem is that we have a single dummy access point for sip.
If I open 2 connection , ie 2 mcprs, the access point config is still shared.
This becomes a problem because the the tierMgr instances set in the provision message
will be of the same type.
*/
namespace SipMCprStates
{
DEFINE_SMELEMENT(TSendProvision, NetStateMachine::MStateTransition, SipMCprStates::TContext)
void SipMCprStates::TSendProvision::DoL()
	{	
	PRStates::TSendProvision  mcprSendProvision(iContext);
	mcprSendProvision.DoL();
	}
}

// Default activity is overriden here. SIP MCPR needs to extend the access point config
// with SIP MCPR provision info(Appuid, ProfileID and TransitionEngine Mgr).  This extended
// Provision info will be sent in provision messages SendProvision::DoL()
namespace SipMCprBinderRequestActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityBinderRequest, SipMCprBinderRequest, TCFServiceProvider::TCommsBinderRequest, CCommsBinderActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingBinderRequest, CCommsBinderActivity::TNoTagOrUseExistingBlockedByBinderRequest)
	NODEACTIVITY_ENTRY(KNoTag, PRStates::TCreateDataClient, CoreNetStates::TAwaitingDataClientJoin, MeshMachine::TNoTag)	

	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SipMCprStates::TProcessCprCreation, MeshMachine::TTag<CoreStates::KUseExisting>)		
	NODEACTIVITY_ENTRY(CoreStates::KUseExisting, PRActivities::CCommsBinderActivity::TSendBinderResponse, PRActivities::CCommsBinderActivity::TAwaitingBindToComplete, MeshMachine::TNoTagOrErrorTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TClearError)
NODEACTIVITY_END()
}

// For the SIP there is no below layer. Send Select complete when this msg is received
namespace SipMCprSelectNextLayerActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivitySelect, SipMCprSelectNextLayer, TCFSelector::TSimpleSelect)
FIRST_NODEACTIVITY_ENTRY(MCprStates::TAwaitingSelectNextLayer, MeshMachine::TNoTag)
LAST_NODEACTIVITY_ENTRY(KNoTag, MCprStates::TSendFinalSelectComplete)
NODEACTIVITY_END()
}

namespace SipMcpr
{
DEFINE_EXPORT_ACTIVITY_MAP(SipMcprActivities)
   ACTIVITY_MAP_ENTRY(SipMCprBinderRequestActivity, SipMCprBinderRequest)
   ACTIVITY_MAP_ENTRY(SipMCprSelectNextLayerActivity,SipMCprSelectNextLayer)
   ACTIVITY_MAP_END_BASE(PRActivities, coreActivitiesMCpr)   
}

//
//CSipMetaConnectionProvider
CSipMetaConnectionProvider* CSipMetaConnectionProvider::NewL(CMetaConnectionProviderFactoryBase& aFactory, const TProviderInfo& aProviderInfo)
	{
	__CFLOG_VAR((KSipMCprTag, KSipMCprSubTag, _L8("CSipMetaConnectionProvider:\tNewL()")));
	CSipMetaConnectionProvider* self = new (ELeave) CSipMetaConnectionProvider(aFactory,aProviderInfo,SipMcpr::SipMcprActivities::Self());
	CleanupStack::PushL(self);
	self->ConstructL(aProviderInfo);
	CleanupStack::Pop(self);	
	return self;
	}

CSipMetaConnectionProvider::CSipMetaConnectionProvider(CMetaConnectionProviderFactoryBase& aFactory, const TProviderInfo& aProviderInfo, const MeshMachine::TNodeActivityMap& aActivityMap)
:	CCoreMetaConnectionProvider(aFactory,aProviderInfo,aActivityMap)

	{
	LOG_NODE_CREATE(KSipMCprTag, CSipMetaConnectionProvider);
	}

CSipMetaConnectionProvider::~CSipMetaConnectionProvider()
	{
	LOG_NODE_DESTROY(KSipMCprTag, CSipMetaConnectionProvider);
	}

void CSipMetaConnectionProvider::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
//@TODO log
//	__CFLOG_VAR((KSipMCprTag, KSipMCprSubTag, _L8("CSipMetaConnectionProvider %08x:\tReceivedL() aCFMessage=%d"),
//	   this, aCFMessage.MessageId().MessageId()));
	TNodeContext<CSipMetaConnectionProvider> ctx(*this, aMessage, aSender, aRecipient);
    CCoreMetaConnectionProvider::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}


void CSipMetaConnectionProvider::ConstructL(const ESock::TProviderInfo& aProviderInfo)
{
	CMetaConnectionProviderBase::ConstructL(); //This initialises the AccessPointInfo
	
	// Set the SIP Provision info
	if (aProviderInfo.Instance()!= NULL)
	{
	TSipMcprProvisionInfo* sipext = reinterpret_cast<TSipMcprProvisionInfo *>(aProviderInfo.Instance());
	SetAppUid(sipext->iAppUid);
	SetProfileId(sipext->iProfileId);
	SetTransitionEngineManager(sipext->iTransitionEngineMgr);
	}
    
  	//For construction of IpProto providers (construction only) we use the instance field of TProviderInfo
   	//to carry the IAP id. The IAP id is necessary for the initialisation if IpProto specific data.
    //TInt iap = (TInt)ProviderInfo().Instance(); // instance is set to null and hence the iap also will be null
    TInt iap = (TInt)ProviderInfo().APId();    
	SetProviderInfo(TProviderInfo(ProviderInfo().TierId(),iap));
    SetConfigL();	
}

void CSipMetaConnectionProvider::SetConfigL()
	{
	TSipMcprProvisionInfo*	ProvisionInfo = new (ELeave) TSipMcprProvisionInfo();
	ProvisionInfo->iAppUid = iAppUid;
	ProvisionInfo->iProfileId = iProfileId;
	ProvisionInfo->iTransitionEngineMgr = iTransitionEngineMgr;

	// Append the above three to the accesspointconfig and send to CPR in ProvisionConfig message

	RMetaExtensionContainer mec;
	mec.Open(AccessPointConfig());
	CleanupClosePushL(mec);
	CleanupStack::PushL(ProvisionInfo);
	mec.AppendExtensionL(ProvisionInfo);
	CleanupStack::Pop(ProvisionInfo);
	AccessPointConfig().Close();
	AccessPointConfig().Open(mec);
	CleanupStack::PopAndDestroy(&mec);
	}
