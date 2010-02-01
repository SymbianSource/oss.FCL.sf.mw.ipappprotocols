/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSipRequestStrategy.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPREQUESTSTRATEGY_H
#define CSIPREQUESTSTRATEGY_H

#include "CSipHeaderStrategyBase.h"

class CSIPContentTypeHeader;
NONSHARABLE_CLASS(CSipRequestStrategy)
: public CSipHeaderStrategyBase
    {
    public:

        static CSipRequestStrategy* NewL(
            MSipClients& aSipClients,
            CSipHeaderStrategyBase* aNextStrategy,
    		CSipHeaderStrategyBase* aNextStrategy2);
            
        static CSipRequestStrategy* NewLC(
            MSipClients& aSipClients,
            CSipHeaderStrategyBase* aNextStrategy,
    		CSipHeaderStrategyBase* aNextStrategy2);
        
        ~CSipRequestStrategy ();

    public: // From CSipHeaderStrategyBase

        CSIPResponse* ApplyL( CSIPRequest& aRequest, 
            				  RArray<TUid>& aUids,
            				  TBool& aContinueSearch,
    						  CSIPClientResolver2& aClientResolver2 );

    protected: // From CSipHeaderStrategyBase
        
		CSIPResponse* MoreThanOneMatchL( CSIPRequest& aRequest, 
    								RArray<TUid>& aUids,
    								TBool& aContinueSearch,
    								CSIPClientResolver2& aClientResolver2 );
    								
    	TBool MatchResolvedClientL( CSIPResolvedClient2& aClient,
    								CSIPRequest& aRequest,
    								TUid& aClientUid,
    								const CUri8& aUri8 );
    	
    private:

        CSipRequestStrategy(
            MSipClients& aSipClients,
            CSipHeaderStrategyBase* aNextStrategy,
    		CSipHeaderStrategyBase* aNextStrategy2);	

    private: // For testing purposes

	    UNIT_TEST(CSipRequestStrategyTest)
    };

#endif // CSIPREQUESTSTRATEGY_H

// End of File
