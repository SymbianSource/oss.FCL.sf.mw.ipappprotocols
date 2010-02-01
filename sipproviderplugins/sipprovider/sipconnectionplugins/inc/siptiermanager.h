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
// SIPTIERMANAGER_H.H
// SIP Tier Manager class.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_SIP_TIER_MANAGER_H
#define SYMBIAN_SIP_TIER_MANAGER_H

#include <comms-infras/coretiermanager.h>
#include "TransitionEngineMgr.h"

class CSipTierManager : public CCoreTierManager
	{
public:
	~CSipTierManager();
	static CSipTierManager* NewL(ESock::CTierManagerFactoryBase& aFactory);
	CTransitionEngineMgr* TransitionEngineManager();
	

protected:
	CSipTierManager(ESock::CTierManagerFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap);
	virtual ESock::MProviderSelector* DoCreateProviderSelectorL(const Meta::SMetaData& aSelectionPreferences);
    virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
	void ConstructL();
	
	//Transition Engine Manager maintained in Sip TierMgr as it a single point of reference
    CTransitionEngineMgr* iTransitionEngineManager;
	};

#endif //SYMBIAN_SIP_TIER_MANAGER_H

