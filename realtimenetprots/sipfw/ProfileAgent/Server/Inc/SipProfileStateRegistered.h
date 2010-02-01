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
* Name        : sipprofilestateregistred.h
* Part of     : Sip Profile Server
* Interface   : private
* Class provides functionality for registration state automation
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILERESTATEREGISTERED_H
#define CSIPPROFILERESTATEREGISTERED_H

// INCLUDES
#include "SipProfileState.h"
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Class provides functionality for profile state automation
*  Registered state.
*  @lib 
*/

class CSIPProfileStateRegistered: public CSIPProfileState
	{
	public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSIPProfileStateRegistered* NewL(CSIPPluginDirector& aDirector);

        /**
        * Destructor.
        */
        virtual ~CSIPProfileStateRegistered();

    public: // From CSIPProfileState
        
		CSIPConcreteProfile::TStatus Name() const;

		void EnterL(CSIPProfileCacheItem& aItem,
					TBool aMaySendRegStatusEvent,
					CSIPProfileServerCore& aServerCore);

		void UpdateRegistrationL(
			CSIPProfileCacheItem& aItem,
			const MSIPExtendedConcreteProfileObserver& aObserver);

		void EnableL(
			CSIPProfileCacheItem& aItem,
			const MSIPExtendedConcreteProfileObserver& aObserver);

		void EnableSnapInUseL(
			CSIPProfileCacheItem& aItem,
			const MSIPExtendedConcreteProfileObserver& aObserver,
			TUint32 aSnapId);

		void DisableL(
			CSIPProfileCacheItem& aItem,
			const MSIPExtendedConcreteProfileObserver& aObserver);        

		void RemoveL(CSIPProfileCacheItem& aItem);

 		void RegistrationStatusEventL(
			CSIPProfileCacheItem& aItem,
			CSIPConcreteProfile::TStatus aStatus);

		TBool ErrorOccurred(CSIPProfileCacheItem& aItem, TInt aError);

		void IapAvailableL(CSIPProfileCacheItem& aItem,
					       TUint32 aSnapId,
						   TUint32 aNewIapId);

		TBool ShouldRefreshIAPs(CSIPProfileCacheItem& aItem) const;

		void ShutdownInitiated(CSIPProfileCacheItem& aItem);

	public: // New functions

		/**
        * Sets next automaton states.
        * @param aUnregInProg unregistering in progress state
        * @param aUnregistered unregistred state
        * @param aRegInProg Registering in progress state
        * @param aWaitForPermission Wait for IAP migration permission state
        */
		void SetNeighbourStates(CSIPProfileState& aUnregInProg,
								CSIPProfileState& aUnregistered,
								CSIPProfileState& aRegInProg,
								CSIPProfileState& aWaitForPermission);

	private:

        /**
        * Constructor.
        */
        CSIPProfileStateRegistered(CSIPPluginDirector& aDirector);

	private: // Data

		// Neighbour states, not owned
		CSIPProfileState* iUnregInProg;
		CSIPProfileState* iUnregistered;
		CSIPProfileState* iRegInProg;
		CSIPProfileState* iWaitForPermission;
	};

#endif // CSIPPROFILERESTATEREGISTERED_H
