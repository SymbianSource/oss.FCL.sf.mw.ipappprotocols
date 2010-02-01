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
* Name          : CTcpTransConnecting.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __CTCPTRANSCONNECTING_H__
#define __CTCPTRANSCONNECTING_H__

#include "CTcpTransState.h"

class CTcpTransportConnecting : public CTcpTransportState
	{
public:
	static CTcpTransportConnecting* NewL(const TSIPTransportParams& aParams);
	static CTcpTransportConnecting* NewLC(const TSIPTransportParams& aParams);
	~CTcpTransportConnecting();
	
	void SendToNetL(const TSIPTransportParams& aParams,
	                CSIPMessage& aMessage,
				    const TInetAddr& aAddr,
					TRequestStatus &aStatus,
					TUint aOrigTransport,
					CTransportTcp* aContext);
	
	TBool CancelSend(TRequestStatus& aStatus, CTransportTcp* aContext);
	void Wait(CTransportTcp* aContext);
	TBool HandleMessage(RStringF aProtocol,
						const TInetAddr& aRemoteAddr,
						CTransportTcp* aContext);
	void CancelAllRequests(CSIPConnection::TState aReason,
						   CTransportTcp* aContext);
	void ConnectionOpenL(CTransportTcp* aContext);
	void DisconnectedL(CTransportTcp* aContext);
	void SetNeighborState(CTcpTransportState* aState);
	
private:
	CTcpTransportConnecting(const TSIPTransportParams& aParams);
	
	CTcpTransportState* iConnected;
	};
#endif // end of __CTCPTRANSCONNECTING_H__

// End of File
