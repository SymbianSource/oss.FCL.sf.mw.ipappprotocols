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
* Name        : sipprofilestateunreginprog.h
* Part of     : Sip Profile Server
* Interface   : private
* Class provides functionality for registration state automation
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILERESTATEUNREGINPROG_H
#define CSIPPROFILERESTATEUNREGINPROG_H

// INCLUDES
#include <e32base.h>
#include <ecom/implementationinformation.h>
#include "sip.h"
#include "SipProfileState.h"
#include "sipconcreteprofile.h"

//FORWARD DECLARATIONS

class CSIPProfileCacheItem;

// CLASS DECLARATION

/**
*  Class provides functionality for profile state automation
*  Unregistration in progress state.
*  @lib 
*/
class CSIPProfileStateUnregInProg: public CSIPProfileState
	{
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSIPProfileStateUnregInProg* NewL(CSIPPluginDirector& aDirector);

        /**
        * Destructor.
        */
        virtual ~CSIPProfileStateUnregInProg();

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

	public: // New functions

		/**
        * Sets next automaton states.
        * @param aUnregistered Unregistered state
        * @param aUnregistered Registered state
        */
		void SetNeighbourStates(CSIPProfileState& aUnregistered,
								CSIPProfileState& aRegistered);

   private:

        /**
        * Constructor.
        */
        CSIPProfileStateUnregInProg(CSIPPluginDirector& aDirector);

	private:

		CSIPProfileState* iUnregistered;
		CSIPProfileState* iRegistered;
	};

#endif

