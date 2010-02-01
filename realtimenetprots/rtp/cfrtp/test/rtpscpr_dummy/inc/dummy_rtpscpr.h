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
// rtpscpr.h
// RTP SubConnection Provider class definition.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __DUMMY_RTPSCPR_H__
#define __DUMMY_RTPSCPR_H__

#include <comms-infras/ss_subconnprov.h>
#include <comms-infras/corescpr.h>
#include <comms-infras/corescprstates.h>

#include <cs_subconevents.h>
#include <cs_subconparams.h>


_LIT8(KRTPSCprTag, "DummyRtpScpr");
_LIT (KRtpScprPanic,"RtpScprPanic");

namespace DummyRtpSCprStates
    {
	class TNoTagOrBearerPresentOrErrorTag;
	class TNoTagOrErrorTag;
    class TCreateDataClient;
    class TSetParameters;
    class TCheckParamsAndDoSomething;    
    class TRequestCommsBinder;
    class TAwaitingBinderRequest;
    }

class CRtpDummySubConnectionProvider : public CCoreSubConnectionProvider
/** Non-default/reserved IP subconnection provider

@internalTechnology
@released Since 9.4 */
    {
    friend class CRtpDummySubConnectionProviderFactory;  
    friend class DummyRtpSCprStates::TNoTagOrBearerPresentOrErrorTag;
	friend class DummyRtpSCprStates::TNoTagOrErrorTag;
    friend class DummyRtpSCprStates::TCreateDataClient;
    friend class DummyRtpSCprStates::TSetParameters;
    friend class DummyRtpSCprStates::TCheckParamsAndDoSomething;
    friend class DummyRtpSCprStates::TRequestCommsBinder;
    friend class DummyRtpSCprStates::TAwaitingBinderRequest;
    
public:
	virtual ~CRtpDummySubConnectionProvider();


protected:
    //-====================================
    //Construction methods bundle - accessible thru the factory only
    //-====================================
    CRtpDummySubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory);
	static CRtpDummySubConnectionProvider* NewL(ESock::CSubConnectionProviderFactoryBase& aFactory);
	void ConstructL();
	
    //-====================================
    //ACFNode overrides
    //-====================================    
    //virtual ESock::RNodeInterface* NewClientInterfaceL(TInt aClientType);
    void Received( MeshMachine::TNodeContextBase& aContext );
    void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
    
private:
   	//RTP param stored here
	TInt								iRTPProtocolID;			
	TUid								iRTPProtocolUiD;
	TInt								iServiceProtocolID;
	RSubConnection::TSubConnType		iSubConnType;
	
	/* Basically a count of the number of times TCreateDataClient was
	   called. If the count is zero the request is send for an RTP Flow.
	   Else an RTCP flow is created. A third request will result in a Leave(KErrNotSupported).
	   We cud have used the Count of DataClients here. But the flow requests are Async and two
	   maybe in Service at the same time. */
	TInt                               iNumFlowrequestServiced;
	ESock::RCFParameterFamilyBundle_const iBinderRequestParameters;
    };


#endif //__DUMMY_RTPSCPR_H__
