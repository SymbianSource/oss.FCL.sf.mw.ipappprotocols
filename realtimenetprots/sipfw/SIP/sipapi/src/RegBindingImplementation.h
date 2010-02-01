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
* Name        : RegBindingImplementation.h
* Part of     : SIP Client
* Interface   : SDK API, SIP Client API
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CREGBINDINGIMPLEMENTATION_H
#define CREGBINDINGIMPLEMENTATION_H

// INCLUDES
#include <stringpool.h>
#include "sipregistrationcontext.h"
#include "transactionassociation.h"
#include "siprefreshassociation.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class CSIPRegistrationBinding;
class CSIPResponseElements;
class CSIPContactHeader;
class CSIPRouteHeader;
class CUri8;
class CRegistrationState;
class CSIPToHeader;
class CSIPFromHeader;
class CConnectionCallback;


// CLASS DECLARATION

/**
*  Implementation class for managing SIP registration binding.
*/
class CRegBindingImplementation :
    public CBase,
    public MSIPRegistrationContext,
    public MTransactionAssociation,
    public MSIPRefreshAssociation
	{
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
		* @pre aAor != 0 &&		
		*      aContact != 0
        * @pre "Expires" parameter in aContact must be >= KSIPMinExpirationValue
        * @param aRegistration Registration binding who creates this
        *		 CRegBindingImplementation instance.
		* @param aConnection The SIP connection to be used
		* @param aAor contains user's address-of-record to register;
		*        the ownership is transfered
		* @param aContact contact(s) to be registered. The "expires" parameter
		*        indicates how long the client would like the binding to be
        *        valid.
		*        "expires" parameter value must be >= KSIPMinExpirationValue; 
		*		 the ownership is transfered
 		* @param aRefresh if set, created binding will be refreshed;
		*        the ownership is transfered
		* @param aOutboundProxy an outbound proxy address;
		*        the ownership is transfered
		* @param aRemoteUri if set, will be used in request-uri construction;
		*        the ownership is transfered
        * @param aFrom From header, the ownership is transferred
        * @return New object, ownership is transferred.
		*/
		static CRegBindingImplementation*
            NewL(CSIPRegistrationBinding& aRegistration,
            	 CSIPConnection& aConnection,
			     CSIPToHeader* aAor,
			     CSIPContactHeader* aContact,
 			     CSIPRefresh* aRefresh,
			     CSIPRouteHeader* aOutboundProxy,
			     CUri8* aRemoteUri,
                 CSIPFromHeader* aFrom);

		/**
		* Destructor
		*/
		~CRegBindingImplementation();

	public: // From MSIPRegistrationContext

        /**
        * Checks if the registration context can be
        * used for creating SIP messages/dialogs
        * @return ETrue if the registration binding exists; EFalse
        *         otherwise
        */
        TBool IsContextActive() const;

        /**
        * Gets the context id
        * @return context id
        * For internal use only
        */
        TUint32 ContextId() const;

	public: // From MTransactionAssociation

        CSIPClientConnection& ClientConnectionL();
        
        CSIPClientConnection* ClientConnection();

        CSIPConnection& SIPConnectionL();

        void AddTransactionL(CSIPTransactionBase& aTransaction);

        void RemoveTransaction(const CSIPTransactionBase& aTransaction);

    public: // From MSIPRefreshAssociation

        CSIPClientTransaction*
            UpdateRefreshL(CSIPRefresh& aRefresh,
                           CSIPMessageElements* aElements,
                           TBool aTerminate);

        void DeletingRefresh(CSIPRefresh& aRefresh, TUint32 aRefreshId);
        
        MTransactionAssociation& TransactionAssociation();
        
        TInt CheckIfStandAlone();

    public: // New functions

        /**
		* Creates SIP REGISTER request, sends it to the given destination.
		* Received 2xx response will create at least one registration
		* binding.
		* @pre Connection().State()==EActive
		* @param aElements contains user SIP headers and content; the ownership
		*        is taken
        * @return SIP REGISTER transaction; the ownership is transfered
        * @leave KErrSIPMalformedMessage if aElements contain contact or
        *		 expires headers
        */
		CSIPClientTransaction* RegisterL(CSIPMessageElements* aElements=0);	

        /**
		* Creates SIP (de)REGISTER request, sends it to the given destination.
		* This function will remove registration binding.
		* Any SIP response (2xx - 6xx) to this request removes the binding.
		* Note that deregistration can be done then the 2xx is received to the
		* initial REGISTER transaction and there is no on-going registration
		* updates.
		* @pre Connection().State()==EActive
		* @param aElements contains user SIP headers and content; the ownership
		*        is transfered. If set, user SIP headers must not contain any 
		*        Contact-header nor Expires-header
        * @return SIP REGISTER transaction; the ownership is transfered
        */
		CSIPClientTransaction* DeregisterL(CSIPMessageElements* aElements=0);

		CSIPClientTransaction*
            UpdateL(CSIPMessageElements* aElements, TUint* aExpirationValue=0);

        /**
		* Gets the SIP connection used for the registration.
		* @return SIP connection used for the registration, or 0-pointer if the
        *   	  connection has been deleted. Ownership is not transferred.
		*/
        CSIPConnection* Connection();

        /**
		* Gets the SIP connection used for the registration.
		* @return SIP connection used for the registration, or 0-pointer if the
        *   	  connection has been deleted. Ownership is not transferred.
		*/
        const CSIPConnection* Connection() const;

		/**
		* Sets/resets outbound proxy.
		* If a 0-pointer is given the outbound proxy will be reset.
		* @param aOutboundProxy an outbound proxy. Ownership is transferred.		
		*/
		void SetOutboundProxyL(CSIPRouteHeader* aOutboundProxy);

		/**
		* Gets the address-of-record (AOR) used for creating binding
		* @return address-of-record (AOR)
		*/
		const CSIPToHeader& Aor() const;

		/**
		* Gets Contact header associated with the registration binding
		* @return Contact header
		*/
		const CSIPContactHeader& ContactHeader() const;

		/**
		* Gets Contact header associated with the registration binding
		* @return Contact header
		*/
		CSIPContactHeader& ContactHeader();
		
        /**
        * Gets the registered Contact-header received in the 200 OK response.
        * @return Contact header, or NULL if IsContextActive() is EFalse.
        *         Ownership is not transferred.
        */
        const CSIPContactHeader* RegisteredContact() const;
		
		/**
		* Gets used outbound proxy
		* @return outbound proxy or a 0-pointer if not set. Ownership is not
        *   transferred.
		*/
		const CSIPRouteHeader* OutboundProxy() const;        

        /**
		* Gets associated refresh in case the user has requested the refresh of
        * the registration. Note that client cannot use update or terminate
        * functions defined in the CSIPRefresh class. 
		* @return Associated refresh or a 0-pointer if the user has not
        *   requested a refresh of registration binding. Ownership is not
        *	transferred.
		*/
		const CSIPRefresh* SIPRefresh() const;

       /**
		* Gets associated refresh in case the user has requested the refresh of
        * the registration. Note that client cannot use update or terminate
        * functions defined in the CSIPRefresh class. 
		* @return Associated refresh or a 0-pointer if the user has not
        *  requested a refresh of registration binding. Ownership is not
        *  transferred.
		*/
		CSIPRefresh* SIPRefresh();
		
        /**
		* Compares this object to another object
		* @param aRegistration CRegBindingImplementation object to compare
		* @return ETrue if the objects are equal, otherwise EFalse
		*/
        TBool operator==(const CRegBindingImplementation& aRegistration) const;

        CSIPClientTransaction* DoRegisterL(CSIPMessageElements* aElements);

        CSIPClientTransaction* DoDeregisterL(CSIPMessageElements* aElements);

        CSIPClientTransaction* DoUpdateL(CSIPMessageElements* aElements,
                                         TUint* aExpirationValue);

        TBool IncomingResponseL(CSIPResponseElements* aElements,
		                        TUint32 aRequestId,
					            TUint32 aRegistrationId,
					            TUint32 aRefreshId,
                                CConnectionCallback& aCallback);

        TBool ErrorOccured(TInt aError,
                           TUint32 aRequestId,
                           CConnectionCallback& aCallback);

        void SetRegistrationId(TUint32 aRegistrationId);        

        /**
		* Searches for a transaction matching the given aRequestId.
		* 
		* @param aRequestId Request id
		* @return Transaction object, or NULL if not found. Ownership isn't
        *   transferred.
		*/
        CSIPClientTransaction* FindTransaction(TUint32 aRequestId);

		/**
		* Change the registration binding state.
		*
		* @param aNewState State to enter, can be NULL.
		*		 		   Ownership is not transferred.		
		*/
        void ChangeState(const CRegistrationState* aNewState);

        void UpdateRefreshState(TUint aStatusCode) const;

        void ConnectionLost();

        CSIPClientTransaction* CreateClientTransactionL();

        TBool HandleError(CConnectionCallback& aCallback,
                          TInt aError,
                          TUint32 aRequestId,
                          const CRegistrationState& aUnregistered);

        /**
		* Informs CRegBindingImplementation that the associated CSIPConnection
		* has been deleted. After this the CSIPRegistrationBinding object can't
		* be used any more.
		*/
        void ConnectionDeleted();

		CSIPRegistrationBinding& Binding() const;
		
		void UpdateRegisteredContactL();
		
		void RemoveRegisteredContact();
		
		/**
		* Sets/resets properties.
		* @pre IsContextActive()==EFalse
		* @param aProperty. Property name
		* @param aValue. Property value
		* @leave KErrSIPInvalidRegistrationState if IsContextActive() is ETrue
		* @leave KErrNotFound if property is not found
		*/
		void SetPropertyL(TUint32 aProperty,TBool aValue);
		
	private: // Constructors, for internal use

		CRegBindingImplementation(CSIPRegistrationBinding& aRegistration,
								  CSIPConnection& aConnection);

        void ConstructL(CSIPToHeader* aAor,
						CSIPContactHeader* aContacts,                        
						CSIPRefresh* aRefresh,
						CSIPRouteHeader* aOutboundProxy,
						CUri8* aRemoteUri,
                        CSIPFromHeader* aFrom);

	private: // New functions, for internal use

        /**
		* Checks that CSIPConnection is available for use (not NULL). If
        * iConnection is NULL, it means user has deleted a resource needed by
        * CRegBindingImplementation, and this function leaves.
		*/
        void CheckConnectionL() const;

        /**
		* Returns the current registration state. If state handler is not
        * available, meaning user has deleted a resource needed by
        * CRegBindingImplementation, this function leaves.        
		*/
        const CRegistrationState& StateL() const;

		/**
		* Checks that aElements don't contain Contact or Expires headers.
        * @param aElements Message elements
        * @leave KErrSIPMalformedMessage If Contact or Expires header is
        *		 present in aElements.
		*/
        void CheckElementsL(const CSIPMessageElements* aElements) const;        

		/**
		* Removes, does not delete, Contact headers from aElements.
		* @pre aElements != NULL
        * @param aElements Message elements. Ownership is not transferred.        
		*/
        static void DetachContactHeader(TAny* aElements);

		/**
		* Removes, does not delete, the headers identified by aName, from
		* aElements.
		* @pre aElements != NULL
        * @param aElements Message elements. Ownership is not transferred.
        * @param aName Header name
		*/
        static void DetachHeaders(TAny* aElements, RStringF aName);        

	private: // Data

		//Registration binding who owns this CRegBindingImplementation instance
		CSIPRegistrationBinding& iRegistration;
		
        //The used connection. CRegBindingImplementation doesn't own it.
        //If this pointer is NULL; it means application has deleted the
        //connection before deleting the CRegBindingImplementation, and this
        //CRegBindingImplementation is now useless.
        CSIPConnection* iConnection;		

        //Outbound proxy or NULL if no proxy is specified. Owned.
        CSIPRouteHeader* iOutboundProxy;

        CSIPToHeader* iAor; //Owned.
        CSIPFromHeader* iFrom; //Owned, can be NULL.
        CSIPContactHeader* iContact; //Owned
        
        CSIPContactHeader* iRegisteredContact; //Owned, can be NULL

        CUri8* iRemoteUri; //Owned, can be NULL.


        //CRegBindingImplementation owns the CSIPRefresh object. It is not
        //deleted even the refresh ends since user can try to register again,
        //using the same CRegBindingImplementation (and CSIPRefresh) instance.
        //CRegBindingImplementation can have maximum of one refresh object
        CSIPRefresh* iRefresh;

        //Id received from SIP client        
        TUint32 iRegistrationId;        

		//Only client transactions can be bound to registration. Transactions
		//are not owned. Application owns them.
        RPointerArray<CSIPClientTransaction> iTransactions;

        //Current state of the registration. CRegBindingImplementation doesn't
        //own the state object. If iState is NULL, it means the CSIP owning the
        //registration states has been deleted.
        const CRegistrationState* iState;        

        TBool iSendWithExpires;
        TBool iCacheOutboundProxyIP;

	private: // For testing purposes
	
	    UNIT_TEST(CSIP_Test)
        UNIT_TEST(CSIPRegistrationBinding_Test)
        UNIT_TEST(CSIPInviteDialogAssoc_Test)
        UNIT_TEST(CSIPSubscribeDialogAssoc_Test)
        UNIT_TEST(CSIPReferDialogAssoc_Test)
        
		void __DbgTestInvariant() const;

	};

#endif
