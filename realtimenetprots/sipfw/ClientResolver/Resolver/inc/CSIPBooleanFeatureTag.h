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
* Name          : CSIPBooleanFeatureTag.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef CSIPBOOLEANFEATURETAG_H
#define CSIPBOOLEANFEATURETAG_H

//  INCLUDES
#include "CSIPFeatureTag.h"

// CLASS DECLARATION
/**
* Class for boolean-valued (RFC 3840 page 14) SIP feature tags.
* @lib sipresolver.lib
*/
class CSIPBooleanFeatureTag : public CSIPFeatureTag
    {
    public: // Constructors and destructor

        static CSIPBooleanFeatureTag* NewL(
            RStringF aName,
            TBool aNegation, 
            TBool aValue);

        static CSIPBooleanFeatureTag* NewLC(
            RStringF aName,
            TBool aNegation, 
            TBool aValue);

        ~CSIPBooleanFeatureTag();

    private: // From CSIPFeatureTag

        TBool MatchValue(const CSIPFeatureTag& aFeatureTag) const;
    
    private: // Constructors

        CSIPBooleanFeatureTag();
        
        CSIPBooleanFeatureTag(
            RStringF aName,
            TBool aNegation, 
            TBool aValue);
    
    private: // Data
       
        TBool iValue;
    };

#endif // CSIPBOOLEANFEATURETAG_H

// End of File
