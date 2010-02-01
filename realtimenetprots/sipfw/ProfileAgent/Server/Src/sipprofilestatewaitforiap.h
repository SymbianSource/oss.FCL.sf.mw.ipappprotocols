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
* Name        : sipprofilestatewaitforiap.h
* Part of     : Sip Profile Server
* Interface   : private
* Class provides functionality for registration state automation
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILERESTATEWAITFORIAP_H
#define CSIPPROFILERESTATEWAITFORIAP_H

//  INCLUDES
#include "sip.h"
#include "SipProfileState.h"
#include "sipconcreteprofile.h"
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Class provides functionality for profile state automation
*  "Wait for IAP" state.
*  @lib 
*/
class CSIPProfileStateWaitForIAP: public CSIPProfileState
	{
   	public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSIPProfileStateWaitForIAP* NewL(CSIPPluginDirector& aDirector);

        /**
        * Destructor.
        */
        virtual ~CSIPProfileStateWaitForIAP();

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
		
		TBool ShouldRefreshIAPs(CSIPProfileCacheItem& aItem) const;

		TBool IsAlrState() const;
		
		void NoNewIapAvailable(CSIPProfileCacheItem& aItem);
		
		TBool CanBePermanentlyRemoved(const CSIPProfileCacheItem& aItem) const;

    public: // New functions
	
        /**
        * Sets next automaton states, ownership is not transferred
        * @param aWaitForPermission Wait for IAP migration permission state
        * @param aRegInProg Registration in progress state
        * @param aUnregInProg Unregistration in progress state
        * @param aUnregistered Unregistred state
        */
		void SetNeighbourStates(CSIPProfileState& aWaitForPermission,
								CSIPProfileState& aRegInProg,
								CSIPProfileState& aUnregInProg,
							    CSIPProfileState& aUnregistered); 		

   private:

        /**
        * Constructor
        */
        CSIPProfileStateWaitForIAP(CSIPPluginDirector& aDirector);

	private: // Data

		// Neighbour states, not owned
		CSIPProfileState* iWaitForPermission;
		CSIPProfileState* iRegInProg;
		CSIPProfileState* iUnregInProg;
		CSIPProfileState* iUnregistered;
	};

#endif // CSIPPROFILERESTATEWAITFORIAP_H
