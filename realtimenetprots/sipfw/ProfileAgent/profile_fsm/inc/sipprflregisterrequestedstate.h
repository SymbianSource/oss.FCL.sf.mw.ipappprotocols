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
* Name        : sipprflregisterrequestedstate.h
* Part of     : sip profile fsm
* Version     : %version: 2.1.1 %
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPRFLREGISTERREQUESTEDSTATE_H
#define CSIPPRFLREGISTERREQUESTEDSTATE_H

//  INCLUDES
#include "sipprflstatebase.h"


// FORWARD DECLARATIONS
class MSIPProfileContext;

// CLASS DECLARATION
/**
* Reacts to events that are possible in 
* "Register Requested" state. Profile is in this state 
* when the registration is requested but connection state
* is suspended.
*/
class CSIPPrflRegisterRequestedState: public CSIPPrflStateBase
	{
	public:// Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aUser user for the state machine
        */
		IMPORT_C static CSIPPrflRegisterRequestedState* NewL(
		    MSIPProfileFSMUser& aUser);

		/**
		* Destructor
		*/
		IMPORT_C ~CSIPPrflRegisterRequestedState();

	public://new functions
		/**
		* Links states
		* @param aInitState "Init"-state
		* @param aResolvingProxiesState "Resolving Proxies"-state
		* @param aRegistrationInProgressState "Registration In Progress"-state
		*/

		IMPORT_C void LinkStates(
		    CSIPPrflStateBase& aInitState,
		    CSIPPrflStateBase& aResolvingProxiesState,
		    CSIPPrflStateBase& aRegistrationInProgressState);

	public://functions from base class
	
	    CSIPConcreteProfile::TStatus ConcreteProfileState() const;
	
		/**
		* Deregisters the profile. Moves profile to 
		* "Init" state.
		* @param aContext a profile context
		*/
		void DeregisterL(
			MSIPProfileContext& aContext);

		/**
		* Connection state has changed. Moves profile
		* to "Registration In Progress" or "Init" state according
		* to the connection state Or
		* In case of UnAvailable connection report error to client and move profile to "Init" state 
		* @param aContext a profile context
		* @param aState indicates the current connection state
		*/
		void ConnectionStateChanged(
			MSIPProfileContext& aContext,
			CSIPConnection::TState aState);	

		/**
		* Refreshes the connection.
		* @param aContext a profile context
		*/		
		void RegisterL(
					MSIPProfileContext& aContext);

	private:

		CSIPPrflRegisterRequestedState(MSIPProfileFSMUser& aUser);

	private: // Data

		CSIPPrflStateBase* iInit;
		CSIPPrflStateBase* iResolvingProxies;
        CSIPPrflStateBase* iRegistrationInProgress;
	};

#endif // CSIPPRFLREGISTERREQUESTEDSTATE_H

