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
* Name        : sipprofilestateunregistred.h
* Part of     : Sip Profile Server
* Interface   : private
* Class provides functionality for registration state automation
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILERESTATEUNREGISTERED_H
#define CSIPPROFILERESTATEUNREGISTERED_H

// INCLUDES
#include <e32base.h>
#include "sip.h"
#include "SipProfileState.h"
#include "sipconcreteprofile.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Class provides functionality for profile state automation
*  Unregistered state.
*  @lib 
*/

class CSIPProfileStateUnregistered: public CSIPProfileState
	{

   public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSIPProfileStateUnregistered* NewL(CSIPPluginDirector& aDirector);
		

        /**
        * Destructor.
        */
        virtual ~CSIPProfileStateUnregistered();

    public: // From CSIPProfileState

		CSIPConcreteProfile::TStatus Name() const;

		void EnterL(CSIPProfileCacheItem& aItem,
					TBool aMaySendRegStatusEvent,
					CSIPProfileServerCore& aServerCore);

		void RegisterL(CSIPProfileCacheItem& aItem);

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

		TBool CanBePermanentlyRemoved(
			const CSIPProfileCacheItem& aItem) const;		

 		void RegistrationStatusEventL(
			CSIPProfileCacheItem& aItem,
			CSIPConcreteProfile::TStatus aStatus);

		TBool ErrorOccurred(CSIPProfileCacheItem& aItem, TInt aError);

		void IapAvailableL(CSIPProfileCacheItem& aItem,
				           TUint32 aSnapId,
					       TUint32 aNewIapId);

	public: // New functions

		/**
        * Sets next automaton states.
        * @param aRegInProg registration in progress state
        * @param aWaitForIAP Wait for IAP state
        */
		void SetNeighbourStates(CSIPProfileState& aRegInProg,
								CSIPProfileState& aWaitForIAP);

	private:

        /**
        * Constructor.
        */
        CSIPProfileStateUnregistered(CSIPPluginDirector& aDirector);

	private: // Data

		// Neightbour states, not owned
		CSIPProfileState* iRegInProg;
		CSIPProfileState* iWaitForIAP;
	};

#endif

