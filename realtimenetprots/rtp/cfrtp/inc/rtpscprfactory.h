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
// RTP SubConnection Provider factory class definition.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_RTPSCPR_FACTORY_H
#define SYMBIAN_RTPSCPR_FACTORY_H

#include <comms-infras/ss_subconnprov.h>
#include <rtp_uid.h>

class CRtpSubConnectionProviderFactory : public ESock::CSubConnectionProviderFactoryBase
/** RTP Subconnection provider factory. 

@internalTechnology
*/
	{
public:
    enum { iUid = KRtpScprFactoryImplementationUid };
	static CRtpSubConnectionProviderFactory* NewL(TAny* aParentContainer);
	
protected:
	CRtpSubConnectionProviderFactory(TUid aFactoryId, ESock::CSubConnectionFactoryContainer& aParentContainer);
	virtual ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};

#endif
//SYMBIAN_RTPSCPR_FACTORY_H
