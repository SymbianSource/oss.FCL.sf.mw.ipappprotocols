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
// Sip SubConnection Provider implementation
// 
//

/**
 @file
 @internalComponent
*/


#include "sipscpr.h"
#include "sipscprstates.h"

using namespace ESock;
using namespace NetStateMachine;
using namespace Messages;
using namespace MeshMachine;
_LIT(KPanicSIPSCPRText,"SCPR doesn't appear to expect an incoming connection");

//-=========================================================
//
// Activities
//
//-=========================================================


namespace SipSCprProvisionActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityStoreProvision, SipSCprProvision, TCFDataClient::TProvisionConfig)
	NODEACTIVITY_ENTRY(KNoTag, SipSCprStates::TStoreProvision, CoreNetStates::TAwaitingProvision, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace SipSCprNoBearerActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityNoBearer, SipSCprNoBearer, TCFControlProvider::TNoBearer)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TRaiseError<KErrNotSupported>, CoreNetStates::TAwaitingNoBearer, MeshMachine::TNoTag)
NODEACTIVITY_END()	
}

namespace SipSCprStartConnectionActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStart, SipSCprStartConnection, TCFServiceProvider::TStart,PRActivities::CStartActivity::NewL)	
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingStart, SipSCprStates::TNoTagOrAlreadyStarted)
	NODEACTIVITY_ENTRY(KNoTag, SipSCprStates::TStartSession, SipSCprStates::TAwaitingSessionEstablished, MeshMachine::TNoTagOrErrorTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendStarted)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KAlreadyStarted, CoreNetStates::TSendStarted)	
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TRaiseActivityError)
NODEACTIVITY_END()
}


namespace SipSCprStopActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityStop, SipSCprStop, TCFServiceProvider::TStop)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingStop, SipSCprStates::TNoTagOrAlreadyStopped)	
	//If SCPR is active, stop it, else jumpt to  EAlreadyStopped
	NODEACTIVITY_ENTRY(KNoTag, SipSCprStates::TStopSession, SipSCprStates::TAwaitingSessionTerminated, MeshMachine::TNoTagOrErrorTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, /*PRStates::TSendStopped*/PRStates::TSendStoppedAndGoneDown)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KAlreadyStopped, /*PRStates::TSendStopped*/PRStates::TSendStoppedAndGoneDown)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TRaiseAndClearActivityError)
NODEACTIVITY_END()
}


namespace SipSCprParamsRequestActivity
{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
DECLARE_DEFINE_NODEACTIVITY(ECFActivityParamRequest, SipSCprSetParams, TCFScpr::TSetParamsRequest)
	FIRST_NODEACTIVITY_ENTRY(PRStates::TAwaitingParamRequest, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, SipSCprStates::TStoreAndSetParams)	
NODEACTIVITY_END()
#else
DECLARE_DEFINE_NODEACTIVITY(ECFActivityParamRequest, SipSCprSetParams, TCFScpr::TParamsRequest)
	FIRST_NODEACTIVITY_ENTRY(SCprStates::TAwaitingParamRequest, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, SipSCprStates::TStoreAndSetParams)	
NODEACTIVITY_END()
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
}

namespace SipSCprBinderRequestActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityBinderRequest, SipSCprBinderRequest, TCFServiceProvider::TCommsBinderRequest, MeshMachine::CNodeParallelActivityBase::NewL)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TRaiseError<KErrNotSupported>, CoreNetStates::TAwaitingBinderRequest, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace SipSCprRejoinDataClient
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityRejoin, SipSCprRejoin, TCFRejoiningProvider::TRejoinDataClientRequest, PRActivities::CRejoinDataClientActivity::NewL)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TRaiseError<KErrNotSupported>, CoreNetStates::TAwaitingDataClientRejoin, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace SipSCprActivities
{
DECLARE_DEFINE_ACTIVITY_MAP(activityMap)	
	ACTIVITY_MAP_ENTRY(SipSCprProvisionActivity, SipSCprProvision)
	ACTIVITY_MAP_ENTRY(SipSCprNoBearerActivity, SipSCprNoBearer)
	ACTIVITY_MAP_ENTRY(SipSCprStartConnectionActivity, SipSCprStartConnection)	
	ACTIVITY_MAP_ENTRY(SipSCprStopActivity, SipSCprStop)
	ACTIVITY_MAP_ENTRY(SipSCprParamsRequestActivity, SipSCprSetParams)
	ACTIVITY_MAP_ENTRY(SipSCprBinderRequestActivity, SipSCprBinderRequest)
	ACTIVITY_MAP_ENTRY(SipSCprRejoinDataClient, SipSCprRejoin)	
	ACTIVITY_MAP_ENTRY(SipSCprRejoinDataClient, SipSCprRejoin)	
ACTIVITY_MAP_END_BASE(SCprActivities, coreSCprActivities)
}  

//-=========================================================
//
// CSipSubConnectionProvider methods
//
//-=========================================================	
CSipSubConnectionProvider::~CSipSubConnectionProvider()
    {
    LOG_NODE_DESTROY(KSipSCprTag, CSipSubConnectionProvider);
    if(iSipSm != NULL)
    iSipSm->DeleteWhenReady();
    }

CSipSubConnectionProvider::CSipSubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory)
	: CCoreSubConnectionProvider(aFactory, SipSCprActivities::activityMap::Self())
    {
    LOG_NODE_CREATE(KSipSCprTag, CSipSubConnectionProvider);
    
    }

CSipSubConnectionProvider* 
CSipSubConnectionProvider::NewL(CSubConnectionProviderFactoryBase& aFactory, TSubConnOpen::TSubConnType aSubConnType)
	{
	CSipSubConnectionProvider* provider = new(ELeave)CSipSubConnectionProvider(aFactory);
	CleanupStack::PushL(provider);
    provider->ConstructL(aSubConnType);
    CleanupStack::Pop();
    return provider;
	}
	
	
void CSipSubConnectionProvider::ConstructL(TSubConnOpen::TSubConnType aSubConnType)
	{
	CCoreSubConnectionProvider::ConstructL();
	iStage = SipSCpr::EFresh;		
	subconType = aSubConnType;
	// Set the flag if the sub con type is WaitForIncoming	
	if (aSubConnType == RSubConnection::EWaitIncoming)
		{
		iAwaitingSubConnection = ETrue;		
		}
		else
		{
		iAwaitingSubConnection = EFalse;
		}		
	iSipSm = NULL;
	}


void CSipSubConnectionProvider::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
	TNodeContext<CSipSubConnectionProvider> ctx(*this, aMessage, aSender, aRecipient);
    CCoreSubConnectionProvider::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}
	
void CSipSubConnectionProvider::DoParametersAboutToBeSetL(RCFParameterFamilyBundle_const& aParameterBundle)
	{
	
	if (iStage != SipSCpr::EFresh)
    	{
    	//Can't change parameters after starting.
    	User::Leave(KErrNotSupported);  
    	}
    	
	RParameterFamily calldescrFamily = aParameterBundle.FindFamily(KSubConnCallDescrParamsFamily);
	RParameterFamily authFamily = aParameterBundle.FindFamily(KSubConAuthorisationFamily);
	if(!calldescrFamily.IsNull())
		{
        TSipParams& sipParams = iSipSm->GetSipParams();		
        STypeId idInv = STypeId::CreateSTypeId(KSubConSIPParametersUid,KSubConSIPInviteParamsType);
        STypeId idSbs = STypeId::CreateSTypeId(KSubConSIPParametersUid,KSubConSIPSubscribeParamsType);
		CSubConSIPInviteParamSet* sipInvitationParams = (CSubConSIPInviteParamSet*)calldescrFamily.FindParameterSet(
						idInv, RParameterFamily::ERequested);
		CSubConSIPSubscribeParamSet* sipSubscribeParams = (CSubConSIPSubscribeParamSet*)calldescrFamily.FindParameterSet(
						idSbs, RParameterFamily::ERequested);

        if (sipInvitationParams)
            { 
    	    __CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("CSipSubConnectionProvider::DoParametersAboutToBeSet() [this=%08x]\t -- CSubConSIPInviteParamSet found "), this));
            
            sipParams.iRequest = TSipHLConsts::ERequestInvite;
            sipParams.iInvParams.iReqUri.Set(sipInvitationParams->GetRequestUri());
            sipParams.iInvParams.iTo.Set(sipInvitationParams->GetTo());
            if (sipInvitationParams->GetFrom().Length() == 0)
                {
                //The application may fail to supply 'from' in which case we
                //should reuse the 'from' field stored by the profile.
           	    __CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("CSipSubConnectionProvider::DoParametersAboutToBeSet() [this=%08x]\t -- From field not found"), this));
                sipInvitationParams->SetFromL(*iFromField);
        	    }   
            sipParams.iInvParams.iFrom.Set(sipInvitationParams->GetFrom());
            sipParams.iInvParams.iContact.Set(sipInvitationParams->GetContact());
            sipParams.iInvParams.iContentType.Set(sipInvitationParams->GetContentType());
            sipParams.iInvParams.iContentSubType.Set(sipInvitationParams->GetContentSubType());
            sipParams.iInvParams.iContent.Set(sipInvitationParams->GetContent());
            }
        else if (sipSubscribeParams)
            {
    	    __CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("CSipSubConnectionProvider::DoParametersAboutToBeSet() [this=%08x]\t -- CSubConSIPSubscribeParamSet found"), this));
            
            sipParams.iRequest = TSipHLConsts::ERequestSubscribe;
            sipParams.iSubsParams.iReqUri.Set(sipSubscribeParams->GetRequestUri());
            sipParams.iSubsParams.iTo.Set(sipSubscribeParams->GetTo());
            if (sipSubscribeParams->GetFrom().Length() == 0)
                {
                //The application may fail to supply 'from' in which case we
                //should reuse the 'from' field stored by the profile.
                __CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("CSipSubConnectionProvider::DoParametersAboutToBeSet() [this=%08x]\t -- From field not found"), this));
        	    sipSubscribeParams->SetFromL(*iFromField);
                }
            sipParams.iSubsParams.iFrom.Set(sipSubscribeParams->GetFrom());
            sipParams.iSubsParams.iContact.Set(sipSubscribeParams->GetContact());
            sipParams.iSubsParams.iEventType.Set(sipSubscribeParams->GetEventType());
            sipParams.iSubsParams.iAcceptType.Set(sipSubscribeParams->GetAcceptType());
            sipParams.iSubsParams.iAcceptSubType.Set(sipSubscribeParams->GetAcceptSubType());
            sipParams.iSubsParams.iExpires = sipSubscribeParams->GetExpires();
            sipParams.iSubsParams.iAutoRefresh = sipSubscribeParams->GetAutoRefresh();
            }
        else
            {
    	    __CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("CSipSubConnectionProvider::DoParametersAboutToBeSet() [this=%08x]\t -- UnSupported Call Description Params"), this));
    	    //Send error message to mesh. Error activity will handle it and pass it originator
		    User::Leave(KErrNotSupported);		    
		    }
		}
	if (!authFamily.IsNull())
    	{
        STypeId idAuth = STypeId::CreateSTypeId(KSubConSIPParametersUid,KSubConSIPAuthenticateParamsType);
		CSubConSIPAuthenticateParamSet* sipAuthenticateParams = (CSubConSIPAuthenticateParamSet*)authFamily.FindParameterSet(
						idAuth, RParameterFamily::ERequested);
    	
        if (sipAuthenticateParams)
            {
    	    __CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("CSipSubConnectionProvider::DoParametersAboutToBeSet() [this=%08x]\t -- CSubConSIPAuthenticateParamSet found"), this));

            TSIPCredentials credentials;
            credentials.iUserName.Set(sipAuthenticateParams->GetUserName());
            credentials.iPasswd.Set(sipAuthenticateParams->GetPassword());
            credentials.iRealm.Set(sipAuthenticateParams->GetRealm());
            iTransitionEngine->AddCredentialsL(credentials);
            }
        else
            {            
    	    __CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("CSipSubConnectionProvider::DoParametersAboutToBeSet() [this=%08x]\t -- UnSupported Authentication Params"), this));
    	    //Send error message to mesh. Error activity will handle it and pass it originator
		    User::Leave(KErrNotSupported);
            }            
    	}
    if(authFamily.IsNull() && calldescrFamily.IsNull())
		{
   	    __CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("CSipSubConnectionProvider::DoParametersAboutToBeSet() [this=%08x]\t --Auth and Call Description Family NULL"), this));
   	    //Send error message to mesh. Error activity will handle it and pass it originator
        User::Leave(KErrNotFound);        
		}	
	}
		
	void CSipSubConnectionProvider::InitialiseParametersL(TIncomingCallParameters& aCallParams)
		{
		RCFParameterFamilyBundle newBundle;
		iParameterBundle.CreateL();
		newBundle.CreateL();
		newBundle.Open(iParameterBundle);
    	RParameterFamily sipCallDescrFamily = newBundle.CreateFamilyL(KSubConnCallDescrParamsFamily);
		CSubConSIPInviteParamSet* subConSipRequestedParSet = 
	    CSubConSIPInviteParamSet::NewL(sipCallDescrFamily, RParameterFamily::ERequested);

    	subConSipRequestedParSet->SetRequestUriL(aCallParams.iReqUri);
    	subConSipRequestedParSet->SetFromL(aCallParams.iFrom);
    	subConSipRequestedParSet->SetToL(aCallParams.iTo);
		subConSipRequestedParSet->SetContentL(aCallParams.iContent);
		}
//-=========================================================
//
// MSIPStateMachineClient methods
//
//-=========================================================	

void CSipSubConnectionProvider::CallEstablished()
	{
	
	__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("CSipSubConnectionProvider::Call Established")));
    
    //We dont need this error here, neither do we need to pass erro in SessionEstablished message
    TInt error = KErrNone;
    ASSERT(iStage == SipSCpr::EStarting);
	RClientInterface::OpenPostMessageClose(Id(),TNodeCtxId(iActivityAwaitingResponse, Id()),TSipSCprMessages::TSipSessionEstablished(error).CRef());	
	iActivityAwaitingResponse = MeshMachine::KActivityNull;  
    }


void CSipSubConnectionProvider::CallTerminated(TInt aError, TInt aSipCode)
	{
	
	//The CallTerminated callback method can be triggered in three cases
	//1. As a result of RsubConnection::Stop(). The UE wants to send a BYE
	//2. As a result of the remote UE sending a BYE
	//3. SCPR start fails (in the cases where sip HL API's doesn't support the given SIP method)
	
	__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("CSipSubConnectionProvider::Call Terminated")));	
    
    iStage = SipSCpr::EStopped;
    
    if(iActivityAwaitingResponse == ECFActivityStart)        
    	{
    	__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("CSipSubConnectionProvider::Call Terminated : Activity Start")));	
    	RClientInterface::OpenPostMessageClose(Id(),TNodeCtxId(iActivityAwaitingResponse, Id()),TSipSCprMessages::TSipSessionEstablished(aError).CRef());	
    	iActivityAwaitingResponse = MeshMachine::KActivityNull;  
    	}
    	else if (iActivityAwaitingResponse == ECFActivityStop)
    	{
    	__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("CSipSubConnectionProvider::Call Terminated : Activity Stop")));	
    	RClientInterface::OpenPostMessageClose(Id(),TNodeCtxId(iActivityAwaitingResponse, Id()),TSipSCprMessages::TSipSessionTerminated(aError).CRef());	
    	iActivityAwaitingResponse = MeshMachine::KActivityNull;  
    	}
    	else
    	{
    	// it if for incoming connection
    	//if (iAwaitingSubConnection)
    	//	{
    	// When the remote party sends BYE send a notification for which the 
    	// app is subscribed to.
    	//Notify the upper layeer about the extended SIP reason for termination
    	CSubConSIPResponseEvent* sipevent = NULL;
    	TRAPD(error, sipevent = CSubConSIPResponseEvent::NewL());    
    	if (error != KErrNone)
        	{
       		__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("CSubConSIPResponseEvent::NewL() left with the error: [%d]"), error));
    	  	return;
        	}
    	sipevent->SetResponse(aSipCode);
    	TRAP(error, NotifyClientsL(*sipevent));    		        	    	
    	if (error != KErrNone)
		    {
		    __CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("NotifyClientsL left with the error: [%d]"), error));
		    return;
        	}
		}
    }

/**
This is a call-back function of SIP stata machine. 
It will be called when an incoming connection(incoming INVITE) is received with the 
incoming connection parameters
*/
void CSipSubConnectionProvider::IncomingCall(TIncomingCallParameters& aCallParams)
	{
	__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("CSipSubConnectionProvider::IncomingCall")));
    
    ASSERT(iStage == SipSCpr::EFresh);
    TInt error = KErrNone;
    
    if ((!iAwaitingSubConnection) || !iParameterBundle.IsNull())
        {
        //The logic behind this panic is that IncomingCall() should not
        //arrive, if it doesn't represent an incoming subconnection
        //that is:
        // iAwaitingSubConnection has have to be true
	    User::Panic(KPanicSIPSCPRText, KErrBadHandle);
        }
        
    TRAP(error, InitialiseParametersL(aCallParams));
	if (error != KErrNone)
    	{
    	__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("CSipSubConnectionProvider::InitialiseParametersL() left with error [%d]"), error));
		}		
	//at this point the incoming subconnection has been deemed fit
	//scpr should now send a TIncomingConnection message to its CPR so that RConn::WaitForIncoming can return
        
    // post this message to its control provider SIP CPR to send binder response 
    // so that the subcon.open will be returned
    RNodeInterface* cp = ControlProvider();
    cp->PostMessage(Id(),SipCpr::TSipCprMessages::TIncomingConnection(0).CRef());
    
	}
	
/**
This is a call-back function of the SIP state machine.
It will be called when the high-level API ignores the received challenge
(Response code 401). 
*/
void CSipSubConnectionProvider::CredentialsRequired(const TDesC8 &/* aRealm*/)
	{
	
	__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("CSipSubConnectionProvider::CredentialsRequired")));
	    
    ASSERT(iStage == SipSCpr::EStarting);
    TInt error = KErrNone;
    
    CSubConSIPAuthenticationRequiredEvent* event = NULL;
	TRAP(error, event = CSubConSIPAuthenticationRequiredEvent::NewL());
	if (error != KErrNone)
    	{
    	__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8(" CSubConSIPAuthenticationRequiredEvent::NewL()left with error [%d]"), error));	    	    
	    return;
    	}
    delete event; // remove this if the following code is uncommented
    // uncomment the following code if app is subscribed for the event
    #if 0
    TRAP(error, event->SetRealmL(aRealm));
	if (error != KErrNone)
    	{
    	__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("CSubConSIPAuthenticationRequiredEvent::SetRealmL()  left with error [%d]"), error));    	      	
	    return;
    	}
     TRAP(error, NotifyClientsL(*event)); 
	if (error != KErrNone)
	{
	__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("NotifyClientsL left with the error: [%d]"), error));
	return;
	}
     #endif   
	}

/**
This is callback function. This will be called when the SIP high-level
API receives notification for the Subscribe request
*/
void CSipSubConnectionProvider::ReceiveNotification(TDesC8 & aNotification)
	{
	
	__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("CSipSubConnectionProvider::ReceiveNotification")));
	CSubConSIPNotificationEvent* event = CSubConSIPNotificationEvent::NewL();
	TRAPD(error, event->SetNotificationL(aNotification));
	if (error != KErrNone)
	{
	__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("event->SetNotificationL left with the error: [%d]"), error));
	return;
	}
	
	TInt32 gId = event->GroupId();
		
	TRAP(error, NotifyClientsL(*event));      
	if (error != KErrNone)
	{
	__CFLOG_VAR((KSipSCprTag, KSipSCprSubTag, _L8("NotifyClientsL left with the error: [%d]"), error));
	return;
	}
	}
	
/**
Creates SIP high-level state machine
*/
void CSipSubConnectionProvider::CreateStateMachineL()
	{	
 	iSipSm = CSipStateMachine::NewL(iTransitionEngine,NodeAddr(), iAwaitingSubConnection); 	
 	}

// this pointer is used in the creation of the SIP statemachine	
MSIPStateMachineClient* CSipSubConnectionProvider::NodeAddr()
{
	return this; 
}
