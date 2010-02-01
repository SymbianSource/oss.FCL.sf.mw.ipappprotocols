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
// ECOM implementation for the SIP connection and sub-connection provider factories
// SIP TierManagerFactory class definition
// 
//

/**
 @file
 @internalComponent
*/


#include <ecom/implementationproxy.h>
#include "sipscprfactory.h"
#include "sipcprfactory.h"
#include "sipmcprfactory.h"
#include "siptiermanagerfactory.h"
#include "SIP_subconparams.h"
#include "SIP_subconevents.h"

using namespace ESock;

/**
Data required for instantiating ECOM Plugin
*/
	
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(CSipConnectionProviderFactory::iUid, CSipConnectionProviderFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(CSipMetaConnectionProviderFactory::iUid, CSipMetaConnectionProviderFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(CSipSubConnectionProviderFactory::iUid, CSipSubConnectionProviderFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(CSipTierManagerFactory::iUid, CSipTierManagerFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KSubConSIPParametersUid, CSIPSubConnExtensionParamsFactory::NewL),
  	IMPLEMENTATION_PROXY_ENTRY(KSubConSIPEventsUid, CSIPSubConnExtensionEventsFactory::NewL)
	};

/**
ECOM Implementation Factory
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
   }

