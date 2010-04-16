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
* Name        : sipprofilestate.h
* Part of     : Sip Profile Server
* Interface   : private
* Base class for profile states
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILESTATE_H
#define CSIPPROFILESTATE_H

// INCLUDES
#include "sipconcreteprofile.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class CSIPProfileCacheItem;
class CSIPPluginDirector;
class MSIPExtendedConcreteProfileObserver;
class CSIPProfileServerCore;

// CLASS DECLARATION

/**
*  Base class for profile state classes
*  @lib 
*/
class CSIPProfileState: public CBase
	{
	public:  // Enums

		// State ids used for logging.
		// The actual values of these enums must be different from the values
		// of CSIPConcreteProfile::TStatus.
        enum TStatus
            {
            /** Wait for IAP */
            EWaitForIAP = 10,
            /** Wait for IAP migration permission */
            EWaitForPermission,
            /** Migrating to new IAP */
            EMigratingToNewIAP,
            /** Unregistering old IAP */
            EUnregisteringOldIAP
            };

   public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~CSIPProfileState();

    public: // New functions
        
        /**
        * Gets CSIPConcreteProfile status of the state
        * @return CSIPConcreteProfile status of the state
        */      
		virtual CSIPConcreteProfile::TStatus Name() const = 0;

        /**
        * Called when entering a new state.
        * @param aItem Profile cache item
        * @param aMaySendRegStatusEvent ETrue if registration status event can
        *		 be sent to client. EFalse if it must not be sent.
        * @param aServerCore Server core where status event is sent to
        */
		virtual void EnterL(CSIPProfileCacheItem& aItem,
							TBool aMaySendRegStatusEvent,
							CSIPProfileServerCore& aServerCore);

        /**
        * Registers profile, leaves because not allowed
        * @param aItem profile cache item
        */
		virtual void RegisterL(CSIPProfileCacheItem& aItem);

		/**
        * Updates profile registration
        * @param aItem profile cache item
        * @param aObserver client updating the profile
        */
		virtual void UpdateRegistrationL(
			CSIPProfileCacheItem& aItem,
			const MSIPExtendedConcreteProfileObserver& aObserver);

		/**
        * Enables profile
        * @param aItem profile cache item
        * @param aObserver client enabling the profile
        */      
		virtual void EnableL(
			CSIPProfileCacheItem& aItem,
			const MSIPExtendedConcreteProfileObserver& aObserver) = 0;

		/**
        * Enables profile
        * @param aItem profile cache item
        * @param aObserver client enabling the profile
        * @param aSnapId SNAP id
        */      
		virtual void EnableSnapInUseL(
			CSIPProfileCacheItem& aItem,
			const MSIPExtendedConcreteProfileObserver& aObserver,
			TUint32 aSnapId) = 0;

        /**
        * Disables profile.
        * @param aItem profile cache item
        * @param aObserver client disabling the profile
        */
		virtual void DisableL(
			CSIPProfileCacheItem& aItem,
			const MSIPExtendedConcreteProfileObserver& aObserver) = 0;

        /**
        * Profile Removed, proper actions done in order.
		* to unregister if not unregistered
        * @param aItem profile cache item
        */
		virtual void RemoveL(CSIPProfileCacheItem& aItem);

        /**
        * Checks if profile can be permanently removed
        * @param aItem profile cache item
		* @return ETrue, if unregistered and can be removed
        */
		virtual TBool CanBePermanentlyRemoved(
			const CSIPProfileCacheItem& aItem) const;

        /**
        * Updates registration status.
        * @param aItem profile cache item
        * @param aStatus new registration status
        */
		virtual void RegistrationStatusEventL(
			CSIPProfileCacheItem& aItem,
			CSIPConcreteProfile::TStatus aStatus);

        /**
        * Resets registration status to unregistered befause of an error.
        * @param aItem profile cache item
        * @param aError occurred error code
        * @return ETrue Work normally
        *         EFalse Don't send error or registration state events to client
        */
		virtual TBool ErrorOccurred(CSIPProfileCacheItem& aItem,
								    TInt aError) = 0;

		/**
		* A new IAP became available.
		* This event is only received by a profile that has a SNAP configured.
		* @param aItem Profile cache item
		* @param aSnapId a SNAP id.
		* @param aNewIapId an IAP id. 
		*/
		virtual void IapAvailableL(CSIPProfileCacheItem& aItem,
								   TUint32 aSnapId,
								   TUint32 aNewIapId);

		/**
        * Migration to new IAP is allowed by a client.
        * @param aItem Profile cache item
		* @param aIapId IAP id
		* @param aObserver Observer
        */
		virtual void ClientAllowsMigrationL(CSIPProfileCacheItem& aItem,
			TUint32 aIapId,
			const MSIPExtendedConcreteProfileObserver& aObserver);

		/**
        * Migration to new IAP is not allowed by a client.
        * @param aItem Profile cache item
		* @param aIapId IAP id
		* @param aObserver Observer
        */
		virtual void ClientDisallowsMigrationL(CSIPProfileCacheItem& aItem,
			TUint32 aIapId,
			const MSIPExtendedConcreteProfileObserver& aObserver);

		/**
        * Overall result of all clients of all profiles using the same SNAP, is
        * that migration is allowed.
        * @param aItem Profile cache item
		* @param aIapId IAP id
        */
		virtual void MigrationIsAllowedL(CSIPProfileCacheItem& aItem,
										 TUint32 aIapId);

		/**
        * Overall result of all clients of all profiles using the same SNAP, is
        * that migration is not allowed.
        * @param aItem Profile cache item
		* @param aIapId IAP id
        */
		virtual void MigrationIsDisallowedL(CSIPProfileCacheItem& aItem,
										 	TUint32 aIapId);

		virtual TBool ShouldRefreshIAPs(CSIPProfileCacheItem& aItem) const;

		/**
        * Determines if the state is an ALR-related state, or not.
		* @return ETrue If the state is one of the ALR-related ones
		*		  EFalse Otherwise
        */
		virtual TBool IsAlrState() const;

		/**
        * System is about to be shut down.
        * @param aItem Profile cache item
        */
		virtual void ShutdownInitiated(CSIPProfileCacheItem& aItem);
		
		/**
        * No unused IAP available in SNAP
        * @param aItem Profile cache item
        */
		virtual void NoNewIapAvailable(CSIPProfileCacheItem& aItem);

		/**
        * Determines if profile can be updated in current state, or if it must
        * be queued and updated later.
        * @param aItem Profile cache item
		* @return ETrue Can update now
		*		  EFalse Must queue
        */
		TBool CanProfileBeUpdated(CSIPProfileCacheItem& aItem) const;

		/**
        * Checks if an initial registration has been done.
        * @param aItem Profile cache item
        * @return ETrue Initial registration has been done
        *		  EFalse Otherwise
        */
		TBool IsInitialRegisterDone(CSIPProfileCacheItem& aItem) const;

		/**
        * Register profile using the IAP id.
        * @param aItem Profile cache item
        * @param aIapId IAP id to register with
        * @param aRegInProg "Register in progress" state
        */
		void RegisterWithIapL(CSIPProfileCacheItem& aItem,
							  TUint32 aIapId,
							  CSIPProfileState& aRegInProg) const;

		/**
        * De-register the used profile
        * @param aItem profile cache item
        * @param aUnregInProg "Unregister in progress" state
        */
		void StartDeregisterL(CSIPProfileCacheItem& aItem,
							  CSIPProfileState& aUnregInProg) const;

		/**
        * Update profile.
        * @param aProfile Profile with updated data
        * @param aOldProfile Existing profile
        * @return ETrue  if de-register started for the existing profile
        *		  EFalse if the update is so minor that e.g. sending an UPDATE
        *				 is enough, and there is no need for de-register
        */
		TBool DoUpdateL(CSIPConcreteProfile& aProfile,
						CSIPConcreteProfile& aOldProfile) const;

		/**
        * Refreshing IAPs has failed. Terminate the handling of this profile.
        * @param aItem profile cache item
        */
		void RefreshIAPsFailed(CSIPProfileCacheItem& aItem) const;

    protected:

        /**
        * Constructor.
		* @param aPluginDirector plugin mgr
        */
        CSIPProfileState(CSIPPluginDirector& aPluginDirector);

        /**
        * Enables profile, leaves because not allowed
        * @param aItem profile cache item
        * @param aObserver client enabling the profile
        */      
		void DoCommonEnableL(
			CSIPProfileCacheItem& aItem,
			const MSIPExtendedConcreteProfileObserver& aObserver) const;

        /**
        * Disables profile
		* @param aCore server core
        * @param aItem profile cache item
        */
		void DoCommonDisable(
			CSIPProfileCacheItem& aItem,
			const MSIPExtendedConcreteProfileObserver& aObserver) const;

        /**
        * Resets registartion status of underlying layers 
		* to unregistred befause of error
        * @param aItem profile cache item
        */
		void DoCommonErrorOccurred(CSIPProfileCacheItem& aItem) const;

		void DisableWhileWaitingIAPL(CSIPProfileCacheItem& aItem,
			const MSIPExtendedConcreteProfileObserver& aObserver,
			CSIPProfileState& aUnregInProg,
			CSIPProfileState& aUnregistered) const;

		void DisableWhileInProgressL(CSIPProfileCacheItem& aItem,
			CSIPProfileState& aUnregInProg,
			CSIPProfileState& aUnregistered) const;

        /**
        * Deregister profiles while registration is is progress
        * @param aItem profile cache item
        * @param aItem aUnregistered "Unregister" state
        */
        void DeregisterWhileRegInProgressL(CSIPProfileCacheItem& aItem,
            CSIPProfileState& aUnregistered);

	protected: // Data

		CSIPPluginDirector& iPluginDirector;

	private: // For testing purposes
#ifdef CPPUNIT_TEST
		friend class CSIPProfileStateTest;
#endif
	};

#endif
