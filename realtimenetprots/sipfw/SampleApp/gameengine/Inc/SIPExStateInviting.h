
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



#ifndef __TSIPSTATEINVITING_H__
#define __TSIPSTATEINVITING_H__

// INCLUDES
#include    "SIPExState.h"

// CLASS DECLARATIONS
/**
* The state is valid when user has invited
* remote peer to the game.
*/
class TSIPExStateInviting 
:   public TSIPExState
    {
    public:
        /**
        * InviteAcceptedByRemote
        * From TSIPExState
        */
        virtual void InviteAcceptedByRemote( CSIPExEngine* aContext, const TInetAddr& aIPAddress, const TUint32 aIapId );
                        
        /**
        * InvitationCancelled
        * From TSIPExState
        */
        virtual void InvitationCancelled( CSIPExEngine* aContext);

        /**
        * InvitationCancelled
        * From TSIPExState
        */
        virtual void InviteDeclinedByRemote( CSIPExEngine* aContext, const TInt aResponse );

        /**
        * SessionEnded
        * From TSIPExState
        */
        virtual void EndGameL( CSIPExEngine* aContext );
    };

#endif // __TSIPSTATEINVITING_H__

// End of file
