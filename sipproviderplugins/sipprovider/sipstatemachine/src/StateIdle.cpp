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
// CStateIdle implementation file.
// 
//

/**
 @file
 @internalComponent
*/
 
#include <e32std.h>
#include <e32base.h>

#include "StateIdle.h"

CStateIdle * CStateIdle::NewL( CSipStateMachine * aStateMachine )
/**
Instantiaion static function
*/
	{
	return new(ELeave) CStateIdle (aStateMachine);
	}

CStateIdle::CStateIdle ( CSipStateMachine * aStateMachine ) :
CSipStateBase ( aStateMachine, TSipHLConsts::EStateSessionIdle )
/**
Constuctor
*/
	{
	// Open Logs
	__FLOG_OPEN(_L8("SipStateMachine"), _L8("StateIdle"));
	__FLOG_1(_L("[StateIdle]:[%x]::CStateIdle() "), this);
	}
CStateIdle::~CStateIdle ()
/**
Destructor
*/	
	{
	// Close Logging
	__FLOG_0(_L("[StateIdle]: CStateIdle::~CStateIdle()"));
	__FLOG_CLOSE;
	}

CSipStateBase * CStateIdle::ProcessClientL( TRequestStatus & aStatus )
/**
Function which process the request and responses from the client
*/
	{
	__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessClientL()"), this);
	// Handle Requests from Client
	switch (iClientRequest)
		{
		case TSipHLConsts::ERequestNone:
			{
			__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessClientL()   <<REQUEST NONE>> "), this);
			iSipSM->SetSuspendRequest(ETrue);
			iNext = this;		
			break;
			}
		case TSipHLConsts::ERequestRegister:
			{
			__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessClientL()   <<REQUEST REGISTER>> "), this);
			TRAPD(err,iSipSM->SendRegisterMessageL());
			if(err != KErrNone)
				{
				iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
				iSipSM->SetGeneralError(err);
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
				}
			else
				{
				iSipSM->SetSuspendRequest(ETrue);
				iNext = this;	
				}
			// Store this because this is required also in the case of failure
			iSipSM->GetTe()->SetTransaction(iSipSM,iSipSM->GetCommandContext().iClientTx);
			break;
			}
		case TSipHLConsts::ERequestInvite:
			{
			__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessClientL()   <<REQUEST INVITE>> "), this);
			TRAPD(err,iSipSM->SendInviteMessageL());
			if(err != KErrNone)
				{
				iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
				iSipSM->SetGeneralError(err);
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
				}
			else
				{
				iSipSM->SetSuspendRequest(ETrue);
				iNext = this;	
				}
			iSipSM->GetTe()->SetTransaction(iSipSM,iSipSM->GetCommandContext().iClientTx);
			iSipSM->GetTe()->SetDialogAssoc(iSipSM,iSipSM->GetCommandContext().iDialogAssoc);
			break;				
			}
		case TSipHLConsts::ERequestBye:
		case TSipHLConsts::ERequestCancel:
			{
			__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessClientL()   <<REQUEST BYE / REQUEST CANCEL>> "), this);
			TRAPD(err,iSipSM->SendCancelL());
			if(err != KErrNone)
				{
				iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
				// Just set the error , as transition to terminated state is automatically happening
				iSipSM->SetGeneralError(err);
				}
			else
				{
				iSipSM->SetSuspendRequest(ETrue);	
				}
			// Change to session terminated as its not sure that we will get 
			// and Response to the cancel while in trying state
			iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
			iNext->ResetRequestResponse();
			}
			break;
		case TSipHLConsts::ERequestSubscribe:
			{
			__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessClientL()   <<REQUEST SUBSCRIBE>> "), this);
			TRAPD(err,iSipSM->SendSubscribeMessageL());
			if(err != KErrNone)
				{
				iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
				iSipSM->SetGeneralError(err);
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
				}
			else
				{
				iSipSM->SetSuspendRequest(ETrue);
				iNext = this;	
				}
			iSipSM->GetTe()->SetTransaction(iSipSM,iSipSM->GetCommandContext().iClientTx);
			iSipSM->GetTe()->SetDialogAssoc(iSipSM,iSipSM->GetCommandContext().iSubsDialogAssoc);
			}
			break;
		default:
			{
			__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessClientL()   <<REQUEST DEFAULT>> "), this);
			if(iReqIdentifier == 1)
				{
				iSipSM->SetSuspendRequest(ETrue);
				iNext = this;
				}
			}
		}
	TRequestStatus * stat = &aStatus;		
	User::RequestComplete(stat,KErrNone);
#ifdef __FLOG_ACTIVE	
	// This indormation is required only when logging is enabled
	TSipHLConsts::SIP_STATES	iD = (iNext != NULL)?iNext->GetStateId() : TSipHLConsts::EStateNone;
	if(iD == KErrNotFound)
		{
		__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessClientL() <<Next State --> NULL>> "), this);
		}
	if(iD == TSipHLConsts::EStateSessionIdle)
		{
		__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessClientL() <<Next State --> StateIdle>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionInitiation)
		{
		__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessClientL() <<Next State --> StateSessionInitiation>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionEstablished)
		{
		__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessClientL() <<Next State --> StateSessionEstablished>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionTerminated)
		{
		__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessClientL() <<Next State --> StateSessionTerminated>> "), this);	
		}
	else
		{
		__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessClientL() <<Next State --> Unknown, its a serious error>> "), this);		
		}
#endif	
	return iNext;	
	}

CSipStateBase * CStateIdle::ProcessServerL( TRequestStatus & aStatus )
/**
Function which process the request and responses from the Server
*/
	{
	__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerL()"), this);
	TSipMsgBundle SipMsgBundle = iSipSM->GetSipMessageBundle();
	// Handle response from server
	switch (iServerResponse)
		{
		case TSipHLConsts::EResponseNone:
			{
			__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerL()  <<RESPONSE NONE>>"), this);
			iSipSM->SetSuspendRequest(ETrue);
			iNext = this;		
			break;	
			}
		case TSipHLConsts::EResponseRegister:
		case TSipHLConsts::EResponseUnRegister:
			{
			__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerL()  <<RESPONSE REGISTER/ RESPONSE UNREGISTER>>"), this);
			if(SipMsgBundle.iStatusCode == 200)
				{
				iSipSM->SetSuspendRequest(ETrue);
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionEstablished);
				iNext->ResetRequestResponse();
				}
			else
				{
				iSipSM->SetSIPError(SipMsgBundle.iStatusCode);
				this->SetClientRequest(TSipHLConsts::ERequestNone);
				iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
				iNext->ResetRequestResponse();
				}
			}
			break;
		case TSipHLConsts::EResponseInvite:
			{
			__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerL()  <<RESPONSE INVITE>>"), this);
			if(SipMsgBundle.iStatusCode == 100)
				{
				__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerL()  <<100 Trying>>"), this);
				iSipSM->SetSuspendRequest(ETrue);
				iNext = this;
				iNext->ResetRequestResponse();
				}
			else if(SipMsgBundle.iStatusCode == 180)	
				{
				__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerL()  <<180 Trying>>"), this);
				iSipSM->SetSuspendRequest(ETrue);
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionInitiation);
				iNext->ResetRequestResponse();
				}
			else // error condition
				{
				__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerL()  <<Error Condition neither 100 nor 180>>"), this);
				iSipSM->SetSIPError(SipMsgBundle.iStatusCode);
				iSipSM->SetOutgoingStatus(EFalse);
				iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
				iNext->ResetRequestResponse();		
				}
			}
			break;
		case TSipHLConsts::EResponseSubscribe:
			{
			__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerL()  <<RESPONSE SUBSCRIBE>>"), this);
			if(SipMsgBundle.iStatusCode == 100)
				{
				__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerL()  <<100 Trying>>"), this);
				iSipSM->SetSuspendRequest(ETrue);
				iNext = this;
				iNext->ResetRequestResponse();
				}
			else if(SipMsgBundle.iStatusCode == 200 || SipMsgBundle.iStatusCode == 202)
				{
				__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerL()  <<200 or 202>>"), this);
				iSipSM->SetOutgoingStatus(EFalse);
				iSipSM->SetSuspendRequest(ETrue);
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionEstablished);
				iNext->ResetRequestResponse();
				iSipSM->CallEstablishedCallback();	
				}
			else
				{
				__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerL()  <<Error neither 100. nore 200 0r 202>>"), this);
				iSipSM->SetSIPError(SipMsgBundle.iStatusCode);
				iSipSM->SetOutgoingStatus(EFalse);
				iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
				iNext->ResetRequestResponse();	
				}
			}
			break;
		default:
			{
			__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerL()  <<RESPONSE DEFAULT>>"), this);
			if(iReqIdentifier == 4)
				{
				iSipSM->SetSuspendRequest(ETrue);
				iNext = this;
				}
			}
		}
	// Handle Request from server
	switch(iServerRequest)
		{
		case TSipHLConsts::ERequestInvite:
			{
			__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerL()  <<REQUEST INVITE>>"), this);
			iSipSM->GetTe()->SetDialogAssoc(iSipSM,SipMsgBundle.iDialog);
			iSipSM->GetTe()->SetTransaction(iSipSM,SipMsgBundle.iServTransaction);
			iSipSM->GetCommandContext().iDialogAssoc = (CSIPInviteDialogAssoc*)SipMsgBundle.iDialog;
			iSipSM->GetCommandContext().iServerTx = SipMsgBundle.iServTransaction;
			TRAPD(err,iSipSM->GenerateInviteResponseL(iSipSM->GetCommandContext().iServerTx, TSipHLConsts::EResponseRinging));
			if(err != KErrNone)
				{
				iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
				iSipSM->SetGeneralError(err);
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
				}
			else
				{
				// Go to Next State as Session is Intiated
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionInitiation);
				iNext->ResetRequestResponse();
				// since user input is required , SM will go to suspended state
				iSipSM->SetSuspendRequest(ETrue);
				// Send Incomin call Notification to User
				iSipSM->CallIncomingCallback();	
				}
			}
			break;
		default:
		__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerL()  <<REQUEST DEFAULT>>"), this);
			if(iReqIdentifier == 2)
				{
				iSipSM->SetSuspendRequest(ETrue);
				iNext = this;
				}
		}
	TRequestStatus * stat = &aStatus;		
	User::RequestComplete (stat , KErrNone);
#ifdef __FLOG_ACTIVE	
	// This indormation is required only when logging is enabled
	TSipHLConsts::SIP_STATES	iD = (iNext != NULL)?iNext->GetStateId() : TSipHLConsts::EStateNone;
	if(iD == TSipHLConsts::EStateNone)
		{
		__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerL() <<Next State --> NULL>> "), this);
		}
	if(iD == TSipHLConsts::EStateSessionIdle)
		{
		__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerL() <<Next State --> StateIdle>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionInitiation)
		{
		__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerL() <<Next State --> StateSessionInitiation>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionEstablished)
		{
		__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerL() <<Next State --> StateSessionEstablished>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionTerminated)
		{
		__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerL() <<Next State --> StateSessionTerminated>> "), this);	
		}
	else
		{
		__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerL() <<Next State --> Unknown, its a serious error>> "), this);		
		}
#endif	
	return iNext;
	}
	
CSipStateBase * CStateIdle::ProcessServerErrorL(TRequestStatus & aStatus)
/**
Handles the Error condition for this state from the server
*/
	{
	__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerErrorL()"), this);
	TSipMsgBundle SipMsgBundle = iSipSM->GetSipMessageBundle();
	// Call the call back to Notify the client
	iSipSM->SetGeneralError(SipMsgBundle.iErrCode);
	iSipSM->SetSIPError(SipMsgBundle.iStatusCode);
	iSipSM->CallTerminateCallback();
	iSipSM->SetCallTerminateCallBack(EFalse);
	// Get to The terminated state
	iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
	iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
	TRequestStatus * stat = &aStatus;		
	User::RequestComplete(stat, KErrNone);
	if(iNext)
	{
	    iNext->ResetRequestResponse();
	}
#ifdef __FLOG_ACTIVE	
	// This indormation is required only when logging is enabled
	TSipHLConsts::SIP_STATES	iD = (iNext != NULL)?iNext->GetStateId() : TSipHLConsts::EStateNone;
	if(iD == KErrNotFound)
		{
		__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerErrorL() <<Next State --> NULL>> "), this);
		}
	if(iD == TSipHLConsts::EStateSessionIdle)
		{
		__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerErrorL() <<Next State --> StateIdle>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionInitiation)
		{
		__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerErrorL() <<Next State --> StateSessionInitiation>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionEstablished)
		{
		__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerErrorL() <<Next State --> StateSessionEstablished>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionTerminated)
		{
		__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerErrorL() <<Next State --> StateSessionTerminated>> "), this);	
		}
	else
		{
		__FLOG_1(_L("[StateIdle]:CStateIdle[%x]::ProcessServerErrorL() <<Next State --> Unknown, its a serious error>> "), this);		
		}
#endif	
	return iNext;
	}













