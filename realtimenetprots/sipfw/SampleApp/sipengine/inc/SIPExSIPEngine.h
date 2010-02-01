    
/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#ifndef _SIPEXSIPENGINE_H_
#define _SIPEXSIPENGINE_H_

// INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <e32math.h>
#include <s32mem.h>
#include <in_sock.h>

#include <sip.h>
#include <sipdialog.h>
#include <sipobserver.h>
#include <sipinvitedialogassoc.h>
#include <sipconnectionobserver.h>
#include <sipservertransaction.h>
#include <sipmessageelements.h>
#include <siprequestelements.h>

#include <sdpdocument.h>
#include <sdpmediafield.h>
#include <sdporiginfield.h>
#include <sdpconnectionfield.h>
#include <sdpcodecstringconstants.h>
#include <sdpcodecstringpool.h>

#include <sipprofile.h>
#include <sipprofileregistry.h>
#include <sipprofileregistryobserver.h>

#include <sipaddress.h>
#include <sipcontenttypeheader.h>
#include <sipfromheader.h>

#include "SIPExSIPEngineObserver.h"

#include "SIPExSIPIdleState.h"
#include "SIPExSIPClientEstablishingState.h"
#include "SIPExSIPClientOfferingState.h"
#include "SIPExSIPServerOfferingState.h"
#include "SIPExSIPServerEstablishingState.h"
#include "SIPExSIPEstablishedState.h"
#include "SIPExSIPTerminatingState.h"


// FORWARD DECLARATIONS
class CSIP;
class CSIPDialog;
class CSIPServerTransaction;
class CSIPMessageElements;
class CSIPRequestElements;

class CSIPAddress;
class CSIPContentTypeHeader;
class CSIPFromHeader;

class MSIPExSIPEngineObserver;

class CSIPExSIPIdleState;
class CSIPExSIPClientEstablishingState;
class CSIPExSIPClientOfferingState;
class CSIPExSIPServerOfferingState;
class CSIPExSIPServerEstablishingState;
class CSIPExSIPEstablishedState;
class CSIPExSIPTerminatingState;


// CLASS DECLARATION

/**
* CSIPExSIPEngine
* Class for implementing the SIP Engine for the SIP
* Example Application. The Engine provides API functions
* for other parts of the Application, and maintains an
* internal state machine to handle the SIP session.
*/
class CSIPExSIPEngine: public CBase,
							   MSIPObserver,
							   MSIPConnectionObserver,
							   MSIPProfileRegistryObserver
	{

    public: // Constructors and destructor

		/**
		* NewL
		* Create new instance of SIP Engine.
		* @param aAppUid Application uid.
		* @param aObserver Pointer to the Engine Observer.
		* @return Pointer to new SIP Engine instance.
		*/
		IMPORT_C static CSIPExSIPEngine* NewL( TUid aAppUid,
												MSIPExSIPEngineObserver* aObserver );
		
		/**
		* Destructor.
		*/
		virtual ~CSIPExSIPEngine();
		
	private:
	
		/**
		* C++ default constructor.
		*/
		CSIPExSIPEngine();
		
		/**
		* 2nd phase construction.
		* @param aAppUid Application uid.
		* @param aObserver Pointer to the Engine Observer.
		*/
		void ConstructL( TUid aAppUid,
						 MSIPExSIPEngineObserver* aObserver );
		
	public:	// New functions - services provided by the SIP Engine

		/**
		* EnableProfileL
		* Enable the default profile.
		* Callback is notified when operation is complete.
		* Leaves if default SIP profile is not found or profile type is not expected.
		* Returns true if profile was registered immediately after enabling.
		*/
		IMPORT_C TBool EnableProfileL();
		
		/**
		* DisableProfile
		* Disable the current profile.
		* Callback is notified when operation is complete.
		*/
		IMPORT_C void DisableProfileL();
		
		
		/**
		* SendInvite
		* Create and send an INVITE to the recipient
		* identified by the parameter.
		* @param aSipUri Address of the recipient.
		*/
		IMPORT_C void SendInviteL( const TDesC8& aSipUri );
		
		
		/**
		* CancelInvite
		* CANCEL a previously sent INVITE. If a final
		* response for the INVITE has been sent, the
		* CANCEL request fails.
		*/
		IMPORT_C void CancelInviteL();


		/**
		* AcceptInvite
		* Send a 200 (OK) response to an INVITE sent
		* by a remote party.
		* @param aIPAddr local ip-address.
		*/
		IMPORT_C void AcceptInviteL(const TInetAddr& aIPAddr );


		/**
		* DeclineInvite
		* Send a 488 (Not Acceptable Here) response to
		* an INVITE sent by a remote party.
		*/
		IMPORT_C void DeclineInviteL();


		/**
		* EndSession
		* Terminate the SIP session by sending a BYE
		* request.
		*/
		IMPORT_C void EndSessionL();


		/**
		* CreateIML
		* Create an instant message and send it to the
		* remote party using the MESSAGE method.
		* @param aMessage The message to be sent.
		* @param aSipUri Address of the recipient.
		*/
		IMPORT_C void CreateIML( const TDesC8& aMessage,
							 	 const TDesC8& aSipUri );
							 	 

	public:		// State machine & internal functionality

		/**
		* IMReceivedL
		* An instant message has been received from the
		* network. Inform the observer.
		* @param aTransaction Contains message elements.
		* Ownership is transferred.
		*/
		void IMReceivedL( CSIPServerTransaction* aTransaction );


		/**
		* IMReceived
		* A non-leaving version of the IMReceivedL, the
		* possible errors are trapped.
		* @param aTransaction Contains message elements.
		* Ownership is transferred.
		*/
		void IMReceived( CSIPServerTransaction* aTransaction );


		/**
		* SetCurrentState
		* Sets the active state of the state machine.
		* @param aState The current state.
		*/
		void SetCurrentState( CSIPExSIPStateBase* aState );
		
    
		/**
		* Connection
		* Sets the active connection.
		*/
		CSIPConnection& ConnectionL();

		/**
		* Profile
		* Returns the enabled profile.
		*/
		CSIPProfile& Profile();

	
		/**
		* SetServerTx
		* Sets the current Server Transaction.
		* The ownership is transferred to the Engine.
		* @param aTx The transaction.
		*/
		void SetServerTx( CSIPServerTransaction* aTx );

		/**
		* ServerTx
		* Gets the current Server Transaction.
		*/
		CSIPServerTransaction& ServerTx();
		
		
		/**
		* SetClientTx
		* Sets the current Client Transaction.
		* The ownership is transferred to the Engine.
		* @param aTx The transaction.
		*/
		void SetClientTx( CSIPClientTransaction* aTx );
		
		/**
		* ClearClientTx
		* Deletes the current Client Transaction.
		*/
		void ClearClientTx();

		/**
		* ClientTx
		* Gets the current Client Transaction.
		*/
		CSIPClientTransaction& ClientTx();
		
		
		/**
		* SetDialogAssoc
		* Sets the current Invite Dialog Association.
		* @param aAssoc The Dialog Assoc.
		*/
		void SetDialogAssoc( CSIPInviteDialogAssoc& aAssoc );
		
		
		/**
		* DialogAssoc
		* Returns the current Invite Dialog Association.
		*/
		CSIPInviteDialogAssoc& DialogAssoc();

		
	public:		// Methods from base classes
	
// From MSIPObserver

		/**
		* IncomingRequest (from MSIPObserver)
		* A SIP request has been received from the network.		
		* @param aIapId The IapId from which
		*        the SIP request was received. 
		* @param aTransaction contains local address,
		*        remote address of a sip message,
		*        as well as optional SIP message method, headers and body.
		*        The ownership is transferred.        
        */

		void IncomingRequest( TUint32 aIapId,
							  CSIPServerTransaction* aTransaction );

		/**
		* TimedOut (from MSIPObserver)
		*/

		void TimedOut( CSIPServerTransaction& aSIPServerTransaction );
		
		
// From MSIPConnectionObserver

		/**
		* IncomingRequest (from MSIPConnectionObserver)
		* A SIP request outside a dialog has been received from the network.
        *
		* @param aTransaction SIP server transaction. The ownership is
        *   transferred.
        */
		void IncomingRequest( CSIPServerTransaction* aTransaction );

		/**
		* IncomingRequest (from MSIPConnectionObserver)
		* A SIP request within a dialog has been received from the network.
		* The client must resolve the actual dialog association to which
		* this request belongs.
		*
		* @param aTransaction SIP server transaction. The ownership is
        *   transferred.
		* @param aDialog the dialog that this transaction belongs to.        
		*/
		void IncomingRequest( CSIPServerTransaction* aTransaction,
							  CSIPDialog& aDialog );

		/**
		* IncomingResponse (from MSIPConnectionObserver)
		*/
		void IncomingResponse( CSIPClientTransaction& aTransaction );

		/**
		* IncomingResponse (from MSIPConnectionObserver)
		* A SIP response that is within a dialog association or creates
		* a dialog association.
        *
		* @param aTransaction contains response elements.
		* @param aDialogAssoc a dialog association.        
		*/
		void IncomingResponse( CSIPClientTransaction& aTransaction,
							   CSIPDialogAssocBase& aDialogAssoc );

        /**
		* IncomingResponse (from MSIPConnectionObserver)
		*/
		void IncomingResponse( CSIPClientTransaction& aTransaction,
							   CSIPInviteDialogAssoc* aDialogAssoc );


		/**
		* IncomingResponse (from MSIPConnectionObserver)
		*/
		void IncomingResponse( CSIPClientTransaction& aTransaction,
							   CSIPRegistrationBinding& aRegistration );


 		/**
		* An asynchronous error has occurred in the stack related to the
		* request indicated by the given transaction.
		*
		* @param aError system wide or sip error code
		* @param aTransaction failed transaction.
		* @param aSIPConnection a SIP connection        
		*/
		void ErrorOccured( TInt aError,
						   CSIPTransactionBase& aTransaction );

		/**
		* An asynchronous error has occurred in the stack related
		* to the request indicated by the given transaction.
        *
		* @param aError system wide or sip error code
		* @param aTransaction the failed transaction.
		* @param aRegistration the failed registration.        
		*/
		void ErrorOccured( TInt aError,
						   CSIPClientTransaction& aTransaction,
						   CSIPRegistrationBinding& aRegistration );

		/**
		* An asynchronous error has occured related to a request within
		* an existing dialog.
        *
		* @param aError system wide or sip error code
		* @param aTransaction the failed transaction.
		* @param aDialogAssoc the failed dialog associoation.        
		*/
		void ErrorOccured( TInt aError,
						   CSIPTransactionBase& aTransaction,
						   CSIPDialogAssocBase& aDialogAssoc );

		/**
		* An asynchronous error has occured related to a refresh 
        *
		* @param aError system wide or sip error code
		* @param aSIPRefresh original refresh object.        
		*/
		void ErrorOccured( TInt aError, CSIPRefresh& aSIPRefresh );

		/**
		* An asynchronous error has occured related to a periodical refresh
        * that relates to a registration.
        *
		* @param aError system wide or sip error code; 
		*		 KErrCouldNotConnect if the refresh has failed
		*		 due to the suspended connection.
		* @param aRegistration associated registration.
		*/
		void ErrorOccured( TInt aError,
						   CSIPRegistrationBinding& aRegistration );

		/**
		* An asynchronous error has occured related to a periodical refresh
        * that belongs to SIP dialog association.
        *
		* @param aError system wide or sip error code; 
		*        KErrCouldNotConnect if the refresh has failed
		*		 due to the suspended connection.
		* @param aDialogAssoc SIP dialog association.        
		*/
		void ErrorOccured( TInt aError,
						   CSIPDialogAssocBase& aDialogAssoc );

        /**
		* SIP stack has completed UAC core INVITE transaction 64*T1 seconds
        * after the reception of the first 2xx response. No more 2xx responses
        * can be received to the issued single INVITE.
        *
        * If the INVITE transaction does not create a dialog, or the INVITE
        * transaction encounters an error, this event will not be sent.
        *
		* @param aTransaction a complete UAC core INVITE transaction
		*/
        void InviteCompleted( CSIPClientTransaction& aTransaction );
        
       	/**
        * Invite was canceled with the CANCEL
        * @param aTransaction a canceled INVITE UAS transaction
        */
        void InviteCanceled( CSIPServerTransaction& aTransaction );

		/**
		* Connection state has changed.
        * If connection state has changed to EInactive or EUnavailable,
		* SIP stack has removed all stand-alone SIP refreshes, registrations 
		* and dialog associations that client requested to refresh. Client may
		* re-issue refresh requests (stand-alone, registration or dialog 
		* association related) when connection becomes EActive again.
		* SIP stack also terminates all pending sip client transactions and no
        * errors are reported back to the client about the terminated
        * transactions nor about removed refreshes in order to avoid event
        * flood.
		* 
		* @param aState indicates the current connection state        
		*/
		void ConnectionStateChanged( CSIPConnection::TState aState );

		
// From MSIPProfileRegistryObserver


        /** 
		* An event related to SIP Profile has accorred
		* @param aProfileId a profile Id
		* @param aEvent an occurred event
		**/
       	void ProfileRegistryEventOccurred( TUint32 aProfileId, TEvent aEvent );

		/**
		* An asynchronous error has occurred related to SIP profile
		* Event is send to those observers, who have the
		* corresponding profile instantiated.
		* @param aProfileId the id of failed profile 
		* @param aError an occurred error
		*/
		void ProfileRegistryErrorOccurred( TUint32 aProfileId, TInt aError );



	public:	// Methods internal to the Engine

		/**
		* CreateToHeaderLC
		* Return a To header object based on URI.
		* @param aSipUri The URI address as a string.
		* @return Pointer to CSIPToHeader instance.
		* Ownership is transferred.
		*/
		CSIPToHeader* CreateToHeaderLC( const TDesC8& aSipUri );
		
		/**
		* CreateReqElementsLC
		* Return a RequestElements object based on URI.
		* @param aSipUri The URI address as a string.
		* @return Pointer to CSIPRequestElements instance.
		*/
		CSIPRequestElements* CreateReqElementsLC( const TDesC8& aSipUri );
			
		/**
		* CreateMessageElementsLC
		* Instantiate a Message Elements object.
		* @return Pointer to CSIPMessageElements instance.
		* Ownership is transferred.
		*/
		CSIPMessageElements* CreateMessageElementsLC();
		
		/**
		* ConvertToUri8LC
		* Convert textual representation of uri to CUri8
		* @return Pointer to CUri8 instance.
		* Ownership is transferred.
		*/
		CUri8* ConvertToUri8LC( const TDesC8& aSipUri );

		/**
		* SdpDocumentLC
		* Instantiate a SDP Document object.
		* @return Pointer to CSdpDocument instance.
		* Ownership is transferred.
		*/
		CSdpDocument* SdpDocumentLC();

		/**
		* SdpBodyL
		* Return SDP message body in textual form.
		* @return Pointer to SDP Document as HBufC8.
		* Ownership is transferred.
		*/
		HBufC8* SdpBodyL( CSdpDocument* aDocument );


		/**
         * Get SDP codec string pool. Open string pool if not opened.
		 *
         * @return String pool.
         */
		RStringPool StringPoolL();


		/**
		* IPAddressFromResponseElementsL
		* Get IP Address from the Response Elements
		* received from peer.
		* @param aRespElem The Response Elements.
		*/
		const TInetAddr IPAddressFromResponseElementsL(
			const CSIPResponseElements& aRespElem );


		/**
		* Observer
		* Return a pointer to the Engine Observer class.
		*/
		MSIPExSIPEngineObserver* Observer();
		
	private:

		/**
		* SessionId
		* Return the Session ID
		*/
		TInt64 SessionId();
		
		void MediaFieldsL( CSdpDocument* aDocument );
		
		/**
		* CurrentConnection returns currently used connection.
		* Can be used also for checking if connection exists.
		* Returns either iConnection, iNotOwnedConnection or NULL
		*/
		CSIPConnection* CurrentConnection();

		/**
		* Handle SIP profile registration event
		* @param aSIPProfileId id of registered profile
		*/
		void HandleProfileRegistered( TUint32 aSIPProfileId );
		
		/**
		* Handle SIP profile deregistration event
		* @param aSIPProfileId id of deregistered profile
		*/
		void HandleProfileDeregistered( TUint32 aSIPProfileId );

		/**
		* Handle SIP profile destruction event.
		* Event is send to those observers, who have the
		* corresponding profile instantiated.
		* @param aSIPProfileId id of profile which was destroyed
		*/
		void HandleProfileDestroyed( TUint32 aSIPProfileId );
		


	private:	// Data

		TInt64						iSessionId;
		TInetAddr					iLocalAddr;
		MSIPExSIPEngineObserver*	iObserver;

		CSIP*						iSIP;
		CSIPProfile*				iProfile;
		CSIPProfileRegistry*		iProfileRegistry;
		CSIPConnection*				iConnection;
		CSIPInviteDialogAssoc*		iDialogAssoc;

		CSIPConnection::TState		iConnState;

		CSIPExSIPStateBase*			iCurrentState;
		CSIPClientTransaction*		iClientTx;
		CSIPServerTransaction*		iServerTx;

		// States of the machine
		CSIPExSIPIdleState*					iIdle;
		CSIPExSIPClientEstablishingState*	iClientEstablishing;
		CSIPExSIPClientOfferingState*		iClientOffering;
		CSIPExSIPServerOfferingState*		iServerOffering;
		CSIPExSIPServerEstablishingState*	iServerEstablishing;
		CSIPExSIPEstablishedState*			iEstablished;
		CSIPExSIPTerminatingState*			iTerminating;
		
	};

#endif	// _SIPEXSIPENGINE_H_
