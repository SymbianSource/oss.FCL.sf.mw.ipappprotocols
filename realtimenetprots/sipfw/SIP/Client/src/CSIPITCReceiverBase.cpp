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
// Name          : CSIPITCReceiverBase.cpp
// Part of       : SIPClient
// Version       : SIP/5.0 
//



#include "CSIPITCReceiverBase.h"
#include "MSIPITC.h"
#include "siprequestelements.h"
#include "sipresponseelements.h"
#include "sipmessageelements.h"
#include "siphttpdigestchallengeobserver.h"
#include "SipAssert.h"
#include "sipinternalerr.h"

// -----------------------------------------------------------------------------
// CSIPITCReceiverBase::CSIPITCReceiverBase
// -----------------------------------------------------------------------------
//
CSIPITCReceiverBase::CSIPITCReceiverBase (MSIPITC& aITC) 
 : CActive (EPriorityStandard),
   iITC (aITC)
	{
    CActiveScheduler::Add (this);
    iITCMsgArgs.Set (ESipItcArgIds, &iIdsPckg);
    iITCMsgArgs.Set (ESipItcArgBufSizes, &iSizesPckg);
	
	ReceiveNext ();
	}

// -----------------------------------------------------------------------------
// CSIPITCReceiverBase::~CSIPITCReceiverBase
// -----------------------------------------------------------------------------
//
CSIPITCReceiverBase::~CSIPITCReceiverBase () 
	{
    CActive::Cancel();
	}
	
// -----------------------------------------------------------------------------
// CSIPITCReceiverBase::SetHttpDigestObserver
// -----------------------------------------------------------------------------
//	
void CSIPITCReceiverBase::SetHttpDigestObserver(
    MSIPHttpDigestChallengeObserver& aObserver)
    {
    iHttpDigestObserver = &aObserver;
    iHttpDigestObserver2 = NULL;
    }

// -----------------------------------------------------------------------------
// CSIPITCReceiverBase::SetHttpDigestObserver
// -----------------------------------------------------------------------------
//	
void CSIPITCReceiverBase::SetHttpDigestObserver(
    MSIPHttpDigestChallengeObserver2& aObserver)
    {
    iHttpDigestObserver2 = &aObserver;
    iHttpDigestObserver = NULL;
    }

// -----------------------------------------------------------------------------
// CSIPITCReceiverBase::RemoveHttpDigestObserver
// -----------------------------------------------------------------------------
// 
void CSIPITCReceiverBase::RemoveHttpDigestObserver()
    {
    iHttpDigestObserver = NULL;
    iHttpDigestObserver2 = NULL;
    }

// -----------------------------------------------------------------------------
// CSIPITCReceiverBase::RunL
// -----------------------------------------------------------------------------
//
void CSIPITCReceiverBase::RunL ()
	{
	TInt err = iStatus.Int();
	if (err == KErrServerTerminated)
		{
		// This will leave to RunError-function. See below.
		User::Leave(err);
		}
    // Take copy of the ids. iIdsPckg could be overwritten by ReceiveNext.
    TSIPIds ids(iIdsPckg());		
    if (err == KErrNone)
        {
		IncomingMessageL(ids);
        }
    else if (err == KChallengeReceived)
        {
        ChallengeReceivedL(ids);
        }    
    else
        {
        // For errors receive has to be renewed before calling the observer 
        // which might delete the CSIP or CSIPConnection, 
        // which leads to deletion of this object.
        ReceiveNext();
        ErrorOccuredL(ids,err);
        }    
	}

// -----------------------------------------------------------------------------
// CSIPITCReceiverBase::RunError
// -----------------------------------------------------------------------------
//
TInt CSIPITCReceiverBase::RunError (TInt aError)
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
// CSIPITCReceiverBase::RunError
// -----------------------------------------------------------------------------
//
void CSIPITCReceiverBase::DoCancel()
    {
    iITC.CancelReceive();
    }

// -----------------------------------------------------------------------------
// CSIPITCReceiverBase::ReceiveNext
// -----------------------------------------------------------------------------
//
void CSIPITCReceiverBase::ReceiveNext ()
	{
	if (!IsActive())
        {
        iITC.Receive(iITCMsgArgs,iStatus);
	    SetActive();
        }
	}

// -----------------------------------------------------------------------------
// CSIPITCReceiverBase::ChallengeReceivedL
// -----------------------------------------------------------------------------
//	
void CSIPITCReceiverBase::ChallengeReceivedL (TSIPIds& aIds)
    {
    HBufC8* response = NULL;
    HBufC8* content = NULL;
    ReadHeadersAndContentLC(response,content);
        
    // On one hand receive has to be renewed before calling the observer 
    // which might delete the CSIP or CSIPConnection, 
    // which leads to deletion of this object.
    // On the other hand for incoming SIP requests and responses 
    // asynchronous receive has to be called only after fetching 
    // the previous message syncrounously.
    // This is why ReceiveNext is called here instead of RunL. 
    ReceiveNext();        
    
    if (iHttpDigestObserver)
        {
        // The IPC place holder for response is used here for realm
        iHttpDigestObserver->ChallengeReceived(*response);
        CleanupStack::PopAndDestroy(content);
        }
    else if (iHttpDigestObserver2)
        {
	    RDesReadStream readStream(*response);
        CleanupClosePushL(readStream);
	    TUint8 isRequest = readStream.ReadUint8L();
	    __ASSERT_ALWAYS(!isRequest, User::Leave(KErrArgument));
		IncomingResponseL(readStream,content,aIds);
        readStream.Pop();
        readStream.Close();
        CleanupStack::Pop(content);
        }
    else
        {
        // No observer, should not happen, prevent memory leak
        CleanupStack::PopAndDestroy(content);
        }
        
    CleanupStack::PopAndDestroy(response);
    }

// -----------------------------------------------------------------------------
// CSIPITCReceiverBase::IncomingMessageL
// -----------------------------------------------------------------------------
//
void CSIPITCReceiverBase::IncomingMessageL (TSIPIds& aIds)
    {
    HBufC8* headers = NULL;
    HBufC8* content = NULL;
    ReadHeadersAndContentLC(headers,content);
        
    // On one hand receive has to be renewed before calling the observer 
    // which might delete the CSIP or CSIPConnection, 
    // which leads to deletion of this object.
    // On the other hand for incoming SIP requests and responses 
    // asynchronous receive has to be called only after fetching 
    // the previous message syncrounously.
    // This is why ReceiveNext is called here instead of RunL. 
    ReceiveNext();        

	RDesReadStream readStream(*headers);
    CleanupClosePushL(readStream);
	TUint8 isRequest = readStream.ReadUint8L();
	if (isRequest)
		{
		IncomingRequestL(readStream,content,aIds);
		}
	else
		{
		IncomingResponseL(readStream,content,aIds);
		}
    readStream.Pop();
    readStream.Close();
    CleanupStack::Pop(content);
    CleanupStack::PopAndDestroy(headers);
	}

// -----------------------------------------------------------------------------
// CSIPITCReceiverBase::IncomingRequestL
// -----------------------------------------------------------------------------
//
void CSIPITCReceiverBase::IncomingRequestL (RReadStream& aReadStream,
                                            HBufC8* aContent,
                                            TSIPIds& aIds)
    {
    __ASSERT_ALWAYS (aContent != 0, User::Leave(KErrArgument));

	CSIPRequestElements* request = 
        CSIPRequestElements::InternalizeL(aReadStream);
    CleanupStack::PushL(request);
    CSIPMessageElements& messageElements = request->MessageElements();
    messageElements.SetContent(aContent);
    CleanupStack::PushL(TCleanupItem(DetachContent,&messageElements));
    IncomingRequestL (aIds,request);
    CleanupStack::Pop(1); // cleanup item
    CleanupStack::Pop(request);
	}

// -----------------------------------------------------------------------------
// CSIPITCReceiverBase::IncomingResponseL
// -----------------------------------------------------------------------------
//
void CSIPITCReceiverBase::IncomingResponseL (RReadStream& aReadStream,
                                             HBufC8* aContent,
										     TSIPIds& aIds)
	{
    __ASSERT_ALWAYS (aContent != 0, User::Leave(KErrArgument));

	CSIPResponseElements* response = 
        CSIPResponseElements::InternalizeL(aReadStream);
    CleanupStack::PushL(response);
    CSIPMessageElements& messageElements = response->MessageElements();
    messageElements.SetContent(aContent);
    CleanupStack::PushL(TCleanupItem(DetachContent,&messageElements));
    IncomingResponseL (aIds,response);
    CleanupStack::Pop(1); // cleanup item
    CleanupStack::Pop(response);
	}

// -----------------------------------------------------------------------------
// CSIPITCReceiverBase::DetachContent
// -----------------------------------------------------------------------------
//
void CSIPITCReceiverBase::DetachContent (TAny* aElements)
    {
    __SIP_ASSERT_RETURN(aElements, KErrGeneral);
    CSIPMessageElements* elements =
        reinterpret_cast<CSIPMessageElements*>(aElements);
    elements->DetachContent();
    }

// -----------------------------------------------------------------------------
// CSIPITCReceiverBase::ReadHeadersAndContentLC
// -----------------------------------------------------------------------------
//
void CSIPITCReceiverBase::ReadHeadersAndContentLC (HBufC8*& aHeaders,
                                                   HBufC8*& aContent)
    {
    TIpcArgs tmpITCArgs;

    TPckgBuf<TSIPIds> tmpIdsPckg;
    tmpITCArgs.Set (ESipItcArgIds, &tmpIdsPckg);

    HBufC8* headers = HBufC8::NewLC(iSizesPckg().iHeaderBufSize);
    TPtr8 headersPtr(headers->Des());
    tmpITCArgs.Set (ESipItcArgMessageHeaders, &headersPtr);

    HBufC8* content = HBufC8::NewLC(iSizesPckg().iContentBufSize);
    TPtr8 contentPtr(content->Des());
    tmpITCArgs.Set (ESipItcArgMessageContent, &contentPtr);

    TInt err = iITC.Receive(tmpITCArgs);
    if (err != KErrNone) 
        {
        User::Leave(err);
        }
    aHeaders = headers;
    aContent = content;
    }
