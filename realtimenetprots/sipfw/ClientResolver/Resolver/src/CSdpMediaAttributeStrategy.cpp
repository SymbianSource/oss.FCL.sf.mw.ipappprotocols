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
// Name          : CSdpMediaAttributeStrategy.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSdpMediaAttributeStrategy.h"
#include "MSipClients.h"
#include "MSipClient.h"
#include "sdpdocument.h"
#include "sdpmediafield.h"
#include "sdpattributefield.h"
#include "sdpcodecstringconstants.h"
#include "sdpcodecstringpool.h"

// ----------------------------------------------------------------------------
// CSdpMediaAttributeStrategy::NewLC
// ----------------------------------------------------------------------------
//
CSdpMediaAttributeStrategy* CSdpMediaAttributeStrategy::NewLC(
    const MSipClients& aSipClients)
	{
	CSdpMediaAttributeStrategy* self = 
		new(ELeave)CSdpMediaAttributeStrategy(aSipClients);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CSdpMediaAttributeStrategy::CSdpMediaAttributeStrategy
// ----------------------------------------------------------------------------
//
CSdpMediaAttributeStrategy::CSdpMediaAttributeStrategy(
    const MSipClients& aSipClients) 
 : CSdpStrategyBase(aSipClients)
	{
	}

// ----------------------------------------------------------------------------
// CSdpMediaAttributeStrategy::ConstructL
// ----------------------------------------------------------------------------
//
void CSdpMediaAttributeStrategy::ConstructL()
	{
	CSdpStrategyBase::ConstructL();
	}

// ----------------------------------------------------------------------------
// CSdpMediaAttributeStrategy::~CSdpMediaAttributeStrategy
// ----------------------------------------------------------------------------
//
CSdpMediaAttributeStrategy::~CSdpMediaAttributeStrategy ()
	{
	}

// ----------------------------------------------------------------------------
// CSdpMediaAttributeStrategy::ApplyL
// ----------------------------------------------------------------------------
//	
CSIPResponse* CSdpMediaAttributeStrategy::ApplyL(
    CSdpDocument& aDocument, 
    RArray<TUid>& aUids)
    {	
	RArray<TUid> uids;
	CleanupClosePushL(uids);
	
    for (TInt i=0; i<aUids.Count(); i++)
        {
        uids.AppendL(aUids[i]);
        }
	
	for (TInt i=uids.Count()-1; i>=0; i--)
		{
		MSipClient* client = iSipClients.GetByUID(uids[i]);
		if (client)
			{
        	if (!Match(aDocument,*client))
        	    {
			    uids.Remove(i);
        	    }           
			}		
		}
    
	if (uids.Count() > 0)
	    {
	    aUids.Reset();
        for (TInt i=0; i<uids.Count(); i++)
            {
            aUids.AppendL(uids[i]);
            }	    
	    }

    CleanupStack::PopAndDestroy(&uids);
    return NULL;   
    }
 
// ----------------------------------------------------------------------------
// CSdpMediaAttributeStrategy::Match
// ----------------------------------------------------------------------------
//
TBool CSdpMediaAttributeStrategy::Match(
    CSdpDocument& aDocument,
    MSipClient& aClient)
    {    
	RPointerArray<CSdpMediaField>& clientMedias = 
	    aClient.SdpDocument().MediaFields();
	
	RPointerArray<CSdpMediaField>& receivedMedias = 
	    aDocument.MediaFields();	
	  
	for (TInt i=0; i < receivedMedias.Count(); i++)
	    {
	    CSdpMediaField* receivedMedia = receivedMedias[i];
	    for (TInt j=0; j < clientMedias.Count(); j++)
            {
            CSdpMediaField* clientMedia = clientMedias[j];
            if (HasMatchingAttribute(*receivedMedia,*clientMedia))
                {
                return ETrue;
                }
	        }
	    }

	return EFalse;
	}
    
// ----------------------------------------------------------------------------
// CSdpMediaAttributeStrategy::HasMatchingAttribute
// ----------------------------------------------------------------------------
//    
TBool CSdpMediaAttributeStrategy::HasMatchingAttribute(
    CSdpMediaField& aReceivedMedia,
    CSdpMediaField& aClientMedia)
    {
    if (aReceivedMedia.Media() != aClientMedia.Media())
        {
        return EFalse;
        }
    
    const RPointerArray<CSdpAttributeField>& clientAttributes = 
	    aClientMedia.AttributeFields();	
	
    const RPointerArray<CSdpAttributeField>& receivedAttributes = 
	    aReceivedMedia.AttributeFields();	
	  
	for (TInt i=0; i < clientAttributes.Count(); i++)
	    {
    	for (TInt j=0; j < receivedAttributes.Count(); j++)
    	    {
    	    if (*clientAttributes[i] == *receivedAttributes[j])
    	        {
    	        return ETrue;
    	        }
    	    }
	    }    
    
    return EFalse;
    }
