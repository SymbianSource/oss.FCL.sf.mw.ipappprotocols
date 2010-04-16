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
// CSIPTransitionEngine implementation file.
// 
//

/**
 @file
 @internalComponent
*/
 

#include "transitionengine.h"
#include "SipStateMachine.h"
#include <siphlerr.h>


CSIPTransitionEngine* CSIPTransitionEngine::NewL(CSIP& aSip, TInt aIapId)
/**
Transition Engine Instantiation Function
*/
	{	
	CSIPTransitionEngine * self = new (ELeave) CSIPTransitionEngine(aSip, aIapId);
	CleanupStack::PushL (self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;	
	}
 

void CSIPTransitionEngine::ConstructL()
/**
2nd Phase construction
*/
	{
	__FLOG_1(_L("CSIPTransitionEngine %08x:\tConstructL(), expect 'ConstructL successful', otherwise the method left"), this);
	ASSERT(iConnection == NULL);
	
	// Startup Nokia Stack Related Objects
	iConnState = CSIPConnection::EInactive;

	__FLOG_1(_L("CSIPTransitionEngine %08x:\tInstantiating CSIPProfileRegistry - may leave"), this);
	iProfileRegistry = CSIPProfileRegistry::NewL(iSIP, *this);
	__FLOG_1(_L("CSIPTransitionEngine %08x:\tInstantiating CSIPHttpDigest - may leave"), this);
	iDigest = CSIPHttpDigest::NewL(iSIP, *this );
	__FLOG_1(_L("CSIPTransitionEngine %08x:\tInstantiating CSIPConnection - may leave"), this);
    iConnection = CSIPConnection::NewL(iSIP, iIapId, *this );

	// Wait for the connectio to become Active 
	if( iConnection->State() == CSIPConnection::EInit )
		{
		iActiveWait.Start();
		}
		
	__FLOG_1(_L("CSIPTransitionEngine %08x:\tConstructL successful"), this);		
	}
	
CSIPTransitionEngine::CSIPTransitionEngine(CSIP& aSip, TInt aIapId)
:iIapId(aIapId),
 iSIP(aSip)
/**
constructor
*/
	{
	// Open Logs
	__FLOG_OPEN(_L8("SipStateMachine"), _L8("TransitionEngine"));
	__FLOG_1(_L("[TransitionEngine]:[%x]::CSIPTransitionEngine() "), this);	
	}
	
	
CSIPTransitionEngine::~CSIPTransitionEngine()
/**
Destructor , cleanup wil be done here
*/
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine [%x]::~CSIPTransitionEngine"), this);
	// Delete all the State Machines that are there registered
	// with the TE
	CHLStateMachine *hlSm = NULL;
	TInt numElements = iSMArray.Count();
	for(TInt count=0; count<numElements; count++)
		{
		hlSm = (CHLStateMachine*)iSMArray[count];
		hlSm->iDialog = NULL;
		hlSm->iTransaction = NULL;
		delete hlSm;
		}
	iSMArray.Close();
	
	// Clean Profiles
	numElements = iProfileArray.Count();
	for (TInt count = 0; count<numElements; count++)
		{
		CSIPProfile* profile = iProfileArray[count];
		delete profile;
		}
	iProfileArray.Close();
	
	//Close RegClientArray
	//TransitionEngine will be deleted only when no SIPConnProviders are using this
	iRegClientArray.Close();
	
	delete iProfileRegistry;
	delete iConnection;
    __FLOG_1(_L("<<completed...>> CSIPTransitionEngine [%x]::~CSIPTransitionEngine"), this);
	__FLOG_CLOSE;	
	}

void CSIPTransitionEngine::ConnectionStateChanged( CSIPConnection::TState aState )
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine [%x]::ConnectionStateChanged()"), this);
	// No Matter what the connection state is become we have to stop the wait
	if (iActiveWait.IsStarted())
		{
			iActiveWait.AsyncStop();
		}
	
	if (iConnState == CSIPConnection::EActive && aState != CSIPConnection::EActive)
		{
		//CSIP Connection Lost
		//Notify all Registration Clients
		for ( TInt count = 0; count<iRegClientArray.Count(); count++)
			{
			iRegClientArray[count]->ErrorOccurred(KErrSIPTransportFailure);
			}
			
		}
	}


void CSIPTransitionEngine::IncomingRequest( CSIPServerTransaction* aTransaction )
/**
Incoming request recieved outside a dialog
*/	
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingRequest() <<Recieved Outside of a Dialog>>"),this);
	TRAPD(err,IncomingRequestHandlerL(aTransaction));	
	if(err != KErrNone)
		{
		return;
		}
	}
	
void CSIPTransitionEngine::IncomingRequest( CSIPServerTransaction* aTransaction, CSIPDialog& aDialog )
/**
Incoming request recieved on an established dialog
*/
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingRequest() <<Recieved on Established Dialog>>"),this);
	//find state machine from associated DialogAssoc
	CSipStateMachine* smPtr = FindStateMachine(aDialog);
	//Extract Method
	RStringF method = aTransaction->RequestElements()->Method();
	TSipMsgBundle msgBundle;
	
	if (method == SIPStrings::StringF(SipStrConsts::EInvite))
		{
		__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingRequest() <<INVITE>>"),this);
		msgBundle.iRequest = TSipHLConsts::ERequestInvite;		
		}
	else if (method == SIPStrings::StringF(SipStrConsts::EAck))
		{
		__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingRequest() <<ACK>>"),this);
		msgBundle.iRequest = TSipHLConsts::ERequestAck;
		}
	else if (method == SIPStrings::StringF(SipStrConsts::EBye))
		{
		__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingRequest() <<BYE>>"),this);
		msgBundle.iRequest = TSipHLConsts::ERequestBye;
		}
	else if (method == SIPStrings::StringF(SipStrConsts::ECancel))
		{
		__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingRequest() <<CANCEL>>"),this);
		msgBundle.iRequest = TSipHLConsts::ERequestCancel;
		}
	else if (method == SIPStrings::StringF(SipStrConsts::ENotify))
		{
		__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingRequest() <<NOTIFY>>"),this);
		msgBundle.iRequest = TSipHLConsts::ERequestNotify;
		}
	
	msgBundle.iServTransaction = aTransaction;
	// Match the state machine which is associated with this message
	if(smPtr)
		{
		__FLOG_0(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingRequest() <<Found state machine for handling this request>>"));
		smPtr->IncomingRequestInsideDialog(msgBundle);	
		}
	}


void CSIPTransitionEngine::IncomingResponse( CSIPClientTransaction& /*aTransaction*/ )
	{
	// We will Not handle this case
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingResponse() <<Not HANDLED CASE CALLED < ITS AN ERROR>>"),this);
	}

void CSIPTransitionEngine::IncomingResponse( CSIPClientTransaction& aTransaction,
							   CSIPDialogAssocBase& aDialogAssoc )
/**
Incoming Response inside an established dialog is received
*/
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingResponse() <<Recieved on Established Dialog>>"),this);
	CSIPCSeqHeader* CseqHdr = (CSIPCSeqHeader*)aTransaction.ResponseElements()->CSeqHeader();
	RStringF method = CseqHdr->Method();
	TSipMsgBundle msgBundle;
	msgBundle.iStatusCode = aTransaction.ResponseElements()->StatusCode();
	msgBundle.iClientTransaction = &aTransaction;
	
	if (method == SIPStrings::StringF(SipStrConsts::EInvite))
		{
		__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingResponse() <<INVITE>>"),this);
		msgBundle.iResponse = TSipHLConsts::EResponseInvite;		
		}
	else if (method == SIPStrings::StringF(SipStrConsts::EAck))
		{
		__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingResponse() <<ACK>>"),this);
		msgBundle.iResponse = TSipHLConsts::EResponseAck;
		}
	else if (method == SIPStrings::StringF(SipStrConsts::EBye))
		{
		__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingResponse() <<BYE>>"),this);
		msgBundle.iResponse = TSipHLConsts::EResponseBye;
		}
	else if (method == SIPStrings::StringF(SipStrConsts::ECancel))
		{
		__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingResponse() <<CANCEL>>"),this);
		msgBundle.iResponse = TSipHLConsts::EResponseCancel;
		}
	else if (method == SIPStrings::StringF(SipStrConsts::ESubscribe))
		{
		__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingResponse() <<SUBSCRIBE>>"),this);
		msgBundle.iResponse = TSipHLConsts::EResponseSubscribe;
		}

	//Find the StateMachine associated with this Dialog
	CSipStateMachine* smPtr = FindStateMachine(aDialogAssoc);
	if (smPtr)
	{
	//Set the error code in case the status code is 3XX 6XX errors.
		if(msgBundle.iStatusCode >299)
		{
		msgBundle.iErrCode= KErrSIPErrorResponse;
		smPtr->ErrorOccured(msgBundle);
		}
		else
		{
		__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingResponse() <<Found state machine for handling this response>>"),this);
		smPtr->IncomingResponse(msgBundle);
		}
	}	
}	
	

void CSIPTransitionEngine::IncomingResponse( CSIPClientTransaction& aTransaction,
							   CSIPInviteDialogAssoc* aDialogAssoc )
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingResponse() <<Recieved on InviteDialog Assoc>>"),this);
	CSIPCSeqHeader * CseqHdr = (CSIPCSeqHeader*)aTransaction.ResponseElements()->CSeqHeader();
	RStringF method = CseqHdr->Method();
	TSipMsgBundle msgBundle;
	msgBundle.iStatusCode = aTransaction.ResponseElements()->StatusCode();
	
	if (method == SIPStrings::StringF(SipStrConsts::EInvite))
		{
		__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingResponse() <<INVITE>>"),this);
		msgBundle.iResponse = TSipHLConsts::EResponseInvite;		
		}
	else if (method == SIPStrings::StringF(SipStrConsts::EAck))
		{
		__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingResponse() <<ACK>>"),this);
		msgBundle.iResponse = TSipHLConsts::EResponseAck;
		}
	else if (method == SIPStrings::StringF(SipStrConsts::EBye))
		{
		__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingResponse() <<BYE>>"),this);
		msgBundle.iResponse = TSipHLConsts::EResponseBye;
		}
	else if (method == SIPStrings::StringF(SipStrConsts::ECancel))
		{
		__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingResponse() <<CANCEL>>"),this);
		msgBundle.iResponse = TSipHLConsts::EResponseCancel;
		}
	else if (method == SIPStrings::StringF(SipStrConsts::ESubscribe))
		{
		__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingResponse() <<SUBSCRIBE>>"),this);
		msgBundle.iResponse = TSipHLConsts::EResponseSubscribe;
		}
		
	//Find the StateMachine associated with this Dialog
	CSipStateMachine* smPtr = FindStateMachine(reinterpret_cast<CSIPDialogAssocBase*>(aDialogAssoc));
	if (smPtr)
		{
		__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingResponse() <<Found state machine for handling this response>>"),this);
		smPtr->IncomingResponse(msgBundle);
		}
	}

void CSIPTransitionEngine::IncomingResponse( CSIPClientTransaction& aTransaction,
							   CSIPRegistrationBinding& aRegistration )
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingResponse() <<Recieved on Registration Assoc>>"),this);

	CSipStateMachine* smPtr = FindStateMachine(aTransaction);
	if (smPtr)
		{
		TSipMsgBundle	msgBundle;
		if (aRegistration.IsContextActive()) // Registeration Response
			{
			__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingResponse() <<REGISTER>>"),this);
			msgBundle.iResponse = TSipHLConsts::EResponseRegister;
			msgBundle.iStatusCode = aTransaction.ResponseElements()->StatusCode();
			smPtr->IncomingResponse(msgBundle);
			}
		else // Unregistration Response
			{
			__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingResponse() <<UNREGISTER>>"),this);
			msgBundle.iResponse = TSipHLConsts::EResponseUnRegister;
			msgBundle.iStatusCode = aTransaction.ResponseElements()->StatusCode();
			smPtr->IncomingResponse(msgBundle);
			}
		}
	}

void CSIPTransitionEngine::ErrorOccured( TInt aError,
						   CSIPTransactionBase& aTransaction )
/**
This function will ask the particular state machine to handle the 
error
*/						   
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::ErrorOccurred(TInt aError,CSIPTransactionBase& aTransaction)"),this);
	CSipStateMachine* smPtr = FindStateMachine(aTransaction);
	if(smPtr)
		{
		TSipMsgBundle 	errBundle;
		errBundle.iErrCode = aError;
		smPtr->ErrorOccured(errBundle);
		}
	}

void CSIPTransitionEngine::ErrorOccured( TInt aError,
						   CSIPClientTransaction& aTransaction,
						   CSIPRegistrationBinding& /*aRegistration*/ )
/**
Handle errors related to Failed Registration. The registration binding is destroyed by
the state machine, so need not to be destroyed here
*/						   
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::ErrorOccurred(TInt aError,CSIPTransactionBase& aTransaction, CSIPRegistrationBinding & aRegistration)"),this);
	CSipStateMachine* smPtr = FindStateMachine(aTransaction);
	if(smPtr)
		{
		__FLOG_0(_L("[TransitionEngine]: <<Found State Machine to Handle the Error>>"));
		TSipMsgBundle msgBundle;
		msgBundle.iErrCode = aError;
		msgBundle.iResponse = TSipHLConsts::EResponseRegister;
		const CSIPResponseElements * resElems = aTransaction.ResponseElements();
		if(resElems)
			{
			msgBundle.iStatusCode = resElems->StatusCode();		
			}
		else
			{
			msgBundle.iStatusCode = -1;			
			}
		msgBundle.iClientTransaction = &aTransaction;
		smPtr->ErrorOccured(msgBundle);
		}
	else
		{
		__FLOG_0(_L("[TransitionEngine]: <<Not Found State Machine to Handle the Error>>"));	
		}
	}

void CSIPTransitionEngine::ErrorOccured( TInt aError,
						   CSIPTransactionBase& /*aTransaction*/,
						   CSIPDialogAssocBase& aDialogAssoc)
/**
Handles the errors related to failed transaction. No need delete the transaction
or the dialogassoc , as state machine will delete those upon termination
*/						   
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::ErrorOccurred(TInt aError,CSIPTransactionBase& aTransaction, CSIPDialogAssocBase & aDialogAssoc)"),this);
	CSipStateMachine* smPtr = FindStateMachine(aDialogAssoc);
	if(smPtr)
		{
		__FLOG_0(_L("[TransitionEngine]: <<Found State Machine to Handle the Error>>"));
		TSipMsgBundle	errBundle;
		errBundle.iErrCode = aError;
		// we cant get status code from response
		errBundle.iStatusCode = -1;	
		smPtr->ErrorOccured(errBundle);
		}
	else
		{
		__FLOG_0(_L("[TransitionEngine]: <<Not Found State Machine to Handle the Error>>"));
		// No State Machine, Nothing to do , SM might have been terminated by some 
		// other request
		return;	
		}
	}

void CSIPTransitionEngine::ErrorOccured(TInt aError, CSIPRefresh & aSIPRefresh)
/**
Handles Errors related to refresh 
*/
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::ErrorOccurred(TInt aError,CSIPRefresh & aSIPRefresh)"),this);
	TSipMsgBundle	errBundle;
	// Get The state machine associated with this particular Refresh request 
	const CSIPClientTransaction * clientTnx = aSIPRefresh.SIPTransaction();
	if(clientTnx)
		{
		__FLOG_0(_L("[TransitionEngine]: <<Found State Machine to Handle the Error>>"));
		CSipStateMachine* smPtr = FindStateMachine(clientTnx);
		if(smPtr)
			{
			errBundle.iErrCode = aError;
			const CSIPResponseElements * resElems = clientTnx->ResponseElements();
			if(resElems)
				{
				errBundle.iStatusCode = resElems->StatusCode();
				}
			else
				{
				errBundle.iStatusCode = -1;	
				}
			smPtr->ErrorOccured(errBundle);
			}
		else
			{
			// NO SM , nothing to do	
			}
		}
	else
		{
		__FLOG_0(_L("[TransitionEngine]: <<Not Found State Machine to Handle the Error>>"));
		// No Client Transaction , Noting can be done	
		}
	}
	
void CSIPTransitionEngine::ErrorOccured( TInt aError,
						   CSIPRegistrationBinding& aRegistration)
/**
Handles the error occurred with the periodic refreshing of an registration
binding
*/						   
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::ErrorOccurred(TInt aError,CSIPRegistrationBinding& aRegistration)"),this);
	TSipMsgBundle	errBundle;
	// Get The state machine to Handle this condition 
	CSIPRefresh * refresh = aRegistration.SIPRefresh();
	if(refresh)
		{
		const CSIPClientTransaction *clientTxn = refresh->SIPTransaction();
		if(clientTxn)
			{
			CSipStateMachine* smPtr = FindStateMachine(clientTxn);
			if(smPtr)
				{
				errBundle.iErrCode = aError;
				const CSIPResponseElements * resElems = clientTxn->ResponseElements();
				if(resElems)
					{
					errBundle.iStatusCode = resElems->StatusCode();
					}
				else
					{
					errBundle.iStatusCode = -1;	
					}
				smPtr->ErrorOccured(errBundle);
				}
			else
				{
				// NO SM , nothing to do	
				}
			}
		else
			{
			// No client Txn , Nothing can be done	
			}
		}
	else
		{
		// No Refesh inside nothing can be done	
		}
	}
	
void CSIPTransitionEngine::ErrorOccured( TInt aError,
						CSIPDialogAssocBase& aDialogAssoc)
/**
Handles SIP errors based That happens on a Existing Dialog association
*/						   
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::ErrorOccurred(TInt aError,CSIPDialogAssocBase& aDialogAssoc)"),this);
	TSipMsgBundle errBundle;
	// Find the state machine to Handle this error condition
	CSipStateMachine* smPtr = FindStateMachine(aDialogAssoc);
	if(smPtr)
		{
		__FLOG_0(_L("[TransitionEngine]: <<Found State Machine to Handle the Error>>"));
		errBundle.iErrCode = aError;
		// we dont have handle to SIP message elements from here
		errBundle.iStatusCode = -1;	
		smPtr->ErrorOccured(errBundle);
		}
	else
		{
		__FLOG_0(_L("[TransitionEngine]: <<Not Found State Machine to Handle the Error>>"));
		// NO SM , nothing to do	
		}
	}

void CSIPTransitionEngine::InviteCompleted( CSIPClientTransaction& aTransaction )
/**
Handles the case related to the Completion of Invite Request
*/
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::InviteCompleted()"),this);
	CSipStateMachine*	smPtr = FindStateMachine(aTransaction);
	smPtr->InviteCompleted(aTransaction);
	}


void CSIPTransitionEngine::InviteCanceled( CSIPServerTransaction& aTransaction )
/**
This callback will be called when a incoming Invite is cancelled by the remote machine
*/
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::InviteCanceled()"),this);
	CSipStateMachine*	smPtr = FindStateMachine(aTransaction);
	TSipMsgBundle	msgBundle;
	msgBundle.iServTransaction = &aTransaction;
	msgBundle.iRequest = TSipHLConsts::ERequestCancel;
	smPtr->IncomingRequest(msgBundle);
	}

//From MSIPProfileRegistryObserver
void CSIPTransitionEngine::ProfileRegistryErrorOccurred(TUint32 aProfileId, TInt aError)
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::ProfileRegistryErrorOccurred()"),this);
	//if(iActiveWait.IsStarted())
    //	iActiveWait.AsyncStop();
	CSIPProfile* profile = NULL;
	TUint32 profileId;
	MSIPRegistrationClient* regClient = FindRegistrationClient(aProfileId);
	
	TInt size = iProfileArray.Count();
	for (TInt count = 0; count<size; count++)
		{
		profile = iProfileArray[count];
		profile->GetParameter(KSIPProfileId, profileId);
		if (profileId == aProfileId)
			{
			iProfileRegistry->Disable(*profile);
			iProfileArray.Remove(count);
			delete profile;
			break;	
			}
		}

	//MSIPRegistrationClient* regClient = FindRegistrationClient(aProfileId);
	if(regClient)
		{
		regClient->ErrorOccurred(aError);
		}
	}
	
void CSIPTransitionEngine::ProfileRegistryEventOccurred(TUint32 aProfileId, TEvent aEvent)
/**
Handle the registry event for Registration and Deregistration
*/
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::ProfileRegistryEventOccurred()"),this);
	//if(iActiveWait.IsStarted())
    //	iActiveWait.AsyncStop();
	
	TUint32 		profileId;
	CSIPProfile*	profile = NULL;
	MSIPRegistrationClient* regClient = FindRegistrationClient(aProfileId);
	if(regClient == NULL)
		{
		_LIT(KSipConnProvPanic, "SIPCPR");
		User::Panic(KSipConnProvPanic, KSIPProfileNotFound);
		}
	switch(aEvent)
		{
		case EProfileRegistered: 
			{
			__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::ProfileRegistryEventOccurred()  <<EProfileRegistered>>"),this);
			regClient->RegistrationComplete(KErrNone);	
			}
			break;		
			
		case EProfileDeregistered:
			{
			__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::ProfileRegistryEventOccurred()  <<EProfileDeRegistered>>"),this);
			TInt size = iProfileArray.Count();
			for (TInt count = 0; count<size; count++)
				{
				profile = iProfileArray[count];
				profile->GetParameter(KSIPProfileId, profileId);
				if (profileId == aProfileId)
					{
					iProfileArray.Remove(count);
					delete profile;
					break;	
					}
				}
			regClient->DeRegistrationComplete(KErrNone);	
			break;
			}
		default:
			{
			__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::ProfileRegistryEventOccurred()  <<ERROR..>>"),this);
			regClient->ErrorOccurred(KErrUnknown);	
			}
		}
				
	}

CSipStateMachine*	CSIPTransitionEngine::FindStateMachine(CSIPTransactionBase& aTransaction)
/**
Finds the associated state machine with the SIP transaction
*/
	{
	for(TInt count = 0;count<iSMArray.Count();count++)
		{
		if(*(iSMArray[count]->iTransaction) == aTransaction)
			{
			return iSMArray[count]->iStateMachine;
			}
		}
	return NULL;
	};

CSipStateMachine*	CSIPTransitionEngine::FindStateMachine(const CSIPTransactionBase* aTransaction)
/**
Finds the associated state machine with the SIP transaction
*/
	{
	for(TInt count = 0;count<iSMArray.Count();count++)
		{
		if(iSMArray[count]->iTransaction == aTransaction)
			{
			return iSMArray[count]->iStateMachine;
			}
		}
	return NULL;
	};

CSipStateMachine*	CSIPTransitionEngine::FindStateMachine(CSIPDialogAssocBase* aDialogAssoc)
/**
Finds the associated state machine with the SIP Dialog
*/
	{
	TInt numElements = iSMArray.Count();
	for(TInt count=0; count<numElements; count++)
		{
		if (iSMArray[count]->iDialog == aDialogAssoc)
			{
			return iSMArray[count]->iStateMachine;
			}
		}
	return NULL;
	
	};


CSipStateMachine*	CSIPTransitionEngine::FindStateMachine(CSIPDialog& aDialog)
/**
Finds the associated state machine with the SIP Dialog
*/
	{
	TInt numElements = iSMArray.Count();
	for(TInt count=0; count<numElements; count++)
		{
		if(aDialog.IsAssociated(*(iSMArray[count]->iDialog)))
			{
			return iSMArray[count]->iStateMachine;
			}
		}
	return NULL;	
	};

	
CSipStateMachine*	CSIPTransitionEngine::FindStateMachine(CSIPDialogAssocBase& aDialogAssoc)
/**
Finds the associated state machine with the SIP Dialog Assocition
*/
	{
	TInt numElements = iSMArray.Count();
	for(TInt count=0; count<numElements; count++)
		{
		if (iSMArray[count]->iDialog == &aDialogAssoc)
			{
			return iSMArray[count]->iStateMachine;
			}
		}
	return NULL;
	};

EXPORT_C void CSIPTransitionEngine::AddStateMachineL(CSipStateMachine* aStateMachine)
/**
Using this call The client will register the state machine with the Transition 
Engine
*/
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::AddStateMachineL()"),this);
	CHLStateMachine *hlSm = new(ELeave) CHLStateMachine;
	hlSm->iStateMachine = aStateMachine;
	hlSm->iDialog		= NULL;
	hlSm->iTransaction  = NULL;
	iSMArray.AppendL(hlSm);
	}

void CSIPTransitionEngine::Cleanup(CSipStateMachine * aStateMachine)
/**
Handles the cleanup related to the transition Engine
*/
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::Cleanup()"),this);
	CHLStateMachine *hlSm = NULL;
	TInt numElements = iSMArray.Count();
	for(TInt count=0; count<numElements; count++)
		{
		if (iSMArray[count]->iStateMachine == aStateMachine)
			{
			hlSm = (CHLStateMachine*)iSMArray[count];
			hlSm->iDialog = NULL;
			hlSm->iTransaction = NULL;
			delete hlSm;
			}
		}
	}
	
	
void	CSIPTransitionEngine::SetTransaction(CSipStateMachine* aStateMachine, CSIPTransactionBase* aTransaction)
/**
Sets The transaction for a particular state machine
*/
	{
	TInt numElements = iSMArray.Count();
	for(TInt count=0; count<numElements; count++)
		{
		if (iSMArray[count]->iStateMachine == aStateMachine)
			{
			iSMArray[count]->iTransaction = aTransaction;
			}
		}
	}

void	CSIPTransitionEngine::SetDialogAssoc(CSipStateMachine* aStateMachine, CSIPDialogAssocBase* aDialoAssoc)
/**
Sets the dialog association for a particular state machine
*/
	{
	TInt numElements = iSMArray.Count();
	for(TInt count=0; count<numElements; count++)
		{
		if (iSMArray[count]->iStateMachine == aStateMachine)
			{
			iSMArray[count]->iDialog = aDialoAssoc;
			}
		}
	}

CSipStateMachine *  CSIPTransitionEngine::FindSMForIncomingCall()
/**
Find the empty state machine for handling the incoming calls
*/
	{
	TInt numElements = iSMArray.Count();
	for(TInt count=0; count<numElements; count ++)
		{
		if(iSMArray[count]->iTransaction == NULL && iSMArray[count]->iDialog == NULL)
			{
			// Check if this empty state machine is for incoming
			if(iSMArray[count]->iStateMachine->GetIncomingStatus())
				{
				return iSMArray[count]->iStateMachine;
				}
			}
		}
	return NULL;
	}
	

EXPORT_C void  CSIPTransitionEngine::RegisterL(MSIPRegistrationClient* aRegistrationClient)
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::RegisterL()"),this);	
	TUint32 profileId = aRegistrationClient->GetProfileId();
	CSIPProfile* profile = NULL;
	iRegClientArray.AppendL(aRegistrationClient);
	
	TInt err = KErrNone;
	if(profileId == KSIPDefaultProfileId)
		{
		TRAP(err,profile = iProfileRegistry->DefaultProfileL());
		}
	else
		{
		TRAP(err, profile = iProfileRegistry->ProfileL(profileId));	
		}
	if(err != KErrNone)
		{
    	__FLOG_2(_L("CSIPTransitionEngine %08x:The profile [%d] hasn't been found, aborting..."), this, profileId);
		User::Leave(KSIPProfileNotFound);
		}
	if (!iProfileRegistry->IsEnabled(*profile))
		{
    	__FLOG_2(_L("CSIPTransitionEngine %08x:The profile [%d] found, enabling..."), this, profileId);
		TRAP(err,iProfileRegistry->EnableL( *profile, *this ));
		if(err != KErrNone)
			{
			User::Leave(KSIPProfileNotFound);
			}
		iProfileArray.AppendL(profile);
		}
	}
	

EXPORT_C void  CSIPTransitionEngine::Deregister(MSIPRegistrationClient* aRegistrationClient)
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::Deregister()"),this);	
	TUint32 regProfId = aRegistrationClient->GetProfileId();
	TUint32 profileId;
	CSIPProfile* profile;
	TInt size = iProfileArray.Count();
	
	for (TInt count = 0; count<size; count++)
		{
		profile = iProfileArray[count];
		profile->GetParameter(KSIPProfileId, profileId);
		if (profileId == regProfId)
			{
			iProfileRegistry->Disable(*profile);
			break;		
			}
		}
	}
	
	
EXPORT_C TUint32  CSIPTransitionEngine::FindDuplicateProfile(TUint32 aProfileId)
	{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::FindDuplicateProfile()"),this);	
	// IAP,Registrar,User AOR,Contacts, Proxy, Digest User Name, Digest Realm, SecMechanism
	if (aProfileId == KSIPInvalidProfileId)
		{
		return KSIPInvalidProfileId;
		}
	
	CSIPProfile* profile = NULL;
	TRAPD(err,	profile = iProfileRegistry->ProfileL(aProfileId));
	
	if (err != KErrNone)
		{
		return KErrNone;
		}
	
	TUint32	thisIapId;
	TUint32 thisProfileId;
	const TDesC8*	thisAor = NULL;
	const TDesC8*	thisRegistrar = NULL;
	const TDesC8*	thisProxy = NULL;
	const TDesC8*	thisRegRealm = NULL;
	const TDesC8*	thisProxyRealm = NULL;
	const TDesC8*	thisRegUser = NULL;
	const TDesC8*	thisProxyUser = NULL;
	
	profile->GetParameter(KSIPProfileId, thisProfileId);
	profile->GetParameter(KSIPAccessPointId, thisIapId);
	profile->GetParameter(KSIPUserAor, thisAor);
	profile->GetParameter(KSIPRegistrar,KSIPServerAddress, thisRegistrar);
	profile->GetParameter(KSIPOutboundProxy,KSIPServerAddress, thisProxy);
	profile->GetParameter(KSIPRegistrar,KSIPDigestRealm, thisRegRealm);
	profile->GetParameter(KSIPRegistrar,KSIPDigestUserName, thisRegUser);
	profile->GetParameter(KSIPOutboundProxy,KSIPDigestRealm, thisProxyRealm);
	profile->GetParameter(KSIPOutboundProxy,KSIPDigestUserName, thisProxyUser);
	
					 	
	TInt size = iProfileArray.Count();
		
	TUint32	iapId;
	TUint32 profileId;
	const TDesC8*	aor = NULL;
	const TDesC8*	registrar = NULL;
	const TDesC8*	proxy = NULL;
	const TDesC8*	regRealm = NULL;
	const TDesC8*	proxyRealm = NULL;
	const TDesC8*	regUser = NULL;
	const TDesC8*	proxyUser = NULL;
	
	//Iterate through profiles in iProfileArray
	//and match
	
	for (TInt count=0; count<size; count++)
		{
		iProfileArray[count]->GetParameter(KSIPProfileId, profileId);
		if (profileId == thisProfileId)
			{
			//profile Id for an existing profile was passed
			return profileId;
			}
		iProfileArray[count]->GetParameter(KSIPAccessPointId, iapId);
		if (iapId != thisIapId)
			{
		    continue;
			}
		iProfileArray[count]->GetParameter(KSIPUserAor, aor);
		if (aor != thisAor)
			{
		    continue;
			}
		
		iProfileArray[count]->GetParameter(KSIPRegistrar,KSIPServerAddress, registrar);
		if (registrar != thisRegistrar)
			{
			continue;
			}
			
		iProfileArray[count]->GetParameter(KSIPOutboundProxy,KSIPServerAddress, proxy);
		if (proxy != thisProxy)
			{
			continue;
			}
		
		iProfileArray[count]->GetParameter(KSIPRegistrar,KSIPDigestRealm, regRealm);
		if(regRealm != thisRegRealm)
			{
			continue;
			}
		iProfileArray[count]->GetParameter(KSIPOutboundProxy,KSIPDigestRealm, proxyRealm);
		if (proxyRealm != thisProxyRealm)
			{
			continue;
			}
		iProfileArray[count]->GetParameter(KSIPRegistrar,KSIPDigestUserName, regUser);
		if (regUser != thisRegUser)
			{
			continue;
			}
		
		iProfileArray[count]->GetParameter(KSIPOutboundProxy,KSIPDigestUserName, proxyUser);
		if (proxyUser != thisProxyUser)
			{
			continue;
			}
		//Duplicate profile found
		return profileId;
		}
		
	return KSIPInvalidProfileId;
	}
	
	

EXPORT_C void CSIPTransitionEngine::AddCredentialsL(TSIPCredentials aCredential)
	{
	iCredentialsArray.AppendL(aCredential);
	}
	
EXPORT_C const TDesC8* CSIPTransitionEngine::GetFromField(TUint32 aProfileId) const
    {
	TInt size = iProfileArray.Count();
	CSIPProfile* profile = NULL;
    const TDesC8* userName = NULL;
    TUint32 profileId = KSIPInvalidProfileId;
	
	for (TInt count = 0; count<size; count++)
		{
		profile = iProfileArray[count];
		profile->GetParameter(KSIPProfileId, profileId);
		if (profileId == aProfileId)
			{
			profile->GetParameter(KSIPUserAor, userName);
			}
		}
	return userName;
    }	
	
	
MSIPRegistrationClient*	CSIPTransitionEngine::FindRegistrationClient(TUint32 aProfileId)
	{
	TInt size = iRegClientArray.Count();
	for (TInt count = 0; count<size; count++)
		{
		if(aProfileId == iRegClientArray[count]->GetProfileId())
			{
			return iRegClientArray[count];
			}
		}
		return NULL;
	}

//From MSIPHttpDigestChallengeObserver
void CSIPTransitionEngine::ChallengeReceived(const TDesC8& aRealm)
	{
	//Walkthrough the list of Realms to find credentials
	if (!FindAndSetCredentials(aRealm))
		{
		TInt size = iSMArray.Count();
		//Walk through State Machines and find the current outgoing active state machine
		for (TInt count=0; count<size; count++)
			{
			CSipStateMachine* smPtr= iSMArray[count]->iStateMachine;
			if(!smPtr->GetIncomingStatus() && smPtr->GetOutgoingStatus())
				{
				iDigest->IgnoreChallenge(aRealm);
				smPtr->ChallengeIgnored(aRealm);
				}
			}
		}
	
	}
	
	
TBool CSIPTransitionEngine::FindAndSetCredentials(const TDesC8& aRealm)
	{
	TInt size = iCredentialsArray.Count();
	
	for (TInt count=0; count<size; count++)
		{
		if (aRealm == iCredentialsArray[count].iRealm)
			{
			TRAPD(err, iDigest->SetCredentialsL(iCredentialsArray[count].iRealm,
												iCredentialsArray[count].iUserName,
												iCredentialsArray[count].iPasswd));
			if(err != KErrNone)
				{
				return ETrue;
				}
			}
		}
	return EFalse;
	}

void CSIPTransitionEngine::IncomingRequestHandlerL(CSIPServerTransaction* aTransaction)
{
	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingRequestL() <<Recieved Outside of a Dialog>>"),this);
	CleanupStack::PushL(aTransaction); //we're take over the ownership.
	
	RStringF method = aTransaction->RequestElements()->Method();
	if(method == SIPStrings::StringF(SipStrConsts::EInvite))
		{
		__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingRequestL() <<Received Request Invite>>"),this);
		TSipMsgBundle	msgBundle;
		// Create a dialog Association and set server transaction
		CSIPInviteDialogAssoc*	dialogassoc = CSIPInviteDialogAssoc::NewL(*aTransaction);
		CleanupStack::PushL(dialogassoc);
		msgBundle.iRequest = TSipHLConsts::ERequestInvite;
		msgBundle.iDialog = dialogassoc;
		msgBundle.iServTransaction = aTransaction;
		// Look For the state machine which is responsible for handling 
	 	// this particular request
	 	CSipStateMachine*	smPtr = FindSMForIncomingCall();
	 	if(smPtr)
	 		{
	 		__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingRequestL() <<Found State Machine to Handle the Request>>"),this);
	 		smPtr->IncomingRequestOutsideDialog(msgBundle);	
	 		CleanupStack::Pop(dialogassoc);
	 		CleanupStack::Pop(aTransaction);
	 		}
	 	else
	 		{// return NotFound
	 		__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingRequestL() <<No State Machine found to Handle the Request>>"),this);
	 		__FLOG_0(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingRequestL() <<sending 404>>"));
	 		CSIPResponseElements *ResElem = CSIPResponseElements::NewLC(
							404, SIPStrings::StringF(SipStrConsts::EPhraseNotFound));
			aTransaction->SendResponseL(ResElem);
			CleanupStack::Pop(ResElem);		
			CleanupStack::PopAndDestroy(aTransaction);
			CleanupStack::PopAndDestroy(dialogassoc);
	 		}
		}
	 else
	 	{ // bad request , not supported here
	 	__FLOG_1(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingRequestL() <<Received Request other than Invite>>"),this);
	 	__FLOG_0(_L("[TransitionEngine]: CSIPTransitionEngine[%x]::IncomingRequestL() <<sending 404>>"));
	 	CSIPResponseElements *ResElem = CSIPResponseElements::NewLC(
							400, SIPStrings::StringF(SipStrConsts::EPhraseBadRequest));
		aTransaction->SendResponseL(ResElem);
		CleanupStack::Pop(ResElem);		
		CleanupStack::PopAndDestroy(aTransaction);
	 	}
}
