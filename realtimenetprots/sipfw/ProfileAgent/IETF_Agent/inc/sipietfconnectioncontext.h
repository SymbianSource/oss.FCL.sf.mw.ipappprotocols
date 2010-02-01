/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipietfconnectioncontext.h
* Part of     : sip ietf agent
* Interface   : 
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef	CSIPIETFCONNECTIONCONTEXT_H
#define CSIPIETFCONNECTIONCONTEXT_H

//  INCLUDES
#include <e32base.h>
#include <bamdesca.h>
#include "sipconnectionobserver.h"
#include "MSIPProxyResolverObserver.h"

// FORWARD DECLARATIONS
class CSIPIetfProfileContext;
class CSIPRegistrationBinding;
class CSIPClientTransaction;
class CSIPDialogAssocBase;
class CSIPProxyResolver;
class CSIPHttpDigest;
class MSIPHttpDigestChallengeObserver2;
class CSIPConcreteProfile;
class CSIP;
class MSIPObserver;

// CLASS DECLARATION

/**
* Class maintains associations between the SIP connection
* and related SIP profiles. 
* 
*/
class CSIPIetfConnectionContext: public CBase, public MSIPConnectionObserver,
	public MSIPProxyResolverObserver
	{
	public: // Constructors and destructor
		   
        /**
        * Two-phased constructor.
        */
		static CSIPIetfConnectionContext* NewL();

        /**
        * Two-phased constructor.
        */
		static CSIPIetfConnectionContext* NewLC();

        /**
        * Destructor.
        */
		~CSIPIetfConnectionContext();

	public://new functions

	    /**
	    * Gets connection, the ownership is not transferred
	    * @return connection
	    */
		CSIPConnection* Connection();

	    /**
	    * Add profile context, ownership transferred
	    * @param aContext, profile context using the connection
	    */
		void AddProfileContextL(CSIPIetfProfileContext* aContext);

	    /**
	    * Find context of the profile, the ownership is not transferred
	    * @param aProfileId, profile stored in context
	    */
		CSIPIetfProfileContext* FindContext(TUint32 aProfileId);

	    /**
	    * Delete idle contexts
	    */
		void CleanIdleContexts();

	    /**
	    * Set connection for the context
	    * @param aConnection, connection of the context
	    */
		void SetConnection(CSIPConnection* aConnection);

	    /**
	    * Checks if connection idle
	    * @return ETrue, if contains no profile contexts, otherwise EFalse
	    */
		TBool IsIdle() const;

		/**
	    * starts resolving proxy
	    */
		void ResolveL();

		/**
		* Cleans dynamic proxy addesses and cancels any
		* pending requests
		*/
		void CleanProxyResolving();
		
		/**
		* Checks if registration/deregistration should be put
		* in que to wait, because there is registration/deregistration
		* with same registrar address pending.
		* @return ETrue if should be put into que to wait.
		*/
		TBool AddIntoQueue( const TDesC8& aValue );

		/**
		* Sets HTTP Digest credentials from the profile
		* if the given transaction was found from one 
		* of the profiles related to this connection.
		* @return ETrue if the transaction was found and credentials were set.
		*/
        TBool SetCredentials(
            const CSIPClientTransaction& aTransaction,
            CSIPHttpDigest& aDigest);

		/**
		* Sets HTTP Digest credentials from the profile
		* if the given refresh was found from one 
		* of the profiles related to this connection.
		* @return ETrue if the refresh was found and credentials were set.
		*/
        TBool SetCredentials(
            const CSIPRefresh& aRefresh,
            CSIPHttpDigest& aDigest);
        
        /**
        * Checks if this connection is used by given profile.
        * @param aProfile
        * @return ETrue if connection is used by profile
        */    
        TBool ConnectionUser( CSIPConcreteProfile& aProfile );
        
        /**
        * Attaches conection user.
        * @pre connection must be set, otherwise leaves with KErrNotReady
        * @param aProfile
        */
        void SetConnectionUserL( CSIPConcreteProfile& aProfile );
        
        /**
        * Creates CSIP and CSIPHttpDigest used for this connection
        * @param aSipObserver, SIP observer
        * @param aSipHttpDigestObserver, SIP HTTP Digest challenge observer
        */
        CSIP& CreateSipL( 
            MSIPObserver& aSipObserver,
            MSIPHttpDigestChallengeObserver2& aSipHttpDigestObserver );

	public://functions from base class

		/**
		* A SIP request outside a dialog has been received from the network.
		* @param aTransaction SIP server transaction
		* The ownership is transferred.
        */
		void IncomingRequest(CSIPServerTransaction* aTransaction);

		/**
		* A SIP request within a dialog has been received from the network.
		* The client must resolve the actual dialog association to which
		* this request belongs.
		* @param aTransaction SIP server transaction
		* The ownership is transferred.
		* @param aDialog the dialog  that
		*        this transaction belongs to.
 		*/
		void IncomingRequest(
            CSIPServerTransaction* aTransaction,
            CSIPDialog& aDialog);

		/**
		* A SIP response received from the network.
		* @param aTransaction contains response elements.
		* The ownership is transferred.
		*/
		void IncomingResponse(CSIPClientTransaction& aTransaction);

		/**
		* A SIP response that is within a dialog association or creates
		* a dialog association.
		* @param aTransaction contains response elements.
		* The ownership is transferred.
		* @param aDialogAssoc a dialog association
		*/
		void IncomingResponse(
            CSIPClientTransaction& aTransaction,
        	CSIPDialogAssocBase& aDialogAssoc);

		/**
		* A SIP response creating a registration binding or an error response 
		* that is related to an refreshed registration binding  
		* has been received from the network.
		* @param aTransaction contains response elements.
		* The ownership is transferred.
		* @param aSIPRegistration associated registration
		*/
		void IncomingResponse(
        	CSIPClientTransaction& aTransaction,
        	CSIPRegistrationBinding& aRegistration);

		/**
		* A SIP response related a registration binding or an error response
		* that is related to registration binding has been received
        * from the network.
        *
		* @param aTransaction contains response elements.
		* @param aRegistration registration this transaction belongs to
		*/
		void IncomingResponse(
        	CSIPClientTransaction& aTransaction,
            CSIPInviteDialogAssoc* aDialogAssoc);

		/**
		* An asynchronous error has occured related to a periodical refresh 
		* that relates to a registration.
		* @param aError error code
		* @param aSIPRegistration associated registration
		*/
		void ErrorOccured(
            TInt aError,
            CSIPRegistrationBinding& aRegistration);
		/**
		* An asynchronous error has occurred in the stack related
		* to the request indicated by the given transaction.
		* @param aError error code
		* @param aTransaction the failed transaction
		* @param aRegistration the failed registration
		*/
		void ErrorOccured(
            TInt aError,
            CSIPClientTransaction& aTransaction,
            CSIPRegistrationBinding& aRegistration);

		/**
		* An asynchronous error has occurred in the stack related to the
		* request indicated by the given transaction.
		* @param aError error code
		* @param aTransaction failed transaction
		*/
		void ErrorOccured (
            TInt aError,
        	CSIPTransactionBase& aTransaction);
	
		/**
		* An asynchronous error has occured related to a request within
		* an existing dialog.
		* @param aError error code
		* @param aTransaction the failed transaction
		* @param aDialogAssoc the failed dialog associoation
		*/
		void ErrorOccured(
        	TInt aError,
            CSIPTransactionBase& aTransaction,
            CSIPDialogAssocBase& aDialogAssoc);

		/**
		* An asynchronous error has occured related to a refresh 
		* @param aError error code
		* @param aSIPRefresh original refresh object
		*/
		void ErrorOccured (
        	TInt aError,
        	CSIPRefresh& aRefresh);

		/**
		* An asynchronous error has occured related to a periodical refresh 
		* that belongs to SIP dialog association.
		* @param aError error code
		* @param aDialogAssoc SIP dialog association
		*/
		void ErrorOccured(
            TInt aError,
            CSIPDialogAssocBase& aDialogAssoc);

		/**
		* Connection state has changed.
		* @param aState indicates the current connection state
		*/
		void ConnectionStateChanged(CSIPConnection::TState aState);

        /**
		* SIP stack has completed UAC core INVITE transaction 64*T1 seconds
        * after the reception of the first 2xx response. No more 2xx responses
        * can be received to the issued single INVITE.
        *
		* @param aTransaction a complete UAC core INVITE transaction
		*/
        void InviteCompleted(CSIPClientTransaction& aTransaction);
        
        /**
        * Invite was canceled with the CANCEL
        * @param aTransaction a canceled INVITE UAS transaction
        */
        void InviteCanceled(CSIPServerTransaction& aTransaction);


		/**
		* An asynchronous proxy resolving request has been completed.
		* @param aRequestId the id of the resolving request.
		* @param aProxies a list of proxy addresses.
		*        The ownership is transferred.
		*/
		void ProxyResolvingRequestComplete(
		    TUint aRequestId, 
		    MDesC8Array* aProxies);

		/**
		* An asynchronous proxy resolving request has failed.
		* @param aRequestId the id of the resolving request.
		* @param aError a reason why the request failed.
		*/
		void ProxyResolvingRequestFailed(TUint aRequestId, TInt aError);

		/**
	    * starts resolving proxy
	    */
		void ResolveProxyL();

	private:
		/**
		* Default constructor.
		*/
		CSIPIetfConnectionContext();

		/**
		* By default Symbian 2nd phase constructor is private.
		*/
		void ConstructL();

    private:
    
        void SetConnectionParameters();

	private: // Data
	
		CSIPConnection* iConnection;

		CSIPProxyResolver* iProxyResolver;
		TUint iProxyResolveRequestId;

		RPointerArray<CSIPIetfProfileContext> iContexts;
		
		RPointerArray<CSIPConcreteProfile> iUsers;
		
		CSIP* iSip;
		CSIPHttpDigest* iSipHttpDigest;
		
		TBool iConnectionParamsSet;
	private: //Unit test classes are declared as friend
#ifdef CPPUNIT_TEST
		friend class CSIPIetfProfileContextTest;
		friend class CSIPIetfConnectionContextTest;
#endif
	};

#endif // CSIPIETFCONNECTIONCONTEXT_H
