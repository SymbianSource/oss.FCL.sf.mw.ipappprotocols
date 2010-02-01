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
// SIP TierManager class definition
// 
//

/**
 @file
 @internalComponent
*/

#include <comms-infras/ss_log.h>
#include <comms-infras/ss_corepractivities.h>
#include <comms-infras/coretiermanagerstates.h>
#include <comms-infras/coretiermanageractivities.h>
#include "siptiermanager.h"
#include "siptiermanagerselector.h"

#ifdef __CFLOG_ACTIVE
#define KSipTierMgrTag KESockMetaConnectionTag
#endif

using namespace ESock;
using namespace NetStateMachine;
using namespace Messages;
using namespace MeshMachine;

namespace SipTierManagerActivities
{
DECLARE_DEFINE_ACTIVITY_MAP(stateMap)
ACTIVITY_MAP_END_BASE(TMActivities, coreTMActivities)
}


CSipTierManager* CSipTierManager::NewL(ESock::CTierManagerFactoryBase& aFactory)
/** 
@param aParentContainer the parent factory container which owns this factory
@return factory for SIP level meta connection providers
*/
	{
 	CSipTierManager* self =  new (ELeave) CSipTierManager(aFactory,SipTierManagerActivities::stateMap::Self());
 	CleanupStack::PushL (self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSipTierManager::CSipTierManager(ESock::CTierManagerFactoryBase& aFactory,
                                 const MeshMachine::TNodeActivityMap& aActivityMap)
:	CCoreTierManager(aFactory,aActivityMap)
/** 
@param aFactoryId the ID which this factory can be looked up by
@param aParentContainer the parent factory container which owns this factory
*/
	{
	LOG_NODE_CREATE(KSipTierMgrTag, CSipTierManager);
	}

MProviderSelector* CSipTierManager::DoCreateProviderSelectorL(const Meta::SMetaData& aSelectionPreferences)
	{
	
	MProviderSelector* selector = CSipMetaCprSelector::NewL(aSelectionPreferences);
	CSipMetaCprSelector* sipselector = static_cast<CSipMetaCprSelector* >(selector);
	sipselector->SetTierManager(this);
	return selector;
	}

void CSipTierManager::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
	TNodeContext<CSipTierManager> ctx(*this, aMessage, aSender, aRecipient);
    CCoreTierManager::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}

void CSipTierManager::ConstructL()
	{
	//create the TransitionEngineManager here
	iTransitionEngineManager = new (ELeave) CTransitionEngineMgr;
	CCoreTierManager::ConstructL();
	}

CTransitionEngineMgr* CSipTierManager::TransitionEngineManager()
	{
	return iTransitionEngineManager;
	}
	
CSipTierManager::~CSipTierManager()
	{
	delete iTransitionEngineManager;		
	iTransitionEngineManager = NULL;
	}


