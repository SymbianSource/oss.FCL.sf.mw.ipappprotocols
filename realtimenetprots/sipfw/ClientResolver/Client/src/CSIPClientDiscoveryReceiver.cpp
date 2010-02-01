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
// Name          : CSIPClientResolverReceiver.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0 
//



// INCLUDES
#include "CSIPClientDiscoveryReceiver.h"
#include "sipresponse.h"
#include "MSIPClientDiscoveryITC.h"
#include "sipclientdiscoveryobserver.h"
#include "CSIPClientDiscoverySerializer.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPClientDiscoveryReceiver::CSIPClientDiscoveryReceiver
// -----------------------------------------------------------------------------
//
CSIPClientDiscoveryReceiver::CSIPClientDiscoveryReceiver (
                                    MSIPClientDiscoveryITC& aITC, 
                                    MSIPClientDiscoveryObserver& aObserver,
                                    CSIPClientDiscoverySerializer& aSerializer)
: CActive (EPriorityStandard),
  iObserver (aObserver),
  iSerializer (aSerializer),
  iITC(aITC)
    {
    CActiveScheduler::Add (this);
    iITCMsgArgs.Set(ESIPCRIpcArgReqId, &iIdsPckg);
    iITCMsgArgs.Set(ESIPCRIpcArgClientResolved, &iClientResolved);
    iITCMsgArgs.Set(ESIPCRIpcArgBufSizes, &iSizesPckg);
    iITCMsgArgs.Set(ESIPCRIpcArgResolverUid, &iResolverUid);
    ReceiveNext();
    }

// -----------------------------------------------------------------------------
// CSIPClientDiscoveryReceiver::~CSIPClientDiscoveryReceiver
// -----------------------------------------------------------------------------
//
CSIPClientDiscoveryReceiver::~CSIPClientDiscoveryReceiver () 
	{
	CActive::Cancel();
	}

    
// -----------------------------------------------------------------------------
// CSIPClientDiscoveryReceiver::HandleClientNotFoundL
// -----------------------------------------------------------------------------
//
void CSIPClientDiscoveryReceiver::HandleClientNotFoundL()
    {
    TIpcArgs tmpITCArgs;
    tmpITCArgs.Set(ESIPCRIpcArgReqId, &iIdsPckg);

    HBufC8* content = HBufC8::NewLC(iSizesPckg().iContentBufSize);
    TPtr8 contentPtr(content->Des());
    tmpITCArgs.Set(ESIPCRIpcArgSipMessageContent, &contentPtr);

    HBufC8* responseBuf = HBufC8::NewLC(iSizesPckg().iHeaderBufSize);
    TPtr8 responsePtr(responseBuf->Des());
    tmpITCArgs.Set(ESIPCRIpcArgSipResponse, &responsePtr);

    TInt err = iITC.Receive(tmpITCArgs);
    if (err != KErrNone) 
        {
        User::Leave(err);
        }
       
    CSIPResponse* response =  
        iSerializer.InternalizeSIPResponseL(*responseBuf);                              
    CleanupStack::PopAndDestroy(responseBuf);
    CleanupStack::PushL(response);
    
    RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
    
    const RPointerArray<CSIPHeaderBase>& responseHeaders = 
        response->AllHeadersL();
    
    for (TInt i=0; i < responseHeaders.Count(); i++)
        {
        CSIPHeaderBase* header = responseHeaders[i];
        User::LeaveIfError(headers.Append(header));
        response->RemoveHeader(header);
        }       
    
    iObserver.ClientNotFoundL(iIdsPckg(),
                              response->ResponseCode(),
                              response->ReasonPhrase(),
                              headers,
                              content);

    CleanupStack::Pop(1); // headers
    CleanupStack::PopAndDestroy(response);
    CleanupStack::Pop(content);
    }

// -----------------------------------------------------------------------------
// CSIPClientDiscoveryReceiver::HandleChannelResolvedL
// -----------------------------------------------------------------------------
//
void CSIPClientDiscoveryReceiver::HandleChannelResolvedL()
    {
    iObserver.ChannelResolvedL(iResolverUid(), iIdsPckg());
    }

// -----------------------------------------------------------------------------
// CSIPClientDiscoveryReceiver::ErrorOccured
// -----------------------------------------------------------------------------
//
void CSIPClientDiscoveryReceiver::ErrorOccured(TInt aError)
    {
    iObserver.ErrorOccurred(aError, iIdsPckg());
    }
	
// -----------------------------------------------------------------------------
// CSIPClientDiscoveryReceiver::RunL
// -----------------------------------------------------------------------------
//
void CSIPClientDiscoveryReceiver::RunL()
    {
    TInt err = iStatus.Int();
    
    if (err == KErrServerTerminated)
        {
        // This will leave to RunError-function. See below.
    	User::Leave(err);
	    }
		
    if (err == KErrNone)
        {
        switch (iClientResolved())
            {
            case ESIPCRChannelComplete:
                HandleChannelResolvedL();
                break;
            
            case ESIPCRClientNotFound:
                HandleClientNotFoundL();
                break;
            
            default:
                break;
            }
        }
    else
        {
        ErrorOccured(err);
        }

	ReceiveNext ();
    }
    
// -----------------------------------------------------------------------------
// CSIPClientDiscoveryReceiver::RunError
// -----------------------------------------------------------------------------
//
TInt CSIPClientDiscoveryReceiver::RunError(TInt aError) 
    {
	if (aError == KErrServerTerminated)
		{
		// Return the error to the active scheduler and stop receiving.
		// Receiving must be stopped.
		// It would lead to infinite RunL-RunError-loop.
		return aError;
		}
    // Ignore all the other errors and continue receiving
	ReceiveNext ();
	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CSIPClientDiscoveryReceiver::DoCancel
// -----------------------------------------------------------------------------
//
void CSIPClientDiscoveryReceiver::DoCancel()
    {
    iITC.CancelReceive();
    }

// -----------------------------------------------------------------------------
// CSIPClientDiscoveryReceiver::ReceiveNext
// -----------------------------------------------------------------------------
//
void CSIPClientDiscoveryReceiver::ReceiveNext()
    {
	if (!IsActive())
        {
        iITC.Receive( iITCMsgArgs,iStatus);
        SetActive();
        }
    }
    
//  End of File  
