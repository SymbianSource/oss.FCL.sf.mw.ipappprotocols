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
// RTP Flowfactory definitions.
// 
//

/**
 @file
 @internalComponent
*/


#ifndef RTPFLOWFACTORY_H_
#define RTPFLOWFACTORY_H_



#include <e32def.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

#include <comms-infras/ss_subconnflow.h>

#include "rtp_uid.h"

using namespace ESock;
using namespace Factories;


_LIT(KProtocolNameRtp, "RTP");
_LIT(KProtocolNameRtcp, "RTCP");

class CRtpFlowFactory : public CSubConnectionFlowFactoryBase
	/**
	Factory for creation RTP Flow objects.

	@internalComponent 
	
	**/				    
	{
public:
	enum { iUid = KRtpFlowImplementationUid };
   	static CRtpFlowFactory* NewL(TAny* aConstructionParameters);
	//~CRtpFlowFactory();
protected:
	CRtpFlowFactory(TUid aFactoryId,CSubConnectionFlowFactoryContainer& aParentContainer);
	
	ACommsFactoryNodeId* DoFindOrCreateObjectL(TFactoryQueryBase& aQuery);
	
	CSubConnectionFlowBase* DoCreateFlowL(CProtocolIntfBase* aProtocolIntf, TFactoryQueryBase& aQuery);
	
	/* Create RTP PINT factory */
	CProtocolIntfFactoryBase* CreateProtocolIntfFactoryL(CProtocolIntfFactoryContainer& aParentContainer);
	
	/* RTP Flow factory description */
	virtual TServerProtocolDesc* DoCreateFlowDescriptionL(TInt aProtocol);
	};

#endif //RTPFLOWFACTORY_H

