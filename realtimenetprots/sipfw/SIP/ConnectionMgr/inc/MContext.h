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
* Name          : MContext.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __MCONTEXT_H__
#define __MCONTEXT_H__

#include <e32base.h>

class COutgoingData;
class TSIPTransportParams;
class TInetAddr;
class MSigCompController;
class CSIPMessage;
class CSocketContainer;

class MContext
	{
public:
    virtual CSocketContainer& SocketContainer() = 0;
    virtual CSocketContainer* SocketContainer(TUint aIPAddrFamily) = 0;
	virtual void ReceivedDataL(HBufC8* aData,
							   const TInetAddr& aRemoteAddr,
							   TBool aCompressed) = 0;
	virtual void ReceivedDataL(CSIPMessage* aMessage,
							   const TInetAddr& aRemoteAddr,
							   TInt aError,
							   TBool aCompressed) = 0;
	virtual MSigCompController* SigCompHandler() = 0;
	virtual void ResetSocketL() = 0;
	virtual void ContinueL() = 0;
	virtual TBool StopL() = 0;
	virtual TBool ConnectionOpenL() = 0;
	virtual TBool DisconnectedL() = 0;
	virtual TInt Remove() = 0;
	virtual void ReRouteL(TUint aProtocol, 
	                      COutgoingData* aData) = 0;
	virtual TUint32 IapId() = 0;
	virtual void Sending( TBool aIsSending ) = 0;
	};

#endif // end of __MCONTEXT_H__

// End of File
