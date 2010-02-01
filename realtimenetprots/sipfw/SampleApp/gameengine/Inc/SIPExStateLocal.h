
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



#ifndef __TSIPSTATELOCAL_H__
#define __TSIPSTATELOCAL_H__

// INCLUDES
#include    "SIPExState.h"

// CLASS DECLARATIONS
/**
* The state is valid when waiting the user move.
*/
class TSIPExStateLocal
:   public TSIPExState
    {
    public:

        /**
        * CursorPressed
        * From TSIPExState
        */
        virtual void CursorPressed( CSIPExEngine* aContext );
        
        /**
        * CursorRight
        * From TSIPExState
        */
        virtual void CursorRight( CSIPExEngine* aContext );
        
        /**
        * CursorLeft
        * From TSIPExState
        */
        virtual void CursorLeft(  CSIPExEngine* aContext );

        /**
        * MoveCursorL
        * From TSIPExState
        */
        virtual void MoveCursorL( CSIPExEngine* aContext, const TInt aNewPosition );

        /**
        * DrawCursor
        * From TSIPExState
        */        
        virtual TBool DrawCursor();
        
        /**
        * SendInstantMsgL
        * From TSIPExState
        */
        virtual void SendInstantMsgL( CSIPExEngine* aContext, 
            const TDesC& aAddress, const TDesC& aMsg );
        
        /**
        * InviteL
        * From TSIPExState
        */
        virtual void InviteL( CSIPExEngine* aContext, const TDesC& aAddress );
        
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

#endif // __TSIPSTATELOCAL_H__

// End of file
