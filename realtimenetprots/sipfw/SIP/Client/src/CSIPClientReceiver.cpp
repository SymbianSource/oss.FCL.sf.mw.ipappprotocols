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
// Name          : CSIPClientReceiver.cpp
// Part of       : SIPClient
// Version       : SIP/5.0 
//



#include "CSIPClientReceiver.h"
#include "RSIP.h"
#include "sipclientobserver.h"
#include "sipresponseelements.h"
#include "siphttpdigestchallengeobserver2.h"

// -----------------------------------------------------------------------------
// CSIPClientReceiver::CSIPClientReceiver
// -----------------------------------------------------------------------------
//
CSIPClientReceiver::CSIPClientReceiver(
    RSIP& aSip,
    MSIPClientObserver& aObserver) 
 : CSIPITCReceiverBase (aSip),
   iObserver (aObserver)
	{
	}

// -----------------------------------------------------------------------------
// CSIPClientReceiver::~CSIPClientReceiver
// -----------------------------------------------------------------------------
//
CSIPClientReceiver::~CSIPClientReceiver() 
	{
	}

// -----------------------------------------------------------------------------
// CSIPClientReceiver::IncomingRequestL
// -----------------------------------------------------------------------------
//
void CSIPClientReceiver::IncomingRequestL(
    TSIPIds& aIds,
    CSIPRequestElements* aRequest)
    {
    iObserver.IncomingRequestL(aIds.iIapId, aRequest, aIds.iRequestId);
    }

// -----------------------------------------------------------------------------
// CSIPClientReceiver::IncomingResponseL
// -----------------------------------------------------------------------------
//
void CSIPClientReceiver::IncomingResponseL(
    TSIPIds& aIds,
    CSIPResponseElements* aResponse)
    {
    if (aIds.iRefreshId)
        {
        iObserver.ChallengeReceivedInRefreshL(aIds.iRefreshId, aIds.iRequestId,
                                              aResponse, *iHttpDigestObserver2);
        }
    else if (aIds.iRequestId)
        {
        iObserver.ChallengeReceivedL(aIds.iRequestId, aResponse,
                                     *iHttpDigestObserver2);
        }
    else
        {
        // Should not happen, just delete the response to prevent memory leak
        delete aResponse;
        }
    }

// -----------------------------------------------------------------------------
// CSIPClientReceiver::ErrorOccuredL
// -----------------------------------------------------------------------------
//
void CSIPClientReceiver::ErrorOccuredL(TSIPIds& aIds, TInt aError)
	{
    if (aError == KErrTimedOut)
        {
        iObserver.TimeOut(aIds.iRequestId);
        }
	}
