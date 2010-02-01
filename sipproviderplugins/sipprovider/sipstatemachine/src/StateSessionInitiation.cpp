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
// CStateSessionInitiation implementation file.
// 
//

/**
 @file
 @internalComponent
*/
 
#include <e32std.h>
#include <e32base.h>

#include "StateSessionInitiation.h"

CStateSessionInitiation * CStateSessionInitiation::NewL( CSipStateMachine * aStateMachine )
/**
Instantiaion static function
*/
	{
	return new(ELeave) CStateSessionInitiation (aStateMachine);
	}

CStateSessionInitiation::CStateSessionInitiation ( CSipStateMachine * aStateMachine ) :
CSipStateBase ( aStateMachine, TSipHLConsts::EStateSessionInitiation )
/**
Constuctor
*/
	{
	// Open Logs
	__FLOG_OPEN(_L8("SipStateMachine"), _L8("StateSessionInitiation"));
	__FLOG_1(_L("[StateSessionInitiation]:[%x]::CStateSessionInitiation() "), this);
	}

CStateSessionInitiation::~CStateSessionInitiation ()
/**
Destructor
*/	
	{
	// Close Logging
	__FLOG_0(_L("[StateSessionInitiation]: CStateSessionInitiation::~CStateSessionInitiation()"));
	__FLOG_CLOSE;
	}

CSipStateBase * CStateSessionInitiation::ProcessClientL( TRequestStatus & aStatus )
/**
Function which process the request and responses from the client
*/
	{
	__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessClientL()"), this);
	switch(iClientRequest)
		{
		case TSipHLConsts::ERequestBye:
		case TSipHLConsts::ERequestCancel:
			{
			__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessClientL() <<REQUEST BYE/ REQUEST CANCEL>>"), this);
			TRAPD(err,iSipSM->SendCancelL());
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
				iNext->ResetRequestResponse();		
				}
			}
			break;
		default:
			{
			__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessClientL() <<REQUEST DEFAULT>>"), this);
			if(iReqIdentifier == 1)
				{
				iSipSM->SetSuspendRequest(ETrue);	
				iNext = this;					
				}
				
			}
		}
	// Handle Response from client
	switch(iClientResponse)
		{
		case  TSipHLConsts::EResponseInvite:
			{
			__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessClientL() <<RESPONSE INVITE>>"), this);
			TRAPD(err,iSipSM->GenerateInviteResponseL(iSipSM->GetCommandContext().iServerTx, TSipHLConsts::EResponse200OK));
			if(err != KErrNone)
				{
				iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
				iSipSM->SetGeneralError(err);
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
				}
			else
				{
				// expecting an ACK
				iSipSM->SetSuspendRequest(ETrue);
				iNext = this;
				iNext->ResetRequestResponse();	
				}
			}
			break;
		case TSipHLConsts::EResponseInviteReject:
			{
			__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessClientL() <<RESPONSE INVITE REJECT>>"), this);
			TRAPD(err,iSipSM->GenerateInviteResponseL(iSipSM->GetCommandContext().iServerTx, TSipHLConsts::EResponseReject));
			if(err != KErrNone)
				{
				iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
				iSipSM->SetGeneralError(err);
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
				}
			else
				{
				iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
				iNext->ResetRequestResponse();
				}
			}
			break;
		default:
			{
			__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessClientL() <<RESPONSE DEFAULT>>"), this);
			if(iReqIdentifier == 3)
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
	if(iD == TSipHLConsts::EStateNone)
		{
		__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessClientL() <<Next State --> NULL>> "), this);
		}
	if(iD == TSipHLConsts::EStateSessionIdle)
		{
		__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessClientL() <<Next State --> StateIdle>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionInitiation)
		{
		__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessClientL() <<Next State --> StateSessionInitiation>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionEstablished)
		{
		__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessClientL() <<Next State --> StateSessionEstablished>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionTerminated)
		{
		__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessClientL() <<Next State --> StateSessionTerminated>> "), this);	
		}
	else
		{
		__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessClientL() <<Next State --> Unknown, its a serious error>> "), this);		
		}
#endif	
	return iNext;	
	}

CSipStateBase * CStateSessionInitiation::ProcessServerL( TRequestStatus & aStatus )
/**
Function which process the request and responses from the Server
*/
	{
	__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerL()"), this);
	TSipMsgBundle SipMsgBundle = iSipSM->GetSipMessageBundle();
	// Handle Response from Server
	switch (iServerResponse)
		{
		case TSipHLConsts::EResponseInvite:
			{
			__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerL() <<RESPONSE INVITE>>"), this);
			iSipSM->SetOutgoingStatus(EFalse);
			if(SipMsgBundle.iStatusCode == 200)
				{
				__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerL() <<200 OK>>"), this);
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionEstablished);
				iNext->ResetRequestResponse();
				iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
				iNext->SetClientRequest(TSipHLConsts::ERequestAck);
				}
			else if(SipMsgBundle.iStatusCode == 487)
				{
				__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerL() <<487 Request Terminated>>"), this);
				iSipSM->SetSIPError(SipMsgBundle.iStatusCode);
				iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
				iNext->ResetRequestResponse();
				}
			else
				{
				__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerL() <<Error neither 200 nor 487>>"), this);
				iSipSM->SetSIPError(SipMsgBundle.iStatusCode);
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
				}
			}
			break;
		case TSipHLConsts::EResponseCancel:
			{
			__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerL() <<RESPONSE CANCEL>>"), this);
			if(SipMsgBundle.iStatusCode == 200)
				{
				__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerL() <<200 OK>>"), this);
				iSipSM->SetSuspendRequest(ETrue);
				iNext = this;
				}
			else
				{
				__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerL() <<Not 200 OK for cancel>>"), this);
				iSipSM->SetSIPError(SipMsgBundle.iStatusCode);
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
				}
			}
			break;
		default:
			{
			__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerL() <<RESPONSE DEFAULT>>"), this);
			if(iReqIdentifier == 4)
				{
				iSipSM->SetSuspendRequest(ETrue);
				iNext = this;
				}
			}
		}
	// Handle request from server		
	switch(iServerRequest)
		{
		case TSipHLConsts::ERequestCancel:
			{
			__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerL() <<REQUEST CANCEL>>"), this);
			iSipSM->GetCommandContext().iAckCancel = ETrue;
			// This is not seems to be correct , but stack is not observing
			// standard behavior
			iSipSM->SetSIPStimulus(TSipHLConsts::EClientStimulus);
			iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
			iNext->ResetRequestResponse();	
			}
			break;
		case TSipHLConsts::ERequestBye:
			{
			__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerL() <<REQUEST BYE>>"), this);
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
			__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerL() <<REQUEST ACK>>"), this);
			if(iSipSM->GetCommandContext().iAckCancel)
				{
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionTerminated);
				}
			else
				{
				iSipSM->CallEstablishedCallback();
				iNext = iSipSM->GetNextState(TSipHLConsts::EStateSessionEstablished);
				iNext->ResetRequestResponse();
				iSipSM->SetSuspendRequest(ETrue);
				}
			}
			break;
		default:
			{
			__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerL() <<REQUEST DEFAULT>>"), this);
			if(iReqIdentifier == 2)
				{
				iSipSM->SetSuspendRequest(ETrue);
				iNext = this;				
				}
			}
		}
	TRequestStatus * stat = &aStatus;		
	User::RequestComplete(stat , KErrNone);
#ifdef __FLOG_ACTIVE	
	// This indormation is required only when logging is enabled
	TSipHLConsts::SIP_STATES	iD = (iNext != NULL)?iNext->GetStateId() : TSipHLConsts::EStateNone;
	if(iD == KErrNotFound)
		{
		__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerL() <<Next State --> NULL>> "), this);
		}
	if(iD == TSipHLConsts::EStateSessionIdle)
		{
		__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerL() <<Next State --> StateIdle>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionInitiation)
		{
		__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerL() <<Next State --> StateSessionInitiation>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionEstablished)
		{
		__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerL() <<Next State --> StateSessionEstablished>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionTerminated)
		{
		__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerL() <<Next State --> StateSessionTerminated>> "), this);	
		}
	else
		{
		__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerL() <<Next State --> Unknown, its a serious error>> "), this);		
		}
#endif	
	return iNext;	
	}

CSipStateBase * CStateSessionInitiation::ProcessServerErrorL(TRequestStatus & aStatus)
/**
Handles the Error condition for this state from the serevr
*/
	{
	__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerErrorL()"), this);
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
		__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerErrorL() <<Next State --> NULL>> "), this);
		}
	if(iD == TSipHLConsts::EStateSessionIdle)
		{
		__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerErrorL() <<Next State --> StateIdle>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionInitiation)
		{
		__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerErrorL() <<Next State --> StateSessionInitiation>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionEstablished)
		{
		__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerErrorL() <<Next State --> StateSessionEstablished>> "), this);
		}
	else if(iD == TSipHLConsts::EStateSessionTerminated)
		{
		__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerErrorL() <<Next State --> StateSessionTerminated>> "), this);	
		}
	else
		{
		__FLOG_1(_L("[StateSessionInitiation]:CStateSessionInitiation[%x]::ProcessServerErrorL() <<Next State --> Unknown, its a serious error>> "), this);		
		}
#endif	
	return iNext;
	}



