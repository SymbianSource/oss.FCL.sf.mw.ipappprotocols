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
* Name          : CSIPFeatureSet.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef CSIPFEATURESET_H
#define CSIPFEATURESET_H

//  INCLUDES
#include <e32base.h>
#include <stringpool.h>

// FORWARD DECLARATIONS
class CSIPAcceptContactHeader;
class CSIPFeatureTag;

// CLASS DECLARATION
/**
* Container for feature tags
* @lib sipresolver.lib
*/
class CSIPFeatureSet : public CBase
    {
    public:	// Constructors and destructor

		/**
		* Creates a new empty instance of CSIPFeatureSet
		* @return a new instance of CSIPFeatureSet
		*/
        static CSIPFeatureSet* NewL();

		/**
		* Creates a new empty instance of CSIPFeatureSet and 
		* puts it to CleanupStack
		* @return a new instance of CSIPFeatureSet
		*/
        static CSIPFeatureSet* NewLC();

		/**
		* Creates a new instance of CSIPFeatureSet
		* @param aAcceptContactHeader the header from which 
		*        the feature set is extracted from.
		* @return a new instance of CSIPFeatureSet
		*/
        static CSIPFeatureSet* NewL(
		    const CSIPAcceptContactHeader& aAcceptContactHeader);

		/**
		* Creates a new instance of CSIPFeatureSet and puts it to CleanupStack
		* @param aAcceptContactHeader the header from which 
		*        the feature set is extracted from.
		* @return a new instance of CSIPFeatureSet
		*/
        static CSIPFeatureSet* NewLC(
		    const CSIPAcceptContactHeader& aAcceptContactHeader);

		/**
		* Destructor, deletes the resources of CSIPFeatureSet.
		*/
        ~CSIPFeatureSet();

    public: // New functions

		/**
		* Adds new features from a Accept-Contact-header
		* @param aAcceptContactHeader the header from which 
		*        the feature set is extracted from.
		*/
        void AddFeaturesL(const CSIPAcceptContactHeader& aAcceptContactHeader);

		/**
		* Checks whether "require"-tag is present.
		* @return ETrue if present, otherwise EFalse.
		*/
        TBool Require() const;
    
		/**
		* Calculates the matching score with another feature set.
		* @param aFeatureSet the compared feature set.
		* @return if zero the feature set does not match,
		*         otherwise a non-negative matching score
		*/
        TInt CalculateScore(const CSIPFeatureSet& aFeatureSet) const;
    
    private: // Constructors    
   
        CSIPFeatureSet();

        void ConstructL(const CSIPAcceptContactHeader& aAcceptContactHeader);
        
    private: // New functions
            
        void AddFeatureTagsL(
            RStringF aName,
            const TDesC8& aValue);
        
        void AddFeatureTagL(
            RStringF aName,
            const TDesC8& aValue);        
        
        void AddBooleanFeatureTagL(
            RStringF aName,
            TBool aNegation,
            TBool aValue);
            
        void RemoveQuotesL(
            const TDesC8& aValue,
            TPtrC8& aWithoutQuotes) const;
            
        TBool StartsAndEndsWithQuotes(const TDesC8& aValue) const;
            
        TBool IsFeatureParamName(RStringF aName) const;
                   
    private: // Data
    
        RPointerArray<CSIPFeatureTag> iFeatureTags;
        TBool iRequire;
        
    private: // For testing purposes
#ifdef CPPUNIT_TEST    
        friend class CSIPFeatureSetTest;
#endif
    };

#endif // CSIPFEATURESET_H

// End of File
