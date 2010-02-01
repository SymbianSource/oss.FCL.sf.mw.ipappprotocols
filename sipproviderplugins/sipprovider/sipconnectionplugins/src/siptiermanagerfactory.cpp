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
// SIP TierManagerFactory class definition
// This is part of an ECOM plug-in
// 
//

/**
 @file
 @internalComponent
*/

#include "siptiermanagerfactory.h"
#include "siptiermanager.h"
#include <comms-infras/ss_log.h>
#include <ecom/ecom.h>

#ifdef __CFLOG_ACTIVE
#define KSipTierMgrTag KESockMetaConnectionTag
_LIT8(KSipTierMgrSubTag, "siptiermanager");
#endif

using namespace ESock;

CSipTierManagerFactory* CSipTierManagerFactory::NewL(TAny* aParentContainer)
	{
	__CFLOG_VAR((KSipTierMgrTag, KSipTierMgrSubTag, _L8("CSipTierManagerFactory::\tNewL(%08x)"), aParentContainer));
 	return new (ELeave) CSipTierManagerFactory(TUid::Uid(CSipTierManagerFactory::iUid),TUid::Uid(CSipTierManagerFactory::iUid),*(reinterpret_cast<CTierManagerFactoryContainer*>(aParentContainer)));
	}

CSipTierManagerFactory::CSipTierManagerFactory(TUid aTierTypeId, TUid aFactoryUid, CTierManagerFactoryContainer& aParentContainer)
	: CTierManagerFactoryBase(aTierTypeId,aFactoryUid,aParentContainer)
	{
	__CFLOG_VAR((KSipTierMgrTag, KSipTierMgrSubTag, _L8("CSipTierManagerFactory %08x:\tCSipTierManagerFactory"), this));
	}

ACommsFactoryNodeId* CSipTierManagerFactory::DoCreateObjectL(TFactoryQueryBase& /* aQuery */)
	{
	return CSipTierManager::NewL(*this);
	}

