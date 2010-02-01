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
* Name        : sipprflregistrationinprogressstate.h
* Part of     : sip profile fsm 
* Version     : %version: 2.1.1 %
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPRFLREGISTRATIONINPROGRESSSTATE_H
#define CSIPPRFLREGISTRATIONINPROGRESSSTATE_H

//  INCLUDES
#include "sipprflstatebase.h"

// FORWARD DECLARATIONS
class CSIPConcreteProfile;

// CLASS DECLARATION
/**
* Reacts to events that are possible in "Registration in Progress" state. 
* Profile is in this state when 
* the REGISTER has been sent to the remote SIP server.
*/
class CSIPPrflRegistrationInProgressState: public CSIPPrflStateBase
	{
	public:// Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aUser user for the state machine
        */
		IMPORT_C static CSIPPrflRegistrationInProgressState* NewL(
		    MSIPProfileFSMUser& aUser);

		/**
		* Destructor
		*/
		IMPORT_C ~CSIPPrflRegistrationInProgressState();

	public://new functions
	
	    CSIPConcreteProfile::TStatus ConcreteProfileState() const;
	
		/**
		* Links states
		* @param aRegister Requested "Register Requested" state
		* @param aInitState "Init" state
		* @param aRegisteredState "Registered" state
		*/
		IMPORT_C void LinkStates(
			CSIPPrflStateBase& aRegisterRequested,
			CSIPPrflStateBase& aInitState,
			CSIPPrflStateBase& aRegisteredState,
			CSIPPrflStateBase& aRegisterDelayRequestedState);

		/**
		* A SIP response creating a registration binding or an error response 
		* that is related to an refreshed registration binding  
		* has been received from the network.
		* In case of suspended connection and error response the profile is moved
		* to "Register Requested" state.
		* In case of active connection and >=300 response the profile is moved
		* to "Init" state.
		* In case of active connection and 2xx response the profile is moved
		* to "Registered" state.
		* @param aTransaction contains response elements.
		*/
		void ResponseReceived(
			MSIPProfileContext& aContext,
			CSIPClientTransaction& aTransaction);

		/**
		* An asynchronous error has occurred in the stack related
		* to the request indicated by the given transaction.
		* The profile is moved to "Init" state.
		* @param aContext a profile context
		* @param aError error code
		*/
		void ErrorOccured(
			MSIPProfileContext& aContext,
			TInt aError);

		/**
		* Connection state has changed.
		* In case of inactive connection the profile is moved to "Register
		* Requested" state Or
		* In case of UnAvailable connection report error to client and move profile to "Init" state 
		* @param aContext a profile context
		* @param aState indicates the current connection state
		*/
		void ConnectionStateChanged(
			MSIPProfileContext& aContext,
			CSIPConnection::TState aState);
			
		/**
		* In this state register is always pending.
		* @returns ETrue
		*/
		TBool IsRegisterPending() const;		

	private:
	    /**
		* Constructor.
		*/
		CSIPPrflRegistrationInProgressState(MSIPProfileFSMUser& aUser);

	private: // Data
	
		CSIPPrflStateBase* iRegisterRequestedState;
		CSIPPrflStateBase* iInitState;
		CSIPPrflStateBase* iRegisteredState;
		CSIPPrflStateBase* iRegisterDelayRequestedState;
	};

#endif // CSIPPRFLREGISTRATIONINPROGRESSSTATE_H
