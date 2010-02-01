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
* Name          : CSipAllowEventsStrategy.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPALLOWEVENTSSTRATEGY_H
#define CSIPALLOWEVENTSSTRATEGY_H

#include "CSipHeaderStrategyBase.h"


class CSipAllowEventsStrategy : public CSipHeaderStrategyBase
    {
    public:

        static CSipAllowEventsStrategy* NewL(
            MSipClients& aSipClients,
            CSipHeaderStrategyBase* aNextStrategy,
    		CSipHeaderStrategyBase* aNextStrategy2 );
            
        static CSipAllowEventsStrategy* NewLC(
            MSipClients& aSipClients,
            CSipHeaderStrategyBase* aNextStrategy,
    		CSipHeaderStrategyBase* aNextStrategy2 );
        
        ~CSipAllowEventsStrategy ();

    public: // From CSipHeaderStrategyBase

        CSIPResponse* ApplyL( CSIPRequest& aRequest, 
            				  RArray<TUid>& aUids,
            				  TBool& aContinueSearch,
    						  CSIPClientResolver2& aClientResolver2 );

    protected: // From CSipHeaderStrategyBase

        CSIPResponse* CreateResponseL();
        
        TBool CompareHeaders(
            CSIPRequest& aRequest,
            MSipClient& aClient);

    private:

        CSipAllowEventsStrategy(
            MSipClients& aSipClients,
            CSipHeaderStrategyBase* aNextStrategy,
    		CSipHeaderStrategyBase* aNextStrategy2 );

    private: // Data
    
        RStringF iEventHeaderName;
        RStringF iAllowEventsHeaderName;

    private: // For testing purposes

	    UNIT_TEST(CSipAllowEventsStrategyTest)
    };

#endif // CSIPALLOWEVENTSSTRATEGY_H

// End of File
