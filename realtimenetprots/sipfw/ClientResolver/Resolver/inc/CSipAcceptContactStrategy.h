/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSipAcceptContactStrategy.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPACCEPTCONTACTSTRATEGY_H
#define CSIPACCEPTCONTACTSTRATEGY_H

#include "CSipHeaderStrategyBase.h"

class CSIPAcceptContactHeader;
class CSIPFeatureSet;


class CSipAcceptContactStrategy : public CSipHeaderStrategyBase
    {
    public:

        static CSipAcceptContactStrategy* NewL(
            MSipClients& aSipClients,
            CSipHeaderStrategyBase* aNextStrategy,
    		CSipHeaderStrategyBase* aNextStrategy2 );

        static CSipAcceptContactStrategy* NewLC(
            MSipClients& aSipClients,
            CSipHeaderStrategyBase* aNextStrategy,
    		CSipHeaderStrategyBase* aNextStrategy2 );
		    
		~CSipAcceptContactStrategy();

    public: // From CSipHeaderStrategyBase

        /// The returned UIDs are ordered so that 
        /// the most preferred one is the first in the array.
	    CSIPResponse* ApplyL(
	        CSIPRequest& aRequest,
	        RArray<TUid>& aUids,
	        TBool& aContinueSearch,
    		CSIPClientResolver2& aClientResolver2 );

    protected: // From CSipHeaderStrategyBase

        CSIPResponse* CreateResponseL();
        
        TBool CompareHeaders(
            CSIPRequest& aRequest,
            MSipClient& aClient);

    private: // Constructors

        CSipAcceptContactStrategy(
            MSipClients& aSipClients,
            CSipHeaderStrategyBase* aNextStrategy,
    		CSipHeaderStrategyBase* aNextStrategy2 );

    private: // New functions
    
        RPointerArray<CSIPFeatureSet> CreateFeatureSetsL(
            CSIPRequest& aRequest);
    
        TInt CalculateScore(
            MSipClient& aClient,
            const RPointerArray<CSIPFeatureSet>& aRequestFeatureSets) const;

        static void DestroyFeatureSets(TAny* aFeatureSets);

    private: // Data
    
        RStringF iAcceptContactHeaderName;

    private: // For testing purposes

	    UNIT_TEST(CSipAcceptContactStrategyTest)
    };

#endif // CSIPACCEPTCONTACTSTRATEGY_H

// End of File
