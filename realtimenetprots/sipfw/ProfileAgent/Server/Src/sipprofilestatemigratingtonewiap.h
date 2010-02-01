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
* Name        : sipprofilestatemigratingtonewiap.h
* Part of     : Sip Profile Server
* Interface   : private
* Class provides functionality for registration state automation
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILERESTATEMIGRATINGTONEWIAP_H
#define CSIPPROFILERESTATEMIGRATINGTONEWIAP_H

//  INCLUDES
#include "sip.h"
#include "SipProfileState.h"
#include "sipconcreteprofile.h"
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Class provides functionality for profile state automation
*  "Migrating to new IAP" state.
*  @lib 
*/
class CSIPProfileStateMigratingToNewIAP: public CSIPProfileState
	{
   	public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSIPProfileStateMigratingToNewIAP*
        	NewL(CSIPPluginDirector& aDirector);

        /**
        * Destructor.
        */
        virtual ~CSIPProfileStateMigratingToNewIAP();

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

		TBool IsAlrState() const;

		void ShutdownInitiated(CSIPProfileCacheItem& aItem);

    public: // New functions
	
        /**
        * Sets next automaton states, ownership is not transferred
        * @param aUnregisteringOldIAP Unregistering old IAP state
        * @param aWaitForIAP Wait for IAP state
        * @param aRegistered Registered state
        * @param aUnregInProg Unregistration in progress state
        * @param aUnregistered Unregistered state
        */
		void SetNeighbourStates(CSIPProfileState& aUnregisteringOldIAP,
								CSIPProfileState& aWaitForIAP,
							    CSIPProfileState& aRegistered,
							    CSIPProfileState& aUnregInProg,
							    CSIPProfileState& aUnregistered); 		

   private:

        /**
        * Constructor
        */
        CSIPProfileStateMigratingToNewIAP(CSIPPluginDirector& aDirector);

	private: // Data

		// Neighbour states, not owned
		CSIPProfileState* iUnregisteringOldIAP;
		CSIPProfileState* iWaitForIAP;
		CSIPProfileState* iRegistered;
		CSIPProfileState* iUnregInProg;
		CSIPProfileState* iUnregistered;
	};

#endif // CSIPPROFILERESTATEMIGRATINGTONEWIAP_H
