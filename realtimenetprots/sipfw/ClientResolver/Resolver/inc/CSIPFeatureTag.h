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
* Name          : CSIPFeatureTag.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef CSIPFEATURETAG_H
#define CSIPFEATURETAG_H

//  INCLUDES
#include <e32base.h>
#include <stringpool.h>

// CLASS DECLARATION
/**
* Base class for SIP feature tags.
* @lib sipresolver.lib
*/
class CSIPFeatureTag : public CBase
    {
    public: // Destructor

        virtual ~CSIPFeatureTag();

    public: // New functions

        enum TType
            {
			EBooleanFeatureTag=1,
			ENumericFeatureTag,
			EStringFeatureTag,
			ETokenFeatureTag
            };
            
        TType Type() const;

        RStringF Name() const;
        
        TBool Negation() const;

        TBool Match(const CSIPFeatureTag& aFeatureTag) const;
    
    protected: // Constructors
    
        CSIPFeatureTag(TType aType, RStringF aName, TBool aNegation);
        
        CSIPFeatureTag();   
    
    protected: // New functions
    
        virtual TBool MatchValue(const CSIPFeatureTag& aFeatureTag) const = 0;
    
    protected: // Data
    
        TType iType;
        RStringF iName;
        TBool iNegation;
        
    private: // For testing purposes
#ifdef CPPUNIT_TEST        
        friend class CSIPNumericFeatureTagTest;
#endif
    };

#endif // CSIPFEATURETAG_H

// End of File
