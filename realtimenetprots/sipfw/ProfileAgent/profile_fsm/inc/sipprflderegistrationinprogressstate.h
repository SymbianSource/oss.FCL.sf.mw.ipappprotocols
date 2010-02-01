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
* Name        : sipprflderegistrationinprogressstate.h
* Part of     : sip profile fsm 
* Version     : %version: 2.1.1 %
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPRFLDEREGISTERATIONINPROGRESSSTATE_H
#define CSIPPRFLDEREGISTERATIONINPROGRESSSTATE_H

//  INCLUDES
#include "sipprflstatebase.h"

// FORWARD DECLARATIONS
class CSIPConcreteProfile;

// CLASS DECLARATION
/**
* Reacts to events that are possible in 
* "Deregistration in Progress" state. Profile is in this 
* state if (de)REGISTER was successfully sent to the
* remote SIP server.
*/
class CSIPPrflDeregistrationInProgressState: public CSIPPrflStateBase
	{
	public:// Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aUser user for the state machine
        */
		IMPORT_C static CSIPPrflDeregistrationInProgressState* NewL(
		    MSIPProfileFSMUser& aUser);

		/**
		* Destructor
		*/
		IMPORT_C ~CSIPPrflDeregistrationInProgressState();

	public://new functions
		/**
		* Links states
		* @param aInitState "Init" state
		*/
		IMPORT_C void LinkStates(
			CSIPPrflStateBase& aInitState);

	public://functions from base class
	
	    CSIPConcreteProfile::TStatus ConcreteProfileState() const;
	
		/**
		* A SIP response creating a registration binding or an error response 
		* that is related to an refreshed registration binding  
		* has been received from the network.
		* Moves profile to "Init" state.
		* @param aContext a profile context
		* @param aTransaction contains response elements.
		*/
		void ResponseReceived(
			MSIPProfileContext& aContext,
			CSIPClientTransaction& aTransaction);

		/**
		* An asynchronous error has occurred in the stack related
		* to the request indicated by the given transaction.
		* Moves profile to "Init" state.
		* @param aContext a profile context
		* @param aError error code
		*/
		void ErrorOccured(
			MSIPProfileContext& aContext,
			TInt aError);

		/**
		* Connection state has changed; connection can change to
		* "inactive" state.
		* Moves profile to "Init" state  or
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
		CSIPPrflDeregistrationInProgressState(MSIPProfileFSMUser& aUser);

	private: // Data
	
		CSIPPrflStateBase* iInitState;
		CSIPPrflStateBase* iDeregisterRequestedState;
	};

#endif // CSIPPRFLDEREGISTERATIONINPROGRESSSTATE_H

