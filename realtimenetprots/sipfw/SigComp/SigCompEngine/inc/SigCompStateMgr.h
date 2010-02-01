/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : SigCompStateMgr.h
* Part of     : SigComp / state manager
* Interface   : 
* State manager external interface
* Version     : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef SIGCOMPSTATEMGR_H
#define SIGCOMPSTATEMGR_H

//  INCLUDES
#include <e32base.h>
 
class TStateItem;

// CLASS DECLARATION

/**
*  State manager interface.
*  
*
*  @lib sigcomp
*/

class MSigCompStateMgr
    {
    public:
       /**
        * Finds state item of a given hash.
        * 
        * @param aPartialIdentifier hash ID of the state item we are looking
        *                           for.
        * 
        * @return state item, or null if no state item was found
        */
        virtual TStateItem* FindStateItem(
                                         const TDesC8& aPartialIdentifier) = 0;
    };

#endif
            
// End of File
