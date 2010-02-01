
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



#ifndef __TSIPSTATECONNECTING_H__
#define __TSIPSTATECONNECTING_H__

// INCLUDES
#include    "SIPExState.h"

// CLASS DECLARATIONS
/**
* The state is valid when the socket engine
* is trying to connect to the remote peer.
*/
class TSIPExStateConnecting 
:   public TSIPExState
    {
    public:
        
        /**
        * From TSIPExState
        */
        virtual void SocketState( CSIPExEngine* aContext, TInt aNewState );

        /**
        * From TSIPExState
        */
        virtual void EndGameL( CSIPExEngine* aContext );
    };

#endif // __TSIPSTATECONNECTING_H__

// End of file
