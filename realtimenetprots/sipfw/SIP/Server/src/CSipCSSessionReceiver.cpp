// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSipCSSessionReceiver.cpp
// Part of       : SIPServerCore
// Version       : SIP/6.0 
//



#include "CSipCSSessionReceiver.h"
#include "CSipCSSession.h"
#include "TSipCSReceiveItem.h"
#include "sipinternalerr.h"
#include "siperr.h"
#include "CSipCSServerITC.h"
#include "CSIPCSSerializer.h"
#include "sipresponse.h"

// -----------------------------------------------------------------------------
// CSipCSSessionReceiver::NewL
// -----------------------------------------------------------------------------
//
CSipCSSessionReceiver* CSipCSSessionReceiver::NewL(CSipCSServerITC& aITC, 
                                                   MSipCSSession& aSession)
	{
    CSipCSSessionReceiver* self = CSipCSSessionReceiver::NewLC(aITC,aSession);
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSipCSSessionReceiver::NewLC
// -----------------------------------------------------------------------------
//
CSipCSSessionReceiver* CSipCSSessionReceiver::NewLC(CSipCSServerITC& aITC,
                                                    MSipCSSession& aSession)
	{
    CSipCSSessionReceiver* self = 
        new(ELeave)CSipCSSessionReceiver(aITC,aSession);
    CleanupStack::PushL (self);
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CSipCSSessionReceiver::CSipCSSessionReceiver
// -----------------------------------------------------------------------------
//
CSipCSSessionReceiver::CSipCSSessionReceiver(CSipCSServerITC& aITC,
                                             MSipCSSession& aSession)
: CSipCSReceiverBase(aITC,*this),
  iHttpDigestObserverType(ESipNoHttpDigestObserver),
  iSession(aSession)
	{
	}

// -----------------------------------------------------------------------------
// CSipCSSessionReceiver::~CSipCSSessionReceiver
// -----------------------------------------------------------------------------
//
CSipCSSessionReceiver::~CSipCSSessionReceiver()
	{
	iPendingErrorItems.Close();
	}
	
// -----------------------------------------------------------------------------
// CSipCSSessionReceiver::SetHttpDigestObserver
// -----------------------------------------------------------------------------
//	
void CSipCSSessionReceiver::SetHttpDigestObserver(
    TSipHttpDigestObserverType aType)
    {
    iHttpDigestObserverType = aType;
    }

// -----------------------------------------------------------------------------
// CSipCSSessionReceiver::MovePendingErrorsL
// -----------------------------------------------------------------------------
//    
void CSipCSSessionReceiver::MovePendingErrorsL(TUint32 aIapId,
                                               CSipCSReceiverBase& aReceiver)
    {
    for (TInt i=iPendingErrorItems.Count()-1; i >= 0; i--)
        {
        const TSipCSReceiveItem& item = iPendingErrorItems[i];
        if (item.Ids().iIapId == aIapId)
            {
            aReceiver.AddItemL(item);
            iPendingErrorItems.Remove(i);
            }
        }
    }

// -----------------------------------------------------------------------------
// CSipCSSessionReceiver::ReceiveL
// From MTransactionOwner:
// -----------------------------------------------------------------------------
//
void CSipCSSessionReceiver::ReceiveL(TUint32 aIapId,
                                     TTransactionId aTransactionId,
                                     CSIPRequest* aRequest)
	{
	MTransactionOwner& owner = iSession.FindOwner(aIapId);
    if (&owner == this)
        {
        SipRequestReceivedL(aIapId,aTransactionId,aRequest);
        }
    else
        {
        // Subsession with matching IAP ID exists. Route request to it.
        owner.ReceiveL(aIapId,aTransactionId,aRequest);
        }
	}
	
// -----------------------------------------------------------------------------
// CSipCSSessionReceiver::TransactionEnded
// From MTransactionOwner:
// -----------------------------------------------------------------------------
//
TInt CSipCSSessionReceiver::TransactionEnded(TUint32 aIapId,
                                             TTransactionId aTransactionId,
                                             TInt aReason)
	{
	MTransactionOwner& owner = iSession.FindOwner(aIapId);
    if (&owner != this)
        {
        // Subsession with matching IAP ID exists. Route callback to it.
        return owner.TransactionEnded(aIapId,aTransactionId,aReason);
        }
    if (aReason != KErrTimedOut && aReason != KErrNone)
        {
        // For other errors than KErrTimedOut there is no callback
        // in SIP Client API. Put these errors waiting until the 
        // user has created CSIPConnection (C/S subsession).
	    TSIPIds ids;
	    ids.iIapId = aIapId;
	    ids.iRequestId = aTransactionId;        
        TSipCSReceiveItem item(ids,aReason);
        return iPendingErrorItems.Append(item);
        }
    return CSipCSReceiverBase::TransactionEnded(aIapId,aTransactionId,aReason);
	}	

// -----------------------------------------------------------------------------
// CSipCSSessionReceiver::ReceiveL
// From MTransactionOwner:
// -----------------------------------------------------------------------------
//
void CSipCSSessionReceiver::ReceiveL(TTransactionId /*aTransactionId*/,
                                     CSIPResponse* /*aResponse*/)
	{
    User::Leave(KErrNotSupported);
	}

// -----------------------------------------------------------------------------
// CSipCSSessionReceiver::PassOnlyRealmsToUser
// From MSIPSecUser:
// -----------------------------------------------------------------------------
//
TBool CSipCSSessionReceiver::PassOnlyRealmsToUser() const
    {
    return (iHttpDigestObserverType == ESipRealmHttpDigestObserver);
    }

// -----------------------------------------------------------------------------
// CSipCSSessionReceiver::RequestCredentialsL
// From MSIPSecUser:
// -----------------------------------------------------------------------------
//	
void CSipCSSessionReceiver::RequestCredentialsL(const TDesC8& aRealm)
    {
    __ASSERT_ALWAYS(iHttpDigestObserverType == ESipRealmHttpDigestObserver, 
                    User::Leave(KErrSIPForbidden));

    HBufC8* dummyContent = KNullDesC8().AllocLC();
    
    const TInt KRealmBufExpandSize = 50;
    CBufFlat* realmBuf = CBufFlat::NewL(KRealmBufExpandSize);
    CleanupStack::PushL(realmBuf);
    realmBuf->InsertL(0,aRealm);
    
    TSIPIds dummyIds;
    TSipCSReceiveItem item(dummyIds,realmBuf,dummyContent,
                           EFalse,KChallengeReceived);
                           
    HandleNewReceiveItemL(item);  
    
    CleanupStack::Pop(realmBuf);     
    CleanupStack::Pop(dummyContent);
    }
    
// -----------------------------------------------------------------------------
// CSipCSSessionReceiver::RequestCredentialsL
// From MSIPSecUser:
// -----------------------------------------------------------------------------
//
void CSipCSSessionReceiver::RequestCredentialsL(CSIPResponse& aResponse,
	                                            TTransactionId aTransactionId,
	                                            TRefreshId aRefreshId)
    {
    __ASSERT_ALWAYS(iHttpDigestObserverType == ESipResponseHttpDigestObserver, 
                    User::Leave(KErrSIPForbidden));

	TSIPIds ids;
	ids.iRequestId = aTransactionId;
	ids.iRefreshId = aRefreshId;
                    
    CBufFlat* headers = iITC.Serializer().ExternalizeLC(aResponse);
    HBufC8* content = aResponse.Content().AllocLC();
    TSipCSReceiveItem item(ids,headers,content,EFalse,KChallengeReceived);

    HandleNewReceiveItemL(item);

    CleanupStack::Pop(content);
    CleanupStack::Pop(headers);                                    
    }

// -----------------------------------------------------------------------------
// CSipCSSessionReceiver::TrustedUser
// From MSIPSecUser:
// -----------------------------------------------------------------------------
//	
const MSIPSecUser* CSipCSSessionReceiver::TrustedUser(
    TRegistrationId aRegistrationId)
    {
    return iSession.SIPSecUser(aRegistrationId);
    }

// -----------------------------------------------------------------------------
// CSipCSSessionReceiver::ByPassSIPSec
// From MSIPSecUser:
// -----------------------------------------------------------------------------
//
TBool CSipCSSessionReceiver::ByPassSIPSec() const
    {
    return iSession.ByPassSIPSec();
    }
