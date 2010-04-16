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
// SIP Connection Provider implementation
// 
//

/**
 @file
 @internalComponent
*/

#include <comms-infras/ss_log.h>
#include <comms-infras/corecprstates.h>
#include <comms-infras/corecpractivities.h>
#include <comms-infras/metatype.h>
#include "sipcpr.h"
#include "sipcprstates.h"
#include "sipprovisioninfo.h"
#include <elements/mm_states.h>


using namespace SipCprStates;
using namespace SipCpr;
using namespace ESock;
using namespace NetStateMachine;
using namespace Messages;
using namespace MeshMachine;
using namespace Meta;
using namespace CprActivities;

//
//=========================================================
//
// Activities
//
//-=========================================================



// provision activity to store provision message containing TransitionEngineMgr,ProfileId,appUid
// from MCPR
namespace SipCprProvisionActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityStoreProvision, SipCprProvision, TCFDataClient::TProvisionConfig)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingProvision,  MeshMachine::TNoTagOrErrorTag)	
	LAST_NODEACTIVITY_ENTRY(KNoTag, SipCprStates::TStoreProvision)	
NODEACTIVITY_END()
}


namespace SipCprBinderRequestActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityBinderRequest, SipCprBinderRequest, TCFServiceProvider::TCommsBinderRequest, PRActivities::CCommsBinderActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingBinderRequest,PRActivities::CCommsBinderActivity::TNoTagOrWaitForIncomingOrUseExistingDefaultBlockedByBinderRequest)		
	NODEACTIVITY_ENTRY(KNoTag, PRStates::TCreateDataClient, CoreNetStates::TAwaitingDataClientJoin, MeshMachine::TNoTag)    	
	NODEACTIVITY_ENTRY(CoreNetStates::KWaitForIncoming, SipCprStates::TProcessWaitForIncmg, CoreNetStates::TAwaitingDataClientJoin, MeshMachine::TNoTag)	
	// Below this point we need to modify the error handling approach. If we're getting a TError on TBinderResponse,
	// this means the client requesting the binder couldn't bind to it. As far as the client is concerned, this
	// activity is finished (it has flagged an error). The standard error handling will result in erroring
	// the originator. In this case we shouoldn't error the originator, instead, wrap up quietly.
	
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SipCprStates::TProcessSCPRCreation, SipCprStates::TWaitForIncomingOrUseExisting)
	// Wait for the incomingconnection message from SIP SCPR 
	NODEACTIVITY_ENTRY(CoreNetStates::KWaitForIncoming,SipCprStates::TSetPostedToScpr, SipCprStates::TAwaitingIncomingConnection, MeshMachine::TTag<CoreStates::KUseExisting>)
	NODEACTIVITY_ENTRY(CoreStates::KUseExisting, PRActivities::CCommsBinderActivity::TSendBinderResponse, PRActivities::CCommsBinderActivity::TAwaitingBindToComplete/*CoreNetStates::TAwaitingBindToComplete*/, MeshMachine::TNoTagOrErrorTag)	
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)	
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TRaiseAndClearActivityError)
	
NODEACTIVITY_END() 
}

//start activity
//this could have a race condition.Please check SipCprStates::RegisterCpr
namespace SipCprStartActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStart, SipCprStart, TCFServiceProvider::TStart, PRActivities::CStartActivity::NewL)
    FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingStart, SipCprStates::TNoTagOrRegisteredBlockedByStop)		
	NODEACTIVITY_ENTRY(KNoTag, SipCprStates::TRegisterCpr, SipCprStates::TAwaitingRegistrationComplete, MeshMachine::TNoTagOrErrorTag)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TStartSelf, CoreNetStates::TAwaitingDataClientStarted, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendStarted)	
	LAST_NODEACTIVITY_ENTRY(SipCprStates::KRegistered, CoreNetStates::TSendStarted)	
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TRaiseAndClearActivityError)
NODEACTIVITY_END()
}

namespace SipCprStopActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityStop, SipCprStop, TCFServiceProvider::TStop)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingStop, MeshMachine::TActiveOrNoTag<ECFActivityStartDataClient>)	
	THROUGH_NODEACTIVITY_ENTRY(MeshMachine::KActiveTag, CoreNetStates::TCancelDataClientStart, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TStopSelf, CoreNetStates::TAwaitingDataClientStopped, SipCprStates::TNoTagOrDeRegister)
	NODEACTIVITY_ENTRY(SipCprStates::KDeRegister, SipCprStates::TDeRegisterCpr, SipCprStates::TAwaitingDeRegistrationComplete, MeshMachine::TNoTagOrErrorTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, /*SipCprStates::TSendStopped*/PRStates::TSendStoppedAndGoneDown)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TRaiseAndClearActivityError)	
NODEACTIVITY_END()
}

//Activity Map
namespace SipCprActivities	
	{	
	DECLARE_DEFINE_ACTIVITY_MAP(activityMap)
		ACTIVITY_MAP_ENTRY(SipCprProvisionActivity, SipCprProvision)
		ACTIVITY_MAP_ENTRY(SipCprBinderRequestActivity, SipCprBinderRequest)
		ACTIVITY_MAP_ENTRY(SipCprStartActivity, SipCprStart)
		ACTIVITY_MAP_ENTRY(SipCprStopActivity, SipCprStop)				
		
	ACTIVITY_MAP_END_BASE(CprActivities, coreCprActivities)	
	}


//-=========================================================
//
// CSipConnectionProvider methods
//
//-=========================================================	
   	
/**
Creates the SIP connection provider
*/
CSipConnectionProvider* CSipConnectionProvider::NewL(CConnectionProviderFactoryBase& aFactory)
    {
    __CFLOG_VAR((KSipCprTag, KSipCprSubTag, _L8("CSipConnectionProvider:\tNewL()")));    
    CSipConnectionProvider* prov = new (ELeave) CSipConnectionProvider(aFactory);
    CleanupStack::PushL(prov);
    prov->ConstructL();
    CleanupStack::Pop(prov);
    return prov;
    }
    
void CSipConnectionProvider::ConstructL()
    {    
    CCoreConnectionProvider::ConstructL();
    iProvisionInfo = NULL;
    iTransitionEngine = NULL;
    iIsIncoming = NULL;    
    }    

/**
Frees the memory for the transition engine
*/
CSipConnectionProvider::~CSipConnectionProvider()
    {
    __CFLOG_VAR((KSipCprTag, KSipCprSubTag, _L8("CSipConnectionProvider:\t ~CSipConnectionProvider()")));
    LOG_NODE_DESTROY(KSipCprTag, CSipConnectionProvider);
	if(iTransitionEngine != NULL && iTransitionEngineMgr != NULL)
    iTransitionEngineMgr->Detach(iTransitionEngine);
	//delete 	iProvisionInfo;
	iProvisionInfo = NULL;
    }

CSipConnectionProvider::CSipConnectionProvider(ESock::CConnectionProviderFactoryBase& aFactory)
	: CCoreConnectionProvider(aFactory, SipCprActivities::activityMap::Self()),
	  iStage(EFresh),iConnStatus(EConnOutgoing)
    {
    LOG_NODE_CREATE(KSipCprTag, CSipConnectionProvider);
    iSelf.Open(NodeId());
    }
    
 
void CSipConnectionProvider::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
	TNodeContext<CSipConnectionProvider> ctx(*this, aMessage, aSender, aRecipient);
    CCoreConnectionProvider::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}

/*
Constructs SIP CPR provision and appends it to the access point config
*/
void CSipConnectionProvider::SetConfigL()
	{	 
	// This function will be called two times while creation of default SCPR
	// and non-default SCPR.
	if (iProvisionInfo != NULL)
		{
		//delete 	iProvisionInfo;	
		iProvisionInfo = NULL;		
		}	
		
	iProvisionInfo = new (ELeave) TSipCprProvisionInfo();
	
	iProvisionInfo->iTransitionEngine = GetTE();
		
	//its safe to get the iFromField only after Registration.
	//We assume that the since SetConfig is invoked by the Send Provision activity
	//registration would have already been done	
	
	if(iFromField == NULL && iTransitionEngine != NULL)
		{
		iFromField = const_cast<TDesC8* >(iTransitionEngine->GetFromField(iProfileId));
		iProvisionInfo->iFromField = iFromField;
		}
	
	// A local object for RMetaExtensionContainer is created which has it's own base container class 
	// and the extension is appended into the array of this base container class.
	// At the second time for non- default scpr when this function is called again a new local copy of
	// RMetaExtensionContainer is created and the previous extension added is not present in the array of this object. 
	// The extension is added in new container class without panicing the code.
	
		RMetaExtensionContainer mec;
		mec.Open(AccessPointConfig());
		CleanupClosePushL(mec);
		mec.AppendExtensionL(iProvisionInfo);
		AccessPointConfig().Close();
		AccessPointConfig().Open(mec);
		CleanupStack::PopAndDestroy(&mec);
					
	}	
	
//-=========================================================
// MSIPRegistrationClient methods
//-=========================================================
// Will be called when the registration is successful
void CSipConnectionProvider::RegistrationComplete(TInt aError)
	{
	__CFLOG_VAR((KSipCprTag, KSipCprSubTag, _L8("CSipConnectionProvider::RegistrationComplete() errorcode=%d"), aError));
	ASSERT(iStage == SipCpr::EStarting);	
	
	// Send the RegistrationComplete message for waiting start activity
	iSelf.PostMessage(TNodeCtxId(iActivityAwaitingResponse, Id()),TSipCprMessages::TRegistrationComplete(aError).CRef());
	iActivityAwaitingResponse = MeshMachine::KActivityNull;
	}


// Will be called when the De-Registration is successful
void CSipConnectionProvider::DeRegistrationComplete(TInt aError)
	{
	__CFLOG_VAR((KSipCprTag, KSipCprSubTag, _L8("CSipConnectionProvider::DeRegistrationComplete() errorcode=%d"), aError));
	ASSERT(iStage == SipCpr::EStopping);
	iStage = SipCpr::EStopped;
	
	// Send the De-RegistrationComplete message for waiting stop activity
	iSelf.PostMessage(TNodeCtxId(iActivityAwaitingResponse, Id()),TSipCprMessages::TDeRegistrationComplete(aError).CRef());
	iActivityAwaitingResponse = MeshMachine::KActivityNull;
	}


// will be called when error occured. The error can occur either in registration or de-registration
void CSipConnectionProvider::ErrorOccurred(TInt aError)
	{
	__CFLOG_VAR((KSipCprTag, KSipCprSubTag, _L8("CSipConnectionProvider::ErrorOccurred() errorcode=%d"), aError));
	
	if (iActivityAwaitingResponse == ESock::ECFActivityStop)
		{
		//Error occured during DeRegistration
    	iSelf.PostMessage(TNodeCtxId(iActivityAwaitingResponse, Id()),TSipCprMessages::TDeRegistrationComplete(aError).CRef());
		}
	else /*if (iActivityAwaitingResponse == ESock::ECFActivityStart)*/
		{
		//Error should have occured during Registration
    	iSelf.PostMessage(TNodeCtxId(iActivityAwaitingResponse, Id()),TSipCprMessages::TRegistrationComplete(aError).CRef());
		}	
	}

TUint32 CSipConnectionProvider::GetProfileId()
	{
	return iProfileId;
	}
TUid CSipConnectionProvider::GetAppUid()
	{
	return iAppUid;
	}
CSIPTransitionEngine* CSipConnectionProvider::GetTE()
{
	return iTransitionEngine;
}	
