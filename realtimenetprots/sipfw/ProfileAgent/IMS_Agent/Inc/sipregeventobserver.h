/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipregeventobserver.h
* Part of     : SIP Profile Agent
* implementation
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef MSIPREGEVENTOBSERVER_H
#define MSIPREGEVENTOBSERVER_H

#include <e32std.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* This call back is used for notifying when 
* - network has updated the expiration time for the registration
* - network has de-registered the user.
*
*  @lib n/a
*/
class MSIPRegEventObserver
	{
    public: // New functions
 
		/**
		* Subscription to the registration state event was successful.
		* 2XX response has been received to the initial SUBSCRIBE and
		* the first full req-event information has been received in NOTIFY.
		*/     
		virtual void RegEventSubscriptionActive() = 0;
   
		/**
		* A network initiated re-registration event has been received.
		* The implementation must call CSIPRefresh::SetIntervalL
		* with the value of aExpirationTime.
		*
		* @param aExpirationTime the new expiration time for the registration       
		*/    
        virtual void ExpirationTimeUpdatedL(TInt aExpirationTime) = 0;
    
		/**
		* A new initial registration must be performed without de-registration.
		*/     
		virtual void ReRegister() = 0;    
    
		/**
		* A network initiated de-registration event has been received.
		* The network has deactivated the registration and 
		* a new initial registration must be performed.
		*/     
		virtual void RegistrationDeactivated() = 0;		
		
		/**
		* A network initiated de-registration event has been received.
		* The implementation must indicate the registration state as failed 
		* and delete the related CSIPRegistrationBinding and 
		* CSIPReqEventHandler immediately without an explicit deregister. 
		*/     
		virtual void RegistrationTerminated() = 0;
		
		/**
		* Subscription to the registration state event has failed and 
		* there is no way to recover. 
		* The implementation must send an explicit deregister request 
		* to the network and then indicate the registration state 
		* as failed and delete the related CSIPRegistrationBinding 
		* and CSIPReqEventHandler.
		*/     
		virtual void RegEventSubscriptionFailedL() = 0;								
	};

#endif
