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
* Name        : sipprflresolvingproxiesstate.h
* Part of     : sip profile fsm 
* Version     : %version: 2.1.1 %
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPRFLRESOLVINGPROXIESSTATE_H
#define CSIPPRFLRESOLVINGPROXIESSTATE_H

//  INCLUDES
#include "sipprflstatebase.h"

// CLASS DECLARATION
/**
* Reacts to events that are possible in "Resolving Proxies" state. 
* Profile is in this state if the dynamic proxy has been enabled for it
* before it is going to be registered.
*/
class CSIPPrflResolvingProxiesState: public CSIPPrflStateBase
	{
	public:// Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aUser user for the state machine
        */
		IMPORT_C static CSIPPrflResolvingProxiesState* NewL(
		    MSIPProfileFSMUser& aUser);

		/**
		* Destructor
		*/
		IMPORT_C ~CSIPPrflResolvingProxiesState();

	public://new functions
		/**
		* Links the states
		* @param aInitState "Init"-state
		* @param aRegisterState "Register Requested"-state
		* @param aRegistrationInProgressState "Registration In Progress"-state
		*/
		IMPORT_C void LinkStates(
		    CSIPPrflStateBase& aInitState,
			CSIPPrflStateBase& aRegisterRequestedState,
			CSIPPrflStateBase& aRegistrationInProgressState);

	public://functions from base class
	
	    CSIPConcreteProfile::TStatus ConcreteProfileState() const;
	
	    void DeregisterL(MSIPProfileContext& aContext);

        void ConnectionStateChanged(
            MSIPProfileContext& aContext,
            CSIPConnection::TState aState);
    
        void ProxyResolvingRequestCompleteL(
            MSIPProfileContext& aContext,
            MDesC8Array& aProxies);

        void ProxyResolvingRequestFailed(
		    MSIPProfileContext& aContext,
		    TInt aError);

	private:

		CSIPPrflResolvingProxiesState(MSIPProfileFSMUser& aUser);

	private: // Data

	    CSIPPrflStateBase* iInit;
		CSIPPrflStateBase* iRegisterRequested;
		CSIPPrflStateBase* iRegistrationInProgress;
	};

#endif // CSIPPRFLRESOLVINGPROXIESSTATE_H
