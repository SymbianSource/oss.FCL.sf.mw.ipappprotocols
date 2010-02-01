
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



#ifndef __TSIPSTATEREGISTERED_H__
#define __TSIPSTATEREGISTERED_H__

// INCLUDES
#include    "SIPExState.h"

// CLASS DECLARATIONS
/**
* The state is valid when the SIP profile
* has enabled and the socket connection is not openned yet.
*/
class TSIPExStateRegistered 
:   public TSIPExState
    {
    public:

        /**
        * Constructor
        */
        TSIPExStateRegistered() : iEnded( EFalse ) {}
        
        /**
        * InviteL
        * From TSIPExState
        */
        virtual void InviteL( CSIPExEngine* aContext, const TDesC& aAddress );

        /**
        * SendInstantMsgL
        * From TSIPExState
        */
        virtual void SendInstantMsgL( CSIPExEngine* aContext, 
            const TDesC& aAddress, const TDesC& aMsg );

        /**
        * SessionEnded
        * From TSIPExState
        */
        virtual void SessionEnded( CSIPExEngine* aContext );
        
        /**
        * InviteReceived
        * From TSIPExState
        */
        virtual void InviteReceived( CSIPExEngine* aContext, const TDesC8& aFrom, const TUint32 aIapId );

        /**
        * DisableProfile
        * From TSIPExState
        */
        virtual void DisableProfileL( CSIPExEngine* aContext );

        /**
        * SocketState
        * From TSIPExState
        */
        virtual void SocketState( CSIPExEngine* aContext, TInt aNewState );
        
    public:
    
        TBool iEnded;
    };

#endif // __TSIPSTATEREGISTERED_H__

// End of file
