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
* Name          : CSipAllowEventsStrategy2.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPALLOWEVENTSSTRATEGY2_H
#define CSIPALLOWEVENTSSTRATEGY2_H

#include "CSipHeaderStrategyBase.h"

NONSHARABLE_CLASS(CSipAllowEventsStrategy2)
: public CSipHeaderStrategyBase
    {
    public:

        static CSipAllowEventsStrategy2* NewL(
            MSipClients& aSipClients,
            CSipHeaderStrategyBase* aNextStrategy,
    		CSipHeaderStrategyBase* aNextStrategy2);
            
        static CSipAllowEventsStrategy2* NewLC(
            MSipClients& aSipClients,
            CSipHeaderStrategyBase* aNextStrategy,
    		CSipHeaderStrategyBase* aNextStrategy2);
        
        ~CSipAllowEventsStrategy2 ();

    public: // From CSipHeaderStrategyBase

        CSIPResponse* ApplyL(
            CSIPRequest& aRequest, 
            RArray<TUid>& aUids,
            TBool& aContinueSearch,
    		CSIPClientResolver2& aClientResolver2 );

    protected: // From CSipHeaderStrategyBase
            
        TBool MatchResolvedClientL( CSIPResolvedClient2& aClient,
        							CSIPRequest& aRequest,
        							TUid& aClientUid,
        							const CUri8& aUri8 );

    private:

        CSipAllowEventsStrategy2(
            MSipClients& aSipClients,
            CSipHeaderStrategyBase* aNextStrategy,
    		CSipHeaderStrategyBase* aNextStrategy2);

    private: // Data
    
    	RStringF iEventHeaderName;
    	
    private: // For testing purposes

	    UNIT_TEST(CSipAllowEventsStrategy2Test)
    };

#endif // CSIPALLOWEVENTSSTRATEGY2_H

// End of File
