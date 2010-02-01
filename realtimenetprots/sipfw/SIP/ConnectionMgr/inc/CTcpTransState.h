/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name          : CTcpTransState.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __CTCPTRANSSTATE_H__
#define __CTCPTRANSSTATE_H__

#include <e32base.h>
#include <in_sock.h>
#include <stringpool.h>
#include "sipconnection.h"

class CSIPMessage;
class CTransportTcp;
class TSIPTransportParams;


class CTcpTransportState : public CBase
	{
public:
	virtual ~CTcpTransportState() {}

	virtual void SendToNetL(const TSIPTransportParams& aParams,
	                        CSIPMessage& aMessage,
							const TInetAddr& aAddr,
							TRequestStatus &aStatus,
							TUint aOrigTransport,
							CTransportTcp* aContext);

	virtual TBool CancelSend(TRequestStatus& aStatus,
							 CTransportTcp* aContext);
	virtual void Wait(CTransportTcp* aContext);
	virtual TBool HandleMessage(RStringF aProtocol,
								const TInetAddr& aRemoteAddr,
								CTransportTcp* aContext);
	virtual void CancelAllRequests(CSIPConnection::TState aReason,
								   CTransportTcp* aContext);
	virtual void ConnectionOpenL(CTransportTcp* aContext);
	virtual void DisconnectedL(CTransportTcp* aContext);
	virtual void TimerExpiredL(CTransportTcp* aContext);
	virtual void SetNeighborState(CTcpTransportState* aState);
	
protected:
	CTcpTransportState(const TSIPTransportParams& aParams);
	
	const TSIPTransportParams& iTransportParams;
	};

#endif // end of __CTCPTRANSSTATE_H__

// End of File
