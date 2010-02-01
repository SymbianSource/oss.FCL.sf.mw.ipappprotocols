
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



#ifndef __TSIPSTATEREGISTERING_H__
#define __TSIPSTATEREGISTERING_H__

// INCLUDES
#include    "SIPExState.h"

// CLASS DECLARATIONS
/**
* The state is valid when waiting the 
* SIP profile to be enabled.
*/
class TSIPExStateRegistering 
:   public TSIPExState
    {
    public:

        /**
        * ProfileEnabled
        * From TSIPExState
        */
        virtual void ProfileEnabled( CSIPExEngine* aContext, TUint32 aSIPProfileId );

        /**
        * ProfileError
        * From TSIPExState
        */
        virtual void ProfileError( CSIPExEngine* aContext, TInt aError );
    };

#endif // __TSIPSTATEREGISTERING_H__

// End of file
