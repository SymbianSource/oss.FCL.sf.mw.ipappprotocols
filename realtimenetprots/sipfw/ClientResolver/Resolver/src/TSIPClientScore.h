/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : TSIPClientScore.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef TSIPCLIENTSCORE_H
#define TSIPCLIENTSCORE_H

//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION
/**
* A container class for SIP client feature tag matching scores.
* @lib sipresolver.lib
*/
class TSIPClientScore
    {
    public: // Constructors

        TSIPClientScore() 
            : iScore(0), iUid(TUid::Null()) {}
        
        
        TSIPClientScore(TInt aScore, const TUid& aUid) 
            : iScore(aScore), iUid(aUid) {}
    
    public: // Data
    
        TInt iScore;
        TUid iUid;
    };

#endif // TSIPCLIENTSCORE_H

// End of File
