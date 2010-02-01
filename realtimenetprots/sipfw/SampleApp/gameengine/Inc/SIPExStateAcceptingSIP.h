
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



#ifndef __TSIPSTATEACCEPTINGSIP_H__
#define __TSIPSTATEACCEPTINGSIP_H__

// INCLUDES
#include    "SIPExState.h"

// CLASS DECLARATIONS
/**
* The state is valid when we have recived the invitation 
* and socket is listening the connection from remote peer.
*/
class TSIPExStateAcceptingSIP 
:   public TSIPExState
    {
    public:
        
        /**
        * SocketState
        * From TSIPExState
        */
        virtual void SocketState( CSIPExEngine* /*aContext*/, TInt /*aNewState*/ );

        /**
        * SocketState
        * From TSIPExState
        */
        virtual void EndGameL( CSIPExEngine* aContext );

        /**
        * InvitationCancelled
        * From TSIPExState
        */
        virtual void InvitationCancelled( CSIPExEngine* aContext);
    };

#endif // __TSIPSTATEACCEPTINGSIP_H__

// End of file
