/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : SipConnectionImplementation.h
* Part of     : SIP Client
* Interface   : SDK API, SIP API
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCONNECTIONIMPLEMENTATION_H
#define CSIPCONNECTIONIMPLEMENTATION_H

// INCLUDES
#include "sipconnection.h"
#include "sipclientconnectionobserver.h"
#include "transactionassociation.h"
#include "siprefreshassociation.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class CSIP;
class MSIPConnectionObserver;
class CSIPTransactionBase;
class CSIPClientTransaction;
class CSIPServerTransaction;
class CRegBindingImplementation;
class CRegistrationState;
class CSIPDialogImplementation;
class CDialogState;
class CSIPRefresh;
class CSIPMessageElements;
class CSIPRequestElements;
class CSIPResponseElements;
class CConnectionCallback;

//CONSTANTS

// CLASS DECLARATION

/**
* Implementation class for managing a connection.
*/
class CSIPConnectionImplementation :
    public CBase,
    public MSIPClientConnectionObserver,
    public MTransactionAssociation,
    public MSIPRefreshAssociation
	{    
    public: //Constructors and destructor

	    /**
        * Two-phased constructor.
		* @param aSIP Handle to the SIP server
	    * @param aConnection Associated CSIPConnection
		* @param aIapId IAP-id
		* @param aObserver a observer for receiving asynchonous events on this
        *   connection
        * @return New object, ownership is transferred
        */
		static CSIPConnectionImplementation*
			NewL(CSIP& aSIP,
				 CSIPConnection& aConnection,
				 TUint32 aIapId,
				 MSIPConnectionObserver& aObserver);

		/**
		* Destructor
		*/
		~CSIPConnectionImplementation();

	public: //From MSIPClientConnectionObserver:
		void IncomingRequestL(CSIPRequestElements* aElements,
							  TUint32 aRequestId);

		void IncomingDialogRequestL(CSIPRequestElements* aElements,
			                  		TUint32 aRequestId,
							  		TUint32 aDialogId);

		void IncomingResponseL(CSIPResponseElements* aElements,
							   TUint32 aRequestId);

		void IncomingDialogResponseL(CSIPResponseElements* aElements,
			                   		 TUint32 aRequestId,
							   		 TUint32 aDialogId);

		void IncomingDialogResponseL(CSIPResponseElements* aElements,
			                   		 TUint32 aRequestId,
	                           		 TUint32 aRefreshId,
							   		 TUint32 aDialogId);

		void IncomingRegistrationResponseL(CSIPResponseElements* aElements,
										   TUint32 aRequestId,
		                                   TUint32 aRefreshId,
								           TUint32 aRegistrationId);		
	 
		void IncomingRefreshResponseL(CSIPResponseElements* aElements,
			                   		  TUint32 aRequestId,
					           		  TUint32 aRefreshId);

		void ErrorOccured(TInt aError, TUint32 aRequestId);
		
		void RefreshErrorOccured(TInt aError, TUint32 aRefreshId); 

		void RegistrationErrorOccured(TInt aError,
	                      			  TUint32 aRegistrationId,
	                      			  TUint32 aRequestId);
		
		void DialogErrorOccured(TInt aError,
	                      		TUint32 aDialogId,
	                      		TUint32 aRequestId);

		void DialogErrorOccured(TInt aError,
	                      		TUint32 aDialogId,
	                      		TUint32 aRefreshId,
	                      		TUint32 aRequestId);
	    
	    void InviteCompleted(TUint32 aRequestId);

	    void ConnectionStateChanged(CSIPConnection::TState aState);

    public: //From MTransactionAssociation:

        CSIPClientConnection& ClientConnectionL();
        
        CSIPClientConnection* ClientConnection();
	
        CSIPConnection& SIPConnectionL();

        void AddTransactionL(CSIPTransactionBase& aTransaction);

        void RemoveTransaction(const CSIPTransactionBase& aTransaction);

    public: //From MSIPRefreshAssociation:
        CSIPClientTransaction*
            UpdateRefreshL(CSIPRefresh& aRefresh,
                           CSIPMessageElements* aElements,
                           TBool aTerminate);        

        void DeletingRefresh(CSIPRefresh& aRefresh, TUint32 aRefreshId);
        
        MTransactionAssociation& TransactionAssociation();
        
        TInt CheckIfStandAlone();

    public: // New functions
        /**
		* Gets current state of the connection        
        * @return current state of the connection
        */
		CSIPConnection::TState State() const;

		/**
        * Send a SIP request.
        *
		* @param aElements SIP headers. Ownership is transferred.
        * @param aRegistrationId If non-NULL, request will use the outbound
        *   proxy and some SIP headers from the registration identified by
        *   aRegistrationId. Ownership is not transferred.
        * @param aRefresh If non-NULL, the request will be refreshed. Otherwise
        *   NULL. Ownership is not transferred.
		* @return New transaction. Ownership is transferred.
        */
        CSIPClientTransaction*
            SendRequestL(CSIPRequestElements* aElements,
                         TUint32* aRegistrationId,
						 CSIPRefresh* aRefresh);

		/**
		* Creates SIP REGISTER request and sends it to the given destination.
		* User must no define any Contact or Expires-headers
		* in the provided elements.
		* @pre State() == EActive
        * @pre aElements != 0
        * @pre aElements->Method().DesC().Length()==0
		* @param aElements contains local and remote addresses, optional SIP
        *        message headers and body. User must not set any expires values
        *        to aElements. The ownership is transferred.
        * @return SIP REGISTER client transaction, ownership is transferred
        */        
		CSIPClientTransaction*
            FetchRegistrationsL(CSIPRequestElements* aElements);

		/**
		* Gets Iap-id used for this connection
		* @return Iap-id
		*/
        TUint32 IapId() const;

		/**
        * Gets SIP server handle
        * @return handle to SIP server, or 0-pointer if SIP server does not
        *	exist any more
        */        
        CSIP* SIP();
        
        /**
        * Gets SIP server handle
        * @return handle to SIP server, or 0-pointer if SIP server does not
        *	exist any more
        */        
        const CSIP* SIP() const;
        
	    void SetOptL(TUint aOptionName,
	                 TUint aOptionLevel,
	                 const TDesC8& aOption);
		
	    void SetOptL(TUint aOptionName, TUint aOptionLevel, TInt aOption);

		/**
        * Searches for a transaction with the matching aRequestId.
		* @param aRequestId RequestId associated with the transaction
		* @return Found transaction or NULL if not found. Ownership is not
        *   transferred.
        */
        CSIPTransactionBase* FindTransaction(TUint32 aRequestId) const;

        void AddRegistrationL(CRegBindingImplementation& aRegistration);

        /**
        * Removes a registration from the array, but doesn't delete it.
		* @param aRegistration Registration binding to be removed
        */
        void RemoveRegistration(const CRegBindingImplementation& aRegistration);

        void AddDialogL(CSIPDialogImplementation& aDialog);
        void RemoveDialog(const CSIPDialogImplementation& aDialog);

        /**
        * Searches for a dialog, using aRequestId as a key.
        * @param aRequestId RequestId of a transaction associated with the
        *   dialog
		* @return Found dialog or NULL if not found. Ownership is not
        *   transferred.
        */
        CSIPDialogImplementation*
        	FindDialogByRequestId(TUint32 aRequestId) const;

        /**
		* Informs CSIPConnection that the associated CSIP instance has been
        * deleted. After this the CSIPConnection object can't be used anymore.
		*/
        void CSIPDeleted();

        /**
		* Obtains the initial state for registration state machine.
        *
        * @return Initial registration state, ownership is not transferred
		*/
        CRegistrationState* InitialRegisterStateL() const;

        /**
		* Obtains the initial state for dialog state machine.
        *
        * @return Initial dialog state, ownership is not transferred
		*/
        CDialogState* InitialDialogStateL() const;

        /**
        * Response related to a dialog has been received.
		*
        * @param aElements SIP response elements. Ownership is transferred.
        * @param aRequestId Identifies the transaction
        * @param aDialogId DialogId
        * @param aRefreshId NULL if the response is not related to a refresh,
        *   otherwise the RefreshId. Ownership is not transferred.
		*/

        void IncomingResponseToDialogL(CSIPResponseElements* aElements,
		                               TUint32 aRequestId,
                                       TUint32 aDialogId,
                                       TUint32* aRefreshId);

        /**
		* Finds a client transaction by its ID. 
		* All connection, registration and dialog related transactions 
		* are searched through.
        *
        * @param aRequestId request ID of the client transaction
        * 
        * @return transaction or NULL if not found, ownership not transferred.
		*/
        CSIPClientTransaction* FindClientTransaction(TUint32 aRequestId) const;

        /**
		* Finds a refresh by its id. 
		* All connection, registration and dialog related refreshes 
		* are searched through.
        *
        * @param aRequestId RequestId associated with the refresh instance
        * @param aRefreshId RefreshId associated with the refresh instance
        * 
        * @return refresh or NULL if not found, ownership is not transferred.
		*/
        CSIPRefresh* FindRefresh(TUint32 aRequestId, TUint32 aRefreshId) const;

    private:
        CSIPConnectionImplementation(CSIP& aSIP,
        							 CSIPConnection& aConnection,
                       				 TUint32 aIapId,
                       				 MSIPConnectionObserver& aObserver);

        void ConstructL();        

        /**
        * Searches for a refresh and transaction, using aRequestId and
        * aRefreshId as keys.        
		* @param aRequestId RequestId associated with the refreshed transaction
        * @param aRefreshId RefreshId associated with the refresh instance
        * @param aRefresh OUT: if refresh was found, this pointer is set to the
        *   refresh instance. Ownership is not transferred.
        * @param aTransaction OUT: if transaction was found, this pointer is
        *   set to the refreshed client transaction. Ownership is not
        *   transferred.
		* @return ETrue if found, EFalse otherwise
        */
        TBool FindRefresh(TUint32 aRequestId,
                          TUint32 aRefreshId,
                          CSIPRefresh** aRefresh,
                          CSIPClientTransaction** aTransaction) const;

  		/**
        * Searches for a registration, using aRegistrationId and aRequestId as
        * keys. Registration might not yet have received RegistrationId from
        * SIP client, so the aRequestId may be needed for searching.
        * @param aRegistrationId RegistrationId of the registration
		* @param aRequestId RequestId of a transaction associated with the
        *   registration
		* @return Found registration binding implementation, or NULL if not
		*	found. Ownership is not transferred.
        */
        CRegBindingImplementation* FindRegistration(TUint32 aRegistrationId,
                             			   		    TUint32 aRequestId) const;
                             			   		  
        /**
        * Searches for a registration, using aRequestId as a key.
        * @param aRequestId RequestId of a transaction associated with the
        *   registration
		* @return Found registration binding implementation, or NULL if not
		*	found. Ownership is not transferred.
        */
        CRegBindingImplementation* FindRegistration(TUint32 aRequestId) const;
        
        /**
        * Searches for a dialog, using aDialogId as a key.        
        * @param aDialogId DialogId of the dialog.
		* @return Found dialog or NULL if not found. Ownership is not
        *   transferred.
        */
        CSIPDialogImplementation* FindDialog(TUint32 aDialogId) const;

        /**
        * A request has been received with a dialogId, but no matching dialog
        * exists.
		* If the request is NOTIFY, it is stored until a response with the same
        * dialogId is received. Then the response is passed to dialog and after
        * that all the stored NOTIFY are passed to the same dialog.
        * This is needed since NOTIFY can be received before a sent SUBSCRIBE
        * receives a response with dialogId.
		*
        * @param aTransaction Server transaction, ownership is transferred
        * @param aDialogId DialogId
		*/
        void IncomingRequestBeforeDialogExistsL(
                                    CSIPServerTransaction* aTransaction,
                                    TUint32 aDialogId);

        /**
		* Extracts the oldest stored transaction for the dialog identified by
        * aDialogId.
        * @param aDialogId DialogId
        * @return Server transaction or NULL if no transaction is stored for
        *   the dialog. Ownership is transferred.
		*/
        CSIPServerTransaction* GetStoredTransaction(TUint32 aDialogId);

        /**
		* Checks that CSIP is available for use (not NULL). If iSIP is NULL,
        * it means user has deleted a resource needed by
        * CSIPConnectionImplementation, and this function leaves.
		*/
        void CheckCSipL() const;

        /**
		* Connection is no longer available. All registrations, dialogs,
        * transactions and refreshes using this connection are terminated, but
        * not deleted.
		*/
        void ConnectionLost() const;

	private: // Data
	
        //The used CSIP instance. CSIPConnectionImplementation doesn't own it.
        //If this is NULL, it means application has deleted the CSIP instance,
        //and the CSIPConnectionImplementation is now useless.
        CSIP* iSIP;

        //Associated CSIPConnection of this instance
        CSIPConnection& iConnection;

        //IAP-id of this connection
        TUint32 iIapId;

        //Callback to upper layer (to application)
        MSIPConnectionObserver& iObserver;

        //Client connection, owned by CSIPConnectionImplementation.
        CSIPClientConnection* iClientConnection;

        //CSIPConnection doesn't own CRegBindingImplementation
		RPointerArray<CRegBindingImplementation> iRegistrations;

        //CSIPConnectionImplementation doesn't own the CSIPDialogImplementation
        //objects. CSIPDialogImplementation is deleted when the last dialog
        //association is deleted.
        //NOTE: When CSIPDialogImplementation has just been created and has no
        //dialog associations yet, it must be stored in CleanupStack, otherwise
        //it wouldn't be freed if leave occurs.
        RPointerArray<CSIPDialogImplementation> iDialogs;

        //CSIPConnectionImplementation has to know which transactions are
        //attached to it, but application still owns the transactions.
        RPointerArray<CSIPTransactionBase> iTransactions;        

        //Standalone refreshes, application owns these
        RPointerArray<CSIPRefresh> iRefreshes;

        //To avoid calling MSIPConnectionObserver from within CSIPDialog and
        //CSIPRegistrationBinding, the CConnectionCallback is passed to the
        //functions which would need to call MSIPConnectionObserver.
        //CSIPRegistrationBinding and CSIPDialog fill CConnectionCallback with
        //the selected callback function and its parameters, and return ETrue
        //to tell to CSIPConnectionImplementation that a callback must be done.
		//CSIPConnectionImplementation owns this.        
        CConnectionCallback* iCallbackInfo;

	private: // For testing purposes
	    UNIT_TEST(CSIP_Test)
        UNIT_TEST(CSIPConnection_Test)
        UNIT_TEST(CSIPSubscribeDialogAssoc_Test)
        UNIT_TEST(CSIPInviteDialogAssoc_Test)
        UNIT_TEST(CSIPReferDialogAssoc_Test)
        UNIT_TEST(CSIPDialogTrying_Test)
        UNIT_TEST(CSIPRegistrationBinding_Test)
        
		void __DbgTestInvariant() const;

	};

#endif
