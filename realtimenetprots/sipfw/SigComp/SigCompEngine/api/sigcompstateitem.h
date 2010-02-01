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
* Name        : SigCompStateItem.h
* Part of     : SigComp / state items
* Interface   : 
* sigcomp state item
* Version     : 1.0
*
*/



#ifndef TSIGCOMPSTATEITEM_H
#define TSIGCOMPSTATEITEM_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION
/**
* @publishedPartner
* @released
*
* @brief Class for collection of state item related parameters.
* 
* The SigComp state item as defined in RFC3320.
*/
class TSigCompStateItem
    {
    public:

        /** State identifier */
        TPtrC8 iStateIdentifier;

        /** State length */
        TUint16 iStateLength;

        /** State address */
        TUint16 iStateAddress;

        /** State instruction */
        TUint16 iStateInstruction;

        /** State minimum access length */
        TUint16 iMinimumAccessLength;

        /** Full file name containing state value */
        TPtrC iStateValue;
    };

#endif // TSIGCOMPSTATEITEM_H
