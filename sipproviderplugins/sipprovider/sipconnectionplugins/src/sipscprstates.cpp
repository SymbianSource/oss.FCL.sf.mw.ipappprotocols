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
// SIP SubConnection Provider states/transitions
// 
//

/**
 @file
 @internalComponent
*/

#include "sipdeftscpr.h"
#include "sipscpr.h"
#include "sipscprstates.h"



using namespace ESock;
using namespace NetStateMachine;
using namespace SipSCpr;
using namespace SipSCprStates;


//===========================================
//
//	SIP Sub Connection Provider States
//
//===========================================
namespace SipSCprStates
{

//===================================================================================
//
// StartConnection Activity
//
//====================================================================================

DEFINE_SMELEMENT(TNoTagOrAlreadyStarted,NetStateMachine::MStateFork, SipSCprStates::TContext)
DEFINE_SMELEMENT(TStartSession, NetStateMachine::MStateTransition, SipSCprStates::TContext)
DEFINE_SMELEMENT(TAwaitingSessionEstablished,NetStateMachine::MState, SipSCprStates::TContext)

	
TInt TNoTagOrAlreadyStarted::TransitionTag()
	{
	if(iContext.Node().iStage == SipSCpr::EActive)
		{
		return CoreNetStates::KAlreadyStarted;
		}
	return MeshMachine::KNoTag;
	}

void TStartSession::DoL()
	{
	//check error conditions before starting sip session
	TInt stage = iContext.Node().iStage;
	const Messages::TNodeId& self = iContext.Node().NodeId();
	if(stage != SipSCpr::EFresh)
		{
		switch(stage)
			{
			case SipSCpr::EStopped:
				__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("Cannot start stopped subconnection")));				
				
            	Messages::RClientInterface::OpenPostMessageClose(self,Messages::TNodeCtxId(ECFActivityStart,self),TSipSCprMessages::TSipSessionEstablished(KErrAbort).CRef());	
				break;
			case SipSCpr::EStopping:
				__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("Cannot start stopping subconnection")));				
            	Messages::RClientInterface::OpenPostMessageClose(self,Messages::TNodeCtxId(ECFActivityStart,self),TSipSCprMessages::TSipSessionEstablished(KErrAbort).CRef());	
				break;
			}
		}		
	
	//In the case of SIP SubConnection, the session params should be be set before we start a call.
	TSipParams& sipParams = iContext.Node().iSipSm->GetSipParams();
	// Check should be made only to non-incoming request. For incoming request the 
	// sipParams won't be set in SIP SCPR. The incoming params will be saved in iParameterBundle
	TInt err;		
	if(!(iContext.Node().iAwaitingSubConnection) && sipParams.iRequest == TSipHLConsts::ERequestNone)
		{
		__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("Cannot start SIP subconnection if Params are notset")));
		err = KErrNotSupported;		
		}
		else
		{				
		if (iContext.Node().subconType == RSubConnection::EWaitIncoming)
        	{    	
    		__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("appears to be an incoming call subconection, accepting")));
        	err = iContext.Node().iSipSm->AcceptCall();        
        	}
        	else
        	{        
        	err = iContext.Node().iSipSm->StartCall();			        	
        	}		
		}
			
	if (KErrNone != err)
     	{
     	__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("Cannot start SIP subconnection if Params are notset")));       	
       	Messages::RClientInterface::OpenPostMessageClose(self,Messages::TNodeCtxId(ECFActivityStart,self),TSipSCprMessages::TSipSessionEstablished(err).CRef());	
		}
	//Expect Response
	iContext.Node().iActivityAwaitingResponse = iContext.iNodeActivity->ActivityId();       
    iContext.iNodeActivity->SetPostedTo(iContext.Node().NodeId());
	iContext.Node().iStage = SipSCpr::EStarting;
	}	

TBool TAwaitingSessionEstablished::Accept()
	{
	if(iContext.iMessage.IsMessage<TSipSCprMessages::TSipSessionEstablished>())	
		{
		TSipSCprMessages::TSipSessionEstablished* msg = Messages::message_cast<TSipSCprMessages::TSipSessionEstablished>(&iContext.iMessage);
		if (msg->iValue == KErrNone)
			{
			// This means we have a successful session
			iContext.Node().iStage = SipSCpr::EActive;
			return ETrue;
			}
			else
			{
			//session start failed
			//Set the error for activity and will be cleared using TClearError
			TInt error = msg->iValue;
			iContext.iNodeActivity->SetError(error);
			return ETrue;							
			}
		}
	
	return EFalse;
	}

//========================================================================================================
//
// Stop Activiity
//
//========================================================================================================

DEFINE_SMELEMENT(TNoTagOrAlreadyStopped, NetStateMachine::MStateFork, SipSCprStates::TContext)
DEFINE_SMELEMENT(TStopSession, NetStateMachine::MStateTransition, SipSCprStates::TContext)
DEFINE_SMELEMENT(TAwaitingSessionTerminated,NetStateMachine::MState, SipSCprStates::TContext)


TInt TNoTagOrAlreadyStopped::TransitionTag()
	{

	//iStage defined only for non-deft SCPR
	TInt stage = iContext.Node().iStage;
	if (iContext.Node().iStage && iContext.Node().iStage >= SipSCpr::EStopping)
		{
		return CoreNetStates::KAlreadyStopped;	
		}
		return MeshMachine::KNoTag;
	}

void TStopSession::DoL()
	{
	__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("AStopSession::DoL")));		

	//there's no need to check message type before casting as it should've been checked
	//by the State::Accept() => cast & assert if not the desired type
	iContext.Node().iStopCode = Messages::message_cast<TCFServiceProvider::TStop>(iContext.iMessage).iValue;
	TInt err;	
	
	if (iContext.Node().iStage == SipSCpr::EFresh)
	{
		err = KErrNotReady;
	}
	//If this is an incoming call that hasn't been accepted yet. We should
	//reject it. Otherwise we should just stop it.
	else if (iContext.Node().iAwaitingSubConnection && iContext.Node().iStage == SipSCpr::EStarting)
        {
    	err = iContext.Node().iSipSm->RejectCall();
        }
    else
        {
    	err = iContext.Node().iSipSm->StopCall();
        }
    
    if (KErrNone != err)
     	{
     	__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("Cannot stop SIP subconnection if Params are notset")));
       	const Messages::TNodeId& self = iContext.Node().NodeId();
       	Messages::RClientInterface::OpenPostMessageClose(self,Messages::TNodeCtxId(ECFActivityStop,self),TSipSCprMessages::TSipSessionTerminated(err).CRef());	
		}
		
    iContext.Node().iActivityAwaitingResponse = iContext.iNodeActivity->ActivityId();    
    iContext.iNodeActivity->SetPostedTo(iContext.Node().NodeId());
    iContext.Node().iStage = SipSCpr::EStopping;
	}
	
TBool TAwaitingSessionTerminated::Accept()
	{		
	if(iContext.iMessage.IsMessage<TSipSCprMessages::TSipSessionTerminated>())	
		{
		TSipSCprMessages::TSipSessionTerminated* msg = Messages::message_cast<TSipSCprMessages::TSipSessionTerminated>(&iContext.iMessage);
		if (msg->iValue == KErrNone)
			{							
			iContext.Node().iStage = SipSCpr::EStopped;
			return ETrue;
			}		
		else
			{
			//Set the error for activity and will be cleared using TClearError
			TInt error = msg->iValue;
			iContext.iNodeActivity->SetError(error);
			return ETrue;			
			}		
	}
	return EFalse;
	}	


//=================================================================================================
//
//Set Params
//
//==================================================================================================

DEFINE_SMELEMENT(TSetParameters,NetStateMachine::MStateTransition, SipSCprStates::TContext)
void TSetParameters::DoL()
    {	
	TRAPD(err, iContext.Node().DoParametersAboutToBeSetL(iContext.Node().iParameterBundle));
	if(KErrNone != err)
		{
		__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("call to DoParemetersAboutToBeSet failed in ASetParameters::DoL() [error=%d]"), err));
		User::Leave(err);
		}
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
    iContext.PostToSender(TCFScpr::TSetParamsResponse(iContext.Node().iParameterBundle).CRef());
#else
    iContext.PostToSender(TCFScpr::TParamsResponse(iContext.Node().iParameterBundle).CRef());
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
    }



DEFINE_SMELEMENT(TStoreProvision, NetStateMachine::MStateTransition, SipSCprStates::TContext)
void TStoreProvision::DoL()
	{
	__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("AStoreProvision::DoL()")));
	PRStates::TStoreProvision storeProvision(iContext);
	storeProvision.DoL();	
	  		
	const TSipCprProvisionInfo* ext = static_cast<const TSipCprProvisionInfo*>(iContext.Node().AccessPointConfig().FindExtension(STypeId::CreateSTypeId(TSipCprProvisionInfo::EUid, TSipCprProvisionInfo::ETypeId)));
	if (ext)
    	{
    	iContext.Node().iTransitionEngine = ext->iTransitionEngine;    	
    	iContext.Node().iFromField = ext->iFromField;
    	
    	// Set the falg for WaitForIncoming connection   	
    	if (iContext.Node().iAwaitingSubConnection)
    		{
    		iContext.Node().iActivityAwaitingResponse = iContext.ActivityId();
    		}    	    	
    	// Create the SIP state machine
    	iContext.Node().CreateStateMachineL();
    	__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("CSipSubConnectionProvider::TStoreProvision() [this=%08x]\t -- SIP State Machine created "), this));
    		
        }
	}   
}//namespace SipSCprStates
	       
