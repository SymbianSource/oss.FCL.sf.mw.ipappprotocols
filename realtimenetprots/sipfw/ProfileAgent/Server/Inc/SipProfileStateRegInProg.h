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
* Name        : sipprofilestatereginprog.h
* Part of     : Sip Profile Server
* Interface   : private
* Class provides functionality for registration state automation
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILERESTATEREGINPROG_H
#define CSIPPROFILERESTATEREGINPROG_H

// INCLUDES
#include "SipProfileState.h"
#include "sipconcreteprofile.h"
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Class provides functionality for profile state automation
*  Registration in progress state.
*  @lib 
*/
class CSIPProfileStateRegInProg: public CSIPProfileState
	{

   public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSIPProfileStateRegInProg* NewL(CSIPPluginDirector& aDirector);

        /**
        * Destructor.
        */
        virtual ~CSIPProfileStateRegInProg();

    public: // From CSIPProfileState

		CSIPConcreteProfile::TStatus Name() const;		

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

 		void RegistrationStatusEventL(
			CSIPProfileCacheItem& aItem,
			CSIPConcreteProfile::TStatus aStatus);

		TBool ErrorOccurred(CSIPProfileCacheItem& aItem, TInt aError);

		void IapAvailableL(CSIPProfileCacheItem& aItem,
					       TUint32 aSnapId,
						   TUint32 aNewIapId);
						   
		// Newly added to handle unregistration request in
		// RegInProg state.
		void ShutdownInitiated( CSIPProfileCacheItem& aItem );

	public: // New functions

		/**
        * Sets next automaton states.
        * @param aRegistered registered state
        * @param aUnregistered unregistered state
        * @param aUnregInProg unregistration in progress state
        * @param aWaitForIAP wait for IAP state
        * @param aWaitForPermission wait for permission state
        */
		void SetNeighbourStates(CSIPProfileState& aRegistered,
							    CSIPProfileState& aUnregistered,
							    CSIPProfileState& aUnregInProg,
							    CSIPProfileState& aWaitForIAP,
							    CSIPProfileState& aWaitForPermission);
	private:

        /**
        * Constructor.
        */
        CSIPProfileStateRegInProg(CSIPPluginDirector& aDirector);

	private: // Data

		// Neighbour states, not owned
		CSIPProfileState* iRegistered;
		CSIPProfileState* iUnregistered;
		CSIPProfileState* iUnregInProg;
		CSIPProfileState* iWaitForIAP;
		CSIPProfileState* iWaitForPermission;
	};

#endif
