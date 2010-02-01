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
// RTP SCPR Factory Implementation
// 
//

/**
 @file
 @internalComponent
*/



#include <comms-infras/ss_log.h>

#include "rtpscprfactory.h"
#include "rtpscpr.h"

using namespace ESock;
using namespace Messages;
using namespace Factories;

//-=========================================================
//
// CRtpSubConnectionProviderFactory methods
//
//-=========================================================	
CRtpSubConnectionProviderFactory* CRtpSubConnectionProviderFactory::NewL(TAny* aParentContainer)
    {
    
    return new (ELeave) CRtpSubConnectionProviderFactory(TUid::Uid(CRtpSubConnectionProviderFactory::iUid), 
                                            *reinterpret_cast<ESock::CSubConnectionFactoryContainer*>(aParentContainer));
    }
CRtpSubConnectionProviderFactory::CRtpSubConnectionProviderFactory(TUid aFactoryId, ESock::CSubConnectionFactoryContainer& aParentContainer)
	: CSubConnectionProviderFactoryBase(aFactoryId, aParentContainer)
    {
    }
ESock::ACommsFactoryNodeId* CRtpSubConnectionProviderFactory::DoCreateObjectL(ESock::TFactoryQueryBase& )
    {
    CSubConnectionProviderBase* provider(NULL);
	
	provider = CRtpSubConnectionProvider::NewL(*this);
    return provider;
    }
