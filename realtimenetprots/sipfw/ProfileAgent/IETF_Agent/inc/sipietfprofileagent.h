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
* Name        : sipietfprofileagent.h
* Part of     : sip ietf1 agent
* Interface   : private
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPIETFPROFILEAGENT_H
#define CSIPIETFPROFILEAGENT_H

//  INCLUDES
#include <e32base.h>
#include "sipprofileagent.h"
#include "sipprofileagentobserver.h"
#include "sipprofilefsmuser.h"
#include "sipobserver.h"
#include "siphttpdigestchallengeobserver2.h"
#include <sipprofileagentextensionparams.h>


//FORWARD DECLARATIONS
class CSIP;
class CSIPHttpDigest;
class TSIPProfileTypeInfo;
class CSIPIetfConnectionContext;
class CSIPPrflInitState;
class CSIPPrflResolvingProxiesState;
class CSIPPrflRegisterRequestedState;
class CSIPPrflRegistrationInProgressState;
class CSIPPrflRegisteredState;
class CSIPPrflDeregisterRequestedState;
class CSIPPrflDeregistrationInProgressState;
class CSIPPrflRegisterDelayRequestedState;
class CSIPIetfProfileContext;
class CSIPProfileQueueHandling;

// CLASS DECLARATION
/**
*  Registers profiles and maintains
*  profile registrations as per RFC3261.
*  
*  @lib n/a
*/
class CSIPIetfProfileAgent: 
    public CSIPProfileAgent, 
	public MSIPProfileAgentObserver,
	public MSIPProfileFSMUser,
	public MSIPObserver,
	public MSIPHttpDigestChallengeObserver2
	{
	public: // Constructors and destructor
		   
        /**
        * Two-phased constructor.
	    * @param aInitParams parameters passed in the construction
        */
        static CSIPIetfProfileAgent* NewL(TAny* aInitParams);
	
        /**
        * Destructor.
        */
        virtual ~CSIPIetfProfileAgent();

	public: // From CSIPProfileAgent

        const TSIPProfileTypeInfo& Type() const;
        
		CSIPConcreteProfile* CreateL();
	
	    void RegisterL(
			CSIPConcreteProfile& aSIPConcreteProfile);

		void UpdateL(
			CSIPConcreteProfile& aNewProfile, 
			CSIPConcreteProfile& aOldProfile);

	    void DeregisterL(
			CSIPConcreteProfile& aSIPConcreteProfile);

		TInt GetProfileState(
		    CSIPConcreteProfile::TStatus& aState, 
			CSIPConcreteProfile& aProfile) const;

	    TBool IsIdle();
		
        TBool RegisterPending(CSIPConcreteProfile& aSIPProfile) const;

		TInt TerminateHandling(CSIPConcreteProfile& aProfile);

        TInt Extension(TInt aOperationCode, TAny* aParams); 

    public: // From MSIPProfileAgentObserver
    
		void SIPProfileStatusEvent(
		    CSIPConcreteProfile& aProfile,
		    TUint32 aContextId);

		void SIPProfileErrorEvent(
		    CSIPConcreteProfile& aProfile,
		    TInt aError);

		TBool ProceedRegistration(CSIPConcreteProfile& aProfile, TInt aError);
		
		void GetFailedProfilesL( 
		    const TSIPProfileTypeInfo& aType,
		    RPointerArray<CSIPConcreteProfile>& aFailedProfiles) const;    


    public: // From MSIPProfileFSMUser

        void RegisterProfileL(
            CSIPConcreteProfile& aSIPConcreteProfile);

        void DeregisterProfileL(
            CSIPConcreteProfile& aSIPConcreteProfile);

        void RetryProfileRegistrationL( 
            CSIPConcreteProfile& aSIPProfile );

        TBool AddProfileIntoQueue( 
            CSIPConcreteProfile& aSIPConcreteProfile ) const;

        void RegisterProfileAfterQueueL( 
            CSIPConcreteProfile& aSIPConcreteProfile );

        void DeregisterProfileAfterQueueL( 
            CSIPConcreteProfile& aSIPConcreteProfile );

        void RetryProfileRegistrationAfterQueueL( 
            CSIPConcreteProfile& aSIPConcreteProfile );
            
        TBool IsInQueue( 
            CSIPConcreteProfile& aSIPProfile ) const;

		void SetInterimProfile( CSIPConcreteProfile* 
        									aSIPConcreteProfile );

    public: // From MSIPObserver

        void IncomingRequest(
            TUint32 aIapId,
            CSIPServerTransaction* aTransaction);

        void TimedOut(CSIPServerTransaction& aTransaction);

    public: // From MSIPHttpDigestChallengeObserver2
    
        void ChallengeReceived(const CSIPClientTransaction& aTransaction);

        void ChallengeReceived(const CSIPRefresh& aRefresh);


	private: // Constructors

	    /**
		* C++ default constructor.
		*/
		CSIPIetfProfileAgent(TSIPProfileAgentInitParams* aInitParams);

		/**
		* By default Symbian 2nd phase constructor is private.
		*/
		void ConstructL();

		
    private: // New functions		

		/**
		* Provides profile context
		* @param aProfile a profile
		* @return a profile context
		*/
		CSIPIetfProfileContext& ProvideProfileContextL(CSIPConcreteProfile& aProfile);

		/**
		* Finds profile context
		* @param aSIPConcreteProfile a profile
		* @return profile context, the ownership is not transferred
		*/
		CSIPIetfProfileContext* FindProfileContext(CSIPConcreteProfile& aSIPConcreteProfile) const;

		/**
		* Finds profile context
		* @param aProfileId identifier of the profile
		* @return profile context, the ownership is not transferred
		*/
		CSIPIetfProfileContext* FindProfileContext(TUint32 aProfileId) const;

		/**
		* Finds connection context
		* @param aPriofile a profile
		* @return a connection context
		*/
		CSIPIetfConnectionContext* FindConnectionContext(CSIPConcreteProfile& aProfile) const;
		
		/**
		* Finds connection context
		* @param aIapId a iap id
		* @return a connection context
		*/
		CSIPIetfConnectionContext* FindConnectionContext( TUint32 aIapId ) const;
	
		/**
		* Destroys idle connection contexts
		*/
		void CleanIdleConnectionContexts();
				
		TBool DeregisterToWaitingQueueL( CSIPIetfProfileContext* aContext );
		
		
		void RegisterQueue( CSIPConcreteProfile& aProfile, TBool aReportError );
    
		TBool AllowedTakeFromQueue( CSIPConcreteProfile& aSIPConcreteProfile );
	
	
	private: //  Data

		CSIP*									iSIP;
		CSIPHttpDigest*                         iHttpDigest;
		MSIPProfileAgentObserver&				iSIPProfileAgentObserver;
		CDeltaTimer&							iDeltaTimer;
		TSIPProfileTypeInfo						iType;
		CSIPPrflInitState*						iInit;
		CSIPPrflResolvingProxiesState*			iResolvingProxies;
		CSIPPrflRegisterRequestedState*			iRegRequested;
		CSIPPrflRegistrationInProgressState*	iRegInProgress;
		CSIPPrflRegisteredState*				iRegistered;
		CSIPPrflDeregisterRequestedState*		iDeregRequested;
		CSIPPrflDeregistrationInProgressState*	iDeregInProgress;
		CSIPPrflRegisterDelayRequestedState*    iRegDelayRequested;
		RPointerArray<CSIPIetfConnectionContext> iConnectionCtxArray;
		CSIPProfileQueueHandling* 				iProfileQueueHandling;
		CSipProfileAgentConfigExtension*		iConfigExtension;//not owned


#ifdef CPPUNIT_TEST	
	    friend class CSIPIetfProfileAgentTest;
#endif
	};

#endif CSIPIETFPROFILEAGENT_H
