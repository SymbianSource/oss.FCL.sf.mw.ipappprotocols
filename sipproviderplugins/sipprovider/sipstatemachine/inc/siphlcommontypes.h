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
// Type definition file for SIP HLAPI
// 
//

/**
 @file
 @internalComponent
*/
 
#ifndef SIP_HLCOMMON_TYPES_H
#define SIP_HLCOMMON_TYPES_H

#include <siptransactionbase.h>
#include <sipdialogassocbase.h>
#include <comms-infras/commsdebugutility.h>

class CSipStateMachine;



const TInt KSIPProfileNotFound=18000;
//Indicates an Invalid Profile Id. Valid profile ids start from 2
const TUint32 KSIPInvalidProfileId = 0;
//A High value to indicate a profile id when no profile is passed in connprefs
const TUint32 KSIPDefaultProfileId = 18001;
const TUint KMaxUriSize = 128;
const TUint KMaxContentSize = 1024;

// CommsDebugUtility logging tags. Use them to enable tracing for SipStateMachine
__FLOG_STMT(_LIT8(KSIPStateMachineLogTag1,"SIPSM");)
__FLOG_STMT(_LIT8(KSIPStateMachineLogTag2,"SIP");)

class TSipHLConsts
/**
This class will hold the High Level API constants 

@internalComponent
@released since v9.2
*/
	{
public:	
	/**
	This enumeration will tell what the state that has to be loaded next
	in order to do a SIP call
	*/	
	enum SIP_STATES
		{
		EStateNone,
		EStateSessionIdle,
		EStateSessionInitiation,
		EStateSessionEstablished,
		EStateSessionTerminated
		};

	/**
	This enumeration will configure the type of request received 
	*/
	enum SIP_REQUESTS
		{
		ERequestNone,
		ERequestRegister,
		ERequestUnRegister,
		ERequestInvite,
		ERequestSubscribe,
		ERequestUnSubscribe,
		ERequestNotify,
		ERequestAck,
		ERequestCancel,
		ERequestBye
		};
		
	/**
	This enumeration will configure the type of request received 
	*/
		
	enum SIP_RESPONSES
		{
		EResponseNone,
		EResponseRegister,
		EResponseUnRegister,
		EResponseInvite,
		EResponseInviteReject,
		EResponseAck,
		EResponseBye,
		EResponseSubscribe,
		EResponseCancel
		};
	/**
	This enumeration will identify the type of Invite/Other response that will be
	send by the server
	*/
	enum SIP_RESPONSE_TYPE
		{
		EResponseTrying,
		EResponseRinging,
		EResponseSessionProgress,
		EResponse200OK,
		EResponseReject
		};
	/**
	This enumeration will have the state machine stimulus according to which 
	the SM will function
	*/
	enum SIP_SM_STIMULUS
		{
		EClientStimulus, 
		EServerStimulus	
		};	
		
	}; // end of class TSipHLConsts

class TSipMsgBundle
/**
This class will hold the Message bundle that will be filled by the TE
while it is handed over to the state machine for processing

@internalComponent
@released since v9.2
*/
	{
public:
	TInt  						iStatusCode;
	TInt						iErrCode;
	TSipHLConsts::SIP_REQUESTS	iRequest;
	TSipHLConsts::SIP_RESPONSES	iResponse;
	CSIPServerTransaction*		iServTransaction;
	CSIPClientTransaction*		iClientTransaction;
	CSIPDialogAssocBase*		iDialog;
	};

class CHLStateMachine	: 	public CBase
	{
/**
This class will hold the informations that will be required to uniquely match
the State machine with a specific dialog and Transaction. This class holds the 
instance of the state machine along with associated transaction and dialog

@internalComponent
@released since v9.2
*/
public:
	CSIPTransactionBase*	iTransaction;
	CSIPDialogAssocBase*	iDialog;
	CSipStateMachine*		iStateMachine;
	};


class TRegisterParams 
/**
This class contains the parameters that is needed for a simple IETF registration

@internalComponent
@released since v9.2
*/
	{
public:	
	TPtrC8	iRegistrar;
	TPtrC8	iTo;
	TPtrC8	iFrom;
	TPtrC8	iContact;
	};
	
class TInviteParams
/**
This class contains the parameters Required to invite the other party 
for a call

@internalComponent
@released since v9.2
*/
	{
public:	
	TPtrC8	iReqUri;
	TPtrC8	iTo;
	TPtrC8	iFrom;
	TPtrC8	iContact;
	TPtrC8	iContentType;
	TPtrC8	iContentSubType;
	TPtrC8	iContent;
	};
	
class TSubscribeParams
/**
This class contains the parameters that is required to subscribe for an 
event using SIP subscribe method

@internalComponent
@released since v9.2
*/
	{
public:
	TPtrC8	iReqUri;
	TPtrC8	iTo;
	TPtrC8	iFrom;
	TPtrC8	iContact;
	TPtrC8	iEventType;
	TPtrC8	iAcceptType;
	TPtrC8	iAcceptSubType;
	TInt	iExpires;
	TBool   iAutoRefresh;
	};

class TSipParams
/**
Since at a time a State machine can only do a registration, Invitation, or a
subscription. so a Union of all parameters have been created and only one will
be used at any point of time

@internalComponent
@released since v9.2
*/
	{
public:	
	TRegisterParams 	iRegParams;
	TInviteParams		iInvParams;
	TSubscribeParams	iSubsParams;
	TSipHLConsts::SIP_REQUESTS iRequest;
	};


class TIncomingCallParameters
/**
This class carry a parameters that describe a (incoming or outgoing)
call.

@internalComponent
@released since v9.2
*/
	{
public:
	TPtrC8  iReqUri;
    TPtrC8	iFrom;
	TPtrC8	iTo;
	TPtrC8	iContent;
	};
	
class TUriHolder
/**
This class will server as a temporary place holder of the Uris that will 
be passed onto the incoming parameters.

@internalComponent
@released since v9.2
*/

	{
public:
	TBufC8<KMaxUriSize>	iReqUri;
	TBufC8<KMaxUriSize>	iFrom;
	TBufC8<KMaxUriSize>	iTo;	
	TBufC8<KMaxContentSize>	iContent;		
	};	

class TSIPCredentials
/**
This class will contain credentials
Realm, UserName and Passwd
*/
	{
public:	
	TPtrC8	iRealm;
	TPtrC8	iUserName;
	TPtrC8	iPasswd;
	};

#endif
