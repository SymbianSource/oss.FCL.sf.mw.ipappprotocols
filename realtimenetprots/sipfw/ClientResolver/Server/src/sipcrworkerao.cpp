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
// Name          : sipcrworkerao.cpp
// Part of       : SIP / SIP Client Resolver
// Version       : %version: 2.1.1 %
//



#include "sipcrworkerao.h"
#include "CSipCRServerSession.h"
 
// ----------------------------------------------------------------------------
// CSipCrWorkerAo::NewL
// ----------------------------------------------------------------------------
//
CSipCrWorkerAo* CSipCrWorkerAo::NewL(CSIPCRServerSession& aSession)
    {
    CSipCrWorkerAo* self = CSipCrWorkerAo::NewLC(aSession);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CSipCrWorkerAo::NewLC
// ----------------------------------------------------------------------------
//	        
CSipCrWorkerAo* CSipCrWorkerAo::NewLC(CSIPCRServerSession& aSession)
    {
    CSipCrWorkerAo* self = new(ELeave)CSipCrWorkerAo(aSession);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CSipCrWorkerAo::NewL
// ----------------------------------------------------------------------------
//
CSipCrWorkerAo* CSipCrWorkerAo::NewL(
    TUint32 aRequestId,
    CBufBase* aSerializedSipRequest,
    HBufC8* aSipRequestBody,
    const TSipClient& aSipClient,
    const TUid& aResolverUid,
    CSIPCRServerSession& aSession)
    {
    CSipCrWorkerAo* self = 
        CSipCrWorkerAo::NewLC(
            aRequestId,aSerializedSipRequest,aSipRequestBody,
            aSipClient,aResolverUid,aSession);
    CleanupStack::Pop(self);
    return self;    
    }

// ----------------------------------------------------------------------------
// CSipCrWorkerAo::NewLC
// ----------------------------------------------------------------------------
//	        
CSipCrWorkerAo* CSipCrWorkerAo::NewLC(
    TUint32 aRequestId,
    CBufBase* aSerializedSipRequest,
    HBufC8* aSipRequestBody,
    const TSipClient& aSipClient,
    const TUid& aResolverUid,
    CSIPCRServerSession& aSession)
    {
    CSipCrWorkerAo* self = 
        new(ELeave)CSipCrWorkerAo(
            aRequestId,aSipClient,aResolverUid,aSession);
    CleanupStack::PushL(self);
    self->ConstructL(aSerializedSipRequest,aSipRequestBody);
    return self;
    }

// ----------------------------------------------------------------------------
// CSipCrWorkerAo::CSipCrWorkerAo
// ----------------------------------------------------------------------------
//
CSipCrWorkerAo::CSipCrWorkerAo(CSIPCRServerSession& aSession)
 : CActive(EPriorityStandard),
   iSession(aSession),
   iSipClient(TUid::Null(),EFalse,EFalse)
    {
	CActiveScheduler::Add(this);
    }

// ----------------------------------------------------------------------------
// CSipCrWorkerAo::CSipCrWorkerAo
// ----------------------------------------------------------------------------
//
CSipCrWorkerAo::CSipCrWorkerAo(
    TUint32 aRequestId,
    const TSipClient& aSipClient,
    const TUid& aResolverUid,    
    CSIPCRServerSession& aSession)
 : CActive(EPriorityStandard),
   iSession(aSession),
   iRequestId(aRequestId),
   iSipClient(aSipClient),
   iResolverUid(aResolverUid)
    {
	CActiveScheduler::Add(this);
    }

// ----------------------------------------------------------------------------
// CSipCrWorkerAo::ConstructL
// ----------------------------------------------------------------------------
//
void CSipCrWorkerAo::ConstructL()
    {
    iSerializedSipRequest = CBufFlat::NewL(1);
    iSipRequestBody = KNullDesC8().AllocL();
    }

// ----------------------------------------------------------------------------
// CSipCrWorkerAo::ConstructL
// ----------------------------------------------------------------------------
//
void CSipCrWorkerAo::ConstructL(
    CBufBase* aSerializedSipRequest,
    HBufC8* aSipRequestBody)
    {
    __ASSERT_ALWAYS(aSerializedSipRequest != NULL, User::Leave(KErrArgument));
    iSerializedSipRequest = aSerializedSipRequest;
    iSipRequestBody = aSipRequestBody;  
    }

// ----------------------------------------------------------------------------
// CSipCrWorkerAo::~CSipCrWorkerAo
// ----------------------------------------------------------------------------
//
CSipCrWorkerAo::~CSipCrWorkerAo()
    {
	CActive::Cancel();
	iThread.Close();
    delete iSerializedSipRequest;
    delete iSipRequestBody;	
    }

// ----------------------------------------------------------------------------
// CSipCrWorkerAo::StartThreadL
// ----------------------------------------------------------------------------
//
void CSipCrWorkerAo::StartThreadL(RThread& aThread)
    {
    User::LeaveIfError(iThread.Duplicate(aThread));
    aThread.Logon(iStatus);
    aThread.Resume();
    SetActive();
    }

// ----------------------------------------------------------------------------
// CSipCrWorkerAo::Session
// ----------------------------------------------------------------------------
//
CSIPCRServerSession& CSipCrWorkerAo::Session()
    {
    return iSession;
    }

// ----------------------------------------------------------------------------
// CSipCrWorkerAo::SerializedSipRequest
// ----------------------------------------------------------------------------
//
CBufBase& CSipCrWorkerAo::SerializedSipRequest()
    {
    return *iSerializedSipRequest;
    }

// ----------------------------------------------------------------------------
// CSipCrWorkerAo::GetSipRequestBody
// ----------------------------------------------------------------------------
//
HBufC8* CSipCrWorkerAo::GetSipRequestBody()
    {
    HBufC8* tmp = iSipRequestBody;
    iSipRequestBody = NULL;
    return tmp;
    }

// ----------------------------------------------------------------------------
// CSipCrWorkerAo::SipClient
// ----------------------------------------------------------------------------
//
TSipClient& CSipCrWorkerAo::SipClient()
    {
    return iSipClient;
    }

// ----------------------------------------------------------------------------
// CSipCrWorkerAo::ResolverUid
// ----------------------------------------------------------------------------
//
const TUid& CSipCrWorkerAo::ResolverUid() const
    {
    return iResolverUid;
    }

// ----------------------------------------------------------------------------
// CSipCrWorkerAo::RequestId
// ----------------------------------------------------------------------------
//
TUint32 CSipCrWorkerAo::RequestId() const
    {
    return iRequestId;
    }

// ----------------------------------------------------------------------------
// CSipCrWorkerAo::CompletionCode
// ----------------------------------------------------------------------------
//       
TInt CSipCrWorkerAo::CompletionCode() const
    {
    TExitType exitType = iThread.ExitType();
    if (exitType == EExitPanic)
        {
        return KErrGeneral;
        }
    return iStatus.Int();
    }

// ----------------------------------------------------------------------------
// CSipCrWorkerAo::RunL
// ----------------------------------------------------------------------------
//
void CSipCrWorkerAo::RunL()
    {
	iSession.WorkerAoRequestCompleted(this); // Note: deletes this
    }

// ----------------------------------------------------------------------------
// CSipCrWorkerAo::DoCancel
// ----------------------------------------------------------------------------
//	
void CSipCrWorkerAo::DoCancel()
    {
	iThread.LogonCancel(iStatus);
    }
