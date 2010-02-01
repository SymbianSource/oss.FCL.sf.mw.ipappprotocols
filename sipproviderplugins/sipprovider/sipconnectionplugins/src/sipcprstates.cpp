// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SIP Connection Provider States/Transitions
// 
//

/**
 @file
 @internalComponent
*/
 

#include "sipcprstates.h"
#include <comms-infras/ss_subconnprov.h>
#include <elements/nm_messages_base.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include <comms-infras/ss_nodemessages_factory.h>

using namespace SipCpr;
using namespace SipCprStates;
using namespace ESock;
using namespace NetStateMachine;

      
namespace SipCprStates
{

//=======================================================================================================================
//
// BinderRequest Activity
//
//==========================================================================================================================
DEFINE_SMELEMENT(TIntializeWaitForIncmgParams, NetStateMachine::MStateTransition, SipCprStates::TContext)
void TIntializeWaitForIncmgParams::DoL()
	{	
	TCFServiceProvider::TCommsBinderRequest* binderReq = Messages::message_cast<TCFServiceProvider::TCommsBinderRequest>(&iContext.iMessage);
	if (binderReq)
    	{
    	TSubConnOpen::TSubConnType subconType = (TSubConnOpen::TSubConnType)binderReq->iValue;
    	iContext.Node().iSubconType = subconType;
    	if (subconType == RSubConnection::EWaitIncoming)
    		{
    		iContext.Node().iConnStatus = SipCpr::EConnIncoming ;    		
	    	}
	    	else
	    	{
	    	iContext.Node().iConnStatus = SipCpr::EConnOutgoing ;    			
	    	}    		
    	}    	    
	}
	

//=====================================================
//
// Provision Activity
//
//=============================================================
DEFINE_SMELEMENT(TStoreProvision, NetStateMachine::MStateTransition, SipCprStates::TContext)
void TStoreProvision::DoL()
	{
	__CFLOG_VAR((KSipCprTag, KSipCprSubTag, _L8("AStoreProvision::DoL()")));
	PRStates::TStoreProvision storeProvision(iContext);
	storeProvision.DoL();
	
	//Retrieve profileId, appUid and ptr to TransitionEngineMgr;

	const TSipMcprProvisionInfo* ext = static_cast<const TSipMcprProvisionInfo*>(iContext.Node().AccessPointConfig().FindExtension(STypeId::CreateSTypeId(TSipMcprProvisionInfo::EUid, TSipMcprProvisionInfo::ETypeId)));
	if (ext)
    	{
    	iContext.Node().iProfileId = ext->iProfileId;
    	iContext.Node().iAppUid	= ext->iAppUid;
    	iContext.Node().iTransitionEngineMgr = ext->iTransitionEngineMgr;
    	// Create the transition engine here so that it can be sent in
    	// send provision to SCPR 
    	
    	CTransitionEngineMgr* mgr = iContext.Node().iTransitionEngineMgr;
		TUint32 profileId = iContext.Node().iProfileId;
    	TUid appUid = iContext.Node().GetAppUid();
    	CSIPTransitionEngine* transitionEngine = NULL;
		TInt err;
		TRAP(err, transitionEngine = mgr->FindOrCreateL(appUid, profileId));
		if(KErrNone != err)
			{
			__CFLOG_VAR((KSipCprTag, KSipCprSubTag, _L8("ARegister::DoL: Error [%d] while executing CTransitionEngineMgr::FindOrCreate")));			
			User::Leave(err);
			}
			else
			{
			__CFLOG_VAR((KSipCprTag, KSipCprSubTag, _L8("ARegister::DoL: Transition Engine has created")));			
			iContext.Node().iTransitionEngine = transitionEngine;
			if (profileId == KSIPDefaultProfileId)
				{
				profileId = mgr->DefaultProfileId();	
				}					
			}		
         }	
	}
	
DEFINE_SMELEMENT(TSendProvision, NetStateMachine::MStateTransition, SipCprStates::TContext)
void SipCprStates::TSendProvision::DoL()
	{	 
	//Adds the extension to Access Config which will beretrieved in 
	// non-default SCPR and ignored in default SCPR. 	
	iContext.Node().SetConfigL();    		    
	
	PRStates::TSendProvision  cprSendProvision(iContext);
	cprSendProvision.DoL();
	}	
	
//============================================================================
//
// Start Activity
//
//===========================================================================
DEFINE_SMELEMENT(TRegisterCpr,NetStateMachine::MStateTransition, SipCprStates::TContext)
DEFINE_SMELEMENT(TAwaitingRegistrationComplete,NetStateMachine::MState, SipCprStates::TContext)
DEFINE_SMELEMENT(TAwaitingIncomingConnection,NetStateMachine::MState, SipCprStates::TContext)
DEFINE_SMELEMENT(TSetPostedToScpr,NetStateMachine::MStateTransition, SipCprStates::TContext)

void TSetPostedToScpr::DoL()
	{
	// Get the SCPR node ID and wait for the IncomingConneciton message from him
	Messages::RNodeInterface* client = iContext.iNode.FindClient(Messages::message_cast<TCFFactory::TPeerFoundOrCreated>(iContext.iMessage).iNodeId);
	if (client)
		{
		iContext.iNodeActivity->SetPostedTo(client->RecipientId());	
		}	
	}

void TRegisterCpr::DoL()
	{	
	//check if profile ID has been set by passed by MCPR
	//if no profile Id was paseed as part of conn preferences
	//profile id is set to KSIPDefaultProfileId (which is a filler because 0 wont work)
	if (iContext.Node().iProfileId == KSIPDefaultProfileId)
		{
		iContext.Node().iProfileId = iContext.Node().iTransitionEngineMgr->DefaultProfileId();	
		}
	// Call the SIP High-level API for the Registration
	if (iContext.Node().iTransitionEngine == NULL)
		{				
		iContext.iNodeActivity->SetError(KErrNotFound);
		
		const Messages::TNodeId& self = iContext.Node().Id();		
		// Post RegistrationComplete msg with error set
		Messages::RClientInterface::OpenPostMessageClose(self,self,TSipCprMessages::TRegistrationComplete(KErrNotFound).CRef());				
		}
		else
		{
		iContext.Node().iTransitionEngine->RegisterL(&(iContext.Node()));
		iContext.Node().iStage = SipCpr::EStarting;		
		}
	//Expect Response
    iContext.Node().iActivityAwaitingResponse = iContext.iNodeActivity->ActivityId();
    iContext.iNodeActivity->SetPostedTo(iContext.Node().Id());			
	}	
	
TBool TAwaitingIncomingConnection::Accept()
	{
	if(iContext.iMessage.IsMessage<TSipCprMessages::TIncomingConnection>())		
		{		
		// reset it to the initial value so that so that it is taken care in next time
		// around. That i.e after Rconn.Waitforincoming() if subcon open is called 
		// on Rcon like subcon.open(Rcon) then it run into problems. 
		iContext.Node().iConnStatus = SipCpr::EConnOutgoing ;		
		return ETrue;
		}
	return EFalse;					
	}

TBool TAwaitingRegistrationComplete::Accept()
	{	
	if(iContext.iMessage.IsMessage<TSipCprMessages::TRegistrationComplete>())	
		{
		TSipCprMessages::TRegistrationComplete* msg = Messages::message_cast<TSipCprMessages::TRegistrationComplete>(&iContext.iMessage);
		if (msg->iValue == KErrNone)
			{
			iContext.Node().iStage = SipCpr::EActive;			
			}
			else
			{
			//Set the error for activity and will be raised
			TInt error = msg->iValue;
			iContext.iNodeActivity->SetError(error);			
			}
		
		return ETrue;	
		}
	return EFalse;
	}

DEFINE_SMELEMENT(TNoTagOrRegistered,NetStateMachine::MStateFork, SipCprStates::TContext)
TInt TNoTagOrRegistered::TransitionTag()
	{
	if(iContext.Node().iStage == SipCpr::EActive)
		{
		return SipCprStates::KRegistered;
		}
	return MeshMachine::KNoTag;
	}
	
//===============================================================================
//
// Stop Activity
//
//===============================================================================  

DEFINE_SMELEMENT(TDeRegisterCpr,NetStateMachine::MStateTransition, SipCprStates::TContext)
DEFINE_SMELEMENT(TAwaitingDeRegistrationComplete,NetStateMachine::MState, SipCprStates::TContext)

void TDeRegisterCpr::DoL()
	{
	// Call the SIP high level API for de-registraiton
	iContext.Node().iTransitionEngine->Deregister(&(iContext.Node()));	
	if(iContext.Node().iStage <= SipCpr::EStopping)
		{
		//We haven't recieved a DeRegister Request yet
		iContext.Node().iStage = SipCpr::EStopping;
		}
	
	//Expect Response
    iContext.Node().iActivityAwaitingResponse = iContext.iNodeActivity->ActivityId();
    iContext.iNodeActivity->SetPostedTo(iContext.Node().NodeId());
	}
	
	
TBool TAwaitingDeRegistrationComplete::Accept()
	{
	//catch only messages generated by the SIPCPR 
	ASSERT(iContext.iSender == iContext.Node().Id());
	if(iContext.iMessage.IsMessage<TSipCprMessages::TDeRegistrationComplete>())
		{
		TSipCprMessages::TDeRegistrationComplete* msg = Messages::message_cast<TSipCprMessages::TDeRegistrationComplete>(&iContext.iMessage);
		if (msg->iValue == KErrNone)
			{
			iContext.Node().iStage = SipCpr::EStopped;
			}
			else
			{
			//Set the error for activity so that it can be forwarded to applicaiton
			TInt error = msg->iValue;
			iContext.iNodeActivity->SetError(error);				
			}
		return ETrue;	
		}
	return EFalse;	
	}

/*
DEFINE_SMELEMENT(TSendStopped,NetStateMachine::MStateTransition, SipCprStates::TContext)
void TSendStopped::DoL()
	{
	
	TInt stopCode = iContext.Node().iStashedStoppedCode;
		//the section below is reproduced intoto from the CoreStates::TSendStopped :-)
	TCFMessage::TStopped msg(iContext.iNode(), iContext.iCFMessageSig.ActivityId(), stopCode);
	if (iContext.iNodeActivity)
		{
		iContext.iNodeActivity->PostToOriginators(msg);
		}
	else
		{
		//This transition can also be used from a single tripple activity
		ASSERT(iContext.iCFMessageSig.iPeer); //Always a peer message
		iContext.iCFMessageSig.iPeer->PostMessage(msg);
		}
	if (iContext.Node().CountActivities(ECFActivityStart) == 0)
    	{
        TCFMessage::TGoneDown goneDown(iContext.iNode(), ECFActivityNull, stopCode);
        TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(RNodeInterface::ECtrl);
       	for (TInt i = 0; iter[i]; i++)
           	{
           	//Send TGoneDown to every Ctrl client except the originator (who would be recieving TStopped)
           	if ((iContext.iNodeActivity && iContext.iNodeActivity->FindOriginator(*iter[i]) != KErrNone) ||
           	    (iContext.iNodeActivity == NULL && iter[i] != iContext.iCFMessageSig.iPeer))
               	{
               	iter[i]->PostMessage(goneDown);
               	}
           	}
    	}
	if (iContext.iNodeActivity)
    	{
        iContext.iNodeActivity->SetError(KErrNone);
    	}
	}
*/	

/**
Comms-binder activity will use this function. 
It returns KWaitForIncoming if the connection type is WaitForIncoming
otherwise returns KUseExisting
*/
DEFINE_SMELEMENT(TWaitForIncomingOrUseExisting,NetStateMachine::MStateFork, SipCprStates::TContext)
TInt TWaitForIncomingOrUseExisting::TransitionTag()
	{
	if(iContext.Node().iConnStatus == SipCpr::EConnIncoming)
		{
		return CoreNetStates::KWaitForIncoming;
		}
		else
		{		
		return CoreStates::KUseExisting;
		}
	}
		
DEFINE_SMELEMENT(TNoTagOrDeRegister,NetStateMachine::MStateFork, SipCprStates::TContext)
TInt TNoTagOrDeRegister::TransitionTag()
	{	
	MeshMachine::CNodeActivityBase* activity = iContext.iNodeActivity;
	Messages::TSignatureBase& message = iContext.iMessage;
	TInt code = KErrCancel;
	if (activity && activity->Error() != KErrNone)
        {
        code = activity->Error();
        }
    
	if (message.IsTypeOf(Messages::TSigNumber::TypeId()))
		{
   		code = static_cast<const Messages::TSigNumber&>(message).iValue;
   		}
    else if (message.IsTypeOf(Messages::TSigNumberNumber::TypeId()))
		{
   		code = static_cast<const Messages::TSigNumberNumber&>(message).iValue1;
   		}
	
	//stash the stop code to be used after we get DeRegistrationComplete
	iContext.Node().iStashedStoppedCode = code;
	
	//Now do the regular fork
	if(iContext.Node().iStage == SipCpr::EActive)
		{
		return SipCprStates::KDeRegister;
		}
	return MeshMachine::KNoTag;
	}	
}// namespace SipCprStates
