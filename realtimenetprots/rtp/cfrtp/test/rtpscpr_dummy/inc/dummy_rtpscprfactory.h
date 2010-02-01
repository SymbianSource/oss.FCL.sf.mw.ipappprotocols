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
// rtpscprfactory.h
// RTP SubConnection Provider factory class definition.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __DUMMY_RTPSCPR_FACTORY_H__
#define __DUMMY_RTPSCPR_FACTORY_H__

#include <comms-infras/ss_subconnprov.h>
#include "dummy_rtp_uid.h"

class CRtpDummySubConnectionProviderFactory : public ESock::CSubConnectionProviderFactoryBase
	{
public:
    enum { iUid = KDummyRtpScprFactoryImplementationUid };
	static CRtpDummySubConnectionProviderFactory* NewL(TAny* aParentContainer);
	//~CRtpSubConnectionProviderFactory();
	
protected:
	CRtpDummySubConnectionProviderFactory(TUid aFactoryId, ESock::CSubConnectionFactoryContainer& aParentContainer);
	virtual ESock::CSubConnectionProviderBase& DoCreateL(TSubConnOpen::TSubConnType aSubConnType);
	};

#endif
//__DUMMY_RTPSCPR_FACTORY_H__
