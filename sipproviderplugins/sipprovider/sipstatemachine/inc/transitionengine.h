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
// CSIPTransitionEngine definition file
// 
//

/**
 @file
 @internalComponent
*/

#ifndef TRANSITIONENGINE_H
#define TRANSITIONENGINE_H


#include <e32base.h>

#include <sip.h>
#include <sipobserver.h>
#include <sipconnectionobserver.h>
#include <sipprofileregistryobserver.h>
#include <sipprofile.h>
#include <sipprofileregistry.h>
#include <sipfromheader.h>
#include <siptoheader.h>
#include <sipcontactheader.h>

#include <sipmessageelements.h>
#include <siprequestelements.h>
#include <sipresponseelements.h>

#include <siprefresh.h>
#include <sipregistrationbinding.h>

#include <sipdialog.h>
#include <sipinvitedialogassoc.h>
#include <sipsubscribedialogassoc.h>
#include <sipnotifydialogassoc.h>
#include <sipreferdialogassoc.h>
#include <sipservertransaction.h>
#include <sipclienttransaction.h>
#include <sipaddress.h>
#include <sdpdocument.h>
#include <sipcontenttypeheader.h>
#include <sdpcodecstringconstants.h>
#include <sdpcodecstringpool.h>
#include <sipstrings.h>
#include <sipstrconsts.h>  

#include <sipcseqheader.h>
#include <sipeventheader.h>
#include <sipexpiresheader.h>
#include <sipacceptheader.h>
#include "siphlcommontypes.h"
#include <siperr.h>

#include <siphttpdigestchallengeobserver.h>
#include <siphttpdigest.h>

#ifdef __FLOG_ACTIVE
	// This will do the logging if logging is enabled
    #include <comms-infras/ss_log.h>
#endif

class CSipStateMachine;

class MSIPRegistrationClient
/**
To be implemented by whoever uses CSIPTransitionEngine to Register
at the SIP registrar - see CSIPTransitionEngine::RegisterL
*/
	{
	public:
	
	/**
	Called by the TE when the requested registration is complete
	@param aError error indication, if KErrNone - the registration 
	has been successful, non-successful otherwise.
	*/	
	virtual void RegistrationComplete(TInt aError) = 0;
	
	/**
	Called by the TE when 'this' has been unregistered either as a result of:
	- previously issued deregistration request
	- unregistered by the registrar
	
	@param aError error indication
	*/	
	virtual void DeRegistrationComplete(TInt aError) = 0;
	
	/**
	Called by the TE when a error occurs which has to be notified to "its"
	connection providers
	
	@param aError error indication
	*/
	virtual void ErrorOccurred(TInt aError) = 0;
	
	/**
	Called by the State Machine to flag that a peer is calling.
	
    @return the SIP profileId the TE will use to identify 'this'
	*/	
	virtual TUint32 GetProfileId() = 0;
	
	/**
	Called by the TransitionEngien Manager to obtain the AppUId of the
	Conn Provider when Detaching
	
	@return the appUid stored by the ConnProvider
	*/
	virtual TUid	GetAppUid() = 0;
	};   
		

class CSIPTransitionEngine : public CBase,
							 MSIPConnectionObserver,
							 MSIPProfileRegistryObserver,
							 MSIPHttpDigestChallengeObserver
							 
/**
 SIPTransitionEngine 
 This class implements the following set of methods
 - Call back methods which are called by the Sip stack. These trigger transition in the State Machines
 - Call back methods which are triggered by the Profile Manager (for Registration, Profile events)
 
@internalComponent
@released since v9.2
*/						
	{
public:
	friend class CSIPTransitionEngineBundle;
	
	//New Functions - Services provided by CSIPTransitionEngine
	/**
	This method is called by a Connection Provider to initialize
	Registration
	
	@param aRegistrationClient , an object which implements MSIPRegistrationClient  interface ( a SIPCPR)
	*/
	IMPORT_C void RegisterL(MSIPRegistrationClient* aConnProv);
			
	/**
	This method is called by a Connection Provider to initialize
	DeRegistration
	
	@param aRegistrationClient , an object which implements MSIPRegistrationClient interface ( a SIPCPR)
	*/
	IMPORT_C void Deregister(MSIPRegistrationClient* aConnProv);
		
	
	/**
	This method is called by a State Machine, to register itself
	with the TransitionEngine
	
	@param aStateMachine Register Instance of State machine with TE
	*/
	IMPORT_C void AddStateMachineL(CSipStateMachine* aStateMachine);
	
		
	/**
	This method is called by a Connection Provider to initialize
	Registration
	
	@return profile id of a matching profile already available with TransitionEngine
	@param  aProfileId , profile id to be matached
	*/
	IMPORT_C TUint32 FindDuplicateProfile(TUint32 aProfileId);
	
	
	/**
	This method is called to add a Credentials structure to a list of credentials
	maintained by the TransitionEngine
	*/
	
	IMPORT_C void AddCredentialsL(TSIPCredentials aCredential);
	
	/**
	Returns the 'from' field associated with the given profile. 
	@param aProfileId the id of the profile the 'from' field should pertain to.
	
	@return the 'from' field associated with aProfileId. Given 'this' has been
	asked to register the profile.
	*/
    IMPORT_C const TDesC8* GetFromField(TUint32 aProfileId) const;
	
	/**
	This method associates the SIP transaction with a particular State machine
	
	@param  aStateMachine	Instance of the state machine
	@param  aTransaction	which will be associated with the state machine
	*/
	void	SetTransaction(CSipStateMachine* aStateMachine, CSIPTransactionBase* aTransaction);
	
	/**
	This method associates the SIP Dialog association with a particular State machine
	
	@param  aStateMachine	Instance of the state machine
	@param  aDialoAssoc		which will be associated with the state machine
	*/
	void	SetDialogAssoc(CSipStateMachine* aStateMachine, CSIPDialogAssocBase* aDialoAssoc);
	
	/**
	This method will cleanup the transition engine related to a particular state machine
	
	@param  aStateMachine	which needs to be cleaned up
	*/
	void	Cleanup(CSipStateMachine * aStateMachine);
	
	/**
	This method will find the state machine to handle the incoming call.
	*/
	CSipStateMachine* 	FindSMForIncomingCall();
	
	/**
	This method will find the state machine which is associated with the particular
	transaction.
	
	@param  aTransaction which needs to be matched with SM
	*/
	CSipStateMachine*	FindStateMachine(CSIPTransactionBase& aTransaction);
	
	/**
	This method will find the state machine which is associated with the particular
	Dialog.
	
	@param  aDialogAssoc which needs to be matched with SM
	*/
	CSipStateMachine*	FindStateMachine(CSIPDialogAssocBase& aDialogAssoc);
	
	/**
	Overloaded method will find the state machine which is associated with the particular
	Dialog.
	
	@param  aDialogAssoc which needs to be matched with SM
	*/
	CSipStateMachine*	FindStateMachine(CSIPDialogAssocBase* aDialogAssoc);
	
	/**
	Overloaded method will find the state machine which is associated with the particular
	Dialog.
	
	@param  aDialogAssoc which needs to be matched with SM
	*/
	CSipStateMachine*	FindStateMachine(CSIPDialog& aDialogAssoc);
	
	/**
	Overloaded method will find the state machine which is associated with the particular
	Dialog.
	
	@param  aTransaction which needs to be matched with SM
	*/
	CSipStateMachine*	FindStateMachine(const CSIPTransactionBase* aTransaction);
	
	/**
	This method will return the SIP connection associated with the Transition Engine
	*/
	CSIPConnection* 	GetSipConnection();
	
	/**
	This method returns the handle of the registration client associated with the 
	profile Id
	
	@param  aProfileId for registration association
	*/
	MSIPRegistrationClient*	FindRegistrationClient(TUint32 aProfileId);
	
	/**
	This function increases the reference count 
    */
	void Attach();
	
	/**
	This function decreases the reference count 
    */
    //todo
    TInt Detach();
   	//TInt Detach(MSIPRegistrationClient* aRegClient);
	
	/**
	This function returns an IAP id
    */
	TInt IapId();

	// Methods from base classes
	// From MSIPConnectionObserver
	/**
	A SIP request outside a dialog has been received from the network.
    
	@param aTransaction SIP server transaction. The ownership is transferred.
    */
	void	IncomingRequest( CSIPServerTransaction* aTransaction );

	/**
	A SIP request within a dialog has been received 
	
	@param aTransaction SIP server transaction. The ownership is transferred.
	@param aDialog the dialog that this transaction belongs to.        
	*/
	void IncomingRequest( CSIPServerTransaction* aTransaction,
						  CSIPDialog& aDialog );

	/**
	IncomingResponse (from MSIPConnectionObserver)
	*/
	void IncomingResponse( CSIPClientTransaction& aTransaction );

	/**
	A SIP response that is within a dialog association or creates a dialog association.
    
    @param aTransaction contains response elements.
	@param aDialogAssoc a dialog association.        
	*/
	void IncomingResponse( CSIPClientTransaction& aTransaction,
						   CSIPDialogAssocBase& aDialogAssoc );

    /**
	IncomingResponse (from MSIPConnectionObserver)
	Multiple SIP responses have been received to the single INVITE due to the forking proxy. 
	Note that each response creates a separate INVITE dialog association.Multiple responses 
	can arrive until SIP stack completes UAC core INVITE transaction.If clients deletes INVITE 
	transaction after first SIP response other possible responses will be consumed by the implementation.
		 
	@param aTransaction contains response elements
	@param aDialogAssoc INVITE dialog association;
	*/
	void IncomingResponse( CSIPClientTransaction& aTransaction,
						   CSIPInviteDialogAssoc* aDialogAssoc );

	/**
	IncomingResponse (from MSIPConnectionObserver)
	
	@param aTransaction contains response elements
	@param aRegistration contains the Registration Binding
	*/
	void IncomingResponse( CSIPClientTransaction& aTransaction,
						   CSIPRegistrationBinding& aRegistration );

	/**
	An asynchronous error has occurred in the stack related to the
	request indicated by the given transaction.
	
	@param aError system wide or sip error code
	@param aTransaction failed transaction.
	@param aSIPConnection a SIP connection        
	*/
	void ErrorOccured( TInt aError, CSIPTransactionBase& aTransaction );

	/**
	An asynchronous error has occurred in the stack related
	to the request indicated by the given transaction.
    
	@param aError system wide or sip error code
	@param aTransaction the failed transaction.
	@param aRegistration the failed registration.        
	*/
	void ErrorOccured( TInt aError,
					   CSIPClientTransaction& aTransaction,
					   CSIPRegistrationBinding& aRegistration );

	/**
	An asynchronous error has occured related to a request within an existing dialog.
    
	@param aError system wide or sip error code
	@param aTransaction the failed transaction.
	@param aDialogAssoc the failed dialog associoation.        
	*/
	void ErrorOccured( TInt aError,
					   CSIPTransactionBase& aTransaction,
					   CSIPDialogAssocBase& aDialogAssoc );

	/**
	An asynchronous error has occured related to a refresh 
    
    @param aError system wide or sip error code
	@param aSIPRefresh original refresh object.        
	*/
	void ErrorOccured( TInt aError, CSIPRefresh& aSIPRefresh );

	/**
	An asynchronous error has occured related to a periodical refresh
    that relates to a registration.
    
    @param aError system wide or sip error code; 
	@param aRegistration associated registration.
	*/
	void ErrorOccured( TInt aError,
					   CSIPRegistrationBinding& aRegistration );

	/**
	An asynchronous error has occured related to a periodical refresh
    that belongs to SIP dialog association.
    
    @param aError system wide or sip error code; 
	@param aDialogAssoc SIP dialog association.        
	*/
	void ErrorOccured( TInt aError,
					   CSIPDialogAssocBase& aDialogAssoc );

    /**
	SIP stack has completed UAC core INVITE transaction 64*T1 seconds after the reception of 
	the first 2xx response. No more 2xx responses can be received to the issued single INVITE.
    If the INVITE transaction does not create a dialog, or the INVITE transaction encounters 
    an error, this event will not be sent.
    
    @param aTransaction a complete UAC core INVITE transaction
	*/
    void InviteCompleted( CSIPClientTransaction& aTransaction );
    
   	/**
    Invite was canceled with the CANCEL
    
    @param aTransaction a canceled INVITE UAS transaction
    */
    void InviteCanceled( CSIPServerTransaction& aTransaction );

	/**
	Connection state has changed.If connection state has changed to EInactive or EUnavailable,
	SIP stack has removed all stand-alone SIP refreshes, registrations and dialog associations 
	that client requested to refresh. Client may re-issue refresh requests (stand-alone, 
	registration or dialog association related) when connection becomes EActive again.
	SIP stack also terminates all pending sip client transactions and no errors are reported back 
	to the client about the terminated transactions nor about removed refreshes in order to 
	avoid event flood.
	 
	@param aState indicates the current connection state        
	*/
	void ConnectionStateChanged( CSIPConnection::TState aState );
	
	//From MSIPProfileRegistryObserver
	/** 
	An event related to SIP Profile has accorred

	@param aProfileId a profile Id
	@param aEvent an occurred event
	*/
	void ProfileRegistryErrorOccurred(TUint32 aProfileId, TInt aError);
	
	/**
	An asynchronous error has occurred related to SIP profile Event is send to 
	those observers, who have the corresponding profile instantiated.
	
	@param aProfileId the id of failed profile 
	@param aError an occurred error
	*/
	void ProfileRegistryEventOccurred(TUint32 aProfileId, TEvent aEvent);
	
	/**
	SIP request resulted in 401/407 response that contains
	a challenge.
	
	@param aRealm a realm for which the challenge was received
	*/
	void ChallengeReceived(const TDesC8& aRealm);

private:
	/**
	Instantiation function will be called from a friend class
	
	@param  aSip 	Handle to the SIP
	@param  aIapId	IAP on which the connection will be created
	*/
	static CSIPTransitionEngine* NewL(CSIP& aSip, TInt aIapId);
	
	/**
	constructor of the class
	*/
	CSIPTransitionEngine(CSIP& aSip, TInt aIapId);
	
	/**
	destructor of the class
	*/
	~CSIPTransitionEngine();
	
	/**
	2nd phase construction of the class

	*/
	void	ConstructL();
	
	/**
	This method handles the incoming request if it comes outside of a existing dialog
	
	@param  aTransaction The transaction associated with the new dialog.
	*/
	void IncomingRequestL( CSIPServerTransaction* aTransaction );
	
	/**
	This method handles the requests coming inside a Dialog
	
	@param  aTransaction The transaction associated with the new dialog.
	@param  aDialog  Handle to the dialog itself
	*/
	void IncomingRequestL( CSIPServerTransaction* aTransaction, CSIPDialog& aDialog );
	
	/**
	This method is used to walkthrough the array of credentials 
	and set them once a challenge is recieved
	
	@param aRealm - Realm of the issuer of the challenge
	*/
	TBool FindAndSetCredentials(const TDesC8& aRealm);

	void IncomingRequestHandlerL(CSIPServerTransaction* aTransaction);
	
private:
	// For Logging
	__FLOG_DECLARATION_MEMBER;
	
	// The IAP id of the CSipConnection
	TUint 						iIapId;
	// The connection state 
	CSIPConnection::TState		iConnState;
	// Handle to the SIP stack
	CSIP&						iSIP;
	// Handle to the SIP connection towards a particular IAP
	CSIPConnection*				iConnection;
	// Handle to the SIP Profile Registry
	CSIPProfileRegistry*		iProfileRegistry;
	// Handle to the wait till the Connection Starts
	CActiveSchedulerWait		iActiveWait;
	// Reference count 
	TInt 						iRefCount;
	//Digest Class Instance for Setting Credentials
	CSIPHttpDigest				*iDigest;
	// Holder of Registration clients		
	RPointerArray<MSIPRegistrationClient>	iRegClientArray;
	// Holder of SIP profiles
	RPointerArray<CSIPProfile>		iProfileArray;
	// Holder of SIP state machine
	RPointerArray<CHLStateMachine>	iSMArray;
	//Holder of TSIPCredentials	
	RArray<TSIPCredentials>	iCredentialsArray;
	};


inline CSIPConnection * CSIPTransitionEngine::GetSipConnection()
/**
Return the CSIp connection
*/
	{
	return iConnection;
	}
	
inline void CSIPTransitionEngine::Attach()
/**
Increments the refrence count
*/
	{
	iRefCount++;
	}
//todo
inline TInt CSIPTransitionEngine::Detach()
//inline TInt CSIPTransitionEngine::Detach(MSIPRegistrationClient* aRegClient)
/**
Decrements the reference count
*/
	{
	return --iRefCount;
	}	

inline TInt CSIPTransitionEngine::IapId()
/**
returns the IAP id
*/
	{
	return iIapId;	
	}


#endif
