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
// rtpscprfactory.cpp
// RTP SubConnection Provider factory class definition.
// 
//

/**
 @file
 @internalComponent
*/

#include "dummy_rtpscprfactory.h"

#include "dummy_rtpscpr.h"

#include <comms-infras/ss_log.h>

//#include <comms-infras/msgintercept_std.h>


using namespace ESock;

//-=========================================================
//
// CRtpSubConnectionProviderFactory methods
//
//-=========================================================	
CRtpDummySubConnectionProviderFactory* CRtpDummySubConnectionProviderFactory::NewL(TAny* aParentContainer)
    {
    
    return new (ELeave) CRtpDummySubConnectionProviderFactory(TUid::Uid(CRtpDummySubConnectionProviderFactory::iUid), 
                                            *reinterpret_cast<ESock::CSubConnectionFactoryContainer*>(aParentContainer));
    }
CRtpDummySubConnectionProviderFactory::CRtpDummySubConnectionProviderFactory(TUid aFactoryId, ESock::CSubConnectionFactoryContainer& aParentContainer)
	: CSubConnectionProviderFactoryBase(aFactoryId, aParentContainer)
    {
//	LOG_NODE_CREATE(KRtpSCprFactoryTag, CRtpDummySubConnectionProviderFactory);
    }

CSubConnectionProviderBase& CRtpDummySubConnectionProviderFactory::DoCreateL(TSubConnOpen::TSubConnType aSubConnType)
    {
    (void)aSubConnType;
    CSubConnectionProviderBase* provider(NULL);
    
    /* Assert only in debug mode. This is not a very serious problem. Let go in Release mode */
    __ASSERT_DEBUG(aSubConnType == RSubConnection::ECreateNew, User::Panic(KRtpScprPanic, CorePanics::KPanicPeerMisbehaving));
    
    provider = CRtpDummySubConnectionProvider::NewL(*this);
    return *provider;
    }

/* The destructor is not needed as such. 
CRtpSubConnectionProviderFactory::~CRtpSubConnectionProviderFactory()
    {
    LOG_NODE_DESTROY(KRtpSCprFactoryTag, CRtpSubConnectionProviderFactory);
    }
*/
