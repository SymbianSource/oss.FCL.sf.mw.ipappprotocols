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
// Name          : Transmitter.cpp
// Part of       : Transaction
// Version       : SIP/5.0 
//



#include <in_sock.h>

#include "siperr.h"
#include "SipAssert.h"
#include "CSipConnectionMgr.h"
#include "TSIPTransportParams.h"
#include "SipLogs.h"

#include "Transmitter.h"
#include "SendStatus.h"

// -----------------------------------------------------------------------------
// CTransmitter::NewL
// -----------------------------------------------------------------------------
//
CTransmitter* CTransmitter::NewL(MSipConnectionMgr& aConnectionMgr)
	{
	return new (ELeave) CTransmitter(aConnectionMgr);	
	}

// -----------------------------------------------------------------------------
// CTransmitter::CTransmitter
// -----------------------------------------------------------------------------
//
CTransmitter::CTransmitter(MSipConnectionMgr& aConnectionMgr) :
	CActive(EPriorityStandard),
	iConnectionMgr(aConnectionMgr)
	{
	CActiveScheduler::Add(this);
	}

// -----------------------------------------------------------------------------
// CTransmitter::~CTransmitter
// -----------------------------------------------------------------------------
//
CTransmitter::~CTransmitter()
	{
	Cancel();
	}

// -----------------------------------------------------------------------------
// CTransmitter::SendRequestL
// -----------------------------------------------------------------------------
//
void CTransmitter::SendRequestL(CSIPRequest& aReq,
							    const TInetAddr& aAddress,
							    TBool aForceUDP,
							    const TSIPTransportParams& aParams,
							    const CUri8* aOutboundProxy,
								MSendStatus* aObserver)
	{	
	__SIP_ASSERT_LEAVE(aObserver, KErrArgument);

	if (IsActive())
		{
		User::Leave(KErrAlreadyExists);
        }
    iConnectionMgr.SendL(aParams,
    					 aReq,
    					 aForceUDP,
    					 aOutboundProxy,
    					 aAddress,
    					 iStatus);
    iObserver = aObserver;
	SetActive();
	}

// -----------------------------------------------------------------------------
// CTransmitter::SendResponseL
// -----------------------------------------------------------------------------
//
void CTransmitter::SendResponseL(CSIPResponse& aResp,
								 const TSIPTransportParams& aParams,
								 MSendStatus* aObserver,
								 TTransactionId aTransactionId)
	{	
	__SIP_ASSERT_LEAVE(aObserver, KErrArgument);

	if (IsActive())
		{
		User::Leave(KErrAlreadyExists);
		}

    iConnectionMgr.SendL(aParams, aResp, aTransactionId, iStatus);
    iObserver = aObserver;
	SetActive();	
	}

// -----------------------------------------------------------------------------
// CTransmitter::TcpDisconnect
// -----------------------------------------------------------------------------
//
void CTransmitter::TcpDisconnect(TInetAddr& aRemoteAddr,
							     const TSIPTransportParams& aParams) const
    {
    iConnectionMgr.TcpDisconnect(aParams, aRemoteAddr);
    }

// -----------------------------------------------------------------------------
// CTransmitter::MapErrorCode
// ICMP Errors are seperated with a new Error Code KErrSIPICMPFailure
// -----------------------------------------------------------------------------
//
TInt CTransmitter::MapErrorCode(TInt aError) const
	{	
	switch (aError)
		{
    //Connection is (temporarily) not available. Treat as sending error.
	case CSIPConnection::ESuspended:
	case CSIPConnection::EInactive:
		return KErrSIPTransportFailure;
    //ICMP errors
	case CSipConnectionMgr::EHostUnreachable:
	case CSipConnectionMgr::ENetUnreachable:
	case CSipConnectionMgr::EPortUnreachable:
	case CSipConnectionMgr::EProtocolUnreachable:
		return KErrSIPICMPFailure;

	default:
		return aError;
		}
	}

// -----------------------------------------------------------------------------
// CTransmitter::DetachCallback
// -----------------------------------------------------------------------------
//
void CTransmitter::DetachCallback(MSendStatus* aObserver)
	{	
	if (iObserver == aObserver)
		{
		iObserver = NULL;
		Cancel();
		}
	}

// -----------------------------------------------------------------------------
// CTransmitter::CancelSendResponses
// CTransmitter doesn't cancel its possibly existing send request yet.
// -----------------------------------------------------------------------------
//
void CTransmitter::CancelSendResponses(TTransactionId aTransactionId,
									   TBool aAlso2xx) const
	{    
    iConnectionMgr.CancelResponseSend(aTransactionId, aAlso2xx);
	}

// -----------------------------------------------------------------------------
// CTransmitter::DoCancel
// If ConnectionMgr didn't cancel the request, complete it if iStatus is still
// KRequestPending. If ConnectionMgr used User::RequestComplete but RunL hasn't
// been called, CTransmitter is still active, but iStatus isn't KRequestPending.
// -----------------------------------------------------------------------------
//
void CTransmitter::DoCancel()
	{
	__SIP_LOG("CTransmitter::DoCancel")

	if (iConnectionMgr.CancelSend(iStatus) != KErrNone &&
        iStatus == KRequestPending)
        {
    	__SIP_LOG("CTransmitter::DoCancel cancel own request")

        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrCancel);
        }

    __SIP_LOG("CTransmitter::DoCancel ends")
	}

// -----------------------------------------------------------------------------    
// CTransmitter::RunL
// -----------------------------------------------------------------------------
//
void CTransmitter::RunL()
    {
	if (iObserver)
		{
		if (iStatus.Int() == KErrNone)
			{
			iObserver->SendCompleteL();
			}
		else
			{
			iObserver->SendFailedL(MapErrorCode(iStatus.Int()));
			}
		}
	}

// -----------------------------------------------------------------------------
// CTransmitter::RunError
// -----------------------------------------------------------------------------
//
TInt CTransmitter::RunError(TInt aError)
    {
    __SIP_INT_LOG1("CTransmitter::RunError, err=", aError)

	if (iObserver)
		{
		iObserver->LeaveFromTransmitter(aError);
		}

	if (aError == KErrNoMemory)
		{
		return aError;
		}

	return KErrNone;
	}
