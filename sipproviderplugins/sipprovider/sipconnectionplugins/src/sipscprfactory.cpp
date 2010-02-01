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
// SIP SubConnection Provider factory class definition.
// 
//

/**
 @file
 @internalComponent
*/

//#include <ecom/implementationproxy.h>
#include <comms-infras/ss_msgintercept.h>
#include "sipscprfactory.h"
#include "sipscpr.h"
#include "sipdeftscpr.h"

using namespace ESock;


//-=========================================================
//
// CSipSubConnectionProviderFactory methods
//
//-=========================================================	
CSipSubConnectionProviderFactory* CSipSubConnectionProviderFactory::NewL(TAny* aParentContainer)
    {
    return new (ELeave) CSipSubConnectionProviderFactory(TUid::Uid(CSipSubConnectionProviderFactory::iUid), 
                                            *reinterpret_cast<ESock::CSubConnectionFactoryContainer*>(aParentContainer));
    }
    
CSipSubConnectionProviderFactory::CSipSubConnectionProviderFactory(TUid aFactoryId, ESock::CSubConnectionFactoryContainer& aParentContainer)
	: CSubConnectionProviderFactoryBase(aFactoryId, aParentContainer)
    {
//    LOG_NODE_CREATE(KSipSCprTag, CSipSubConnectionProviderFactory);
    }

ACommsFactoryNodeId* CSipSubConnectionProviderFactory::DoCreateObjectL(TFactoryQueryBase& aQuery)
    {
	const TDefaultSCPRFactoryQuery& query = static_cast<const TDefaultSCPRFactoryQuery&>(aQuery);
    CSubConnectionProviderBase* provider = NULL;
    if (query.iSCPRType == RSubConnection::EAttachToDefault)
        {        
        provider = CSipDefaultSubConnectionProvider::NewL(*this);
        }    
    else 
    	{
     	provider = CSipSubConnectionProvider::NewL(*this, query.iSCPRType);   
        }    
    ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);	
    return provider;
    }
    
    

