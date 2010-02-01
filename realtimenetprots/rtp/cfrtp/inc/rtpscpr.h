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
// RTP SubConnection Provider class definition.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_RTPSCPR_H
#define SYMBIAN_RTPSCPR_H

#include <comms-infras/ss_subconnprov.h>
#include <comms-infras/corescpr.h>
#include <comms-infras/corescprstates.h>

#include <cs_subconevents.h>
#include <cs_subconparams.h>


#if defined(_DEBUG) || defined(__RTP_LOG_ACTIVE)
#define KRTPSCprTag  KESockSubConnectionTag
_LIT8(KRTPSCprSubTag, "RtpScpr");
#endif

_LIT(KRtpScprPanic, "RtpScprPanic");

namespace RtpSCprStates
    {
	class TNoTagOrErrorTag;
    class TCreateDataClient;
    class TSetParameters;
    class TRequestCommsBinder;
    class TAwaitingBinderRequest;
    }

NONSHARABLE_CLASS(CRtpSubConnectionProvider) : public CCoreSubConnectionProvider
/** RTP Subconnection provider. Created as a result of CSubConRTPGenericParamSet 
    parameter setting on subconnection. 

@internalTechnology
*/
    {
    friend class CRtpSubConnectionProviderFactory;  
    friend class RtpSCprStates::TNoTagOrErrorTag;
    friend class RtpSCprStates::TCreateDataClient;
    friend class RtpSCprStates::TSetParameters;
    friend class RtpSCprStates::TRequestCommsBinder;
    friend class RtpSCprStates::TAwaitingBinderRequest;
    
public:
	virtual ~CRtpSubConnectionProvider();
	inline ESock::RCFParameterFamilyBundle_const& GetLocalParameterBundle()
		{
		return iLocalParameterBundle;
		}
	ESock::RCFParameterFamilyBundle& GetOrCreateLocalParameterBundleL();
protected:
    //-====================================
    //Construction methods bundle - accessible thru the factory only
    //-====================================
    CRtpSubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory);
	static CRtpSubConnectionProvider* NewL(ESock::CSubConnectionProviderFactoryBase& aFactory);
	void ConstructL();
	
    //-====================================
    //ACFNode overrides
    //-====================================
 	void Received(MeshMachine::TNodeContextBase& aContext);
 	void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
    
private:
   	//RTP param stored here
	TInt								iRTPProtocolID;			
	TUid								iRTPProtocolUiD;
	TInt								iServiceProtocolID;
	RSubConnection::TSubConnType		iSubConnType;
	ESock::RCFParameterFamilyBundle     iLocalParameterBundle;
    };


#endif //SYMBIAN_RTPSCPR_H
