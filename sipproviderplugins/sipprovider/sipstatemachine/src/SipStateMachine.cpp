// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CSipStateMachine implementation file.
// 
//

/**
 @file
 @internalComponent
*/
 
#include <e32cons.h>
#include <e32base.h>

#include "SipStateMachine.h"
#include "StateIdle.h"
#include "StateSessionInitiation.h"
#include "StateSessionEstablished.h"
#include "StateSessionTerminate.h"

EXPORT_C CSipStateMachine * CSipStateMachine::NewL(CSIPTransitionEngine * aTe, MSIPStateMachineClient* aClient, TBool aSMDirection)
/**
 SIP State Machine Instantiation function
*/
	{
	CSipStateMachine * SipSm = new(ELeave) CSipStateMachine(aTe, aClient, aSMDirection);
	CleanupStack::PushL (SipSm);
	SipSm->ConstructL();
	CleanupStack::Pop (SipSm);
	return SipSm;
	}

void CSipStateMachine::ConstructL()
/**
 This Function will startup the state machine with a default
 state
 */
    {
    // Tell the TE there's another StateMachine wishing to speak to it.
    // Remek: Where do you remove it??? Cos i couldn't find !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    iTe->AddStateMachineL(this);

    // Initialize booleans
    iClientStimulus = EFalse;
    iServerStimulus = EFalse;
    iServerErrorStimulus = EFalse;
    iCallTerminateCallback = ETrue;
    iDeleteMeNow = EFalse;
    isInviteSM = EFalse;
    isSubscribeSM = EFalse;

    iCallTerminateCode.iErrorCode = KErrNone;
    iCallTerminateCode.iSipCode = KErrNone;

    // Construct States
    iStateIdle = CStateIdle::NewL(this);

    iCommandCntx.iCommandState = iStateIdle;
    iCommandCntx.iClientTx = NULL;
    iCommandCntx.iCancelClientTx = NULL;
    iCommandCntx.iServerTx = NULL;
    iCommandCntx.iRegBinding = NULL;
    iCommandCntx.iDialogAssoc = NULL;

    iStateSessionInit = CStateSessionInitiation::NewL(this);
    iStateSessionEstablished = CStateSessionEstablished::NewL(this);
    iStateSessionTerminate = CStateSessionTerminate::NewL(this);

    iActiveState = iStateIdle;
	
    this->Start(&iStatus, NULL);
    // Maithra's fix : For stray signal
    MakeActive();
    }
	
CSipStateMachine::CSipStateMachine(CSIPTransitionEngine * aTe, MSIPStateMachineClient* aClient,TBool aSMDirection) 
:CActive(EPriorityStandard), 
 iTe(aTe),
 iClient(aClient),
 iIncomingStateMachine (aSMDirection)
/**
 constructor
*/
	{
	// Open Logs
	__FLOG_OPEN(_L8("SipStateMachine"), _L8("StateMachine"));
	__FLOG_1(_L("[SipStateMachine]:[%x]::CSipStateMachine() "), this);

	if(this->IsAdded() == EFalse)
		{
		CActiveScheduler::Add(this);
		}		
	}
	

CSipStateMachine::~CSipStateMachine()
/**
 destructor
*/
	{
	//Clean-up the iCommand Context
	Cleanup();
	iSipMsgBundle.Close();
	// delete States
	delete iStateIdle;
	delete iStateSessionInit;
	delete iStateSessionEstablished;	
	delete iStateSessionTerminate;
	// Close Logging
	__FLOG_0(_L("[SipStateMachine]: CSipStateMachine::~CSipStateMachine()"));
	__FLOG_CLOSE;
	}
	
EXPORT_C void CSipStateMachine::DeleteWhenReady()
/**
This function will mark the state machine that the CallTerminated callback
should not be called, and state machine should delete itself gracefully when
completed the operation
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::DeleteWhenReady()"),this);
	iClient = NULL;
	iCallTerminateCallback = EFalse;
	iDeleteMeNow = ETrue;
	// send the signal to state machine 
	if(iActiveState == NULL)
		{
		MakeActive();
		MarkRequestComplete(KErrNone);
		}
	}
	
EXPORT_C TSipParams & CSipStateMachine::GetSipParams()
/**
Returns the SIP parameters
*/
 	{
 	return iSipParams;	
 	}

EXPORT_C TInt CSipStateMachine::AcceptCall()
/**
Accepts and incoming call
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::AcceptCall()"),this);
	// This will always be invite as of now. The implementation of the function needs
	// changes when more functionality would be added onto this SM machine to handle more
	// SIp works
	isInviteSM = ETrue;
	return SendSIPResponse(TSipHLConsts::EResponseInvite);
	}

EXPORT_C TInt CSipStateMachine::StartCall()
/**
Start an outgoing call
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::StartCall()"),this);
	if(iSipParams.iRequest == TSipHLConsts::ERequestInvite)
		{
		isInviteSM = ETrue;
		return SendSIPRequest(TSipHLConsts::ERequestInvite);			
		}
	else if(iSipParams.iRequest == TSipHLConsts::ERequestSubscribe)
		{
		isSubscribeSM = ETrue;
		return SendSIPRequest(TSipHLConsts::ERequestSubscribe);			
		}
	else
		{
		iActiveState = NULL;
		return KErrNotSupported;			
		}
	}

EXPORT_C TInt CSipStateMachine::StopCall()
/**
Terminate an established connection
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::StopCall()"),this);
	if(isSubscribeSM)
		{
		return SendSIPRequest(TSipHLConsts::ERequestUnSubscribe);
		}
	else if(isInviteSM)
		{
		return SendSIPRequest(TSipHLConsts::ERequestBye);	
		}
	else
		{
		return KErrNotSupported;	
		}
	}

EXPORT_C TInt CSipStateMachine::RejectCall()
/**
Rejects an incoming call
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::RejectCall()"),this);
	return SendSIPResponse(TSipHLConsts::EResponseInviteReject);	
	}
		
TInt CSipStateMachine::SendSIPRequest(TSipHLConsts::SIP_REQUESTS aRequest)
/**
Sends a SIP request of a particular type 
*/
	{
	if(iActiveState == NULL)
		return KErrNotReady;
	iActiveState->ResetRequestResponse();
	iActiveState->SetClientRequest(aRequest);
	iClientStimulus = ETrue;
	if (iSuspendRequest)
    	{
    	SetSuspendRequest(EFalse);   
    	}
		
	MakeActive();
	MarkRequestComplete(KErrNone);
	return KErrNone;
	}

TInt CSipStateMachine::SendSIPResponse(TSipHLConsts::SIP_RESPONSES aResponse)
/**
Sends a SIP response of a particular type
*/
	{
	if(iActiveState == NULL)
    	{
    	return KErrNotReady;    
    	}
		
	iActiveState->ResetRequestResponse();
	iActiveState->SetClientResponse(aResponse);
	iClientStimulus = ETrue;
	if (iSuspendRequest)
    	{
    	SetSuspendRequest(EFalse);  
    	}
		
	MakeActive();
	MarkRequestComplete(KErrNone);
	return KErrNone;
	}

	
void CSipStateMachine::Start( TRequestStatus* aClientStatus, CSipStateBase* aErrorState )
/** 
Starts the state machine
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::Start()"),this);
	iClientStatus 	= 	aClientStatus;
	iErrorState 	= 	aErrorState;
	iSuspendRequest = 	EFalse;

	if (iClientStatus != NULL)
    	{
    	*iClientStatus = KRequestPending;    
    	}
	  
	// we will not call RunL explicitly over here, That means the SM will be 
	// created in suspended mode.	  
	}
	
void CSipStateMachine::IncomingResponse(TSipMsgBundle aMsgBundle)
/**
Handles the incoming response inside a dialog
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::IncomingResponse()"),this);
	iActiveState->ResetRequestResponse();
	iActiveState->SetServerResponse(aMsgBundle.iResponse);
	// wake up the State Machine if suspended and pass the response back to the state
	if (iSuspendRequest)
    	{
    	SetSuspendRequest(EFalse);   
    	}
		
	iServerStimulus = ETrue;
	iSipMsgBundle.Append(aMsgBundle);
	MakeActive();
	MarkRequestComplete(KErrNone);	
	}
void CSipStateMachine::IncomingRequestInsideDialog(TSipMsgBundle aMsgBundle)
/**
Incoming Request inside established dialog
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::IncomingResponseInsideDialog()"),this);
	iActiveState->ResetRequestResponse();
	iActiveState->SetServerRequest(aMsgBundle.iRequest);
	// Delete the earlier server transaction if any as earlier transaction comes on outside a
	// dialog and not needed as new server transaction that comes with ACK / or inside dialoog
	// will be used
	if(iCommandCntx.iServerTx != NULL)
		{
		delete iCommandCntx.iServerTx;
		iCommandCntx.iServerTx = NULL;
		}
	// Set the server Transaction
	iCommandCntx.iServerTx = aMsgBundle.iServTransaction;
	if (iSuspendRequest)
    	{
    	SetSuspendRequest(EFalse);   
    	}
		
	iServerStimulus = ETrue;
	iSipMsgBundle.Append(aMsgBundle);
	MakeActive();
	MarkRequestComplete(KErrNone);
	}
	
void CSipStateMachine::ErrorOccured(TSipMsgBundle aErrBundle)	
/**
Handle the error from TE
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::ErrorOccurred()"),this);
	iActiveState->ResetRequestResponse();
	if (iSuspendRequest)
    	{
    	SetSuspendRequest(EFalse);   
    	}
		
	iServerErrorStimulus = ETrue;
	iSipMsgBundle.Append(aErrBundle);
	MakeActive();
	MarkRequestComplete(KErrNone);
	}
	
void CSipStateMachine::InviteCompleted(CSIPClientTransaction& /*aTransaction*/)
/**
This callback is called to maek the completion of invite 
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::InviteCompleted()"),this);
	// we will not match and delete the client transaction as it will be anyway
	// deleted when a BYE is being sent or received.
	}
	
void CSipStateMachine::IncomingRequest(TSipMsgBundle aMsgBundle)
/**
New Incoming request Inside established dialog
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::IncomingRequest()"),this);
	iActiveState->ResetRequestResponse();
	iActiveState->SetServerRequest(aMsgBundle.iRequest);
	iSipMsgBundle.Append(aMsgBundle);
	iServerStimulus = ETrue;
	if (iSuspendRequest)
    	{
    	SetSuspendRequest(EFalse);    
    	}
		
	MakeActive();
	MarkRequestComplete(KErrNone);
	}

void CSipStateMachine::IncomingRequestOutsideDialog(TSipMsgBundle & aMsgBundle)
/**
New Incoming request outside any established dialog
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::IncomingRequestOutsideDialog()"),this);
	// set the command context
	iCommandCntx.iDialogAssoc	= (CSIPInviteDialogAssoc*)aMsgBundle.iDialog;
	iCommandCntx.iServerTx  	= aMsgBundle.iServTransaction;
	iActiveState->ResetRequestResponse();
	iActiveState->SetServerRequest(aMsgBundle.iRequest);
	iSipMsgBundle.Append(aMsgBundle);
	iServerStimulus = ETrue;
	if (iSuspendRequest)
    	{
    	SetSuspendRequest(EFalse);    
    	}
		
	MakeActive();
	MarkRequestComplete(KErrNone);
	}
	
void CSipStateMachine::GenerateInviteResponseL(
						CSIPServerTransaction *& aTransaction,
						TSipHLConsts::SIP_RESPONSE_TYPE aType)
/**
 */
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::GenerateInviteResponseL()"),this);
	switch(aType)
		{
		case TSipHLConsts::EResponseRinging:
			{
			__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::GenerateInviteResponseL() <<180 Ringing>>"),this);
			CSIPResponseElements *ResElem = CSIPResponseElements::NewLC(
							180, SIPStrings::StringF(SipStrConsts::EPhraseRinging));
			aTransaction->SendResponseL(ResElem);
			CleanupStack::Pop(ResElem);	
			}
			break;
		case TSipHLConsts::EResponse200OK:
			{
			__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::GenerateInviteResponseL() <<200 OK>>"),this);
			CSIPResponseElements *ResElem = CSIPResponseElements::NewLC(
							200, SIPStrings::StringF(SipStrConsts::EPhraseOk));
			aTransaction->SendResponseL(ResElem);
			CleanupStack::Pop(ResElem);		
			}
			break;
		case TSipHLConsts::EResponseReject:
			{
			__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::GenerateInviteResponseL() <<488 Reject>>"),this);
			CSIPResponseElements *ResElem = CSIPResponseElements::NewLC(
							488, SIPStrings::StringF(SipStrConsts::EPhraseNotAcceptableHere));
			aTransaction->SendResponseL(ResElem);
			CleanupStack::Pop(ResElem);			
			}
			break;
		default:
			{
			__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::GenerateInviteResponseL() <<No Response Generated>>"),this);
			}
		}
	
	}
	
void CSipStateMachine::GenerateSubscribeResponseL
						(CSIPServerTransaction *& aTransaction,
						TSipHLConsts::SIP_RESPONSE_TYPE aType)
/**
 */
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::GenerateSubscribeResponseL()"),this);
	switch(aType)
		{
		case TSipHLConsts::EResponse200OK:
			{
			__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::GenerateSubscribeResponseL() <<200 OK>>"),this);
			CSIPResponseElements *ResElem = CSIPResponseElements::NewLC(
						200, SIPStrings::StringF(SipStrConsts::EPhraseOk));
			aTransaction->SendResponseL(ResElem);
			CleanupStack::Pop(ResElem);		
			// We will not delete this here , this will be deleted after giving notification
			// to the client
			/*
			delete aTransaction;
			aTransaction = NULL;
			*/
			}
			break;
		default:
			{
			__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::GenerateSubscribeResponseL() <<No Response Generated>>"),this);
			}
		}
	}
	
HBufC8 * CSipStateMachine::GenerateNotificationData()
/**
*/
	{
	// Generate the notification parameter from the incoming server Transaction	
	CSIPServerTransaction *sTran = iCommandCntx.iServerTx;
	// get the request elements
	const CSIPRequestElements *reqElems = sTran->RequestElements();
	// Get the contents of the request elements
	// Get the Message Elements
	const CSIPMessageElements & msgElems = reqElems->MessageElements();
	// Get the content
	TPtrC8 content = msgElems.Content();
	HBufC8 * notificationData = NULL;
	TRAPD(err,notificationData = content.AllocL());
	if(err != KErrNone)
		{
		return NULL;
		}
	return notificationData;
	}
	
void CSipStateMachine::RunL()
/** 
 * Called on completion of one active event
 */
	{
	if (iStatus.Int() != KErrNone)
		{
		OnError();
		}
	
	if (iActiveState)
		{
		if (iSuspendRequest)
			{
			// do nothing wait for somebody to enable the state machine
			return;	
			}
		else if (iClientStimulus)
			{
			iClientStimulus = EFalse;
			MakeActive();
			iActiveState = iActiveState->ProcessClientL(iStatus);
			}
		else if (iServerStimulus)
			{
			iServerStimulus = EFalse;
			MakeActive();
			iActiveState = iActiveState->ProcessServerL(iStatus);
			}
		else if(iServerErrorStimulus)
			{
			iServerErrorStimulus = EFalse;
			MakeActive();
			iActiveState = iActiveState->ProcessServerErrorL(iStatus);
			}
		else
			{
			iSuspendRequest = ETrue;	
			MakeActive();
			TRequestStatus *stat = &iStatus;
			User::RequestComplete(stat,KErrNone);
			}
		}
	else if (iActiveState == NULL && iDeleteMeNow )
		{
		// Ask the state machine to delete itself
		OnCompletion();
		}
	else
		{
		//do nothing	
		}
	}

void CSipStateMachine::OnError()
/** 
 * Called from ::RunL when an error occures
 */
	{
	// Put the state machine in suspended mode , and return. 
	SetSuspendRequest(ETrue);
	TRequestStatus *stat = &iStatus;
	User::RequestComplete(stat,KErrNone);
	}

void CSipStateMachine::OnCompletion()
/** 
 * Called by RunL when it is needed to delete the state machine
 */
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::OnCompletion() <<delete this>>"),this);
	delete this;
	}

TInt CSipStateMachine::RunError(TInt aError)
/** 
 */
	{
	TRequestStatus * Stat = &iStatus;
	User::RequestComplete( Stat , aError );
	return KErrNone; 
	}

void CSipStateMachine::DoCancel()
/** 
 */
	{
	OnCompletion();
	}

void CSipStateMachine::MakeActive()
/**
 Function call to wake State Machine from Suspended
 state
*/	{
	if(!IsActive())
    	{
    	SetActive();   
    	}
	}
	
void CSipStateMachine::MarkRequestComplete(TInt aErrCode)
/**
This function will mimic implicitly calling of the 
Runl function of the State Machine base
*/	
	{
	TRequestStatus * ThisStat = &iStatus;
	User::RequestComplete ( ThisStat, aErrCode);
	}
	
CSIPTransitionEngine * CSipStateMachine::GetTe()
/**
returns the current instance of TE
*/
	{	
	return iTe;
	}
	
CSipStateBase * CSipStateMachine::GetActiveState()	
/**
Return the Currently active state
*/
	{
	return iActiveState;
	}
	
void  CSipStateMachine::CallTerminateCallback()
/**
Varifies and call the terminate call back with appropriate values
*/
	{
	__FLOG_2(_L("[SipStateMachine]: CSipStateMachine[%x]::CallTerminateCallback() <<Callback Flag = %d>>"),this,iCallTerminateCallback);
	if(iCallTerminateCallback)
		{
		iClient->CallTerminated(iCallTerminateCode.iErrorCode, iCallTerminateCode.iSipCode);
		}
	}
	
void CSipStateMachine::CallIncomingCallback()
/**
Calls the client with incoming call parameters
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::CallIncomingCallback()"),this);
	TIncomingCallParameters inParam;
	GenerateIncomingCallParameters(inParam);	
	iClient->IncomingCall(inParam);
	}
	
void CSipStateMachine::CallEstablishedCallback()
/**
Notifies the client about the established call
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::CallEstablishedCallback()"),this);
	iClient->CallEstablished();
	}
	
void CSipStateMachine::CallNotificationCallback(TPtrC8 aNotifyData)
/**
Notifies the client about the incoming notification
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::CallNotificationCallback()"),this);
	iClient->ReceiveNotification(aNotifyData);
	}
	
void CSipStateMachine::GenerateIncomingCallParameters(TIncomingCallParameters & aParam)
/**
Store the incoming call parameters
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::GenerateIncomingCallParameters()"),this);
	_LIT8(KAtTheRate,"@");
	
	CSIPServerTransaction *sTran = iCommandCntx.iServerTx;
	const CSIPRequestElements *reqElems = sTran->RequestElements();
	// Get Request Uri
	const CUri8 & uri = reqElems->RemoteUri();
	TPtr8 ptrUri = iUriValues.iReqUri.Des();
		
	ptrUri.Append(uri.Uri().Extract(EUriUserinfo));
	ptrUri.Append(KAtTheRate);
	ptrUri.Append(uri.Uri().Extract(EUriHost));
	// Set Request Uri
	aParam.iReqUri.Set(iUriValues.iReqUri);
	
	
	// Get from 
	const CSIPFromHeader *fromHdr = reqElems->FromHeader();
	const CUri8 & uriFrom = fromHdr->SIPAddress().Uri8();
	
	TPtr8 ptrFrom = iUriValues.iFrom.Des();
	ptrFrom.Append(uriFrom.Uri().Extract(EUriUserinfo));
	ptrFrom.Append(KAtTheRate);
	ptrFrom.Append(uriFrom.Uri().Extract(EUriHost));
	// Set from
	aParam.iFrom.Set(iUriValues.iFrom);
	
	// Get To
	const CSIPToHeader *toHdr = reqElems->ToHeader();
	const CUri8 & uriTo = toHdr->SIPAddress().Uri8();
	
	TPtr8 ptrTo = iUriValues.iTo.Des();
	ptrTo.Append(uriTo.Uri().Extract(EUriUserinfo));
	ptrTo.Append(KAtTheRate);
	ptrTo.Append(uriTo.Uri().Extract(EUriHost));
	// Set To
	aParam.iTo.Set(iUriValues.iTo);
	
	// Get the Message Elements
	const CSIPMessageElements & msgElems = reqElems->MessageElements();
	aParam.iContent.Set(msgElems.Content());
	}
	
TBool CSipStateMachine::GetIncomingStatus()
/**
Sets whether the state machine is for incoming call
*/
	{
	return iIncomingStateMachine;
	}

TBool CSipStateMachine::GetOutgoingStatus()
/**
sets whether an active outgoing is pending or not
*/
	{
	return isActiveOutgoingSession;
	}
	
void CSipStateMachine::SetOutgoingStatus(TBool aStatus)	
/**
Resets the status of active outgoing pending
*/
	{
	isActiveOutgoingSession = aStatus;
	}
	
CSipStateBase * CSipStateMachine::GetNextState (TSipHLConsts::SIP_STATES aNextState)
	{
	switch (aNextState)
		{
		case TSipHLConsts::EStateSessionIdle:
			return iStateIdle;
		case TSipHLConsts::EStateSessionInitiation:
			return iStateSessionInit;
		case TSipHLConsts::EStateSessionEstablished:
			return iStateSessionEstablished;
		case TSipHLConsts::EStateSessionTerminated:
			return iStateSessionTerminate;
		case TSipHLConsts::EStateNone:
		default:
			return NULL;
		}	
	
	}
void CSipStateMachine::Cleanup()
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::Cleanup()"),this);
	// Cleanup everything in Command Context
	delete iCommandCntx.iClientTx;
	iCommandCntx.iClientTx = NULL;
	
	delete iCommandCntx.iCancelClientTx;
	iCommandCntx.iCancelClientTx = NULL;
	
	delete iCommandCntx.iRegBinding;
	iCommandCntx.iRegBinding = NULL;
	
	delete iCommandCntx.iClientTx;
	iCommandCntx.iClientTx = NULL;
	
	delete iCommandCntx.iServerTx;
	iCommandCntx.iServerTx = NULL;
	
	delete iCommandCntx.iDialogAssoc;
	iCommandCntx.iDialogAssoc = NULL;
	
	delete iCommandCntx.iSubsDialogAssoc;
	iCommandCntx.iSubsDialogAssoc = NULL;
	}
	
	
void CSipStateMachine::SendRegisterMessageL()
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::SendRegisterMessageL()"),this);
	// create all the necessary ingredients to send Register Message
	CSIPRefresh * refresh = CSIPRefresh::NewLC();
	CSIPToHeader * to 	  = CreateToHeaderLC(iSipParams.iRegParams.iTo);
	CSIPFromHeader *from  = CreateFromHeaderLC (iSipParams.iRegParams.iFrom);
	CSIPAddress * addr 	  = CSIPAddress::DecodeL(iSipParams.iRegParams.iContact);
	CSIPContactHeader * contact = CSIPContactHeader::NewL(addr);
	CleanupStack::PushL(contact);
	TUriParser8 parser;
	User::LeaveIfError(parser.Parse(iSipParams.iRegParams.iRegistrar));
    CUri8* remoteUri 	  = CUri8::NewLC(parser);   	
    // Send Registration
    CSIPConnection * sipConn = GetTe()->GetSipConnection();
    iCommandCntx.iRegBinding 	= CSIPRegistrationBinding::NewL(
    							  *sipConn, to, contact,
    							  refresh,0, remoteUri, from);
	iCommandCntx.iClientTx		= iCommandCntx.iRegBinding->RegisterL();
	CleanupStack::Pop(5, refresh);
	}
	
void CSipStateMachine::SendUnRegisterMessageL()	
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::SendUnRegisterMessageL()"),this);
	if (iCommandCntx.iClientTx != NULL)
		{
		delete iCommandCntx.iClientTx;
		iCommandCntx.iClientTx = NULL;
		}
	iCommandCntx.iClientTx = iCommandCntx.iRegBinding->DeregisterL();
	}

void CSipStateMachine::SendInviteMessageL()
/**
Send an invitation to remote phone
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::SendInviteMessageL()"),this);
	// set the active outgoing session
	isActiveOutgoingSession = ETrue;
	// Get Invite Parameters
	CSIPToHeader 	* to 	= CreateToHeaderLC(iSipParams.iInvParams.iTo);
	CSIPFromHeader	* from 	= CreateFromHeaderLC(iSipParams.iInvParams.iFrom);
	CSIPAddress		* addr 	= CSIPAddress::DecodeL(iSipParams.iInvParams.iContact);
	CSIPContactHeader * contact = CSIPContactHeader::NewLC(addr);
	TUriParser8		parser;
	User::LeaveIfError(parser.Parse(iSipParams.iInvParams.iReqUri));
	CUri8 *	remoteUri	=	CUri8::NewLC(parser);	
	CSIPMessageElements* msgElements = NULL;

	// Check whether the content is provided or not, if not no SDP will be genrated	
	if((iSipParams.iInvParams.iContentType.Length() != 0) &&
		(iSipParams.iInvParams.iContentSubType.Length() != 0) &&
		(iSipParams.iInvParams.iContent.Length() != 0))
		{
	   // Generate the SDP 
		SdpCodecStringPool::OpenL();
		
		 msgElements = CSIPMessageElements::NewLC();
		CSIPContentTypeHeader* ct = CSIPContentTypeHeader::NewLC( 
				iSipParams.iInvParams.iContentType, iSipParams.iInvParams.iContentSubType );
		CSdpDocument * sdp = CSdpDocument::DecodeLC(iSipParams.iInvParams.iContent);
		HBufC8* content = GetContentBodyL( sdp );
		CleanupStack::PushL( content );
		msgElements->SetContentL( content, ct );
		CleanupStack::Pop();
		CleanupStack::Check(sdp);
		CleanupStack::PopAndDestroy();// for the sdp document.
		SdpCodecStringPool::Close();
		}
	// Create Invite Dialog Association
	CSIPConnection * sipConn = GetTe()->GetSipConnection();
	iCommandCntx.iDialogAssoc = CSIPInviteDialogAssoc::NewLC(
					*sipConn, from, remoteUri, to, contact );
	iCommandCntx.iClientTx	= iCommandCntx.iDialogAssoc->SendInviteL(msgElements);

	if(msgElements)
		{
		CleanupStack::Pop(7, to);
		}
	else
		{
		CleanupStack::Pop(5, to);
		}
	}

void CSipStateMachine::SendAckForInviteL()
/**
Send in response to the response of Initial invite request
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::SendAckForInvite()"),this);
	iCommandCntx.iDialogAssoc->SendAckL(*(iCommandCntx.iClientTx),NULL);
	}
	
void CSipStateMachine::SendCancelL()
/**
Cancels the ongoing session
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::SendCancelL()"),this);
	iCommandCntx.iCancelClientTx = iCommandCntx.iClientTx->CancelL();	
	}

void CSipStateMachine::SendByeL()
/** 
Terminates the ongoing session associated with a dialog
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::SendByeL()"),this);
	if (iCommandCntx.iClientTx != NULL)
		{
		delete iCommandCntx.iClientTx;
		iCommandCntx.iClientTx = NULL;	
		}
	iCommandCntx.iClientTx = iCommandCntx.iDialogAssoc->SendByeL(NULL);
	}

void CSipStateMachine::SendByeResponseL()
/**
Acknoweldge the bye
*/
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::SendByeResponseL() <<200 OK>>"),this);
	CSIPResponseElements* RespElem =  CSIPResponseElements::NewLC( 
            200, SIPStrings::StringF( SipStrConsts::EPhraseOk ) );
	iCommandCntx.iServerTx->SendResponseL(RespElem);
	CleanupStack::Pop(RespElem);	
	}

HBufC8* CSipStateMachine::GetContentBodyL( CSdpDocument* aContent )
	{
	HBufC8* contentBuf = NULL;
	if ( aContent->IsValid() )
		{
		CBufFlat* enBuf = CBufFlat::NewL( 1000 );
		CleanupStack::PushL( enBuf );
		RBufWriteStream writeStream;
		writeStream.Open( *enBuf, 0 );
		aContent->EncodeL( writeStream );
		writeStream.Close();
		TPtr8 ptr = enBuf->Ptr( 0 );
		contentBuf = ptr.AllocL();
		CleanupStack::PopAndDestroy( enBuf );
		writeStream.Close();
		}
	return contentBuf;
	}	
	
void CSipStateMachine::SendSubscribeMessageL()
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::SendSubscribeMessageL()"),this);
	// set the active outgoing session
	isActiveOutgoingSession = ETrue;
	// get Refresh
	CSIPRefresh* refresh = NULL;
	if (iSipParams.iSubsParams.iAutoRefresh)
    	{
    	refresh = CSIPRefresh::NewLC();
    	}
	
	// SIP Message Elements required for Subscribe
	CSIPMessageElements* message = CSIPMessageElements::NewLC();
    RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
    // Add Expires
    CSIPExpiresHeader* expires = new(ELeave)CSIPExpiresHeader(iSipParams.iSubsParams.iExpires);
    CleanupStack::PushL(expires);
    headers.AppendL(expires);
    CleanupStack::Pop(expires);
    // Add Accept 
    CSIPAcceptHeader* accept = CSIPAcceptHeader::NewLC(
    				iSipParams.iSubsParams.iAcceptType,iSipParams.iSubsParams.iAcceptSubType);
    headers.AppendL(accept);
    CleanupStack::Pop(accept); 
    // add user headers
    message->SetUserHeadersL(headers);
    CleanupStack::Pop(1); // headers
    headers.Close();
        
	
	CSIPToHeader 	* to 	= CreateToHeaderLC(iSipParams.iSubsParams.iTo);
	CSIPFromHeader	* from 	= CreateFromHeaderLC(iSipParams.iSubsParams.iFrom);
	CSIPAddress		* addr 	= CSIPAddress::DecodeL(iSipParams.iSubsParams.iContact);
	CSIPContactHeader * contact = CSIPContactHeader::NewLC(addr);
	TUriParser8		parser;
	User::LeaveIfError(parser.Parse(iSipParams.iSubsParams.iReqUri));
	CUri8 *	remoteUri	=	CUri8::NewLC(parser);
	CSIPEventHeader	*evtHdr	= 	CSIPEventHeader::DecodeL(iSipParams.iSubsParams.iEventType);
	CleanupStack::PushL(evtHdr);
	CSIPConnection * sipConn = GetTe()->GetSipConnection();
	// Create subscription to send
	iCommandCntx.iSubsDialogAssoc = CSIPSubscribeDialogAssoc::NewLC(
									*sipConn,from,remoteUri,evtHdr,to,contact);
	iCommandCntx.iClientTx = iCommandCntx.iSubsDialogAssoc->SendSubscribeL(message,refresh);
	if (refresh)
    	{
    	CleanupStack::Pop(8, refresh);   
    	}
    else
        {
        CleanupStack::Pop(7, message); 
        }			

	}

void CSipStateMachine::SendUnSubscribeMessageL()
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::SendUnSubscribeMessageL()"),this);
	if (iCommandCntx.iClientTx != NULL)
		{
		delete iCommandCntx.iClientTx;
		iCommandCntx.iClientTx = NULL;
		}
	iCommandCntx.iClientTx = iCommandCntx.iSubsDialogAssoc->SendUnsubscribeL(NULL);
	}
	
CSIPToHeader* CSipStateMachine::CreateToHeaderLC( const TDesC8& aSipUri )
	{
		CSIPAddress* addr = CSIPAddress::DecodeL( aSipUri );
		CleanupStack::PushL( addr );
	
		CSIPToHeader* to = CSIPToHeader::NewL( addr );
		CleanupStack::Pop( addr );
		CleanupStack::PushL( to );
		return to;
	}

CSIPFromHeader* CSipStateMachine::CreateFromHeaderLC( const TDesC8& aSipUri )
	{
		CSIPAddress* addr = CSIPAddress::DecodeL( aSipUri );
		CleanupStack::PushL( addr );
	
		CSIPFromHeader* from = CSIPFromHeader::NewL( addr );
		CleanupStack::Pop( addr );
		CleanupStack::PushL( from );
		return from;
	}
	
	
void	CSipStateMachine::ChallengeIgnored(const TDesC8& aRealm)
	{
	__FLOG_1(_L("[SipStateMachine]: CSipStateMachine[%x]::ChallengeIgnored()"),this);
	//Notify SIPSCPR that challenge has been ignored
	//and credentials need to be set
	iClient->CredentialsRequired(aRealm);
	}
	
