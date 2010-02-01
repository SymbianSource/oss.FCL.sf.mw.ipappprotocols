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
* Name          : MServerTransportOwner.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __MSERVERTRANSPORTOWNER_H__
#define __MSERVERTRANSPORTOWNER_H__

#include <in_sock.h>

class CSIPMessage;
class CServerTransport;
class COutgoingData;
class TSIPTransportParams;
class MSIPTransportRemovalObserver;


class MServerTransportOwner
	{
public:
	virtual ~MServerTransportOwner() {}
	virtual void RemoveL(CServerTransport* aTransport,
						 const TInetAddr& aRemoteAddr,
						 MSIPTransportRemovalObserver* aRemovalObserver) = 0;
	virtual void ReceiveL(CSIPMessage* aMessage,
						  const TInetAddr& aRemoteAddr,
						  TInt aError,
						  TBool aCompressed) = 0;
	virtual void ReRouteL(COutgoingData* data) = 0;
	virtual MSigCompController* SigCompHandler() = 0;
	virtual TUint32 IapId() = 0;
	virtual TBool IsReservedTransportOwner() const = 0;
	};

#endif // end of __MSERVERTRANSPORTOWNER_H__

// End of File
