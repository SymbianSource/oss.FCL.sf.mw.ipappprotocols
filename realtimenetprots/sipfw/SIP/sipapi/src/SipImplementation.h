/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : SipImplementation.h
* Part of     : SIP Client
* Version     : SIP /6.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPIMPLEMENTATION_H
#define CSIPIMPLEMENTATION_H

// INCLUDES
#include <e32base.h>
#include "_sipcodecdefs.h"
#include "sipclientobserver.h"
#include "transactionassociation.h"

// FORWARD DECLARATIONS
class CDesC8Array;
class MSIPObserver;
class CSIPClient;
class CSIPConnection;
class CSIPConnectionImplementation;
class CRegistrationState;
class CUnregistered;
class CRegistering;
class CRegistered;
class CUnregistering;
class CDialogState;
class CDialogTrying;
class CDialogEarly;
class CDialogConfirmed;
class CDialogTerminated;
class CSIPRequestElements;
class CPendingTransaction;
class CSIPClientTransaction;
class CSIPRefresh;
class CSIPHttpDigest;

// CLASS DECLARATION

/**
*  Implementation class for connecting to SIP server.
*/
class CSIPImplementation :
    public CBase,
    public MSIPClientObserver,
    public MTransactionAssociation
	{
    public: //Constructors and destructor
        
	    /**
        * Two-phased constructor.
		* @param aUid an unique id that identifies client
		* @param aObserver a observer for asynchronous event
        * @return New object, ownership is transferred
        */
		static CSIPImplementation* NewL(const TUid& aUid,
										MSIPObserver& aObserver);
	    
        /**
        * Destructor.               
        */
		~CSIPImplementation();

	public : //From MSIPClientObserver:
		/**
		* Request has been received from network.
        *
        * @param aIapId IAP-id for which the user should create a
        *   CSIPConnection
        * @param aElements Some SIP headers of the received request. Ownership
        *   is transferred.
        * @param aRequestId RequestId associated with the received request
		*/
		void IncomingRequestL(TUint32 aIapId,
        	                  CSIPRequestElements* aElements,
            	              TUint32 aRequestId);


        /**
	     * The transcation related to the received SIP request timed out and it
         * has been destroyed. This happens if the user fails to create a SIP
         * connection and does not send an appropriate SIP response.
         *
         * @param aRequestId RequestId of the request
	     */
    	void TimeOut(TUint32 aRequestId);

        void ChallengeReceivedL(
            TUint32 aRequestId,
            CSIPResponseElements* aResponse,
            MSIPHttpDigestChallengeObserver2& aObserver);

        void ChallengeReceivedInRefreshL(
            TUint32 aRefreshId,
            TUint32 aRequestId,
            CSIPResponseElements* aResponse,
            MSIPHttpDigestChallengeObserver2& aObserver);

    public: //From MTransactionAssociation:        
        
        CSIPClientConnection& ClientConnectionL();
        
        CSIPClientConnection* ClientConnection();

        CSIPConnection& SIPConnectionL();

        void AddTransactionL(CSIPTransactionBase& aTransaction);

        void RemoveTransaction(const CSIPTransactionBase& aTransaction);

    public: // New functions

		/**
		* Gets all supported security mechanisms
		* @return supported SIP security mechanisms names (e.g. digest);
        *   the ownership is transferred
		*/
		CDesC8Array* SupportedSecurityMechanismsL() const;

		/**
		* Checks if the signalling compression (SigComp) is supported.
        * Leaves on failure with a system wide error code.
		* @return ETrue if supported, EFalse otherwise
		*/
		TBool IsSigCompSupportedL() const;

		/**
		* Gets negotiated security mechanism with the next hop
		* on the signaling path.
		* @param aHop the next hop's FQDN or numeric IP address
		* @return negotiated security mechanism or zero-pointer in case no
		*		  security mechanism was negotiated with the next hop;
		*		  the ownership is transferred
		*/
		HBufC8* NegotiatedSecurityMechanismL(const TDesC8& aHop) const;

		/**
		* Checks if the user has a connection created for given IAP-id
		* @param aIapId IAP-id
 		* @return sip connection or 0-pointer if connection does not exist.
        *   Ownership is not transferred.
		*/
		CSIPConnection* Connection(TUint32 aIapId) const;

        /*
        * By default the SIP built-in security mechanisms are enabled.
        * @param aEnabled if set to EFalse the build-in SIP security handling 
        *        mechanism is disabled.
        */
        void SetSecurityHandlingL(TBool aEnabled);

        /*
        * Tests if SIP security handling mechanism is enabled/disabled
        * @return ETrue if built-in mechanisms are enabled, EFalse otherwise.
        */
        TBool IsSecurityHandlingEnabled() const;

        /*
        * Create a client transaction for handling a received challenge.
        * @param aRequestId Request id
        * @param aResponse Response elements, ownership is transferred
        * @param aRefresh Refresh to which the transaction will be associated,
        *				  or NULL if no refresh used.
        *				  Ownership is not transferred.
        */
		CSIPClientTransaction*
			CreateTransactionL(TUint32 aRequestId,
							   CSIPResponseElements* aResponse,
							   CSIPRefresh* aRefresh);

		/**
		* Ignore each challenge present in the response in the transaction.
        * @param aTransaction Client transaction
        * @param aDigest HTTP digest
		*/
        void IgnoreChallenges(const CSIPClientTransaction& aTransaction,
        					  CSIPHttpDigest& aDigest);

        /**
		* Obtains the used CSIPClient instance
        *
 		* @return SIPClient
		*/
        CSIPClient& SIPClient();

        /**
		* Adds a new connection into CSIPImplementation.
        *        
		* @param aConnImplementation Implementation of the connection to add
        * @param aIapId IAP-id        
		*/
        void AddConnectionL(CSIPConnectionImplementation& aConnImplementation,
        					TUint32 aIapId);

        /**
		* Detach a connection from CSIPImplementation
        *
		* @param aConnection Connection to remove
		*/
        void RemoveConnection(const CSIPConnection& aConnection);

		/**
		* Attach a CSIPHttpDigest to CSIPImplementation
        *        
		* @param aDigest CSIPHttpDigest object to attach
		*/
        void AddDigestL(CSIPHttpDigest& aDigest);

		/**
		* Detach a CSIPHttpDigest from CSIPImplementation
        *
		* @param aDigest CSIPHttpDigest object to remove
		*/
		void RemoveDigest(const CSIPHttpDigest& aDigest);

        /**
		* Obtains the initial state for CSIPRegistrations
        *
		* @return Initial state of registration state machine. Ownership isn't
        *   transferred.
		*/
        CRegistrationState* InitialRegisterState();
        
        /**
		* Obtains the initial state for CSIPDialog
        *
		* @return Initial state of dialog state machine. Ownership isn't
        *   transferred.
		*/
        CDialogState* InitialDialogState();

    private: // Constructors
        CSIPImplementation(MSIPObserver& aObserver);

        void ConstructL(const TUid& aUid);

	private: // Data
        //Application's UID
        TUid iUid;
        
        TBool iSecurityHandlingEnabled;

        //Callback to the application
        MSIPObserver& iObserver;

        //SIP client
        CSIPClient* iSIPClient;

        //Existing connections, CSIPImplementation doesn't own them.
        RPointerArray<CSIPConnection> iConnections;

        //When a request comes from network with an IAP-id for which there is
        //no CSIPConnection instance, the created ServerTransaction is stored
        //here. In case TimeOut occurs, CSIPImplementation can give the pointer
        //to the associated ServerTransaction to application.
        //When application creates a CSIPConnection, all ServerTransactions
        //which have the same IAP-id are removed from this array.
        RPointerArray<CPendingTransaction> iPendingServerTransactions;

		//CSIPHttpDigest objects are not owned
		RPointerArray<CSIPHttpDigest> iDigests;

        //Registration states. These are owned by CSIPImplementation and are
        //shared by all CSIPRegistrationBinding instances.
        CUnregistered* iUnregistered;
        CRegistering* iRegistering;
        CRegistered* iRegistered;
        CUnregistering* iUnregistering;        

        //Dialog states. These are owned by CSIPImplementation and are shared
        //by all CSIPDialog instances. There is no proceeding state.
        CDialogTrying* iDialogTrying;        
        CDialogEarly* iDialogEarly;
        CDialogConfirmed* iDialogConfirmed;
        CDialogTerminated* iDialogTerminated;		
	
	private: // For testing purposes
	    UNIT_TEST(CSIP_Test)
        UNIT_TEST(CSIPRegistrationBinding_Test)
        UNIT_TEST(CSIPInviteDialogAssoc_Test)
        UNIT_TEST(CSIPSubscribeDialogAssoc_Test)
        UNIT_TEST(CSIPReferDialogAssoc_Test)
        
		void __DbgTestInvariant() const;

		};

#endif
