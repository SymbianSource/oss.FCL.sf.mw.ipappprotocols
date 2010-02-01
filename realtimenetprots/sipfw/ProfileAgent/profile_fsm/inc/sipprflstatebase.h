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
* Name        : sipprflstatebase.h
* Part of     : sip profile fsm
* Version     : %version: 2.1.1 %
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPRFLSTATEBASE_H
#define CSIPPRFLSTATEBASE_H

//  INCLUDES
#include <bamdesca.h>
#include "sipprofilecontext.h"
#include "sipconcreteprofile.h"
#include "sipconnection.h"

// FORWARD DECLARATIONS
class CSIPClientTransaction;
class MSIPProfileFSMUser;

// CLASS DECLARATION

/**
* Profile state base; provides function default implementation,
* common error handling and function for getting state name.
*
* Function will leave with KErrTotalLossOfPrecision in case of
* invalid state.
*/
class CSIPPrflStateBase: public CBase
	{
	public:// Constructors and destructor
		/**
		* Destructor
		*/
		IMPORT_C virtual ~CSIPPrflStateBase();

	public://new functions
	
		/**
		* Gets state name
		* @return state name
		*/
        IMPORT_C MSIPProfileContext::TSIPProfileState Name() const;
        
        /*
        * Converts the state to one of the states defined in CSIPConcreteProfile
        * @return state of the CSIPConcreteProfile
        */
        virtual CSIPConcreteProfile::TStatus ConcreteProfileState() const = 0;
	
		/**
		* Registers the profile
		* @param aContext a profile context
		*/
        virtual void RegisterL(
			MSIPProfileContext& aContext);

		/**
		* Deregisters the profile
		* @param aContext a profile context
		*/
        virtual void DeregisterL(
			MSIPProfileContext& aContext);

		/**
		* Updates the profile
		* @param aContext a profile context
		* @param aNewProfile a new profile
		*/
        virtual void UpdateL(
			MSIPProfileContext& aContext,
			CSIPConcreteProfile& aNewProfile);

		/**
		* A SIP response creating a registration binding or an error response 
		* that is related to an refreshed registration binding  
		* has been received from the network.
		* @param aContext a profile context
		* @param aTransaction contains response elements.
		*/
        virtual void ResponseReceived(
			MSIPProfileContext& aContext,
			CSIPClientTransaction& aTransaction);

		/**
		* An asynchronous error has occured related to a periodical refresh 
		* that relates to a registration.
		* @param aContext a profile context
		* @param aError error code
		*/
        virtual void ErrorOccured(
			MSIPProfileContext& aContext,
			TInt aError);
		/**
		* Connection state has changed.
		* @param aContext a profile context
		* @param aState indicates the current connection state
		*/
        virtual void ConnectionStateChanged(
			MSIPProfileContext& aContext,
			CSIPConnection::TState aState);
		
        /**
        * Checks whether a registration is pending.
        * In other words if a REGISTER has been sent 
        * but a final response has not yet been received.
        * @return ETrue if a registration is pending, otherwise EFalse
        */
        virtual TBool IsRegisterPending() const;
		
        virtual void ChangeRegistrationStateL(
			MSIPProfileContext& aContext);

		/**
        * Resolving proxies has been succesfully completed
        * @param aContext a profile context
        * @param aProxies the proxies resolved
        */        
        virtual void ProxyResolvingRequestCompleteL(
            MSIPProfileContext& aContext,
            MDesC8Array& aProxies);

		/**
		* Proxy resolving has failed.
		* @param aContext a profile context
		* @param aError the failure reason.
		*/
		virtual void ProxyResolvingRequestFailed(
		    MSIPProfileContext& aContext,
		    TInt aError);
		    
		/**
		* Checks if the profile  is in idle state.
		* @return ETrue if conditions are met.
		*/
		virtual TBool IsIdle( MSIPProfileContext& aContext );

	protected:
	    /**
		* C++ default constructor.
		* @param aUser user for the state machine	
		* @param aName a state name
		*/
		CSIPPrflStateBase(
		    MSIPProfileFSMUser& aUser,
		    MSIPProfileContext::TSIPProfileState aName);

		/**
		* Applies default error handling
		* @param aContext a profile context
		* @param aError a error
		* @param aNextState a next state to move to
		*/
		void HandleError(
			MSIPProfileContext& aContext,
			TInt aError,
			CSIPPrflStateBase* aNextState);

		/**
		* Instructs to forget profile and 
		* registration
		* @param aContext a profile context
		* @param aContextId an Id of registration context
		* @return the concrete profile that was detached from the profile 
		*/
	    CSIPConcreteProfile& DoCleanup(
			MSIPProfileContext& aContext,
			TUint32& aContextId);

    protected: // Data
    
        MSIPProfileFSMUser& iUser;

	private: // Data
	
		MSIPProfileContext::TSIPProfileState iName;
	};

#endif // CSIPPRFLSTATEBASE_H

