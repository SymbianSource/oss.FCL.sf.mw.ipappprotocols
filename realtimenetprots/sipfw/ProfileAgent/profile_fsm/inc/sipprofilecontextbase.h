/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipprofilecontextbase.h
* Part of     : sip profile fsm
* Interface   : internal interface
* Version     : %version: 2.1.1 %
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPRROFILECONTEXTBASE_H
#define CSIPPRROFILECONTEXTBASE_H

//INCLUDES 
#include <e32base.h>
#include <badesca.h>
#include "sipconnection.h"
#include "sipclienttransaction.h"
#include "sipregistrationbinding.h"
#include "sipprofilecontext.h"
#include "sipprofileagent.h"

// FORWARD DECLARATIONS
class CSIP;
class CSIPHttpDigest;

// CLASS DECLARATION
/**
*  A base class for maintaining the dynamic information related
*  to a particular SIP profile. Class stores the related registration, 
*  sip connection and transactions.
*
*  @lib sipprofilefsm.lib
*/
class CSIPProfileContextBase : public CBase, public MSIPProfileContext
	{
	public:

        /**
        * Destructor.
        */
		IMPORT_C virtual ~CSIPProfileContextBase();


    public: // From MSIPProfileContext
    
        IMPORT_C const CSIPConnection& Connection() const; 
        
        IMPORT_C CSIPRegistrationBinding* Registration();
        
        IMPORT_C void SetNextState(CSIPPrflStateBase& aState);

        IMPORT_C void ForgetProfile();

        IMPORT_C void SetTransaction(CSIPClientTransaction* aTransaction);

        IMPORT_C CSIPConcreteProfile* Profile();

        IMPORT_C virtual MSIPProfileAgentObserver& AgentObserver() const;

        IMPORT_C void SetProfile(CSIPConcreteProfile* aProfile);
        
        IMPORT_C TInt SetRegisteredAORs();	
        
        IMPORT_C void RemoveDeltaTimerEntry();
        
        IMPORT_C void HandleProxyResolvingError(TInt aError);
        
        IMPORT_C TBool IsProxyResolvingEnabled() const;
        
        IMPORT_C TBool ProxiesAlreadyResolved();
        
        IMPORT_C void SetProxiesL(MDesC8Array& aProxies);
       
		IMPORT_C void SetNegotiatedSecurityMechanismL();
		
		IMPORT_C void ResetNegotiatedSecurityMechanismL();       
        
	public: // New functions

		/**
		* Gets the current SIP Profile FSM state
		* @return current state
		*/
		IMPORT_C TSIPProfileState CurrentState() const;

		/**
		* Gets the current state mapped to the state of CSIPConcreteProfile  
		* @return the current mapped state
		*/
        IMPORT_C CSIPConcreteProfile::TStatus CurrentMappedState() const;

	    /**
	    * Registers SIP profile to the network and maintains
	    * the registration. Takes actions needed for the registration
        * according to the profile type.
	    */
	    IMPORT_C void RegisterL();

        /**
        * Checks whether a registration is pending for the profile.
        * In other words if a REGISTER has been sent 
        * but a final response has not yet been received.
        */ 		
        IMPORT_C TBool IsRegisterPending() const;

		/**
		* Retries registration with next proxy address if needed
		*/
		IMPORT_C void RetryRegistration();

		/**
		* Sets resolved proxy addresses
		* @param aProxies array of dynamicly resolved proxies
		*/
		IMPORT_C void ProxyResolvingRequestCompleteL(MDesC8Array& aProxies);

		/**
		* An asynchronous proxy resolving request has failed.
		* @param aError a reason why the request failed.
		*/
		IMPORT_C void ProxyResolvingRequestFailed(TInt aError);

		/**
		* Checks if error event is ignored and not sent to client
		* @return ETrue if error can ignored
		*/
		IMPORT_C TBool IgnoreErrorEvent();
		
		/**
		* Terminates abnormally profile handling i.e.
		* cleanups reserved resources for the profile and forgets it.
		*/
		IMPORT_C void TerminateHandling();		

		/**
		* An asynchronous error has occured related to a periodical refresh 
		* that relates to a registration.
		* @param aError error code
		* @param aRegistration associated registration
		* @param aHandled, returned ETrue if response handled
		*/
		IMPORT_C void ErrorOccured(TInt aError,
		                           CSIPRegistrationBinding& aRegistration,
		                           TBool& aHandled);

		/**
		* An asynchronous error has occurred in the stack related
		* to the request indicated by the given transaction.
		* @param aError error code
		* @param aTransaction the failed transaction
		* @param aRegistration the failed registration
		* @param aHandled, returned ETrue if response handled
		*/
		IMPORT_C void ErrorOccured(TInt aError,
		                           CSIPClientTransaction& aTransaction,
		                           CSIPRegistrationBinding& aRegistration,
		                           TBool& aHandled);

		/**
		* Connection state has changed.
		* @param aState indicates the current connection state
		*/
		IMPORT_C void ConnectionStateChanged(CSIPConnection::TState aState);

		IMPORT_C void RetryDeltaTimer( TUint aTime, TInt aError );
		
		/**
		* Checks if profile has not received final response to 
		* registration/deregistration and the registrar has same value
		* as input value aValue
		* @return ETrue if conditions are met.
		*/
		IMPORT_C TBool AddIntoQueue( const TDesC8& aValue );
		
		/**
		* Checks if profile is in state when register has not yet been sent
		* @return ETrue if conditions are met.
		*/
		IMPORT_C TBool AddDeregisterIntoQueue();
		
		/**
		* Checks if the profile  is in idle state.
		* @return ETrue if conditions are met.
		*/
		IMPORT_C TBool IsIdle();

		/**
		* Sets HTTP Digest credentials from the profile
		* if the given transaction is related to this profile.
		* @return ETrue if the transaction was found and credentials were set.
		*/
        IMPORT_C TBool SetCredentials(
            const CSIPClientTransaction& aTransaction,
            CSIPHttpDigest& aDigest);

		/**
		* Sets HTTP Digest credentials from the profile
		* if the given refresh is related to this profile.
		* @return ETrue if the refresh was found and credentials were set.
		*/
        IMPORT_C TBool SetCredentials(
            const CSIPRefresh& aRefresh,
            CSIPHttpDigest& aDigest);

    public: // For internal use

		static TInt RetryDeltaTimerExpired(TAny* aPtr);
		
		void ContinueRegistrationL();

	protected: // Constructors
	
		/**
		* Constructor
		*/
		IMPORT_C CSIPProfileContextBase(
			CSIP& aSIP,
			CSIPConnection& iConnection,
			MSIPProfileAgentObserver& aObserver,
			CSIPPrflStateBase& aInitState,
			CSIPConcreteProfile& aProfile,
			CDeltaTimer& aDeltaTimer);
    
    protected: // Virtual functions

    	IMPORT_C virtual TBool HasTransaction(
    	    const CSIPClientTransaction& aTransaction) const;
    
    	IMPORT_C virtual TBool HasRefresh(
    	    const CSIPRefresh& aRefresh) const;
    	    
    	IMPORT_C virtual const TDesC8& RegistrarUsername() const;

		virtual void ConnectionStateChangedImpl(
		    CSIPConnection::TState aState) = 0;
		    
		virtual void SetRegisteredAORsL() = 0;

		virtual TBool RetryRegister(CSIPClientTransaction* aTransaction,
							        TInt aError) = 0;
							  
		virtual TBool ShouldRetryRegistration(TInt aError) = 0;					  
    	
    	virtual void InitializeRetryTimerValue() = 0;
    
    protected: // New functions
    
 		/**
		* Require dynamic proxy resolving
		* @return ETrue, if required
		*/
		IMPORT_C TBool RequireProxyResolving();
    
		/**
		* Returns either static or dynamic proxy address
		* @return proxy address
		*/
		IMPORT_C const TDesC8& ProxyAddressL();

		/**
		* Returns either static or dynamic proxy address
		* @return proxy address
		*/
		IMPORT_C const TDesC8& ProxyAddressL(CSIPConcreteProfile& aProfile);
		
		/**
		* A 2XX response to a REGISTER request was received.
		*/
		IMPORT_C void Received2XXRegisterResponse();
		
		/**
		* Checks if retry of registration is possible
		* @return ETrue if retry possible
		*/
		IMPORT_C TBool RetryPossible(TInt aError);		
				
		/**
		* Set flag to indicate whether retry of registration is possible
		* @param aRetryPossible ETrue if retry possible
		*/
		IMPORT_C void SetRetryPossible(TBool aRetryPossible);
		
		/**
		 * Returns profile ID
		 * @return profile ID or 0 if profile doesn't exist
		 */
		IMPORT_C TUint32 ProfileId() const;
		
    private: // New functions

        void RegisterDynamicL(MDesC8Array& aProxies);

		void ConstructDynamicProxyL(const TDesC8& aProxy);
		
		void ClearProxyResolving();
		
		void ResetRegisteredAORsL();				
 
        template<class T> void SetCredentialsImplementation(
            const T& aChallengeTarget,
            const CSIPResponseElements& aResponse,
            CSIPHttpDigest& aDigest);
 
        TBool GetRegistrarCredentials(
            TPtrC8& aUsername,
            TPtrC8& aPassword) const;
 
         TBool GetProxyCredentials(
            TPtrC8& aProxyHost,
            TPtrC8& aUsername,
            TPtrC8& aPassword) const;
 
        TPtrC8 ExtractProxyHost() const;


    protected: // Data

		CSIP&						iSIP;
		CSIPConnection&             iConnection;
		MSIPProfileAgentObserver&	iObserver;
		CSIPPrflStateBase*			iCurrentState; // Not owned
		CSIPPrflStateBase*			iInitState; // Not owned
		CSIPConcreteProfile*		iProfile; // Not owned
		CSIPClientTransaction*		iClientTx; // Owned
		CSIPRegistrationBinding*	iRegistration; // Owned
		TUint32						iProfileId;
		CDeltaTimer&				iDeltaTimer;
		MDesC8Array*				iProxies; // Owned
		TInt                        iErrorForDeltaTimer;

	private:
	
		TInt						iDynamicProxyCount;
		TCallBack					iDeltaTimerCallBack;
		TDeltaTimerEntry			iDeltaTimerEntry;
		TBool					    iRetryPossible;
		TBool						iResolvingCompleted;
        RStringF                    iWwwAuthenticateHeaderName;
        RStringF                    iProxyAuthenticateHeaderName;
        TBool 						iUseBackupProxy;
		
	private: // For testing purposes
	
        #ifdef CPPUNIT_TEST	
	    	friend class CSIPProfileContextBaseTest;
        #endif
	};

#endif // CSIPPRROFILECONTEXTBASE_H

