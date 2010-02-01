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
// CSipStateMachine definition file
// 
//

/**
 @file
 @internalComponent
*/


#ifndef SIP_STATEMACHINE_H
#define SIP_STATEMACHINE_H

#include "transitionengine.h"
#include "SipStateBase.h"
#include "siphlcommontypes.h"

#ifdef __FLOG_ACTIVE
	// This will do the logging if logging is enabled
    #include <comms-infras/ss_log.h>
#endif


// Forward Declarations
class CStateIdle;
class CStateSessionInitiation;
class CStateSessionEstablished;
class CStateSessionTerminate;
class CSipStateMachine;


class TCommandContext
/**This class represents the Handles of the SIP stack that is required
for a particular state machine. Using these handles the State machine
interact with the Stack to achieve a particular functionality

@internalComponent
@released since v9.2
 */
	{
public:
	CSipStateBase * 			iCommandState;
	CSIPClientTransaction*		iClientTx;
	CSIPClientTransaction*		iCancelClientTx;
	CSIPServerTransaction*		iServerTx;
	CSIPInviteDialogAssoc*		iDialogAssoc;
	CSIPRegistrationBinding*	iRegBinding;
	CSIPSubscribeDialogAssoc*	iSubsDialogAssoc;
	TBool						iAckCancel;
	};

class TCallTerminateCode
/** This class will contain the call termiated error codes that needs to be sent to
the calling application
*/
	{
public:
	TInt	iErrorCode;
	TInt	iSipCode;
	};


class MSIPStateMachineClient
/**
To be implemented by whoever uses CSipStateMachine and wishes to
be notified about completions and events.

@internalComponent
@released since v9.2
*/
	{
	public:

	/**
	Called by the State Machine that call has been established
	*/
	virtual void CallEstablished() = 0;

	/**
	Called by the State Machine when the running or being established
	call has been terminated. Can either:
	- acknowledge local termination
	- flag termination by the peer
	- flag error during the call or during its establishment

	@param aError system wide error, SIP proprietary error or KErrNoError if no error.
	*/
	virtual void CallTerminated(TInt aError, TInt aSipCode) = 0;

	/**
	Called by the State Machine to flag that a peer is calling.

    @param aCallParams the description of the call.
	*/
	virtual void IncomingCall(TIncomingCallParameters& aCallParams) = 0;

	/**
	Called by the State Machine to flag that credentials are
	required for the pending call to proceed.

    @param aRealm the description of the credentials
	*/
	virtual void CredentialsRequired(const TDesC8 & aRealm) = 0;

	/**
	Called by the state machine when some notifications arrived

	@param aNotification contains notification Information
	*/
	virtual void ReceiveNotification(TDesC8 & aNotification) = 0;
	};


class CSipStateMachine : public CActive
/**
This class contains the complete logic for all SIP realted functionalities supported
by this release of High Level API.

@internalComponent
@released since v9.2
*/

	{
public:
	/**
	Static Member furnction to give life to Sip State Machine

	@param aTe Handle to the Transition engine, whicn encapsulated the sip stack.
	@param aClient, Handle to the Notification client for callback notifications
	*/
	IMPORT_C static CSipStateMachine * NewL(CSIPTransitionEngine * aTe, MSIPStateMachineClient* aClient, TBool aSMDirection );

	/**
	This method is responsible for Answering an Incoming call. With call to this
	method the client is accepting a call, and call will be established. After this
	only StopCall() will work, not RejectCall();
	*/
	IMPORT_C TInt AcceptCall();

	/**
	This method is responsible Starting an outgoing call.
	*/
	IMPORT_C TInt StartCall();

	/**
	This method is responsible to terminate an ongoing call.
	*/
	IMPORT_C TInt StopCall();

	/**
	This method Rejects an incoming call.
	*/
	IMPORT_C TInt RejectCall();

	/**
	This function will Return the handle to the Sip Paramters of the state machine
	*/
	IMPORT_C TSipParams & GetSipParams();

	/**
	This method paves the way for the state machine to delete itself
	*/
	IMPORT_C void DeleteWhenReady();

	/**
    Implementing Pure virtual of Base class , cancels any outstanding request
    */
   	virtual void DoCancel();

   	/**
    Implementing Pure virtual of Base class. This function calls the functionality
    associated with the states
    */
	virtual void RunL();

	/**
	Overriding the base member function, will get called in case of an error
	*/
	virtual TInt RunError(TInt aError);

	/**
    Set the suspend state of the State machine. Using this call one can bring
    state machine into suspended mode from active mode and vice versa

    @param aSuspendRequest Tells whether to put state machine into suspended or active mode
    */
    void SetSuspendRequest(TBool aSuspendRequest);

    /**
    This function will actually start the SIP State machine.

    @param aClientStatus Current status of the State machine Task
    @param aErrorEvent currently not implemented, for future use
    */
    void Start( TRequestStatus* aClientStatus, CSipStateBase* aErrorEvent);

	/**
	Function will be called in The RunL gets executed in event of an
	error
	*/
	void OnError();

	/**
	This function is meant for cleanup activities and get called when the
	state machine is about to complete.
	*/
	void OnCompletion();

	/**
	This function gets triggered from the TE in case a SIP Response comes for
	this state machine.

	@param aMsgBundle Message specific to Handle SIP request or response
	*/
	void IncomingResponse(TSipMsgBundle aMsgBundle);

	/**
	This function gets triggered from the TE in case a SIP Request comes for
	this state machine.

	@param aMsgBundle Message specific to Handle SIP request or response
	*/
	void IncomingRequest(TSipMsgBundle aMsgBundle);

	/**
	This function gets triggered from the TE when an error is encounterd pertaining
	to this state machine request or response.

	@param aErrBundle Message specific to Handle SIP request or response
	*/
	void ErrorOccured(TSipMsgBundle aErrBundle);

	/**
	This function gets triggered from the TE after around 3-4 minutes when a call is
	established. This is meant for cleaning up of the client transaction. In some case
	a call can ne terminated before this function can gets called. In that case the client
	transaction will be deleted before call termination and this function implementation
	will not make any sense.

	@param aTransaction Handle to the client transaction that should be deleted
	*/
	void InviteCompleted(CSIPClientTransaction& aTransaction);

	/**
	This function will generate the response for the incoming invite

	@param aTransaction Sever Transaction using which the response must be sent
	@param aType The type of responses that must be sent by this function call
	*/
	void GenerateInviteResponseL(CSIPServerTransaction *& aTransaction,TSipHLConsts::SIP_RESPONSE_TYPE aType);

	/**
	This function will generate the response for the incoming subscribe method

	@param aTransaction Sever Transaction using which the response must be sent
	@param aType The type of responses that must be sent by this function call
	*/
	void GenerateSubscribeResponseL(CSIPServerTransaction *& aTransaction,TSipHLConsts::SIP_RESPONSE_TYPE aType);

	/**
	This function will get called in case any request comes when a dialog is
	established, and the incoming request matched to the existing dialog

	@param aMsgBundle Message specific to Handle SIP request or response
	*/
	void IncomingRequestInsideDialog(TSipMsgBundle aMsgBundle);

	/**
	This function will get called in case any request comes doesnt matches
	with any of the existing dialog

	@param aMsgBundle Message specific to Handle SIP request or response
	*/
	void IncomingRequestOutsideDialog(TSipMsgBundle & aMsgBundle);

	/**
	This function will scheduler the state machine RunL to be called in next cycle
	in not alreay active
	*/
	void MakeActive ();

	/**
	This function will issue a request complete onto itself and force the RunL method
	of the state machine to get called

	@param aErrCode Current status of the state machine
	*/
	void MarkRequestComplete(TInt aErrCode);

	/**
	This function will return the Handle to the Transition engine that is with
	the current state machine
	*/
	CSIPTransitionEngine *GetTe();

	/**
	This function will return the pointer to the currently active state
	*/
	CSipStateBase * GetActiveState();

	/**
	This function will call the terminated callback to the application client
	*/
	void CallTerminateCallback();

	/**
	This function will notify the client about incoming calls
	*/
	void CallIncomingCallback();

	/**
	This function will call the extablished callback onto the client
	*/
	void CallEstablishedCallback();

	/**
	This functin will call the notification callback onto the client
	*/
	void CallNotificationCallback(TPtrC8 aNotifyData);

	/**
	This function will returns the handle to the state requested

	@param aNextState Handle to the request which is to be returned
	*/
	CSipStateBase * GetNextState (TSipHLConsts::SIP_STATES aNextState);

	/**
	This function returns the First/Oldesst element of the SIP message Bundle
	*/
	TSipMsgBundle	GetSipMessageBundle();

	/**
	This function tells the state machine that it is being called from upper layer
	or the lower layer. Upper Layer being the client which instantiated the state
	machine and lower layer is the TE, which call State machine to either give some
	information to the user or to get some functionality done

	@param aStimulus Identifier of the upper or lower layer
	*/
	void	SetSIPStimulus(TSipHLConsts::SIP_SM_STIMULUS aStimulus);

	/**
	This function will return the command context of the state machine
	*/
	TCommandContext & GetCommandContext();

	/**
	This function will return whether the state machine is for handling incoming call
	*/
	TBool GetIncomingStatus();

	/**
	This function will return ETrue if thsi state machine represents
	the current active outgoing session
	*/
	TBool GetOutgoingStatus();

	/**
	This function will reset the outgoing status of the state machine
	*/
	void SetOutgoingStatus(TBool aStatus);

	/**
	This function will do cleanup activities before terminting the state machine
	*/
	void Cleanup();

	/**
	This function will invite the functionalities of doing a sip registration
	*/
	void SendRegisterMessageL();

	/**
	This function will invite the functionalities of doing a sip Unregistration
	*/
	void SendUnRegisterMessageL();

	/**
	This function will invite the functionalities of doing a sip Invite
	*/
	void SendInviteMessageL();

	/**
	This function will Send the acknowledgement of the Sucess response of
	the Invitation.
	*/
	void SendAckForInviteL();

	/**
	This function will send the cancellation for the onging invite
	*/
	void SendCancelL();

	/**
	This function will terminte the ongoing session by sending a SIP
	bye
	*/
	void SendByeL();

	/**
	This function will send the response of an incoming bye from remote user
	*/
	void SendByeResponseL();

	/**
	This function will invite the functionalities of doing a sip Subscribe
	*/
	void SendSubscribeMessageL();

	/**
	This function will invite the functionalities of doing a sip UnSubscribe
	*/
	void SendUnSubscribeMessageL();
	/**
	This function will set the status of call terminate call back

	@param aCallTerminateCallBack
	*/
	void SetCallTerminateCallBack(TBool aCallTerminateCallback);

	/**
	This method is called by the TransditionEngine when
	it ignores a challenge recieved on a session represented by
	this StateMachine

	@param aRealm - The realm on which the challenge was recieved
	*/
	void	ChallengeIgnored(const TDesC8& aRealm);

	/**
	This function will set the error that occurred in processing of SIP Message

	@param aError - SIP Error Message
	*/
	void SetGeneralError(TInt aError);

	/**
	This function will set the SIP Specific codes

	@param aError - SIP Message code
	*/
	void SetSIPError(TInt aError);

	/**
	This function copy the notification data from the Server Transaction
	*/
	HBufC8 * GenerateNotificationData();

	/**
	Destructor of the class
	*/
	~CSipStateMachine();

private:

	/**
	The constructor of the class.

	@param aTe Handle to the Transition Engine
	@param aClient Handle to the callback Notifications
	*/
	CSipStateMachine(CSIPTransitionEngine * aTe, MSIPStateMachineClient* aClient, TBool aSmDirection);

	/**
	2nd Phase construction method of the State machine, which actually starts the
	state machine
	*/
	void ConstructL();

	/**
	This method is to give instructions pertaining to SIP request to the state
	machine

	@param aRequest Enumeration which defines a set of Requests that are supported
	*/
	TInt SendSIPRequest(TSipHLConsts::SIP_REQUESTS aRequest);

	/**
	This method is to respond for any incoming request that comes to the client
	and need user response, e.g Answering an Incoming call

	@param aResponse Enumeration which defines a set of supported responses
	*/
	TInt SendSIPResponse(TSipHLConsts::SIP_RESPONSES aResponse);

	/**
	This function will Create the TO header from the descriptor

	@param aSipUri The Uri which sould constitute the to header
	*/
	CSIPToHeader* CreateToHeaderLC( const TDesC8& aSipUri );

	/**
	This function will Create the From header from the descriptor

	@param aSipUri The Uri which sould constitute the from header
	*/
	CSIPFromHeader* CreateFromHeaderLC( const TDesC8& aSipUri );

	/**
	This function will Create SDP body from the CSdpDocument

	@param aContent The pointer from which content body should be made
	*/
	HBufC8* GetContentBodyL( CSdpDocument* aContent );

	/**
	This function generates the incoming call parameters from the server
	transaction available with the current state machine

	@param aParam store the incoming call parameters
	*/
	void GenerateIncomingCallParameters(TIncomingCallParameters & aParam);


private:
	// For Logging
	__FLOG_DECLARATION_MEMBER;

	// True when the SM recieves call from the client
	TBool iClientStimulus;

	// True when the SM recieves the call from TE
	TBool iServerStimulus;

	// True in case of error, can be either through client or TE
	TBool iServerErrorStimulus;

	// set the status of the State machine
	TBool iSuspendRequest;

	// Array of Sip Message bundles
	RArray <TSipMsgBundle> 	iSipMsgBundle;

	// Current Request Status of the state machine
	TRequestStatus * iClientStatus;

	// Handle to the active state of the state machine
	CSipStateBase  * iActiveState;

	// handle to the error state of the state machine
	CSipStateBase  * iErrorState;

	// handle to the command context of this state machine
	TCommandContext iCommandCntx;

	// Handle to the current parameter set for this state machine
	TSipParams	iSipParams;

	// Handle to the Transition engine
	CSIPTransitionEngine 	 * iTe;

	// handle to the Idle State
	CStateIdle  			 * iStateIdle;

	// handle to the Session Initialisation State
	CStateSessionInitiation  * iStateSessionInit;

	// handle to the Session Established State
	CStateSessionEstablished * iStateSessionEstablished;

	// handle to the Terminated state
	CStateSessionTerminate	 * iStateSessionTerminate;

	// Handle to the callback clinet for SIP notifications
	MSIPStateMachineClient		 * iClient;

	// Handle to Values that should be passed onto the client
	TUriHolder	iUriValues;

	// This contains the Error values to be sent to Calling application
	TCallTerminateCode	iCallTerminateCode;

	// This variable will mark the state machine to be used only
	// for incoming calls
	TBool	iIncomingStateMachine;

	// This variable will tell whther to call the Terminate notification
	// when the state is going to terminated state
	TBool	iCallTerminateCallback;

	//This flag indicates if this StateMachine represents the current
	//active outgoing session
	TBool 	isActiveOutgoingSession;

	// This flag will tell whether the state machine is a Invite state machine
	TBool	isInviteSM;

	// This flag will tell whether the state machine is a subscribe state machine
	TBool	isSubscribeSM;

	// This flag will tell the state machine whether to delete it or not
	TBool	iDeleteMeNow;
	};



inline void CSipStateMachine::SetSuspendRequest( TBool aSuspendRequest )
/**
 */
   {
   iSuspendRequest = aSuspendRequest;
   }


inline TSipMsgBundle CSipStateMachine::GetSipMessageBundle()
/**
 */
 	{
 	TSipMsgBundle  CurrBundle;
	CurrBundle.iStatusCode = 0;
        CurrBundle.iErrCode = 0;
	CurrBundle.iRequest = TSipHLConsts::ERequestNone;
	CurrBundle.iResponse = TSipHLConsts::EResponseNone;
	CurrBundle.iServTransaction = 0;
	CurrBundle.iClientTransaction = 0;
	CurrBundle.iDialog = 0;
 	if (iSipMsgBundle.Count() > 0)
 		{
 		CurrBundle = iSipMsgBundle[0];
 		iSipMsgBundle.Remove(0);
 		}
 	else
 		{
 		CurrBundle.iStatusCode = -1;
 		}
 	return CurrBundle;
 	}

inline void	CSipStateMachine::SetSIPStimulus(TSipHLConsts::SIP_SM_STIMULUS aStimulus)
/**
 */
	{
	if (aStimulus == TSipHLConsts::EClientStimulus)
		{
		iClientStimulus = ETrue;
		}
	if (aStimulus == TSipHLConsts::EServerStimulus)
		{
		iServerStimulus = ETrue;
		}
	}


inline TCommandContext & CSipStateMachine::GetCommandContext()
/**
 */
 	{
 	return iCommandCntx;
 	}

inline void CSipStateMachine::SetCallTerminateCallBack(TBool aCallTerminateCallBack)
/**
 */
	{
	iCallTerminateCallback = aCallTerminateCallBack;
	}

inline void CSipStateMachine::SetGeneralError(TInt aError)
/**
 */
 	{
 	iCallTerminateCode.iErrorCode = aError;
 	}

inline void CSipStateMachine::SetSIPError(TInt aError)
/**
 */
 	{
 	iCallTerminateCode.iSipCode = aError;
 	}


#endif

