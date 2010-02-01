// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSdpMLineStrategyBase.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSdpMLineStrategyBase.h"
#include "MSipClients.h"
#include "MSipClient.h"
#include "sipresponse.h"
#include "sdpdocument.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

const TUint K488ResponseCode = 488;

// ----------------------------------------------------------------------------
// CSdpMLineStrategyBase::CSdpMLineStrategyBase
// ----------------------------------------------------------------------------
//
CSdpMLineStrategyBase::CSdpMLineStrategyBase(const MSipClients& aSipClients)
: CSdpStrategyBase(aSipClients)
	{
	}

// ----------------------------------------------------------------------------
// CSdpMLineStrategyBase::~CSdpMLineStrategyBase
// ----------------------------------------------------------------------------
//
CSdpMLineStrategyBase::~CSdpMLineStrategyBase()
	{	
	}

// ----------------------------------------------------------------------------
// CSdpMLineStrategyBase::ApplyL
// ----------------------------------------------------------------------------
//
CSIPResponse* CSdpMLineStrategyBase::ApplyL(
    CSdpDocument& aDocument, 
    RArray<TUid>& aUids)
	{
	CSIPResponse* response = MediaLineExistL(aDocument);
	if (!response && IsApplicable(aDocument))
	    {
	    FindApplicationL(aDocument,aUids);
	    if(aUids.Count() == 0)
		    {
			response = CSIPResponse::NewL(
			    K488ResponseCode,
			    SIPStrings::StringF(SipStrConsts::EPhraseNotAcceptableHere));
		    }
	    }
	return response;
	}

// ----------------------------------------------------------------------------
// CSdpMLineStrategyBase::FindApplicationL
// ----------------------------------------------------------------------------
//
void CSdpMLineStrategyBase::FindApplicationL(CSdpDocument& aDocument,
                                             RArray<TUid>& aUids)
    {
	for(TInt i=0; i < aUids.Count();i++)
		{
		MSipClient* client = iSipClients.GetByUID(aUids[i]);
		if (!client || !MatchL(aDocument,*client))
			{
			aUids.Remove(i);
			i--;
			}		
		}
    }

// ----------------------------------------------------------------------------
// CSdpMLineStrategyBase::MediaLineExist
// ----------------------------------------------------------------------------
//
CSIPResponse* CSdpMLineStrategyBase::MediaLineExistL(CSdpDocument& aDocument)
 	{
   	if(aDocument.MediaFields().Count() == 0)
        {
        return CSIPResponse::NewL(K488ResponseCode,
            SIPStrings::StringF(SipStrConsts::EPhraseNotAcceptableHere));
        }
    return NULL;
    }
	
// ----------------------------------------------------------------------------
// CSdpMLineStrategyBase::MatchL
// ----------------------------------------------------------------------------
//
TBool CSdpMLineStrategyBase::MatchL(CSdpDocument& aDocument,
                                    MSipClient& aClient)
    {
	const RPointerArray<CSdpMediaField>& clientMedias = 
	    aClient.SdpDocument().MediaFields();
	
	const RPointerArray<CSdpMediaField>& receivedMedias = 
	    aDocument.MediaFields();	
	
	// All the media fields in received request must be supported 
	// by the client. If not the client is removed from the list of clients,
	// that are capable of handling the request.  
	for (TInt i=0; i < receivedMedias.Count(); i++)
	    {
	    CSdpMediaField* receivedMedia = receivedMedias[i];
	    if (IsApplicable(*receivedMedia))
	        {
	        TBool mediaFound = EFalse;
	        for (TInt j=0; j < clientMedias.Count() && !mediaFound; j++)
	            {
	            if (FieldsMatchL(*receivedMedia,*(clientMedias[j])))
	                {
	                mediaFound = ETrue;
	                }
	            }
	        if (!mediaFound)
	            {
	            return EFalse;
	            }
	        }
	    }

	return ETrue;
	}

// ----------------------------------------------------------------------------
// CSdpMLineStrategyBase::IsApplicable
// ----------------------------------------------------------------------------
//	
TBool CSdpMLineStrategyBase::IsApplicable(CSdpDocument& aDocument)
    {
 	RPointerArray<CSdpMediaField>& mediaFields = aDocument.MediaFields();
	for (TInt i=0; i < mediaFields.Count(); i++)
	    {
	    if (IsApplicable(*mediaFields[i]))
	        {
	        return ETrue;
	        }
	    }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CSdpMLineStrategyBase::IsApplicable
// ----------------------------------------------------------------------------
//	
TBool CSdpMLineStrategyBase::IsApplicable(CSdpMediaField& /*aMediaField*/)
    {
    // default implementation
    return ETrue;
    }
