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
// SIP Meta Conneciton Provider factory definition
// 
//

/**
 @file
 @internalComponent
*/

#include "sipmcprfactory.h"
#include "sipmcpr.h"
#include <comms-infras/ss_log.h>
#include <ss_glob.h>
#include <ecom/ecom.h>
#include <comms-infras/ss_msgintercept.h>


#ifdef __CFLOG_ACTIVE
#define KSipMCprFactoryTag KESockMetaConnectionTag
_LIT8(KSipMCprFactorySubTag, "sipmcprfactory");
#endif

using namespace ESock;

CSipMetaConnectionProviderFactory* CSipMetaConnectionProviderFactory::NewL(TAny* aParentContainer)
/** Factory function for the factory which manages SIP level meta connection providers.
This function also acts as the single ECom entry point into this object.
@param aParentContainer the parent factory container which owns this factory
@return factory for SIP level meta connection providers
*/
	{
	__CFLOG_VAR((KSipMCprFactoryTag, KSipMCprFactorySubTag, _L8("CSipMetaConnectionProviderFactory ::\tNewL(%08x)"), aParentContainer));
 	return new (ELeave) CSipMetaConnectionProviderFactory(TUid::Uid(CSipMetaConnectionProviderFactory::iUid), *(reinterpret_cast<CMetaConnectionFactoryContainer*>(aParentContainer)));
	}

CSipMetaConnectionProviderFactory::CSipMetaConnectionProviderFactory(TUid aFactoryUid, CMetaConnectionFactoryContainer& aParentContainer)
	: CMetaConnectionProviderFactoryBase(aFactoryUid,aParentContainer)
/** Constructor for SIP level meta connection providers.
@param aFactoryId the ID which this factory can be looked up by
@param aParentContainer the parent factory container which owns this factory
*/
	{
//	LOG_NODE_CREATE(KSipMCprFactoryTag, CSipMetaConnectionProviderFactory);
	}

ACommsFactoryNodeId* CSipMetaConnectionProviderFactory::DoCreateObjectL(TFactoryQueryBase& aQuery)
	{
   	const TMetaConnectionFactoryQuery& query = static_cast<const TMetaConnectionFactoryQuery&>(aQuery);
    CMetaConnectionProviderBase* provider = CSipMetaConnectionProvider::NewL(*this,query.iProviderInfo);
	ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);
	return provider; 
	}


