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
* Name        : sipprofilestateunregisteringoldiap.h
* Part of     : Sip Profile Server
* Interface   : private
* Class provides functionality for registration state automaton
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILERESTATEUNREGISTERINGOLDIAP_H
#define CSIPPROFILERESTATEUNREGISTERINGOLDIAP_H

//  INCLUDES
#include "sip.h"
#include "SipProfileState.h"
#include "sipconcreteprofile.h"
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Class provides functionality for profile state automaton
*  "Unregistering old IAP" state.
*  @lib 
*/
class CSIPProfileStateUnregisteringOldIAP: public CSIPProfileState
	{
   	public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSIPProfileStateUnregisteringOldIAP*
        	NewL(CSIPPluginDirector& aDirector);

        /**
        * Destructor.
        */
        virtual ~CSIPProfileStateUnregisteringOldIAP();

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

		TBool IsAlrState() const;

		void ShutdownInitiated(CSIPProfileCacheItem& aItem);

    public: // New functions
	
        /**
        * Sets next automaton states, ownership is not transferred
        * @param aRegistered Registered state
        * @param aUnregInProg Unregister in progress state
        */
		void SetNeighbourStates(CSIPProfileState& aRegistered,
								CSIPProfileState& aUnregInProg);

   private:

        /**
        * Constructor
        */
        CSIPProfileStateUnregisteringOldIAP(CSIPPluginDirector& aDirector);

	private: // Data

		// Neighbour states, not owned
		CSIPProfileState* iRegistered;
		CSIPProfileState* iUnregInProg;
	};

#endif // CSIPPROFILERESTATEUNREGISTERINGOLDIAP_H
