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
* Name        : sipprflinitstate.h
* Part of     : sip profile fsm
* Version     : %version: 2.1.1 %
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPRFLINITSTATE_H
#define CSIPPRFLINITSTATE_H

//  INCLUDES
#include "sipprflstatebase.h"

// CLASS DECLARATION
/**
* Reacts to events that are possible in 
* "Init" state. Profile is in this state 
* before it is going to be registered.
*/
class CSIPPrflInitState: public CSIPPrflStateBase
	{
	public:// Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aUser user for the state machine
        */
		IMPORT_C static CSIPPrflInitState* NewL(MSIPProfileFSMUser& aUser);

		/**
		* Destructor
		*/
		IMPORT_C ~CSIPPrflInitState();

	public://new functions
		/**
		* Links the states
		* @param aResolvingProxiesState "Resolving Proxies" state
		* @param aRegisterRequestedState "Register Requested" state
		* @param aRegistrationInProgressState "Registration In Progress" state
		*/
		IMPORT_C void LinkStates(
		    CSIPPrflStateBase& aResolvingProxiesState,
			CSIPPrflStateBase& aRegisterRequestedState,
			CSIPPrflStateBase& aRegistrationInProgressState);

	public://functions from base class
	
        CSIPConcreteProfile::TStatus ConcreteProfileState() const;
	
		/**
		* Registers the profile. 
		* Moves profile to "Resolving Proxies" state 
		* if dynamic proxy discovery has been enabled for the profile.
		* Otherwise moves profile to´"Register Requested" state in case 
		* of suspended or inactive or init connection or to 
		* "Registration In Progress" state in case of active connection.
		* according to the connection state.
		* @param aContext a profile context
		*/
		void RegisterL(
			MSIPProfileContext& aContext);
			
		/**
		* Checks if the profile  is in idle state.
		* @return ETrue if conditions are met.
		*/
		TBool IsIdle( MSIPProfileContext& aContext );	

	private:
	    /**
		* Constructor.
		*/
		CSIPPrflInitState(MSIPProfileFSMUser& aUser);

	private: // Data
	
	    CSIPPrflStateBase* iResolvingProxies;
		CSIPPrflStateBase* iRegisterRequested;
		CSIPPrflStateBase* iRegistrationInProgress;
	};

#endif // CSIPPRFLINITSTATE_H
