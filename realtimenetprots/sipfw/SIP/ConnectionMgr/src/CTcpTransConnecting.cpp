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
// Name          : CTcpTransConnecting.cpp
// Part of       : ConnectionMgr
// Version       : SIP/6.0 
//




#include "CTcpTransConnecting.h"
#include "sipmessage.h"
#include "CTransportTcp.h"
#include "CReceiverTcp.h"
#include "CSender.h"
#include "COutgoingData.h"
#include "MTransportOwner.h"
#include "CommonConsts.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "siperr.h"

// -----------------------------------------------------------------------------
// CTcpTransportConnecting::NewL
// -----------------------------------------------------------------------------
//
CTcpTransportConnecting* CTcpTransportConnecting::NewL(
    const TSIPTransportParams& aParams)
	{
	CTcpTransportConnecting* self = NewLC(aParams);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnecting::NewLC
// -----------------------------------------------------------------------------
//
CTcpTransportConnecting* CTcpTransportConnecting::NewLC(
    const TSIPTransportParams& aParams)
	{
	CTcpTransportConnecting* self = 
	                new (ELeave) CTcpTransportConnecting(aParams);
	CleanupStack::PushL(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnecting::CancelSend
// -----------------------------------------------------------------------------
//
TBool CTcpTransportConnecting::CancelSend(TRequestStatus& aStatus,
										  CTransportTcp* aContext)
	{
	__ASSERT_DEBUG(aContext, User::Panic(_L("Nullpointer"), KErrGeneral));
	
	return aContext->GetSender()->FindAndCancel(aStatus);
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnecting::Wait
// -----------------------------------------------------------------------------
//
void CTcpTransportConnecting::Wait(CTransportTcp* /*aContext*/)
	{
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnecting::HandleMessage
// -----------------------------------------------------------------------------
//
TBool CTcpTransportConnecting::HandleMessage(RStringF aProtocol, 
										 	 const TInetAddr& aRemoteAddr, 
										 	 CTransportTcp* aContext)
	{
	__ASSERT_DEBUG(aContext, User::Panic(_L("Nullpointer"), KErrGeneral));
	
	TInetAddr connect;
	aContext->GetRemoteAddr(connect);
	
	return (aProtocol == SIPStrings::StringF(SipStrConsts::ETCP) &&
	    	connect.CmpAddr(aRemoteAddr));
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnecting::SendToNetL
// -----------------------------------------------------------------------------
//
void CTcpTransportConnecting::SendToNetL(const TSIPTransportParams& aParams,
                            CSIPMessage& aMessage,
							const TInetAddr& aAddr,
							TRequestStatus &aStatus,
							TUint aOrigTransport,
							CTransportTcp* aContext)
	{
	__ASSERT_DEBUG(aContext, User::Panic(_L("Nullpointer"), KErrGeneral));
	
	TBool connect = aContext->GetSender()->SendL(aParams,
                	                             aMessage,
                							     aAddr,
                			                     aOrigTransport,
                			                     aStatus,
                			                     ETrue,
                			                     EFalse);
	if ( connect )
	    {
	    aContext->ConnectSocket();
	    }
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnecting::CancelAllRequests
// -----------------------------------------------------------------------------
//
void CTcpTransportConnecting::CancelAllRequests(CSIPConnection::TState aReason, 
												CTransportTcp* aContext)
	{
	__ASSERT_DEBUG(aContext, User::Panic(_L("Nullpointer"), KErrGeneral));
	
	aContext->GetSender()->CancelAllRequests(aReason);
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnecting::ConnectionOpenL
// -----------------------------------------------------------------------------
//
void CTcpTransportConnecting::ConnectionOpenL(CTransportTcp* aContext)
	{
	__ASSERT_DEBUG(aContext, User::Panic(_L("Nullpointer"), KErrGeneral));
	
	aContext->SetCurrentState(iConnected);
	// Possibly update sender socket opts before sending
	aContext->GetSender()->SendNextL();
	aContext->StartTimerL();
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnecting::DisconnectedL
// -----------------------------------------------------------------------------
//
void CTcpTransportConnecting::DisconnectedL(CTransportTcp* aContext)
	{
	__ASSERT_DEBUG(aContext, User::Panic(_L("Nullpointer"), KErrGeneral));
	
	COutgoingData* data = 0;
	CSender* sender = aContext->GetSender();
	while ( (data = sender->StoredData()) != 0)
		{
		if (data->OrigTransport() == KProtocolInetUdp &&
			data->Message().IsRequest())
			{
			data->Sent();
			CleanupStack::PushL( data );
			aContext->TransportOwner()->ReRouteL(KProtocolInetUdp, 
			                                     data);
			CleanupStack::Pop( data );
			}
		else if (data->OrigTransport() == KProtocolInetTcp &&
				 !data->Message().IsRequest())
			{
			data->Sent();
			CleanupStack::PushL( data );
			aContext->TransportOwner()->ReRouteL(KProtocolInetTcp, 
			                                     data);
			CleanupStack::Pop( data );
			}
		else
			{
			TRequestStatus* stat = data->Status();
			User::RequestComplete(stat, KErrSIPTransportFailure);
			delete data;
			}
		}
	aContext->Remove();
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnecting::SetNeighborState
// -----------------------------------------------------------------------------
//
void CTcpTransportConnecting::SetNeighborState(CTcpTransportState* aState)
	{
	iConnected = aState;
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnecting::~CTcpTransportConnecting
// -----------------------------------------------------------------------------
//
CTcpTransportConnecting::~CTcpTransportConnecting()
	{
	}

// -----------------------------------------------------------------------------
// CTcpTransportConnecting::CTcpTransportConnecting
// -----------------------------------------------------------------------------
//
CTcpTransportConnecting::CTcpTransportConnecting(
    const TSIPTransportParams& aParams) : 
    CTcpTransportState(aParams)
	{
	}
