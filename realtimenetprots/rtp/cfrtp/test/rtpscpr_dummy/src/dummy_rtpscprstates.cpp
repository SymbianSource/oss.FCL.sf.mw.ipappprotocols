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
#include "dummy_rtp_uid.h"
#include "dummy_rtp_subconparams.h"
#include "dummy_rtpscprstates.h"
#include "rtpprovisioninfo.h" 
#include "rtp_cfmessages.h"


using namespace ESock;
using namespace NetStateMachine;
using namespace DummyRtpSCprStates;



// Modified to do the following
// 1.If service provider does not exist, check if RTP
// 2.1If yes, then send commsBinder Request to Ctrl Provider, if not RTP (ie IP) panic as in original scheme
// 3.If service provider exists => stacking has already been done, work as original scheme
DEFINE_SMELEMENT(TSendCommsBinderToCntrlProv, NetStateMachine::MStateTransition,DummyRtpSCprStates::TContext)
void TSendCommsBinderToCntrlProv::DoL()
	{
	Messages::RNodeInterface* ctrlProvider = iContext.Node().ControlProvider();
	ESock::TCFServiceProvider::TCommsBinderRequest commsBinderReq(RSubConnection::EAttachToDefault);
	iContext.iNodeActivity->PostRequestTo(*ctrlProvider,commsBinderReq);
	}


DEFINE_SMELEMENT(TNoTagOrBearerPresentOrErrorTag, NetStateMachine::MStateFork, DummyRtpSCprStates::TContext)
TInt TNoTagOrBearerPresentOrErrorTag::TransitionTag()
	{
	 if(iContext.Node().ServiceProvider())
	 	{
	 	return MeshMachine::KErrorTag | NetStateMachine::EForward;
	 	}
	 	
	 /* We cannot proceed without RTP Parameter Bundle. 
	 	   Anyway the SCPR was created because the Parameter Bundle was present */
	__ASSERT_DEBUG(iContext.Node().iParameterBundle.IsNull(), User::Panic(KRtpScprPanic, CorePanics::KPanicPeerMisbehaving));
	
	RParameterFamily parameterFamily =	iContext.Node().iParameterBundle.FindFamily(KProtocolExtensionFamily);

    if(parameterFamily.IsNull())
    	{
		return MeshMachine::KNoTag | NetStateMachine::EForward;
    	}
    	
	STypeId typeId = STypeId::CreateSTypeId(static_cast<TUint>(KDummySubConnRTPParamsImplUid), KSubConnDummyRTPGenericParamsType);
	CSubConRTPGenericParamSetDummy* reqGenericParams = static_cast<CSubConRTPGenericParamSetDummy*>(parameterFamily.FindParameterSet(typeId,RParameterFamily::ERequested));
	if(0 == reqGenericParams)
		{
		return MeshMachine::KNoTag | NetStateMachine::EForward;
		}		

	if(reqGenericParams->GetNoBearerFlag())
		{
		iContext.iNodeActivity->SetError(KErrGeneral);
		return MeshMachine::KErrorTag | NetStateMachine::EForward;
		}
		
	return MeshMachine::KNoTag | NetStateMachine::EForward;
	}

DEFINE_SMELEMENT(TNoTagOrErrorTag, NetStateMachine::MStateFork, DummyRtpSCprStates::TContext)
TInt TNoTagOrErrorTag::TransitionTag()
	{
	Messages::TEBase::TError* msg = Messages::message_cast<Messages::TEBase::TError>(&iContext.iMessage);
	if (msg)
		{
		iContext.iNodeActivity->SetError(msg->iValue);
		}
	/* Now that was not an Error Message so check if it the response to some
	   message we know of. */
	TRtpMessages::TRtpMessageReadyForBind* prov = Messages::message_cast<TRtpMessages::TRtpMessageReadyForBind>(&iContext.iMessage);
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
	
	/* Default */
	return MeshMachine::KErrorTag | NetStateMachine::EForward;
	}


/* Overriden TCommsBinderRequest. Transition will send comms-binder with ECreateNew to IpDefScpr. */
DEFINE_SMELEMENT(TRequestCommsBinder, NetStateMachine::MStateTransition,DummyRtpSCprStates::TContext)
void TRequestCommsBinder::DoL()
	{
	Messages::RNodeInterface* sp = iContext.Node().ServiceProvider();
	
    __ASSERT_DEBUG(sp , User::Panic(KRtpScprPanic, CorePanics::KPanicNoServiceProvider));
	iContext.iNodeActivity->PostRequestTo(*sp,TCFServiceProvider::TCommsBinderRequest(RSubConnection::ECreateNew,iContext.Node().iBinderRequestParameters).CRef());
	}


/* State which will accept the ProvisionDone message from RTP Flow. */
DEFINE_SMELEMENT(TAwaitingReadyForBind, NetStateMachine::MState,DummyRtpSCprStates::TContext)
TBool TAwaitingReadyForBind::Accept()
	{
	return iContext.iMessage.IsMessage<TRtpMessages::TRtpMessageReadyForBind>();
	}
	
/* Overriden TCommsBinderRequest. Transition will send comms-binder with ECreateNew to IpDefScpr. */
DEFINE_SMELEMENT(TSetPostedToFlow, NetStateMachine::MStateTransition,DummyRtpSCprStates::TContext)
void TSetPostedToFlow::DoL()
	{
	Messages::RNodeInterface* client = iContext.iNode.FindClient(Messages::message_cast<TCFFactory::TPeerFoundOrCreated>(iContext.iMessage).iNodeId);
	/* No need for explicit check. We will not be here if there is no client anyways */
	iContext.iNodeActivity->SetPostedTo(client->RecipientId());	
	}

DEFINE_SMELEMENT(TAwaitingBinderRequest, NetStateMachine::MState, DummyRtpSCprStates::TContext)
TBool TAwaitingBinderRequest::Accept()
	{
	if (! iContext.iMessage.IsMessage<ESock::TCFServiceProvider::TCommsBinderRequest>())
		{
		return EFalse;
		}

	//ASSERT(iContext.iMessage.iPeer); //TCommsBinderRequest is a peer message only! EC120??
	
	/* Now extract the FlowParams (if present) and store it for sending it along with NoBearer
	   to IpDefScpr */
	ESock::TCFServiceProvider::TCommsBinderRequest& msg = Messages::message_cast<ESock::TCFServiceProvider::TCommsBinderRequest>(iContext.iMessage);

   /* Only one type of bundle is supported( Whatever is used for RTP will be used by RTCP.
      If they are different it is an error case [TO BE DONE] Handle these */
    if(!msg.iFamilyBundle.IsNull())
		{
		iContext.Node().iBinderRequestParameters.Open(msg.iFamilyBundle);
		}
	else
		{
		/* Donot accept the message without a ParameterBundle */
		return EFalse;
		}


	return ETrue;
	}

DEFINE_SMELEMENT(TCreateDataClient, NetStateMachine::MStateTransition,DummyRtpSCprStates::TContext)    
void TCreateDataClient::DoL()
	{
	ESock::TCFServiceProvider::TCommsBinderRequest* binderReq = Messages::message_cast<ESock::TCFServiceProvider::TCommsBinderRequest>(&iContext.iMessage);
	__ASSERT_DEBUG(binderReq, User::Panic(KRtpScprPanic, CorePanics::KPanicIncorrectMessage));
   		
	/* Parameter bundle is must. Without that RtpScpr should'nt have been created */
	__ASSERT_DEBUG(iContext.Node().iParameterBundle.IsNull(), User::Panic(KRtpScprPanic, CorePanics::KPanicPeerMisbehaving));
	
	
	RParameterFamily parameterFamily =	iContext.Node().iParameterBundle.FindFamily(KProtocolExtensionFamily);

    if(parameterFamily.IsNull())
    	{
		User::Leave(KErrNotSupported);
    	}
    	
	STypeId typeId = STypeId::CreateSTypeId(static_cast<TUint>(KDummySubConnRTPParamsImplUid), KSubConnDummyRTPGenericParamsType);
	CSubConRTPGenericParamSetDummy* reqGenericParams = static_cast<CSubConRTPGenericParamSetDummy*>(parameterFamily.FindParameterSet(typeId,RParameterFamily::ERequested));
	if(0 == reqGenericParams)
		{
		User::Leave(KErrNotSupported);
		}		

	
	/* Error cases */
	if( reqGenericParams->GetServiceProtocolID() != KProtocolInetUdp ||
	    reqGenericParams->GetLowerSubConnType() != RSubConnection::EAttachToDefault ||
	    reqGenericParams->GetProtocolUiDRtp() != TUid::Uid(KRtpFlowImplementationUid))
 		{
 		User::Leave(KErrNotSupported);
	 	}

		 	
	/* The First Flow request should be for RTP*/
	/* The next one for RTCP. */
	/* We should Leave if these conditions fails.*/			
	if(iContext.Node().iNumFlowrequestServiced >= 2)
		{
		User::Leave(KErrNotSupported);
		}

	Messages::TNodeId factoryContainer;
    factoryContainer = SockManGlobals::Get()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::EDataPlane));
    __ASSERT_DEBUG(iContext.Node().ControlProvider(), User::Panic(KRtpScprPanic, CorePanics::KPanicNoControlProvider));
    Messages::TNodeId controlProvider = iContext.Node().ControlProvider()->RecipientId();			
	if(0 == iContext.Node().iNumFlowrequestServiced)
		{
		/* The factory decides which Flow to create(RTP/RTCP) based on the Message ID. */
		/* The factory decides which Flow to create(RTP/RTCP) based on the Message ID. */
		/* Should we move this Dirty code to the Factory and make SCPR "cleaner" ?     */
		Messages::TNodeSignal::TMessageId msgId(TRtpMessages::ERtpMessageCreateRtp,TRtpMessages::ERealmId);
		//TCFFactory::TFindOrCreateFactoryObject msg(iContext.Node()(),iContext.iNodeActivity->ActivityId(),
		/* Should we move this Dirty code to the Factory and make SCPR "cleaner" ?     */
		/**/                                
		Messages::TNodeId controlProvider = iContext.Node().ControlProvider()->RecipientId(); 
	
		TDefaultFlowFactoryQuery dataClientQuery( controlProvider, iContext.NodeId(),msgId, TSubConnOpen::ECreateNew);
		
		TCFFactory::TFindOrCreatePeer msg(TCFPlayerRole::EDataPlane, reqGenericParams->GetProtocolUiDRtp(),&dataClientQuery);
		
		iContext.Node().iNumFlowrequestServiced++;
	    
		RMetaExtensionContainer mec;
		mec.Open(iContext.Node().AccessPointConfig());
		CleanupClosePushL(mec);
		
	    /* Now add the RTP Creation parameters to the AccessPointconfig() */
	    CRtpProvisionConfig *rtpProvision = CRtpProvisionConfig::NewLC();
	    rtpProvision->SetBandWidth(reqGenericParams->GetBandWidth());
	    rtpProvision->SetRtptimeConversion(reqGenericParams->GetRtptimeConversion());
	    rtpProvision->SetRtpAutoSend(reqGenericParams->GetRtpAutoSend());
	    rtpProvision->SetRtpTimeNow(reqGenericParams->GetRtpTimeNow());
	    rtpProvision->SetCNAMEL(reqGenericParams->GetCNAME());
	    rtpProvision->SetDefDestinationAddr(reqGenericParams->GetDefDestinationAddr());
	    rtpProvision->SetMaxDropOut(reqGenericParams->GetMaxDropOut());
	    rtpProvision->SetMaxMisorder(reqGenericParams->GetMaxMisorder());
	    rtpProvision->SetMinSequential(reqGenericParams->GetMinSequential());
		
		mec.AppendExtensionL(rtpProvision);
	    
	    /* Ownership no longer with us */
	    CleanupStack::Pop();
		
		iContext.Node().AccessPointConfig().Close();
		iContext.Node().AccessPointConfig().Open(mec);
		CleanupStack::PopAndDestroy(&mec);
		
		}
	else
		{
		Messages::TNodeSignal::TMessageId msgId(TRtpMessages::ERtpMessageCreateRtcp,TRtpMessages::ERealmId);
		TDefaultFlowFactoryQuery dataClientQuery( controlProvider, iContext.NodeId(),msgId, TSubConnOpen::ECreateNew);
		TCFFactory::TFindOrCreatePeer msg(TCFPlayerRole::EDataPlane, reqGenericParams->GetProtocolUiDRtp(),&dataClientQuery);
		iContext.iNodeActivity->PostRequestTo(factoryContainer, msg); 
	    iContext.Node().iNumFlowrequestServiced++; //Now shud be 2
		}
	}

/* Overriden TCommsBinderRequest. Transition will send comms-binder with ECreateNew to IpDefScpr. */
DEFINE_SMELEMENT(TCheckParamsAndDoSomething, NetStateMachine::MStateTransition,DummyRtpSCprStates::TContext)
void TCheckParamsAndDoSomething::DoL()
	{
	RParameterFamily parameterFamily =	iContext.Node().iParameterBundle.FindFamily(KProtocolExtensionFamily);

    if(parameterFamily.IsNull())
    	{
		User::Leave(KErrNotSupported);
    	}
    	
	STypeId typeId = STypeId::CreateSTypeId(static_cast<TUint>(KDummySubConnRTPParamsImplUid), KSubConnDummyRTPGenericParamsType);
	CSubConRTPGenericParamSetDummy* reqGenericParams = static_cast<CSubConRTPGenericParamSetDummy*>(parameterFamily.FindParameterSet(typeId,RParameterFamily::ERequested));
	if(0 == reqGenericParams)
		{
		User::Leave(KErrNotSupported);
		}		

	if(reqGenericParams->GetRandomErrorFlag())
		{
	//	Messages::TEBase::TError ErrorMessage(iContext.iMessage.iSender, iContext.iMessage.ActivityId(), 
	//	                                      TCFFactory::TFindOrCreateFactoryObject::Id(), KErrGeneral);
		                                      
	//	iContext.Node().PostToClients<Messages::TDefaultClientMatchPolicy>(ErrorMessage, ESock::TCFClientType::EData);	
		}
	}
