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
// CStateSessionEstablished implementation file.
// 
//

/**
 @file
 @internalComponent
*/
 
#include <e32std.h>
#include <e32base.h>

#include "StateSessionEstablished.h"

CStateSessionEstablished * CStateSessionEstablished::NewL( CSipStateMachine * aStateMachine )
/**
Instantiaion static function
*/
	{
	return new(ELeave) CStateSessionEstablished (aStateMachine);
	}

CStateSessionEstablished::CStateSessionEstablished ( CSipStateMachine * aStateMachine ) :
CSipStateBase (aStateMachine, TSipHLConsts::EStateSessionEstablished )
/**
Constuctor
*/
	{
	iDoneUnSubscribe = EFalse;
	// Open Logs
	__FLOG_OPEN(_L8("SipStateMachine"), _L8("StateSessionEstablished"));
	__FLOG_1(_L("[StateSessionEstablished]:[%x]::CStateSessionEstablished() "), this);
	}

CStateSessionEstablished::~CStateSessionEstablished ()
/**
Destructor
*/	
	{
	// Close Logging
	__FLOG_0(_L("[StateSessionEstablished]: CStateSessionEstablished::~CStateSessionEstablished()"));
	__FLOG_CLOSE;
	}
	
CSipStateBase * CStateSessionEstablished::ProcessClientL(TRequestStatus & aStatus)
/**
Function which process the request and responses from the client
*/
	{
	__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessClientL()"), this);
	switch(iClientRequest)
		{
		case TSipHLConsts::ERequestUnRegister:
			{
			__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessClientL() <<REQUEST UNREGISTER>>"), this);
			TRAPD(err,iSipSM->SendUnRegisterMessageL());
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
			}
			break;
		case TSipHLConsts::ERequestUnSubscribe:
			{
			__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessClientL() <<REQUEST UNSUBSCRIBE>>"), this);
			TRAPD(err,iSipSM->SendUnSubscribeMessageL());
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
			}
			break;
		case TSipHLConsts::ERequestAck:
			{
			__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessClientL() <<REQUEST ACK>>"), this);
			TRAPD(err,iSipSM->SendAckForInviteL());
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
				iSipSM->CallEstablishedCallback();	
				}
			}
			break;
		case TSipHLConsts::ERequestBye:
			{
			__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessClientL() <<REQUEST BYE>>"), this);
			TRAPD(err,iSipSM->SendByeL());
			if(err != KErrNone)
				{
				iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
				iSipSM->SetGeneralError(err);
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
				}
			else
				{
				iSipSM->GetTe()->SetTransaction(iSipSM,iSipSM->GetCommandContext().iClientTx);
				iSipSM->SetSuspendRequest(ETrue);
				iNext = this;					
				}
			}
			break;
		default:
			{
			__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessClientL() <<REQUEST DEFAULT>>"), this);
			if(iReqIdentifier == 1)
				{
				iSipSM->SetSuspendRequest(ETrue);
				iNext = this;
				}
			}
		}
	TRequestStatus * stat = &aStatus;		
	User::RequestComplete (stat, KErrNone);
#ifdef __FLOG_ACTIVE	
	// This indormation is required only when logging is enabled
	TSipHLConsts::SIP_STATES	iD = (iNext != NULL)?iNext->GetStateId() : TSipHLConsts::EStateNone;
	if(iD == TSipHLConsts::EStateNone)
		{
		__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessClientL() <<Next State --> NULL>> "), this);
		}
	if(iD == TSipHLConsts::EStateSessionIdle)
		{
		__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessClientL() <<Next State --> StateIdle>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionInitiation)
		{
		__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessClientL() <<Next State --> StateSessionInitiation>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionEstablished)
		{
		__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessClientL() <<Next State --> StateSessionEstablished>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionTerminated)
		{
		__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessClientL() <<Next State --> StateSessionTerminated>> "), this);	
		}
	else
		{
		__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessClientL() <<Next State --> Unknown, its a serious error>> "), this);		
		}
#endif	
	return iNext;
	}

CSipStateBase * CStateSessionEstablished::ProcessServerL( TRequestStatus & aStatus )
/**
Function which process the request and responses from the Serevr
*/
	{
	__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessServerL()"), this);
	TSipMsgBundle SipMsgBundle = iSipSM->GetSipMessageBundle();
	switch(iServerResponse)
		{
		case TSipHLConsts::EResponseNone:
			{
			__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessServerL() <<RESPONSE NONE>>"), this);	
			}
			break;
		case TSipHLConsts::EResponseUnRegister: // This means unregister over here
			{
			__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessServerL() <<RESPONSE UNREGISTER>>"), this);	
			if(SipMsgBundle.iStatusCode != 200)
				{
				iSipSM->SetSIPError(SipMsgBundle.iStatusCode);
				}
			iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
			iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
			iNext->ResetRequestResponse();
			}
			break;
		case TSipHLConsts::EResponseBye:
			{
			__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessServerL() <<RESPONSE BYE>>"), this);	
			if(SipMsgBundle.iStatusCode != 200)
				{
				iSipSM->SetSIPError(SipMsgBundle.iStatusCode);
				}
			iSipSM->SetSuspendRequest(EFalse);
			iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
			iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
			iNext->ResetRequestResponse();
			}
			break;
		case TSipHLConsts::EResponseSubscribe:
			{
			__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessServerL() <<RESPONSE SUBSCRIBE>>"), this);	
			if(SipMsgBundle.iStatusCode != 200)
				{
				iSipSM->SetSIPError(SipMsgBundle.iStatusCode);
				}
			// This is For unsubscribe if comes
			// at this stage
			// After being unsubscribed wait for a final notify
			iDoneUnSubscribe = ETrue;
			iSipSM->SetSuspendRequest(ETrue);
			iNext = this;
			iNext->ResetRequestResponse();		
			}
			break;
		default:
			{
			__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessServerL() <<RESPONSE DEFAULT>>"), this);	
			if(iReqIdentifier == 4)
				{
				iSipSM->SetSuspendRequest(ETrue);
				iNext = this;				
				}
			}
		}
	switch (iServerRequest)
		{
		case TSipHLConsts::ERequestNone:
			{
			__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessServerL() <<REQUEST NONE>>"), this);	
			}
			break;	
		case TSipHLConsts::ERequestBye:
			{
			__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessServerL() <<REQUEST BYE>>"), this);	
			TRAPD(err,iSipSM->SendByeResponseL());
			if(err != KErrNone)
				{
				iSipSM->SetGeneralError(err);
				}
			iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
			iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
			iNext->ResetRequestResponse();
			}
			break;
		case TSipHLConsts::ERequestAck:
			{
			__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessServerL() <<REQUEST ACK>>"), this);	
			iSipSM->CallEstablishedCallback();
			}
			break;
		case TSipHLConsts::ERequestNotify:
			{
			__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessServerL() <<REQUEST NOTIFY>>"), this);	
			HBufC8 * notificationData = iSipSM->GenerateNotificationData();
			TRAPD(err,iSipSM->GenerateSubscribeResponseL(iSipSM->GetCommandContext().iServerTx, TSipHLConsts::EResponse200OK));
			if(err != KErrNone)
				{
				iSipSM->SetGeneralError(err);
				iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
				}
			else
				{
				if (iDoneUnSubscribe)
					{
					iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
					iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
					iNext->ResetRequestResponse();
					}
				else
					{
					iSipSM->SetSuspendRequest(ETrue);
					iNext = this;
					iNext->ResetRequestResponse();		
					}
				}
			// we will not call notification to the client as RSubConnection::Stop() will terminate the 
			// client and we dont have anything to call the event upon
			// This is against the SIP functionality over here, But this is done in view of the current limitations
			// of the rsubconnection APIs, In future this needs to be changes with evolving APIs.
			if(!iDoneUnSubscribe)
				{
				iSipSM->CallNotificationCallback(*notificationData);				
				}
			// delete Notification Data as not required
			delete notificationData;
			notificationData = NULL;
			}
			break;
		default:
			{
			__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessServerL() <<REQUEST DEFAULT>>"), this);	
			if(iReqIdentifier == 2)
				{
				iSipSM->SetSuspendRequest(ETrue);
				iNext = this;					
				}
			}
		}
	TRequestStatus * stat = &aStatus;
	User::RequestComplete(stat, KErrNone);
#ifdef __FLOG_ACTIVE	
	// This indormation is required only when logging is enabled
	TSipHLConsts::SIP_STATES	iD = (iNext != NULL)?iNext->GetStateId() : TSipHLConsts::EStateNone;
	if(iD == KErrNotFound)
		{
		__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessServerL() <<Next State --> NULL>> "), this);
		}
	if(iD == TSipHLConsts::EStateSessionIdle)
		{
		__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessServerL() <<Next State --> StateIdle>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionInitiation)
		{
		__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessServerL() <<Next State --> StateSessionInitiation>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionEstablished)
		{
		__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessServerL() <<Next State --> StateSessionEstablished>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionTerminated)
		{
		__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessServerL() <<Next State --> StateSessionTerminated>> "), this);	
		}
	else
		{
		__FLOG_1(_L("[StateSessionEstablished]:CStateSessionEstablished[%x]::ProcessServerL() <<Next State --> Unknown, its a serious error>> "), this);		
		}
#endif	
	return iNext;	
	}

CSipStateBase * CStateSessionEstablished::ProcessServerErrorL(TRequestStatus & aStatus)
/**
Handles the Error condition for this state from the server
*/
	{
	__FLOG_1(_L("[StateSessionEstablished]:StateSessionEstablished[%x]::ProcessServerErrorL()"), this);	
	TSipMsgBundle SipMsgBundle = iSipSM->GetSipMessageBundle();
	// Call the call back to Notify the client
	iSipSM->SetGeneralError(SipMsgBundle.iErrCode);
	iSipSM->SetSIPError(SipMsgBundle.iStatusCode);
	iSipSM->CallTerminateCallback();
	iSipSM->SetCallTerminateCallBack(EFalse);
	// Get to The terminated state
	iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
	iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
	if(iNext)
	{
	    iNext->ResetRequestResponse();
	}
	TRequestStatus * stat = &aStatus;
	User::RequestComplete(stat, KErrNone);
#ifdef __FLOG_ACTIVE	
	// This indormation is required only when logging is enabled
	TSipHLConsts::SIP_STATES	iD = (iNext != NULL)?iNext->GetStateId() : TSipHLConsts::EStateNone;
	if(iD == KErrNotFound)
		{
		__FLOG_1(_L("[StateSessionEstablished]:StateSessionEstablished[%x]::ProcessServerErrorL() <<Next State --> NULL>> "), this);
		}
	if(iD == TSipHLConsts::EStateSessionIdle)
		{
		__FLOG_1(_L("[StateSessionEstablished]:StateSessionEstablished[%x]::ProcessServerErrorL() <<Next State --> StateIdle>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionInitiation)
		{
		__FLOG_1(_L("[StateSessionEstablished]:StateSessionEstablished[%x]::ProcessServerErrorL() <<Next State --> StateSessionInitiation>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionEstablished)
		{
		__FLOG_1(_L("[StateSessionEstablished]:StateSessionEstablished[%x]::ProcessServerErrorL() <<Next State --> StateSessionEstablished>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionTerminated)
		{
		__FLOG_1(_L("[StateSessionEstablished]:StateSessionEstablished[%x]::ProcessServerErrorL() <<Next State --> StateSessionTerminated>> "), this);	
		}
	else
		{
		__FLOG_1(_L("[StateSessionEstablished]:StateSessionEstablished[%x]::ProcessServerErrorL() <<Next State --> Unknown, its a serious error>> "), this);		
		}
#endif	
	return iNext;
	}


