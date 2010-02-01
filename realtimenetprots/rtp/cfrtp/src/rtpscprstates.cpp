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
// RTP SubConnection Provider states/transitions
// 
//

/**
 @file
 @internalComponent
*/

#include <comms-infras/corescpractivities.h>
#include <comms-infras/ss_tiermanager.h>
#include <ss_glob.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <cflog.h>

#include <rtp_uid.h>
#include <rtp_subconparams.h>
#include "rtpscprstates.h"
#include <rtp_subconparams.h>
#include "rtpprovisioninfo.h" 
#include "rtp_cfmessages.h"


using namespace ESock;
using namespace NetStateMachine;
using namespace RtpSCprStates;
using namespace Messages;


// Modified to do the following
// 1.If service provider does not exist, check if RTP
// 2.1If yes, then send commsBinder Request to Ctrl Provider, if not RTP (ie IP) panic as in original scheme
// 3.If service provider exists => stacking has already been done, work as original scheme
DEFINE_SMELEMENT(TSendCommsBinderToCntrlProv, NetStateMachine::MStateTransition,RtpSCprStates::TContext)
void TSendCommsBinderToCntrlProv::DoL()
	{
	RNodeInterface* ctrlProvider = iContext.Node().ControlProvider();
	ESock::TCFServiceProvider::TCommsBinderRequest commsBinderReq(RSubConnection::EAttachToDefault);
	iContext.iNodeActivity->PostRequestTo(*ctrlProvider,commsBinderReq);
	}

DEFINE_SMELEMENT(TNoTagOrErrorTag, NetStateMachine::MStateFork, RtpSCprStates::TContext)
TInt TNoTagOrErrorTag::TransitionTag()
	{
	Messages::TEBase::TError* msg = message_cast<Messages::TEBase::TError>(&iContext.iMessage);
	if (msg)
		{
		iContext.iNodeActivity->SetError(msg->iValue);
		}
	/* Now that was not an Error Message so check if it the response to some
	   message we know of. */
	TRtpMessages::TRtpMessageReadyForBind* prov = message_cast<TRtpMessages::TRtpMessageReadyForBind>(&iContext.iMessage);
	if (prov)
		{
		if(prov->iValue != KErrNone)
			{
			iContext.iNodeActivity->SetError(prov->iValue);
			return MeshMachine::KErrorTag | NetStateMachine::EForward;
			}
		else
			{
			return MeshMachine::KNoTag | NetStateMachine::EForward;	
			}
		}
	/* Activity not in an error due to the message we received */
	if (KErrNone==iContext.iNodeActivity->Error())
		{
		return MeshMachine::KNoTag | NetStateMachine::EForward;
		}
	
	__CFLOG_0(KRTPSCprTag, KRTPSCprSubTag, _L8("TNoTagOrErrorTag:Returning Error Tag"));
	/* Default */
	return MeshMachine::KErrorTag | NetStateMachine::EForward;
	}


/* Overriden TCommsBinderRequest. Transition will send comms-binder with ECreateNew to IpDefScpr. */
DEFINE_SMELEMENT(TRequestCommsBinder, NetStateMachine::MStateTransition,RtpSCprStates::TContext)
void TRequestCommsBinder::DoL()
	{
	RNodeInterface* sp = iContext.Node().ServiceProvider();
	
    __ASSERT_DEBUG(sp , User::Panic(KRtpScprPanic, CorePanics::KPanicNoServiceProvider));
	iContext.iNodeActivity->PostRequestTo(*sp,TCFServiceProvider::TCommsBinderRequest(RSubConnection::ECreateNew,iContext.Node().GetLocalParameterBundle()).CRef());
	}

/* State which will accept the ProvisionDone message from RTP Flow. */
DEFINE_SMELEMENT(TAwaitingReadyForBind, NetStateMachine::MState,RtpSCprStates::TContext)
TBool TAwaitingReadyForBind::Accept()
	{
	return iContext.iMessage.IsMessage<TRtpMessages::TRtpMessageReadyForBind>();
	}
	
	
DEFINE_SMELEMENT(TSetPostedToFlow, NetStateMachine::MStateTransition,RtpSCprStates::TContext)
void TSetPostedToFlow::DoL()
	{
	RNodeInterface* client = iContext.iNode.FindClient(message_cast<TCFFactory::TPeerFoundOrCreated>(iContext.iMessage).iNodeId);
	/* No need for explicit check. We will not be here if there is no client anyways */
	iContext.iNodeActivity->SetPostedTo(client->RecipientId());	
	}


DEFINE_SMELEMENT(TAwaitingBinderRequest, NetStateMachine::MState, RtpSCprStates::TContext)
TBool TAwaitingBinderRequest::Accept()
	{
	if (! iContext.iMessage.IsMessage<ESock::TCFServiceProvider::TCommsBinderRequest>())
		{
		return EFalse;
		}
	
	/* Now extract the FlowParams (if present) and store it for sending it along with NoBearer
	   to IpDefScpr */
	ESock::TCFServiceProvider::TCommsBinderRequest& msg = message_cast<ESock::TCFServiceProvider::TCommsBinderRequest>(iContext.iMessage);

	/* Only one type of bundle is supported( Whatever is used for RTP will be used by RTCP.
      If they are different it is an error case [TO BE DONE] Handle these */
    if(msg.iFamilyBundle.IsNull())
    	{
    	return EFalse;
    	}
    else
    	{
    	return ETrue;
    	}
	}
	
	
DEFINE_SMELEMENT(TCreateDataClient, NetStateMachine::MStateTransition,RtpSCprStates::TContext)    
void TCreateDataClient::InitLocalParameterBundleL(CSubConRTPGenericParamSet* aReqGenericParams)
	{
	ESock::RCFParameterFamilyBundle &localBundle = iContext.Node().GetOrCreateLocalParameterBundleL();
	
	/* Also add a TFlowParams to the LocalParametrBundle. This will be used by the lower
	 * SCPR to create the appropriate flows for the APL Protocol 
	 */
	TFlowParams flowParams;
	/* Currently some values are hardcoded. this will be removed when full migration to
	 * Dynamic Stacking is done 
	 */
	flowParams.iProtocol   = aReqGenericParams->GetServiceProtocolID();
	flowParams.iAddrFamily = KAfInet;
	flowParams.iSocketType = KSockDatagram; //Or ProtocolManager::FindProtocolL will Leave
	
	CFlowRequestParameters* pFlowParams = CFlowRequestParameters::NewL();
	CleanupStack::PushL(pFlowParams);
	pFlowParams->SetFlowParams(flowParams);

	RParameterFamily family = localBundle.CreateFamilyL(KFlowParametersFamily);
	family.AddParameterSetL(pFlowParams, RParameterFamily::ERequested);
	CleanupStack::Pop(pFlowParams);
	}

void TCreateDataClient::DoL()
	{
	ESock::TCFServiceProvider::TCommsBinderRequest* binderReq = message_cast<ESock::TCFServiceProvider::TCommsBinderRequest>(&iContext.iMessage);
	__ASSERT_DEBUG(binderReq, User::Panic(KRtpScprPanic, CorePanics::KPanicIncorrectMessage));
   	
	/* Parameter bundle is must. Without that RtpScpr should'nt have been created */
	__ASSERT_DEBUG(!iContext.Node().iParameterBundle.IsNull(), User::Panic(KRtpScprPanic, CorePanics::KPanicPeerMisbehaving));
	
	RParameterFamily parameterFamily =	iContext.Node().iParameterBundle.FindFamily(KProtocolExtensionFamily);

    if(parameterFamily.IsNull())
    	{
 		__CFLOG_0(KRTPSCprTag, KRTPSCprSubTag, _L8("TCreateDataClient IncorrectParameterBundle"));
		User::Leave(KErrNotSupported);
    	}
    	
	STypeId typeId = STypeId::CreateSTypeId(CSubConRTPParamFactory::iUid, KSubConnRTPGenericParamsType);
	CSubConRTPGenericParamSet* reqGenericParams = static_cast<CSubConRTPGenericParamSet*>(parameterFamily.FindParameterSet(typeId,RParameterFamily::ERequested));
	if(0 == reqGenericParams)
		{
		__CFLOG_0(KRTPSCprTag, KRTPSCprSubTag, _L8("TCreateDataClient IncorrectParameterBundle"));
		User::Leave(KErrNotSupported);
		}		
	
	/* Error cases */
	if( reqGenericParams->GetServiceProtocolID() != KProtocolInetUdp ||
	    reqGenericParams->GetLowerSubConnType() != RSubConnection::EAttachToDefault ||
	    reqGenericParams->GetProtocolUiDRtp() != TUid::Uid(KRtpFlowImplementationUid))
 		{
 		__CFLOG_0(KRTPSCprTag, KRTPSCprSubTag, _L8("TCreateDataClient IncorrectArguments"));
 		User::Leave(KErrNotSupported);
	 	}

	/* There can be only Two DataFlows per SCPR. Should leave if this condition fails */
	TInt numberOfDefaultDataClient = iContext.Node().CountClients<TDefaultClientMatchPolicy>(ESock::TCFClientType::EData);
	if(numberOfDefaultDataClient >= 2)
		{
		__CFLOG_0(KRTPSCprTag, KRTPSCprSubTag, _L8("TCreateDataClient: Tried to create third flow <ERROR>"));
		User::Leave(KErrNotSupported);
		}

	Messages::TNodeId factoryContainer;
    factoryContainer = SockManGlobals::Get()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::EDataPlane));
    
    __ASSERT_DEBUG(iContext.Node().ControlProvider(), User::Panic(KRtpScprPanic, CorePanics::KPanicNoControlProvider));
    
    Messages::TNodeId controlProvider = iContext.Node().ControlProvider()->RecipientId();			
    TSubConnOpen::TSubConnType protocolType = TSubConnOpen::EAttachToDefault;
	
	// Determine what protocol we want the flow factory to create
	if(binderReq && ! binderReq->iFamilyBundle.IsNull())
		{
		// Find the flow parameter family of parameters
		RParameterFamily parameterFamily = binderReq->iFamilyBundle.FindFamily(KFlowParametersFamily);

		if(!parameterFamily.IsNull())
			{
			STypeId typeId = STypeId::CreateSTypeId(CFlowRequestParameters::EUid, CFlowRequestParameters::EType);
			CFlowRequestParameters* flowParams =	static_cast<CFlowRequestParameters*>(parameterFamily.FindParameterSet(typeId, RParameterFamily::ERequested));

			// The type of flow created by the flow factory is dependent only on the protocol type
			protocolType = (TSubConnOpen::TSubConnType)flowParams->GetFlowParams().iProtocol;
			}
		}
		
	TDefaultFlowFactoryQuery dataClientQuery( controlProvider, iContext.NodeId(),iContext.iMessage.MessageId(), protocolType);
	    
	TCFFactory::TFindOrCreatePeer msg(TCFPlayerRole::EDataPlane, reqGenericParams->GetProtocolUiDRtp(),&dataClientQuery);
		
	iContext.iNodeActivity->PostRequestTo(factoryContainer, msg);


	if(0 == numberOfDefaultDataClient)
		{

		__CFLOG_0(KRTPSCprTag, KRTPSCprSubTag, _L8("TCreateDataClient Creating Data Flow (1)."));
		
		InitLocalParameterBundleL(reqGenericParams);
		
	    /* If a Second subconnection is Opened. Then the existing Config should
		 * be removed */	

	    RMetaExtensionContainer mec;
		mec.Open(iContext.Node().AccessPointConfig());
		CleanupClosePushL(mec);
		
		/* Now add the RTP Creation parameters to the AccessPointconfig() */
	    CRtpProvisionConfig* rtpProvision = CRtpProvisionConfig::NewLC();
	    
	    rtpProvision->SetBandWidth(reqGenericParams->BandWidth());
	    rtpProvision->SetRtptimeConversion(reqGenericParams->RtptimeConversion());
	    rtpProvision->SetRtpAutoSend(reqGenericParams->RtpAutoSend());
	    rtpProvision->SetRtpTimeNow(reqGenericParams->RtpTimeNow());
	    rtpProvision->SetCNAMEL(reqGenericParams->GetCNAME());
	    rtpProvision->SetDefDestinationAddr(reqGenericParams->GetDefDestinationAddr());
	    rtpProvision->SetMaxDropOut(reqGenericParams->MaxDropOut());
	    rtpProvision->SetMaxMisorder(reqGenericParams->MaxMisorder());
	    rtpProvision->SetMinSequential(reqGenericParams->MinSequencial());
		rtpProvision->SetRtcpReportInterval(reqGenericParams->GetRtcpReportInterval());
	    
		mec.AppendExtensionL(rtpProvision);
		
	    /* Ownership no longer with us */
	    CleanupStack::Pop();
		
		iContext.Node().AccessPointConfig().Close();
		iContext.Node().AccessPointConfig().Open(mec);
		CleanupStack::PopAndDestroy(&mec);
		}
	else
		{
		__CFLOG_0(KRTPSCprTag, KRTPSCprSubTag, _L8("TCreateDataClient Creating Data Flow (2)."));
		}
	}
