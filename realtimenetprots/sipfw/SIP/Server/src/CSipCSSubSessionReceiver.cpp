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
// Name          : CSipCSSubSessionReceiver.cpp
// Part of       : SIPServerCore
// Version       : SIP/5.0 
//



#include "CSipCSSubSessionReceiver.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipinternalerr.h"
#include "MSIPCSReceiverOwner.h"


// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::NewL
// -----------------------------------------------------------------------------
//
CSipCSSubSessionReceiver* CSipCSSubSessionReceiver::NewL(
    CSipCSServerITC& aITC,
    MSIPSecUser& aSIPSecUser,
    MSIPCSReceiverOwner& aOwner)
	{
    CSipCSSubSessionReceiver* self = 
        CSipCSSubSessionReceiver::NewLC(
            aITC,aSIPSecUser,aOwner);
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::NewLC
// -----------------------------------------------------------------------------
//
CSipCSSubSessionReceiver* CSipCSSubSessionReceiver::NewLC(
    CSipCSServerITC& aITC,
    MSIPSecUser& aSIPSecUser,
    MSIPCSReceiverOwner& aOwner)
	{
    CSipCSSubSessionReceiver* self = 
        new(ELeave)CSipCSSubSessionReceiver(
            aITC,aSIPSecUser,aOwner);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}


// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::CSipCSSubSessionReceiver
// -----------------------------------------------------------------------------
//
CSipCSSubSessionReceiver::CSipCSSubSessionReceiver(
    CSipCSServerITC& aITC,
    MSIPSecUser& aSIPSecUser,
    MSIPCSReceiverOwner& aOwner)
: CSipCSReceiverBase (aITC,aSIPSecUser),
  iOwner(aOwner)
	{
	}

// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::~CSipCSSubSessionReceiver
// -----------------------------------------------------------------------------
//
CSipCSSubSessionReceiver::~CSipCSSubSessionReceiver()
	{
	}

// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::ReceiveL
// From MTransactionOwner:
// -----------------------------------------------------------------------------
//
void CSipCSSubSessionReceiver::ReceiveL(TUint32 aIapId,
                                        TTransactionId aTransactionId,
                                        CSIPRequest* aRequest)
	{
	SipRequestReceivedL(aIapId,aTransactionId,aRequest);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::ReceiveL
// From MTransactionOwner
// -----------------------------------------------------------------------------
//
void CSipCSSubSessionReceiver::ReceiveL(TTransactionId aTransactionId,
                                        CSIPResponse* aResponse)
	{
	TSIPIds ids;
	ids.iRequestId = aTransactionId;
    SipMessageReceivedL (ids,aResponse);
	}	
	
// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::TransportParams
// From MRegistrationOwner
// -----------------------------------------------------------------------------
//    
const TSIPTransportParams& CSipCSSubSessionReceiver::TransportParams() const
    {
    return iOwner.TransportParams();
    }	

// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::TransportParams
// From MRefreshOwner and MDialogOwner
// -----------------------------------------------------------------------------
//    
TSIPTransportParams CSipCSSubSessionReceiver::TransportParams(
    const TRegistrationId& aRegistrationId)
    {
    return iOwner.TransportParams(aRegistrationId);
    }

// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::SIPSecUser
// From MRefreshOwner and MDialogOwner
// -----------------------------------------------------------------------------
//
const MSIPSecUser* CSipCSSubSessionReceiver::SIPSecUser(
    const TRegistrationId& aRegistrationId)
    {
    return iOwner.SIPSecUser(aRegistrationId);
    }

// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::RefreshReceivedL
// From MRefreshOwner
// -----------------------------------------------------------------------------
//
void CSipCSSubSessionReceiver::RefreshReceivedL(TTransactionId aTransactionId,
                                                TRefreshId aRefreshId,
                                                CSIPResponse* aResponse)
	{
	TSIPIds ids;
	ids.iRequestId = aTransactionId;
	ids.iRefreshId = aRefreshId;
    SipMessageReceivedL (ids,aResponse);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::RefreshError
// From MRefreshOwner
// -----------------------------------------------------------------------------
//
TInt CSipCSSubSessionReceiver::RefreshError(TRefreshId aRefreshId,
											TTransactionId aTransactionId,
											TInt aReason)
	{		
	TSIPIds ids;
	ids.iRequestId = aTransactionId;
	ids.iRefreshId = aRefreshId;
	return ErrorReceived(ids,aReason);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::NextRefreshCSeq
// From MRefreshOwner
// -----------------------------------------------------------------------------
//
TInt CSipCSSubSessionReceiver::NextRefreshCSeq(TUint& aCSeq)
	{
    return NextCSeq (aCSeq);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::RefreshDurationL
// -----------------------------------------------------------------------------
//
TUint CSipCSSubSessionReceiver::RefreshDurationL()
	{
	User::Leave(KErrNotFound);
	return 0;
	}

// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::RegistrationReceivedL
// From MRegistrationOwner
// -----------------------------------------------------------------------------
//
void 
CSipCSSubSessionReceiver::RegistrationReceivedL(TTransactionId aTransactionId,
                                                TRefreshId aRefreshId,
                                                TRegistrationId aRegistrationId,
                                                CSIPResponse* aResponse)
	{
	TSIPIds ids;
	ids.iRequestId = aTransactionId;
	ids.iRefreshId = aRefreshId;
	ids.iRegistrationId = aRegistrationId;
    SipMessageReceivedL (ids,aResponse);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::RegistrationError
// From MRegistrationOwner 
// -----------------------------------------------------------------------------
//
TInt 
CSipCSSubSessionReceiver::RegistrationError(TTransactionId aTransactionId,
                                            TRegistrationId aRegistrationId,
                                            TInt aReason)
	{	
	TSIPIds ids;
	ids.iRequestId = aTransactionId;
	ids.iRegistrationId = aRegistrationId;
	return ErrorReceived(ids,aReason);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::DialogsReceivedL
// From MDialogOwner
// -----------------------------------------------------------------------------
//
void CSipCSSubSessionReceiver::DialogsReceivedL(TTransactionId aTransactionId,
                                                TDialogId aDialogId,
                                                CSIPRequest* aRequest)
	{
	TSIPIds ids;
	ids.iRequestId = aTransactionId;
	ids.iDialogId = aDialogId;
    SipMessageReceivedL (ids,aRequest);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::DialogsReceivedL
// From MDialogOwner
// -----------------------------------------------------------------------------
//
void CSipCSSubSessionReceiver::DialogsReceivedL(TTransactionId aTransactionId,
                                                CSIPResponse* aResponse)
	{
	TSIPIds ids;
	ids.iRequestId = aTransactionId;
    SipMessageReceivedL (ids,aResponse);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::DialogsReceivedL
// From MDialogOwner
// -----------------------------------------------------------------------------
//
void CSipCSSubSessionReceiver::DialogsReceivedL(TTransactionId aTransactionId,
                                                TRefreshId aRefreshId,
                                                TDialogId aDialogId,
                                                CSIPResponse* aResponse)
	{
	TSIPIds ids;
	ids.iRequestId = aTransactionId;
	ids.iRefreshId = aRefreshId;
	ids.iDialogId = aDialogId;
    SipMessageReceivedL (ids,aResponse);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::DialogsError
// From MDialogOwner
// -----------------------------------------------------------------------------
//
TInt CSipCSSubSessionReceiver::DialogsError(TTransactionId aTransactionId,
                                            TInt aReason)
	{
	TSIPIds ids;
	ids.iRequestId = aTransactionId;
    return ErrorReceived(ids,aReason);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::DialogsError
// From MDialogOwner
// -----------------------------------------------------------------------------
//
TInt CSipCSSubSessionReceiver::DialogsError(TTransactionId aTransactionId,
                                            TRefreshId aRefreshId,
                                            TDialogId aDialogId,
                                            TInt aReason)
	{
	TSIPIds ids;
	ids.iRequestId = aTransactionId;
	ids.iRefreshId = aRefreshId;
	ids.iDialogId = aDialogId;
    return ErrorReceived(ids,aReason);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::InviteTransactionCompleted
// From MDialogOwner
// -----------------------------------------------------------------------------
//
TInt CSipCSSubSessionReceiver::InviteTransactionCompleted(TTransactionId aId)
    {
	TSIPIds ids;
	ids.iRequestId = aId;
    return ErrorReceived(ids,KInviteTransactionCompleted);
    }

// -----------------------------------------------------------------------------
// CSipCSSubSessionReceiver::ConnectionStateChangedL
// -----------------------------------------------------------------------------
//
void 
CSipCSSubSessionReceiver::ConnectionStateChangedL(CSIPConnection::TState aState)
	{
	TSIPIds ids;
	User::LeaveIfError (ErrorReceived(ids,aState));
	}
