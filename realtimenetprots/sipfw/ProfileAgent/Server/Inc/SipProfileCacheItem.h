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
* Name        : sipprofilecacheitem.h
* Part of     : SIP Profile Server
* Interface   : private
* The class used for caching and reference counting of profiles 
* Version     : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef CSIPPROFILECACHEITEM_H
#define CSIPPROFILECACHEITEM_H

// INCLUDES
#include "sipalrmigrationobserver.h"
#include "sipconcreteprofile.h"
#include <e32base.h>

// FORWARD DECLARATIONS

class MSIPExtendedConcreteProfileObserver;
class CSIPProfileState;
class CSIPProfileServerCore;
class CSipAlrMigrationController;

// CLASS DECLARATION

/**
 * The class used for caching and reference counting of profiles.
 */
class CSIPProfileCacheItem : public CBase, public MSipAlrMigrationObserver
	{
	public: // Enumerations

		enum TState 
			{
			// Normal profile state
			ENormal,
			// Profile to be removed, cannot be used
			EToBeRemoved
			};
			
    public: // Constructors and destructor

	    /**
        * Two-phased constructor.
		* @return new instance
        */
		static CSIPProfileCacheItem* NewL(CSIPProfileServerCore& aCore,
										  CSIPProfileState* aUnregistered);

	    /**
        * Two-phased constructor.
		* @return new instance
        */
		static CSIPProfileCacheItem* NewLC(CSIPProfileServerCore& aCore,
										   CSIPProfileState* aUnregistered);
	    /**
        * Destructor
        */
		~CSIPProfileCacheItem();

	public: // From MSipAlrMigrationObserver

		void IapAvailable(TUint32 aSnapId, TUint32 aNewIapId);

		void MigrationIsAllowedL(TUint32 aIapId);

        void MigrationIsDisallowedL(TUint32 aIapId);

        void ErrorOccurred(TInt aError);

        void NoNewIapAvailable();

		void RefreshIAPsFailed();

		void OfferedIapRejected();


	public: // New functions

	    /**
        * Sets concrete profile to be cached, ownership is
		* transferred
		* @param aProfile profile to be cached
        */
		void SetProfile(CSIPConcreteProfile* aProfile);

	    /**
        * Sets profile id of cached profile
		* @param aProfileId profile id
        */
		void SetProfileId(TUint aProfileId);

	    /**
        * Gets profile id of cached profile
		* @return profile id
        */
		TUint ProfileId() const;

	    /**
        * Add observer for profile
		* Client is considered observer when it instantiates profile
		* @param aObserver observer to send events
        */
		void AddObserverL(const MSIPExtendedConcreteProfileObserver& aObserver);

	    /**
        * Gets observers of the profile
		* @return array of observers
        */
		const RPointerArray<MSIPExtendedConcreteProfileObserver>&
			Observers() const;

	    /**
        * Checks if client is profile's observer or user.
		* Client is observer if it has instance of the disabled profile
		* @param aObserver checked observer
		* @return ETrue If client is observer or user
        */
		TBool IsObserverOrUser(
			const MSIPExtendedConcreteProfileObserver& aObserver) const;

	    /**
        * Checks if client is user
		* Client is user if it has instance of the enabled profile
		* @param aObserver checked observer
		* @return ETrue if user
        */
		TBool IsUser(
			const MSIPExtendedConcreteProfileObserver& aObserver) const;

	    /**
        * movies client from observer to user
		* Client is moved to user when it enables the profile
		* @param aObserver observer to be moved
        */
		void MoveToUserL(const MSIPExtendedConcreteProfileObserver& aObserver);

	    /**
        * Remove user for profile
		* Client is deleted enabled profile
		* @param aObserver observer to be removed
        */
		void RemoveUser(const MSIPExtendedConcreteProfileObserver& aObserver);

	    /**
        * Remove observer for profile
		* Client is no longer observer when it deletes profile
		* @param aObserver observer to be removed
        */
		void RemoveObserver(
			const MSIPExtendedConcreteProfileObserver& aObserver);

	    /**
        * Gets the number of users of the profile
		* @return number of users
        */
		TUint ReferenceCount() const;

	    /**
        * Gets the cached profile
		* @return Cached profile
        */
		CSIPConcreteProfile& Profile();
		
		/**
        * Gets the cached profile
		* @return Cached profile
        */
		const CSIPConcreteProfile& Profile() const;

		/**
        * Gets the cached profile, including the most recent updates.
		* @return Cached profile
        */
		CSIPConcreteProfile& LatestProfile();

	    /**
        * Caches old profile that cannot be deleted yet.
		* Old profile is stored if there is not yet old profile stored.
		* @param aNewProfile New profile.
		* @param aObserver Profile observer
		* @return ETrue If profile can be updated immediately
		*		  EFalse Update is queued and starts later
        */
		TBool CacheOldProfile(CSIPConcreteProfile* aNewProfile,
			const MSIPExtendedConcreteProfileObserver& aObserver);

	    /**
        * Clears the cached old profile. It can be done
		* when no one has reference to old profile anymore
        */
		void ClearOldProfile();

	    /**
        * Checks if a profile is being updated.
		* @return ETrue if exists
		*/
		TBool HasProfileUpdate() const;

	    /**
        * Gets the profile instance currently used
		* @return old profile if exists, otherwise new
        */
		CSIPConcreteProfile& UsedProfile();

	    /**
        * Gets the state of the cached profile.
		* @return ETrue if normal
		*		  EFalse if cache removed and can no longer be used
        */
		TBool IsActiveState() const;

	    /**
        * Checks if profile has certain AOR
		* @param aAOR AOR to be checked
		* @return ETrue if found
        */
		TBool HasAorL(const TDesC8& aAOR) const;

		/**
        * Start registering the profile. If profile has SNAP configured, then
        * depending on the value of aGetIap, it must first obtain an IAP.
        * @param aWaitForIAP Wait for IAP state
        * @param aRegInProg Registration in progress state
        * @param aGetIap If ETrue, then a profile that has SNAP, must obtain an
        *		         IAP before it can register.
        *				 If EFalse, profile can use the current IAP
        */
		void StartRegisterL(CSIPProfileState& aWaitForIAP,
							CSIPProfileState& aRegInProg,
							TBool aGetIap);

		/**
        * Enables profile
        * @param aObserver client enabling the profile
        */
		void EnableL(const MSIPExtendedConcreteProfileObserver& aObserver);

		/**
        * Enables profile
        * @param aObserver client enabling the profile
        * @param aSnapId SNAP id
        */
		void EnableSnapInUseL(
			const MSIPExtendedConcreteProfileObserver& aObserver,
			TUint32 aSnapId);

        /**
        * Disables profile
        * @param aObserver client disabling the profile
        */
		void DisableL(const MSIPExtendedConcreteProfileObserver& aObserver); 

        /**
        * Removes profile
        */
		void RemoveL(); 

		/**
        * System is about to be shut down.
        */
		void ShutdownInitiated();

		/**
        * Tells if system is about to be shut down.
        * @return ETrue if system shutdown has been initiated
        *		  EFalse otherwise
        */
		TBool IsShutdownInitiated() const;
		
      /**
        * System is about to go in Offline Mode
        */
        void OfflineInitiated(TBool aOfflineInitiated);

        /**
        * Tells if system is about to be Offline.
        * @return ETrue if system Offline has been initiated
        *         EFalse otherwise
        */
        TBool IsOfflineInitiated() const;
		
		/**
        * System is about to be shut down.
        */
        void RfsInprogress(TBool aStatus);

        /**
        * Tells if system is about to be shut down.
		* @return ETrue if system shutdown has been initiated
		*         EFalse otherwise
		*/
		TBool IsRfsInprogress() const;
		
		/**
		 * VPN session is about to start.
		 */
		void VpnInUse(TBool aStatus);

		/**
		 * Tells if VPN is in use.
		 * @return ETrue if VPN is in use.
		 *         EFalse otherwise
		 */
		TBool IsVpnInUse() const;
		
		/**
		 * Sets the initial APN for the profile
		 */
		void SetApnSelected(TBool aStatus);
		
		/**
		 * Tells if initial Apn is selected or not
		 * @return ETrue if Initial APN settings are done
         *         EFalse otherwise
		 */
		TBool IsInitialApnSelected() const;
		
	    /**
	     * Tells if Apn can be switched for a given IAP
	     * @return ETrue if Profile all the switchable parameters
         *         EFalse otherwise
	     */
		TBool IsApnSwitchEnabled() const;
		
		/**
		 * Sets the switch value for the profile
		 */
		 void SetApnSwitchStatus(TBool aStatus);
		        
        /**
        * Checks if profile can be permanently removed
		* @return ETrue if not used and can be removed
        */
		TBool CanBePermanentlyRemoved() const; 

        /**
        * Updates profile's registration if needed
        * @param aObserver client updating the profile
        */
		void UpdateRegistrationL(
			const MSIPExtendedConcreteProfileObserver& aObserver);

        /**
        * Updates registration status
        * @param aStatus new registration status
        * @param aStatusId internal id related to registration
        */
 		void RegistrationStatusEventL(
			CSIPConcreteProfile::TStatus aStatus,
			TUint32 aStatusId);

        /**
        * Updates current state of profile, ownership is not transferred
        * @param aNewState new state of profile
        */
 		void ChangeStateL(CSIPProfileState* aNewState);

        /**
        * Sends event to all observers of the profile 
        */
		void SendUnregisteredStatusEventL();

        /**
        * Pass error to current state, and in most cases this leads to
        * unregistered state.
        * @param aError Error code
        * @return ETrue Work normally
        *         EFalse Don't send error or registration state events to client
        */
 		TBool HandleError(TInt aError);

	    /**
        * Compares two items based on profile id.
		* @param aSearched Search condition
		* @param aArrayItem An item of the array, that is compared
		* @return ETrue if items match
        */
		static TBool Compare(const CSIPProfileCacheItem& aSearched,
							 const CSIPProfileCacheItem& aItem2);

		/**
        * Client allows migration to new IAP.
		* @param aIapId IAP id
		* @param aObserver Observer
        */
		void ClientAllowsMigrationL(TUint32 aIapId,
			const MSIPExtendedConcreteProfileObserver& aObserver);

		/**
        * Client does not allow migration to new IAP.
		* @param aIapId IAP id
		* @param aObserver Observer
        */
		void ClientDisallowsMigrationL(TUint32 aIapId,
			const MSIPExtendedConcreteProfileObserver& aObserver);

		/**
        * A client has (dis)allowed migration to a new IAP.
		* @param aIapId IAP id
		* @param aObserver Observer (client) who (dis)allows migration.
		*		 If NULL, then CSIPProfileCacheItem itself decides to (dis)
		*		 allow migration. Ownership is not transferred.
		* @param aAllowMigration ETrue if client allows migration
		*						 EFalse otherwise
        */
		void SetClientPermission(TUint32 aIapId,
			const MSIPExtendedConcreteProfileObserver* aObserver,
			TBool aAllowMigration);		

	    /**
        * Checks if the profile has a SNAP configured.
		* @param aSnapId OUT: SNAP id, if the profile has it
		* @return ETrue If SNAP id is configured, EFalse otherwise
        */
		TBool IsSNAPConfigured(TUint32& aSnapId) const;

		/**
        * Checks if aNewProfile has a different SNAP id than the current
        * profile.
		* @param aNewProfile Updated profile
		* @return ETrue aNewProfile Has a different SNAP id than current profile
        */
		TBool IsSnapIdUpdated(CSIPConcreteProfile& aNewProfile) const;

		/**
        * Sends "migration started" event to clients.
		* @param aSnapId SNAP id
		* @param aIapId IAP id
        */
		void PassMigrationStartedToClientL(TUint32 aSnapId, TUint32 aIapId);

		/**
        * Notifies the client about an ALR related error.
		* @param aError Error code
		* @param aIapId IAP id
		* @return system wide error code on failure
        */
		TInt PassAlrErrorToClient(TInt aError, TUint32 aIapId);

		/**
        * Process "IAP available" event.        
		* @param aSnapId SNAP id
		* @param aIapId Id of the new IAP
		* @param aRegistrationExists ETrue if profile has an existing
		*		 registration
		* @param aWaitForPermission Wait for permission state
        */
		void HandleNewIapL(TUint32 aSnapId,
						   TUint32 aIapId,
						   TBool aRegistrationExists,
						   CSIPProfileState& aWaitForPermission);						   

		/**
        * Profile starts to monitor the SNAP.
		* @param aSnapId SNAP id
		* @param aWaitForIAP Wait for IAP state
		* @param aRegInProg Registration in progress state
        */
		void MonitorSnapL(TUint32 aSnapId,
						  CSIPProfileState& aWaitForIAP,
						  CSIPProfileState& aRegInProg);

		/**
        * Profile has been registered.
        * @param aMigrating ETrue if now migrating to an IAP,
        *					EFalse otherwise
        */
		void ProfileRegisteredL(TBool aMigrating);

		/**
        * An existing registration has ended because the IAP dropped.
        */
		void RegistrationEnded();

		TBool IsReferred() const;

		/**
        * If "update profile" has been issued, continue it.
        * @param aUnregInProg Unregister in progress state
        */
		void ProceedUpdatingProfileL(CSIPProfileState& aUnregInProg);

		/**
        * Profile no longer monitors SNAP.
        */
		void StopSnapMonitoring();

		CSIPConcreteProfile& CloneProfileL(TUint aIapId);

		void ClearMigrationProfiles();
		
		CSIPConcreteProfile& UseProfileWithNewIAP();

		/**
        * Checks if migration to a new IAP is happening. Includes the period
        * when registering with the new IAP and de-registering with the old IAP.
        */
		TBool IAPMigrationInProgress() const;

		/**
        * Check if there are pending activities that were queued and could now
        * be resumed.
        * If there is a queued profile update, it is continued. Otherwise IAPs
        * are refreshed.
        */
		void ResumeL();

		/**
		* Set the iMustRefreshIAPs flag to remind that IAPs must be refreshed
		* when the profile enters a suitable state.
        * @post iMustRefreshIAPs == ETrue
        */
		void SetIAPRefreshReminder();

		/**
		* Starts timer to inform ALR monitor that the new IAP 
		* does not work successfully.
        */
		void NewIAPFailed();
		
		/**
        * Sets SNAP retry counter value
        * @param aCounter Counter value
        */
		void SetSnapRetryCounter( TInt aCounter );
		
		/**
        * Returns SNAP retry counter value
		* @return retry counter
        */
		TInt SnapRetryCounter() const;
		
		TBool SnapRetryCountReached() const;

		/**
        * Switches state machine to unregistered state
        */
		TBool SwitchToUnregisteredState(TBool aResume); 	
		
		/**
		* Handles Errors caused by faulty profiles
		* @param aError Error code
		* @param aStatus Status of the profile
		*/
		void HandleProfileError(TInt aError, 
			CSIPConcreteProfile& aProfile);		
/**
		* Function returns the Bearer ID for the Profile. 
		*/
		TUint32 BearerID();

		TBool HasQueuedUpdate()const;
	private:

	    /**
        * Removes all users for profile
		* @param aObserver observer to be removed
        */
		void RemoveAllUsers();
		
		/**
        * Updates enabled state of the associated concrete profile. 
        */
		void CheckProfileEnabledState();

	    /**
        * Default constructor
        */
		CSIPProfileCacheItem(CSIPProfileServerCore& aCore, 
							 CSIPProfileState* aUnregistered);		

		void ConstructL(); 

		const CSIPConcreteProfile& UsedProfile() const;

		void StoreProfileToUpdate(CSIPConcreteProfile* aNewProfile);

		void ResumeQueuedUpdateL();		

		void RemoveFromPendingObservers(
			const MSIPExtendedConcreteProfileObserver& aObserver);

		/**
		* Inform ALR monitor that the new IAP does not work successfully.
        */
		static TInt DoNewIapFailed(TAny* aPtr);
		void DefaultSNAPL(TUint32& aSnapId) const;
	
	private:

		// Profile id. This is used when communicating with profile client.
		TUint						iProfileId;


		// iProfile, iOldProfile, iProfileWithNewIAP, iProfileWithOldIAP and
		// iQueuedProfile all have same storage id.

		// Concrete profile. Owned.
		CSIPConcreteProfile*		iProfile;
		// When profile is updated, the existing profile is put here. Owned.
		CSIPConcreteProfile*		iOldProfile;

		// When migrating to a new IAP, the cloned profile is stored here.
		// Owned.
		CSIPConcreteProfile*		iProfileWithNewIAP;
		
		// After migration to a new IAP, the profile with old IAP is stored
		// here. Owned.
		CSIPConcreteProfile*		iProfileWithOldIAP;

		// If "update profile" is issued during IAP migration, the updated
		// profile and observer are stored here. Only the profile and observer
		// of the most recent update are stored.
		// Queued profile is owned.
		CSIPConcreteProfile*		iQueuedProfile;
		// Queued observer is not owned
		const MSIPExtendedConcreteProfileObserver* iQueuedObserver;


		TState						iCacheState;
		CSIPProfileState*			iCurrentState;
		CSIPProfileServerCore&		iServerCore;

		RPointerArray<MSIPExtendedConcreteProfileObserver> iObservers;
		RPointerArray<MSIPExtendedConcreteProfileObserver> iUsers;

		// After passing "IAP available" to observers, the observers that have
		// not yet (dis)allowed the migration, are kept here. When an observer
		// tells it (dis)allows migration or if the observer itself is removed
		// with RemoveObserverL(), it is removed from this array.
		// When this array is empty, the it is determined whether the migration
		// is allowed or disallowed. Array items are not owned.
		RPointerArray<MSIPExtendedConcreteProfileObserver>
									iObserversWaitedForPermission;

		
		// ETrue if system shutdown has been initiated
		TBool 						iIsShutdownInitiated;
		
		//ETrue if system offline has been initiated
		TBool                       iIsOfflineInitiated;                    
		
		// ETrue if Rfs has been initiated
		TBool                       iIsRfsInprogress;

		// ETrue if VPN is in use
		TBool                       iIsVpnInUse;
		
		// Tells if at least one client disallowed migration to a new IAP
		TBool 						iMigrationDisallowed;

		// ETrue if a new IAP came available, but it was disallowed by the
		// profile, because new IAP couldn't be handled.
		TBool						iMustRefreshIAPs;
		
		// ETrue when if the initial APN is selcted.
		TBool                       iInitialApnSelected;
		
		// ETrue when APN can be switched for the profile. 
		TBool                       iApnSwitchEnabled;

		// Not owned. NULL if SNAP is not configured.
		CSipAlrMigrationController* iMigrationController;

		CDeltaTimer* iDeltaTimer;
		TCallBack iDeltaTimerCallBack;
		TDeltaTimerEntry iDeltaTimerEntry;
		TInt iSnapRetryCounter; 

	private: // For testing purposes
#ifdef CPPUNIT_TEST
		friend class CSIPProfileStateTest;
		friend class CSIPProfileServerCoreTest;
		friend class CSIPProfileStateRegInProgTest;
		friend class CSIPProfileStateRegisteredTest;
		friend class CSIPProfileStateUnregInProgTest;
		friend class CSIPProfileStateUnregisteredTest;
		friend class CSIPProfileStateWaitForIAPTest;
		friend class CSIPProfileStateWaitForPermissionTest;
		friend class CSIPProfileStateMigratingToNewIAPTest;
		friend class CSIPProfileStateUnregisteringOldIAPTest;
		friend class CSipAlrHandlerTest;
		friend class CSIPProfileAlrTest;
		friend class CSIPProfileCacheItemTest;
#endif
	};

#endif
