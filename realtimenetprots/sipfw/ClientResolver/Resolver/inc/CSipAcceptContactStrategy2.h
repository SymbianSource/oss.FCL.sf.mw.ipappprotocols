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
* Name          : CSipAcceptContactStrategy2.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPACCEPTCONTACTSTRATEGY2_H
#define CSIPACCEPTCONTACTSTRATEGY2_H

#include "CSipHeaderStrategyBase.h"

class CSIPAcceptContactHeader;

NONSHARABLE_CLASS(CSipAcceptContactStrategy2) 
: public CSipHeaderStrategyBase
    {
    public:

        static CSipAcceptContactStrategy2* NewL( 
            						MSipClients& aSipClients,
            						CSipHeaderStrategyBase* aNextStrategy,
            						CSipHeaderStrategyBase* aNextStrategy2 );

        static CSipAcceptContactStrategy2* NewLC( 
            						MSipClients& aSipClients,
            						CSipHeaderStrategyBase* aNextStrategy,
            						CSipHeaderStrategyBase* aNextStrategy2 );
		    
		~CSipAcceptContactStrategy2();

    public: // From CSipHeaderStrategyBase

	    CSIPResponse* ApplyL( CSIPRequest& aRequest,
	        				  RArray<TUid>& aUids,
	        				  TBool& aContinueSearch,
	        				  CSIPClientResolver2& aClientResolver2 );

    protected: // From CSipHeaderStrategyBase
            				  
        TBool MatchResolvedClientL( CSIPResolvedClient2& aClient,
        							CSIPRequest& aRequest,
    								TUid& aClientUid,
    								const CUri8& aUri8 );

    private: // Constructors

        CSipAcceptContactStrategy2( MSipClients& aSipClients,
            						CSipHeaderStrategyBase* aNextStrategy,
            						CSipHeaderStrategyBase* aNextStrategy2 );
        
    private: // Data
    
        RStringF iAcceptContactHeaderName;

    private: // For testing purposes

	    UNIT_TEST( CSipAcceptContactStrategy2Test )
    };

#endif // CSIPACCEPTCONTACTSTRATEGY2_H

// End of File
