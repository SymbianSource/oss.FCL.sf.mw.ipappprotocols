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
* Name        : sipimsprofileagent.h
* Part of     : sip ims agent
* Interface   : private
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPIMSPROFILEAGENT_H
#define CSIPIMSPROFILEAGENT_H

//  INCLUDES
#include <e32base.h>
#include "sipprofileagent.h"
#include "sipprofileagentobserver.h"
#include "sipprofilefsmuser.h"
#include "Sipimsprofileusimprovider.h"
#include "msipprofileimsauthenticationdataobserver.h"
#include "sipobserver.h"
#include "siphttpdigestchallengeobserver2.h"
#include <sipprofileagentextensionparams.h>

//FORWARD DECLARATIONS
class CSIPConcreteProfile;
class CSIP;
class TSIPProfileTypeInfo;
class CSIPIMSConnectionContext;
class CSIPPrflInitState;
class CSIPPrflResolvingProxiesState;
class CSIPPrflRegisterRequestedState;
class CSIPPrflRegisterDelayRequestedState;
class CSIPPrflRegistrationInProgressState;
class CSIPPrflRegisteredState;
class CSIPPrflDeregisterRequestedState;
class CSIPPrflDeregistrationInProgressState;
class CSIPIMSProfileContext;
class CDeltaTimer;
class CSIPNotifyXmlBodyParser;
class CSIPProfileSIMRecord;
class CSIPProfileQueueHandling;
class CSIPHttpDigest;

// CLASS DECLARATION
/**
*  Registers profiles and maintains
*  profile registrations as per RFC3261.
*  
*  @lib n/a
*/
class CSIPIMSProfileAgent: 
    public CSIPProfileAgent, 
	public MSIPProfileAgentObserver,
	public MSIPProfileFSMUser, 
	public MSIPProfileIMSAuthenticationDataObserver,
	public MSIPObserver,
	public MSIPHttpDigestChallengeObserver2	
	{
	public: // Constructors and destructor
		   
        /**
        * Two-phased constructor.
	    * @param aInitParams parameters passed in the construction
        */
        static CSIPIMSProfileAgent* NewL(TAny* aInitParams);
	
        /**
        * Destructor.
        */
        virtual ~CSIPIMSProfileAgent();

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
            
        void SetInterimProfile( 
            CSIPConcreteProfile* aSIPConcreteProfile ); 
		
	public: // From MSIPProfileIMSAuthenticationDataObserver
		
        void AuthorizedL();

        void AuthorizationFailed();
			
        void AuthorizationSIMFailed();
				
		void AsyncDeltaTimer();
		
		void UpdateRegistrationsL();
			
    	// A callback for CDeltaTimer
        static TInt ASyncTimerExpired(TAny* aPtr);
		
    public: // From MSIPObserver

        void IncomingRequest(
            TUint32 aIapId,
            CSIPServerTransaction* aTransaction);

        void TimedOut(CSIPServerTransaction& aTransaction);

    public: // From MSIPHttpDigestChallengeObserver2
    
        void ChallengeReceived(const CSIPClientTransaction& aTransaction);

        void ChallengeReceived(const CSIPRefresh& aRefresh);

	private:

	    /**
		* C++ default constructor.
		*/
		CSIPIMSProfileAgent(TSIPProfileAgentInitParams* aInitParams);

		/**
		* Provides profile context
		* @param aProfile a profile
		* @return a profile context
		*/
		CSIPIMSProfileContext& ProvideProfileContextL(CSIPConcreteProfile& aProfile);

		/**
		* Finds profile context
		* @param aSIPConcreteProfile a profile
		* @return profile context, the ownership is not transferred
		*/
		CSIPIMSProfileContext* FindProfileContext(CSIPConcreteProfile& aSIPConcreteProfile) const;

		/**
		* Finds profile context
		* @param aProfileId identifier of the profile
		* @return profile context, the ownership is not transferred
		*/
		CSIPIMSProfileContext* FindProfileContext(TUint32 aProfileId) const;

		/**
		* Finds connection context
		* @param aPriofile a profile
		* @return a connection context
		*/
		CSIPIMSConnectionContext* FindConnectionContext(CSIPConcreteProfile& aProfile) const;
	
	    /**
		* Finds connection context
		* @param aIapId an iap id
		* @return a connection context
		*/
		CSIPIMSConnectionContext* FindConnectionContext(TUint32 aIapId) const;
		
		/**
		* Destroys idle connection contexts
		*/
		void CleanIdleConnectionContexts();
  
		/**
		* By default Symbian 2nd phase constructor is private.
		*/
		void ConstructL();
		
		/**
		* Finds connection context
		* @param aProfile a profile
		* register profile or puts it in to que
		*/
		void CheckIPSecRulesL(CSIPConcreteProfile& aProfile);
		
		/**
		* Checks if securitymechanism ipsec is in use, return ETrue if in use
		* @param aIsRegInProgress ETrue, if other profile
		* has registration in progress
		*/
		TBool IsIPSecAlreadyInUse(TBool& aIsRegInProgress);
		
		/**
		* Deletes all the profiles which are in que
		* waiting for permission to be registered
		*/
		void DeleteAllProfilesInWaitingQue();
		
		
		void RegisterFromWaitingQueL();
		
		TBool CheckIfConfigured(CSIPConcreteProfile& aSIPProfile);

		TBool MandatoryValuesConfigured(CSIPConcreteProfile& aSIPProfile);
		
		void RegisterFromAuthorizedArrayL(CSIPConcreteProfile& aSIPProfile);
		
		void ProfileToWaitingQueueL( CSIPConcreteProfile* aProfile );
		
		TBool DeregisterToWaitingQueueL( CSIPIMSProfileContext* aContext );

        void AuthorizationUSIMFailed();
        
        void HandleWaitingForRegisteringArray();
        
        TBool RegAllowedWithUSIMorISIML(CSIPConcreteProfile& aSIPProfile);
        
        TBool RegAllowedWithSIM(CSIPConcreteProfile& aSIPProfile);

        void GetTsyModuleNameL( TDes& aTsyModuleName ) const;
        
        CSIPConcreteProfile* RemoveFirstProfileFromArray( 
        						RPointerArray<CSIPConcreteProfile>& aProfileArray);
        
        
        void TerminateAndErrorEvent( CSIPConcreteProfile& aProfile,
									 TInt aError );
												   
		TBool FoundIPSecProfileCandidate();
		
		void ChooseProfileTypeL( CSIPConcreteProfile* aProfile, 
										CSIPIMSProfileContext* context );
										
		void CheckIMSReleaseTypeProfilesL();
		
		TBool CheckRegistrationsL();
		
		TBool HandleISIMUpdateRegistrationEventL(
		    CSIPConcreteProfile& aProfile, 
            CSIPIMSProfileContext* aContext);
    
	    TInt RegisterQueue( CSIPConcreteProfile& aProfile, TBool aReportError );
	    

	    TBool AllowedTakeFromQueue( CSIPConcreteProfile& aSIPConcreteProfile );
	    
	    CSIPConcreteProfile* PreviouslyFailedIMSProfileL();
	    
	    CSIPConcreteProfile* RemoveProfileFromWaitingQueue( 
	    					RPointerArray<CSIPConcreteProfile>& aProfileArray,
	    					CSIPConcreteProfile& aSIPConcreteProfile );
	    					
	    void FindProfileContextAndRegisterL( CSIPConcreteProfile& aSIPConcreteProfile );
	    void ResetISIMUpdateProfile( CSIPConcreteProfile* aSIPConcreteProfile );
    
	private: // Data

		CSIP*									iSIP;
		CSIPHttpDigest*                         iHttpDigest;
		MSIPProfileAgentObserver&				iSIPProfileAgentObserver;
		TSIPProfileTypeInfo						iType;
		CSIPPrflInitState*						iInit;
		CSIPPrflResolvingProxiesState*          iResolvingProxies;
		CSIPPrflRegisterRequestedState*			iRegRequested;
		CSIPPrflRegisterDelayRequestedState*	iRegDelayRequested;
		CSIPPrflRegistrationInProgressState*	iRegInProgress;
		CSIPPrflRegisteredState*				iRegistered;
		CSIPPrflDeregisterRequestedState*		iDeregRequested;
		CSIPPrflDeregistrationInProgressState*	iDeregInProgress;
		RTelServer                              iTelServer;
		RPointerArray<CSIPIMSConnectionContext> iConnectionCtxArray;
		RPointerArray<CSIPConcreteProfile> 		iWaitForRegisteringArray;
		RPointerArray<CSIPConcreteProfile>		iUSimWaitForAuthorizedArray;
		RPointerArray<CSIPConcreteProfile>		iSIMWaitForAuthorizedArray;
		RPointerArray<CSIPConcreteProfile>		iWaitForAuthorizedArray;

		CDeltaTimer&							iDeltaTimer;
		TUint32									iReTryProfileId;
		CSIPNotifyXmlBodyParser*                iXMLParser;
		CSIPProfileUsimProvider*				iUsimProvider;
		CSIPProfileSIMRecord*					iSIMRecord;
		TCallBack 								iDeltaTimerCallBack;
		TDeltaTimerEntry 						iDeltaTimerEntry;
		TUint         							iDeltaTimerEntryListCount;
		CSIPProfileQueueHandling* 				iProfileQueueHandling;
		CSIPConcreteProfile*					iISIMUpdateProfile;//not owned
		CSIPConcreteProfile*					iPreviouslyFailedIMSProfile;//not owned
		RPointerArray<CSIPConcreteProfile>		iFailedProfilesArray;
		CSIPConcreteProfile*					iInterimProfile;//not owned
		CSipProfileAgentConfigExtension*		iConfigExtension;//not owned
		
#ifdef CPPUNIT_TEST	
	    friend class CSIPIMSProfileAgentTest;
#endif
	};

#endif // CSIPIMSPROFILEAGENT_H
