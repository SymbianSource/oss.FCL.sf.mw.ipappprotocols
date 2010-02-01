/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipprofilecontext.h
* Part of     : SIP / SIP Profile Agent / SIP Profile FSM 
* Version     : %version: 2.1.1 %
*
*/




/**
 @internalComponent
*/


#ifndef MSIPPROFILECONTEXT_H
#define MSIPPROFILECONTEXT_H

//  INCLUDES
#include <e32base.h>
#include "sipconnection.h"

// FORWARD DECLARATIONS
class CSIPConcreteProfile;
class CSIPRegistrationBinding;
class CSIPClientTransaction;
class CSIPPrflStateBase;
class CSIPMessageElements;
class MSIPProfileAgentObserver;


// CLASS DECLARATION
/**
* The interfaces to be implemented for SIP Profile FSM
* client. The interface provides functions for requesting
* needed information and setting information in the particular state.
*/
class MSIPProfileContext
	{
    public: // Enumerations
        
        /** Profile states */
        enum TSIPProfileState
            {
            /** profile is in init state*/
            EInit=1,
            /** registration was requested for this profile*/
            ERegisterRequested,
            /** registration is in progress for the profile*/
            ERegistrationInProgress,
            /** profile is registered */
            ERegistered,
            /** deregistration was requested for this profile*/
            EDeregisterRequested,
            /** deregistration is in progress for this profile*/
            EDeregistrationInProgress,
            /** resolving of proxy address is in progress for this profile*/
            EProxyResolvingInProgress,
            /** the profile is waiting for registration retry timer to expire*/
            EDelayedRegisterRequested
            };	
	
	public:
	
		/**
		* Gets the connection
		* @return the connection
		*/
		virtual const CSIPConnection& Connection() const = 0;
		
		/**
		* Gets the registration
		* @return the registration; the ownership is not transfered
		*/
		virtual CSIPRegistrationBinding* Registration() = 0;

		/**
		* Sets next state
		* @param aState a next state
		*/
		virtual void SetNextState(CSIPPrflStateBase& aState) = 0;

		/**
		* Removes http digest configuration, deletes
		* the transaction and sets profile pointer to zero
		*/
		virtual void ForgetProfile() =0;

		/**
		* Sets the transaction; zero-pointer resets the transaction.
		* @param aTransaction a transaction; the ownership is transfered
		*/
		virtual void SetTransaction(CSIPClientTransaction* aTransaction) = 0;

		/**
		* Gets profile, the ownership is not transfered
		* @return profile
		*/
		virtual CSIPConcreteProfile* Profile() = 0;

		/**
		* Gets profile agent observer
		* @return profile agent observer
		*/
		virtual MSIPProfileAgentObserver& AgentObserver() const = 0;

		/**
		* Destroys registration object
		*/
		virtual void DestroyRegistration() = 0;

		/**
		* creates a registration object
		*/
		virtual void CreateRegistrationL() = 0;

		/**
		* creates Message Elements for initial REGISTER
		*/
		virtual CSIPMessageElements* CreateMsgElementsLC() = 0;
		
		/**
		* creates Message Elements for de-REGISTER
		*/
		virtual CSIPMessageElements* CreateDeRegisterElementsL() = 0;

		/**
		* sets the profile 
		*/
		virtual void SetProfile(CSIPConcreteProfile* aProfile) = 0;
		
		/**
		* updates SIP stack with new Contact header parameters
		* @param aNewProfile a new profile
		*/
		virtual void UpdateContactHeaderParamsL(
		    CSIPConcreteProfile& aNewProfile) = 0;
		    
		/**
		* Sets the registered AORs for the profile
		* @return KErrNone, if succesfully set, otherwise an error.
		*/
		virtual TInt SetRegisteredAORs() = 0;	
		
		virtual TBool RetryAfterTimer() = 0;
		
		virtual TBool RetryTimerInUse() = 0;
		
		virtual void RetryRegistration() = 0;
		
		virtual void RemoveDeltaTimerEntry() = 0;
		
		virtual TBool IsProxyResolvingEnabled() const = 0;
		
		virtual TBool ProxiesAlreadyResolved() = 0;
		
		virtual void ResolveProxyL() = 0;

        virtual void HandleProxyResolvingError(TInt aError) = 0;
        
        virtual void SetProxiesL(MDesC8Array& aProxies) = 0;
        
        virtual void CancelProxyResolving() = 0;
        
        virtual void SetNegotiatedSecurityMechanismL() = 0;
		
		virtual void ResetNegotiatedSecurityMechanismL() = 0;
	};

#endif // MSIPPROFILECONTEXT_H

