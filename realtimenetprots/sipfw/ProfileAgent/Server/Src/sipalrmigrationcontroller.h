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
* Name          : sipalrmigrationcontroller.h
* Part of       : SIP / SIP Profile Server
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPALRMIGRATIONCONTROLLER_H
#define CSIPALRMIGRATIONCONTROLLER_H

// INCLUDES
#include "sipalrobserver.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class CSipAlrMonitor;
class MSipAlrMigrationObserver;
class TSipAlrMigrationObserverInfo;


// CLASS DECLARATION
/**
* Combines the migration permissions of all profiles using the same SNAP id.
*
* @lib sipsrofilesrv.exe
*/
class CSipAlrMigrationController :
	public CBase,
	public MSipAlrObserver
	{
	public: // Enumerations

        enum TIapId
            {
            // The most recent IAP id offered to MSipAlrMigrationObserver
            EMostRecentIap = 0
            };

	public:
    	static CSipAlrMigrationController* NewLC(CSipAlrMonitor& aAlrMonitor,
                             TSipSNAPConfigurationData aSnapData);

    	~CSipAlrMigrationController();

	public: // From MSipAlrObserver

		void AlrEvent(MSipAlrObserver::TEvent aEvent,
					  TUint32 aSnapId,
					  TUint32 aIapId);

    public: // New functions

		TUint32 SnapId() const;
		
		TUint32 BearerId() const;

		/**
        * Check if any profile is using this instance.
        * @return ETrue At least one profile is using this migration controller
        *		  EFalse otherwise
        */
		TBool IsUsed() const;

		/**
        * A profile starts to listen for IAP notifications of the SNAP.
        * @param aObserver Observer to attach
        * @return Initial IAP id to use, or zero of no IAP is available yet
        */
        TUint32 AttachProfileL(MSipAlrMigrationObserver& aObserver);

		/**
        * A profile quits listening IAP notifications of the SNAP.
        * @param aObserver Observer to detach
        */
		void DetachProfile(MSipAlrMigrationObserver& aObserver);

		/**
        * A new IAP became available.
        * @param aSnapId SNAP id
        * @param aNewIap IAP id
        */
		void IapAvailable(TUint32 aSnapId, TUint32 aNewIapId);

		/**
        * Profile cache item has formed its decision regarding the migration.
        * @param aObserver Observer who is now giving its answer
        * @param aAllow ETrue if this observer allows migration,
        *				EFalse otherwise
        * @param aIapId IAP id
        */
		void SetMigrationPermission(MSipAlrMigrationObserver& aObserver,
									TBool aAllow,
									TUint32 aIapId = EMostRecentIap);

		void RefreshIapAvailabilityL(TUint32 aSnapId);

		TInt SetIapAcceptance(MSipAlrMigrationObserver& aObserver,
							  TBool aIapWorks);
							  
		void ResetFlags();
		
		

	private: // Constructors

	    CSipAlrMigrationController(CSipAlrMonitor& aAlrMonitor,
                                    TSipSNAPConfigurationData aSnapData);

    private: // New functions

		void SendMigrationResult(TUint32 aIapId, TBool aAllowed);

		TInt FindIndex(MSipAlrMigrationObserver& aObserver) const;

		TBool ObserversWaitingPermission() const;

		TBool ObserversWaitingAcceptance() const;
		
		void NoNewIapAvailable() const;
		
		void OfferedIapRejected() const;

    private: // Data

		// Not owned
    	CSipAlrMonitor& iAlrMonitor;

		RArray<TSipAlrMigrationObserverInfo> iObservers;

		// The IAP id, that was most recently offered to observers with
		// MSipAlrMigrationObserver::IapAvailableL.
		TUint32 iOfferedIapId;

		// Most recent IAP id, where the migration was allowed.
		TUint32 iAllowedIapId;

		// Is the migration to iOfferedIapId disallowed.
		TBool iMigrationDisallowed;

		// Used to prevent refreshing the IAP availability when it has already
		// been done.
		// ETrue if CSipAlrMonitor has been requested to refresh the IAP
		// availability.
		// Will be set to EFalse, when ALR monitor notifies about IAP, using
		// MSipAlrObserver::IapAvailable.
		TBool iRefreshIssued;
		
		// This contains the SNAP ID and the Bearer ID configured for the SNAP.
		TSipSNAPConfigurationData iSnapData;

    private: // For testing purposes		
#ifdef CPPUNIT_TEST
		friend class CSipAlrMigrationControllerTest;    
        friend class CSIPProfileAlrTest;
        friend class CSIPProfileStateWaitForPermissionTest;
        friend class CSIPProfileCacheItemTest;
#endif
    };

#endif // CSIPALRMIGRATIONCONTROLLER_H
