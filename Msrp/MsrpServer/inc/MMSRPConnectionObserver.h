/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

#ifndef MMMSRPCONNECTIONOBSERVER_H
#define MMMSRPCONNECTIONOBSERVER_H

// INCLUDES
#include <e32base.h>

class CMSRPMessageHandler;


// CLASS DECLARATIONS
/**
* Interface for observing the connection
*/
class MMSRPConnectionObserver
    {
    public:
        
        enum TMSRPConnErrors
             {
             EConnectionStateUnexpected = -150,
             };

        /**
        * SocketState
        * @param aNewState The new state of the connection
        * connected, timed out, disconnected etc.., unknown or undefined
		* @param aStatus system-wide error code
        *///pass connection also
        virtual void ConnectionStateL( TInt aNewState, TInt aStatus ) = 0;
        
        /**
         * Message Received
         * @return: if session id match return TRUE
         * else return FALSE
         */
        virtual TBool MessageReceivedL( CMSRPMessageHandler* aMsg ) = 0;
        
        /**
         * Message received which does not belong to any existing session
         * @param aMsg the received message
         */
        virtual void UnclaimedMessageL( CMSRPMessageHandler* aMsg ) = 0;
        
        /**
        * Current message receive progress reported to connected subsessions
        * @param aMessageHandler message handler instance, ownership not transferred
         */
        virtual void MessageReceiveProgressL( CMSRPMessageHandler* aMessageHandler ) = 0;
    };

#endif // MMMSRPCONNECTIONOBSERVER_H

// End of file
