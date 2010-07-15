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
// RTCPFlowFactory Implementation
// 
//

/**
 @file
 @internalComponent
*/

#include <ss_glob.h>
#include <rtp_proto_id.h>
#include "rtpflowfactory.h"
#include "rtpflow.h"
#include "rtcpflow.h"
#include "rtppint.h"
#include "rtp_cfmessages.h"

#include "rtpflowfactory.h"

using namespace ESock;

CRtpFlowFactory* CRtpFlowFactory::NewL(TAny* aConstructionParameters)
	{
	CRtpFlowFactory* ptr = new (ELeave) CRtpFlowFactory(TUid::Uid(KRtpFlowImplementationUid), *(reinterpret_cast<CSubConnectionFlowFactoryContainer*>(aConstructionParameters)));
	CleanupStack::PushL(ptr);
	CleanupStack::Pop();
	return ptr;
	}

CRtpFlowFactory::CRtpFlowFactory(TUid aFactoryId, CSubConnectionFlowFactoryContainer& aParentContainer)
	: CSubConnectionFlowFactoryBase(aFactoryId, aParentContainer)
	{
	}
	

CProtocolIntfFactoryBase* CRtpFlowFactory::CreateProtocolIntfFactoryL(ESock::CProtocolIntfFactoryContainer& aParentContainer)
    {
    CProtocolIntfFactoryBase* factory = CRtpProtocolIntfFactory::NewL(Uid(), aParentContainer);
    return factory;
    }

ACommsFactoryNodeId* CRtpFlowFactory::DoFindOrCreateObjectL(TFactoryQueryBase& aQuery)
/** Default implementation to create a factory object. This may be overridden by a derived class
*/
	{
	ACommsFactoryNodeId *factoryObject = 0;
	factoryObject = CreateObjectL(aQuery);
	return factoryObject;		
	}

/* Check the MessageID and Create the appropriate Flow */
CSubConnectionFlowBase* CRtpFlowFactory::DoCreateFlowL(CProtocolIntfBase* aProtocolIntf, TFactoryQueryBase& aQuery)
	{
	CSubConnectionFlowBase* flow = 0;
	const TDefaultFlowFactoryQuery& query  = static_cast < const TDefaultFlowFactoryQuery& > (aQuery);
	TInt protocolID = (TInt)query.iProtocolType;
	
	if(KProtocolRtp == protocolID)
		{
		flow = CRtpFlow::NewL(*this, query.iSCprId, aProtocolIntf);
		}
	else if(KProtocolRtcp == protocolID)
		{
		flow = CRtcpFlow::NewL(*this, query.iSCprId, aProtocolIntf);
		}
	
	return flow;
	}

TServerProtocolDesc* CRtpFlowFactory::DoCreateFlowDescriptionL(TInt aProtocol)
	{
 	TServerProtocolDesc* protocolDescription = NULL;
     switch(aProtocol)
     	{
        case KProtocolRtp:
             protocolDescription = new(ELeave) TServerProtocolDesc();
             protocolDescription->iName        = KProtocolNameRtp;
             protocolDescription->iAddrFamily  = KAfInet;
             protocolDescription->iSockType    = KSockDatagram;
             protocolDescription->iProtocol    = KProtocolRtp;
             protocolDescription->iVersion     = TVersion(2,0,1);
             protocolDescription->iByteOrder   = EBigEndian;
             protocolDescription->iServiceInfo     = KSIConnectionLess | KSIDatagram |
                                     KSIGracefulClose | KSIPeekData |
                                     KSIRequiresOwnerInfo;
             protocolDescription->iNamingServices      = 0;
             protocolDescription->iSecurity    = KSocketNoSecurity;
             protocolDescription->iMessageSize     = 65536-128; /*KSocketMessageSizeUndefined;*/
             protocolDescription->iServiceTypeInfo  = ESocketSupport | ETransport |
                                     EPreferMBufChains | ENeedMBufs |
                                     EUseCanSend;
             protocolDescription->iNumSockets      = KUnlimitedSockets;
             break;
	
        case KProtocolRtcp:
             protocolDescription = new(ELeave) TServerProtocolDesc();
             protocolDescription->iName        = KProtocolNameRtcp;
             protocolDescription->iAddrFamily  = KAfInet;
             protocolDescription->iSockType    = KSockDatagram;
             protocolDescription->iProtocol    = KProtocolRtcp;
             protocolDescription->iVersion     = TVersion(2,0,1);
             protocolDescription->iByteOrder   = EBigEndian;
             protocolDescription->iServiceInfo     = KSIConnectionLess | KSIDatagram |
                                     KSIGracefulClose | KSIPeekData |
                                     KSIRequiresOwnerInfo;
             protocolDescription->iNamingServices      = 0;
             protocolDescription->iSecurity    = KSocketNoSecurity;
             protocolDescription->iMessageSize     = 65536-128; /*KSocketMessageSizeUndefined;*/
             protocolDescription->iServiceTypeInfo  = ESocketSupport | ETransport |
                                     EPreferMBufChains | ENeedMBufs |
                                     EUseCanSend;
             protocolDescription->iNumSockets      = KUnlimitedSockets;
             break;
		}
	return protocolDescription;
	}

