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
// Name          : CTcpTransState.cpp
// Part of       : ConnectionMgr
// Version       : SIP/3.0 
//




#include "CTcpTransState.h"
#include "sipmessage.h"

// -----------------------------------------------------------------------------
// CTcpTransportState::SendToNetL
// -----------------------------------------------------------------------------
//
void CTcpTransportState::SendToNetL(const TSIPTransportParams& /*aParams*/,
                                    CSIPMessage& /*aMessage*/,
									const TInetAddr& /*aAddr*/,
									TRequestStatus& /*aStatus*/,
									TUint /*aOrigTransport*/,
									CTransportTcp* /*aContext*/)
	{
	__ASSERT_DEBUG(EFalse,
		User::Panic(_L("CTcpTransState baseclass is called"),
		KErrGeneral));
	}

// -----------------------------------------------------------------------------
// CTcpTransportState::CancelSend
// -----------------------------------------------------------------------------
//
TBool CTcpTransportState::CancelSend(TRequestStatus& /*aStatus*/,
									 CTransportTcp* /*aContext*/)
	{
	__ASSERT_DEBUG(EFalse,
				   User::Panic(_L("CTcpTransState baseclass is called"),
				   KErrGeneral));
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CTcpTransportState::Wait
// -----------------------------------------------------------------------------
//
void CTcpTransportState::Wait(CTransportTcp* /*aContext*/)
	{
	__ASSERT_DEBUG(EFalse,
		User::Panic(_L("CTcpTransState baseclass is called"),
		KErrGeneral));
	}

// -----------------------------------------------------------------------------
// CTcpTransportState::HandleMessage
// -----------------------------------------------------------------------------
//
TBool CTcpTransportState::HandleMessage(RStringF /*aProtocol*/,
									 	const TInetAddr& /*aRemoteAddr*/,
									 	CTransportTcp* /*aContext*/)
	{
	__ASSERT_DEBUG(EFalse,
		User::Panic(_L("CTcpTransState baseclass is called"),
		KErrGeneral));
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CTcpTransportState::CancelAllRequests
// -----------------------------------------------------------------------------
//
void CTcpTransportState::CancelAllRequests(CSIPConnection::TState /*aReason*/,
										   CTransportTcp* /*aContext*/)
	{
	__ASSERT_DEBUG(EFalse,
	User::Panic(_L("CTcpTransState baseclass is called"),
	KErrGeneral));
	}

// -----------------------------------------------------------------------------
// CTcpTransportState::ConnectionOpenL
// -----------------------------------------------------------------------------
//
void CTcpTransportState::ConnectionOpenL(CTransportTcp* /*aContext*/)
	{
	__ASSERT_DEBUG(EFalse,
		User::Panic(_L("CTcpTransState baseclass is called"),
		KErrGeneral));
	}

// -----------------------------------------------------------------------------
// CTcpTransportState::DisconnectedL
// -----------------------------------------------------------------------------
//
void CTcpTransportState::DisconnectedL(CTransportTcp* /*aContext*/)
	{
	__ASSERT_DEBUG(EFalse,
		User::Panic(_L("CTcpTransState baseclass is called"),
		KErrGeneral));
	}

// -----------------------------------------------------------------------------
// CTcpTransportState::SetNeighborState
// -----------------------------------------------------------------------------
//
void CTcpTransportState::SetNeighborState(CTcpTransportState* /*aState*/)
	{
	__ASSERT_DEBUG(EFalse,
		User::Panic(_L("CTcpTransState baseclass is called"),
		KErrGeneral));
	}

// -----------------------------------------------------------------------------
// CTcpTransportState::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CTcpTransportState::TimerExpiredL(CTransportTcp* /*aContext*/)
	{
	__ASSERT_DEBUG(EFalse,
		User::Panic(_L("CTcpTransState baseclass is called"),
		KErrGeneral));
	}

// -----------------------------------------------------------------------------
// CTcpTransportState::CTcpTransportState
// -----------------------------------------------------------------------------
//
CTcpTransportState::CTcpTransportState(const TSIPTransportParams& aParams) :
    iTransportParams(aParams)
	{
	}
