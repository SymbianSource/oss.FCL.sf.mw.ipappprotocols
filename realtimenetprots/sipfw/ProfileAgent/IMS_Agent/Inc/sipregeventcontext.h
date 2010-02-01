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
* Name        : sipregeventcontext.h
* Part of     : SIP Profile Agent
* implementation
* Version     : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef MSIPREGEVENCONTEXT_H
#define MSIPREGEVENCONTEXT_H

#include <e32std.h>


// CLASS DECLARATION
/**
* A context for reg-event subscription states
*
*  @lib n/a
*/
class MSIPRegEventContext
	{
	public: // Enumerations
	
        /** Subscription states */
        enum TState 
            {
            /** Not subscribed  */
			ENotSubscribed=0,
			/** SUBSCRIBE sent but final response not yet received */
			ESubscribing=1,			          
            /** SUBSCRIBE sent and a 2XX response received */
			ESubscribed=2,
			/** Deleted old SUBSCRIBE dialog and sent a new SUBSCRIBE 
			 *  but final response not yet received */
			EReSubscribing=3,
			/** Not a real state. Upper limit for the number of states */
			EMaxStates=4
            };
	
    public: // New functions
       
        virtual void DoSubscribeL() = 0;
        
        virtual void DoRefreshL() = 0;
        
        virtual void ReSubscribeAfterL(TUint aRetryAfter) = 0;
        
        virtual void SubscriptionFailedL() = 0;
        
        virtual void RegEventNotSupportedByNetworkL() = 0;
        
        virtual void ReRegister() = 0;
        
        virtual void ChangeState(TState aNewState) = 0;
	};

#endif
