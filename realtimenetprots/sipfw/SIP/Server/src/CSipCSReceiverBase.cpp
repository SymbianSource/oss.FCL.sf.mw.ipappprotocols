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
// Name          : CSipCSReceiverBase.cpp
// Part of       : SIPServerCore
// Version       : SIP/4.0 
//



#include "CSipCSReceiverBase.h"
#include "CSipCSReceiveQueue.h"
#include "CSipCSServerITC.h"
#include "CSIPCSSerializer.h"
#include "sipmessage.h"
#include "siprequest.h"
#include "SipAssert.h"
#include "sipinternalerr.h"
#include "SipLogs.h"

// -----------------------------------------------------------------------------
// CSipCSReceiverBase::ConstructL
// -----------------------------------------------------------------------------
//
void CSipCSReceiverBase::ConstructL()
	{
    iReceiveQueue = new(ELeave)CSipCSReceiveQueue;
	}

// -----------------------------------------------------------------------------
// CSipCSReceiverBase::CSipCSReceiverBase
// -----------------------------------------------------------------------------
//
CSipCSReceiverBase::CSipCSReceiverBase(CSipCSServerITC& aITC,
                                       MSIPSecUser& aSIPSecUser)
: iITC (aITC),
  iSIPSecUser (aSIPSecUser),
  iClientReadyToReceive (EFalse)
	{
	}

// -----------------------------------------------------------------------------
// CSipCSReceiverBase::~CSipCSReceiverBase
// -----------------------------------------------------------------------------
//
CSipCSReceiverBase::~CSipCSReceiverBase()
	{
    delete iReceiveQueue;
	}

// -----------------------------------------------------------------------------
// CSipCSReceiverBase::WriteSipMessageToClientL
// -----------------------------------------------------------------------------
//
void CSipCSReceiverBase::WriteSipMessageToClientL(const RMessage2& aMessage)
	{
    __ASSERT_ALWAYS (!iReceiveQueue->IsEmpty(), User::Leave (KErrNotFound));
    __ASSERT_ALWAYS (iReceiveQueue->First().Headers() != 0, 
                     User::Leave (KErrNotFound));
    __ASSERT_ALWAYS (iReceiveQueue->First().Content() != 0,
                     User::Leave (KErrNotFound));

    iITC.WriteL (aMessage, *iReceiveQueue->First().Headers());
    iITC.WriteL (aMessage, *iReceiveQueue->First().Content(), 
                 ESipItcArgMessageContent);
                 
    iReceiveQueue->RemoveFirst();
	}

// -----------------------------------------------------------------------------
// CSipCSReceiverBase::ClientReadyToReceiveL
// -----------------------------------------------------------------------------
//
void CSipCSReceiverBase::ClientReadyToReceiveL(const RMessage2& aMessage)
	{
    __ASSERT_ALWAYS (!iClientReadyToReceive, User::Leave (KErrAlreadyExists));

    if (iReceiveQueue->IsEmpty())
		{
        iClientReadyToReceive = ETrue;
        iReceiveRMessage = aMessage;
		}
    else
		{
        const TSipCSReceiveItem& item = iReceiveQueue->First();
        TInt error = item.Error();
        iITC.WriteL (aMessage,item.Ids());
        if (error == KErrNone || error == KChallengeReceived)
            {
            iITC.WriteL (aMessage,item.BufSizes());
            }
        else
            {
            iReceiveQueue->RemoveFirst();
            }
        iITC.Complete (aMessage,error);
		}
	}

// -----------------------------------------------------------------------------
// CSipCSReceiverBase::CancelClientReceiveL
// -----------------------------------------------------------------------------
//
void CSipCSReceiverBase::CancelClientReceiveL()
	{
    __ASSERT_ALWAYS (iClientReadyToReceive, User::Leave (KErrNotFound));

    iClientReadyToReceive = EFalse;
    iITC.Complete (iReceiveRMessage, KErrCancel);
	}

// -----------------------------------------------------------------------------
// CSipCSReceiverBase::AddItemL
// -----------------------------------------------------------------------------
//	
void CSipCSReceiverBase::AddItemL(const TSipCSReceiveItem& aReceiveItem)
    {
    HandleNewReceiveItemL(aReceiveItem);
    }

// -----------------------------------------------------------------------------
// CSipCSReceiverBase::TransactionEnded
// From MTransactionOwner:
// -----------------------------------------------------------------------------
//
TInt CSipCSReceiverBase::TransactionEnded(TUint32 aIapId,
                                          TTransactionId aTransactionId,
                                          TInt aReason)
	{
	TSIPIds ids;
	ids.iIapId = aIapId;
	ids.iRequestId = aTransactionId;
	return ErrorReceived(ids,aReason);
	}

// -----------------------------------------------------------------------------
// CSipCSReceiverBase::NextCSeq
// From MTransactionOwner:
// -----------------------------------------------------------------------------
//
TInt CSipCSReceiverBase::NextCSeq(TUint& aCSeq)
    {
    aCSeq++;
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CSipCSReceiverBase::SIPSecUser
// From MTransactionOwner:
// -----------------------------------------------------------------------------
//    
const MSIPSecUser* CSipCSReceiverBase::SIPSecUser() const
    {
    return &(iSIPSecUser);
    }
    
// -----------------------------------------------------------------------------
// CSipCSReceiverBase::SipRequestReceivedL
// -----------------------------------------------------------------------------
//
void CSipCSReceiverBase::SipRequestReceivedL(TUint32 aIapId,
                                             TTransactionId aTransactionId,
                                             CSIPRequest* aRequest)
	{
	TSIPIds ids;
    ids.iIapId = aIapId;
	ids.iRequestId = aTransactionId;
    SipMessageReceivedL(ids,aRequest);
	}

// -----------------------------------------------------------------------------
// CSipCSReceiverBase::SipMessageReceivedL
// -----------------------------------------------------------------------------
//
void CSipCSReceiverBase::SipMessageReceivedL(const TSIPIds& aIds,
                                             CSIPMessage* aSipMessage)
    {
    __ASSERT_ALWAYS (aSipMessage, User::Leave(KErrArgument));
    __SIP_MESSAGE_LOG ("ServerCore: ReceiveL",*aSipMessage)

    CBufFlat* headers = iITC.Serializer().ExternalizeLC(*aSipMessage);
    HBufC8* content = aSipMessage->TakeContentOwnershipL();
    CleanupStack::PushL (content);
    TSipCSReceiveItem item(aIds,headers,content,aSipMessage->IsRequest());

    HandleNewReceiveItemL(item);

    CleanupStack::Pop(content);
    CleanupStack::Pop(headers);
    delete aSipMessage;
    }

// -----------------------------------------------------------------------------
// CSipCSReceiverBase::HandleNewReceiveItemL
// -----------------------------------------------------------------------------
//       
void CSipCSReceiverBase::HandleNewReceiveItemL(const TSipCSReceiveItem& aItem)
    {
    if (iClientReadyToReceive)
        {
        iITC.WriteL (iReceiveRMessage,aItem.Ids());
        iITC.WriteL (iReceiveRMessage,aItem.BufSizes());
        User::LeaveIfError(iReceiveQueue->AddLast(aItem));
        iITC.Complete (iReceiveRMessage,aItem.Error());
        iClientReadyToReceive = EFalse;
        }
    else
        {
        User::LeaveIfError(iReceiveQueue->AddLast(aItem));
        }     
    }

// -----------------------------------------------------------------------------
// CSipCSReceiverBase::ErrorReceived
// -----------------------------------------------------------------------------
//
TInt CSipCSReceiverBase::ErrorReceived(const TSIPIds& aIds, TInt aError)
    {
    TInt err = KErrNone;
    if (aError != KErrNone)
        {
        if (iClientReadyToReceive)
            {
            TRAP (err, iITC.WriteL (iReceiveRMessage,aIds));
            if (err == KErrNone)
                {
                iITC.Complete(iReceiveRMessage,aError);
                iClientReadyToReceive = EFalse;
                }
            }
        else
        	{
            TSipCSReceiveItem item(aIds,aError);
            err = iReceiveQueue->AddLast(item);       		
        	}
        if (aError == KErrTimedOut && err == KErrNone)
        	{
        	// In case a transaction times out before a client
        	// has received the SIP request related to the transaction
        	// these SIP requests are dropped.
        	iReceiveQueue->RemoveSIPRequestsWith(aIds.iRequestId);
        	}
        }
    return err;
    }
