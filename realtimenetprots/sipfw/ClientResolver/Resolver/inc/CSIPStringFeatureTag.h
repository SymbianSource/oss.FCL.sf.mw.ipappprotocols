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
* Name          : CSIPStringFeatureTag.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef CSIPSTRINGFEATURETAG_H
#define CSIPSTRINGFEATURETAG_H

//  INCLUDES
#include "CSIPFeatureTag.h"

// CLASS DECLARATION
/**
* Class for string-valued (RFC 3840 page 14) SIP feature tags.
* @lib sipresolver.lib
*/
class CSIPStringFeatureTag : public CSIPFeatureTag
    {
    public: // Constructors and destructor

        static CSIPStringFeatureTag* NewL(
            RStringF aName, 
            const TDesC8& aValueWithBrackets);

        static CSIPStringFeatureTag* NewLC(
            RStringF aName,
            const TDesC8& aValueWithBrackets);

        ~CSIPStringFeatureTag();
    
    private: // Constructors

        CSIPStringFeatureTag();
        
        CSIPStringFeatureTag(RStringF aName);
    
        void ConstructL(const TDesC8& aValueWithBrackets);

    private: // From CSIPFeatureTag

        TBool MatchValue(const CSIPFeatureTag& aFeatureTag) const;
  
    private: // Data
       
        HBufC8* iValue;
    };

#endif // CSIPSTRINGFEATURETAG_H

// End of File
