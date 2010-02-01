
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


    
#ifndef __TSIPSTATEREMOTE_H__
#define __TSIPSTATEREMOTE_H__

// INCLUDES
#include    "SIPExState.h"

// CLASS DECLARATIONS
/**
* The state is valid when the turn is in remote
* player.
*/
class TSIPExStateRemote 
:   public TSIPExState
    {
    public:

        /**
        * SocketData
        * From TSIPExState
        */
        virtual void SocketData( CSIPExEngine* aContext, TDesC8& aData );

        /**
        * SendInstantMsgL
        * From TSIPExState
        */
        virtual void SendInstantMsgL( CSIPExEngine* aContext, 
            const TDesC& aAddress, const TDesC& aMsg );

        /**
        * DisableProfile
        * From TSIPExState
        */
        virtual void DisableProfileL( CSIPExEngine* aContext );

        /**
        * EndGameL
        * From TSIPExState
        */
        virtual void EndGameL( CSIPExEngine* aContext );
    };

#endif // __TSIPSTATEREMOTE_H__

// End of file
