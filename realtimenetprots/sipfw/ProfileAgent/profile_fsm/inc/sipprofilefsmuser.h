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
* Name        : sipprofilefsmuser.h
* Part of     : sip profile fsm
* Version     : SIP 4.1
*
*/




/**
 @internalComponent
*/

#ifndef MSIPPROFILEFSMUSER_H
#define MSIPPROFILEFSMUSER_H

//INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class CSIPConcreteProfile;

// CLASS DECLARATION
/**
 * Callback for SIP Profile FSM to register / deregister SIP profiles.
 */
class MSIPProfileFSMUser
    {
    public:

        /**
        * Registers a SIP profile.
        * @param aSIPProfile sip profile to register
        */
        virtual void RegisterProfileL(
            CSIPConcreteProfile& aSIPProfile) = 0;

        /**
        * Deregisters a SIP profile.
        * @param aSIPProfile sip profile to deregister
        */
        virtual void DeregisterProfileL(
            CSIPConcreteProfile& aSIPProfile) = 0;

        /**
        * Retries registration for a SIP profile for which 
        * a previously attempted registration has failed. 
        * @param aSIPProfile sip profile to be registered
        */
        virtual void RetryProfileRegistrationL( 
            CSIPConcreteProfile& aSIPProfile ) = 0;

        /**
        * Checks whether a SIP profile has to be added into the 
        * registration/deregistration queue.
        * @return ETrue if the profile has to be added to queue,
        *         otherwise EFalse.
        */        
        virtual TBool AddProfileIntoQueue( 
            CSIPConcreteProfile& aSIPProfile ) const = 0;

        /**
        * Registers a SIP profile that was pending in registration queue.
        * @param aSIPProfile sip profile to register
        */            
        virtual void RegisterProfileAfterQueueL( 
            CSIPConcreteProfile& aSIPProfile ) = 0;

        /**
        * Deregisters a SIP profile that was pending in deregistration queue.
        * @param aSIPProfile sip profile to deregister
        */                           
        virtual void DeregisterProfileAfterQueueL( 
            CSIPConcreteProfile& aSIPProfile ) = 0;

        /**
        * Retries registration for a SIP profile for which 
        * a previously attempted registration has failed. 
        * and that was pending in the registration queue.
        * @param aSIPProfile sip profile to register
        */
        virtual void RetryProfileRegistrationAfterQueueL( 
            CSIPConcreteProfile& aSIPProfile ) = 0;
            
        /**
        * Check if the profile is in queue.
        * @return ETrue if idle, EFalse otherwise
        * @param aSIPProfile sip profile to register
        */
        virtual TBool IsInQueue( 
            CSIPConcreteProfile& aSIPProfile ) const = 0;
            
        virtual void SetInterimProfile( 
        			CSIPConcreteProfile* aSIPConcreteProfile ) = 0;
   
        
    };

#endif // MSIPPROFILEFSMUSER_H

