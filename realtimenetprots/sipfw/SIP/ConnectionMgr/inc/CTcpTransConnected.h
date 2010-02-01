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
* Name          : CTcpTransConnected.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __CTCPTRANSCONNECTED_H__
#define __CTCPTRANSCONNECTED_H__

#include "CTcpTransState.h"

class CTcpTransportConnected : public CTcpTransportState
	{
public:
	static CTcpTransportConnected* NewL(const TSIPTransportParams& aParams);
	static CTcpTransportConnected* NewLC(const TSIPTransportParams& aParams);
	~CTcpTransportConnected();
	
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
	void DisconnectedL(CTransportTcp* aContext);
	void TimerExpiredL(CTransportTcp* aContext);
	void SetNeighborState(CTcpTransportState* aState);
	
private:
	CTcpTransportConnected(const TSIPTransportParams& aParams);
	};
#endif // end of __CTCPTRANSCONNECTED_H__

// End of File
