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
* Name          : CSipContentTypeStrategy.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCONTENTTYPESTRATEGY_H
#define CSIPCONTENTTYPESTRATEGY_H

#include "CSipHeaderStrategyBase.h"

class CSIPResponse;
class CSIPRequest;
class MSipClients;
class MSipClient;
class CSIPHeaderBase;


class CSipContentTypeStrategy : public CSipHeaderStrategyBase
    {
    public:

        static CSipContentTypeStrategy* NewL(
            MSipClients& aSipClients,
            CSipHeaderStrategyBase* aNextStrategy);
            
        static CSipContentTypeStrategy* NewLC(
            MSipClients& aSipClients,
            CSipHeaderStrategyBase* aNextStrategy);
        
         ~CSipContentTypeStrategy ();


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

	    void EliminateClientsL(
	        CSIPRequest& aRequest,
	        RArray<TUid>& aUids);

    private: // New functions

        CSipContentTypeStrategy(
            MSipClients& aSipClients,
            CSipHeaderStrategyBase* aNextStrategy);
            
	    void SaveContentType(
	        CSIPRequest& aRequest);
	    
	    void CollectHeadersL(
	        MSipClient* aClient);
	    
	    void RemoveDuplicateAcceptHeaders();

    private: // Data

	    TPtrC8 iType;
	    TPtrC8 iSubType;
	    TBool iIsEmpty;
	    RPointerArray<CSIPHeaderBase> iSipAcceptHeaders;
    };

#endif // CSIPCONTENTTYPESTRATEGY_H

// End of File
