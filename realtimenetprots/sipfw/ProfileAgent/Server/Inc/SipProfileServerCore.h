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
* Name        : SipProfileServerCore.h
* Part of     : SIP Profile Server
* Interface   : private
* The class providing core functionality of profile server
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILESERVERCORE_H
#define CSIPPROFILESERVERCORE_H

//  INCLUDES
#include "sipconcreteprofile.h"
#include "sipprofileagentobserver.h"
#include "sipprofileerrorhandler.h"
#include <sipsystemstateobserver.h>
#include "sipapnmanager.h"
#include "sipalrobserver.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class CSIPProfileCSServer;
class CSIPProfileStorage;
class CSIPProfileStorageSecureBackup;
class CSIPProfileCacheItem;
class MSIPExtendedConcreteProfileObserver;
class CSIPProfilePlugins;
class CSIPPluginDirector;
class CSIPProfileState;
class CSIPProfileStateUnregistered;
class CSIPProfileStateRegistered;
class CSIPProfileStateUnregInProg;
class CSIPProfileStateRegInProg;
class CSIPProfileStateWaitForIAP;
class CSIPProfileStateWaitForPermission;
class CSIPProfileStateMigratingToNewIAP;
class CSIPProfileStateUnregisteringOldIAP;
class CImplementationInformation;
class CSipAlrHandler;
class CSipAlrMigrationController;
class CSipSystemStateMonitor;

// CLASS DECLARATION

/**
 * The class providing core functionality of profile server.
 */
class CSIPProfileServerCore : public CBase, 
                              public MSIPProfileAgentObserver,
                              public MSipProfileErrorHandler,
                              public MSipSystemStateObserver,
                              public MSIPApnChangeObserver
	{
    
    public:
    /*
     * struct to store ApnSwitchEnabled profiles in the event 
     * when IAPSettings are not same as required.
     */
    struct TStoreSwitchEnabledProfile
        {
    public:
        enum TOperation
            {
            Update =1,
            Enable,
            Register
            };
        CSIPConcreteProfile*                          iProfile;
        const MSIPExtendedConcreteProfileObserver*    iObserver;
        TOperation                                    operation;
        };
    
	public: // Constructors and destructor
    
	    /**
        * Two-phased constructor.
        * @param aServer server of the session
		* @return new instance
        */
		static CSIPProfileServerCore* NewL();

	    /**
        * Two-phased constructor.
        * @param aServer server of the session
		* @return new instance
        */
		static CSIPProfileServerCore* NewLC();

	    /**
        * Destructor.
        */
		~CSIPProfileServerCore();

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
            RPointerArray<CSIPConcreteProfile>& aFailedProfiles ) const;

	public: // MSipProfileErrorHandler

		TBool HandleProfileError(CSIPProfileCacheItem& aItem, TInt aOwnError);

	public: // MSipSystemStateObserver

        void SystemVariableUpdated( 
            CSipSystemStateMonitor::TSystemVariable aVariable,
    		TInt aObjectId,
    		TInt aValue );
        
	public: // MSIPApnChangeObserver
	    
        void ApnChanged( const TDesC8& aApn, TUint32 aIapId, TInt aError );

    public: // New functions

	    /**
        * Stores client observer for sending events
		* @param aObserver observer to be stored
        */
		void SessionRegisterL(
			const MSIPExtendedConcreteProfileObserver& aObserver);

	    /**
        * Removes client observer
		* @param aObserver observer to be removed
        */
		void SessionCleanup(
			const MSIPExtendedConcreteProfileObserver& aObserver);

	    /**
        * Gets profile plugins containing plugins type info
		* @return profile plugins containing plugins type info
        */
		const CSIPProfilePlugins& ProfilePluginsL();

	    /**
        * Adds profile into permanent store, ownership is transferred
		* @param aProfile a stored profile
		* @param aObserver client observer of the profile
        */
		void AddProfileL(CSIPConcreteProfile* aProfile,
			const MSIPExtendedConcreteProfileObserver& aObserver);

	    /**
        * Does the first phase of a profile update, by making update to
        * permanent store.
		* @param aProfile Profile to store, ownership is transferred.
		* @param aObserver client observer of the profile
		* @return ETrue If can proceed to the next phase of the update
		*		  EFalse Must queue the update
        */
		TBool UpdateProfileToStoreL(CSIPConcreteProfile* aProfile,
			const MSIPExtendedConcreteProfileObserver& aObserver);

	    /**
        * Removes profile from permanent store
		* @param aProfileId a removed profile id
        */
		void RemoveProfileL(TUint32 aProfileId);

	    /**
        * Updates profile registration if needed
		* @param aProfileId a registered profile id
		* @param aObserver client observer of the profile
        */
		void UpdateRegistrationL(TUint32 aProfileId,
			const MSIPExtendedConcreteProfileObserver& aObserver);

	    /**
        * Enables profile for use
		* @param aProfileId a enabled profile id
		* @param aObserver client observer of the profile
		* @return status of the profile
        */
		CSIPConcreteProfile::TStatus EnableProfileL(
			TUint32 aProfileId,
			const MSIPExtendedConcreteProfileObserver& aObserver);

	    /**
        * Disables profile from use
		* @param aProfileId a enabled profile id
		* @param aObserver client observer of the profile
		* @return status of the profile
        */
		CSIPConcreteProfile::TStatus DisableProfileL(
			TUint32 aProfileId,
			const MSIPExtendedConcreteProfileObserver& aObserver);

		/**
		* Force disables profile from use
		* @param aProfileId a enabled profile id
		* @param aObserver client observer of the profile
		* @return status of the profile
		*/
		CSIPConcreteProfile::TStatus ForceDisableProfileL(
			TUint32 aProfileId,
			const MSIPExtendedConcreteProfileObserver& aObserver);
		
	    /**
        * Gets number of users of a profile
		* @param aProfileId a enabled profile id
		* @return number of users
        */
		TInt ProfileUsageL(TUint32 aProfileId) const;

	    /**
        * Gets profile.
		* This method should only be used for calculating profile size.
		* @param aProfileId a required profile id
		* @return Found profile, or NULL if not found. Ownership is not
		*		  transferred.
        */
		const CSIPConcreteProfile* Profile(TUint32 aProfileId) const;
		
		/**
        * Gets modifiable profile object pointer
		* This method should only be used when you need to update profile.
		* @param aProfileId a required profile id
		* @return Found profile, or NULL if not found. Ownership is not
		*		  transferred.
        */
		CSIPConcreteProfile* Profile(TUint32 aProfileId);
		
	    /**
        * Gets default profile
		* Ownership is not transferred.
		* This method should only be used for calculating profile size
		* @return profile
        */
		const CSIPConcreteProfile* ProfileDefaultL();
	
	    /**
        * Gets profile.
		* @param aProfileId a required profile id
		* @param aObserver client observer of the profile
		* @return Found profile
		* @leave KErrNotFound if profile was not found
        */
		CSIPConcreteProfile& ProfileL(
			TUint32 aProfileId,
			const MSIPExtendedConcreteProfileObserver& aObserver) const;
	
	    /**
        * Gets updated profile to refresh previously fetched data.
		* @param aProfileId a required profile id
		* @param aObserver client observer of the profile
		* @return Profile
        */
		CSIPConcreteProfile& ProfileRefreshL(
			TUint32 aProfileId,
			const MSIPExtendedConcreteProfileObserver& aObserver) const;

		/**
		* Creates new profile prefilled with default values.
		* Ownership is transferred.
		* @aParam aType type to match
		* @return new instance of CSIPConcreteProfile
		*/
		CSIPConcreteProfile* CreateProfileL(const TSIPProfileTypeInfo& aType);

	    /**
        * Gets array of all profiles
		* @param aObserver client observer of the profiles
		* @param aArray array to store profiles
        */
		void ProfilesL(const MSIPExtendedConcreteProfileObserver* aObserver,
			RPointerArray<CSIPConcreteProfile>& aArray) const;

		/**
        * Gets array of profiles with matching AOR
		* @param aAOR AOR to match
		* @param aObserver client observer of the profiles
		* @param aArray array to store profiles
        */
		void ProfilesByAORL(const TDesC8& aAOR,
			const MSIPExtendedConcreteProfileObserver* aObserver,
			RPointerArray<CSIPConcreteProfile>& aArray) const;

	    /**
        * Gets array of profiles with matching type
		* @param aType type to match
		* @param aObserver client observer of the profiles
		* @param aArray array to store matching profiles
        */
		void ProfilesByTypeL(const TSIPProfileTypeInfo& aType, 
			const MSIPExtendedConcreteProfileObserver* aObserver,
			RPointerArray<CSIPConcreteProfile>& aArray) const;

	    /**
        * Clears client observer from profile
		* @param aProfileId a required profile id
		* @param aObserver client observer of the profile
        */
		void DeleteProfileL(TUint32 aProfileId, 
			const MSIPExtendedConcreteProfileObserver& aObserver);

		/**
        * Checks if server can be stopped
		* @return ETrue, if there are no clients using the profiles
		*		  and there are not autoregistered profiles to be managed
        */
		TBool CanServerStop() const;

		/**
        * Checks if server is currently being backupped/restored
		* @return ETrue, if server is currently being backupped/restored
		*		  , EFalse otherwise
        */
		TBool BackupInProgress() const;

	    /**
        * Sends registration status event to observers of the profile
		* @param aItem holds profile and observers
        */
		void SendRegistrationStatusEventL(CSIPProfileCacheItem& aItem) const;

	    /**
        * Sends unregistered status event to observers of the profile
		* @param aItem holds profile and observers
        */
		void SendUnregisteredStatusEventL(CSIPProfileCacheItem& aItem) const;

	    /**
        * Sends error event to observers of the profile
		* @param aItem holds profile and observers
		* @param aStatus status indicating if error occurred
		*		  during registering (ERegistrationInProgress)
		*		  or deregistering (EUnregistrationInProgress)
		* @param aError error code
		* @return system wide error code on failure
        */
		TInt SendErrorEvent(CSIPProfileCacheItem& aItem,
							CSIPConcreteProfile::TStatus aStatus,
							TInt aError) const;
	
	    /**
        * Sends profile added event to all clients
		* @param aProfile added profile
        */
		void SendProfileAddedEvent(const CSIPConcreteProfile& aProfile) const;
	
	    /**
        * Sends profile update event to observers of the profile
		* @param aItem Holds the observers
		* @param aProfile Profile that has been updated
        */
		void SendProfileUpdatedEventL(const CSIPProfileCacheItem& aItem,
			const CSIPConcreteProfile& aProfile) const;

	    /**
        * Sends profile remove event to observers of the profile
		* @param aItem holds profile and observers
        */
		void SendProfileRemovedEventL(CSIPProfileCacheItem& aItem) const;		

		/**
		* Storage notifies that store file must be released. This
		* occurs during backup/restore process
		* aParam aRestoreOngoing ETrue if restore is ongoing,
		*						 EFalse if backup is ongoing
		*/
		void ReleaseStorage(TBool aRestoreOngoing);

		/**
		* Storage notifies that store file can again be reserved. This
		* occurs during backup/restore process
		* aParam aRestoreOngoing ETrue if restore is ongoing,
		*						 EFalse if backup is ongoing
		*/
		void ReserveStorage(TBool aRestoreOngoing);
		
		/**
		* Allows the migration from the old
		*  IAP to the new one.
		* @param aProfileId the SIP profile id
		* @param aIapId the new IAP id
		* @param aObserver Observer
		*/
		void AllowMigrationL(TUint32 aProfileId,
			TUint32 aIapId,
			const MSIPExtendedConcreteProfileObserver& aObserver);
		
		/**
		* Disallows the migration from the 
		* old IAP to the new one.
		* @param aProfileId the SIP profile id
		* @param aIapId the new IAP id.
		* @param aObserver Observer
		*/
		void DisallowMigrationL(TUint32 aProfileId,
			TUint32 aIapId,
			const MSIPExtendedConcreteProfileObserver& aObserver);

		/**
        * Refreshes the list of available IAPs for a profile.
		* @param aProfileId the SIP profile id
		*/
		void RefreshIapAvailabilityL(TUint32 aProfileId);

		/**
        * Migrating to an IAP has been initiated.
		* @param aItem Profile cache item
		* @param aSnapId SNAP id
		* @param aIapId IAP id
		*/
		void MigrationStartedL(const CSIPProfileCacheItem& aItem,
							   TUint32 aSnapId,
							   TUint32 aNewIapId) const;

		/**
        * Migrating to an IAP has been completed successfully.
		* @param aItem Profile cache item
		* @param aSnapId SNAP id
		* @param aIapId IAP id
		*/
		void MigrationCompletedL(const CSIPProfileCacheItem& aItem,
							     TUint32 aSnapId,
							     TUint32 aNewIapId) const;

		/**
        * Forward an ALR related error to client. These errors include etc.
        * "migration failed".
		* @param aItem Profile cache item
		* @param aError Error code.
		* @param aSnapId SNAP id
		* @param aIapId IAP id
		* @return system wide error code on failure
		*/
		TInt PassAlrErrorToClient(const CSIPProfileCacheItem& aItem,
								  TInt aError,
								  TUint32 aSnapId,
								  TUint32 aIapId) const;

		/**
        * Gets the migration controller of the given SNAP id. If a controller
        * does not exists, creates a new one.
		* @param aSnapData 
		* @return Migration controller, ownership is not transferred.
		*/
		CSipAlrMigrationController& MigrationControllerL(TSipSNAPConfigurationData aSnapData);

		CSIPProfileState& UnregisteredState();

		TUint32 GenerateProfileIdL();

		/**
		* Sends forcibly disable profile added event to all clients
		* @param aProfileId: Id of the profile being disabled forcibly 
		*/
		void SendProfileForciblyDisabledEvent(const CSIPProfileCacheItem& aItem) const;
		
    /**
    * Gets cached profile, leave if not found
    * ownership is not transfered
    * @param aProfileId id of profile. 
    * @return profile cache item 
    */
    CSIPProfileCacheItem* ProfileCacheItemL(TUint32 aProfileId) const;

		/**
		*Starts timer of type CDeltaTimer, 
		*which callback is ConnectionCloseTimerExpired function
		*/
		void StartConnectionCloseTimer(TInt aValue);
		
		/**
		* A callback for CDeltaTimer
		*/
		static TInt ConnectionCloseTimerExpired(TAny* aPtr);
		
		/**
		* Notify system state monitor about event processing completion
		*/
		void ConfirmSystemstateMonitor(
			CSipSystemStateMonitor::TSystemVariable aVariable);
			
		/*
         * Checks whether the Update can be performed when the profile
         * has IAP as modem bearer.
         */
        TBool IsUpdateAllowed( CSIPConcreteProfile *aProfile );

	private:

		/**
		* Default constructor
		* @param aServer server of the session
		*/
		CSIPProfileServerCore();

		/**
		* Symbian 2nd phase constructor
		*/
		void ConstructL();

		/**
    * Sends status event to observers of the profile
		* @param aItem holds profile and observers
    */
		void SendStatusEventL(CSIPProfileCacheItem& aItem, 
							  CSIPConcreteProfile::TStatus aStatus) const;

		/**
		* Loads profiles from store to cache, register
		* markes as autoregistration on.
		* Runned after server is started and after
		* store file is modified by external party
		* like backup system.
		*/
		void LoadProfilesL(TBool aNotifyProfileCreation);

		/**
		* Terminates usage of profiles and cleans cache.
		* Runned when external party has modified store.
		*/
		void TerminateProfilesL();

		/**
		* Gets cached profile, returns 0 if not found
		* ownership is not transfered
		* @param aProfileId id of profile. 
		* @return profile cache item 
		*/
		CSIPProfileCacheItem* ProfileCacheItem(TUint32 aProfileId) const;

		/**
		* Gets cached profile, returns 0 if not found
		* ownership is not transfered
		* @param aIndex, index of item in array 
		* @return profile cache item 
		*/
		CSIPProfileCacheItem* FindProfileCacheItem(TInt aIndex) const;

		/**
		* Checks if profile is cached
		* @param aProfileId id of profile. 
		* @return index of profile, KErrNotFound if not found 
		*/
		TInt ProfileCached(TUint32 aProfileId) const;

		/**
		* Searches the profile cache for an item with a mathing storage id.
		* @param aStorageId Storage id
		* @return Found item
		* @leave KErrNotFound If matching item was not found
		*/
		CSIPProfileCacheItem&
			ProfileCacheItemByStorageIdL(TUint32 aStorageId) const;

		/**
		* Searches the profile cache for an item with a mathing storage id.
		* @param aStorageId Storage id
		* @return Found item or NULL if not found. Ownership is not transferred.
		*/
		CSIPProfileCacheItem*
			ProfileCacheItemByStorageId(TUint32 aStorageId) const;

		/**
		* Registers profiles marked as auto-register in startup
		*/
		void RegisterProfiles();
		
		/**
		* Add a profile to array and check if the observer should be added to
		* the profile.
		* @param aObserver Observer
		* @param aArray Array where the profile is added
		* @param aItem Profile cache item containing the profile
		*/
		void HandleMatchingProfileL(
			const MSIPExtendedConcreteProfileObserver* aObserver,
			RPointerArray<CSIPConcreteProfile>& aArray,
			CSIPProfileCacheItem& aItem) const;

	    /**
        * Removes client observer, leaves on failure
		* @param aObserver observer to be removed
        */
		void SessionCleanupL(
			const MSIPExtendedConcreteProfileObserver& aObserver);

	    /**
        * Converts ECom plugin into profile type
		* @param aEcomInfo ECom plugin info
		* @param aInfo returned profile type info
        */
		void ConvertTypeInfoL(const CImplementationInformation& aEcomInfo,
							  TSIPProfileTypeInfo& aInfo) const;

	    /**
        * Checks if the server can stop, informs CS-server
		* if can be stopped;
        */
		void CheckServerStatus() const;

	    /**
        * Get status weather error occurred during registration
		* or deregistartion
		* @param aItem profile cache item to be validated
		* @return in progress status of cached profile
        */
		CSIPConcreteProfile::TStatus ValidateErrorStatus(
			CSIPProfileCacheItem& aItem) const;

		/**
		* Registration status event occurred.
		* @param aProfileId a profile id
		* @param aContextId a registration context identifier
		*/
		void SIPProfileStatusEventL(
			TUint32 aProfileId,
			TUint32 aContextId);

	    /**
        * Handles possible asynchronous errors from plugin manager
		* @param aItem profile item to be handled
		* @param aStatus status indicating if error occurred
		*		  during registering (ERegistrationInProgress)
		*		  or deregistering (EUnregistrationInProgress)
		* @param aError Error code to be handled
        */
		void HandleAsyncError(CSIPProfileCacheItem& aItem,
							  CSIPConcreteProfile::TStatus aStatus,
							  TInt aError);

		/**
		* Storage notifies that store file must be released. This
		* occurs during backup/restore process
		* aParam aRestoreOngoing ETrue if restore is ongoing,
		*						 EFalse if backup is ongoing
		*/
		void ReleaseStorageL(TBool aRestoreOngoing);

		/**
		* Storage notifies that store file can again be reserved. This
		* occurs during backup/restore process
		* aParam aRestoreOngoing ETrue if restore is ongoing,
		*						 EFalse if backup is ongoing
		*/
		void ReserveStorageL(TBool aRestoreOngoing);

	  /**
    * Cleans up array in case of failure
    * ownership of aArray is transferred
		* @param aArray array to be cleaned up
    */
		static void ResetAndDestroy(TAny* aArray);

	  /**
    * Cleans up array in case of failure
    * ownership of aArray is transferred
		* @param aArray array to be cleaned up
    */
		static void ResetAndDestroyInfo(TAny* aArray);

	  /**
    * Reverts back cache in case of failure      
		* @param aItem cache cleanup item
    */
		static void CrashRevert(TAny* aItem);
		
		/**
    * Handles the errors occured during the profile restore     
		* @param aErr
		* @param fileStore specifies the kind of the file on which 
		* the storage error has occured
    */
		void HandleProfileStorageErrorL(TInt aErr, TBool fileStore=EFalse);

		/**
        * Remove the cached profile entry identified by aProfileId.
		* @param aProfileId Profile id
        */
		void RemoveProfileItem(TUint32 aProfileId);

		/**
    * Removes unused migration controllers, except if it uses the specified
    * SNAP id.
		* @param aSnapId SNAP id
    */
		void RemoveUnusedMigrationControllers(TUint32 aSnapId);

		void LoadSystemStateMonitorL();

		/**
		* Gets the default profile
		* @return Pointer to the default profile, or NULL if not found.
		*/
		CSIPConcreteProfile* FindDefaultProfile() const;

    TBool ShouldChangeIap(CSIPConcreteProfile& aProfile, TInt aError) const;

    /**
    * @return ETrue if any registered profile is using aIap
    */
    TBool AnyRegisteredProfileUsesIap(TUint aIap) const;
        
    /**
    * Add profiles in Profile Cache
    * @param aProfiles Array of the profiles to be added
    * @param aNotifyProfileCreation specifies whether the notification
    * to be sent to the profile clients
    */
    void AddProfilesInCacheL(RPointerArray<CSIPConcreteProfile>& aProfiles,
    	TBool aNotifyProfileCreation);
        
		/**
		* Gets the IAP Count in the Snap
    * @return number of aIaps in the Snap
    */
    TInt IAPCountL(TUint32 aSnapId) const;
	
	 /*
         * Checks whether the current seetings are same as 
         * required seetings for Registeration.
         * @ returns ETrue when the seetings are correct
         *      EFalse otherwise
         */
        
        TBool IsRegistrationAllowedWithCurrentApnSettings( TUint32 aIapId );
        
        /**
         ** Selecting Initial Apn for the profile
         **/
        void SelectInitialApnL( const CSIPConcreteProfile& aProfile );
        
        /**
         * Checks whether the profile's IAP is switch enabled 
         * @ returns ETrue when the profile is APN switch enabled
         *      EFalse otherwise
         */
        TBool CheckApnSwitchEnabledL( const CSIPConcreteProfile& aProfile );
               
        /**
         * Checks the IAP settings 
         * @ returns ETrue when all the settings are correct
         *      EFalse otherwise 
         */
        TBool CheckIapSettings(TUint32 aProfileId);
        
        /**
         * Issues the change of APN to PrimaryAPN 
         */
        void UsePrimaryApn(TUint32 aIapId);
        
        /**
         * Issues the change of APN to SecondaryAPN
         */
        void UseBackupApn( TUint32 aIapId, TBool aFatalFailure = ETrue );
        
        /**
         * Compares the two plugin typr info
         * @returns ETrue if the match completely
         * EFalse otherwise
         */ 
        static TBool Compare(const TSIPProfileTypeInfo& first,
                        const TSIPProfileTypeInfo& second);
		
        
	private: // Data
	
		RFs										iFs;
		TBool									iBackupInProgress;

		// Owned
		CSIPProfileCSServer*					iServer;
		// Owned
		CSIPProfileStorage*						iProfileStorage;
		// Owned
		CSIPProfileStorageSecureBackup*			iNotify;

		// Array items are owned
		RPointerArray<CSIPProfileCacheItem>		iProfileCache;
		// Array items are not owned
		RPointerArray<MSIPExtendedConcreteProfileObserver>
												iObservers;

		// Owned
		CSIPProfilePlugins*						iProfilePlugins;
		// Owned
		CSIPPluginDirector*						iPluginDirector;
		// Owned
		CSIPProfileCacheItem*					iFindEntry;

		// Profile states. Owned.
		CSIPProfileStateUnregistered* 			iUnregistered;
		CSIPProfileStateRegistered* 			iRegistered;
		CSIPProfileStateUnregInProg* 			iUnregInProg;
		CSIPProfileStateRegInProg* 				iRegInProg;
		CSIPProfileStateWaitForIAP* 			iWaitForIAP;
		CSIPProfileStateWaitForPermission* 		iWaitForPermission;
		CSIPProfileStateMigratingToNewIAP*		iMigratingToNewIAP;
    	CSIPProfileStateUnregisteringOldIAP*	iUnregisteringOldIAP;

		// Owned
		CSipAlrHandler* 						iAlrHandler;

		// Migration controllers. Array items are owned.
		RPointerArray<CSipAlrMigrationController> iMigrationControllers;

		// Owned
		CSipSystemStateMonitor*				iSystemStateMonitor;
		//ApnManager
		CSIPApnManager*                     iApnManager; // iBackupApnSettings;
		RArray<TStoreSwitchEnabledProfile>                iWaitForApnSettings;
		
		TBool									iFeatMgrInitialized;
		
		CDeltaTimer* 						iDeltaTimer;
		TCallBack 							iDeltaTimerCallBack;
		TDeltaTimerEntry 					iDeltaTimerEntry;

	private: // For testing purposes
#ifdef CPPUNIT_TEST
		friend class CSIPProfileServerCoreTest;
		friend class CSIPProfileCSSessionTest;
		friend class CSIPProfileCSServerTest;
		friend class CSIPProfileStateTest;
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

#endif // CSIPPROFILESERVERCORE_H
