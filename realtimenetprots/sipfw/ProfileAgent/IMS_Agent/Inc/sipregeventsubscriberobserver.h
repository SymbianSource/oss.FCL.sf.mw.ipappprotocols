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
* Name        : sipregeventsubscriberobserver.h
* Part of     : SIP Profile Agent
* implementation
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef MSIPREGEVENTSUBSCRIBEROBSERVER_H
#define MSIPREGEVENTSUBSCRIBEROBSERVER_H

#include <e32std.h>

// FORWARD DECLARATIONS
class CSIPRegInfoElement;

// CLASS DECLARATION

/**
* This call back is used for notifying about changes in registration state
* event subscription state.
*
*  @lib n/a
*/
class MSIPRegEventSubscriberObserver
	{
    public: // New functions
            
		/**
		* Subscription was sent and an error occured.
		*/
		virtual void SubscriptionFailedL() = 0;
		
		/**
		* Subscription was sent and an error occured.
		* A new subscription should be tried after the given interval.
		* Subscription state should be indicated as active.   
		*
		* @param aRetryAfter the time after which the subscription retried.
		*/
		virtual void SubscriptionFailedL(TInt aRetryAfter) = 0;		
		
		/**
		* Subscription was sent and a response was received that indicated
		* that the reg-event is not supported.
		*/		
		virtual void RegEventNotSupportedByNetworkL() = 0;
		
		/**
		* A new initial registration must be performed.
		*/		
		virtual void ReRegister() = 0;		
		
		/**
		* A notification for the registration state event was received. 
		*
		* @param aNotifyData a parsed registration state data.
		*/
		virtual void NotifyReceivedL(CSIPRegInfoElement& aNotifyData) = 0;
		
		/**
		* A notification for the registration state event was received
		* indication the subscription has terminated.
		* In practice Subscription-State header was set to terminated.
		* This means the current instance of CSIPRegEventSubscriber 
		* should be deleted and a new one should be created and subscribed
		* after the interval indicated in aRetryAfter parameter. 
		*
		* @param aNotifyData a parsed registration state data.
		* @param aRetryAfter the subscription should be tried again after
		*        the given time in seconds. 
		*        Negative value: The subscription must not be retried. 
		*        Zero value: The subscription can be tried immediately.
		*        Positive value: Try again after this interval.       
		*/
		virtual void TerminatedL(CSIPRegInfoElement& aNotifyData,
		                         TInt aRetryAfter) = 0;				
	};

#endif
