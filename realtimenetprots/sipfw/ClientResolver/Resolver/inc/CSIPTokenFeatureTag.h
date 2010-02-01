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
* Name          : CSIPTokenFeatureTag.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPTOKENFEATURETAG_H
#define CSIPTOKENFEATURETAG_H

//  INCLUDES
#include "CSIPFeatureTag.h"

// CLASS DECLARATION
/**
* Class for token-nobang-valued (RFC 3840 page 14) SIP feature tags.
* @lib sipresolver.lib
*/
class CSIPTokenFeatureTag : public CSIPFeatureTag
    {
    public: // Constructors and destructor

        static CSIPTokenFeatureTag* NewL(
            RStringF aName,
            TBool aNegation,
            const TDesC8& aValue);

        static CSIPTokenFeatureTag* NewLC(
            RStringF aName,
            TBool aNegation,
            const TDesC8& aValue);

        ~CSIPTokenFeatureTag();
    
    private: // Constructors

        CSIPTokenFeatureTag();
        
        CSIPTokenFeatureTag(RStringF aName, TBool aNegation);
    
        void ConstructL(const TDesC8& aValue);
        
    private: // From CSIPFeatureTag

        TBool MatchValue(const CSIPFeatureTag& aFeatureTag) const;
          
    private: // Data
       
        HBufC8* iValue;
    };

#endif // CSIPTOKENFEATURETAG_H

// End of File
