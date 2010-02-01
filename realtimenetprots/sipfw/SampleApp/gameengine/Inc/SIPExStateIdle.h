
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



#ifndef __TSIPSTATEIDLE_H__
#define __TSIPSTATEIDLE_H__

// INCLUDES
#include    "SIPExState.h"

// CLASS DECLARATIONS
/**
* The state is valid when the profile haven't 
* been enabled yet.
*/
class TSIPExStateIdle 
:   public TSIPExState
    {
    public: // From base class

        /**
        * Constructor
        */
        TSIPExStateIdle() : iEnded( EFalse ) {}
        
        /**
        * EnableProfile
        * From TSIPExState
        */
        virtual void EnableProfileL( CSIPExEngine* aContext );

        /**
        * SessionEnded
        * From TSIPExState
        */
        virtual void SessionEnded( CSIPExEngine* aContext );

        /**
        * InviteReceived
        * From TSIPExState
        */
        virtual void InviteReceived( CSIPExEngine* aContext, 
                                     const TDesC8& aFrom,
                                     const TUint32 aIapId );
                                     
    public:
    
        TBool iEnded;
        
    };

#endif // __TSIPSTATEIDLE_H__

// End of file
