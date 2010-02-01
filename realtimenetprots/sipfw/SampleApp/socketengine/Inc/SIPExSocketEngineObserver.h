
/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#ifndef MSIPEXSOCKETENGINEOBSERVER_H
#define MSIPEXSOCKETENGINEOBSERVER_H

// INCLUDES
#include    "SIPExSocketEngine.h"

// CLASS DECLARATIONS
/**
* Interface for observing the socket engine.
*/
class MSIPExSocketEngineObserver
    {
    public:

        /**
        * SocketState
        * @param aNewState The new state of the socket engine.
        */
        virtual void SocketState( TInt aNewState ) = 0;
        
        /**
        * SocketData
        * @param aData The data from socket.
        */
        virtual void SocketData( TDesC8& aData ) = 0;
    };

#endif // MSIPEXSOCKETENGINEOBSERVER_H

// End of file
