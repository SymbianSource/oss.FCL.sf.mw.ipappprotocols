// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : CSipAcceptContactStrategy.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSipAcceptContactStrategy.h"
#include "MSipClients.h"
#include "MSipClient.h"
#include "sipresponse.h"
#include "siprequest.h"
#include "sipacceptcontactheader.h"
#include "CSIPFeatureSet.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "TSIPClientScore.h"
#include "SIPCRLogs.h"
	
const TUint K480ResponseCode = 480;


// ----------------------------------------------------------------------------
// CSipAcceptContactStrategy::NewL
// ----------------------------------------------------------------------------
//
CSipAcceptContactStrategy* CSipAcceptContactStrategy::NewL(
    MSipClients& aSipClients,
    CSipHeaderStrategyBase* aNextStrategy,
    CSipHeaderStrategyBase* aNextStrategy2 )
	{
	CSipAcceptContactStrategy* self = 
		CSipAcceptContactStrategy::NewLC( aSipClients,
										  aNextStrategy,
										  aNextStrategy2 );
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSipAcceptContactStrategy::NewLC
// ----------------------------------------------------------------------------
//
CSipAcceptContactStrategy* CSipAcceptContactStrategy::NewLC(
    MSipClients& aSipClients,
    CSipHeaderStrategyBase* aNextStrategy,
    CSipHeaderStrategyBase* aNextStrategy2 )
	{
	CSipAcceptContactStrategy* self = 
		new(ELeave)CSipAcceptContactStrategy( aSipClients,
											 aNextStrategy,
											 aNextStrategy2 );
	CleanupStack::PushL(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSipAcceptContactStrategy::CSipAcceptContactStrategy
// ----------------------------------------------------------------------------
//
CSipAcceptContactStrategy::CSipAcceptContactStrategy(
    MSipClients& aSipClients,
    CSipHeaderStrategyBase* aNextStrategy,
    CSipHeaderStrategyBase* aNextStrategy2 ) 
 : CSipHeaderStrategyBase( aSipClients,aNextStrategy,aNextStrategy2 )
	{
	iAcceptContactHeaderName = 
	    SIPStrings::StringF(SipStrConsts::EAcceptContactHeader);
	}

// ----------------------------------------------------------------------------
// CSipAcceptContactStrategy::~CSipAcceptContactStrategy
// ----------------------------------------------------------------------------
//
CSipAcceptContactStrategy::~CSipAcceptContactStrategy ()
	{
	}

// ----------------------------------------------------------------------------
// CSipAcceptContactStrategy::ApplyL
// ----------------------------------------------------------------------------
//
CSIPResponse* CSipAcceptContactStrategy::ApplyL(
    CSIPRequest& aRequest, 
    RArray<TUid>& aUids,
    TBool& aContinueSearch,
    CSIPClientResolver2& aClientResolver2 )
	{
	SIP_CR_LOG("CSipAcceptContactStrategy::ApplyL")
	CSIPResponse* response = NULL;
	// The strategy is applied only for requests that contain Accept-Contact
	if (aRequest.HasHeader(iAcceptContactHeaderName))
        {
        RArray<TSIPClientScore> scores(1);
        CleanupClosePushL(scores);
        RPointerArray<CSIPFeatureSet> requestFeatureSets = 
            CreateFeatureSetsL(aRequest);
        TCleanupItem cleanupItem(DestroyFeatureSets,&requestFeatureSets);
	    CleanupStack::PushL(cleanupItem);
	    for (TInt i=0; i < aUids.Count(); i++)
		    {
		    TUid uid(aUids[i]);
		    MSipClient* client = iSipClients.GetByUID(uid);
		    if (client)
			    {
			    TInt score = CalculateScore(*client,requestFeatureSets);
			    if (score > 0)
			        {
			        TSIPClientScore clientScore(score,uid);
			        // The score is used as a key
			        scores.InsertInSignedKeyOrderAllowRepeatsL(clientScore);
			        }
			    }	
		    }
    	CleanupStack::PopAndDestroy(1); // cleanupItem
    	TInt clientCount = scores.Count();
    	aUids.Reset(); // empties the array
	    if (clientCount > 0)
	        {
	        // The scores are in increasing order. 
	        // The last is the best match. Reverse the order.
	        for (TInt i=scores.Count()-1; i>=0; i--)
	            {
	            // In the resulting array the first is the best match
	            aUids.AppendL(scores[i].iUid);
	            } 
	        if (iNextStrategy && clientCount > 1)
	            {
	            // Apply the next strategy only if 
	            // there are still more than one matching clients.
	            response = iNextStrategy->ApplyL(aRequest,aUids,
	                                             aContinueSearch,
	                                             aClientResolver2);
	            }
	        }
	    else
		    {
		    if ( iNextStrategy2 )
		    	{
		    	response = iNextStrategy2->ApplyL(aRequest,aUids,
		    									  aContinueSearch,
		    									  aClientResolver2);
		    	}
		    else
		    	{
		    	response = CreateResponseL();
		    	}
		    
		    }
		CleanupStack::PopAndDestroy(1); // scores
        }
    else
        {
        if (iNextStrategy)
            {
	        response = iNextStrategy->ApplyL(aRequest,aUids,aContinueSearch,
	        												aClientResolver2);
	        }
        }

    return response;
	}

// ----------------------------------------------------------------------------
// CSipAcceptContactStrategy::CreateResponseL
// ----------------------------------------------------------------------------
//
CSIPResponse* CSipAcceptContactStrategy::CreateResponseL()
    {
    return CSIPResponse::NewL(K480ResponseCode,
        SIPStrings::StringF(SipStrConsts::EPhraseTemporarilyNotAvailable));
    }

// ----------------------------------------------------------------------------
// CSipAcceptContactStrategy::CompareHeaders
// ----------------------------------------------------------------------------
//
TBool CSipAcceptContactStrategy::CompareHeaders(
    CSIPRequest& /*aRequest*/,
    MSipClient& /*aClient*/)
	{
	// Not implemented for this sub-class
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CSipAcceptContactStrategy::CreateFeatureSetsL
// ----------------------------------------------------------------------------
//
RPointerArray<CSIPFeatureSet> CSipAcceptContactStrategy::CreateFeatureSetsL(
    CSIPRequest& aRequest)
    {
    RPointerArray<CSIPFeatureSet> featureSets;
    TCleanupItem cleanupItem(DestroyFeatureSets,&featureSets);
	CleanupStack::PushL(cleanupItem);
	TSglQueIter<CSIPHeaderBase> iter = 
	    aRequest.Headers(iAcceptContactHeaderName);	
	while (iter)
		{
		CSIPHeaderBase* tmp = iter++;
		CSIPAcceptContactHeader* acceptContact = 
		    static_cast<CSIPAcceptContactHeader*>(tmp);
		CSIPFeatureSet* featureSet = CSIPFeatureSet::NewLC(*acceptContact);
		featureSets.AppendL(featureSet);
		CleanupStack::Pop(featureSet);
		}	
	CleanupStack::Pop(1); // cleanupItem 
    return featureSets;
    }

// ----------------------------------------------------------------------------
// CSipAcceptContactStrategy::CalculateScore
// ----------------------------------------------------------------------------
//	
TInt CSipAcceptContactStrategy::CalculateScore(
    MSipClient& aClient,
    const RPointerArray<CSIPFeatureSet>& aRequestFeatureSets) const
    {
    TInt score = 0;
    CSIPFeatureSet& clientFeatureSet = aClient.SIPFeatureSet();
    for (TInt i = 0; i < aRequestFeatureSets.Count(); i++)
        {
        CSIPFeatureSet& requestFeatureSet = *aRequestFeatureSets[i];
        TInt tmp = clientFeatureSet.CalculateScore(requestFeatureSet);
        if (tmp == 0 && requestFeatureSet.Require())
            {
            return 0;
            }
        score += tmp;
        }
    return score;
    }
    
// -----------------------------------------------------------------------------
// CSipAcceptContactStrategy::DestroyFeatureSets
// -----------------------------------------------------------------------------
//
void CSipAcceptContactStrategy::DestroyFeatureSets(TAny* aFeatureSets)
	{
    RPointerArray<CSIPFeatureSet>* featureSets =
        reinterpret_cast<RPointerArray<CSIPFeatureSet>*>(aFeatureSets);
    if (featureSets)
        {
        featureSets->ResetAndDestroy();
        }
	}
