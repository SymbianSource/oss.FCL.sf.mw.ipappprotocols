/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : MTransportOwner.h
* Part of       : ConnectionMgr
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/

#ifndef MTRANSPORTOWNER_H
#define MTRANSPORTOWNER_H

#include <in_sock.h>

class CTransportBase;
class COutgoingData;
class TSIPTransportParams;

class MTransportOwner
	{
public:
	
	virtual void AddListenerL(const TSIPTransportParams& aParams, 
	                          TUint aProtocol, 
	                          TUint aPort) = 0;
	
	virtual TInt RemoveTransport(CTransportBase* aTransport) = 0;
	
	virtual void ReRouteL(TUint aProtocol, COutgoingData* aData) = 0;
	
	virtual void TcpConnected(const TInetAddr& aAddr) = 0;
	
	virtual TBool IsLocalAddr(TInetAddr& aAddr) const = 0;
	
	virtual void GetLocalAddrL(TInetAddr& aAddr, 
	                           const TInetAddr* aNextHop) = 0;             
	
	virtual TBool GetLocalIPAddr(TUint aFamily, TInetAddr& aAddr) = 0;
    virtual void FillWithMatchingAddrFamily(TInetAddr& aAddr, 
		                                    const TInetAddr* aNextHop) = 0;	
	
	virtual TUint32 IapId() const = 0;
	};

#endif // MTRANSPORTOWNER_H

// End of File
