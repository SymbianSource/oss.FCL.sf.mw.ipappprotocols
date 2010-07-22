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
* Name        : sipprflderegisterrequestedstate.h
* Part of     : sip profile fsm 
* Version     : %version: 2.1.2 %
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPRFLDEREGISTERREQUESTEDSTATE_H
#define CSIPPRFLDEREGISTERREQUESTEDSTATE_H

//  INCLUDES
#include "sipprflstatebase.h"

// FORWARD DECLARATIONS
class MSIPProfileContext;


// CLASS DECLARATION
/**
* Reacts to events that are possible in "Deregister Requested" state. 
* Profile is in this state if deregistration was requested and 
* connection was suspended.
*/
class CSIPPrflDeregisterRequestedState: public CSIPPrflStateBase
	{
	public:// Constructors and destructor		   
        /**
        * Two-phased constructor.
        * @param aUser user for the state machine
        */
		IMPORT_C static CSIPPrflDeregisterRequestedState* NewL(
		    MSIPProfileFSMUser& aUser);

        /**
        * Destructor
        */
		IMPORT_C ~CSIPPrflDeregisterRequestedState();

	public://new functions
		/**
		* Links the states
		* @param aInitState "Init" state
		* @param aRegisteredState "Registered" state
		* @param aDeregistrationInProgressState "Deregistration in Progress" state
		*/
		IMPORT_C void LinkStates(
			CSIPPrflStateBase& aInitState,
			CSIPPrflStateBase& aRegisteredState,
			CSIPPrflStateBase& aDeregistrationInProgressState);

	public://functions from base class
		
		CSIPConcreteProfile::TStatus ConcreteProfileState() const;
		
		/* DeRegister requested
		 * update on multiple profile issue, when update on the previous profile is pending.
		 * 07 May 2010    
		 * */
		void DeregisterL(
		            MSIPProfileContext& aContext);
		
		/**
		* Moves back the profile to "Registered" state
		* @param aContext a profile context
		*/
		void RegisterL(
			MSIPProfileContext& aContext);
		
		/**
		* Indicates that refresh failed during the
		* suspended connection. Moves profile to "Init" state.
		* @param aContext a profile context
		* @param aError an error
		*/
		void ErrorOccured(
			MSIPProfileContext& aContext,
			TInt aError);

		/**
		* Connection state has changed.
		* Moves profile to "Init" state in case of inactive connection or to 
		* "Deregistration In Progress" state in case of active connection.
		* @param aContext a profile context
		* @param aState indicates the current connection state
		*/
		void ConnectionStateChanged(
			MSIPProfileContext& aContext,
			CSIPConnection::TState aState);	

	private:

		CSIPPrflDeregisterRequestedState(MSIPProfileFSMUser& aUser);

	private: // Data

		CSIPPrflStateBase* iInitState;
		CSIPPrflStateBase* iRegisteredState;
		CSIPPrflStateBase* iDeregistrationInProgressState;
	};

#endif // CSIPPRFLDEREGISTERREQUESTEDSTATE_H
