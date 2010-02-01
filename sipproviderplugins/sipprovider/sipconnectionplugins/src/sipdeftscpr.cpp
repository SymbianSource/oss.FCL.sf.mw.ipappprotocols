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
// SIP Default SubConnection Provider implementation
// 
//

/**
 @file
 @internalComponent
*/

#include "sipdeftscpr.h"
#include "sipscprstates.h"
//#include <ss_nodemessages_service_provider.h>

using namespace ESock;
using namespace NetStateMachine;
using namespace Messages;
using namespace MeshMachine;


//-=========================================================
//
// Activities
//
//-========================================================

namespace SipDeftSCprProvisionActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityStoreProvision, SipDeftSCprProvision, TCFDataClient::TProvisionConfig)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingProvision, MeshMachine::TNoTag)	
	LAST_NODEACTIVITY_ENTRY(KNoTag, PRStates::TStoreProvision)
NODEACTIVITY_END()
}

namespace SipDeftSCprNoBearerActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityNoBearer, SipDeftSCprNoBearer, TCFControlProvider::TNoBearer)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingNoBearer, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

//Default SIP SCPR shouldn't get Set Params, we do nothing instead of raising an error.
namespace SipDeftSCprParamsRequest
{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
DECLARE_DEFINE_NODEACTIVITY(ECFActivityParamRequest, SipDeftSCprSetParams, TCFScpr::TSetParamsRequest)
	FIRST_NODEACTIVITY_ENTRY(PRStates::TAwaitingParamRequest, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
#else
DECLARE_DEFINE_NODEACTIVITY(ECFActivityParamRequest, SipDeftSCprSetParams, TCFScpr::TParamsRequest)
	FIRST_NODEACTIVITY_ENTRY(SCprStates::TAwaitingParamRequest, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
}

namespace SipDeftSCprNotification
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityNotification, SipDeftSCprNotif, TCFSubConnControlClient::TSubConnNotification)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, CoreNetStates::TAwaitingSubConEvent, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace SipDeftSCprApplyReq
{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
DECLARE_DEFINE_NODEACTIVITY(ECFActivityApplyChanges, SipDeftSCprApplyReq, TCFScpr::TApplyRequest)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, PRStates::TAwaitingApplyRequest, MeshMachine::TNoTag)
NODEACTIVITY_END()
#else
DECLARE_DEFINE_NODEACTIVITY(ECFActivityApplyChanges, SipDeftSCprApplyReq, TCFScpr::TApplyRequest)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, SCprStates::TAwaitingApplyRequest, MeshMachine::TNoTag)
NODEACTIVITY_END()
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
}

//copied the PR:StartActivity and culled stuff that we don't need
// Since SIP scpr never has a service provider, fork will always return KNoTag
//This should explain the NoTagOrBearerPresentBlockedByStop :-)
namespace SipDeftSCprStartActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStart, SipDeftSCprStart, TCFServiceProvider::TStart, PRActivities::CStartActivity::NewL)
    FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingStart, CoreNetStates::TNoTagOrBearerPresentBlockedByStop)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendStarted)
NODEACTIVITY_END()
}

namespace SipDeftSCprStopActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityStop, SipDeftSCprStop, TCFServiceProvider::TStop)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingStop, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, PRStates::TSendStopped)
NODEACTIVITY_END()
}

namespace SipDeftSCprRejoinDataClient
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityRejoin, SipDeftSCprRejoin, TCFRejoiningProvider::TRejoinDataClientRequest, PRActivities::CRejoinDataClientActivity::NewL)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TRaiseError<KErrNotSupported>, CoreNetStates::TAwaitingDataClientRejoin, MeshMachine::TNoTag)
NODEACTIVITY_END()
}


namespace SipDeftPRDataClientStartActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityStartDataClient, SipDeftPRDataClientStart, TCFDataClient::TStart)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, CoreNetStates::TAwaitingDataClientStart, MeshMachine::TNoTag)
NODEACTIVITY_END()
}


namespace SipDeftPRGoneDownActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityGoneDown, SipDeftPRGoneDown, TCFControlClient::TGoneDown)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, CoreNetStates::TAwaitingGoneDown, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace SipDeftPRBindToRequestActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityBindTo, SipDeftPRBindToRequest, TCFDataClient::TBindTo)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, CoreNetStates::TAwaitingBindTo, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace SipDeftPRDataClientIdleActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityDataClientIdle, SipDeftPRDataClientIdle, TCFControlProvider::TIdle)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, CoreNetStates::TAwaitingDataClientIdle, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace SipDeftPRDataClientActiveActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityDataClientActive, SipDeftPRDataClientActive, TCFControlProvider::TActive)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, CoreNetStates::TAwaitingDataClientActive, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

/*
namespace SipDeftReBindLeaveAndForward
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityReBindLayer, SipDeftPRReBindLayer, TCFReconnect::TReBindLayer)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, CoreNetStates::TAwaitingReBindLayer, MeshMachine::TNoTag)
NODEACTIVITY_END()
}


namespace SipDeftPRControlClientJoinActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityControlClientJoin, SipDeftPRControlClientJoin, TCFNodeSignalBase::TNullMessageId)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, CoreNetStates::TAwaitingControlClientJoin, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace SipDeftPRDataClientJoinActivity
{ //This activity needs the activity object (& it can fail on AddClientL, so no point converting)
DECLARE_DEFINE_NODEACTIVITY(ECFActivityDataClientJoin, SipDeftPRDataClientJoin, TCFFactory::TPeerFoundOrCreated)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingDataClientJoinRequest, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace SipDeftPRBuildLowerLayerActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityBuildLowerLayer, SipDeftPRBuildLowerLayer, TCFReconnect::TBuildLowerLayer)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingBuildLowerLayer, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

*/
namespace SipDefaultSCprActivities
{
DECLARE_DEFINE_ACTIVITY_MAP(activityMap)
	ACTIVITY_MAP_ENTRY(SipDeftSCprProvisionActivity, SipDeftSCprProvision)
	ACTIVITY_MAP_ENTRY(SipDeftSCprNoBearerActivity, SipDeftSCprNoBearer)
	ACTIVITY_MAP_ENTRY(SipDeftSCprStartActivity, SipDeftSCprStart)
	ACTIVITY_MAP_ENTRY(SipDeftSCprStopActivity, SipDeftSCprStop)
	ACTIVITY_MAP_ENTRY( SipDeftSCprParamsRequest, SipDeftSCprSetParams)	
	ACTIVITY_MAP_ENTRY(SipDeftSCprRejoinDataClient, SipDeftSCprRejoin)		
	ACTIVITY_MAP_END_BASE(SCprActivities, coreSCprActivities)
}  



//-=========================================================
//
// CSipDefaultSubConnectionProvider methods
//
//-=========================================================	
CSipDefaultSubConnectionProvider::~CSipDefaultSubConnectionProvider()
    {
    LOG_NODE_DESTROY(KSipSCprTag, CSipDefaultSubConnectionProvider);
    }

CSipDefaultSubConnectionProvider::CSipDefaultSubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory)
	: CCoreSubConnectionProvider(aFactory, SipDefaultSCprActivities::activityMap::Self())
    {
    LOG_NODE_CREATE(KSipSCprTag, CSipDefaultSubConnectionProvider);
    }
    
void CSipDefaultSubConnectionProvider::ConstructL()
    {    
    CCoreSubConnectionProvider::ConstructL();
    }    

CSipDefaultSubConnectionProvider *CSipDefaultSubConnectionProvider::NewL(ESock::CSubConnectionProviderFactoryBase& aFactory)
	{
	CSipDefaultSubConnectionProvider* prov = new (ELeave) CSipDefaultSubConnectionProvider(aFactory);
    CleanupStack::PushL(prov);
    prov->ConstructL();
    CleanupStack::Pop(prov);
    return prov;
	}

void CSipDefaultSubConnectionProvider::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
	TNodeContext<CSipDefaultSubConnectionProvider> ctx(*this, aMessage, aSender, aRecipient);
    CCoreSubConnectionProvider::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}
	



