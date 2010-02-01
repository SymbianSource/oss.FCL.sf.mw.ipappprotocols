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
* Name          : CIcmpV6Receiver.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef __CICMPV6RECEIVER_H__
#define __CICMPV6RECEIVER_H__

#include <e32base.h>
#include <in_sock.h>
#include "CommonConsts.h"

// Forward declarations
class MIcmpErrorObserver;


class CIcmpV6Receiver : public CActive
	{
public:
	static CIcmpV6Receiver* NewL(MIcmpErrorObserver& aObserver,
								 RSocketServ& aServer);
	void RunL();
	void DoCancel();
	~CIcmpV6Receiver();

private:
	CIcmpV6Receiver();
	void Receive();
	void ConstructL(MIcmpErrorObserver& aObserver, RSocketServ& aServer);
	void CheckError();

	//data
	RSocket iSocket;
	RSocketServ* iServer;
	TBuf8<KMaxUdpMessageSize> iData;
	TInetAddr iAddress;
	MIcmpErrorObserver* iObserver;

	RConnection* iConnection;
private: //For testing purposes
#ifdef CPPUNIT_TEST
	friend class CIcmprecv6Test;
	friend class CIcmprecvTest;
#endif
	};

#endif // end of __CIcmpV6Receiver_H__

// End of File
