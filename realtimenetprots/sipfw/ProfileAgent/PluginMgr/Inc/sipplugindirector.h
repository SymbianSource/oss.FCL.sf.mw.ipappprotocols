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
* Name        : sipplugindirector.h
* Part of     : sip profile server
* Interface   : private
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPLUGINDIRECTOR_H
#define CSIPPLUGINDIRECTOR_H

//  INCLUDES
#include "sipprofileagentobserver.h"
#include "sipconcreteprofile.h"
#include <e32base.h>
#include <ecom/implementationinformation.h>
#include <sipprofileagentextensionparams.h>


//FORWARD DECLARATIONS
class CSIPProfileAgent;
class CSIPConcreteProfile;
class TSIPProfileTypeInfo;
class CSipProfileAgentConfigExtension;

// CLASS DECLARATION
/**
*  Delegates requests to the correct implementation plug-in.
*  Provides functions for registering, unregistering and
*  updating registration.
*
*  @lib sipprofilesrv.exe
*/
class CSIPPluginDirector: public CBase
	{
	public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
		* @param aObserver profile agent observer
        */
        static CSIPPluginDirector* NewL(MSIPProfileAgentObserver& aObserver);

        /**
        * Two-phased constructor.
		* @param aObserver profile agent observer
        */
        static CSIPPluginDirector* NewLC(MSIPProfileAgentObserver& aObserver);
		
        /**
        * Destructor.
        */
        virtual ~CSIPPluginDirector();

    public: // New functions
        
        /**
        * Returns iDeltaTimer as reference.
        * @return Reference to member variable iDeltaTimer
        */ 
        CDeltaTimer& DeltaTimer();
        
        /**
        * Registers profile
        * @param aSIPProfile sip profile to register
        */        
		void RegisterL(CSIPConcreteProfile& aSIPProfile);

        /**
        * Deregisters profile
        * @param aSIPProfile sip profile to unregister
        */        
		void DeregisterL(CSIPConcreteProfile& aSIPProfile);

        /**
        * Updates profile registration
        * @param aOldProfile sip profile which registration will be updated
        * @param aNewProfile old sip profile
        */   
		void UpdateRegistrationL(CSIPConcreteProfile& aNewProfile, 
			CSIPConcreteProfile& aOldProfile);

		/**
		* Lists all implementations available; leaves on failure
		* @param aImplementations on return will contains all implementations
		*/
		void ListImplementationsL(RImplInfoPtrArray& aImplementations);

        /**
        * Checks whether a registration is pending for the profile.
        * In other words if a REGISTER has been sent 
        * but a final response has not yet been received.
        * @param aSIPProfile sip profile
        */        
		TBool IsRegisterPending(CSIPConcreteProfile& aSIPProfile);

        /**
        * Checks state of profile from plugin
        * @param aStatus returned profile status
        * @param aSIPProfile sip profile to be checked
		* @return KErrNotFound, if not found, otherwise KErrNone
        */        
		TInt State(CSIPConcreteProfile::TStatus& aStatus, 
				   CSIPConcreteProfile& aSIPProfile);

		/**
		* Terminates abnormally profile handling i.e.
		* cleanups reserved resources for the profile and forgets it.
		* @param aSIPProfile a SIP profile
		* @return KErrNone if successfull; 
		*         KErrNotFound if the profile was not found
		*/
		TInt TerminateHandling(CSIPConcreteProfile& aSIPProfile);

		/**
		* Cleans up idle plugins.
		*/
		void Cleanup();

		/**
		* Creates new profile prefilled with default values.
		* Ownership is transferred.
		* @param aType a type of SIP profile
		* @return new instance of CSIPConcreteProfile
		*/
		CSIPConcreteProfile* CreateProfileL(const TSIPProfileTypeInfo& aType);
		
		/**
		*Starts timer of type CDeltaTimer, 
		*which callback is CleanupTimerExpired function
		*/
		void StartTimerForCleanup();
		
		/**
		* A callback for CDeltaTimer
		*/
		static TInt CleanupTimerExpired(TAny* aPtr);

    private:

        /**
        * C++ default constructor.
        */
        CSIPPluginDirector(MSIPProfileAgentObserver& aObserver);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

		/**
		* provides correct plugin
		*/
		CSIPProfileAgent& ProvideImplementationL(const TSIPProfileTypeInfo& aType);

		/**
		* Checks for correct plugin
		* @return 0-pointer if not found
		*/
		CSIPProfileAgent* FindImplementation(const TSIPProfileTypeInfo& aType);

		/**
		* Creates implementation
		*/
		CSIPProfileAgent& CreateImplementationL(const TSIPProfileTypeInfo& Type);


	private: // Data

		RPointerArray<CSIPProfileAgent> iSIPProfileAgents;
		MSIPProfileAgentObserver& iProfileAgentObserver;
		CDeltaTimer* iDeltaTimer;
		TUid iServerId;
		TCallBack iDeltaTimerCallBack;
		TDeltaTimerEntry iDeltaTimerEntry;
		CSipProfileAgentConfigExtension* iConfigExtension;
		
    private: // For testing purposes
#ifdef CPPUNIT_TEST
		friend class CSIPPluginDirectorTest;
#endif
	};

#endif CSIPPLUGINDIRECTOR_H

