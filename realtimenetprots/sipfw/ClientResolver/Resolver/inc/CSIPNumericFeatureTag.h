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
* Name          : CSIPNumericFeatureTag.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef CSIPNUMERICFEATURETAG_H
#define CSIPNUMERICFEATURETAG_H

//  INCLUDES
#include "CSIPFeatureTag.h"

// CLASS DECLARATION
/**
* Class for numeric-valued (RFC 3840 page 14) SIP feature tags.
* @lib sipresolver.lib
*/
class CSIPNumericFeatureTag : public CSIPFeatureTag
    {
    public: // Constructors and destructor

        static CSIPNumericFeatureTag* NewL(
            RStringF aName,
            TBool aNegation, 
            const TDesC8& aValueNoHash);

        static CSIPNumericFeatureTag* NewLC(
            RStringF aName,
            TBool aNegation,
            const TDesC8& aValueNoHash);

        ~CSIPNumericFeatureTag();
    
    private: // Constructors

        CSIPNumericFeatureTag();
        
        CSIPNumericFeatureTag(RStringF aName, TBool aNegation);
    
        void ConstructL(const TDesC8& aValueNoHash);

    private: // From CSIPFeatureTag

        TBool MatchValue(const CSIPFeatureTag& aFeatureTag) const;
   
    private: // New functions
    
        enum TRelation
            {
			ELessThanOrEqual=1,
			EEqual,
			EGreaterThanOrEqual,
			ERange
            };    
    
        TInt ParseRelation(TLex8& aLexer, 
                           TRelation& aRelation);
        
        TInt ParseRange(const TDesC8& aDes,
                        TReal& aLowerBound,
                        TReal& aUpperBound);
        
        TInt ParseNumber(const TDesC8& aDes,
                         TReal& aValue);
                         
        TBool IsWithinBounds(TReal aLowerBound,
                             TReal aUpperBound,
                             TBool aNegation) const;
   
    private: // Data        
        
        TReal iLowerBound;
        TReal iUpperBound;
        
    private: // For testing purposes
#ifdef CPPUNIT_TEST    
        friend class CSIPNumericFeatureTagTest;   
#endif
    };

#endif // CSIPNUMERICFEATURETAG_H

// End of File
