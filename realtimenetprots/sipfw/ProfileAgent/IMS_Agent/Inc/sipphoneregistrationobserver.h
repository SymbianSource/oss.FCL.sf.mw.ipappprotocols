/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name        : sipphoneregistrationobserver.h
* Part of     : SIP Profile Agent / SIP IMS Agent
* Version     : %version: 3 %
*
*/




/**
 @internalComponent
*/

#ifndef MSIPPHONEREGISTRATIONOBSERVER_H
#define MSIPPHONEREGISTRATIONOBSERVER_H

// INCLUDES
#include <e32def.h>

// CLASS DECLARATION
/**
* MSipPhoneRegistrationObserver defines an internal interface for observing 
* phone's network registration status.
*/
class MSipPhoneRegistrationObserver
    {
    public: // Abstract methods

        /**
         * Called when the phone's network registration status changes.
         */
        virtual void PhoneRegistrationStatusChangedL() = 0;

        /**
         * Called when there is a fatal failure when monitoring
         * the phone's network registration status.
         * The user should delete the monitor.
         * @param aError the failure reason
         */
        virtual void PhoneRegistrationStatusError( TInt aError ) = 0;
    };

#endif // MSIPPHONEREGISTRATIONOBSERVER_H
