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
* Name        : sipprflregisterdelayrequestedstate.h
* Part of     : sip profile fsm
* Version     : %version: 2.1.1 %
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPRFLREGISTERDELAYREQUESTEDSTATE_H
#define CSIPPRFLREGISTERDELAYREQUESTEDSTATE_H

//  INCLUDES
#include "sipprflstatebase.h"

// FORWARD DECLARATIONS
class MSIPProfileContext;

// CLASS DECLARATION
/**
* Reacts to events that are possible in "Register Delay Requested" state. 
* Profile is in this state when the registration is requested, 
* but after a delay.
*/
class CSIPPrflRegisterDelayRequestedState: public CSIPPrflStateBase
	{
	public:// Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aUser user for the state machine
        */
		IMPORT_C static CSIPPrflRegisterDelayRequestedState* NewL(
		    MSIPProfileFSMUser& aUser);

		/**
		* Destructor
		*/
		IMPORT_C ~CSIPPrflRegisterDelayRequestedState();

	public://new functions
		/**
		* Links states
		* @param aInitState "Init" state
		*/

		IMPORT_C void LinkStates(CSIPPrflStateBase& aInitState);

	public://functions from base class
	
	    CSIPConcreteProfile::TStatus ConcreteProfileState() const;
	
		/**
		* Deregisters the profile. Moves profile to 
		* "Init" state.
		* @param aContext a profile context
		*/
		void DeregisterL(MSIPProfileContext& aContext);

		/**
		* Retry registration timer has expired. Moves profile
		* to "Init"state.
		* @param aContext a profile context
		*/
		void ChangeRegistrationStateL(MSIPProfileContext& aContext);
		
		/**
		* Refreshes the connection.
		* @param aContext a profile context
		*/		
		void RegisterL(MSIPProfileContext& aContext);
		
		/**
		* Connection state has changed. Moves profile
		* to "Init"state. Or
		* In case of UnAvailable connection report error to client and move profile to "Init" state 
		* @param aContext a profile context
		* @param aState indicates the current connection state
		*/
		void ConnectionStateChanged(MSIPProfileContext& aContext,
					CSIPConnection::TState aState);	



	private:

		CSIPPrflRegisterDelayRequestedState(MSIPProfileFSMUser& aUser);

	private: // Data

		CSIPPrflStateBase* iInitState;
	};

#endif // CSIPPRFLREGISTERDELAYREQUESTEDSTATE_H
