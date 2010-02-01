// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// rtpscpr.cpp
// RTP SubConnection Provider implementation
// 
//

/**
 @file
 @internalComponent
*/
 
#include <comms-infras/ss_log.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include "dummy_rtpscpr.h"
#include "dummy_rtpscprstates.h"
#include <comms-infras/ss_log.h>
#include "rtp_cfmessages.h"

using namespace ESock;
using namespace NetStateMachine;
using namespace Messages;
using namespace MeshMachine;

namespace RTPSCprParamsRequest
{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
DECLARE_DEFINE_NODEACTIVITY(ECFActivityParamRequest, RTPSCprSetParams, TCFScpr::TSetParamsRequest)
	FIRST_NODEACTIVITY_ENTRY(PRStates::TAwaitingParamRequest, CoreNetStates::TNoTagOrBearerPresent)
	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, PRStates::TPassToServiceProvider, CoreNetStates::TAwaitingParamResponse, CoreNetStates::TBearerPresent)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, PRStates::TStoreParamsAndPostToOriginators)
	LAST_NODEACTIVITY_ENTRY(KNoTag, PRStates::TStoreAndRespondWithCurrentParams)
NODEACTIVITY_END()
#else
DECLARE_DEFINE_NODEACTIVITY(ECFActivityParamRequest, RTPSCprSetParams, TCFScpr::TParamsRequest)
	FIRST_NODEACTIVITY_ENTRY(SCprStates::TAwaitingParamRequest, CoreNetStates::TNoTagOrBearerPresent)
	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, SCprStates::TPassToServiceProvider, CoreNetStates::TAwaitingParamResponse, CoreNetStates::TBearerPresent)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, SCprStates::TStoreParamsAndPostToOriginators)
	LAST_NODEACTIVITY_ENTRY(KNoTag, SCprStates::TStoreAndRespondWithCurrentParams)
NODEACTIVITY_END()
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
}


//This will check if the scpr is of type RTP and then branch
//If RTP, it will send commsbinder request to IPCPR to make the deftIP SCPR its service provider
//If IPSCPR ( normal case ), it will send commsbinder to its servicwe provider
namespace RTPSCprNoBearerActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityNoBearer, RTPSCprNoBearer, TCFControlProvider::TNoBearer, PRActivities::CNoBearer::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingNoBearer, DummyRtpSCprStates::TNoTagOrBearerPresentOrErrorTag)
	
	NODEACTIVITY_ENTRY(KNoTag, DummyRtpSCprStates::TSendCommsBinderToCntrlProv, CoreNetStates::TAwaitingBinderResponse, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendControlClientJoinRequest, CoreStates::TAwaitingJoinComplete, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CoreActivities::ABindingActivity::TSendBindToComplete, MeshMachine::TNoTag)
	
	NODEACTIVITY_ENTRY(KNoTag, DummyRtpSCprStates::TRequestCommsBinderRetry, CoreNetStates::TAwaitingBinderResponse, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, DummyRtpSCprStates::TRequestCommsBinderRetry, CoreNetStates::TAwaitingBinderResponse, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendBindTo, CoreNetStates::TAwaitingBindToComplete, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CoreActivities::ABindingActivity::TSendBindToComplete, PRActivities::CNoBearer::TNoTagOrDataClientsToStart)

	//Flow asking for a bearer needs to be started (provided KErrNone)
	NODEACTIVITY_ENTRY(CoreNetStates::KDataClientsToStart, PRActivities::CNoBearer::TStartOriginatingDataClient, CoreNetStates::TAwaitingDataClientStarted, MeshMachine::TNoTag)
		
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
	
	/* Error handling */
	LAST_NODEACTIVITY_ENTRY(MeshMachine::KErrorTag, MeshMachine::TRaiseAndClearActivityError)
		
NODEACTIVITY_END()
}


// The activity will wait for ProvisionDone message from the flow. This will prevent the flow hanging in air with out a bearer while the 
// socket is writing data to it. Now there is a case where in flow couldnot bind to the SP also the NoBearer which the flow sends maynot 
// be completed. Unfortunately this activity has no way of knowing that the NoBearer sent by the flow has failed. The ProvisionDone message 
// has a response code which will give an indication as to whether the flow is ready to accept a BindTo from above.
namespace RTPSCprBinderRequestActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityBinderRequest, RTPSCprBinderRequest, ESock::TCFServiceProvider::TCommsBinderRequest, PRActivities::CCommsBinderActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(DummyRtpSCprStates::TAwaitingBinderRequest, PRActivities::CCommsBinderActivity::TNoTagOrUseExistingBlockedByBinderRequest)
	NODEACTIVITY_ENTRY(KNoTag, DummyRtpSCprStates::TCreateDataClient, CoreNetStates::TAwaitingDataClientJoin, MeshMachine::TNoTag)

	THROUGH_NODEACTIVITY_ENTRY(KNoTag, PRActivities::CCommsBinderActivity::TProcessDataClientCreation, MeshMachine::TTag<CoreStates::KUseExisting>) 
	NODEACTIVITY_ENTRY(CoreStates::KUseExisting, DummyRtpSCprStates::TSetPostedToFlow, DummyRtpSCprStates::TAwaitingReadyForBind, DummyRtpSCprStates::TNoTagOrErrorTag)
	LAST_NODEACTIVITY_ENTRY(MeshMachine::KErrorTag, MeshMachine::TRaiseAndClearActivityError)
	// Below this point we need to modify the error handling approach. If we're getting a TError on TBinderResponse,
	// this means the client requesting the binder couldn't bind to it. As far as the client is concerned, this
	// activity is finished (it has flagged an error). The standard error handling will result in erroring
	// the originator. In this case we shouldn't error the originator, instead, wrap up quietly.
	NODEACTIVITY_ENTRY(KNoTag, PRActivities::CCommsBinderActivity::TSendBinderResponse, PRActivities::CCommsBinderActivity::TAwaitingBindToComplete, DummyRtpSCprStates::TNoTagOrErrorTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, DummyRtpSCprStates::TCheckParamsAndDoSomething, MeshMachine::TNoTag) 
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
	LAST_NODEACTIVITY_ENTRY(MeshMachine::KErrorTag, MeshMachine::TClearError)
NODEACTIVITY_END()
}


namespace RTPSCprActivities
{
DECLARE_DEFINE_ACTIVITY_MAP(activityMap)
    ACTIVITY_MAP_ENTRY(RTPSCprParamsRequest, RTPSCprSetParams)
	ACTIVITY_MAP_ENTRY(RTPSCprNoBearerActivity, RTPSCprNoBearer)
	ACTIVITY_MAP_ENTRY(RTPSCprBinderRequestActivity, RTPSCprBinderRequest)
ACTIVITY_MAP_END_BASE(SCprActivities, coreSCprActivities)
}

//-=========================================================
//
// CRtpDummySubConnectionProvider methods
//
//-=========================================================	
CRtpDummySubConnectionProvider* CRtpDummySubConnectionProvider::NewL(ESock::CSubConnectionProviderFactoryBase& aFactory)
/**
Construct a new RTP SubConnection Provider Object

@param aFactory factory that create this object
@param aConnProvider Connection Provider associated with this object
*/
	{
	CRtpDummySubConnectionProvider* self = new (ELeave) CRtpDummySubConnectionProvider(aFactory);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
	return self;
	}


CRtpDummySubConnectionProvider::~CRtpDummySubConnectionProvider()
    {
    LOG_NODE_DESTROY(KRTPSCprTag, CRtpDummySubConnectionProvider);
    }


CRtpDummySubConnectionProvider::CRtpDummySubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory)
:CCoreSubConnectionProvider(aFactory, RTPSCprActivities::activityMap::Self())
    {
    LOG_NODE_CREATE(KRTPSCprTag, CRtpDummySubConnectionProvider);
    }

void CRtpDummySubConnectionProvider::ConstructL()
/**
RTP SubConnection Provider Second Phase Constructor
*/
	{
    CCoreSubConnectionProvider::ConstructL();
	}

void CRtpDummySubConnectionProvider::Received( MeshMachine::TNodeContextBase& aContext )
    {
	
    Messages::TNodeSignal::TMessageId noPeerIds[] = {
        TCFFactory::TPeerFoundOrCreated::Id(),
        TCFPeer::TJoinRequest::Id(),
        Messages::TNodeSignal::TMessageId()
        };

    MeshMachine::AMMNodeBase::Received(noPeerIds, aContext);
	MeshMachine::AMMNodeBase::PostReceived(aContext);
	}

void CRtpDummySubConnectionProvider::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aCFMessage)
    {
    DummyRtpSCprStates::TContext ctx(*this, aCFMessage, aSender, aRecipient);
    CRtpDummySubConnectionProvider::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}

