
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



#ifndef MSIPEXSTATEVIEWNOTIFIER_H
#define MSIPEXSTATEVIEWNOTIFIER_H

// INCLUDES
#include    <e32base.h>

// CLASS DECLARATIONS
/**
* Interface for notifing view about status change and 
* any intresting changes.
*/
class MSIPExStateViewNotifier
    {
    public:
        /**
        * ShowStatusInfoL
        * Called when game status changes.
        * @param aText A free format text.
        */
        virtual void ShowStatusInfoL( const TDesC& aText ) = 0;

        /**
        * ShowInfoL
        * Called when the new info is available about engine state.
        * @param aText A 16-bit free format text.
        */
        virtual void ShowInfoL( const TDesC& aText ) = 0;

        /**
        * ShowInfoL
        * Called when the new info is available about engine state.
        * @param aText A 8-bit free format text.
        */
        virtual void ShowInfoL( const TDesC8& aText ) = 0;
    };

#endif // MSIPEXSTATEVIEWNOTIFIER_H

// End of file
