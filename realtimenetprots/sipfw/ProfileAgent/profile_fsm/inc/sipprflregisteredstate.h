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
* Name        : sipprflregisteredstate.h
* Part of     : sip profile fsm
* Version     : %version: 2.1.1 %
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPRFLREGISTEREDSTATE_H
#define CSIPPRFLREGISTEREDSTATE_H

//  INCLUDES
#include "sipprflstatebase.h"

// FORWARD DECLARATIONS
class CSIPConcreteProfile;

// CLASS DECLARATION
/**
* Reacts to events that are possible in 
* "Registered" state. Profile is in this state 
* when it is registered to the SIP server.
*/
class CSIPPrflRegisteredState: public CSIPPrflStateBase
	{
	public:// Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aUser user for the state machine
        */
		IMPORT_C static CSIPPrflRegisteredState* NewL(
		    MSIPProfileFSMUser& aUser);

		/**
		* Destructor
		*/
		IMPORT_C ~CSIPPrflRegisteredState();

	public://new functions
		/**
		* Links states
		* @param aDeregisterState "Deregister Requested" state
		* @param aDeregistrationInProgressState "Deregistration in Progress" state
		* @param aInitState "Init" state
		* @param aRegisterRequested "Register Requested" state
		* @param aRegisterDelayRequested "Register Delay Requested" state
		*/
		IMPORT_C void LinkStates(
			CSIPPrflStateBase& aDeregisterState,
			CSIPPrflStateBase& aDeregistrationInProgressState,
			CSIPPrflStateBase& aInitState,
			CSIPPrflStateBase& aRegisterRequested,
			CSIPPrflStateBase& aRegisterDelayRequested);

	public://functions from base class
	
	    CSIPConcreteProfile::TStatus ConcreteProfileState() const;
	
		/**
		* Deregisters the profile. Moves profile
		* to "Deregister Requested" state in case of suspended connection or 
		* to "Deregistration in Progress" in case of active connection.
		* according to the connection state.s
		* @param aContext a profile context
		*/
		void DeregisterL(
			MSIPProfileContext& aContext);

		/**
		* Updates the profile. Invokes profile deregistration
		* if so required. 
		* Function will leave with KErrArgument error code if profiles 
		* are the same.
		* @param aContext a profile context
		* @param aNewProfile new profile
		*/
		void UpdateL(
			MSIPProfileContext& aContext,
			CSIPConcreteProfile& aNewProfile);
		/**
		* An asynchronous error has occured related to a periodical refresh 
		* that relates to a registration. Moves to the "Register Requested"
		* state in case of dropped registration during suspended connection or
		* In case of UnAvailable connection report error to client and move profile to "Init" state 
		* @param aContext a profile context
		* @param aError error code
		*/
		void ErrorOccured(
			MSIPProfileContext& aContext,
			TInt aError);
		/**
		* Connection state has changed. If connection
		* state changes to "Inactive" moves profile to
		* "Register Requested" state.
		* @param aContext a profile context
		* @param aState indicates the current connection state
		*/
		void ConnectionStateChanged(
			MSIPProfileContext& aContext,
			CSIPConnection::TState aState);

	private:
	    /**
		* Constructor.
		*/
		CSIPPrflRegisteredState(MSIPProfileFSMUser& aUser);

	private: // Data

		CSIPPrflStateBase* iDeregisterState;
		CSIPPrflStateBase* iDeregistrationInProgressState;
		CSIPPrflStateBase* iInitState;
		CSIPPrflStateBase* iRegisterRequestedState;
		CSIPPrflStateBase* iRegisterDelayRequestedState;
	};

#endif // CSIPPRFLREGISTEREDSTATE_H

