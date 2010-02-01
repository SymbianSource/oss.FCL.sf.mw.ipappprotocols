/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : SipDialogImplementation.h
* Part of     : SIP Client
* Interface   : SDK API, SIP Client API
* Version     : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef CSIPDIALOGIMPLEMENTATION_H
#define CSIPDIALOGIMPLEMENTATION_H

// INCLUDES
#include <e32base.h>
#include <stringpool.h>
#include "sipdialog.h"
#include "siptransactionbase.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class CSIPConnection;
class CSIPConnectionImplementation;
class MSIPRegistrationContext;
class CSIPDialogAssocBase;
class CSIPDialogAssocImplementation;
class CSIPInviteDialogAssoc;
class CSIPSubscribeDialogAssoc;
class CSIPNotifyDialogAssoc;
class CSIPReferDialogAssoc;
class CSIPMessageElements;
class CSIPRefresh;
class CSIPClientTransaction;
class CSIPServerTransaction;
class CDialogState;
class CSIPFromToHeaderBase;
class CSIPFromHeader;
class CSIPToHeader;
class CUri8;
class CSIPContactHeader;
class CSIPCallIDHeader;
class CConnectionCallback;

// CLASS DECLARATION

/**
*  Implementation class for managing SIP dialogs.
*/
class CSIPDialogImplementation: public CBase
	{	
	public: // Constructors and destructor

		/**
        * Creates a new CSIPDialogImplementation, and pushes it to cleanupstack.
        * @param aConnImplementation Connection implementation to use with the
        *	new dialog.
        * @param aContext Registration context whose outbound proxy and other
		*        parameters are to be used. If NULL, context isn't used.
		*		 Ownership is not transferred.
        * @return New object, ownership is transferred.
        */
		static CSIPDialogImplementation*
			NewLC(CSIPConnectionImplementation& aConnImplementation,
				  const MSIPRegistrationContext* aContext=0);
						
		/**
		* Two-phased constructor. 
		* This constructor should be used if the user has received
		* SIP request that creates a SIP dialog association.
        *
        * @param aDialog Dialog which creates this CSIPDialogImplementation.
        *	Ownership is transferred.
        * @param aConnImplementation Implementation of the used SIP connection
        * @return New object, ownership is transferred.
		*/
		static CSIPDialogImplementation*
			NewL(CSIPDialog* aDialog,
				 CSIPConnectionImplementation& aConnImplementation);		

		/**
		* Two-phased constructor
		* This constructor should be used if the user has received
		* SIP request that creates a SIP dialog association.
        *
        * @param aDialog Dialog which creates this CSIPDialogImplementation.
        *	Ownership is transferred.
        * @param aConnImplementation Implementation of the used SIP connection
		* @param aContext Registration context whose outbound proxy and other
		*        parameters are to be used.
        * @return New object, ownership is transferred.
		*/
		static CSIPDialogImplementation*
			NewL(CSIPDialog* aDialog,
				 CSIPConnectionImplementation& aConnImplementation,
                 const MSIPRegistrationContext& aContext);		

		/**
		* Destructor
		*/
		~CSIPDialogImplementation();

	public: // New functions

		/**
		* Gets dialog state		
		* @return dialog state
		*/
        CSIPDialog::TState State() const;

		/**
		* Gets all dialog associations. 		
		* @return All dialog associations. Ownership of the array or the items
        *   inside it, is not transferred.
		*/
		const RPointerArray<CSIPDialogAssocBase>& SIPDialogAssociations() const;

        /**
        * Gets used registration context for this dialog
        * @return associated registration or 0-pointer otherwise.
        *   Ownership is not transferred.
        */
        const MSIPRegistrationContext* RegistrationContext() const;
	
        /**
		* Checks if the dialog association belongs to this dialog
		* @param aAssoc a dialog association
		* @return ETrue if belongs, EFalse otherwise
		*/
		TBool IsAssociated(const CSIPDialogAssocBase& aAssoc) const;

        /**
		* Gets the SIP connection used for this dialog
		* @return SIP connection used for the dialog, or 0-pointer if the
        *   connection has been deleted. Ownership isn't transferred.
		*/
        CSIPConnection* Connection();

        /**
		* Gets the SIP connection used for this dialog.
		* @return SIP connection used for the dialog, or 0-pointer if the
        *   connection has been deleted. Ownership isn't transferred.
		*/
        const CSIPConnection* Connection() const;

        /**
		* Gets originator's address
		* @return originator's address (From-header)
		*/
		const CSIPFromHeader& FromHeader() const;

		/**
		* Gets recipient's address
		* @return recipient's address (To-header)
		*/
		const CSIPToHeader& ToHeader() const;

		/**
		* Gets remote-uri used during dialog creation
		* @return Remote target uri
		*/
		const CUri8& RemoteURI() const;
		
		/**
		* Gets the Call-ID of the dialog.
		* @return Call-ID
		*/
		const CSIPCallIDHeader& CallIdL() const;

		/**
		* Get the Call-ID from SIP Client and store it.		
		*/
		void StoreCallIdL();

		/**
		* Copies Call-ID from aDialog.
		* @pre iCallID == NULL
		* @param aDialog CSIPDialogImplementation where the Call-ID is copied
		*	from.		
		*/
		void CopyCallIdL(const CSIPDialogImplementation& aDialog);

        /**
		* Compares this object to another object
		* @param aDialog CSIPDialogImplementation object to compare
		* @returns ETrue if the objects are equal, otherwise EFalse
		*/
        TBool operator==(const CSIPDialogImplementation& aDialog) const;
        
        TInt ReuseInitialRequestData();
        
        CSIPDialog& Dialog();

        TBool IncomingResponseL(CSIPResponseElements* aElements,
		                        TUint32 aRequestId,
					            TUint32 aDialogId,
                                CConnectionCallback& aCallback);

        TBool IncomingResponseL(CSIPResponseElements* aElements,
		                        TUint32 aRequestId,
                                TUint32 aRefreshId,
					            TUint32 aDialogId,
                                CConnectionCallback& aCallback);

        TBool ResponseToRefreshL(CSIPResponseElements* aElements,
                                 TUint32 aRequestId,
                                 TUint32 aRefreshId,
                                 CConnectionCallback& aCallback,
                                 const CDialogState& aEarly,
                                 const CDialogState& aConfirmed,
                                 const CDialogState& aTerminated,
                                 TBool aIsConfirmed=EFalse);

		/**
		* Handle an incoming SIP request.
        *
        * @param aTransaction Server transaction, ownership is transferred.
        * @param aCallback For selecting a callback function and its parameters
        * @return ETrue if the caller should make a callback function call,
    	*   EFalse otherwise
		*/
        TBool IncomingRequestL(CSIPServerTransaction* aTransaction,
                               CConnectionCallback& aCallback);                              

        TBool ErrorOccured(TInt aError,
                           TUint32 aRequestId,
                           CConnectionCallback& aCallback);
        
        TBool ErrorOccured(TInt aError,             
                           TUint32 aRefreshId,
                           TUint32 aRequestId,
                           CConnectionCallback& aCallback);

		/**
		* Obtains the dialog id.
        *        
        * @return TSIPDialogId
		*/
        TUint32 DialogId() const;

		/**
		* Set the dialog id.
        *
        * @param aDialogId Dialog id        
		*/
        void SetDialogId(TUint32 aDialogId);

		/**
		* Searches for a transaction having the specified id.
        *
        * @param aRequestId Identifier used for searching the transaction.        
        * @return CSIPTransactionBase The found transaction or NULL if
        *	transaction wasn't found. Ownership is not transferred.
		*/
        CSIPTransactionBase* FindTransaction(TUint32 aRequestId) const;

		/**
		* Searches for a transaction having the specified id.
        *
        * @param aRequestId Identifier used for searching the transaction
        * @param aTransaction OUT: pointer is set to the found transaction
        * @param aAssoc OUT: pointer is set to the dialog association related
        *	to the found transaction
        * @return ETrue if found. EFalse if not found, in this case aTransaction
        *	and aAssoc haven't been set.
		*/
        TBool FindTransactionAndAssoc(TUint32 aRequestId,
                                    CSIPTransactionBase** aTransaction,
                                    CSIPDialogAssocBase** aAssoc) const;

        /**
		* Searches for the dialog association and refresh by the aRefreshId.
        *
        * @param aRefreshId RefreshId
        * @param OUT: Dialog association, if found
        * @param OUT: aRefresh CSIPRefresh, if found
        * @return ETrue if a matching refresh and dialog association were
        *   found, EFalse otherwise
		*/
        TBool FindAssocAndRefresh(TUint32 aRefreshId,
                                  CSIPDialogAssocBase** aAssoc,
                                  CSIPRefresh** aRefresh) const;

        /**
		* Searches for the dialog association, refresh and transaction.
        * Both aRequestId and aRefreshId are used for search, because either
        * of them might not yet be stored in the SIP API.        
        *
        * @param aRequestId RequestId
        * @param aRefreshId RefreshId
        * @param aAssoc OUT: Dialog association, if found
        * @param aRefresh OUT: CSIPRefresh, if found
        * @param aTransaction OUT: Transaction, if found
        * @return ETrue if a matching dialog association and refresh were
        *   found, EFalse otherwise
		*/
        TBool FindAssocAndRefreshL(TUint32 aRequestId,
                                   TUint32 aRefreshId,
                                   CSIPDialogAssocBase** aAssoc,
                                   CSIPRefresh** aRefresh,
                                   CSIPTransactionBase** aTransaction) const;
        /**
		* Finds a refresh by first the given request ID and the the refresh ID. 
		* All dialog associations are searched through.
        *
        * @param aRequestId RequestId associated with the refresh instance
        * @param aRefreshId RefreshId associated with the refresh instance
        * 
        * @return refresh or NULL if not found, ownership is not transferred.
		*/
        CSIPRefresh* FindRefresh(TUint32 aRequestId,
                                 TUint32 aRefreshId) const;

        /**
		* Changes the dialog's current state.
        *
		* @param aNewState State which dialog enters		
		*/
        void ChangeState(const CDialogState* aNewState);

        /**
		* Connection state is no longer available.        		
		*/
        void ConnectionLost();

        CSIPClientTransaction*
            SendNonTargetRefreshRequestL(CSIPDialogAssocImplementation& aAssoc,
                                         RStringF aMethod,
                                         CSIPMessageElements* aElements) const;

        CSIPClientTransaction*
            SendInviteL(CSIPInviteDialogAssoc& aAssoc,                                            
                        CSIPMessageElements* aElements) const;

        CSIPClientTransaction*
            SendPrackL(CSIPInviteDialogAssoc& aAssoc,                                            
                       CSIPMessageElements* aElements) const;

        CSIPClientTransaction*
            SendUpdateL(CSIPInviteDialogAssoc& aAssoc,
                        CSIPMessageElements* aElements) const;

        void SendAckL(CSIPInviteDialogAssoc& aAssoc,
                      const CSIPClientTransaction& aTransaction,
                      CSIPMessageElements* aElements) const;
        
        CSIPClientTransaction*
            SendSubscribeL(CSIPSubscribeDialogAssoc& aAssoc,
                           CSIPMessageElements* aElements,
                           CSIPRefresh* aRefresh) const;

		CSIPClientTransaction*
			UpdateL(CSIPSubscribeDialogAssoc& aAssoc,
			        CSIPMessageElements* aElements) const;
			                      
        CSIPClientTransaction*
            SendUnsubscribeL(CSIPSubscribeDialogAssoc& aAssoc,
                             CSIPMessageElements* aElements) const;

		/**
		* Send a SIP response to network. Application can't send a 100 response.
		* That has already been checked by CSIPResponseElements.
		* @param aElements Response elements
		* @param aRequestId Identifies the request to which this response is for
		* @param aAffectsDialogState Tells if the response can cause dialog to
		*		 enter another state.
		* @param aTargetRefresh Tells if this is a response to a target refresh
		*		 request.
		*/
        void SendResponseL(const CSIPResponseElements& aElements,
						   TUint32 aRequestId,
                           TBool aAffectsDialogState,
                           TBool aTargetRefresh=EFalse);

        CSIPClientTransaction* SendByeL(CSIPInviteDialogAssoc& aAssoc,                                        
                                        CSIPMessageElements* aElements) const;

        CSIPClientTransaction* SendCancelL(TUint32 aRequestId) const;

		CSIPClientTransaction*
			SendNotifyL(CSIPNotifyDialogAssoc& aAssoc,
                        CSIPMessageElements* aElements) const;
                        
        CSIPClientTransaction*
			SendReferL(CSIPReferDialogAssoc& aAssoc,
                       CSIPMessageElements* aElements) const;

		/**
		* Add a dialog association to this dialog. A dialog can have only one
		* INVITE association at a time.
		* @param aAssoc Dialog association
		* @param aType Type of the dialog association
		* @leave KErrAlreadyExists If aAssoc is already associated with this
		*		 dialog or if attempting to add another INVITE dialog
		*		 association.
		*/
        void AddAssocL(CSIPDialogAssocBase& aAssoc, RStringF aType);

		/**
		* Remove a dialog association from this dialog.
		* When there are no associations left, the dialog is deleted.		 
		* @param aAssoc Dialog association		
		*/
        void RemoveAssoc(const CSIPDialogAssocBase& aAssoc);

        /**
		* Informs CSIPDialogImplementation that the associated CSIPConnection
		* has been deleted. After this the CSIPDialogImplementation object can't
		* be used anymore.
		*/
        void ConnectionDeleted();

        void CheckNoTransactionExistsL() const;        

		/**
		* Send a request that creates the dialog.
        *
        * @param aAssoc Dialog association which sends the request.
        * @param aElements Message elements, can be NULL. Ownership is
        *	transferred.
        * @param aRefresh Refresh object, if the request is to be refreshed.
        *	Ownership isn't transferred.
        * @return Client transaction. Ownership is transferred.
		*/
		CSIPClientTransaction*
			SendDialogCreatingRequestL(CSIPDialogAssocImplementation& aAssoc,
									   CSIPMessageElements* aElements,
									   CSIPRefresh* aRefresh=0);
									  
		/**
		* Sends a SIP request described by aElements withing dialog and creates
		* a client transaction representing the request.
        *
        * @pre aMethod.Length() > 0
        *
        * @param aAssoc Dialog association with which the transaction will be
        *	associated
        * @param aMethod SIP request method
        * @param aElements Optional SIP message headers and body, can be NULL.
        *	Ownership is transferred.
        * @return CSIPClientTransaction new transaction, ownership is
        *	transferred
		*/
        CSIPClientTransaction*
        	SendRequestInDialogL(CSIPDialogAssocImplementation& aAssoc,
                                 RStringF aMethod,
                                 CSIPMessageElements* aElements) const;

		/**
		* Creates a client transaction.
        *
        * @param aType Transaction type        
        * @param aAssoc Dialog association with which the transaction will be
        *	associated
        * @param aRefresh If transaction is refreshed, this points to a
        *   CSIPRefresh, otherwise this is NULL. Ownership is not transferred.
        * @return CSIPClientTransaction new transaction, ownership is
        *	transferred
		*/
        CSIPClientTransaction*
            CreateClientTransactionL(RStringF aType,                                     
                                     CSIPDialogAssocImplementation& aAssoc,
                                     CSIPRefresh* aRefresh) const;

        /**
		* Sets the dialog related headers when a SIP request that creates a SIP
		* dialog association, is sent. Headers can be set only once.
		* If aTo is NULL, aRemoteUri is used as To-header.
        *
        * @pre aRemoteUri != NULL &&
        *	   iRemoteUri == NULL &&
        *      iFrom == NULL &&
        *      iTo == NULL &&
        *      iContact == NULL
        * @param aFrom Originator's address, can be NULL. Ownership is
        *   transferred.
        * @param aTo Recipient's address, can be NULL. Ownership is transferred
        * @param aRemoteURI Remote target URI. ownership is transferred.
        * @param aContact Contact to be used in dialog creation, can be NULL.
        *		 Must be given only if user intends to re-direct future
        *		 requests. Ownership is transferred.
		*/
        void SetHeadersL(CSIPFromHeader* aFrom,
                         CSIPToHeader* aTo,
                         CUri8* aRemoteUri,
                         CSIPContactHeader* aContact);

        /**
		* Stores the To and From headers of the server transaction and marks
        * the transaction to affect the dialog state.
        *
        * @pre
        * @param aTransaction Server transaction which causes a dialog to be
        *   created.
		*/		
        void InitialTransactionReceivedL(CSIPServerTransaction& aTransaction);

		const CSIPContactHeader* ContactHeader() const;

		/**
		* The dialog creating transaction has been initiated.
		*
		* @pre aRequestId != 0
		* @pre iInitialRequestId == 0
		* @post iInitialRequestId == aRequestId
		*
		* @param aRequestId RequestId of the transaction that creates the dialog
		*/
        void InitialTransactionStarted(TUint32 aRequestId);

		/**
		* Determine if an incoming NOTIFY causes dialog to enter confirmed
		* state.
		*
		* @return ETrue Dialog should enter confirmed state at the reception of
		*	NOTIFY. EFalse otherwise.		
		*/
		TBool DoesNotifyConfirmDialog() const;

		/**
		* Searches if there are dialog related requests, that are being
		* refreshed. If found, those that are in state CSIPRefresh::EInactive
		* are moved into CSIPRefresh::EActive state.		
		*/
		void ChangeRefreshesToActive() const;
		
		/**
		* If aMethod is a target refresh request, and aElements contains a
		* Contact-header, the remote target URI of the dialog is updated.
		*
		* @param aMethod SIP request method
		* @param aElements Message elements
		*/
		void UpdateRemoteTargetL(RStringF aMethod,
								 const CSIPMessageElements& aElements);

		/**
		* Creates a copy of the URI in Contact-header, if aElements contains
		* exactly one Contact-header.
		*		
		* @param aElements Message elements
		* @return Copy of the URI in Contact, or NULL if aElements doesn't have
		*   one Contact header. Ownership is transferred.
		*/
		CUri8* GetUriFromContactL(const CSIPMessageElements& aElements) const;

		/**
		* Based on the received response, dialog may enter another state.
		* A 100 response should not have a To-tag, so dialog won't
		* enter Early-state when a 100 is received.
        *
        * @param aTransaction Client transaction carrying the received response
        * @param aEarly Early-state of the dialog state machine
        * @param aConfirmed Confirmed-state of the dialog state machine
        * @param aTerminated Terminated-state of the dialog state machine
		*/		
        void UpdateState(const CSIPClientTransaction& aTransaction,
                         const CDialogState& aEarly,
                         const CDialogState& aConfirmed,
                         const CDialogState& aTerminated);

		/**
		* If the local tag is not yet known, obtain it from the SIP server via
		* an ITC operation.
		*
		* @param aClientInitiatedDialog
		*	ETrue if the dialog creating request was sent by the local endpoint
		*	EFalse otherwise
		*/
        void FillLocalTagL(TBool aClientInitiatedDialog) const;

        /**
		* If aToHeader has a tag parameter, the tag is stored as a remote-tag
		* in the To-header of the dialog.
		*
		* @param aToHeader To-header of a response from remote endpoint.		
		*/
        void FillRemoteTagL(const CSIPToHeader& aToHeader) const;
        
        TUint32 RegistrationId() const;

    private: // Constructors, for internal use

        CSIPDialogImplementation(
        	CSIPConnectionImplementation& aConnImplementation);

        CSIPDialogImplementation(
        	CSIPConnectionImplementation& aConnImplementation,
            const MSIPRegistrationContext& aContext);

        void ConstructL(CSIPDialog* aDialog);

	private: // New functions, for internal use

        /**
		* Checks that CSIPConnection is available for use (not NULL). If
        * iConnection is NULL, it means user has deleted a resource needed by
        * CSIPDialogImplementation, and this function leaves.
		*/
        void CheckConnectionL() const;

		CSIPConnection* GetConnection() const;

        /**
		* Returns the current dialog state. If state handler is not available,
        * meaning user has deleted a resource needed by
        * CSIPDialogImplementation, this function leaves.        
		*/
        const CDialogState& StateL() const;        

        /**
		* Checks whether the dialog has an INVITE assocation.
        *
        * @return ETrue There is an INVITE association, EFalse otherwise
		*/
        TBool HasInviteAssoc() const;

		/**
		* The dialog creating transaction has been created. The SIP responses
		* to aTransaction will affect the state of dialog.
		*
		* @pre iInitialRequestId == 0
		*
		* @param aTransaction Transaction that creates the dialog.
		*/
        void InitialTransactionStarted(CSIPTransactionBase& aTransaction);

	private: // Data

		//Dialog instance, Owned.
		CSIPDialog* iDialog;

        //DialogId received from SIP client
        TUint32 iDialogId;

        //Current dialog state. Not owned.
        const CDialogState* iState;

        //Initially zero. Is the requestId of the transaction that creates the
        //dialog. RequestId is kept even after the transaction has been deleted.
        TUint32 iInitialRequestId;

        //Connection implementation associated with the dialog. Not owned.
        //If NULL, application has deleted the CSIPConnection associated with
        //the CSIPConnectionImplementation, and this CSIPDialogImplementation
        //is useless as it can't use the CSIPConnection anymore.
        CSIPConnectionImplementation* iConnection;

        //Associated registration, can be NULL. Not owned.
        const MSIPRegistrationContext* iRegistration;

        //Dialog associations. Not owned.
        RPointerArray<CSIPDialogAssocBase> iDialogAssocs;

        //From header of the request that created the dialog. Owned.
        CSIPFromHeader* iFrom;
        //To header of the request that created the dialog. Owned.
        CSIPToHeader* iTo;

        //Remote target, can be NULL. Owned.
        CUri8* iRemoteUri;

        //Contact given by application. Will be deleted after the dialog
        //creating request has been sent. Owned.
        CSIPContactHeader* iContact;

        //Call-ID of the dialog. Owned.
        CSIPCallIDHeader* iCallID;

		TBool iStringPoolOpened;
                
	private: // For testing purposes

	    UNIT_TEST(CSIP_Test)
        UNIT_TEST(CSIPInviteDialogAssoc_Test)
        UNIT_TEST(CSIPSubscribeDialogAssoc_Test)
        UNIT_TEST(CSIPReferDialogAssoc_Test)
        
		void __DbgTestInvariant() const;

	};

#endif
