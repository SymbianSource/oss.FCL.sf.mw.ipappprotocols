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
// Name          : CSipCRSessionReceiver.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



// INCLUDES
#include "CSIPCRSessionReceiver.h"
#include "CSIPCRResponseQueue.h"
#include "CSIPCRITCUtility.h"
#include "CSIPCRResponseItem.h"
#include "sipresponse.h"
#include "SIPCRSerializer.h"

// ----------------------------------------------------------------------------
// CSIPCRSessionReceiver::NewLC
// ----------------------------------------------------------------------------
//
CSIPCRSessionReceiver* CSIPCRSessionReceiver::NewL(CSIPCRITCUtility& aITC)
    {
    CSIPCRSessionReceiver* self = CSIPCRSessionReceiver::NewLC(aITC);
    CleanupStack::Pop (self);
    return self;
    }
    
// ----------------------------------------------------------------------------
// CSIPCRSessionReceiver::NewLC
// ----------------------------------------------------------------------------
//
CSIPCRSessionReceiver* CSIPCRSessionReceiver::NewLC(CSIPCRITCUtility& aITC)
    {
    CSIPCRSessionReceiver* self = new(ELeave)CSIPCRSessionReceiver (aITC);
    CleanupStack::PushL(self);
    self->ConstructL ();
    return self;
	}

// ----------------------------------------------------------------------------
// CSIPCRSessionReceiver::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPCRSessionReceiver::ConstructL()
    {
    User::LeaveIfError(iReceiverMutex.CreateLocal());
    iResponseQueue = new(ELeave)CSIPCRResponseQueue;
    }

// ----------------------------------------------------------------------------
// CSIPCRSessionReceiver::~CSIPCRSessionReceiver
// ----------------------------------------------------------------------------
//
CSIPCRSessionReceiver::~CSIPCRSessionReceiver()
	{
	delete iResponseQueue;
	iReceiverMutex.Close();
	}

// ----------------------------------------------------------------------------
// CSIPCRSessionReceiver::ConstructL
// ----------------------------------------------------------------------------
//
CSIPCRSessionReceiver::CSIPCRSessionReceiver(CSIPCRITCUtility& aITC)
: iITC(aITC),
  iClientReadyToReceive(EFalse)
    {
    }
    
// ----------------------------------------------------------------------------
// CSIPCRSessionReceiver::ClientReadyToReceiveL
// ----------------------------------------------------------------------------
//
void CSIPCRSessionReceiver::ClientReadyToReceiveL(const RMessage2& aMessage)
    {
    CSIPCRITCUtility::WaitForMutexLC(iReceiverMutex);
    
    __ASSERT_ALWAYS (!iClientReadyToReceive, User::Leave (KErrAlreadyExists));

    if (iResponseQueue->IsEmpty())
		{
        iClientReadyToReceive = ETrue;
        iReceiveRMessage = aMessage;
		}
    else
        {
        CSIPCRResponseItem& item = iResponseQueue->First();
        TInt error = item.Error();
        iITC.WriteRequestIdL(aMessage, item.RequestId());
        if (error == KErrNone)
            {
            if (item.SIPResponse() == 0)
                {
                iITC.WriteClientResolvedL(aMessage, ESIPCRChannelComplete);
                iITC.WriteChannelUidL(aMessage,item.Channel());
                iResponseQueue->RemoveFirst();
                }
            else
                {
                iITC.WriteClientResolvedL(aMessage, ESIPCRClientNotFound);
                iITC.WriteResponseSizeL(aMessage, item.BufSizes());
                }
            }
        else
            {
            iResponseQueue->RemoveFirst();
            }
        iITC.Complete (aMessage,error);
        }
        
    CleanupStack::PopAndDestroy(); // signals iReceiverMutex     
    }

// ----------------------------------------------------------------------------
// CSIPCRSessionReceiver::CancelClientReceiveL
// ----------------------------------------------------------------------------
//
void CSIPCRSessionReceiver::CancelClientReceiveL()
    {
    CSIPCRITCUtility::WaitForMutexLC(iReceiverMutex);
    
    __ASSERT_ALWAYS (iClientReadyToReceive, User::Leave (KErrNotFound));
    iClientReadyToReceive = EFalse;
    iITC.Complete (iReceiveRMessage, KErrCancel);
    
    CleanupStack::PopAndDestroy(); // signals iReceiverMutex 
    }

// ----------------------------------------------------------------------------
// CSIPCRSessionReceiver::WriteSipResponseToClientL
// ----------------------------------------------------------------------------
//
void CSIPCRSessionReceiver::WriteSipResponseToClientL(
    const RMessage2& aMessage)
    {
    CSIPCRITCUtility::WaitForMutexLC(iReceiverMutex);
    
    __ASSERT_ALWAYS (!iResponseQueue->IsEmpty(), User::Leave (KErrNotFound));
    CSIPCRResponseItem& item = iResponseQueue->First();
    __ASSERT_ALWAYS (item.RequestId() != 0,User::Leave (KErrGeneral));
    __ASSERT_ALWAYS (item.SIPResponse() != 0,User::Leave (KErrGeneral));
    __ASSERT_ALWAYS (item.SIPResponseContent() != 0,User::Leave (KErrGeneral));   
    iITC.WriteSIPResponseL(aMessage, *item.SIPResponse());
    iITC.WriteSIPResponseContentL(aMessage, *item.SIPResponseContent());
    iResponseQueue->RemoveFirst();
    
    CleanupStack::PopAndDestroy(); // signals iReceiverMutex 
    }

// ----------------------------------------------------------------------------
// CSIPCRSessionReceiver::AddErrorResponseL
// ----------------------------------------------------------------------------
//
void CSIPCRSessionReceiver::AddErrorResponseL(
    TUint32 aRequestId, 
    CSIPResponse* aResponse)
    {
    CSIPCRITCUtility::WaitForMutexLC(iReceiverMutex);
    
    __ASSERT_ALWAYS (aResponse, User::Leave(KErrArgument));  
    CBufFlat* response = SIPCRSerializer::ExternalizeL(*aResponse);
    CleanupStack::PushL(response);
    HBufC8* content = aResponse->TakeContentOwnershipL();
    CleanupStack::PushL(content);
    CSIPCRResponseItem* item = 
        new(ELeave)CSIPCRResponseItem(aRequestId,response,content);
    CleanupStack::Pop(content);
    CleanupStack::Pop(response);
    CleanupStack::PushL(item);    
    if (iClientReadyToReceive)
        {
        iITC.WriteRequestIdL(iReceiveRMessage, aRequestId);
        iITC.WriteClientResolvedL(iReceiveRMessage, ESIPCRClientNotFound);
        iITC.WriteResponseSizeL(iReceiveRMessage, item->BufSizes());
        User::LeaveIfError(iResponseQueue->AddLast(item));
        iITC.Complete (iReceiveRMessage,KErrNone);
        iClientReadyToReceive = EFalse;
        }
    else
        {
        User::LeaveIfError(iResponseQueue->AddLast(item));
        }
    CleanupStack::Pop(item);
    CleanupStack::PopAndDestroy(); // signals iReceiverMutex 
    
    }

// ----------------------------------------------------------------------------
// CSIPCRSessionReceiver::RequestCompletedL
// ----------------------------------------------------------------------------
//
void CSIPCRSessionReceiver::RequestCompletedL(
    TUint32 aRequestId, 
    TUid aChannel)
    {
    CSIPCRITCUtility::WaitForMutexLC(iReceiverMutex);
    
    __ASSERT_ALWAYS (aChannel.iUid != 0, User::Leave(KErrArgument));
    __ASSERT_ALWAYS (aRequestId, User::Leave(KErrArgument));
    if (iClientReadyToReceive)
        {
        iITC.WriteRequestIdL(iReceiveRMessage, aRequestId);
        iITC.WriteClientResolvedL(iReceiveRMessage, ESIPCRChannelComplete);
        iITC.WriteChannelUidL(iReceiveRMessage, aChannel);
        iITC.Complete (iReceiveRMessage,KErrNone);
        iClientReadyToReceive = EFalse;
        }
    else
        {
        CSIPCRResponseItem* item = 
            new(ELeave)CSIPCRResponseItem(aRequestId,aChannel);
        CleanupStack::PushL(item);
        User::LeaveIfError(iResponseQueue->AddLast(item));
        CleanupStack::Pop(item);
        }
        
    CleanupStack::PopAndDestroy(); // signals iReceiverMutex     
    }
    
// ----------------------------------------------------------------------------
// CSIPCRSessionReceiver::ErrorReceivedL
// ----------------------------------------------------------------------------
//
void CSIPCRSessionReceiver::ErrorReceivedL(
    TUint32 aRequestId,
    TInt aError)   
    {
    CSIPCRITCUtility::WaitForMutexLC(iReceiverMutex);
    
    if (aError != KErrNone)
        {
        if (iClientReadyToReceive)
            {
            iITC.WriteRequestIdL (iReceiveRMessage, aRequestId);
            iITC.Complete(iReceiveRMessage,aError);
            iClientReadyToReceive = EFalse;
            }
        else
            {
            CSIPCRResponseItem* item = 
                new(ELeave)CSIPCRResponseItem(aRequestId,aError);
            CleanupStack::PushL(item);
            User::LeaveIfError(iResponseQueue->AddLast(item));
            CleanupStack::Pop(item);
            }
        }
        
    CleanupStack::PopAndDestroy(); // signals iReceiverMutex     
    }

// ----------------------------------------------------------------------------
// CSIPCRSessionReceiver::RemoveByRequestId
// ----------------------------------------------------------------------------
//
void CSIPCRSessionReceiver::RemoveByRequestId(TUint32 aRequestId)
    {
    iReceiverMutex.Wait();
    iResponseQueue->RemoveByRequestId(aRequestId);
    iReceiverMutex.Signal();
    }
    
// ----------------------------------------------------------------------------
// CSIPCRSessionReceiver::RemoveAll
// ----------------------------------------------------------------------------
//    
void CSIPCRSessionReceiver::RemoveAll()
    {
    iReceiverMutex.Wait();
    iResponseQueue->RemoveAll();
    iReceiverMutex.Signal();
    }

// End of File
