// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CTcpTransConnected.cpp
// Part of       : ConnectionMgr
// Version       : SIP/4.0 
//




#include "CTcpTransConnected.h"
#include "sipmessage.h"
#include "CTransportTcp.h"
#include "CSender.h"
#include "CListener.h"
#include "CommonConsts.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "CSocketContainer.h"

// -----------------------------------------------------------------------------
// CTcpTransportConnected::NewL
// -----------------------------------------------------------------------------
//
CTcpTransportConnected* CTcpTransportConnected::NewL( 
    const TSIPTransportParams& aParams)
	{
	CTcpTransportConnected* self = NewLC(aParams);
	CleanupStack::Pop(self);
	return self;	
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnected::NewLC
// -----------------------------------------------------------------------------
//
CTcpTransportConnected* CTcpTransportConnected::NewLC(
    const TSIPTransportParams& aParams)
	{
	CTcpTransportConnected* self = new (ELeave) CTcpTransportConnected(aParams);
	CleanupStack::PushL(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnected::SendToNetL
// -----------------------------------------------------------------------------
//
void CTcpTransportConnected::SendToNetL(const TSIPTransportParams& aParams,
                            CSIPMessage& aMessage,
							const TInetAddr& aAddr,
							TRequestStatus &aStatus,
							TUint aOrigTransport,
							CTransportTcp* aContext)
	{
	__ASSERT_DEBUG(aContext, User::Panic(_L("Nullpointer"), KErrGeneral));
	
	aContext->GetSender()->SendL(aParams,
	                             aMessage,
                                 aAddr, 
                                 aOrigTransport, 
                                 aStatus, 
                                 EFalse,
                                 EFalse);
	aContext->StartTimerL();
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnected::CancelSend
// -----------------------------------------------------------------------------
//
TBool CTcpTransportConnected::CancelSend(TRequestStatus& aStatus,
										 CTransportTcp* aContext)
	{
	__ASSERT_DEBUG(aContext, User::Panic(_L("Nullpointer"), KErrGeneral));
	
	return aContext->GetSender()->FindAndCancel(aStatus);
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnected::Wait
// -----------------------------------------------------------------------------
//
void CTcpTransportConnected::Wait(CTransportTcp* /*aContext*/)
	{
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnected::HandleMessage
// -----------------------------------------------------------------------------
//
TBool CTcpTransportConnected::HandleMessage(RStringF aProtocol,
											const TInetAddr& aRemoteAddr,
											CTransportTcp* aContext)
	{
	__ASSERT_DEBUG(aContext, User::Panic(_L("Nullpointer"), KErrGeneral));
	
	TInetAddr connect;
	aContext->SocketContainer().RemoteName(connect);
	if (aProtocol == SIPStrings::StringF(SipStrConsts::ETCP) &&
		connect.Match(aRemoteAddr) &&
		(connect.Port() == aRemoteAddr.Port()))
		{
		return ETrue;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnected::CancelAllRequests
// -----------------------------------------------------------------------------
//
void CTcpTransportConnected::CancelAllRequests(CSIPConnection::TState aReason,
											   CTransportTcp* aContext)
	{
	__ASSERT_DEBUG(aContext, User::Panic(_L("Nullpointer"), KErrGeneral));
	
	aContext->GetSender()->CancelAllRequests(aReason);
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnected::DisconnectedL
// -----------------------------------------------------------------------------
//
void CTcpTransportConnected::DisconnectedL(CTransportTcp* aContext)
	{
	__ASSERT_DEBUG(aContext, User::Panic(_L("Nullpointer"), KErrGeneral));
	
	aContext->Remove();
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnected::SetNeighborState
// -----------------------------------------------------------------------------
//
void CTcpTransportConnected::SetNeighborState(CTcpTransportState* /*aState*/)
	{	
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnected::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CTcpTransportConnected::TimerExpiredL(CTransportTcp* aContext)
	{
	__ASSERT_DEBUG(aContext, User::Panic(_L("Nullpointer"), KErrGeneral));
	
	DisconnectedL(aContext);
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnected::~CTcpTransportConnected
// -----------------------------------------------------------------------------
//
CTcpTransportConnected::~CTcpTransportConnected()
	{
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnected::CTcpTransportConnected
// -----------------------------------------------------------------------------
//
CTcpTransportConnected::CTcpTransportConnected(
    const TSIPTransportParams& aParams) : 
    CTcpTransportState(aParams)
	{
	}
