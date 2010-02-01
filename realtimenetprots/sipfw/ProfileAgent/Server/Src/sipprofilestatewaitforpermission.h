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
* Name        : sipprofilestatewaitforpermission.h
* Part of     : Sip Profile Server
* Interface   : private
* Class provides functionality for registration state automation
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILERESTATEWAITFORPERMISSION_H
#define CSIPPROFILERESTATEWAITFORPERMISSION_H

//  INCLUDES
#include "sip.h"
#include "SipProfileState.h"
#include "sipconcreteprofile.h"
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Class provides functionality for profile state automation
*  "Wait for IAP migration permission" state.
*  @lib 
*/
class CSIPProfileStateWaitForPermission: public CSIPProfileState
	{
   	public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSIPProfileStateWaitForPermission*
        	NewL(CSIPPluginDirector& aDirector);

        /**
        * Destructor.
        */
        virtual ~CSIPProfileStateWaitForPermission();

	public: // From CSIPProfileState

		CSIPConcreteProfile::TStatus Name() const;

		void EnableL(CSIPProfileCacheItem& aItem,
					 const MSIPExtendedConcreteProfileObserver& aObserver);

		void EnableSnapInUseL(CSIPProfileCacheItem& aItem,
			const MSIPExtendedConcreteProfileObserver& aObserver,
			TUint32 aSnapId);

		void DisableL(CSIPProfileCacheItem& aItem,
					  const MSIPExtendedConcreteProfileObserver& aObserver);

		void RegistrationStatusEventL(
			CSIPProfileCacheItem& aItem,
			CSIPConcreteProfile::TStatus aStatus);

		TBool ErrorOccurred(CSIPProfileCacheItem& aItem, TInt aError);

		void IapAvailableL(CSIPProfileCacheItem& aItem,
					       TUint32 aSnapId,
						   TUint32 aNewIapId);

		void ClientAllowsMigrationL(CSIPProfileCacheItem& aItem,
			TUint32 aIapId,
			const MSIPExtendedConcreteProfileObserver& aObserver);

		void ClientDisallowsMigrationL(CSIPProfileCacheItem& aItem,
			TUint32 aIapId,
			const MSIPExtendedConcreteProfileObserver& aObserver);

		void MigrationIsAllowedL(CSIPProfileCacheItem& aItem, TUint32 aIapId);

		void MigrationIsDisallowedL(CSIPProfileCacheItem& aItem,
									TUint32 aIapId);

		TBool IsAlrState() const;

		void ShutdownInitiated(CSIPProfileCacheItem& aItem);

    public: // New functions
	
        /**
        * Sets next automaton states, ownership is not transferred
        * @param aWaitForIAP Wait for IAP state
        * @param aMigratingToNewIAP Migrating to new IAP state
        * @param aRegInProg Registration in progress state
        * @param aRegistered Registered state
        * @param aUnregInProg Unregistration in progress state
        * @param aUnregistered Unregistered state
        */
		void SetNeighbourStates(CSIPProfileState& aWaitForIAP,
								CSIPProfileState& aMigratingToNewIAP,
								CSIPProfileState& aRegInProg,
							    CSIPProfileState& aRegistered,
							    CSIPProfileState& aUnregInProg,
							    CSIPProfileState& aUnregistered); 		

   private:

        /**
        * Constructor
        */
        CSIPProfileStateWaitForPermission(CSIPPluginDirector& aDirector);		

		void SetClientPermissionL(CSIPProfileCacheItem& aItem,
			TUint32 aIapId,
			const MSIPExtendedConcreteProfileObserver& aObserver,
			TBool aAllowMigration) const;

	private: // Data

		// Neighbour states, not owned
		CSIPProfileState* iWaitForIAP;
		CSIPProfileState* iMigratingToNewIAP;
		CSIPProfileState* iRegInProg;
		CSIPProfileState* iRegistered;
		CSIPProfileState* iUnregInProg;
		CSIPProfileState* iUnregistered;
		
	private: // For testing purposes
#ifdef CPPUNIT_TEST
		friend class CSIPProfileStateWaitForPermissionTest;
#endif
	};

#endif // CSIPPROFILERESTATEWAITFORPERMISSION_H
