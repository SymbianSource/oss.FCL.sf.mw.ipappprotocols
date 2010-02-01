// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPClientConnectionReceiver.cpp
// Part of       : SIPClient
// Version       : SIP/4.0 
//



#include "CSIPClientConnectionReceiver.h"
#include "RSIPConnection.h"
#include "sipclientconnectionobserver.h"
#include "siprequestelements.h"
#include "sipresponseelements.h"
#include "sipmessageelements.h"
#include "sipinternalerr.h"


// -----------------------------------------------------------------------------
// CSIPClientConnectionReceiver::CSIPClientConnectionReceiver
// -----------------------------------------------------------------------------
//
CSIPClientConnectionReceiver::CSIPClientConnectionReceiver (
                                        RSIPConnection& aConnection,
                                        MSIPClientConnectionObserver& aObserver) 
 : CSIPITCReceiverBase (aConnection),
   iConnection (aConnection),
   iObserver (aObserver)
	{
	}

// -----------------------------------------------------------------------------
// CSIPClientConnectionReceiver::~CSIPClientConnectionReceiver
// -----------------------------------------------------------------------------
//
CSIPClientConnectionReceiver::~CSIPClientConnectionReceiver () 
	{
	}

// -----------------------------------------------------------------------------
// CSIPClientConnectionReceiver::IncomingRequest
// -----------------------------------------------------------------------------
//
void 
CSIPClientConnectionReceiver::IncomingRequestL (TSIPIds& aIds,
                                                CSIPRequestElements* aRequest)
    {
    TUint32 requestId = aIds.iRequestId;
	if (aIds.iDialogId != 0)
		{
		iObserver.IncomingDialogRequestL(aRequest, requestId, aIds.iDialogId);
		}
	else
		{
		iObserver.IncomingRequestL(aRequest,requestId);
		}
    }

// -----------------------------------------------------------------------------
// CSIPClientConnectionReceiver::IncomingResponse
// -----------------------------------------------------------------------------
//
void 
CSIPClientConnectionReceiver::IncomingResponseL (TSIPIds& aIds,
                                                 CSIPResponseElements* aResponse)
    {
	TUint32 requestId = aIds.iRequestId;
	TUint32 registrationId = aIds.iRegistrationId;
	TUint32 dialogId = aIds.iDialogId;	
	TUint32 refreshId = aIds.iRefreshId;
	if (dialogId != 0)
		{	
        if (refreshId != 0)
            {
		    iObserver.IncomingDialogResponseL(aResponse, requestId,
		                                      refreshId, dialogId);
            }
        else
            {
            iObserver.IncomingDialogResponseL(aResponse,requestId,dialogId);
            }
		}
	else if (registrationId != 0)
		{
		iObserver.IncomingRegistrationResponseL(aResponse,requestId,
		                                        refreshId, registrationId);
		}
	else if (refreshId != 0)
		{
		iObserver.IncomingRefreshResponseL(aResponse,requestId,refreshId);
		}	
	else
		{
		iObserver.IncomingResponseL(aResponse,requestId);
		}
    }

// -----------------------------------------------------------------------------
// CSIPClientConnectionReceiver::ErrorOccuredL
// -----------------------------------------------------------------------------
//
void CSIPClientConnectionReceiver::ErrorOccuredL (TSIPIds& aIds, TInt aError)
	{
    switch (aError)
        {
        case CSIPConnection::EInit:
		case CSIPConnection::EActive:
		case CSIPConnection::ESuspended:
		case CSIPConnection::EInactive:
        case CSIPConnection::EUnavailable:
			ConnectionStateChanged(static_cast<CSIPConnection::TState>(aError));
            break;

        case KInviteTransactionCompleted:
            InviteCompleted(aIds);
            break;

        default: 
            ErrorOccured(aIds,aError);
            break;
		}
	}

// -----------------------------------------------------------------------------
// CSIPClientConnectionReceiver::ErrorOccured
// -----------------------------------------------------------------------------
//
void CSIPClientConnectionReceiver::ErrorOccured (TSIPIds& aIds, TInt aError)
    {
	TUint32 requestId = aIds.iRequestId;
	TUint32 registrationId = aIds.iRegistrationId;
	TUint32 dialogId = aIds.iDialogId;	
	TUint32 refreshId = aIds.iRefreshId;    
	if (dialogId != 0)
		{
        if (refreshId != 0)
            {
		    iObserver.DialogErrorOccured(aError,dialogId,refreshId,requestId);
            }
        else
            {
            iObserver.DialogErrorOccured(aError,dialogId,requestId);
            }
		}
	else if (registrationId != 0)
		{
		iObserver.RegistrationErrorOccured(aError,registrationId,requestId);
		}		
	else if (refreshId != 0)
		{
		iObserver.RefreshErrorOccured(aError,refreshId);
		}
	else
		{
		iObserver.ErrorOccured(aError,requestId);
		}
    }

// -----------------------------------------------------------------------------
// CSIPClientConnectionReceiver::InviteCompleted
// -----------------------------------------------------------------------------
//
void CSIPClientConnectionReceiver::InviteCompleted (TSIPIds& aIds)
    {
    iObserver.InviteCompleted(aIds.iRequestId);
    }

// -----------------------------------------------------------------------------
// CSIPClientConnectionReceiver::ConnectionStateChanged
// -----------------------------------------------------------------------------
//
void CSIPClientConnectionReceiver::ConnectionStateChanged (
    CSIPConnection::TState aState)
    {
	iConnection.SetState(aState);
	iObserver.ConnectionStateChanged(aState);
    }
