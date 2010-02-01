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
* Name          : CSipHeaderStrategyBase.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPHEADERSTRATEGYBASE_H
#define CSIPHEADERSTRATEGYBASE_H

#include <e32base.h>
#include <stringpool.h>
#include "_sipcodecdefs.h"

class CSIPResponse;
class CSIPRequest;
class MSipClients;
class MSipClient;
class CSIPContentTypeHeader;
class CSIPResolvedClient2;
class CSIPClientResolver2;
class CUri8;

class CSipHeaderStrategyBase : public CBase
    {
    public:

        virtual ~CSipHeaderStrategyBase();
                                              
        virtual CSIPResponse* ApplyL( CSIPRequest& aRequest, 
                                      RArray<TUid>& aUids,
                                      TBool& aContinueSearch,
                                      CSIPClientResolver2& 
                                      				aClientResolver2 ) = 0;
                                     
        TBool MatchL( CSIPResolvedClient2& aClient,CSIPRequest& aRequest,
    						 TUid& aUid );
    						 
    	CUri8* ConvertRequestUriL( CSIPRequest& aRequest );

    protected:

	    CSipHeaderStrategyBase(MSipClients& aSipClients,
	                           CSipHeaderStrategyBase* aNextStrategy,
    						   CSipHeaderStrategyBase* aNextStrategy2);

        CSIPResponse* MatchClientsL(CSIPRequest& aRequest,
                                         RArray<TUid>& aUids);
                                         
        CSIPResponse* MatchClients2L(
        							CSIPRequest& aRequest,
                                    RArray<TUid>& aUids,
    								TBool& aContinueSearch,
    								CSIPClientResolver2& aClientResolver2 );
        
        CSIPResponse* NextStrategyL(CSIPRequest& aRequest, 
    								RArray<TUid>& aUids,
    								TBool& aContinueSearch,
    								CSIPClientResolver2& aClientResolver2 );
    								
    	CSIPResponse* NextStrategy2L( CSIPRequest& aRequest, 
    								  RArray<TUid>& aUids,
    								  TBool& aContinueSearch,
    								  CSIPClientResolver2& aClientResolver2 );
    								
		CSIPResponse* NoMatchL( CSIPRequest& aRequest, 
    							RArray<TUid>& aUids,
    							TBool& aContinueSearch,
    							CSIPClientResolver2& aClientResolver2 );
        
    protected:
    	
    	virtual CSIPResponse* MoreThanOneMatchL(CSIPRequest& aRequest, 
    								RArray<TUid>& aUids,
    								TBool& aContinueSearch,
    								CSIPClientResolver2& aClientResolver2 );
    								
    	virtual TBool MatchResolvedClientL( CSIPResolvedClient2& aClient,
    								CSIPRequest& aRequest,
    								TUid& aClientUid,
    								const CUri8& aUri8 );
    								
        virtual void EliminateClientsL(CSIPRequest& aRequest,
                                            RArray<TUid>& aUids);
                                               
        virtual TBool CompareHeaders(CSIPRequest& aRequest,
                                     MSipClient& aClient);
                                     
        virtual CSIPResponse* CreateResponseL();
        
        /**
        * @return 0 if the ContentType header was not found,
        *          otherwise the header. The ownership is NOT transferred.
        */
        virtual CSIPContentTypeHeader* ContentType(CSIPRequest& aRequest);

    protected: // Data

        MSipClients& iSipClients;
        CSipHeaderStrategyBase* iNextStrategy; // Not owned
        CSipHeaderStrategyBase* iNextStrategy2; // Not owned
    };

#endif // CSIPHEADERSTRATEGYBASE_H

// End of File
