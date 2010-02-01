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
* Name        : sipprofilequeuehandling.h
* Part of     : sip profile fsm
* Interface   : Internal IF
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPRROFILEQUEUEHANDLING_H
#define CSIPPRROFILEQUEUEHANDLING_H

//INCLUDES 
#include "sipprofilequeueitem.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class  MSIPProfileFSMUser;
class CSIPConcreteProfile;

// CLASS DECLARATION
/**
*  A class for maintaining the queue handling for profiles which 
*  are pending for registration or deregistration.
*  Class stores the arrays of profiles.
*
*  @lib sipprofilefsm.lib
*/
class CSIPProfileQueueHandling : public CBase
	{
	public:

		/**
        * Two-phased constructor.
		* @param aUser user for the state machine
        */
		IMPORT_C static CSIPProfileQueueHandling* NewL(
		    MSIPProfileFSMUser& aUser );

   		/**
        * Two-phased constructor.
        * @param aUser user for the state machine
        */
		IMPORT_C static CSIPProfileQueueHandling* NewLC(
		    MSIPProfileFSMUser& aUser );

        /**
        * Destructor.
        */
		IMPORT_C ~CSIPProfileQueueHandling();
		

	public: // New functions
	
		IMPORT_C TBool AddRegisterToQueueL( 
		    CSIPConcreteProfile& aSIPConcreteProfile,
		    TBool aRetryRegistration );	
	
		IMPORT_C TBool AddDeregisterToQueueL( 
		    CSIPConcreteProfile& aSIPConcreteProfile );

		/**
		* After final response is received, queue of profiles
		* which are waiting deregistration is checked.
		* If there is found profile with the same registrar as 
		* input profile aSIPConcreteprofile has, that profile is removed 
		* from queue and registration is continued with that profile.
		*/
		IMPORT_C void RegisterFromQueueL( 
		    CSIPConcreteProfile& aSIPConcreteProfile );

		/**
		* Check if queue has profile which has 
		* SecurityNegotiationEnabled and does not has HttpDigestSettings
		* configured.
		* return ETrue in that case.
		*/
		IMPORT_C TBool FoundIPSecProfileCandidate();

		/**
		* Check if queue has profile which has 
		* SecurityNegotiationEnabled and does not has HttpDigestSettings
		* configured.If profile found, it will be removed from queue
		* and will be returned.Input value aRegistering get value EFalse if profile 
		* found from deregistering queue, otherwise value is ETrue;
		* Return NULL and ETrue, if no profile found.
		* return .
		*/
		IMPORT_C CSIPConcreteProfile* IPSecProfileCandidate(TBool& aRegistering);
		

        /**
		* Check if queue has profile which is waiting for allowed network. 
		* A profile may be configured so that it can be used only in home network
		* or used in roaming state only if user has explicitly enabled it.
		* @param aOnHomeNetwork ETrue if phone is currently on home nw, EFalse
		*        if phone is roaming
		* @return profile to be registered or NULL
		*/
        IMPORT_C CSIPConcreteProfile* WaitingForAllowedNetworkProfile( 
            TBool aOnHomeNetwork );
        
        IMPORT_C CSIPConcreteProfile* RemoveProfileFromRegQueueL(
            CSIPConcreteProfile& aSIPConcreteProfile);                
                            
        /**
		* Check if both (registration and deregistration) queues are empty.
		* @return ETrue if both queues are empty
		*/
        IMPORT_C TBool IsEmpty() const;

        /*
        * Cleanup of queue, profiles which have been changed from auto 
        * registration mode to when needed or which don't have any users
        * are removed.
        */
        IMPORT_C void Cleanup( CSIPConcreteProfile& aOldProfile,
                               CSIPConcreteProfile* aNewProfile );
        
        /**
		* Check if profile is in queue 
		* Return ETrue, if profile found.
		*/     
        IMPORT_C TBool IsInQueue(
            CSIPConcreteProfile& aSIPConcreteProfile);                

		/**
		* Checks whether registration is allowed in current network 
		* for the given profile
		* @param aSIPConcreteProfile the profile to be checked
		* @param aOnHomeNetwork ETrue if the phone is currently on home network, 
		*        EFalse if the phone is roaming
		* @return ETrue if registration is allowed
		*/
		IMPORT_C TBool IsRegistrationAllowed(
		    CSIPConcreteProfile& aSIPConcreteProfile,
		    TBool aOnHomeNetwork);

    public: // New functions
    
		/**
		* Checks if registration/deregistration should be put
		* in que to wait, because there is registration/deregistration
		* with same registrar address pending.
		* @return ETrue if should be put into que to wait.
		*/
		TBool AddIntoQueue( CSIPConcreteProfile& aSIPConcreteProfile );

		/**
		* aSIPConcreteProfile is added into queue ,
		* until there is no more registration/deregistration
		* with same registration address pending final response.
		* @return ETrue if profile was added
		*/
		TBool AddIntoDeregisterQueueL( 
		    CSIPConcreteProfile& aSIPConcreteProfile );

		/**
		* aSIPConcreteProfile is added into queue ,
		* until there is no more registration/deregistration
		* with same registration address pending final response.
		* @return ETrue if profile was added
		*/
		TBool AddIntoRegisterQueueL( 
		    CSIPConcreteProfile& aSIPConcreteProfile,
		    TBool aRetryRegistration );
    
		/**
		* After final response is received, queue of profiles
		* which are waiting deregistration is checked.
		* If there is found profile with the same registrar as 
		* input profile aSIPConcreteprofile has, that profile is removed 
		* from queue and deregistration is continued with that profile.
		*/
		TBool CheckDeregisterQueueL( 
		    CSIPConcreteProfile& aSIPConcreteProfile );

		/**
		* After final response is received, queue of profiles
		* which are waiting registration is checked.
		* If there is found profile with the same registrar as 
		* input profile aSIPConcreteprofile has, that profile is removed 
		* from queue and registration is continued with that profile.
		*/
		TBool CheckRegisterQueueL( 
		    CSIPConcreteProfile& aSIPConcreteProfile );
		
		/**
		* If from aProfileArray is found a profile with the same 
		* registrar as input profile aSIPConcreteprofile has, 
		* that profile is removed from queue and returned.
		*/
		TInt FindIndex(
            CSIPConcreteProfile& aSIPConcreteProfile,
            RArray<TSIPProfileQueueItem>& aProfileArray );

    private: // New functions
    
        TBool AddToQueueNoDuplicatesL( 
            RArray<TSIPProfileQueueItem>& aProfileArray,
            TSIPProfileQueueItem& aProfileItem );
    
        void QueueCleanup( RArray<TSIPProfileQueueItem>& aProfileArray,
                           CSIPConcreteProfile& aOldProfile,
                           CSIPConcreteProfile* aNewProfile );

		/**
		* Constructor
		*/
	    CSIPProfileQueueHandling( MSIPProfileFSMUser& aUser );
	    
	    void ConstructL();

	private: //data
	
	    MSIPProfileFSMUser& iUser;
		RArray<TSIPProfileQueueItem> iRegisteringQueue;
		RArray<TSIPProfileQueueItem> iDeregisteringQueue;
		
	private: // For testing purposes
	
        #ifdef CPPUNIT_TEST	
	    	friend class CSIPProfileQueueHandlingTest;
        #endif
	};

#endif // CSIPPRROFILEQUEUEHANDLING_H
