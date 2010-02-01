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
// SIP Connection Provider factory class definition.
// 
//

/**
 @file
 @internalComponent
*/

#include <in_sock.h>
#include <comms-infras/ss_msgintercept.h>
#include "sipcprfactory.h"
#include "sipcpr.h"

using namespace ESock;


//-=========================================================
//
// CSipConnectionProviderFactory methods
//
//-=========================================================	
CSipConnectionProviderFactory* CSipConnectionProviderFactory::NewL(TAny* aParentContainer)
    {
    return new (ELeave) CSipConnectionProviderFactory(TUid::Uid(CSipConnectionProviderFactory::iUid), *reinterpret_cast<ESock::CConnectionFactoryContainer*>(aParentContainer));
    }
    
/**
Constructor of the class
*/
CSipConnectionProviderFactory::CSipConnectionProviderFactory(TUid aFactoryId, ESock::CConnectionFactoryContainer& aParentContainer)
	: ESock::CConnectionProviderFactoryBase(aFactoryId, aParentContainer)
    {
    }
    
/**
Destructor of the class
*/
CSipConnectionProviderFactory::~CSipConnectionProviderFactory()
{	
}

/**
This function creats the SIP Sub-Conneciton provider
*/
ACommsFactoryNodeId* CSipConnectionProviderFactory::DoCreateObjectL(TFactoryQueryBase& /* aQuery */)
    {    
    CConnectionProviderBase* provider = CSipConnectionProvider::NewL(*this);
    ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);
	return provider;
    }
    
